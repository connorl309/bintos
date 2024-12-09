# Building cross-compiler
Run `source initialize.sh` in the top level directory. This will take a while. Make sure you have at least 8GB of RAM available for your system (in WSL's case, at least 16GB of system RAM).

Toolchain can be accessed in the current shell by using `i686-elf-xxxx` where `xxxx` is the toolname, such as `gcc`, `ld`, `ar,` etc.

# Making/running
Navigate into the `src/` directory, then run `make`. This generates a `.bin` and `.iso` file.
Running in QEMU is not automated yet. To do this, run `qemu-system-i386 -cdrom KERNEL_ISO.iso`. Make sure you have `xorriso` and `qemu-system-gui` packages installed (or their equivalents).