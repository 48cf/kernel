arch_c_args = [
    '-fno-pie',
    '-fno-pic',
    '-m64',
    '-mabi=sysv',
    '-mcmodel=kernel',
    '-mno-red-zone',
    '-mno-80387',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-DAMD64',
]

kernel_sources += files(
    'system/cpu.cpp',
    'system/debug.cpp',
)

kernel = executable(
    'kernel',
    kernel_sources,
    c_args: [freestanding_c_args, arch_c_args],
    cpp_args: [freestanding_c_args, freestanding_cpp_args, arch_c_args],
    include_directories: [kernel_include_dirs],
    link_args: [
        freestanding_ld_args,
        '-Wl,-z,max-page-size=0x1000',
        '-Wl,-T', meson.current_source_dir() + '/linker.ld'
    ],
)
