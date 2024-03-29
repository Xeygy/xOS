arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso
img := build/os-$(arch).img
grub := /home/cpe454/opt/cross/lib/grub/i386-pc

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

src_dirs := src/core src/drivers src/games src/memory src/processes src/utils src/fs
include_flags := $(foreach dir,$(src_dirs),-iquote$(dir))
c_source_files := $(foreach dir,$(src_dirs),$(wildcard $(dir)/*.c))
c_depfiles := $(patsubst %.c, \
	build/arch/$(arch)/%.d, $(c_source_files))
c_obj_files := $(patsubst src/%.c, \
	build/arch/$(arch)/%.o, $(c_source_files))

.PHONY: all clean run img run-iso iso

all: $(kernel)

clean:
	@rm -r build

run: $(img)
	@qemu-system-x86_64 -no-reboot -s -drive format=raw,file=$(img) -serial stdio 

# display interrupts, stop on crash
run_int_stop: $(img)
	@qemu-system-x86_64 -d int,cpu_reset -no-reboot -s -drive format=raw,file=$(img) -serial stdio 

# needs another terminal window running `make run`
gdb:
	x86_64-elf-gdb

img: $(img)

# git stash
# git checkout 648b89cc0b5a74f7874d9a54727b557606ec13f7
block_demo:
	dd if=build/os-x86_64.img of=block32 bs=512 skip=32 count=1
	hexdump -C block32
	rm block32

# we use a bash script to create the fs img, for in order 
#   execution of `losetup -f` for loop0 and loop1
# see https://stackoverflow.com/a/29085760 for more info
$(img): $(kernel) $(grub_cfg)
	@dd if=/dev/zero of=$(img) bs=512 count=32768;           \
	sudo parted $(img) mklabel msdos;                        \
	sudo parted $(img) mkpart primary fat32 2048s 30720s;    \
	sudo parted $(img) set 1 boot on;                        \
	loop0=$$(sudo losetup -f);                               \
	sudo losetup $$loop0 $(img);                             \
	loop1=$$(sudo losetup -f);                               \
	sudo losetup $$loop1 $(img) -o 1048576;                  \
	sudo mkdosfs -F32 -f 2 $$loop1;                          \
	sudo mount $$loop1 /mnt/fatgrub;                         \
	sudo grub-install                                        \
		--directory=$(grub)                                  \
		--root-directory=/mnt/fatgrub                        \
		--no-floppy                                          \
		--modules="normal part_msdos ext2 multiboot"         \
		$$loop0;                                             \
	sudo cp $(kernel) /mnt/fatgrub/boot/kernel.bin;          \
	sudo cp $(grub_cfg) /mnt/fatgrub/boot/grub/grub.cfg;     \
	echo "hello world!" | sudo tee /mnt/fatgrub/hello.txt; \
	sudo mkdir /mnt/fatgrub/nest; \
	echo "I'm nested :o" | sudo tee /mnt/fatgrub/nest/bird.txt; \
	md5sum /mnt/fatgrub/hello.txt /mnt/fatgrub/boot/grub/i386-pc/acpi.mod /mnt/fatgrub/boot/grub/i386-pc/minix.mod; \
	sudo umount /mnt/fatgrub;                                \
	sudo losetup -d $$loop0;                                 \
	sudo losetup -d $$loop1;                                 \

run-iso: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(linker_script) $(c_obj_files)
	@x86_64-elf-ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_obj_files)

build/arch/$(arch)/%.o: src/%.c 
	@mkdir -p $(dir $@)	
	@x86_64-elf-gcc $< -c -g -MMD -Wall -Werror $(include_flags) -o $@ -ffreestanding

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(dir $@)
	@nasm -felf64 $< -o $@

include $(wildcard $(c_depfiles))