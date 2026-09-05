# GS-CARD Makefile — build, bundle & deploy via DOSBox
#
# Targets:
#   make build LANG=<ru|en>   — compile inside DOSBox, copy assets, clean intermediates
#   make run                  — launch DOSBox with GSCARD.EXE
#   make clean                — remove build artifacts (keeps latest/ bundles)
#   make release              — build both languages, prompt for version, create .jsdos bundles + manifest.json in latest/,
#                               and create symlinks in web/wrapper/bundles
#   make deploy               — upload bundles and manifest from latest/ to server via SCP

SCRIPT_DIR := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
DOSBOX_CONF_TEMPLATE := $(SCRIPT_DIR)/dosbox_gs.conf
DOSBOX_CONF_RUNTIME := /tmp/gs-card-dosbox-XXXXXX.conf

export SDL_VIDEODRIVER=dummy

# Default language
LANG ?= ru

.PHONY: build run clean release deploy

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
	@rm -rf "$(SCRIPT_DIR)/BUILD/.jsdos"
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
	@echo "  GS-CARD Release"
	@echo "========================================"
	@read -p "Enter version (e.g., 1.0.0): " VERSION; \
	if [ -z "$$VERSION" ]; then \
		echo "ERROR: Version cannot be empty."; \
		exit 1; \
	fi; \
	echo "Version: $$VERSION"; \
	mkdir -p "$(SCRIPT_DIR)/latest"; \
	echo "========================================"; \
	echo "  Building Russian version"; \
	echo "========================================"; \
	$(MAKE) build LANG=ru; \
	echo "Packaging Russian bundle..."; \
	if [ -d "$(SCRIPT_DIR)/ASSETS/.jsdos" ]; then \
		cp -r "$(SCRIPT_DIR)/ASSETS/.jsdos" "$(SCRIPT_DIR)/BUILD/"; \
	fi; \
	cd "$(SCRIPT_DIR)/BUILD" && zip -r "../latest/bundle-ru-$$VERSION.jsdos" .; \
	echo "Cleaning BUILD..."; \
	$(MAKE) clean; \
	echo "========================================"; \
	echo "  Building English version"; \
	echo "========================================"; \
	$(MAKE) build LANG=en; \
	echo "Packaging English bundle..."; \
	if [ -d "$(SCRIPT_DIR)/ASSETS/.jsdos" ]; then \
		cp -r "$(SCRIPT_DIR)/ASSETS/.jsdos" "$(SCRIPT_DIR)/BUILD/"; \
	fi; \
	cd "$(SCRIPT_DIR)/BUILD" && zip -r "../latest/bundle-en-$$VERSION.jsdos" .; \
	echo "Creating manifest.json..."; \
	echo '{ "ru": "bundle-ru-'"$$VERSION"'.jsdos", "en": "bundle-en-'"$$VERSION"'.jsdos" }' > "$(SCRIPT_DIR)/latest/manifest.json"; \
	echo "Creating symbolic links in web/wrapper/bundles..."; \
	mkdir -p "$(SCRIPT_DIR)/web/wrapper/bundles"; \
	for file in "$(SCRIPT_DIR)/latest/"*.jsdos; do \
		base=$$(basename "$$file"); \
		ln -sf "$$file" "$(SCRIPT_DIR)/web/wrapper/bundles/$$base"; \
	done; \
	if [ ! -L "$(SCRIPT_DIR)/web/wrapper/bundles/manifest.json" ] && [ ! -f "$(SCRIPT_DIR)/web/wrapper/bundles/manifest.json" ]; then \
		ln -s "$(SCRIPT_DIR)/latest/manifest.json" "$(SCRIPT_DIR)/web/wrapper/bundles/manifest.json"; \
	else \
		echo "manifest.json link already exists, ignoring."; \
	fi; \
	echo "========================================"; \
	echo "  Release complete!"; \
	echo "  Files: latest/bundle-ru-$$VERSION.jsdos, latest/bundle-en-$$VERSION.jsdos, latest/manifest.json"; \
	echo "  Symlinks created in web/wrapper/bundles/"; \
	echo "========================================"

deploy:
	@echo "========================================"
	@echo "  Deploying bundles and manifest to server"
	@echo "========================================"
	@if [ ! -f "$(SCRIPT_DIR)/latest/manifest.json" ]; then \
		echo "ERROR: manifest.json not found in latest/. Run 'make release' first."; \
		exit 1; \
	fi
	@scp "$(SCRIPT_DIR)/latest/"bundle-*.jsdos "$(SCRIPT_DIR)/latest/manifest.json" "eslider@eslider.me:/var/www/gs.eslider.me/gs-card/web/wrapper/bundles/"
	@echo "Deployment complete."