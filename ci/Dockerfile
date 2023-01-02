FROM registry.fedoraproject.org/fedora:36
MAINTAINER rpm-maint@lists.rpm.org

WORKDIR /srv/popt

RUN echo -e "deltarpm=0\ninstall_weak_deps=0\ntsflags=nodocs" >> /etc/dnf/dnf.conf
RUN rm -f /etc/yum.repos.d/*modular.repo
RUN dnf -y update
RUN dnf -y install \
  cmake \
  gettext-devel \
  make \
  gcc \
  binutils \
  && dnf clean all

COPY . .

RUN cmake -S . -B ./build
CMD cd build && ctest --output-on-failure --force-new-ctest-process
