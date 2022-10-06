SDK_DIR = "sdk"

# Temporary build directory for hello world example.
BUILD_DIR_CHECK = "hello_world_build"

.PHONY: clean
clean:
	rm -rf \
		$(BUILD_DIR_CHECK)

# Checks that the environment has been set up correctly.
.PHONY: check
check:
	# Creates a directory for building into.
	mkdir -p $(BUILD_DIR_CHECK)
	# Create bootable image in build directory.
	make \
		-C $(SDK_DIR)/board/qemu_arm_virt/example/hello/ \
		BUILD_DIR=$(PWD)/$(BUILD_DIR_CHECK) \
		SEL4CP_SDK=$(PWD)/$(SDK_DIR) \
		SEL4CP_BOARD=qemu_arm_virt \
		SEL4CP_CONFIG=debug
	# Run the bootable image in QEMU.
	qemu-system-aarch64 \
		-machine virt \
		-cpu cortex-a53 \
		-serial mon:stdio \
		-device loader,file=hello_world_build/loader.img,addr=0x70000000,cpu-num=0 \
		-m size=1G \
		-display none

.PHONY: build
build: check
	# # Untar the the SDK.
	# tar xvf sdk.tar.gz



