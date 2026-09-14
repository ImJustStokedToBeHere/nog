
#pragma once

#undef ENV64
#undef ENV32

#undef OS
#undef OS_NAME
#undef ENV_NAME
#undef BUILD_ENV
#undef BUILD_ENV_NAME

#if !defined(ENV64) && !defined(ENV32)
// Check windows
    #if defined(_MSC_VER)
        #define BUILD_ENV "msvc"
        #if defined(_WIN64)
            #define ENV64
        #else
            #define ENV32
        #endif
    #endif

    // Check GCC
    #if defined(__GNUC__) || defined(__clang__)
        #if defined(_LP64) || defined(__x86_64__)
            #define ENV64
        #else
            #define ENV32
        #endif
    #endif

    #if defined(ENV64)
        #define ENV_NAME "x64"
    #elif defined(ENV32)
        #defined ENV_NAME "x32"
    #endif
#endif

#define CLANG_BUILD_ENV 1
#define GNU_BUILD_ENV 2
#define MSVC_BUILD_ENV 3

#if !defined(BUILD_ENV)
    #if defined(__clang__)
        #define BUILD_ENV CLANG_BUILD_ENV
        #define BUILD_ENV_NAME "clang"
    #elif defined(__GNUC__)
        #define BUILD_ENV GNU_BUILD_ENV
        #define BUILD_ENV_NAME "gnu"
    #elif defined(_MSC_VER)
        #define BUILD_ENV MSVC_BUILD_ENV
        #define BUILD_ENV_NAME "msvc"
    #endif
#endif

#define WINDOWS_OS 1
#define UNIX_OS 2
#define APPLE_OS 3
#define LINUX_OS 4
#define BSD_OS 5
#define IPHONE_SIMULATOR_OS 6
#define ANDROID_SIMULATOR_OS 7
#define OSX_OS 8
#define IOS_OS 9
#define ANDROID_OS 10
#define WASM_OS 11

#if !defined(OS)
    #if defined(_WIN32)
        #define OS WINDOWS_OS
        #define OS_NAME "WINDOWS"
    #elif defined(__linux__)
        #define OS LINUX_OS
        #define OS_NAME "LINUX"
    #elif defined(unix) || defined(__unix) || defined(__unix__)
        #define OS UNIX_OS
        #define OS_NAME "UNIX"
    #elif defined(__APPLE__) || defined(__MACH__)
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR == 1
            #define OS IPHONE_SIMULATOR_OS
            #define OS_NAME "IOS-simulator"
        #elif TARGET_OS_IPHONE == 1
            #define OS IOS_OS
            #define OS_NAME "IOS"
        #elif TARGET_OS_MAC
            #define OS MAC_OS
            #define OS_NAME "OSX"
        #else
            #define OS APPLE_OS
            #define OS_NAME "APPLE/MAC"
        #endif
    #elif defined(__android__)
        #define OS ANDROID_OS
        #define OS_NAME "ANDROID"
    #elif defined(__FreeBSD__)
        #define OS BSD_OS
        #define OS_NAME "BSD"
    #elif defined(WASM)
        #define OS WASM_OS
        #define OS_NAME "wasm"
    #endif
#endif
