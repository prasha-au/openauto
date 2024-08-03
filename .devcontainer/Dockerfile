FROM ubuntu:22.04

WORKDIR /root

COPY tooling/setup_deps.sh /root/
COPY tooling/setup_runtime.sh /root/
RUN apt-get update && \
    ./setup_deps.sh && \
    ./setup_runtime.sh && \
    apt-get clean

WORKDIR /workspaces
