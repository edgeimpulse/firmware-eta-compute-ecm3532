if (EXISTS "${PROJ_DIR}/framework/sensor_mgr/src")
    target_include_directories(sm PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
    target_include_directories(sm PUBLIC "${PROJ_DIR}/../Common/sensor_mgr/inc")
endif ()
