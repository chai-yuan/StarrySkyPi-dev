# Makefile
APP_DIR        ?= game
BOOTLOADER_DIR := bootloader

APP_BINARY     := $(APP_DIR)/build/$(APP_DIR).bin
BOOTL_BINARY   := $(BOOTLOADER_DIR)/build/bootloader.bin
FINAL_BINARY   := $(APP_DIR)_$(BOOTLOADER_DIR).bin

.PHONY: all clean

all: $(FINAL_BINARY)
	@echo "========================================================"
	@echo "✅ Build successful!"
	@echo "✅ Final image is available at: $(FINAL_BINARY)"
	@echo "========================================================"


$(FINAL_BINARY): $(APP_BINARY)
	@echo "--- Building Bootloader (depends on app) ---"
	$(MAKE) -C $(BOOTLOADER_DIR) PY_LOAD=$(abspath $(APP_BINARY))
	@cp $(BOOTL_BINARY) $(FINAL_BINARY)

$(APP_BINARY):
	@echo "--- Building Application ---"
	$(MAKE) -C $(APP_DIR)

clean:
	@echo "--- Cleaning all projects ---"
	$(MAKE) -C $(APP_DIR) clean || true
	$(MAKE) -C $(BOOTLOADER_DIR) clean || true
	rm -f *.bin
	@echo "Cleanup complete."
