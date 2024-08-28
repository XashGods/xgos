SRC_DIR=src
BUILD_DIR=build

.PHONY: all kernel bootloader clean always 

# bootloader
bootloader: boot create_image

boot: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: kernel
	$(MAKE) -C $(SRC_DIR)/bootloader BUILD_DIR=$(abspath $(BUILD_DIR)) SRC_DIR=$(abspath $(SRC_DIR))

# kernel
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) SRC_DIR=$(abspath $(SRC_DIR))

# always
always:
	mkdir -p $(BUILD_DIR)

create_image: bootloader kernel
	mkdir -p $(BUILD_DIR)/isodir/boot/grub
	sudo cp $(BUILD_DIR)/boot.bin $(BUILD_DIR)/isodir/boot/os.bin
	echo 'set timeout=0' > $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	echo 'set default=0' >> $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	echo '' >> $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	echo 'menuentry "xgOS" {' >> $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	echo '  multiboot /boot/os.bin' >> $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	echo '}' >> $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	sudo grub-mkrescue -o build/os.iso $(BUILD_DIR)/isodir


# clean
clean:
	$(MAKE) -C $(SRC_DIR)/bootloader BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	rm -rf $(BUILD_DIR)/*