FROM ubuntu:latest

RUN apt update 
RUN apt -y upgrade

# compilers
RUN apt install -y build-essential
RUN apt install -y gcc g++ binutils
RUN apt install -y clang

# build tools
RUN apt install -y make autoconf automake pkg-config cmake ninja-build python3 pipx
RUN pipx install conan
RUN pipx ensurepath

ENV PATH="$PATH:/root/.local/bin"
ENV CONAN_HOME="/workspace/conan/cache/"

# static analysis, code format, etc
RUN apt install -y cppcheck clang-tools clang-format clang-tidy

# misc tools
RUN apt install -y curl git zip

# system libraries required by some dependencies
RUN apt install -y libegl-dev libegl1-mesa-dev xkb-data
RUN apt install -y libx11-dev libx11-xcb-dev libfontenc-dev libice-dev libsm-dev libxau-dev libxaw7-dev
RUN apt install -y libxcomposite-dev libxcursor-dev libxdamage-dev libxfixes-dev libxi-dev libxinerama-dev 
RUN apt install -y libxkbfile-dev libxmuu-dev libxrandr-dev libxrender-dev libxres-dev libxss-dev libxtst-dev 
RUN apt install -y libxv-dev libxxf86vm-dev libxcb-glx0-dev libxcb-render0-dev libxcb-render-util0-dev libxcb-xkb-dev 
RUN apt install -y libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-shape0-dev 
RUN apt install -y libxcb-sync-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-dri3-dev uuid-dev libxcb-cursor-dev 
RUN apt install -y libxcb-dri2-0-dev libxcb-dri3-dev libxcb-present-dev libxcb-composite0-dev libxcb-ewmh-dev libxcb-res0-dev
RUN apt install -y libxcb-util-dev libxcb-util0-dev

# setup default working directory
RUN mkdir /workspace/
WORKDIR /workspace/

CMD [make]
