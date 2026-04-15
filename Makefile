build: init clean
	docker build . -o bin
	chmod +x bin/*

clean:
	rm -f bin/*

init:
	mkdir -p bin

run: build
	./bin/flying-toasters-amd64 -windowed

all: build run
