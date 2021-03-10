cmake_minimum_required(VERSION 3.12.1)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/ECM35xx_toolchain.cmake)
set(PROJ_DIR ${CMAKE_CURRENT_LIST_DIR})
project(ECM3532 C CXX ASM)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJ_DIR}/pre_built)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJ_DIR}/pre_built)

#set(CMAKE_VERBOSE_MAKEFILE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message("tookchain file " ${CMAKE_TOOLCHAIN_FILE})
set(CMAKE_SYSTEM_PROCESSOR arm)
enable_language(C CXX ASM)
include("${CMAKE_CURRENT_LIST_DIR}/cmake/extensions.cmake")
include(extensions)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake")
    include(GetGitRevisionDescription)
    git_describe(VERSION --tags)
    message("git tag ver ${VERSION}")
    string(REGEX MATCHALL "^[a-zA-Z0-9\_]+"  PKG_NAME "${VERSION}")
    string(REGEX REPLACE "_[0-9]$" "" PKG_NAME "${PKG_NAME}")
    string(REGEX REPLACE "${PKG_NAME}\." "" VERNUM "${VERSION}")
    string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERNUM}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERNUM}")
    string(REGEX REPLACE "^([0-9\\.])" "" VERSION_EXTRA "${VERNUM}")
    string(REGEX REPLACE "^-" "" VERSION_EXTA "${VERSION_EXTRA}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" BUILD_INFO "${VERSION_EXTRA}")
    string(REGEX REPLACE "^-" "" BUILD_INFO "${BUILD_INFO}")
    set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
    set(VERSION_EXTRA "${BUILD_INFO}")
    set(SRC_PKG_NAME ${PKG_NAME})
endif ()
#find_library(LIB_C NAMES c PATH_SUFFIXES "nofp")
#message("LIB_C:  ${LIB_C}")
set(Kconfiglib_CONFIG_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(Kconfiglib_HEADER_OUTPUT "${APP_DIR}")
set(Python_3_OK TRUE)
find_package(Python)
set(Kconfiglib "${CMAKE_CURRENT_LIST_DIR}/../../../Thirdparty/Kconfiglib")
set(Kconfiglib_CONFIG_FILENAME "Kconfig")
set(Kconfiglib_KCONFIG_CONFIG ${PROJECT_BINARY_DIR}/.config)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

if(NOT EXISTS "${Kconfiglib_KCONFIG_CONFIG}")
    execute_process(
    COMMAND ${Python_EXECUTABLE} ${Kconfiglib}/alldefconfig.py
    ${Kconfiglib_CONFIG_DIR}/${Kconfiglib_CONFIG_FILENAME}
    )
else()
    file(READ ${Kconfiglib_KCONFIG_CONFIG} configfile)
endif()

file(READ ${Kconfiglib_KCONFIG_CONFIG} configfile)
set(getsoc "CONFIG_SOC")
CONFIGVAL(socname ${getsoc})
set(getsoc ${socname})

project(${APP} C CXX ASM)
set(APP_NAME "${APP}")

file(WRITE ${PROJ_DIR}/appKconfig "orsource \"${APP_DIR}/Kconfig\"\n
orsource \"${APP_DIR}/VerKconfig\"\n")
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake")
  file(WRITE ${APP_DIR}/VerKconfig "\n config APP_NAME\n    string\n    default \"${APP}\"
\n config SW_VERSION\n    string\n    default \"${PKG_NAME}-${VERSION}\"
\n config SW_VERSION_EXTRA\n    string\n    default \"${VERSION_EXTRA}\"")
endif ()

set(linkercfg "CONFIG_LINKER_FILE")
CONFIGVAL(linkerf ${linkercfg})
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/scripts/linker/${linkerf}.ld.S)
message("LINKER_SCRIPT ${LINKER_SCRIPT}")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${LINKER_FLAGS} -T ${APP_DIR}/${PROJECT_NAME}.ld ")
set(CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS} \
-Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map --specs=nano.specs -lc -lrdimon -u _printf_float -T ${APP_DIR}/${PROJECT_NAME}.ld ")

