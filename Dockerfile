FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update \
    && apt-get -y install --no-install-recommends apt-utils dialog 2>&1 \
    # stuff
    && apt-get -y install git npm less iproute2 \
    build-essential \
    gcc-5 g++-5 libtool cmake curl debconf-utils \
    git-core liblua5.1 \
    liblua5.1-dev libluabind-dev  \
    minizip make locales \
    nano unzip uuid-dev iputils-ping \
    zlibc wget \
    gdb valgrind \
    # vita sdk 
    python sudo autoconf texinfo bison flex \
    # wii
    gdebi-core ca-certificates pkg-config bzip2 xz-utils bsdtar doxygen gnupg \
    # sdl2
    libsdl2-dev \
    libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev \
    # windows support
    mingw-w64 \
    # xbox
    clang lld llvm llvm-dev \
    # raylib
    libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev \
    #clean up
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# for sdl2 in windows, we have to get the mingw source:
RUN wget https://www.libsdl.org/release/SDL2-devel-2.0.12-mingw.tar.gz -O /tmp/mingw.tar.gz
RUN cd /tmp \
    && tar xzf mingw.tar.gz \
    && cp -R SDL2-2.0.12/x86_64-w64-mingw32/include/SDL2 /usr/x86_64-w64-mingw32/include/SDL2 \
    && cp -R SDL2-2.0.12/x86_64-w64-mingw32/lib/* /usr/x86_64-w64-mingw32/lib/ \
    && cp SDL2-2.0.12/x86_64-w64-mingw32/bin/* /usr/x86_64-w64-mingw32/bin/ \
    && rm -rf /tmp/*
RUN cd /tmp \
    && wget https://github.com/raysan5/raylib/releases/download/3.0.0/raylib-3.0.0-Linux-amd64.tar.gz -O /tmp/raylib.tar.gz \
    && tar xzf raylib.tar.gz \
    && cp -R raylib-3.0.0-Linux-amd64/include/raylib.h /usr/x86_64-w64-mingw32/include/raylib.h \
    && cp -R raylib-3.0.0-Linux-amd64/lib/* /usr/x86_64-w64-mingw32/lib/ \
    && rm -rf /tmp/*

# vita SDK
ENV VITASDK /usr/local/vitasdk
ENV PATH=$VITASDK/bin:$PATH
RUN cd /tmp \
    && git clone https://github.com/vitasdk/vdpm \
    && cd vdpm \
    && ./bootstrap-vitasdk.sh \
    && ./install-all.sh \
    && rm -rf /tmp/*

# wii, nds, gamecube, devkit pro
RUN cd /tmp \
    && wget https://github.com/devkitPro/pacman/releases/latest/download/devkitpro-pacman.amd64.deb \
    && gdebi -n devkitpro-pacman.amd64.deb \
    && rm devkitpro-pacman.amd64.deb \
    && dkp-pacman -Scc --noconfirm
RUN dkp-pacman -Syyu --noconfirm gamecube-dev wii-dev wiiu-dev nds-dev \
    && dkp-pacman -S --needed --noconfirm `dkp-pacman -Slq dkp-libs | grep '^ppc-'` \
    && dkp-pacman -Scc --noconfirm
# switch
RUN dkp-pacman -Syyu --noconfirm switch-dev \
    && dkp-pacman -S --needed --noconfirm `dkp-pacman -Slq dkp-libs | grep '^switch-'` \
    && dkp-pacman -Scc --noconfirm \
    && rm -rf /tmp/*

ENV DEVKITPRO=/opt/devkitpro
ENV PATH=/opt/devkitpro/tools/bin:$PATH
ENV DEVKITPPC=/opt/devkitpro/devkitPPC
ENV DEVKITARM=/opt/devkitpro/devkitARM
# GRRLIB for gamecube
RUN cd /tmp \
    && git clone https://github.com/xackery/GRRLIB.git \
    && cd GRRLIB/GRRLIB \
    && make clean all install \
    && rm -rf /tmp/*
ENV GRRLIB=/opt/grrlib
# Raylib
RUN git clone https://github.com/raysan5/raylib.git /opt/raylib \
    && cd /opt/raylib/src/ \
    && make CC=/usr/bin/x86_64-w64-mingw32-gcc PLATFORM=PLATFORM_DESKTOP \
    && make install
# grrlib for various nintendo 
#RUN wget https://github.com/GRRLIB/GRRLIB/archive/v4.4.0.tar.gz -O /tmp/grrlib.tar.gz \
 #   && cd /tmp \
  #  && tar xzf /tmp/grrlib.tar.gz \
  #  && cd GRRLIB-4.4.0/GRRLIB \
  #  && dkp-pacman --sync --needed --noconfirm libfat-ogc ppc-libpng ppc-freetype ppc-libjpeg-turbo \
  #  && make clean all install


# xbox
RUN cd /opt \
    && git clone -b xgu --recurse-submodules https://github.com/dracc/nxdk.git
ENV PATH=/usr/lib/llvm-6.0/bin/:$PATH
# fix locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen \
    && dpkg-reconfigure --frontend=noninteractive locales \
    && update-locale LANG=en_US.UTF-8
ENV DEBIAN_FRONTEND=dialog
ENV LANG en_US.UTF-8
ENV LC_ALL en_US.UTF-8
ENV HOME /src
WORKDIR /src

CMD ["make"]