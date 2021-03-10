if (EXISTS "${PROJ_DIR}/framework/serial_proto/src")
    target_include_directories(sproto PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
else ()
    target_include_directories(${PROJECT_NAME}.elf BEFORE PUBLIC
      "${CMAKE_CURRENT_LIST_DIR}")
endif ()