include_directories(${APP_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/util/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/hw/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/hw/include/${getsoc})

GETCONFIG(azen "AZURE_IOT_SDK")
string(COMPARE EQUAL "${azen}" y _azcmp)

if (_azcmp)
    add_library(azure "${Kconfiglib_HEADER_OUTPUT}/config.h")
    SET_TARGET_PROPERTIES(azure PROPERTIES LINKER_LANGUAGE C)
    set(azurelib azure)
else ()
    set(azurelib "")
endif ()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/hw/csp/${getsoc}/m3/csp/src")
    message("building csp from src")
    add_library(csp "${Kconfiglib_HEADER_OUTPUT}/config.h")
    SET_TARGET_PROPERTIES(csp PROPERTIES LINKER_LANGUAGE C)
    SET_TARGET_PROPERTIES(csp PROPERTIES COMPILE_FLAGS "-Os")
    set(csplib csp)
else ()
    find_library(csplib csp HINTS ${PROJ_DIR}/pre_built)
    add_library(csp STATIC IMPORTED)
    set_property(TARGET csp PROPERTY IMPORTED_LOCATION ${csplib})
    include_directories("${CMAKE_CURRENT_LIST_DIR}/hw/csp/${getsoc}/m3/csp/inc")
    include_directories("${CMAKE_CURRENT_LIST_DIR}/hw/csp/${getsoc}/m3/reg/inc")
    include_directories("${CMAKE_CURRENT_LIST_DIR}/hw/csp/${getsoc}/common/csp/inc")
    include_directories("${CMAKE_CURRENT_LIST_DIR}/hw/csp/common/inc")
endif()

if(EXISTS "${PROJ_DIR}/../../../Thirdparty/eta_ai_bsp")
    add_library(eta_ai_bsp "${Kconfiglib_HEADER_OUTPUT}/config.h")
    SET_TARGET_PROPERTIES(eta_ai_bsp PROPERTIES LINKER_LANGUAGE C)
    SET_TARGET_PROPERTIES(eta_ai_bsp PROPERTIES COMPILE_FLAGS "-O3")
    target_link_libraries(eta_ai_bsp LINK_PUBLIC ${csplib} ${hallib})
    set(eta_ai_bsplib eta_ai_bsp)
else ()
    find_library(eta_ai_bsplib eta_ai_bsp HINTS ${PROJ_DIR}/pre_built)
    add_library(eta_ai_bsp STATIC IMPORTED)
    set_property(TARGET eta_ai_bsp PROPERTY IMPORTED_LOCATION ${eta_ai_bsplib})
    include_directories("${CMAKE_CURRENT_LIST_DIR}/include/")
endif()

if(EXISTS "${PROJ_DIR}/../../../Thirdparty/FreeRTOS")
    add_library(freertos "${Kconfiglib_HEADER_OUTPUT}/config.h")
    SET_TARGET_PROPERTIES(freertos PROPERTIES LINKER_LANGUAGE C)
if (${FORCE_OPT_SIZE})
    SET_TARGET_PROPERTIES(freertos PROPERTIES COMPILE_FLAGS "-Os")
else ()
    SET_TARGET_PROPERTIES(freertos PROPERTIES COMPILE_FLAGS "-O3")
endif ()
    set(freertoslib freertos)
else ()
    find_library(freertoslib freertos HINTS ${PROJ_DIR}/pre_built)
    add_library(freertos STATIC IMPORTED)
    set_property(TARGET freertos PROPERTY IMPORTED_LOCATION ${freertoslib})
endif()

if(EXISTS "${PROJ_DIR}/../../../Thirdparty/edge_impulse")
    add_library(edgeimpulse "${Kconfiglib_HEADER_OUTPUT}/config.h")
    SET_TARGET_PROPERTIES(edgeimpulse PROPERTIES LINKER_LANGUAGE CXX)
    SET_TARGET_PROPERTIES(edgeimpulse PROPERTIES COMPILE_FLAGS "-O3 -mcpu=cortex-m3 -mthumb -mlittle-endian -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -fno-common -MMD -MP")
    target_link_libraries(edgeimpulse LINK_PUBLIC ${csplib} ${hallib} ${eta_ai_bsplib} ${freertoslib})
    set(edgeimpulselib edgeimpulse)
else ()
    find_library(edgeimpulselib edgeimpulse HINTS ${PROJ_DIR}/pre_built)
    add_library(edgeimpulse STATIC IMPORTED)
    set_property(TARGET edgeimpulse PROPERTY IMPORTED_LOCATION ${edgeimpulselib})
    include_directories("${CMAKE_CURRENT_LIST_DIR}/ingestion-sdk-platform/eta-compute/")
    include_directories("${CMAKE_CURRENT_LIST_DIR}/QCBOR/inc")
    include_directories("${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/third_party/flatbuffers/include")
endif()

GETCONFIG(rpcen "RPC")
string(COMPARE EQUAL "${rpcen}" y _rpcen)
if (_rpcen)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/framework/rpc/src")
        add_library(rpc "${Kconfiglib_HEADER_OUTPUT}/config.h")
        SET_TARGET_PROPERTIES(rpc PROPERTIES LINKER_LANGUAGE C)
if (${FORCE_OPT_SIZE})
        SET_TARGET_PROPERTIES(rpc PROPERTIES COMPILE_FLAGS  "-Os")
else ()
        SET_TARGET_PROPERTIES(rpc PROPERTIES COMPILE_FLAGS  "-O3")
endif ()
        set(rpclib rpc)
        target_link_libraries(${rpclib} LINK_PUBLIC ${freertoslib} ${hallib})
    else ()
        find_library(rpclib rpc HINTS ${PROJ_DIR}/pre_built)
        include_directories("${CMAKE_CURRENT_LIST_DIR}/framework/rpc/include")
        include_directories("${CMAKE_CURRENT_LIST_DIR}/framework/include/common")
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/hw/hal/${socname}/uart/src" OR
        EXISTS "${CMAKE_CURRENT_LIST_DIR}/hw/hal/common/uart/src")
    add_library(hal "${Kconfiglib_HEADER_OUTPUT}/config.h")
    SET_TARGET_PROPERTIES(hal PROPERTIES LINKER_LANGUAGE C)
