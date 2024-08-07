FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
       build-essential graphviz llvm cmake git peg \
 && rm -rf /var/lib/apt/lists/*
