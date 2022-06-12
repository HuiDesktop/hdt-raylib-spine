add_library(hdt-raylib-spine SHARED spine-raylib.c spine-raylib.h spine-export.h spine-hittest.c spine-hittest.h)
target_include_directories(hdt-raylib-spine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# link spine library
target_include_directories(hdt-raylib-spine PUBLIC ${SPINE_INCLUDE_DIR})
target_link_libraries(hdt-raylib-spine PUBLIC spine-c)

# temporary link raylib from built dist
target_include_directories(hdt-raylib-spine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../raylib/include")
target_link_libraries(hdt-raylib-spine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/lib/raylibdll.lib)

# link winmm (timeBeginPeriod timeEndPeriod)
target_link_libraries (hdt-raylib-spine PUBLIC winmm)

message(${CMAKE_CURRENT_SOURCE_DIR}/../raylib/include)