if (${FORCE_OPT_SIZE})
    SET_TARGET_PROPERTIES(hal PROPERTIES COMPILE_FLAGS "-Os")
else ()
    SET_TARGET_PROPERTIES(hal PROPERTIES COMPILE_FLAGS "-O3")
endif ()
    target_link_libraries(hal LINK_PUBLIC ${csplib} ${freertoslib} ${rpclib})
    set(hallib hal)
else ()
    find_library(hallib hal HINTS ${PROJ_DIR}/pre_built)
    add_library(hal STATIC IMPORTED)
    set_property(TARGET hal PROPERTY IMPORTED_LOCATION ${hallib})
    include_directories("${CMAKE_CURRENT_LIST_DIR}/hw/hal/common/include")
    message("linking hal ${hallib} ${csplib}")
endif ()

find_package(Doxygen)
if ( DOXYGEN_FOUND )
    include(doxy)
   else (DOXYGEN_FOUND)
    message("Doxygen not found ")
endif (DOXYGEN_FOUND)


GETCONFIG(execen "EXECUTOR")
string(COMPARE EQUAL "${execen}" y _execen)
if (_execen)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/framework/executor/src")
        add_library(executor "${Kconfiglib_HEADER_OUTPUT}/config.h")
        SET_TARGET_PROPERTIES(executor PROPERTIES LINKER_LANGUAGE C)
        SET_TARGET_PROPERTIES(executor PROPERTIES COMPILE_FLAGS  "-Os")
        set(executorlib executor)
        find_library(nn_lib eta_nn_functions HINTS ${PROJ_DIR}/pre_built)
        target_link_libraries(${executorlib} LINK_PUBLIC ${freertoslib}
            ${rpclib} ${nn_lib})
    else ()
        find_library(nn_lib eta_nn_functions HINTS ${PROJ_DIR}/pre_built)
        find_library(executorlib executor HINTS ${PROJ_DIR}/pre_built)
        set(executorlib ${executorlib};${nn_lib})
        message("executor ${executorlib}")
    endif()
