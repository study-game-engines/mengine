#pragma once

#ifndef MENGINE_MAX_PATH
#define MENGINE_MAX_PATH 260
#endif

#ifndef MENGINE_UNUSED
#define MENGINE_UNUSED(X) ((void)X)
#endif

#ifdef __GNUC__
#define MENGINE_CODE_FILE __FILE__
#define MENGINE_CODE_FUNCTION __PRETTY_FUNCTION__
#define MENGINE_CODE_LINE __LINE__
#else
#define MENGINE_CODE_FILE __FILE__
#define MENGINE_CODE_FUNCTION __FUNCTION__
#define MENGINE_CODE_LINE __LINE__
#endif

#ifndef NDEBUG
#define MENGINE_DEBUG
#endif

#ifdef MENGINE_DEBUG
#define MENGINE_DEBUG_ATTRIBUTE(X, Y) (X)
#else
#define MENGINE_DEBUG_ATTRIBUTE(X, Y) (Y)
#endif

#if defined(WIN32)
#   define MENGINE_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if defined(TARGET_OS_OSX)
#       define MENGINE_PLATFORM_OSX
#   endif
#   if defined(TARGET_OS_IPHONE)
#       define MENGINE_PLATFORM_IOS
#   endif
#elif defined(__ANDROID__)
#   define MENGINE_PLATFORM_ANDROID
#else
#   error "undefine platform"
#endif

#if defined(__MINGW32__)
#   define MENGINE_TOOLCHAIN_MINGW
#elif defined(_MSC_VER)
#   define MENGINE_TOOLCHAIN_MSVC
#endif

#if defined(MENGINE_PLATFORM_IOS) || defined(MENGINE_PLATFORM_ANDROID)
#   define MENGINE_PLATFORM_MOBILE
#endif

#ifdef MENGINE_DEBUG
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_WINDOWS_DEBUG
#   endif
#endif
