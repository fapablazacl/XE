FROM ubuntu:latest

RUN apt update 
RUN apt -y upgrade

RUN apt install -y binutils
RUN apt install -y build-essential
RUN apt install -y cmake
RUN apt install -y gcc
RUN apt install -y clang
RUN apt install -y cppcheck
RUN apt install -y ninja-build
RUN apt install -y clang-tools
RUN apt install -y clang-format
RUN apt install -y clang-tidy
RUN apt install -y make
RUN apt install -y g++
RUN apt install -y autoconf 
RUN apt install -y automake 
RUN apt install -y curl
RUN apt install -y git
RUN apt install -y zip
RUN apt install -y pkg-config

RUN mkdir /workspace/
WORKDIR /workspace/

CMD [make]
