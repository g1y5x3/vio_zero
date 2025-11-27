#!/bin/bash

# ==========================================
# Setup Script for VIO Zero Data (LaMAR)
# ==========================================

# Stop the script if any command fails
set -e

echo "[1/6] Installing Python dependencies..."
pip install -r requirements.txt

echo "[2/6] Downloading download_lamaria.py..."
# Use the raw GitHub URL to download the file content, not the HTML page
wget -O download_lamaria.py https://raw.githubusercontent.com/cvg/lamaria/main/tools/download_lamaria.py

echo "[3/6] Creating data directory..."
mkdir -p data

echo "[4/6] Running LaMAR downloader..."
# Downloads the specific sequence to the 'data' folder
python3 download_lamaria.py --output_dir data --sequences R_01_easy --type asl

echo "[5/6] Unzipping dataset..."
# Define the expected path of the zip file based on your structure
ZIP_PATH="data/training/R_01_easy/asl_folder/R_01_easy.zip"
TARGET_DIR="data/training/R_01_easy/asl_folder"

if [ -f "$ZIP_PATH" ]; then
    echo "Unzipping $ZIP_PATH..."
    # -o: overwrite files without prompting
    # -d: specify destination directory
    unzip -o "$ZIP_PATH" -d "$TARGET_DIR"
else
    echo "Error: Zip file not found at $ZIP_PATH"
    exit 1
fi

echo "[6/6] Cleaning up directory structure..."
# Move the nested 'aria' folder up one level
SOURCE_MOVE="$TARGET_DIR/R_01_easy/aria"
DEST_MOVE="$TARGET_DIR/"

if [ -d "$SOURCE_MOVE" ]; then
    mv "$SOURCE_MOVE" "$DEST_MOVE"
    echo "Moved 'aria' folder to $DEST_MOVE"
    
    # Optional: Remove the now empty intermediate folder
    rmdir "$TARGET_DIR/R_01_easy" 2>/dev/null || true
else
    echo "Warning: Source folder $SOURCE_MOVE does not exist. Skipping move."
fi

echo "Done! Data setup complete."