endif()

GETCONFIG(smen "SENSOR_MANAGER")
string(COMPARE EQUAL "${smen}" y _smen)
if (_smen)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/framework/sensor_mgr/src")
        add_library(sm "${Kconfiglib_HEADER_OUTPUT}/config.h")
        SET_TARGET_PROPERTIES(sm PROPERTIES LINKER_LANGUAGE C)
if (${FORCE_OPT_SIZE})
        SET_TARGET_PROPERTIES(sm PROPERTIES COMPILE_FLAGS  "-Os")
else ()
        SET_TARGET_PROPERTIES(sm PROPERTIES COMPILE_FLAGS  "-O3")
endif ()
        set(smlib sm)
        target_link_libraries(${smlib} LINK_PUBLIC ${freertoslib} ${hallib})
    else ()
        find_library(smlib sm HINTS ${PROJ_DIR}/pre_built)
        include_directories("${CMAKE_CURRENT_LIST_DIR}/framework/sensor_mgr/include")
        include_directories("${CMAKE_CURRENT_LIST_DIR}/../Common/sensor_mgr/inc")
    endif()
endif()

GETCONFIG(sproto "SERIAL_PROTOCOL")
string(COMPARE EQUAL "${sproto}" y _sproto)
if (_sproto)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/framework/serial_proto/src")
        add_library(sproto "${Kconfiglib_HEADER_OUTPUT}/config.h")
        SET_TARGET_PROPERTIES(sproto PROPERTIES LINKER_LANGUAGE C)
if (${FORCE_OPT_SIZE})
        SET_TARGET_PROPERTIES(sproto PROPERTIES COMPILE_FLAGS  "-Os")
else ()
        SET_TARGET_PROPERTIES(sproto PROPERTIES COMPILE_FLAGS  "-O3")
endif ()
        set(sprotolib sproto)
        target_link_libraries(${sprotolib} LINK_PUBLIC ${freertoslib} ${hallib})
    endif()
endif()

GETCONFIG(pmen "PM_ENABLE")
string(COMPARE EQUAL "${pmen}" y _pmen)
if (_pmen)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/hw/power/${socname}/src")
    add_library(pm "${Kconfiglib_HEADER_OUTPUT}/config.h")
    SET_TARGET_PROPERTIES(pm PROPERTIES LINKER_LANGUAGE C)
if (${FORCE_OPT_SIZE})
    SET_TARGET_PROPERTIES(pm PROPERTIES COMPILE_FLAGS  "-Os")
else ()
    SET_TARGET_PROPERTIES(pm PROPERTIES COMPILE_FLAGS  "-O3")
endif ()
    set(pmlib pm)
    target_link_libraries(${pmlib} LINK_PUBLIC ${csplib} ${freertoslib} ${hallib})
else ()
    find_library(pmlib pm HINTS ${PROJ_DIR}/pre_built)
endif()
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/hw/drivers")
        add_library(iodev "${Kconfiglib_HEADER_OUTPUT}/config.h")
        SET_TARGET_PROPERTIES(iodev PROPERTIES LINKER_LANGUAGE C)
if (${FORCE_OPT_SIZE})
        SET_TARGET_PROPERTIES(iodev PROPERTIES COMPILE_FLAGS  "-Os")
else ()
        SET_TARGET_PROPERTIES(iodev PROPERTIES COMPILE_FLAGS  "-O3")
endif ()
        set(iodevlib iodev)
        target_link_libraries(${iodevlib} LINK_PUBLIC ${freertoslib} ${hallib})
    else ()
        find_library(iodevlib iodev HINTS ${PROJ_DIR}/pre_built)
        message("iodev ${iodevlib}")
