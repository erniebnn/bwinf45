.PHONY: clean

CFLAGS = 
DEBUG_FLAGS = -O0 -g
RELEASE_FLAGS = -O3

debug: CFLAGS += $(DEBUG_FLAGS)
debug: bin/aufgabe3

release: CFLAGS += $(RELEASE_FLAGS)
release: bin/aufgabe3

bin/aufgabe3: $(patsubst src/%.c,bin/%.o,$(wildcard src/*.c))
	clang $(CFLAGS) $^ -o $@ -lm

bin/%.o: src/%.c
	clang $(CFLAGS) -c $< -I include -o $@

clean:
	rm -rf bin/*
