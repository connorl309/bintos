#!/bin/bash

mkdir -p tools

current_dir="$(basename "$PWD")"

# Check if the directory name is 'bintos'
if [ "$current_dir" != "bintos" ]; then
    echo "Error: You must be in the bintos repository." >&2
    exit 1
fi

# Path setup
export cc_PREFIX="$PWD/tools/"
export cc_TARGET=x86_64-elf
export PATH="$cc_PREFIX/bin:$PATH"

echo "CC at $cc_PREFIX"
echo "Cross-compiler target is $cc_TARGET"
echo "Toolchain in tools/bin"