.PHONY: clean

CFLAGS = 
DEBUG_FLAGS = -O0 -g
RELEASE_FLAGS = -O3

LIBS := -lncurses

SOURCES := $(wildcard src/*.c)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: bin/aufgabe4

release: CFLAGS += $(RELEASE_FLAGS)
release: bin/aufgabe4

bin/aufgabe4: $(patsubst src/%.c,bin/%.o,$(SOURCES))
	mkdir -p bin
	clang $^ $(LIBS) -o $@

bin/%.o: src/%.c
	mkdir -p bin
	clang $(CFLAGS) -c $< -I include -o $@

clean:
	rm -rf bin
