project(
    'kernel', ['c', 'cpp'],
    default_options: ['c_std=gnu2x', 'cpp_std=gnu++23']
)

freestanding_c_args = [
    '-nostdlib',
    '-ffreestanding',
    '-fno-stack-protector',
    '-fno-stack-check',
]

freestanding_cpp_args = [
    '-fno-exceptions',
    '-fno-rtti',
    '-fno-use-cxa-atexit',
]

freestanding_ld_args = [
    '-nostdlib',
    '-nostartfiles',
    '-static',
    '-Wl,--build-id=none',
]

arch = target_machine.cpu()

kernel_sources = files(
    'src/main.cpp',
    'src/runtime.cpp',
    'src/lib/print.cpp',
    'src/memory/pmem.cpp',
)

# limine host tool
executable(
    'limine',
    'vendor/limine/limine.c',
    native: true,
)

# flanterm
kernel_sources += files(
    'vendor/flanterm/flanterm.c',
    'vendor/flanterm/backends/fb.c',
)

kernel_include_dirs = include_directories(
    'src',
    'vendor/flanterm',
    'vendor/limine',
    'vendor/nanoprintf',
)

if arch == 'x86_64'
    subdir('platform/amd64')
else
    error('unsupported architecture: ' + arch)
endif
