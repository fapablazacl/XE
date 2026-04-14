set (target "xe-glaze")

set (sources
    "src/Main.cpp"
)

add_executable(${target} ${sources})
target_link_libraries(${target}
    PRIVATE xe::glaze-facade
    PRIVATE xe::interface
)