endif()

add_executable(${PROJECT_NAME}.elf "")
if (${FORCE_OPT_SIZE})
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES COMPILE_FLAGS "-Os")
else ()
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES COMPILE_FLAGS "-O3")
endif ()



add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../Thirdparty Thirdparty)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/framework framework)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/hw hw)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/util util)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ App)

get_property(plib GLOBAL PROPERTY plib_list_property)
set_property(GLOBAL PROPERTY plib_list_property
        "${plib};${azurelib};${freertoslib};${hallib};${csplib};${pmlib};${rpclib};${smlib};${iodevlib};${executorlib};${sprotolib};${edgeimpulselib};")
get_property(plib GLOBAL PROPERTY plib_list_property)
target_link_libraries(${PROJECT_NAME}.elf LINK_PUBLIC ${plib})
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(LIST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lss)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    #        COMMAND ${CMAKE_OBJCOPY}
    #-Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY}
-Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP}
-x -S -d ${PROJECT_NAME}.elf> ${LIST_FILE}
        COMMENT "Building ${HEX_FILE}
\nBuilding ${BIN_FILE} Building ${LIST_FILE}"
    )

CONFIGVAL(dsp_fw_loc "CONFIG_BUILD_DSP_FW")
CONFIGVAL(dsp_fw_file "CONFIG_PRE_BUILD_DSP_FW")
CONFIGVAL(dsp_fw_file_reloc "CONFIG_PRE_BUILD_DSP_FW_RELOCATED")

add_custom_target(menuconfig
        COMMAND
        ${CMAKE_COMMAND} -E env
        KCONFIG_CONFIG=${Kconfiglib_KCONFIG_CONFIG}
        srctree=${Kconfiglib_CONFIG_DIR}
        ${Python_EXECUTABLE} ${Kconfiglib}/menuconfig.py
        ${Kconfiglib_CONFIG_DIR}/${Kconfiglib_CONFIG_FILENAME}
        COMMAND
        COMMAND ${CMAKE_COMMAND} -E remove -f ${Kconfiglib_HEADER_OUTPUT}/config.h
        COMMAND ${CMAKE_COMMAND} -E remove -f ${APP_DIR}/${PROJECT_NAME}.ld
        COMMAND
        cmake ..  >/dev/null
        COMMAND
        make ses
        COMMAND
        make dsp_update
        )

if (_rpcen)
add_custom_target(dsp_update
        COMMAND
        echo "DSP file found"
        COMMAND
        cp ${CMAKE_CURRENT_LIST_DIR}/util/dsp_fw/${dsp_fw_file} ${dsp_fw_file_reloc}
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/bootloader/relocateStandaloneDSPBinary.py ${dsp_fw_file_reloc} ../${PROJECT_NAME}.ld
        COMMAND
        mv ${dsp_fw_file_reloc} ../${dsp_fw_file_reloc}
        COMMAND
        cp ../${dsp_fw_file_reloc} 03_dsp_fw.bin
        COMMAND
        touch ${CMAKE_CURRENT_LIST_DIR}/util/include/incbin.h
        )
else()
add_custom_target(dsp_update
        )
endif()
add_custom_target(loadconfig
        COMMAND
        ${CMAKE_COMMAND} -E env
        KCONFIG_CONFIG=${Kconfiglib_KCONFIG_CONFIG}
        srctree=${Kconfiglib_CONFIG_DIR}
        ${Python_EXECUTABLE} ${Kconfiglib}/defconfig.py \${CONFIG}
        COMMAND
        cmake ..  >/dev/null
        COMMAND ${CMAKE_COMMAND} -E remove -f ${Kconfiglib_HEADER_OUTPUT}/config.h
        COMMAND
        make ses
        COMMAND
        make dsp_update
        )

