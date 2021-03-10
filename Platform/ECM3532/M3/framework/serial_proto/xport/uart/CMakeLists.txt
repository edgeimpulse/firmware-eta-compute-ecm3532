target_include_directories(sproto PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
file (GLOB ser "${CMAKE_CURRENT_LIST_DIR}/*.c")
foreach (serf ${ser})
    target_sources(sproto PRIVATE ${serf}
			  )
endforeach()
