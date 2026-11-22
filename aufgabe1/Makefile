.PHONY: clean

CFLAGS = 
DEBUG_FLAGS = -O0 -g
RELEASE_FLAGS = -O3

debug: CFLAGS += $(DEBUG_FLAGS)
debug: bin/aufgabe1

release: CFLAGS += $(RELEASE_FLAGS)
release: bin/aufgabe1

bin/aufgabe1: $(patsubst src/%.c,bin/%.o,$(wildcard src/*.c))
	mkdir -p bin
	clang $(CFLAGS) $^ -o $@ -lm

bin/%.o: src/%.c
	mkdir -p bin
	clang $(CFLAGS) -c $< -I include -o $@

clean:
	rm -rf bin
