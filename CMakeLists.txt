cmake_minimum_required(VERSION 3.10)
project(xformer10 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(xformer10
    src/atari8.vm/atari800.c
    src/atari8.vm/atarikey.c
    src/atari8.vm/atariosb.c
    src/atari8.vm/atarixl5.c
    src/atari8.vm/atarixlb.c
    src/atari8.vm/atarixlc.c
    src/atari8.vm/atarixlo.c
    src/atari8.vm/x6502.c
    src/atari8.vm/xkey.c
    src/atari8.vm/xlxeram.c
    src/atari8.vm/xmon.c
    src/atari8.vm/xsio.c
    src/atari8.vm/xvideo.c
    src/blocklib/blockapi.c
    src/blocklib/sectorio.c
    src/blocklib/mac_hfs.c
    src/gemul8r.c
    src/ddlib.c
    src/sound.c
    src/props.c
    src/printer.c
    src/serial.c
    src/romcard.c
    src/main_linux.c
)

target_compile_definitions(xformer10 PRIVATE
    XFORMER
    LINUX_PORT
    _GNU_SOURCE
)

target_include_directories(xformer10 PRIVATE
    src
    src/atari8.vm
    src/blocklib
    src/res
)

target_compile_options(xformer10 PRIVATE
    -Wno-incompatible-pointer-types
    -Wno-unused-value
    -Wno-implicit-function-declaration
    -fno-strict-aliasing
    -fwrapv
)

target_link_libraries(xformer10 PRIVATE m pthread)
