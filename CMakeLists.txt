cmake_minimum_required(VERSION 2.6)

# Enable debug symbols by default, must be done before project() statement
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)

set(EXECUTABLE_NAME "tracer")
project(${EXECUTABLE_NAME})

set(OP "-fopenmp -O3 -ffast-math -march=native -mfpmath=sse -msse3 -mtune=native")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OP}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OP}")

add_definitions ( -DEXEC_NAME="${EXECUTABLE_NAME}" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

file(GLOB_RECURSE HEADERS "src/**.h")
file(GLOB_RECURSE HEADERS "include/**.h")
file(GLOB_RECURSE SOURCES_CPP "src/**.cpp")
file(GLOB_RECURSE SOURCES_C "src/**.c")
file(GLOB_RECURSE RESOURCES "resources/")

set( SOURCES ${SOURCES_CPP} ${SOURCES_C} )
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    src
    include
)

# fix for insane windows compile warnings.
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

#add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    "resources"
#    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/resources/")
#    #"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
#    #"${PROJECT_BINARY_DIR}" )
#    #"${RUNTIME_OUTPUT_DIRECTORY}" )
#    #"Debug" )

if(UNIX AND NOT APPLE)
    set(LIBS ${LIBS} ${CMAKE_DL_LIBS})
endif()

target_link_libraries(${EXECUTABLE_NAME} ${LIBS})

