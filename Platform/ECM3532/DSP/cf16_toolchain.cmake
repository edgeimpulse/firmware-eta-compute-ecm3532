include (CMakeForceCompiler)

#set(SHELL /bin/bash)
#set(PYTHON3 /tools/python3/bin/python3.5)
###Tool chain export paths are  done in .bashrc file
#SETUPSCRIPT     := /tools/synopsys/asip_programmer_O-2018.09-SP3-5/linux64/chess_env_LNa64.sh 

set(CC chesscc)
set (BINUTILS_PATH  /tools/synopsys/asip_programmer_O-2018.09-SP3-5/linux64/bin/LNa64bin)
set(CF16_LIB_LOC /tools/dsp_software/CoolFlux/cf16_vO-2018.09-1/lib)   ###for cf16 processor

set(CFLAGS "-P ${CF16_LIB_LOC} -p cf16 +W noodle,+c +W noodle,-Wcull -d  +W llvm,-O1")

##Tools for post build command to generate dsp_fw.c output file.
set(DSP_TOOLS ${CMAKE_CURRENT_LIST_DIR}/tools)

############################
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR coolFlux)

set(CMAKE_SYSROOT ${BINUTILS_PATH})

set(CMAKE_C_COMPILER ${BINUTILS_PATH}/${CC})
set(CMAKE_CXX_COMPILER ${BINUTILS_PATH}/${CC})
set(CMAKE_ASM_COMPILER  ${BINUTILS_PATH}/${CC})

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT} CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS "${CFLAGS}" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${CFLAGS}" CACHE STRING "" FORCE)
