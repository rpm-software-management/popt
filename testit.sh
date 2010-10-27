#!/bin/sh
###############################################
# malloc voo-doo
###############################################
# see http://lists.gnupg.org/pipermail/gcrypt-devel/2010-June/001605.html
export MALLOC_CHECK_=3
# http://udrepper.livejournal.com/11429.html
export MALLOC_PERTURB_=`expr \( $RANDOM % 255 \) + 1 `
#
if [ -z "${MALLOC_PERTURB_}" ]  # XXX: some shell don't have RANDOM ?
then
	r=`ps -ef | cksum | cut -f1 -d" " 2>/dev/null`
	[ -z "${r}" ] && r=1234567890
        export MALLOC_PERTURB_=`expr \( $r % 255 \) + 1 `
fi

run() {
    prog=$1; shift
    name=$1; shift
    answer=$1; shift

    echo Running test $name.

    result=`HOME=$builddir $builddir/$prog $* 2>&1`
    if [ "$answer" != "$result" ]; then
	echo "Test \"$prog $*\" failed with: \"$result\" != \"$answer\" "
	exit 2
    fi
}

run_diff() {
    prog=$1; shift
    name=$1; shift
    in_file=$1; shift
    answer_file=$1; shift

    out=$builddir/tmp.out
    diff_file=$builddir/tmp.diff

    echo Running test $name.

    $builddir/$prog $in_file > $out
    ret=$?
    diff $out $answer_file > $diff_file
    diff_ret=$?

    if [ "$diff_ret" != "0" ]; then
       echo "Test \"$name\" failed output is in $out, diff is:"
       cat $diff_file
       exit 2
    fi
    rm $out $diff_file
}

builddir=`pwd`
srcdir=$builddir
cd ${srcdir}
test1=${builddir}/test1
echo "Running tests in `pwd`"

#make -q testcases

run test1 "test1 - 1" "arg1: 1 arg2: (none)" --arg1
run test1 "test1 - 2" "arg1: 0 arg2: foo" --arg2 foo
run test1 "test1 - 3" "arg1: 1 arg2: something" --arg1 --arg2 something
run test1 "test1 - 4" "arg1: 0 arg2: another" --simple another
run test1 "test1 - 5" "arg1: 1 arg2: alias" --two
run test1 "test1 - 6" "arg1: 1 arg2: (none) rest: --arg2" --arg1 -- --arg2 
run test1 "test1 - 7" "arg1: 0 arg2: abcd rest: --arg1" --simple abcd -- --arg1 
run test1 "test1 - 8" "arg1: 1 arg2: (none) rest: --arg2" --arg1 --takerest --arg2 
run test1 "test1 - 9" "arg1: 0 arg2: foo" -2 foo
run test1 "test1 - 10" "arg1: 0 arg2: (none) arg3: 50" -3 50
run test1 "test1 - 11" "arg1: 0 arg2: bar" -T bar
run test1 "test1 - 12" "arg1: 1 arg2: (none)" -O 
run test1 "test1 - 13" "arg1: 1 arg2: foo" -OT foo
run test1 "test1 - 14" "arg1: 0 arg2: (none) inc: 1" --inc
run test1 "test1 - 15" "arg1: 0 arg2: foo inc: 1" -I --arg2 foo
POSIX_ME_HARDER=1 ; export POSIX_ME_HARDER
run test1 "test1 - 16" "arg1: 1 arg2: (none) rest: foo --arg2 something" --arg1 foo --arg2 something
unset POSIX_ME_HARDER
POSIXLY_CORRECT=1 ; export POSIXLY_CORRECT
run test1 "test1 - 17" "arg1: 1 arg2: (none) rest: foo --arg2 something" --arg1 foo --arg2 something
unset POSIXLY_CORRECT
run test1 "test1 - 18" "callback: c sampledata bar arg1: 1 arg2: (none)" --arg1 --cb bar
run test1 "test1 - 19" "" --echo-args
run test1 "test1 - 20" "--arg1" --echo-args --arg1
run test1 "test1 - 21" "--arg2 something" -T something -e
run test1 "test1 - 22" "--arg2 something more args" -T something -a more args
run test1 "test1 - 23" "--echo-args -a" --echo-args -e -a
run test1 "test1 - 24" "arg1: 0 arg2: (none) short: 1" -onedash
run test1 "test1 - 25" "arg1: 0 arg2: (none) short: 1" --onedash
run test1 "test1 - 26" "callback: c arg for cb2 foo arg1: 0 arg2: (none)" --cb2 foo
run test1 "test1 - 27" "arg1: 0 arg2: (none) rest: -" -
run test1 "test1 - 28" "arg1: 0 arg2: foo rest: -" - -2 foo
run test1 "test1 - 29" "arg1: 0 arg2: bbbb" --arg2=aaaa -2 bbbb
run test1 "test1 - 30" "arg1: 0 arg2: 'foo bingo' rest: boggle" --grab bingo boggle
run test1 "test1 - 31" "arg1: 0 arg2: 'foo bar' rest: boggle" --grabbar boggle

