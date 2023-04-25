build: init clean
	gcc -o bin/flying-toasters src/flying-toasters.c -L/usr/lib -lX11 -lXpm -s -W -Wall

clean:
	rm -f bin/flying-toasters

init:
	mkdir -p bin

run:
	./bin/flying-toasters -windowed

all: build run