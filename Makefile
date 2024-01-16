arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso
img := build/os-$(arch).img

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

.PHONY: all clean run img run-iso iso

all: $(kernel)

clean:
	@rm -r build

run: $(img)
	@qemu-system-x86_64 -s -drive format=raw,file=$(img) -serial stdio

img: $(img)

# we use a bash script to create the fs img, for in order 
#   execution of `losetup -f` for loop0 and loop1
# see https://stackoverflow.com/a/29085760 for more info
$(img): $(kernel) $(grub_cfg)
	dd if=/dev/zero of=$(img) bs=512 count=32768;			\
	sudo parted $(img) mklabel msdos;						\
	sudo parted $(img) mkpart primary fat32 2048s 30720s;	\
	sudo parted $(img) set 1 boot on;						\
	loop0=$$(sudo losetup -f);									\
	sudo losetup $$loop0 $(img);							\
	loop1=$$(sudo losetup -f);									\
	sudo losetup $$loop1 $(img) -o 1048576;					\
	sudo mkdosfs -F32 -f 2 $$loop1;							\
	sudo mount $$loop1 /mnt/fatgrub;						\
	sudo grub-install 										\
		--directory=/home/cpe454/opt/cross/lib/grub/i386-pc \
		--root-directory=/mnt/fatgrub 						\
		--no-floppy 										\
		--modules="normal part_msdos ext2 multiboot" 		\
		$$loop0;											\
	sudo cp $(kernel) /mnt/fatgrub/boot/kernel.bin;			\
	sudo cp $(grub_cfg) /mnt/fatgrub/boot/grub/grub.cfg;	\
	sudo umount /mnt/fatgrub;								\
	sudo losetup -d $$loop0;								\
	sudo losetup -d $$loop1;								\

run-iso: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(linker_script)
	@x86_64-elf-ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files)

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@