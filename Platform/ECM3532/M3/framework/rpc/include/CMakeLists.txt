if (EXISTS "${PROJ_DIR}/framework/rpc/src")
    target_include_directories(rpc PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
    target_include_directories(rpc PUBLIC "${PROJ_DIR}/../Common/framework/inc")
endif ()
