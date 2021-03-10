GETCONFIG(varen "CONFIG_SPI_FLASH")
string(COMPARE EQUAL "${varen}" y _cmp)
if (_cmp)
    target_sources(iodev PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/common/spi_flash.c")
    target_include_directories(iodev PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/common" "${CMAKE_CURRENT_LIST_DIR}/maxim")
endif()
