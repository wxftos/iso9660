CC=gcc
SOURCE=eltorito.c fs.c

run-example-info: build/examples/info/a.out
	build/examples/info/a.out

build/examples/info/a.out: $(SOURCE)
	mkdir -p build/examples/info/
	$(CC) -o build/examples/info/a.out examples/main.c $^

.PHONY: clean
clean:
	$(RM) -r build
