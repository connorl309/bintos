# Define packages for Ubuntu and Arch
UBUNTU_PACKAGES=("build-essential" "bison" "flex" "libgmp3-dev" "libmpc-dev" "libmpfr-dev" "texinfo" "grub-common" "qemu-system-x86_64" "qemu-system-gui" "grub-pc-bin" "xorriso")
ARCH_PACKAGES=("base-devel" "gmp" "libmpc" "mpfr" "grub" "xorriso" "qemu" "qemu-ui")

# Detect the operating system
if [ -f /etc/os-release ]; then
    source /etc/os-release
else
    echo "Cannot determine the operating system. Exiting."
    exit 1
fi

# List of environment variables to check
vars=("cc_TARGET" "cc_PREFIX")

# Flag to track if any variable is missing
missing=false

for var in "${vars[@]}"; do
    if [[ -z "${!var}" ]]; then
        echo "Error: $var is not defined. Please run \`source init.sh!\`"
        missing=true
    else
        echo "$var is set to '${!var}'"
    fi
done

# Exit with error if any variable was missing
if $missing; then
    exit 1
fi

# Function to check and install packages on Ubuntu
install_ubuntu_packages() {
    echo "Detected Ubuntu-based system."
    sudo apt update
    for pkg in "${UBUNTU_PACKAGES[@]}"; do
        if ! dpkg -l | grep -q "^ii\s*$pkg"; then
            echo "Installing $pkg..."
            sudo apt install -y "$pkg"
        else
            echo "$pkg is already installed."
        fi
    done
}

# Function to check and install packages on Arch
install_arch_packages() {
    echo "Detected Arch-based system."
    sudo pacman -Syu --noconfirm
    for pkg in "${ARCH_PACKAGES[@]}"; do
        if ! pacman -Q "$pkg" &>/dev/null; then
            echo "Installing $pkg..."
            sudo pacman -S --noconfirm "$pkg"
        else
            echo "$pkg is already installed."
        fi
    done
}

# Install packages based on the detected OS
case "$ID" in
    ubuntu|debian)
        install_ubuntu_packages
        ;;
    arch|manjaro)
        install_arch_packages
        ;;
    *)
        echo "Unsupported operating system: $ID"
        exit 1
        ;;
esac

# Builds binutils if they don't exist on your local cc/bin/ folder.
build_binutils() {
    if [[ ! -f "$cc_PREFIX/bin/$cc_TARGET-ld" ]]; then

        echo "\ntools/bin/ is missing binutils - building binutils for $cc_TARGET\n"
        cd $cc_PREFIX/build/
        # Extract binutils if the directory doesn't exist
        if [ ! -d "binutils-2.43.1" ]; then
            echo "Extracting binutils-2.43.1..."
            if [ -f "binutils-2.43.1.tar.gz" ]; then
                tar -xvf binutils-2.43.1.tar.gz
            else
                wget https://ftp.gnu.org/gnu/binutils/binutils-2.43.1.tar.gz
                tar -xvf binutils-2.43.1.tar.gz
            fi
        fi
        mkdir -p build_binutils
        cd build_binutils

        # Run the configure script
        ../binutils-2.43.1/configure --target=$cc_TARGET --prefix="$cc_PREFIX" --with-sysroot --disable-nls --disable-werror --disable-multilib
        if [ $? -ne 0 ]; then
            echo "Error: Configuration failed!"
            exit 1
        fi

        # Build and install binutils
        make -j4 > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo "Error: Build failed!"
            exit 1
        fi

        make install -j4 > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo "Error: Installation failed!"
            exit 1
        fi

        echo "Binutils built and installed successfully."

    fi
}
# Builds GCC if they dont exist on your local cc/bin/ folder.
build_gcc() {
    if [[ ! -f "$cc_PREFIX/bin/$cc_TARGET-gcc" ]]; then

        echo "\ntools/bin/ is missing gcc - building gcc for $cc_TARGET\n"
        cd $cc_PREFIX/build/
        mkdir -p build_gcc

        # Extract GCC if the directory doesn't exist
        if [ ! -d "gcc-14.2.0" ]; then
            echo "Extracting gcc-14.2.0..."
            if [ -f "gcc-14.2.0.tar.gz" ]; then
                tar -xvf gcc-14.2.0.tar.gz
            else
                wget https://ftp.gnu.org/gnu/gcc/gcc-14.2.0/gcc-14.2.0.tar.gz
                tar -xvf gcc-14.2.0.tar.gz
            fi
        fi

        # Create and move into build_gcc directory
        mkdir -p build_gcc
        cd build_gcc

        # Run the configure script
        ../gcc-14.2.0/configure --target=$cc_TARGET --prefix="$cc_PREFIX" --disable-nls --enable-languages=c --without-headers --disable-multilib
        if [ $? -ne 0 ]; then
            echo "Error: Configuration failed!"
            exit 1
        fi

        # Build GCC
        echo "Building GCC..."
        make all-gcc -j8 2>&1
        if [ $? -ne 0 ]; then
            echo "Error: Building GCC failed!"
            exit 1
        fi

        make all-target-libgcc
        if [ $? -ne 0 ]; then
            echo "Error: Building target libgcc failed!"
            exit 1
        fi

        # Install GCC
        echo "Installing GCC..."
        make install-gcc
        if [ $? -ne 0 ]; then
            echo "Error: Installing GCC failed!"
            exit 1
        fi

        make install-target-libgcc
        if [ $? -ne 0 ]; then
            echo "Error: Installing target libgcc failed!"
            exit 1
        fi

        echo "GCC built and installed successfully."

        rm -rf $cc_PREFIX/build/
    fi
}

build_binutils
build_gcc