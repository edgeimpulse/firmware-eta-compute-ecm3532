include(CMakeForceCompiler)
if(NOT DEFINED TOOLCHAIN_PREFIX)
set(TOOLCHAIN_PREFIX arm-none-eabi)
endif()
#message (TOOLCHAIN_PREFIX = ${TOOLCHAIN_PREFIX})
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_SYSTEM_VERSION 1)

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

execute_process(
  COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}-gcc
  OUTPUT_VARIABLE BINUTILS_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
get_filename_component(TOOLCHAIN_BASE_DIR ${ARM_TOOLCHAIN_DIR} DIRECTORY)

if (${CMAKE_VERSION} VERSION_EQUAL "3.6.0" OR
        ${CMAKE_VERSION} VERSION_GREATER "3.6")
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc${TOOLCHAIN_EXT}
        CACHE INTERNAL "C Compiler" FORCE)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++${TOOLCHAIN_EXT}
        CACHE INTERNAL "C++ Compiler" FORCE)
else()
    find_program(C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
    cmake_force_c_compiler(${C_COMPILER} GNU)
    find_program(CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
    cmake_force_cxx_compiler(${CXX_COMPILER} GNU)
endif()

if(EXISTS "${TOOLCHAIN_BASE_DIR}/${TOOLCHAIN_PREFIX}")
    set(CMAKE_SYSROOT "${TOOLCHAIN_BASE_DIR}/${TOOLCHAIN_PREFIX}")
else ()
    if(EXISTS "${TOOLCHAIN_BASE_DIR}/lib/${TOOLCHAIN_PREFIX}")
        set(CMAKE_SYSROOT
            "${TOOLCHAIN_BASE_DIR}/lib/${TOOLCHAIN_PREFIX}")
    else()
        message("ERROR SYSROOT path not set")
    endif()
endif()

# Set system depended extensions
if(WIN32)
    set(TOOLCHAIN_EXT ".exe" )
else()
    set(TOOLCHAIN_EXT "" )
endif()

set(CMAKE_ASM_COMPILER
    ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}-gcc${TOOLCHAIN_EXT}
    CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}-objcopy
    CACHE INTERNAL "objcopy tool")
set(CMAKE_OBJDUMP ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}-objdump
    CACHE INTERNAL "objdump tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}-size
    CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT} CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH CACHE STRING "" FORCE)
set(CMAKE_LIBRARY_PATH "${CMAKE_SYSROOT}/lib/thumb/" CACHE STRING "" FORCE)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_FIND_LIBRARY_PREFIXES lib )
set(CMAKE_FIND_LIBRARY_SUFFIXES .a )
set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb -Wall \
-fno-exceptions -ffunction-sections -fdata-sections -Wno-unused-variable -nostartfiles \
-Wno-unused-parameter -Wno-parentheses \
-Wno-unused-function -ggdb -MD -fno-common -fmessage-length=0")
set(LINKER_FLAGS "-Wl,-gc-sections -Wl,-nostdlib -mthumb --specs=nosys.specs ")

set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")

set(ASM_OPTIONS "-x assembler-with-cpp")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} ${ASM_OPTIONS}" CACHE STRING "" FORCE)
