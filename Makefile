
all: build

build:
	g++ src/main.cpp src/light.cpp -o bin/alife_lights -std=c++11 -lSDL -lSDL_gfx -g -Iinclude

run:
	./alife_lights

buildrun: build run
