#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <iconv.h>
#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#include "system.h"
#include "poptint.h"

#define STD_VFPRINTF(stream, format, args, retval) \
  va_start ((args), (format)); \
  (retval) = vfprintf ((stream), (format), (args)); \
  va_end ((args));

static char *
strdup_locale_from_utf8 (char *buffer)
{
  char *codeset = NULL;
  char *pout = NULL, *dest_str;
  iconv_t fd;
  int ib, ob, dest_size;
  int done, is_error;
  size_t err, used;

  if (!buffer)
    return NULL;

#ifdef HAVE_LANGINFO_H
  codeset = nl_langinfo (CODESET);
#endif

  if (codeset &&
      strcmp (codeset, "UTF-8") &&
      (fd = iconv_open (codeset, "UTF-8")) != (iconv_t)-1) {
    char *pin = buffer;
    char *shift_pin = NULL;

    iconv (fd, NULL, (size_t*)&ib, &pout, (size_t*)&ob);
    ib = strlen (buffer);
    dest_size = ob = ib;
    dest_str = pout = malloc ((dest_size + 1) * sizeof (char));
    done = is_error = 0;
    while (!done && !is_error) {
      err = iconv (fd, (const char**)&pin, (size_t*)&ib, &pout, (size_t*)&ob);

      if (err == (size_t)-1) {
        switch (errno) {
        case EINVAL:
          done = 1;
          break;
        case E2BIG:
          used = pout - dest_str;
          dest_size *= 2;
          dest_str = realloc (dest_str, (dest_size + 1) * sizeof (char));
          pout = dest_str + used;
          ob = dest_size - used;
          break;
        case EILSEQ:
          is_error = 1;
          break;
        default:
          is_error = 1;
          break;
        }
      } else {
        if (!shift_pin) {
          shift_pin = pin;
          pin = NULL;
          ib = 0;
        } else {
          done = 1;
        }
      }
    }
    iconv_close (fd);
    *pout = '\0';
    dest_str = strdup (dest_str);
  } else {
    dest_str = strdup (buffer);
  }

  return dest_str;
}

static char *
strdup_vprintf (const char *format, va_list ap)
{
  char *buffer = NULL;
  char c;

  buffer = calloc (sizeof (char), vsnprintf (&c, 1, format, ap) + 1);
  vsprintf (buffer, format, ap);

  return buffer;
}

char *
POPT_prev_char (const char *str)
{
  char *p = (char*)str;

  while (1) {
    p--;
    if ((*p & 0xc0) != 0x80)
      return (char *)p;
  }
}

int
POPT_fprintf (FILE* stream, const char *format, ...)
{
  int retval = 0;
  va_list args;
  char *buffer = NULL;
  char *locale_str = NULL;

  va_start (args, format);
  buffer = strdup_vprintf (format, args);
  va_end (args);

  locale_str = strdup_locale_from_utf8 (buffer);
  if (locale_str) {
    retval = fprintf (stream, "%s", locale_str);
    free (locale_str);
  } else {
    fprintf (stderr, POPT_WARNING "%s\n", "Invalid UTF-8");
    retval = fprintf (stream, "%s", buffer);
  }
  free (buffer);

  return retval;

}

#undef STD_VFPRINTF

