FROM registry.fedoraproject.org/fedora:36
MAINTAINER rpm-maint@lists.rpm.org

WORKDIR /srv/popt

RUN echo -e "deltarpm=0\ninstall_weak_deps=0\ntsflags=nodocs" >> /etc/dnf/dnf.conf
RUN rm -f /etc/yum.repos.d/*modular.repo
RUN dnf -y update
RUN dnf -y install \
  autoconf \
  automake \
  libtool \
  gettext-devel \
  make \
  gcc \
  binutils \
  && dnf clean all

COPY . .

RUN ./autogen.sh
RUN ./configure --enable-werror CFLAGS="-Wall -Wdeclaration-after-statement -Wextra -Wmissing-format-attribute -Wmissing-noreturn -Wno-gnu-alignof-expression -Wpointer-arith -Wshadow -Wstrict-prototypes -Wundef -Wunused -Wwrite-strings"
RUN make

CMD make distcheck DISTCHECK_CONFIGURE_FLAGS=--enable-werror
