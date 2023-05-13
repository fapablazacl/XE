# "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
# "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

param (
    [Parameter(Mandatory=$true)][string]$mode,
    [Parameter(Mandatory=$true)][string]$toolchain,
    [Parameter(Mandatory=$true)][string]$buildType
)

$CMAKE_BUILD_TYPE = ''
$BUILD_TYPE = $buildType
if ($buildType -eq 'debug') {
    $CMAKE_BUILD_TYPE = 'Debug'

} elseif ($buildType -eq 'release') {
    $CMAKE_BUILD_TYPE = 'Release'

} else {
    throw "Unknown build type $buildType"
}

$CMAKE_GENERATOR = ''
if ($toolchain -eq 'vc') {
    $CMAKE_GENERATOR = "NMake Makefiles"
} else {
    throw "Unknown toolchain $toolchain"
}

$SOURCE_PREFIX="$pwd\dependencies"
$BUILD_PREFIX="$pwd\dependencies\.build"
$INSTALL_PREFIX="$pwd\local"

# variables for dependencies
$CATCH2_BUILD_DIR="$BUILD_PREFIX\$TOOLCHAIN\Catch2\$BUILD_TYPE"
$CATCH2_SOURCE_DIR="$SOURCE_PREFIX\Catch2"
$CATCH2_INSTALL_PREFIX="$INSTALL_PREFIX\$TOOLCHAIN\Catch2"

$GLFW3_BUILD_DIR="$BUILD_PREFIX\$TOOLCHAIN\glfw\$BUILD_TYPE"
$GLFW3_SOURCE_DIR="$SOURCE_PREFIX\glfw"
$GLFW3_INSTALL_PREFIX="$INSTALL_PREFIX\$TOOLCHAIN\glfw"

# Setup dependencies
if ($mode -eq 'installdeps') {
    cmake -B"$CATCH2_BUILD_DIR" -S"$CATCH2_SOURCE_DIR" -G "$CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$CATCH2_INSTALL_PREFIX" -DCMAKE_DEBUG_POSTFIX="d" -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF
    cmake --build "$CATCH2_BUILD_DIR"
    cmake --install "$CATCH2_BUILD_DIR"

    # build glfw
    cmake -B"$GLFW3_BUILD_DIR" -S"$GLFW3_SOURCE_DIR" -G "$CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$GLFW3_INSTALL_PREFIX" -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DCMAKE_DEBUG_POSTFIX="d"
    cmake --build "$GLFW3_BUILD_DIR"
    cmake --install "$GLFW3_BUILD_DIR"
} elseif ($mode -eq 'configure') {
    cmake -B "build\$TOOLCHAIN\$BUILD_TYPE" -G "$CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" -DXE_PLUGIN_GL=ON -DXE_PLUGIN_ES2=ON -DXE_PLUGIN_GL_GLFW=ON -DXE_UNIT_TEST=ON -Dglfw3_DIR="$GLFW3_INSTALL_PREFIX\lib\cmake\glfw3" -DCatch2_DIR="$CATCH2_INSTALL_PREFIX\lib\cmake\Catch2"
} else {
    throw "Unknown operation mode $mode."
}
