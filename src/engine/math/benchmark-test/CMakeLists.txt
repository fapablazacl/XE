
set (benchmark_test_target "XE.Math.BenchmarkTest")
set (benchmark_test_sources "math-benchmark-test.cpp")

add_executable(${benchmark_test_target} ${benchmark_test_sources})
target_link_libraries(${benchmark_test_target} XE.Core XE.Interface benchmark)

# NOTE: The test name must have the same name as the command name
add_test(NAME ${benchmark_test_target} COMMAND ${benchmark_test_target})
