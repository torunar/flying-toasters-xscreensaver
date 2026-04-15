FROM ubuntu:18.04 AS build-amd64

RUN apt update && \
    apt upgrade --yes && \
    apt install --yes gcc libx11-dev libxpm-dev 

COPY . /app
WORKDIR /app
RUN gcc -o /tmp/flying-toasters-amd64 /app/src/flying-toasters.c -L/usr/lib -lX11 -lXpm -s -W -Wall

FROM debian:trixie AS build-arm64

RUN dpkg --add-architecture arm64 && \
    apt update && \
    apt upgrade --yes && \
    apt install --yes gcc-aarch64-linux-gnu libx11-dev:arm64 libxpm-dev:arm64

COPY . /app
WORKDIR /app
RUN aarch64-linux-gnu-gcc -o /tmp/flying-toasters-arm64 src/flying-toasters.c -L/usr/lib -lX11 -lXpm -s -W -Wall

FROM scratch AS export
COPY --from=build-amd64 /tmp/flying-toasters-amd64 /flying-toasters-amd64
COPY --from=build-arm64 /tmp/flying-toasters-arm64 /flying-toasters-arm64

