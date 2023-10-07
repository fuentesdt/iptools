FROM ubuntu:20.04

# setup dependencies 
RUN apt-get update
RUN apt-get install -y build-essential
RUN apt-get install -y doxygen 
RUN apt-get install -y git python2.7
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN apt-get install -y tzdata
RUN apt-get install -y graphviz libfreetype6-dev libglu1-mesa-dev
RUN apt-get install -y libssl-dev libtiff5-dev libxcomposite1 libxcursor1 libxdamage-dev libxi-dev libxkbcommon-x11-0 libxt-dev mesa-common-dev
RUN apt-get install -y cmake 

# install 
RUN mkdir /opt/ants
WORKDIR /opt/ants

RUN git clone https://github.com/ANTsX/ANTs.git
WORKDIR /opt/ants/ANTs
RUN git checkout v2.5.0 -b v2.5.0
WORKDIR /opt/ants
RUN mkdir build install
WORKDIR /opt/ants/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/opt/ants/install ../ANTs 2>&1 | tee cmake.log
RUN make -j 4 2>&1 | tee build.log
WORKDIR /opt/ants/build/ANTS-build
RUN make install 2>&1 | tee install.log
# install c3d
RUN mkdir /opt/c3d
WORKDIR /opt/c3d/
RUN apt-get install -y wget 
RUN wget https://downloads.sourceforge.net/project/c3d/c3d/Nightly/c3d-nightly-Linux-x86_64.tar.gz
RUN tar -xvf c3d-nightly-Linux-x86_64.tar.gz
RUN cp c3d-1.1.0-Linux-x86_64/bin/c?d /usr/local/bin/

# env
ENV PATH /opt/ants/install/bin:$PATH

# itk deps
RUN apt-get install -y cmake-curses-gui vim libinsighttoolkit4.13 libinsighttoolkit4-dev 
# docker disable documentation
# https://unix.stackexchange.com/questions/684774/missing-files-after-installing-package-on-nginx-docker-image
RUN sed -i 's/path-exclude=\/usr\/share\/doc/#path-exclude=\/usr\/share\/doc/g' /etc/dpkg/dpkg.cfg.d/excludes
RUN apt-get install -y insighttoolkit4-examples 
run mkdir /opt/itk
RUN gunzip -d -c /usr/share/doc/insighttoolkit4-examples/examples/IO/DicomSeriesReadImageWrite2.cxx.gz  > /opt/itk/DicomSeriesReadImageWrite2.cxx
RUN gunzip -d -c /usr/share/doc/insighttoolkit4-examples/examples/IO/DicomImageReadPrintTags.cxx.gz     > /opt/itk/DicomImageReadPrintTags.cxx
RUN gunzip -d -c /usr/share/doc/insighttoolkit4-examples/examples/IO/DicomSeriesReadPrintTags.cxx.gz    > /opt/itk/DicomSeriesReadPrintTags.cxx

ADD ./CMakeLists.txt /opt/itk
WORKDIR /opt/itk
RUN cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON -DITK_DIR=/usr/lib/cmake/ITK-4.13/
RUN make 
RUN mkdir /data
RUN mkdir /out

# install mitk
RUN mkdir /opt/mitk
WORKDIR /opt/mitk

RUN git clone https://phabricator.mitk.org/source/mitk.git MITK
WORKDIR /opt/mitk/MITK
RUN git checkout  v2023.04.2  -b  v2023.04.2

RUN mkdir /opt/cmake /opt/cmake/install
WORKDIR /opt/cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.20.2/cmake-3.20.2.tar.gz
RUN tar -zxvf cmake-3.20.2.tar.gz
WORKDIR /opt/cmake/cmake-3.20.2
RUN ./bootstrap --prefix=/opt/cmake/install
RUN make 
RUN make install
## 
WORKDIR /opt/mitk
RUN mkdir build install
WORKDIR /opt/mitk/build
RUN /opt/cmake/install/bin/cmake -DMITK_USE_BLUEBERRY=OFF -DMITK_USE_CTK=OFF -DMITK_USE_Qt5=OFF -DCMAKE_INSTALL_PREFIX=/opt/mitk/install ../MITK
RUN make -j6
##  denoising code
RUN apt-get install -y exuberant-ctags
WORKDIR /opt/mitk/ImageDenoising
ADD ./ImageDenoising/ /opt/mitk/ImageDenoising
RUN ctags -R ../MITK/
RUN cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON  -DITK_DIR=../build/ep/lib/cmake/ITK-5.2/
RUN make

