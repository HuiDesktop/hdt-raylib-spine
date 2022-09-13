add_library(hdt-raylib-spine SHARED spine-hittest.h spine-raylib.h spine-event.h ${SPINE_VERSION}/spine-event.c ${SPINE_VERSION}/spine-raylib.c ${SPINE_VERSION}/spine-hittest.c ${SPINE_VERSION}/spine-export.h)
target_include_directories(hdt-raylib-spine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(hdt-raylib-spine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${SPINE_VERSION})

# link spine library
target_include_directories(hdt-raylib-spine PUBLIC ${SPINE_INCLUDE_DIR})
target_link_libraries(hdt-raylib-spine PUBLIC spine-c)

# temporary link raylib from built dist
target_include_directories(hdt-raylib-spine PUBLIC ${RAYLIB_INCLUDE_DIR})
target_link_libraries(hdt-raylib-spine PUBLIC raylib)

# link winmm (timeBeginPeriod timeEndPeriod)
target_link_libraries (hdt-raylib-spine PUBLIC winmm)
