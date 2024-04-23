CC=clang
SOURCE=eltorito.c fs.c
HEADERS=eltorito.h fs.h

run-example-info: build/examples/info/a.out
	a.exe

build/examples/info/a.out: $(SOURCE)
	$(CC) -o a.exe examples/main.c $(SOURCE)
