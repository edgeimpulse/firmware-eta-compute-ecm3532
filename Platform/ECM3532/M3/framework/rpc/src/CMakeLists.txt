file (GLOB rsrc "${CMAKE_CURRENT_LIST_DIR}/*.c")
foreach (rpcf ${rsrc})
    target_sources(rpc PRIVATE ${rpcf})
endforeach()

target_include_directories(rpc PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
