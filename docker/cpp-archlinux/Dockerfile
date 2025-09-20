FROM archlinux:latest

# Update system and install base-devel (includes gcc, g++, make, etc.)
RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm base-devel

# compilers
RUN pacman -S --noconfirm gcc clang binutils

# build tools
RUN pacman -S --noconfirm cmake ninja python-pip autoconf automake pkgconf
RUN pip install --break-system-packages pipx
RUN pipx install conan
RUN pipx ensurepath

ENV PATH="$PATH:/root/.local/bin"

# static analysis, code format, etc
RUN pacman -S --noconfirm cppcheck clang-tools-extra clang-format

# misc tools
RUN pacman -S --noconfirm curl git zip

# system libraries required by some dependencies
RUN pacman -S --noconfirm mesa egl-wayland xorg-xkb-utils
RUN pacman -S --noconfirm libx11 libfontenc libice libsm libxau libxaw libxcomposite libxcursor libxdamage libxfixes libxi libxinerama
RUN pacman -S --noconfirm libxkbfile libxmuu libxrandr libxrender libxres libxss libxtst libxv libxxf86vm
RUN pacman -S --noconfirm libxcb libxcb-util libxcb-image libxcb-keysyms libxcb-randr libxcb-shape libxcb-xfixes libxcb-xinerama libxcb-dri3 libxcb-cursor
RUN pacman -S --noconfirm libxcb-icccm libxcb-ewmh libxcb-present libxcb-composite libxcb-res uuid

# setup default working directory
RUN mkdir /workspace/
WORKDIR /workspace/

CMD ["make"]
