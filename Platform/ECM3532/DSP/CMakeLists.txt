set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cf16_toolchain.cmake)
cmake_minimum_required(VERSION 3.12.1)
set(PROJ_DIR ${CMAKE_CURRENT_LIST_DIR})
message("*********PROJDIR ${PROJ_DIR}")

set(CMAKE_VERBOSE_MAKEFILE ON)  ## This will display messages for all executed command when make is issued.

project(dsp_simple_project C ASM)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJ_DIR}/pre_built)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJ_DIR}/pre_built)
enable_language(C ASM)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/extensions.cmake")
message("### ${CMAKE_CURRENT_LIST_DIR}")

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../M3/cmake/GetGitRevisionDescription.cmake")
    include(${CMAKE_CURRENT_LIST_DIR}/../M3/cmake/GetGitRevisionDescription.cmake)
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


set(Kconfiglib_CONFIG_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(Kconfiglib_HEADER_OUTPUT "${PROJECT_BINARY_DIR}")

set(Python_3_OK TRUE)
find_package(Python)

find_file(KconfiglibPath Kconfiglib HINT "${CMAKE_CURRENT_LIST_DIR}/../../../Thirdparty") 
set(Kconfiglib ${KconfiglibPath})
message(KconfiglibraryPath=${Kconfiglib})

set(Kconfiglib_CONFIG_FILENAME "Kconfig")
set(Kconfiglib_KCONFIG_CONFIG ${PROJECT_BINARY_DIR}/.config)

###Read top level config parameters into configfile list variable
if(NOT EXISTS "${Kconfiglib_KCONFIG_CONFIG}")
    message(.config file doesnt exist, hence generating .config file.)
    execute_process(
    COMMAND ${Python_EXECUTABLE} ${Kconfiglib}/alldefconfig.py
    ${Kconfiglib_CONFIG_DIR}/${Kconfiglib_CONFIG_FILENAME}
    )
else()
    message(reading already existing .config file into configfile list variable)
    file(READ ${Kconfiglib_KCONFIG_CONFIG} configfile)
endif()

file(READ ${Kconfiglib_KCONFIG_CONFIG} configfile)

add_custom_target(preBuildCustomTarget
        COMMENT "genconfig new file"
        COMMAND
        ${CMAKE_COMMAND} -E env KCONFIG_CONFIG=${Kconfiglib_KCONFIG_CONFIG}
        ${Python_EXECUTABLE} ${Kconfiglib}/genconfig.py
        --header-path ${Kconfiglib_HEADER_OUTPUT}/dsp_config.h
        ${Kconfiglib_CONFIG_DIR}/${Kconfiglib_CONFIG_FILENAME}
	#COMMENT "genconfig new file"
        )

set(getsoc "CONFIG_DSP_SOC")
CONFIGVAL(socname ${getsoc})
set(getsoc ${socname})  

project(${APP} C ASM)
set(APP_NAME "${APP}")
message("*****APP:DIR soc ${APP} ${APP_DIR} ${getsoc}")

#set(APP_DIR app/${APPVAL})
file(WRITE ${PROJ_DIR}/appKconfig "orsource \"${APP_DIR}/Kconfig\"")

include_directories(${APP_DIR})
include_directories(${PROJECT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/packaging)

#add include directories for Common (between M3 and DSP)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/framework/inc)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/executor/inc)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/sensor_mgr/inc)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Common/audio/inc)


if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/hw/csp/${socname}/src")
    message("building csp from src")
    add_library(csp "")
    SET_TARGET_PROPERTIES(csp PROPERTIES LINKER_LANGUAGE C)
    set(csplib csp)
else ()
    message("building csp from csplib")
    find_library(csplib csp HINTS ${PROJ_DIR}/pre_built)
    add_library(csp STATIC IMPORTED)
    set_property(TARGET csp PROPERTY IMPORTED_LOCATION ${cspib})
endif()

set(mathvar "CONFIG_ETA_MATH")
CONFIGVAL(mathvaren ${mathvar})
string(COMPARE EQUAL "${mathvaren}" y eta_math_en)
if (eta_math_en)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/eta_math/src")
        message("building eta math from src")
        add_library(etaMath "")
        SET_TARGET_PROPERTIES(etaMath PROPERTIES LINKER_LANGUAGE C)
        set(etaMathlib etaMath)
    else ()
        message("building etaMath from etaMathlib")
        find_library(etaMathlib etaMath HINTS ${PROJ_DIR}/pre_built)
        add_library(etaMath STATIC IMPORTED)
        set_property(TARGET etaMath PROPERTY IMPORTED_LOCATION ${etaMathlib})
        #include_directories("${CMAKE_CURRENT_LIST_DIR}/framework/rpc/include")
    endif()
endif()

set(cfgvar "CONFIG_NN_KERNELS")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y nn_kernel_en)
if (nn_kernel_en)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/kernel/src")
        message("building dspkernel from src")
        add_library(dspkernel "")
        SET_TARGET_PROPERTIES(dspkernel PROPERTIES LINKER_LANGUAGE C)
        set(dspkernellib dspkernel)
    else ()
        message("building dspkernel from dspkernellib")
        find_library(dspkernellib dspkernel HINTS ${PROJ_DIR}/pre_built)
        add_library(dspkernel STATIC IMPORTED)
        set_property(TARGET dspkernel PROPERTY IMPORTED_LOCATION ${dspkernellib})
        #include_directories("${CMAKE_CURRENT_LIST_DIR}/framework/rpc/include")
    endif()
endif()


set(cfgvar "CONFIG_EXECUTOR")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y executorEnabled)
if (executorEnabled)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/executor/src")
        message("building dsp executors from src")
        add_library(executor "")
        SET_TARGET_PROPERTIES(executor PROPERTIES LINKER_LANGUAGE C)
        set(dspexecutorlib executor)
        find_library(dspkernellib dspkernel HINTS ${PROJ_DIR}/pre_built)
        find_library(etaMathlib etaMath HINTS ${PROJ_DIR}/pre_built)
        find_library(csplib csp HINTS ${PROJ_DIR}/pre_built)
        target_link_libraries(executor LINK_PUBLIC ${csplib} ${dspkernellib} ${etaMathlib})
        #target_link_libraries(executor LINK_PUBLIC ${dspkernellib})
    else ()
        message("building dsp executors from dspexecutorlib")
        find_library(dspexecutorlib executor HINTS ${PROJ_DIR}/pre_built)
        add_library(executor STATIC IMPORTED)
        set_property(TARGET executor PROPERTY IMPORTED_LOCATION ${dspexecutorlib})
        #include_directories("${CMAKE_CURRENT_LIST_DIR}/framework/rpc/include")
    endif()
