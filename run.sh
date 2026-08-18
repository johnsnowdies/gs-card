#!/bin/bash
# GS-CARD Build & Run Script
# Generates a temporary DOSBox config with the correct mount path,
# launches DOSBox to compile and run the project

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DOSBOX_CONF_TEMPLATE="$SCRIPT_DIR/dosbox_gs.conf"
DOSBOX_CONF_RUNTIME=$(mktemp /tmp/gs-card-dosbox-XXXXXX.conf)

echo "========================================"
echo "  GS-CARD Build & Run Script"
echo "========================================"
echo ""

# Check if dosbox is installed
if ! command -v dosbox &>/dev/null; then
    echo "ERROR: dosbox is not installed!"
    echo "Install it with: sudo apt install dosbox"
    exit 1
fi

# Generate runtime config with correct mount path
sed "s|__MOUNT_PATH__|${SCRIPT_DIR}|g" "$DOSBOX_CONF_TEMPLATE" > "$DOSBOX_CONF_RUNTIME"

echo "Project root : $SCRIPT_DIR"
echo ""
echo "Launching DOSBox..."
echo "(press Ctrl-C to abort)"
echo ""

# Launch dosbox with the generated config
dosbox -conf "$DOSBOX_CONF_RUNTIME"

# Clean up
rm -f "$DOSBOX_CONF_RUNTIME"

echo ""
echo "Done."