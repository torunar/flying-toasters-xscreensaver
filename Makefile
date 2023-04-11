all: clean build run

run:
	./bin/flying-toasters -windowed

build:
	gcc -o bin/flying-toasters src/flying-toasters.c -L/usr/lib -lX11 -lXpm -s -W -Wall

clean:
	rm -f bin/flying-toasters