# i suck at makefile

ifeq ($(ARCH),)
ARCH := amd64
endif

.PHONY: all
all: build
	@meson compile -C $<

.PHONY: iso
mkiso:
	@bash +x ./tools/mkiso-$(ARCH).sh

.PHONY: run
run:
	@bash +x ./tools/qemu-$(ARCH).sh

.PHONY: clean
clean:
	rm -rf build

build:
	@meson setup kernel $@
