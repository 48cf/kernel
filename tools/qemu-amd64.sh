#!/bin/bash

cwd="$(pwd -P)"
iso_path="$cwd/build/platform/amd64/image.iso"

if [ ! -f "$iso_path" ]; then
    echo "ISO not found. Run mkiso-amd64.sh first."
    exit 1
fi

qemu-system-x86_64 -cdrom "$iso_path" -M q35 -m 4G -debugcon stdio

# qemu-system-x86_64 -cdrom "$iso_path" -cpu qemu64,+fsgsbase -M q35,smm=off -m 4G \
#     -debugcon stdio -d int -no-reboot -no-shutdown
