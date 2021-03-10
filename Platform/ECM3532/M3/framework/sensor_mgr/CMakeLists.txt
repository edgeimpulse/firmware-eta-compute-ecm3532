GETCONFIG(cfgvaren "SENSOR_MANAGER")
string(COMPARE EQUAL "${cfgvaren}" y _cmp)

if (_cmp)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
        add_subdirectory(src)
    endif ()
    add_subdirectory(include)
endif()

GETCONFIG(varen "SM_SRC_PACKAGE")
string(COMPARE EQUAL "${varen}" y _cmp1)
if (NOT _cmp1)
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property
    "${ignore_list};sensor_mgr/src")
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
endif()
