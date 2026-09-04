# GS-CARD Makefile — build & run via DOSBox
#
# Targets:
#   make build LANG=<ru|en>   — compile inside DOSBox, copy assets, clean intermediates
#   make run                  — launch DOSBox with GSCARD.EXE
#   make clean                — remove all build artifacts (except .gitkeep and bundles)
#   make release              — build both language versions and create .jsdos bundles

SCRIPT_DIR := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
DOSBOX_CONF_TEMPLATE := $(SCRIPT_DIR)/dosbox_gs.conf
DOSBOX_CONF_RUNTIME := /tmp/gs-card-dosbox-XXXXXX.conf

export SDL_VIDEODRIVER=dummy

# Default language
LANG ?= ru

.PHONY: build run clean release

build:
	@echo "========================================"
	@echo "  GS-CARD Build (LANG=$(LANG))"
	@echo "========================================"
	@echo ""
	@command -v dosbox >/dev/null 2>&1 || { echo "ERROR: dosbox not installed. Try: sudo apt install dosbox"; exit 1; }
	@sed "s|__MOUNT_PATH__|$(SCRIPT_DIR)|g; s|__AUTOEXEC__|COMPILE.BAT $(LANG)|g" \
		"$(DOSBOX_CONF_TEMPLATE)" > "$(DOSBOX_CONF_RUNTIME)"
	@echo "Project root: $(SCRIPT_DIR)"
	@echo "Launching DOSBox for build..."
	@dosbox -conf "$(DOSBOX_CONF_RUNTIME)"
	@rm -f "$(DOSBOX_CONF_RUNTIME)"
	@echo "Done."
	@cat BUILD/BUILD.LOG

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
	@rm -f "$(SCRIPT_DIR)/BUILD/OBJECTS.SOL"
	@rm -f "$(SCRIPT_DIR)/BUILD/BOUNDS.SOL"
	@rm -f "$(SCRIPT_DIR)/BUILD/RUN.BAT"
	@rm -rf "$(SCRIPT_DIR)/BUILD/.jsdos"   # удаляем временную копию
	@echo "Done."

fix:
	@echo "Converting sources to DOS format (CP866 + CRLF)..."
	@find $(SCRIPT_DIR)/SRC -type f \( -name "*.C" -o -name "*.H" \) -exec sh -c \
		'file="$$1"; \
		 if iconv -f utf-8 -t cp866 "$$file" 2>/dev/null > "$$file.tmp"; then \
		   mv "$$file.tmp" "$$file"; \
		   sed -i '\''s/\r//g; s/$$/\r/'\'' "$$file"; \
		   echo "Converted: $$file"; \
		 else \
		   rm -f "$$file.tmp"; \
		   echo "Warning: $$file is not UTF-8, skipped" >&2; \
		 fi' \
		_ {} \;
	@echo "Done."

release:
	@echo "========================================"
	@echo "  Building Russian version"
	@echo "========================================"
	$(MAKE) build LANG=ru
	@echo "Packaging Russian bundle..."
	@if [ -d "$(SCRIPT_DIR)/ASSETS/.jsdos" ]; then \
		cp -r "$(SCRIPT_DIR)/ASSETS/.jsdos" "$(SCRIPT_DIR)/BUILD/"; \
	fi
	@cd "$(SCRIPT_DIR)/BUILD" && zip -r ../latest/bundle-ru.jsdos .
	@echo "Cleaning BUILD..."
	$(MAKE) clean
	@echo "========================================"
	@echo "  Building English version"
	@echo "========================================"
	$(MAKE) build LANG=en
	@echo "Packaging English bundle..."
	@if [ -d "$(SCRIPT_DIR)/ASSETS/.jsdos" ]; then \
		cp -r "$(SCRIPT_DIR)/ASSETS/.jsdos" "$(SCRIPT_DIR)/BUILD/"; \
	fi
	@cd "$(SCRIPT_DIR)/BUILD" && zip -r ../latest/bundle-en.jsdos .
	@echo "========================================"
	@echo "  Release complete!"
	@echo "  Files: bundle-ru.jsdos, bundle-en.jsdos"
	@echo "========================================"

deploy:
	@echo "========================================"
	@echo "  Deploying bundles to server"
	@echo "========================================"
	@if [ ! -f "$(SCRIPT_DIR)/latest/bundle-ru.jsdos" ] || [ ! -f "$(SCRIPT_DIR)/latest/bundle-en.jsdos" ]; then \
		echo "ERROR: Bundles not found in latest/. Run 'make release' first."; \
		exit 1; \
	fi
	@scp "$(SCRIPT_DIR)/latest/bundle-ru.jsdos" "eslider@eslider.me:/var/www/gs.eslider.me/gs-card/web/wrapper/bundles"
	@scp "$(SCRIPT_DIR)/latest/bundle-en.jsdos" "eslider@eslider.me:/var/www/gs.eslider.me/gs-card/web/wrapper/bundles"
	@echo "Deployment complete."