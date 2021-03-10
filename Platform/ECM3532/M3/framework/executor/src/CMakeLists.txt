target_include_directories(executor PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
file (GLOB execsrc "${CMAKE_CURRENT_LIST_DIR}/*.c")
foreach (execf ${execsrc})
    target_sources(executor PRIVATE ${execf}
		     	  )
endforeach()


