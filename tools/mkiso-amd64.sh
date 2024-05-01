#!/bin/bash

cwd="$(pwd -P)"
build_dir="$cwd/build"
limine_dir="$cwd/kernel/vendor/limine"
iso_root="$build_dir/platform/amd64/iso_root"
output_path="$build_dir/platform/amd64/image.iso"

mkdir -p "$iso_root"

cp "$build_dir/platform/amd64/kernel" \
    "$limine_dir/limine-bios.sys" \
    "$limine_dir/limine-bios-cd.bin" \
    "$limine_dir/limine-uefi-cd.bin" \
    "$iso_root"

cat > "$iso_root/limine.cfg" << EOF
# limine.cfg
TIMEOUT=0

:kernel
PROTOCOL=limine
KERNEL_PATH=boot:///kernel
EOF

xorriso -as mkisofs -b limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot limine-uefi-cd.bin -efi-boot-part \
    --efi-boot-image --protective-msdos-label \
    "$iso_root" -o "$output_path"

"$build_dir/limine" bios-install "$output_path"
