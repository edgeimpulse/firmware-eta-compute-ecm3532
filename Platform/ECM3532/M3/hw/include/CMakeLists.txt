set(getsoc "CONFIG_SOC")
CONFIGVAL(soc ${getsoc})

target_include_directories(${PROJECT_NAME}.elf PUBLIC
                            "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${PROJECT_NAME}.elf PUBLIC
                            "${CMAKE_CURRENT_LIST_DIR}/${soc}")
