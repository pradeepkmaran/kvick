FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    git \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

RUN mkdir build && cd build && cmake .. && make

CMD ["./build/src/kvick"]
