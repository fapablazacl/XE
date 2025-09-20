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
RUN conan profile detect

# static analysis, format, etc
RUN apt install -y cppcheck clang-tools clang-format clang-tidy

# misc tools
RUN apt install -y curl git zip

# setup default working directory
RUN mkdir /workspace/
WORKDIR /workspace/

CMD [make]
