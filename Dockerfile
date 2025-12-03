FROM debian:12

RUN apt update && apt install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libcurl4-openssl-dev \
    pkg-config \
    zlib1g-dev \
    libopus-dev \
    libogg-dev 

WORKDIR /home/container/
