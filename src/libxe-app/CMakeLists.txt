
find_package(glfw3 REQUIRED)

set (target XE.App)
set (sources include/xe/app/Platform.h src/Platform.cpp)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC include/)
target_link_libraries(${target} PUBLIC XE.Core PUBLIC XE.GL PRIVATE glfw)
