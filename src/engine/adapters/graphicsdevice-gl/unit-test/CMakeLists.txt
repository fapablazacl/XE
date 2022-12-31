set (test_target "XE.Graphics.GL.UnitTest")

set (test_sources 
    "unit-test/XEGraphicsGLUnitTest.cpp"
)

add_executable(${test_target} ${test_sources})
target_link_libraries(${test_target} Catch2::Catch2WithMain XE.Graphics.GL)

# NOTE: The test name must have the same name as the command name
add_test(NAME ${test_target} COMMAND ${test_target})