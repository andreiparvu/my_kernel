CC=arm-linux-gnueabi-gcc

CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv6 -msoft-float -fPIC -mapcs-frame -marm

LD=arm-linux-gnueabi-ld

LDFLAGS=-N -Ttext=0x10000

.SUFFIXES: .s .o
.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^

.SUFFIXES: .o .elf
.o.elf:
	$(LD) $(LDFLAGS) -o $@ $^

kernel.elf: bootstrap.o kernel.o context_switch.o syscall.o
