
#ifndef __XE_PREDEF_HPP__
#define __XE_PREDEF_HPP__

#include <cstdint>
#include <cstddef>

//! API export/import definition.
//! Defined when we are using a DLL/Shared Object/DYLIB.
#define XE_API

//! detect target OS 
#if defined(_WINDOWS)
#define XE_OS_WINDOWS
#elif defined(__linux__)
#define XE_OS_LINUX
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
#define XE_OS_IOS_SIMULATOR
#elif TARGET_OS_IPHONE
#define XE_OS_IOS
#elif TARGET_OS_MAC
#define XE_OS_MACOS
#else
#define XE_OS_UNKNOWN
#endif
#elif __ANDROID__
#define XE_OS_ANDROID
#elif __FreeBSD__
#define XE_OS_FREEBSD
#else
#define XE_OS_UNKNOWN
#endif

#if defined(XE_OS_UNKNOWN)
#pragma message "Unknown OS isn't Supported. You are on your own.'"
#endif

#endif
