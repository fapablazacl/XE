
set (target XE.IO)

set (sources
    "include/XE/IO/FileStream.hpp"
	"include/XE/IO/FileStreamSource.hpp"
    "include/XE/IO/MemoryStream.hpp"
    "include/XE/IO/Stream.hpp"
    "include/XE/IO/StreamSource.hpp"

    "src/FileStream.cpp"
	"src/FileStreamSource.cpp"
	"src/MemoryStream.cpp"
	"src/Stream.cpp"
	"src/StreamSource.cpp"
)

add_library(${target} ${sources})

target_include_directories(${target} PUBLIC "include/")
target_link_libraries(${target} XE.Core)
