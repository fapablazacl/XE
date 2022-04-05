
set (target "XE.Graphics.GL")

set (headers
	"include/XE/Graphics/GL.h"
	"include/XE/Graphics/GL/GraphicsDeviceGL.h"
	"include/XE/Graphics/GL/TextureBaseGL.h"
	"include/XE/Graphics/GL/Texture2DGL.h"
	"include/XE/Graphics/GL/Texture2DArrayGL.h"
	"include/XE/Graphics/GL/Texture3DGL.h"
	"include/XE/Graphics/GL/ShaderGL.h"
	"include/XE/Graphics/GL/ProgramGL.h"
	"include/XE/Graphics/GL/ProgramGL.h"
	"include/XE/Graphics/GL/BufferGL.h"
	"include/XE/Graphics/GL/SubsetGL.h"
	"include/XE/Graphics/GL/Conversion.h"
	"include/XE/Graphics/GL/Util.h"
)

set (sources
	${headers}
	"src/GraphicsDeviceGL.cpp"
	"src/TextureBaseGL.cpp"
	"src/Texture2DGL.cpp"
	"src/Texture2DArrayGL.cpp"
	"src/Texture3DGL.cpp"
	"src/ShaderGL.cpp"
	"src/ProgramGL.cpp"
	"src/ProgramGL.cpp"
	"src/BufferGL.cpp"
	"src/SubsetGL.cpp"
	"src/Conversion.cpp"
	"src/Util.cpp"	
) 

add_library (${target} ${sources})

target_precompile_headers (${target} PRIVATE ${headers})
target_include_directories (${target} PUBLIC "include/")
target_link_libraries (${target} 
	XE.Core 
	XE.Graphics 
	glad-gl4
)

if (XE_UNIT_TEST)
    set (test_target "XE.Graphics.GL.UnitTest")

    set (test_sources 
        "unit-test/XEGraphicsGLUnitTest.cpp"
    )

    add_executable(${test_target} ${test_sources})
    target_link_libraries(${test_target} Catch2::Catch2WithMain XE.Graphics.GL)

    # NOTE: The test name must have the same name as the command name
    add_test(NAME ${test_target} COMMAND ${test_target})
endif()
