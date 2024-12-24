set (test_target "XE.Graphics.GL.UnitTest")

set (test_sources 
    "main.cpp"
    "XEGraphicsGLUnitTest.cpp"
)

add_executable(${test_target} ${test_sources})
target_link_libraries(${test_target} GTest::gtest XE.Graphics.GL XE.Interface)

# NOTE: The test name must have the same name as the command name
add_test(NAME ${test_target} COMMAND ${test_target})
