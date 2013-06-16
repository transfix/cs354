CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

#auto hardcoding the tex.ppm location
find_file(PROJECT4_TEXTURE_FILE tex.ppm ${CMAKE_CURRENT_SOURCE_DIR})
#file(TO_NATIVE_PATH ${PROJECT4_TEXTURE_FILE} PROJECT4_TEXTURE_FILE)
configure_file(texture.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/texture.h)

set(SOURCE_FILES 
  geometry.c light.c lowlevel.c raytrace.c readppm.c
)
set(INCLUDE_FILES 
  lowlevel.h raytrace.h readppm.h ${CMAKE_CURRENT_BINARY_DIR}/texture.h
)

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${INCLUDE_FILES})

if(WIN32)
  add_definitions(-D_WIN32)
endif(WIN32)

#find_package(OpenMP)
#if(OPENMP_FOUND)
#  add_definitions(-DUSE_OMP)
#  set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS})
#  set(CMAKE_LD_FLAGS ${CMAKE_LD_FLAGS} -lgomp)
#endif(OPENMP_FOUND)

include_directories(
  ../../inc
  ${CMAKE_CURRENT_BINARY_DIR}
  ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR}
)

add_executable(project4 ${SOURCE_FILES} ${INCLUDE_FILES})

SetupBoost(project4)

target_link_libraries(project4
  ${GLUT_LIBRARIES}
  ${OPENGL_LIBRARIES}
)

