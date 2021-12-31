CC= mips64r5900el-ps2-elf-gcc
OBJ = menu.elf
INPUT = menu.c
CFLAGS = -nostartfiles -nostdlib -O1 -T linker_script -G0 -fno-zero-initialized-in-bss
LINKER_OPTIONS =
ifdef TEXT_BASE
	LINKER_OPTIONS = -Wl,--section-start=.text=$(TEXT_BASE)
	OBJ = menu_$(TEXT_BASE).elf
endif

all: $(OBJ)

$(OBJ): $(INPUT)
	$(CC) $(CFLAGS) $(LINKER_OPTIONS) $^ -o $@

force_clean:
	rm -rf *.elf
clean:
	rm -rf $(OBJ)
