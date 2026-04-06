FROM archlinux:latest

# Update system and install base-devel (includes gcc g++ make etc.)
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
RUN pacman -S --noconfirm cppcheck clang-tools-extra

# misc tools
RUN pacman -S --noconfirm curl git zip

# required system dependencies for conan
RUN pacman -S --noconfirm libglvnd xkeyboard-config
RUN pacman -S --noconfirm libfontenc libice libsm libxaw libxcomposite libxcursor libxdamage libxtst libxinerama libxkbfile libxrandr libxres libxss xcb-util-wm xcb-util-image xcb-util-keysyms xcb-util-renderutil libxv xcb-util xcb-util-cursor

# setup default working directory
RUN mkdir /workspace/
WORKDIR /workspace/

CMD ["make"]
