CC=gcc
SOURCE=eltorito.c fs.c
HEADERS=eltorito.h fs.h

run-example-info: build/examples/info/a.out
	build/examples/info/a.out

build/examples/info/a.out: $(SOURCE)
	mkdir -p build/examples/info/
	gcc -o build/examples/info/a.out examples/main.c $(SOURCE)