run test1 "test1 - 32" "arg1: 0 arg2: (none) aInt: 123456789" -i 123456789
run test1 "test1 - 33" "arg1: 0 arg2: (none) aInt: 123456789" --int 123456789
run test1 "test1 - 34" "arg1: 0 arg2: (none) aShort: 12345" -s 12345
run test1 "test1 - 35" "arg1: 0 arg2: (none) aShort: 12345" --short 12345
run test1 "test1 - 36" "arg1: 0 arg2: (none) aLong: 1123456789" -l 1123456789
run test1 "test1 - 37" "arg1: 0 arg2: (none) aLong: 1123456789" --long 1123456789
run test1 "test1 - 38" "arg1: 0 arg2: (none) aLongLong: 1111123456789" -L 1111123456789
run test1 "test1 - 39" "arg1: 0 arg2: (none) aLongLong: 1111123456789" --longlong 1111123456789

run test1 "test1 - 40" "arg1: 0 arg2: (none) aFloat: 10.1" -f 10.1
run test1 "test1 - 41" "arg1: 0 arg2: (none) aFloat: 10.1" --float 10.1
run test1 "test1 - 42" "arg1: 0 arg2: (none) aDouble: 10.1" -d 10.1
run test1 "test1 - 43" "arg1: 0 arg2: (none) aDouble: 10.1" --double 10.1

run test1 "test1 - 44" "arg1: 0 arg2: (none) oStr: (none)" --optional
run test1 "test1 - 45" "arg1: 0 arg2: (none) oStr: yadda" --optional=yadda
run test1 "test1 - 46" "arg1: 0 arg2: (none) oStr: yadda" --optional yadda
run test1 "test1 - 47" "arg1: 0 arg2: (none) oStr: ping rest: pong" --optional=ping pong
run test1 "test1 - 48" "arg1: 0 arg2: (none) oStr: ping rest: pong" --optional ping pong
run test1 "test1 - 49" "arg1: 0 arg2: (none) aArgv: A B rest: C" --argv A --argv B C

run test1 "test1 - 50" "arg1: 0 arg2: foo=bar" -2foo=bar
run test1 "test1 - 51" "arg1: 0 arg2: foo=bar" -2=foo=bar

run test1 "test1 - 52" "arg1: 0 arg2: (none) aFlag: 0xfeed" --bitxor
run test1 "test1 - 53" "arg1: 0 arg2: (none) aFlag: 0xffff" --bitset
run test1 "test1 - 54" "arg1: 0 arg2: (none) aFlag: 0x28c" --bitclr
run test1 "test1 - 55" "arg1: 0 arg2: (none) aFlag: 0x8888" --nobitset
run test1 "test1 - 56" "arg1: 0 arg2: (none) aFlag: 0xface" --nobitclr

run test1 "test1 - 57" "arg1: 0 arg2: (none) aBits: foo,baz" --bits foo,bar,baz,!bar

run test1 "test1 - 58" "test1: bad argument --arg1=illegal: option does not take an argument" --arg1=illegal
run test1 "test1 - 59" "test1: bad argument --=illegal: option does not take an argument" --=illegal

run test1 "test1 - 60" "\
Usage: test1 [-I?] [-c|--cb2=STRING] [--arg1] [-2|--arg2=ARG]
        [-3|--arg3=ANARG] [-onedash] [--optional=STRING] [--val]
        [-i|--int=INT] [-s|--short=SHORT] [-l|--long=LONG]
        [-L|--longlong=LONGLONG] [-f|--float=FLOAT] [-d|--double=DOUBLE]
        [--randint=INT] [--randshort=SHORT] [--randlong=LONG]
        [--randlonglong=LONGLONG] [--argv=STRING] [--bitset] [--bitclr]
        [--bitxor] [--nstr=STRING] [--lstr=STRING] [-I|--inc]
        [-c|--cb=STRING] [--longopt] [-?|--help] [--usage] [--simple=ARG]" --usage
