set(getsoc "CONFIG_SOC")
CONFIGVAL(soc ${getsoc})
add_subdirectory(${soc})
