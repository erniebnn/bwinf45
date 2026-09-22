bin/aufgabe2: $(patsubst src/%.c,bin/%.o,$(wildcard src/*.c))
	clang -g -O0 $^ -o $@ -lm

bin/%.o: src/%.c
	clang -g -O0 -c $< -I include -o $@

clean:
	rm -rf bin/*
