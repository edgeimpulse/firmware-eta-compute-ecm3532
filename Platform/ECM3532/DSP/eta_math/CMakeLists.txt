set(var "CONFIG_ETA_MATH")
CONFIGVAL(varen ${var})
message("----Eta Math library")
string(COMPARE EQUAL "${varen}" y _en)
if (_en)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
    file (GLOB aud "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
    foreach (etaMathf ${aud})
        message("----adding file ${etaMathf}")
        target_sources(etaMath PRIVATE "${etaMathf}")
    endforeach()

    target_include_directories(etaMath PUBLIC
    	"${CMAKE_CURRENT_SOURCE_DIR}/inc" "${CMAKE_CURRENT_LIST_DIR}/../hw/csp/common/mathlib/inc" "${CMAKE_CURRENT_LIST_DIR}/../hw/csp/ecm3532/nxp/inc")
endif()
endif()

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/inc")

## SRC packaging
GETCONFIG(etavaren "ETA_MATH_DSP_SRC_PACKAGE")
string(COMPARE EQUAL "${etavaren}" y eta_cmp)
if (NOT eta_cmp)
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property
    "${ignore_list};eta_math/src")
endif()
