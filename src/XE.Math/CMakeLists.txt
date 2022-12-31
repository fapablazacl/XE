
set (target XE.Math)

set (headers

)

set (sources
    ${headers}
)

add_library(${target} INTERFACE)

target_include_directories(${target} INTERFACE "include")


if (XE_UNIT_TEST)

endif()
