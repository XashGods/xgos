ASM=nasm

SRC_DIR=src
BUILD_DIR=build

.PHONY: all kernel bootloader clean always 

# bootloader
bootloader: stage1 stage2 create_image

stage1: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: always
	$(MAKE) -C $(SRC_DIR)/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: always
	$(MAKE) -C $(SRC_DIR)/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))

# kernel
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

# always
always:
	mkdir -p $(BUILD_DIR)

create_image: always
	mkdir -p $(BUILD_DIR)/iso/boot/grub
	sudo cp $(BUILD_DIR)/stage2.bin $(BUILD_DIR)/iso/boot/os.bin
	echo 'set timeout=0' > $(BUILD_DIR)/iso/boot/grub/grub.cfg
	echo 'set default=0' >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	echo '' >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	echo 'menuentry "xgOS" {' >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	echo '  multiboot /boot/os.bin' >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	echo '}' >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	sudo grub-mkrescue -o os.iso $(BUILD_DIR)/iso
	rm -rf $(BUILD_DIR)/iso	

# dd if=/dev/zero of=$(BUILD_DIR)/main_floopy.img bs=512 count=2880
# mkfs.fat -F 12 -n "NBOS" $(BUILD_DIR)/main_floopy.img
# dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/main_floopy.img conv=notrunc
# mcopy -i $(BUILD_DIR)/main_floopy.img $(BUILD_DIR)/stage2.bin "::stage2.bin"
# mcopy -i $(BUILD_DIR)/main_floopy.img $(BUILD_DIR)/kernel.bin "::kernel.bin"


# clean
clean:
	$(MAKE) -C $(SRC_DIR)/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	$(MAKE) -C $(SRC_DIR)/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	rm -rf $(BUILD_DIR)/*