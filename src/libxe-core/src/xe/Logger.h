
#pragma once

#include <fmt/core.h>
#include <fmt/color.h>

#define XE_LOG_INFO(msg, ...) fmt::print(msg, ##__VA_ARGS__)
#define XE_LOG_WARNING(msg, ...) fmt::print(fg(fmt::color::yellow), msg, ##__VA_ARGS__)
#define XE_LOG_ERROR(msg, ...) fmt::print(fg(fmt::color::crimson), msg, ##__VA_ARGS__)
