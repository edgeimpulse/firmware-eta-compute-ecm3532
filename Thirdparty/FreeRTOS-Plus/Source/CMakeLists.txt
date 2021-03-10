GETCONFIG(varen "CONFIG_CLI")
string(COMPARE EQUAL "${varen}" y _cmp)
if (_cmp)
    add_subdirectory(FreeRTOS-Plus-CLI)
endif()


