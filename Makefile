CC 			:= gcc
CFLAGS 		:= -Wall

SRCDIR		:= src
BUILDDIR	:= build
INCDIR		:= include
EXAMPLEDIR	:= examples
TARGETDIR	:= build/target

SRC			:= $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS     := $(foreach file, $(SRC:.c=.o), $(subst $(shell echo $(SRCDIR)),$(shell echo $(BUILDDIR)),$(file)))

INC := -I$(INCDIR)

example: $(TARGETDIR)/example
	@echo "Example was built"
	@echo "./build/target/example"

$(TARGETDIR)/example: $(EXAMPLEDIR)/main.c $(OBJECTS)
	$(CC) $(CFLAGS) $(INC) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

.PHONY: $(BUILDDIR)
$(BUILDDIR):
	mkdir $@

.PHONY: $(TARGETDIR)
$(TARGETDIR):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) -r build
