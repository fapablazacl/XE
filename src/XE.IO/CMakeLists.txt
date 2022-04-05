
set (target XE.IO)

set (headers
    "include/XE/IO/FileStream.h"
    "include/XE/IO/FileStreamSource.h"
    "include/XE/IO/MemoryStream.h"
    "include/XE/IO/Stream.h"
    "include/XE/IO/StreamSource.h"
)

set (sources
    ${headers}
    "src/FileStream.cpp"
	"src/FileStreamSource.cpp"
	"src/MemoryStream.cpp"
	"src/Stream.cpp"
	"src/StreamSource.cpp"
)

add_library(${target} ${sources})

target_precompile_headers(${target} PRIVATE ${headers})
target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core)
target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
