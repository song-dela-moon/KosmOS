TARGET = onlyhlt

.PHONY: all
all: $(TARGET)

onlyhlt: onlyhlt.asm Makefile
	nasm -f bin -o $@ $<

.PHONY: clean
clean:
	rm -f onlyhlt