run test1 "test1 - 61" "\
Usage: test1 [OPTION...]
      --arg1                      First argument with a really long
                                  description. After all, we have to test
                                  argument help wrapping somehow, right?
  -2, --arg2=ARG                  Another argument (default: \"(none)\")
  -3, --arg3=ANARG                A third argument
      -onedash                    POPT_ARGFLAG_ONEDASH: Option takes a single -
      --optional[=STRING]         POPT_ARGFLAG_OPTIONAL: Takes an optional
                                  string argument
      --val                       POPT_ARG_VAL: 125992 141421
  -i, --int=INT                   POPT_ARG_INT: 271828 (default: 271828)
  -s, --short=SHORT               POPT_ARG_SHORT: 4523 (default: 4523)
  -l, --long=LONG                 POPT_ARG_LONG: 738905609 (default: 738905609)
  -L, --longlong=LONGLONG         POPT_ARG_LONGLONG: 738905609 (default:
                                  738905609)
  -f, --float=FLOAT               POPT_ARG_FLOAT: 3.14159 (default: 3.14159)
  -d, --double=DOUBLE             POPT_ARG_DOUBLE: 9.8696 (default: 9.8696)
      --randint=INT               POPT_ARGFLAG_RANDOM: experimental
      --randshort=SHORT           POPT_ARGFLAG_RANDOM: experimental
      --randlong=LONG             POPT_ARGFLAG_RANDOM: experimental
      --randlonglong=LONGLONG     POPT_ARGFLAG_RANDOM: experimental
      --argv STRING               POPT_ARG_ARGV: append string to argv array
                                  (can be used multiple times)
      --[no]bitset                POPT_BIT_SET: |= 0x7777
      --[no]bitclr                POPT_BIT_CLR: &= ~0xf842
      --bitxor                    POPT_ARGFLAG_XOR: ^= (0x8ace^0xfeed)
      --nstr=STRING               POPT_ARG_STRING: (null) (default: null)
      --lstr=STRING               POPT_ARG_STRING: \"123456789...\" (default:
                                  \"This tests default strings and exceeds the
                                  ... limit.
                                  123456789+123456789+123456789+123456789+123456789+ 123456789+123456789+123456789+123456789+123456789+ 1234567...\")

arg for cb2
  -c, --cb2=STRING                Test argument callbacks
  -I, --inc                       An included argument

Callback arguments
  -c, --cb=STRING                 Test argument callbacks
      --longopt                   Unused option for help testing

Options implemented via popt alias/exec:
      --simple=ARG                simple description

Help options:
  -?, --help                      Show this help message
      --usage                     Display brief usage message" --help

run test2 "test2 - 1" "\
dbusername popt-DBUserName	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname popt-first	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --keyfile=popt-key-file --cnpath=popt-content-path --dbusername=popt-DBUserName --first=popt-first  -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -C=popt-country -d=popt-dayphone
run test2 "test2 - 2" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --offerfile=popt-offer-file --storeid=popt-store-id  -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax
run test2 "test2 - 3" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --offerfile=popt-offer-file --storeid=popt-store-id --cnpath=popt-content-path --dbusername=popt-DBUserName  -u=popt-user -p=popt-password -1=popt-addr1 -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -2=popt-addr2 -s=popt-state -P=postal -C=popt-country -d=popt-dayphone -F=popt-fax -p=popt-password -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -e=popt-email -F=popt-fax -c=popt-city -s=popt-state -C=popt-country -e=popt-email
run test2 "test2 - 4" "\
dbusername popt-DBUserName	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname popt-first	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --keyfile=popt-key-file --offerfile=popt-offer-file --rcfile=popt-rcfile --cnhost=popt-content-host --cnpath=popt-content-path --dbusername=popt-DBUserName --first=popt-first  -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -c=popt-city -s=popt-state -P=postal -C=popt-country -e=popt-email -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -u=popt-user -1=popt-addr1 -s=popt-state -P=postal -z=popt-postal -C=popt-country -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -c=popt-city -z=popt-postal -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 5" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --offerfile=popt-offer-file --storeid=popt-store-id --txhost=popt-transact-host --first=popt-first --last=popt-last  -u=popt-user -p=popt-password -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax -u=popt-user -c=popt-city -s=popt-state -P=postal -e=popt-email -F=popt-fax -u=popt-user -p=popt-password -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -F=popt-fax -u=popt-user -1=popt-addr1 -s=popt-state -P=postal -C=popt-country -e=popt-email -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -C=popt-country -d=popt-dayphone -F=popt-fax
run test2 "test2 - 6" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --offerfile=popt-offer-file --cnhost=popt-content-host --cnpath=popt-content-path  -p=popt-password -1=popt-addr1 -c=popt-city -s=popt-state -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 7" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax (null)" --keyfile=popt-key-file --cnhost=popt-content-host  -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -e=popt-email -u=popt-user -p=popt-password -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone
run test2 "test2 - 8" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password (null)	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 (null)	city popt-city	state popt-state	postal postal
country popt-country	email (null)	dayphone popt-dayphone	fax (null)" --txhost=popt-transact-host  -u=popt-user -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -C=popt-country -d=popt-dayphone
run test2 "test2 - 9" "\
dbusername popt-DBUserName	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password (null)	firstname popt-first	lastname popt-last
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --cnpath=popt-content-path --dbusername=popt-DBUserName --first=popt-first --last=popt-last  -u=popt-user -1=popt-addr1 -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -z=popt-postal -e=popt-email -1=popt-addr1 -2=popt-addr2 -c=popt-city -z=popt-postal -C=popt-country -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -z=popt-postal -C=popt-country -F=popt-fax
run test2 "test2 - 10" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal postal
country popt-country	email (null)	dayphone popt-dayphone	fax popt-fax" --offerfile=popt-offer-file  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -C=popt-country -d=popt-dayphone -F=popt-fax
run test2 "test2 - 11" "\
dbusername (null)	dbpassword popt-DBpassword
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 (null)	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --keyfile=popt-key-file --txhost=popt-transact-host --cnhost=popt-content-host --dbpassword=popt-DBpassword  -u=popt-user -p=popt-password -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax -p=popt-password -1=popt-addr1 -c=popt-city -1=popt-addr1 -c=popt-city -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax -u=popt-user -p=popt-password -c=popt-city -s=popt-state -z=popt-postal -C=popt-country
run test2 "test2 - 12" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --storeid=popt-store-id --txsslport=popt-transact-ssl-port  -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -d=popt-dayphone -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 13" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state (null)	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax (null)" --txhost=popt-transact-host --cnhost=popt-content-host  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -d=popt-dayphone -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone
run test2 "test2 - 14" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone (null)	fax popt-fax" --keyfile=popt-key-file --offerfile=popt-offer-file --rcfile=popt-rcfile --txhost=popt-transact-host  -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -z=popt-postal -e=popt-email -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -C=popt-country -u=popt-user -p=popt-password -s=popt-state
run test2 "test2 - 15" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --storeid=popt-store-id --dbpassword=popt-DBpassword --last=popt-last  -p=popt-password -c=popt-city -s=popt-state -F=popt-fax -p=popt-password -1=popt-addr1 -P=postal -C=popt-country -e=popt-email -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -P=postal -z=popt-postal -d=popt-dayphone -u=popt-user -p=popt-password -c=popt-city -s=popt-state -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 16" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname popt-first	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --cnhost=popt-content-host --first=popt-first  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 17" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --storeid=popt-store-id --txhost=popt-transact-host --dbusername=popt-DBUserName  -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -2=popt-addr2 -s=popt-state -P=postal -z=popt-postal -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -z=popt-postal -C=popt-country
run test2 "test2 - 18" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --txsslport=popt-transact-ssl-port --cnpath=popt-content-path --dbpassword=popt-DBpassword --last=popt-last  -u=popt-user -p=popt-password -1=popt-addr1 -c=popt-city -P=postal -z=popt-postal -u=popt-user -1=popt-addr1 -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -u=popt-user -2=popt-addr2 -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -p=popt-password -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax
run test2 "test2 - 19" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city (null)	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --keyfile=popt-key-file --rcfile=popt-rcfile  -1=popt-addr1 -s=popt-state -P=postal -C=popt-country -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 20" "\
dbusername popt-DBUserName	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname popt-first	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --cnhost=popt-content-host --dbusername=popt-DBUserName --first=popt-first  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -P=postal -z=popt-postal
run test2 "test2 - 21" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --storeid=popt-store-id --txhost=popt-transact-host --dbpassword=popt-DBpassword --first=popt-first --last=popt-last  -p=popt-password -c=popt-city -s=popt-state -P=postal -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -C=popt-country -d=popt-dayphone -u=popt-user -c=popt-city -z=popt-postal -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -e=popt-email -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -C=popt-country -d=popt-dayphone
run test2 "test2 - 22" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" 
run test2 "test2 - 23" "\
dbusername (null)	dbpassword popt-DBpassword
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname (null)	lastname popt-last
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --keyfile=popt-key-file --txhost=popt-transact-host --dbpassword=popt-DBpassword --last=popt-last  -u=popt-user -p=popt-password -2=popt-addr2 -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 24" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --txsslport=popt-transact-ssl-port  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax
run test2 "test2 - 25" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --storeid=popt-store-id --rcfile=popt-rcfile --last=popt-last  -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -C=popt-country -d=popt-dayphone
run test2 "test2 - 26" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password (null)	firstname (null)	lastname popt-last
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state (null)	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --txhost=popt-transact-host --cnpath=popt-content-path --last=popt-last  -u=popt-user -1=popt-addr1 -2=popt-addr2 -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -1=popt-addr1 -2=popt-addr2 -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -c=popt-city -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone
run test2 "test2 - 27" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --txsslport=popt-transact-ssl-port --cnhost=popt-content-host  -u=popt-user -2=popt-addr2 -s=popt-state -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -C=popt-country -d=popt-dayphone
run test2 "test2 - 28" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --storeid=popt-store-id --txsslport=popt-transact-ssl-port --cnpath=popt-content-path  -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -1=popt-addr1 -c=popt-city -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax
run test2 "test2 - 29" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname popt-first	lastname popt-last
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal postal
country (null)	email popt-email	dayphone popt-dayphone	fax popt-fax" --rcfile=popt-rcfile --txhost=popt-transact-host --first=popt-first --last=popt-last  -u=popt-user -2=popt-addr2 -c=popt-city -e=popt-email -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -s=popt-state -P=postal -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -c=popt-city -s=popt-state -z=popt-postal -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -d=popt-dayphone
run test2 "test2 - 30" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --rcfile=popt-rcfile --txsslport=popt-transact-ssl-port --cnpath=popt-content-path  -u=popt-user -2=popt-addr2 -z=popt-postal -e=popt-email -F=popt-fax -p=popt-password -1=popt-addr1 -c=popt-city -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -d=popt-dayphone -F=popt-fax
run test2 "test2 - 31" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --storeid=popt-store-id  -u=popt-user -p=popt-password -2=popt-addr2 -c=popt-city -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -1=popt-addr1 -s=popt-state -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 32" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --txhost=popt-transact-host --txsslport=popt-transact-ssl-port --cnhost=popt-content-host --cnpath=popt-content-path --first=popt-first --last=popt-last  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -z=popt-postal -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -P=postal -z=popt-postal -d=popt-dayphone -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -z=popt-postal -e=popt-email -p=popt-password -c=popt-city -s=popt-state -C=popt-country -d=popt-dayphone -F=popt-fax
run test2 "test2 - 33" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --keyfile=popt-key-file --rcfile=popt-rcfile --txhost=popt-transact-host --cnhost=popt-content-host  -1=popt-addr1 -s=popt-state -P=postal -z=popt-postal -p=popt-password -c=popt-city -s=popt-state -P=postal -z=popt-postal -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax
run test2 "test2 - 34" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --storeid=popt-store-id --cnpath=popt-content-path --dbpassword=popt-DBpassword --last=popt-last  -u=popt-user -1=popt-addr1 -2=popt-addr2 -P=postal -C=popt-country -p=popt-password -1=popt-addr1 -2=popt-addr2 -P=postal -z=popt-postal -e=popt-email -d=popt-dayphone -F=popt-fax -p=popt-password -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -e=popt-email -F=popt-fax -p=popt-password -c=popt-city -s=popt-state -z=popt-postal -e=popt-email -F=popt-fax
run test2 "test2 - 35" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --storeid=popt-store-id --rcfile=popt-rcfile --last=popt-last  -u=popt-user -p=popt-password -c=popt-city -s=popt-state -P=postal -z=popt-postal -e=popt-email -d=popt-dayphone -u=popt-user -1=popt-addr1 -s=popt-state -P=postal -z=popt-postal -C=popt-country -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax
run test2 "test2 - 36" "\
dbusername popt-DBUserName	dbpassword popt-DBpassword
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax (null)" --keyfile=popt-key-file --rcfile=popt-rcfile --dbpassword=popt-DBpassword --dbusername=popt-DBUserName  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -1=popt-addr1 -c=popt-city -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -u=popt-user -p=popt-password -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone
run test2 "test2 - 37" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --offerfile=popt-offer-file --storeid=popt-store-id --cnhost=popt-content-host  -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -C=popt-country -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -c=popt-city -s=popt-state -C=popt-country -F=popt-fax -u=popt-user -2=popt-addr2 -c=popt-city -s=popt-state -e=popt-email -d=popt-dayphone
run test2 "test2 - 38" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --txsslport=popt-transact-ssl-port --dbpassword=popt-DBpassword --dbusername=popt-DBUserName --last=popt-last  -p=popt-password -2=popt-addr2 -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -p=popt-password -1=popt-addr1 -s=popt-state -z=popt-postal -C=popt-country -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -z=popt-postal -e=popt-email -d=popt-dayphone -u=popt-user -1=popt-addr1 -P=postal -z=popt-postal -e=popt-email -d=popt-dayphone
run test2 "test2 - 39" "\
dbusername (null)	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --txhost=popt-transact-host --txsslport=popt-transact-ssl-port  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -F=popt-fax
run test2 "test2 - 40" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --offerfile=popt-offer-file --storeid=popt-store-id --cnhost=popt-content-host --cnpath=popt-content-path --dbusername=popt-DBUserName --last=popt-last  -p=popt-password -2=popt-addr2 -c=popt-city -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -p=popt-password -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -e=popt-email -u=popt-user -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -d=popt-dayphone -u=popt-user -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -s=popt-state -z=popt-postal
run test2 "test2 - 41" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country (null)	email popt-email	dayphone (null)	fax popt-fax" --cnpath=popt-content-path  -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -e=popt-email -F=popt-fax
run test2 "test2 - 42" "\
dbusername (null)	dbpassword popt-DBpassword
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username popt-user	password popt-password	firstname popt-first	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --keyfile=popt-key-file --offerfile=popt-offer-file --rcfile=popt-rcfile --cnhost=popt-content-host --dbpassword=popt-DBpassword --first=popt-first  -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -p=popt-password -2=popt-addr2 -P=postal -C=popt-country -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -z=popt-postal -C=popt-country -F=popt-fax
run test2 "test2 - 43" "\
dbusername (null)	dbpassword popt-DBpassword
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname popt-first	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city popt-city	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone (null)	fax (null)" --offerfile=popt-offer-file --dbpassword=popt-DBpassword --first=popt-first  -u=popt-user -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -u=popt-user -p=popt-password -2=popt-addr2 -c=popt-city -s=popt-state -z=popt-postal -e=popt-email -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -s=popt-state -z=popt-postal
run test2 "test2 - 44" "\
dbusername popt-DBUserName	dbpassword (null)
txhost popt-transact-host	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname (null)	lastname (null)
addr1 popt-addr1	addr2 popt-addr2	city (null)	state (null)	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --offerfile=popt-offer-file --txhost=popt-transact-host --cnpath=popt-content-path --dbusername=popt-DBUserName  -p=popt-password -2=popt-addr2 -d=popt-dayphone -u=popt-user -1=popt-addr1 -P=postal -e=popt-email -F=popt-fax -p=popt-password -1=popt-addr1 -2=popt-addr2 -z=popt-postal -e=popt-email -F=popt-fax -u=popt-user -1=popt-addr1 -C=popt-country -e=popt-email -F=popt-fax
run test2 "test2 - 45" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname popt-first	lastname (null)
addr1 (null)	addr2 popt-addr2	city (null)	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone (null)	fax popt-fax" --cnpath=popt-content-path --first=popt-first  -p=popt-password -s=popt-state -P=postal -C=popt-country -e=popt-email -F=popt-fax -u=popt-user -p=popt-password -2=popt-addr2 -s=popt-state -z=popt-postal -C=popt-country -e=popt-email
run test2 "test2 - 46" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --offerfile=popt-offer-file --storeid=popt-store-id --txsslport=popt-transact-ssl-port --first=popt-first  -p=popt-password -1=popt-addr1 -2=popt-addr2 -e=popt-email -d=popt-dayphone -F=popt-fax -1=popt-addr1 -c=popt-city -s=popt-state -C=popt-country -e=popt-email -d=popt-dayphone -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -P=postal -d=popt-dayphone -F=popt-fax -u=popt-user -1=popt-addr1 -2=popt-addr2 -c=popt-city -s=popt-state -e=popt-email -u=popt-user -1=popt-addr1 -c=popt-city -s=popt-state -z=popt-postal -F=popt-fax
run test2 "test2 - 47" "\
dbusername (null)	dbpassword popt-DBpassword
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username popt-user	password popt-password	firstname (null)	lastname popt-last
addr1 popt-addr1	addr2 popt-addr2	city (null)	state popt-state	postal popt-postal
country popt-country	email popt-email	dayphone popt-dayphone	fax popt-fax" --dbpassword=popt-DBpassword --last=popt-last  -1=popt-addr1 -2=popt-addr2 -s=popt-state -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email -F=popt-fax
run test2 "test2 - 48" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --storeid=popt-store-id --dbpassword=popt-DBpassword --dbusername=popt-DBUserName --last=popt-last  -u=popt-user -p=popt-password -2=popt-addr2 -s=popt-state -P=postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -F=popt-fax -P=postal -z=popt-postal -e=popt-email -d=popt-dayphone -F=popt-fax -p=popt-password -1=popt-addr1 -s=popt-state -z=popt-postal -e=popt-email -d=popt-dayphone -F=popt-fax
run test2 "test2 - 49" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --keyfile=popt-key-file --storeid=popt-store-id --dbusername=popt-DBUserName  -p=popt-password -1=popt-addr1 -2=popt-addr2 -c=popt-city -P=postal -z=popt-postal -C=popt-country -e=popt-email -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -c=popt-city -z=popt-postal -C=popt-country -F=popt-fax -p=popt-password -1=popt-addr1 -s=popt-state -P=postal -C=popt-country -e=popt-email
run test2 "test2 - 50" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile (null)
username (null)	password (null)	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone (null)	fax (null)" --txsslport=popt-transact-ssl-port --cnhost=popt-content-host --cnpath=popt-content-path  -u=popt-user -2=popt-addr2 -P=postal -C=popt-country -d=popt-dayphone -F=popt-fax -u=popt-user -p=popt-password -1=popt-addr1 -2=popt-addr2 -P=postal -z=popt-postal -d=popt-dayphone -F=popt-fax -p=popt-password -1=popt-addr1 -c=popt-city -s=popt-state -P=postal -z=popt-postal -C=popt-country -e=popt-email
run test2 "test2 - 51" "\
dbusername (null)	dbpassword (null)
txhost (null)	txsslport 443	txstoreid 0	pathofkeyfile popt-key-file
username (null)	password poptsecret	firstname (null)	lastname (null)
addr1 (null)	addr2 (null)	city (null)	state (null)	postal (null)
country (null)	email (null)	dayphone popt-dayphone	fax popt-fax" --pwd poptsecret --twotest2 -Z popt-fax -V popt-dayphone
run test2 "test2 - 52" "Have your way. Have Fun" -Y "Have your way. Have Fun"

#run_diff test3 "test3 - 51" test3-data/01.input test3-data/01.answer
#run_diff test3 "test3 - 52" test3-data/02.input test3-data/02.answer
#run_diff test3 "test3 - 53" test3-data/03.input test3-data/03.answe

# If called from VALGRIND_ENVIRONMENT 
[ -s $builddir/popt-valgrind-result ] && { echo "Some test fail under Valgrind. Check $builddir/popt-valgrind-result" && exit 1 ; }

rm -f $builddir/popt-valgrind-result 

echo "Passed."
