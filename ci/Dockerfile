FROM fedora:latest
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

RUN autoreconf -vi
RUN ./configure
RUN make

CMD make distcheck
