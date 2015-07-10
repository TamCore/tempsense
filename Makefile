CC=c++ -std=c++11

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CFLAGS=$(shell pkg-config hidapi --cflags)
    LIBS=$(shell pkg-config hidapi --libs)
endif
ifeq ($(UNAME_S),Linux)
    CFLAGS=$(shell pkg-config hidapi-libusb --cflags)
    LIBS=$(shell pkg-config hidapi-libusb --libs)
endif

all:
	$(CC) $(CFLAGS) $(LIBS) -o tempsense main.cpp 

install:
	install -m0755 tempsense /usr/local/bin/tempsense 

clean:
	rm tempsense
