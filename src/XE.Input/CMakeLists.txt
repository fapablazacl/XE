
set (target XE.Input)

set (headers

)

set (sources
    ${headers}

)

add_library(${target} ${sources})

target_precompile_headers(${target} PRIVATE ${headers})
target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core XE.Math)

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${target} PRIVATE -Wall -Wextra)

    if (XE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE -Werror)
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${target} PRIVATE /Wall)

    if (XE_WARNINGS_AS_ERRORS)
        target_compile_options(${target} PRIVATE /WX)
    endif()
endif()
