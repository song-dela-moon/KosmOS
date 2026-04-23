# Master Makefile for KosmOS

.PHONY: all
all:
	$(MAKE) -C kernel
	for dir in apps/*/ ; do \
		if [ -f $$dir/Makefile ]; then \
			$(MAKE) -C $$dir; \
		fi; \
	done

.PHONY: clean
clean:
	$(MAKE) -C kernel clean
	for dir in apps/*/ ; do \
		if [ -f $$dir/Makefile ]; then \
			$(MAKE) -C $$dir clean; \
		fi; \
	done
