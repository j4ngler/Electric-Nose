PORT ?= /dev/ttyUSB0

.PHONY: all build flash monitor clean menuconfig

all: build

build:
	idf.py build

flash:
	idf.py -p $(PORT) flash

monitor:
	idf.py -p $(PORT) monitor

flash-monitor:
	idf.py -p $(PORT) flash monitor

menuconfig:
	idf.py menuconfig

clean:
	idf.py fullclean