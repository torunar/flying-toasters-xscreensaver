FROM ubuntu:18.04 AS build

RUN apt update && \
    apt upgrade --yes && \
    apt install --yes build-essential gcc libx11-dev libxpm-dev

COPY . /app
WORKDIR /app
RUN make build

FROM scratch AS export
COPY --from=build /app/bin/flying-toasters /flying-toasters
