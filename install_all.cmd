@echo off
setlocal enabledelayedexpansion

set PROFILE_ARG=
if not "%~1"=="" (
    set PROFILE_ARG=--profile="%~1"
    echo Using profile: %~1
) else (
    echo Using default profile.
)

set BUILD_TYPES=Debug Release RelWithDebInfo

for %%b in (%BUILD_TYPES%) do (
    echo.
    echo ========================================================
    echo Installing build type: %%b
    echo ========================================================
    conan install . --build=missing !PROFILE_ARG! --settings=build_type=%%b
    if !errorlevel! neq 0 (
        echo Error installing %%b
        exit /b !errorlevel!
    )
)

echo.
echo All build types installed successfully.