add_custom_command(OUTPUT ${Kconfiglib_HEADER_OUTPUT}/config.h PRE_LINK
        COMMAND
        ${CMAKE_COMMAND} -E env KCONFIG_CONFIG=${Kconfiglib_KCONFIG_CONFIG}
        ${Python_EXECUTABLE} ${Kconfiglib}/genconfig.py
        --header-path ${Kconfiglib_HEADER_OUTPUT}/config.h
        ${Kconfiglib_CONFIG_DIR}/${Kconfiglib_CONFIG_FILENAME}
        COMMENT "genconfig file"
        )

#execute_process(
#        COMMAND
#        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/copy_defconfig.py ../configs/
#        COMMENT "copy defconfig files"
#        )

add_custom_command(OUTPUT ${APP_DIR}/${PROJECT_NAME}.ld
        MAIN_DEPENDENCY ${LINKER_SCRIPT} ${Kconfiglib_HEADER_OUTPUT}/config.h
        COMMAND ${CMAKE_C_COMPILER} -E ${LINKER_SCRIPT} -P
        -o ${APP_DIR}/${PROJECT_NAME}.ld -I ${APP_DIR}
        VERBATIM )


if(${getsoc} STREQUAL "ecm3531")
    set (interface "ocd")
else ()
    set (interface "jlink")
endif ()
add_custom_target(load
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/program.py
        --ide=gcc --soc=${getsoc} --interface=${interface} --bin ${PROJECT_NAME}.bin --type s
        )

add_custom_target(load_ocd
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/program.py
        --ide=gcc --soc=${getsoc} --interface=ocd --bin ${PROJECT_NAME}.bin --type s
        )

add_custom_target(flash
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/program.py
        --ide=gcc --soc=${getsoc} --interface=${interface} --bin ${PROJECT_NAME}.bin --type f
        )

add_custom_target(flash_app
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/program.py
        --ide=gcc --soc=${getsoc} --interface=${interface} --bin ${PROJECT_NAME}.bin --type a
        )

add_custom_target(flash_ocd
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/program.py
        --ide=gcc --soc=${getsoc} --interface=ocd --bin ${PROJECT_NAME}.bin --type f
        )

GETCONFIG(blapp "BOOTLOADER_APP")
string(COMPARE EQUAL "${blapp}" y _blapp)
GETCONFIG(blen "BOOTLOADER")
string(COMPARE EQUAL "${blen}" y _blen)

if (_blapp)

if (APPLE)
add_custom_target(flash_bl
    COMMAND
    sh ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/flashtools/flash_mac.sh
)
elseif (UNIX)
add_custom_target(flash_bl
    COMMAND
    sh ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/flashtools/flash_linux.sh
)
elseif (WIN32)
add_custom_target(flash_bl
    COMMAND
    ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/flashtools/flash_windows.bat
)
else ()
    message( FATAL_ERROR "Could not find platform (neither APPLE, UNIX or WIN32) - cannot flash" )
endif()

add_custom_target(create_bin_files
        COMMAND
        sh ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/bootloader/create_bin_files.sh ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_CURRENT_LIST_DIR}/../DSP/app/${dsp_fw_loc}/build/dsp_fw.bin
        )

add_custom_target(flash_ses_bl
        COMMAND
        sh ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/bootloader/app_flash.sh ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_Debug/${PROJECT_NAME}.elf ${CMAKE_CURRENT_LIST_DIR}/../DSP/app/${dsp_fw_loc}/build/dsp_fw.bin
        )
endif()

if (${FORCE_OPT_SIZE})
if (_blen)
add_custom_target(bl_update
        COMMAND
        eta-flash-tool --firmware-path ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/bootloader/bootloader_update_app.bin
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/bootloader/genPartitionTable.py ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
        COMMAND
        cp bootloader.bin 02_m3_fw.bin
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/../../../Tools/bootloader/bl_download.py . 1
        )
endif()
endif()

add_custom_target(erase
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/program.py
        --ide=gcc --soc=${getsoc} --interface=${interface} --bin ${PROJECT_NAME}.bin -me
        )
add_custom_target(erase_ocd
        COMMAND
        ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/program.py
        --ide=gcc --soc=${getsoc} --interface=ocd --bin ${PROJECT_NAME}.bin -me
        )

