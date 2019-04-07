.PHONY: clean distclean build-setup

# Find out where we are.
WEK_SRC_ROOT:=$(shell pwd)

# Choose build directory name.
WEK_PREFIX:=${PREFIX}
ifeq (${WEK_PREFIX},)
	WEK_PREFIX:=${WEK_SRC_ROOT}/build
endif

# Choose default toolchain
CC_DEFAULT:=gcc
CPP_DEFAULT:=${CC_DEFAULT} -E
AS_DEFAULT:=as
LD_DEFAULT:=${CC_DEFAULT}
MAKE_DEFAULT:=make

# Set toolchain programs: Allow user to override defaults.
WEK_CC:=$(if ${CC},${CC},${CC_DEFAULT})
WEK_CPP:=$(if ${CPP},${CPP},${CPP_DEFAULT})
WEK_AS:=$(if ${AS},${AS},${AS_DEFAULT})
WEK_LD:=$(if ${CC},${CC},${LD_DEFAULT})
WEK_MAKE:=$(if ${MAKE},${MAKE},${MAKE_DEFAULT})

# Set up toolchain flags. Assuming GNU toolchain.
WEK_CPPFLAGS:='-I${WEK_SRC_ROOT}/include' \
	$(if ${CPPFLAGS},${CPPFLAGS},)
WEK_CFLAGS:=-m32 -std=gnu99 -Wall -Wextra -pedantic -ffreestanding -fno-pic \
	$(if ${CFLAGS},${CFLAGS},)
WEK_ASFLAGS:=-m32 \
	$(if ${ASFLAGS},${ASFLAGS},)
WEK_LDFLAGS:=-nostdlib '-Blibc' '-Tlinker.ld' \
	$(if ${LDFLAGS},${LDFLAGS},)

WEK_MAKEVARS:=CC="${WEK_CC}" CPP="${WEK_CPP}" AS="${WEK_AS}" LD="${WEK_LD}" \
	CFLAGS="${WEK_CFLAGS}" CPPFLAGS="${WEK_CPPFLAGS}" \
	ASFLAGS="${WEK_ASFLAGS}" LDFLAGS="${WEK_LDFLAGS}"

# Go ahead and commit to the toolchain and flags we set up.
CC:=${WEK_CC}
CPP:=${WEK_CPP}
AS:=${WEK_AS}
LD:=${WEK_LD}
CFLAGS:=${WEK_CFLAGS}
CPPFLAGS:=${WEK_CPPFLAGS}
ASFLAGS:=${WEK_ASFLAGS}
LDFLAGS:=${WEK_LDFLAGS}

all: kernel.elf
dependencies depends depend deps dep:
	$(error build-essential genisoimage i586-elf-gcc i586-elf-binutils)

include rules.mk
include drivers/drivers.mk libc/libc.mk core/core.mk

fat32: kernel.fat32
iso: kernel.iso

kernel.iso: kernel.elf
	genisoimage
kernel.elf: linker.ld libc.a drivers.a core.a
	${LD} ${LDFLAGS} -o $@ \
		-Wl,--start-group \
			$(filter-out linker.ld, $^) \
		-Wl,--end-group

linker.ld: linker.ldS.in
	${CPP} ${CPPFLAGS} -x c -P $^ > $@

# Clean targets
clean: clean-drivers clean-libc clean-core
	rm -f linker.ld kernel *.elf *.iso

distclean: clean
