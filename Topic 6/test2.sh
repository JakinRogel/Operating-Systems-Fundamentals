#!/bin/bash

# Change to the home directory
cd ~ || { echo "Failed to change to home directory"; exit 1; }

# Create the mycode directory
echo "Creating mycode directory..."
mkdir -p mycode
if [ ! -d "mycode" ]; then
    echo "Failed to create mycode directory"
    exit 1
fi

# Change to the mycode directory
echo "Changing to mycode directory..."
cd mycode || { echo "Failed to change to mycode directory"; exit 1; }

# Copy all .c files from the source directory to the mycode directory
echo "Copying .c files to mycode directory..."
find /media/sf_Operating-Systems-Fundamentals -name "*.c" -exec cp {} . \;
if [ $? -ne 0 ]; then
    echo "Failed to copy .c files"
    exit 1
fi

# Navigate up one directory
echo "Navigating up one directory..."
cd .. || { echo "Failed to navigate up one directory"; exit 1; }

# Create the mycode2 directory
echo "Creating mycode2 directory..."
mkdir -p mycode2
if [ ! -d "mycode2" ]; then
    echo "Failed to create mycode2 directory"
    exit 1
fi

# Copy all files from mycode to mycode2
echo "Copying files from mycode to mycode2..."
cp mycode/* mycode2/
if [ $? -ne 0 ]; then
    echo "Failed to copy files from mycode to mycode2"
    exit 1
fi

# Rename the mycode directory to deadcode
echo "Renaming mycode directory to deadcode..."
mv mycode deadcode
if [ $? -ne 0 ]; then
    echo "Failed to rename mycode to deadcode"
    exit 1
fi

# Remove the deadcode directory and all its contents
echo "Removing deadcode directory..."
rm -r deadcode
if [ $? -ne 0 ]; then
    echo "Failed to remove deadcode directory"
    exit 1
fi

echo "Script executed successfully!"
