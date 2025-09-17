
find_package(glad CONFIG REQUIRED)
find_package(KTX CONFIG REQUIRED)
find_package(DevIL CONFIG REQUIRED)
find_package(Vulkan 1.2 REQUIRED)

set (target xe-ktxc)
set (sources
        src/xe-ktxc-main.cpp
)

add_executable(${target} ${sources})
target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} PUBLIC XE.Core PUBLIC XE.GL PUBLIC XE.App PUBLIC glad::glad PUBLIC DevIL::IL PUBLIC KTX::ktx PUBLIC Vulkan::Vulkan)
