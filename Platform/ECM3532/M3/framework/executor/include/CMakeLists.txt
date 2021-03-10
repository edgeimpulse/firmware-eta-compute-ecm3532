if (EXISTS "${PROJ_DIR}/framework/executor/src")
    target_include_directories(executor PUBLIC "${CMAKE_CURRENT_LIST_DIR}/pub"
    "${PROJ_DIR}/NN_kernels/include"
	)
target_include_directories(executor PRIVATE "${CMAKE_CURRENT_LIST_DIR}/priv")
target_include_directories(executor PRIVATE "${PROJ_DIR}/../Common/executor/inc")
target_include_directories(${PROJECT_NAME}.elf PUBLIC "${PROJ_DIR}/../Common/executor/inc")

else ()
    target_include_directories(${PROJECT_NAME}.elf BEFORE PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/pub"
        "${PROJ_DIR}/NN_kernels/include"
	)
endif ()
