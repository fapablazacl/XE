
set (target "XE.Sandbox")
set (src "Main.cpp")

source_group ("\\" FILES ${src})

add_executable (${target} ${src})

target_link_libraries (${target} XE.Core glfw glad-gl4)
