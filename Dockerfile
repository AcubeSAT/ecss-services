# This is the Dockerfile for the lightspot21/acubesat-ci:latest
# Docker image used at the pipeline. Please take care to generate 
# and push a new image to the lightspot21/acubesat-ci repo every 
# 1-2 weeks in order to ensure that the tools are at the latest version.
# 
# P.S. Tag properly your images with --tag lightspot21/acubesat-ci when
# building.

FROM alpine:latest

# Set a new work directory. DO NOT DELETE THE LINE BELOW
WORKDIR /root/

# Set up clang-tidy version 8
RUN echo "@testing http://dl-cdn.alpinelinux.org/alpine/edge/testing" \
    >> /etc/apk/repositories && apk update && \
    apk add --no-cache --virtual git-deps git && \
    apk add --no-cache build-base cmake && \
    apk add --no-cache python3 && \
    git clone --depth=1 https://github.com/llvm/llvm-project.git -b release/8.x && \
    cmake \
    -DLLVM_ENABLE_PROJECTS="clang-tools-extra;clang" \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DLLVM_TARGETS_TO_BUILD="host" \
    -G "Unix Makefiles" ./llvm-project/llvm && \
    make -j$(nproc) clang-tidy && mv bin/clang-tidy /usr/bin/clang-tidy && \
    rm -rf * && apk del git-deps

# Update package lists and install cmake, cppcheck, doxygen, vera++, 
# gcc and lcov with their dependencies
RUN apk add --no-cache findutils python3-dev \
    cppcheck doxygen vera++@testing lcov@testing

# Install gcovr
RUN python3 -m pip install gcovr

# Start a new shell
ENTRYPOINT ["/bin/sh", "-c"]
