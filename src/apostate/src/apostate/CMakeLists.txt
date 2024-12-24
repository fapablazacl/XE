set (target apostate)
add_executable(${target} "src/main.cpp")
target_link_libraries(${target} apostate-core)
