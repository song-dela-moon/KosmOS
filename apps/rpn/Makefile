TARGET = rpn

CPPFLAGS += -I.
CFLAGS   += -O2 -Wall -g --target=x86_64-elf -ffreestanding -fPIE
CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding \
            -fno-exceptions -fno-rtti -std=c++17 -fPIE
LDFLAGS += --entry main -z norelro --image-base 0 --static --pie -n

.PHONY: all
all: $(TARGET)

#@range_begin(link_stdlib)
rpn: rpn.o Makefile
	ld.lld $(LDFLAGS) -o rpn rpn.o -lc -lc++ -lc++abi
#@range_end(link_stdlib)

%.o: %.cpp Makefile
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
