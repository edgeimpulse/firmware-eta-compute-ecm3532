SUBDIRLIST(SDIRS ${CMAKE_CURRENT_LIST_DIR})
FOREACH(subdir ${SDIRS})
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property 
                    "${ignore_list};${subdir}/build/\./*")
ENDFOREACH()
get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
set_property(GLOBAL PROPERTY ignore_list_property "${ignore_list};executor_tiny_eye")