endif()




add_executable(${PROJECT_NAME}.elf "")
add_dependencies(${PROJECT_NAME}.elf preBuildCustomTarget)
add_dependencies(csp preBuildCustomTarget)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/framework framework)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/hw hw)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sensor_mgr sensor_mgr)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel kernel)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/executor executor)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/audio audio)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/eta_math eta_math)


find_package(Doxygen)
if ( DOXYGEN_FOUND )
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/doxy.cmake")
    #include(doxy)
   else (DOXYGEN_FOUND)
    message("Doxygen not found ")
endif (DOXYGEN_FOUND)


get_property(plib GLOBAL PROPERTY plib_list_property)
set_property(GLOBAL PROPERTY plib_list_property
        "${plib};${csplib};${dspkernellib};${etaMathlib};${dspexecutorlib};")
get_property(plib GLOBAL PROPERTY plib_list_property)
#message("library list="${plib})
target_link_libraries(${PROJECT_NAME}.elf ${plib})


#####Linking libraries
find_library(mathLibrary libmathlib.a HINTS ${CMAKE_CURRENT_LIST_DIR}/hw/csp/common/mathlib/lib)
find_library(nxpLibrary  libnxplib.a  HINTS ${CMAKE_CURRENT_LIST_DIR}/hw/csp/${socname}/nxp/lib)

