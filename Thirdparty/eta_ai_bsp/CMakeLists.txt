
file(GLOB AI_BSP "${CMAKE_CURRENT_LIST_DIR}/src/*.c")

foreach (ai_bsp ${AI_BSP})
    target_sources(eta_ai_bsp PUBLIC ${ai_bsp})
endforeach()

#target_sources(eta_ai_bsp PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src/eta_bsp.c")

target_include_directories(eta_ai_bsp PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/inc")