CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCE_FILES 
  drawing.c subdiv.c
)
set(INCLUDE_FILES 
  drawing.h
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
  ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR}
)

add_executable(project3 ${SOURCE_FILES} ${INCLUDE_FILES})

SetupBoost(project3)

target_link_libraries(project3
  ${GLUT_LIBRARIES}
  ${OPENGL_LIBRARIES}
)