add_custom_target(distclean
        COMMAND
        cmake ..   >/dev/null
        COMMAND
        make clean
        )

file(GLOB buildFiles "${CMAKE_BINARY_DIR}/*")
file(GLOB cmakefiles "${CMAKE_BINARY_DIR}/CMakeFiles")
file(GLOB makefile "${CMAKE_BINARY_DIR}/Makefile")
file (GLOB cpackf "${CMAKE_BINARY_DIR}/_CPack_Packages/")
file (GLOB keepit "${CMAKE_BINARY_DIR}/.gitkeep")
file (GLOB cfg "${CMAKE_BINARY_DIR}/.config")
list(REMOVE_ITEM buildFiles ${cmakefiles} ${makefile} ${keepit} ${cfg})
set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${buildFiles}
;\\.cmake;CMakeCache.txt;util;cmake_install.cmake;CPackConfig.cmake;CPackSourceConfig.cmake;_CPack_Packages;\\*.bin;\\*.lss;\\*.map;${PKG_NAME}*")
add_custom_target(autoconf ALL DEPENDS ldconf ${Kconfiglib_HEADER_OUTPUT}/config.h)
add_custom_target(ldconf ALL DEPENDS ${APP_DIR}/${PROJECT_NAME}.ld)
add_custom_target(ses ALL DEPENDS autoconf ldconf)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/CPackConfig.cmake")
    include(CPackConfig)
endif ()

include("${CMAKE_CURRENT_LIST_DIR}/cmake/SEGGER/genSESproject.cmake")

if (_rpcen)
  get_property(dspfw_file GLOBAL PROPERTY dspfw_property)
  message("dspfw ${dspfw_file}")
endif()

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../DSP")
    CONFIGVAL(dsp_fw "CONFIG_BUILD_DSP_FW")
    set (dspapp "${CMAKE_CURRENT_LIST_DIR}/../DSP/app/${dsp_fw}")
    set (dspcfg "${CMAKE_CURRENT_LIST_DIR}/../DSP/configs/${dsp_fw}_dsp_defconfig")
    include(ExternalProject)
    ExternalProject_Add(dsp_fw
        SOURCE_DIR ${dspapp}
        BINARY_DIR ${dspapp}/build
        BUILD_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy ${Kconfiglib_HEADER_OUTPUT}/config.h ${dspapp}
        COMMAND ${CMAKE_COMMAND} -E touch ${dspapp}/CMakeLists.txt
        COMMAND cmake .. && make loadconfig CONFIG=${dspcfg} && make
        INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy ${dspapp}/build/dsp_fw.bin ${dspfw_file}
        DEPENDS autoconf
        EXCLUDE_FROM_ALL 1
        )
    add_custom_target(dsp DEPENDS dsp_fw COMMAND cmake .. && make dsp_update)
    ExternalProject_Add(dsp_src
        SOURCE_DIR ${dspapp}
        BINARY_DIR ${dspapp}/build
        BUILD_COMMAND
        COMMAND cmake .. && make loadconfig
        CONFIG=${dspapp}/../../configs/dsp_src_package_defconfig  && make && make src_package
        INSTALL_COMMAND
        COMMAND ${CMAKE_COMMAND} -E copy ${dspapp}/build/DSP.tar.gz ${PROJECT_BINARY_DIR}/
        DEPENDS autoconf
        EXCLUDE_FROM_ALL 1
        COMMAND
        make dsp_update
        )
    add_custom_target(dsp_clean
        COMMAND
        rm -rf ${PROJECT_BINARY_DIR}/dsp_fw-prefix/
        COMMAND
        rm -rf ${PROJECT_BINARY_DIR}/dsp_src-prefix/
    )
      add_custom_target(src_package_all
        COMMAND make loadconfig CONFIG=../../../Platform/ECM3532/M3/configs/src_package_all_defconfig
        COMMAND make dsp_src
        COMMAND make src_package)

endif ()