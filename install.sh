#!/bin/bash

INSTALL_DIR="$HOME/bin"

if [[ ! ":$PATH:" == *":$INSTALL_DIR:"* ]]; then
    echo "Your PATH does not include $INSTALL_DIR, so we'll add it."
    export PATH="$INSTALL_DIR:$PATH"
    echo "export PATH=\"$INSTALL_DIR:\$PATH\"" >> "$HOME"/.zshrc
fi

mkdir -p "$INSTALL_DIR"

if ! make; then
    echo "Compilation failed."
    exit 1
else
    echo "Compilation successful."
fi

mv "catr" "$INSTALL_DIR"

chmod +x "$INSTALL_DIR/catr"

echo "Installation completed. 'catr' is now available."
echo "You might need to restart your terminal or source your profile."
