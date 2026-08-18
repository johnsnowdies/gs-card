# GS-CARD Makefile — build & run via DOSBox
#
# Targets:
#   make build   — compile inside DOSBox, copy assets, clean intermediates
#   make run     — launch DOSBox with GSCARD.EXE
#   make clean   — remove all build artifacts (except .gitkeep)

SCRIPT_DIR := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
DOSBOX_CONF_TEMPLATE := $(SCRIPT_DIR)/dosbox_gs.conf
DOSBOX_CONF_RUNTIME := /tmp/gs-card-dosbox-XXXXXX.conf

.PHONY: build run clean

build:
	@echo "========================================"
	@echo "  GS-CARD Build"
	@echo "========================================"
	@echo ""
	@command -v dosbox >/dev/null 2>&1 || { echo "ERROR: dosbox not installed. Try: sudo apt install dosbox"; exit 1; }
	@sed "s|__MOUNT_PATH__|$(SCRIPT_DIR)|g; s|__AUTOEXEC__|COMPILE.BAT|g" \
		"$(DOSBOX_CONF_TEMPLATE)" > "$(DOSBOX_CONF_RUNTIME)"
	@echo "Project root: $(SCRIPT_DIR)"
	@echo "Launching DOSBox for build..."
	@dosbox -conf "$(DOSBOX_CONF_RUNTIME)"
	@rm -f "$(DOSBOX_CONF_RUNTIME)"
	@echo "Done."

run:
	@echo "========================================"
	@echo "  GS-CARD Run"
	@echo "========================================"
	@echo ""
	@command -v dosbox >/dev/null 2>&1 || { echo "ERROR: dosbox not installed. Try: sudo apt install dosbox"; exit 1; }
	@if [ ! -f "$(SCRIPT_DIR)/BUILD/GSCARD.EXE" ]; then \
		echo "GSCARD.EXE not found — run 'make build' first."; \
		exit 1; \
	fi
	@sed "s|__MOUNT_PATH__|$(SCRIPT_DIR)|g; s|__AUTOEXEC__|cd BUILD \&\& GSCARD.EXE|g" \
		"$(DOSBOX_CONF_TEMPLATE)" > "$(DOSBOX_CONF_RUNTIME)"
	@echo "Project root: $(SCRIPT_DIR)"
	@echo "Launching DOSBox for GSCARD.EXE..."
	@dosbox -conf "$(DOSBOX_CONF_RUNTIME)"
	@rm -f "$(DOSBOX_CONF_RUNTIME)"
	@echo "Done."

clean:
	@echo "Cleaning BUILD..."
	@rm -f "$(SCRIPT_DIR)/BUILD/"*.obj
	@rm -f "$(SCRIPT_DIR)/BUILD/"*.exe
	@rm -f "$(SCRIPT_DIR)/BUILD/"*.map
	@rm -f "$(SCRIPT_DIR)/BUILD/"*.rsp
	@rm -f "$(SCRIPT_DIR)/BUILD/"*.log
	@rm -rf "$(SCRIPT_DIR)/BUILD/BGI"
	@rm -f "$(SCRIPT_DIR)/BUILD/LOGO.BMP"
	@rm -f "$(SCRIPT_DIR)/BUILD/SYSTEM.SOL"
	@echo "Done."