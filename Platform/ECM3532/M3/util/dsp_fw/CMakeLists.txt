GETCONFIG(rpcen "RPC")
string(COMPARE EQUAL "${rpcen}" y _rpcen)
if (_rpcen)
  CONFIGVAL(dspfw "CONFIG_PRE_BUILD_DSP_FW")
  set_property(GLOBAL PROPERTY dspfw_property
    "${CMAKE_CURRENT_LIST_DIR}/${dspfw}")
endif()