message(mathlibloc = ${mathLibrary})
message(nxplibloc  = ${nxpLibrary})

target_link_libraries(${PROJECT_NAME}.elf
    ${mathLibrary}
    ${nxpLibrary}
)

target_link_libraries(csp INTERFACE ${nxpLibrary})
#####Linking libraries done

add_custom_command(TARGET ${PROJECT_NAME}.elf  PRE_LINK
    COMMENT "executing pre-link commands"
    #COMMAND source ../${SETUPSCRIPT}
    VERBATIM
)
# NRC : uncomment below commented lines  this when you have single config file integarted
#set(getsection "CONFIG_BIN_FILE_SECTION")
#CONFIGVAL(sectionname ${getsection})
#set(getsection ${sectionname})

#NRC: Comment below  line  this when you have single config file integarted
set(getsection ".binSection" )

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMENT "Creating output file"
    COMMAND mkdir -p bin
    COMMAND mv ${PROJECT_NAME}.elf bin
    COMMAND read_elf bin/${PROJECT_NAME}.elf -e -G -fhath -mIOMEM=8 -mYMEM=8 -mXMEM=8 -pPMEM=32 +g
    COMMAND ${DSP_TOOLS}/create_dsp_include_3532.pl ${PROJECT_NAME} ${getsection}
    COMMAND ${Python_EXECUTABLE} ${DSP_TOOLS}/genDSPBin.py ${PROJECT_BINARY_DIR}/bin/${PROJECT_NAME}.elf
    VERBATIM
)

find_file(linkerScript cf16.bcf HINT ${CMAKE_CURRENT_LIST_DIR}/hw/csp/${socname}/linker)

set(LINK_FILE " +Wbridge,-c${linkerScript}")
set(CMAKE_EXE_LINKER_FLAGS "${LINK_FILE}" CACHE STRING "" FORCE)

add_custom_target(menuconfig
        COMMAND
        ${CMAKE_COMMAND} -E env
        KCONFIG_CONFIG=${Kconfiglib_KCONFIG_CONFIG}
        srctree=${Kconfiglib_CONFIG_DIR}
        ${Python_EXECUTABLE} ${Kconfiglib}/menuconfig.py
        ${Kconfiglib_CONFIG_DIR}/${Kconfiglib_CONFIG_FILENAME}
        COMMAND
        COMMAND ${CMAKE_COMMAND} -E remove -f ${Kconfiglib_HEADER_OUTPUT}/dsp_config.h  
        COMMAND
	cmake ..
	)

add_custom_target(loadconfig
        COMMAND
        ${CMAKE_COMMAND} -E env
        KCONFIG_CONFIG=${Kconfiglib_KCONFIG_CONFIG}
        srctree=${Kconfiglib_CONFIG_DIR}
        ${Python_EXECUTABLE} ${Kconfiglib}/defconfig.py \${CONFIG}
        COMMAND
        cmake ..
        COMMAND ${CMAKE_COMMAND} -E remove -f ${Kconfiglib_HEADER_OUTPUT}/dsp_config.h
        )


SUBDIRLIST(SDIRS ${CMAKE_CURRENT_LIST_DIR}/app)
FOREACH(subdir ${SDIRS})
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property 
                    "${ignore_list};${subdir}/build/\./*")
ENDFOREACH()

    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property 
                    "${ignore_list};${CMAKE_CURRENT_LIST_DIR}/packaging")


if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/CPackConfig.cmake")
    #include(CPackConfig)
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/CPackConfig.cmake")
endif ()


message(Python_EXECUTABLE = ${Python_EXECUTABLE})
message(tool chain file = ${CMAKE_TOOLCHAIN_FILE})
message(cross compiler =  ${CMAKE_C_COMPILER})
#message("cross compilation is on =" ${CMAKE_CROSSCOMPILING})
#message("CFLAGS= ${CMAKE_C_FLAGS}")
#message("CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
message(linker scrpit file = ${linkerScript})

