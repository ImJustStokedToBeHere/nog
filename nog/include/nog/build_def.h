
#pragma once
// #include <float.h>

// #define NOMINMAX

#if defined(__has_include)
/*#if __has_include(<vcruntime.h>)
    #include <vcruntime.h>
#endif*/
#else
/*#if defined(_VCRUNTIME_H)
    #include <vcruntime.h>
#endif*/
#endif

#if !defined(API_DEF)
    #define API_DEF

    #if defined(EXPORT)
        #if defined(_MSC_VER) && !defined(__clang__)
            #define API __declspec(dllexport)
        #elif defined(__GNUC__) || defined(__clang__)
            #define API __attribute__((visibility("default")))
        #endif
    #elif defined(IMPORT)
        #if defined(_MSC_VER)
            #define API __declspec(dllimport)
        #else
            #define API
        #endif
    #else
        #define API
    #endif
#endif

#if defined(RELEASE)
    #define INLINE inline
#else
    #define INLINE
#endif

#if !defined(CPP_COMPILER)

    #define CPP_COMPILER_MSVC 1
    #define CPP_COMPILER_CLANG 2
    #define CPP_COMPILER_GCC 4

    #if (defined(_MSC_VER) && !defined(__clang__))
        #define CPP_COMPILER_NAME "msvc"
        #define CPP_COMPILER CPP_COMPILER_MSVC
    #elif defined(__clang__)
        #define CPP_COMPILER_NAME "clang"
        #define CPP_COMPILER CPP_COMPILER_CLANG
    #elif defined(__GNUC__)
        #define CPP_COMPILER_NAME "gcc"
        #define CPP_COMPILER CPP_COMPILER_GCC
    #else
        #define CPP_COMPILER_NAME "unknown"
        #define CPP_COMPILER "unknown"
    #endif

#endif

#if !defined(CPP_LANG_VER)
    /*#define CPP_LANG_VER_NUM 0
    #define CPP_VER_SUPPORT 0
    #define CPP_LANG_VER 0*/
    #undef CPP_LANG_VER_NUM
    #undef CPP_VER_SUPPORT
    #undef CPP_LANG_VER

    #define CPP_LANG_VER_PRE98 (1 << 0)
    #define CPP_LANG_VER_98 (1 << 1) //| CPP_LANG_VER_PRE98
    #define CPP_LANG_VER_11 (1 << 2) //| CPP_LANG_VER_98
    #define CPP_LANG_VER_14 (1 << 3) //| CPP_LANG_VER_11
    #define CPP_LANG_VER_17 (1 << 5) //| CPP_LANG_VER_14
    #define CPP_LANG_VER_20 (1 << 6) //| CPP_LANG_VER_17
    #define CPP_LANG_VER_23 (1 << 7) //| CPP_LANG_VER_20

    #if !(CPP_COMPILER == CPP_COMPILER_MSVC)
        #if (CPP_LANG_VER == CPP_LANG_VER_PRE98 || __cplusplus == 1)
            // cpp pre 98
            #define CPP_LANG_VER_NUM 1
            #define CPP_VER_SUPPORT CPP_LANG_VER_PRE98
            #define CPP_LANG_VER CPP_LANG_VER_PRE98

        #elif (CPP_LANG_VER == CPP_LANG_VER_98 || __cplusplus == 199711L)
            // cpp 98
            #define CPP_LANG_VER_NUM CPP_LANG_VER_98
            #define CPP_VER_SUPPORT CPP_LANG_VER_PRE98 | CPP_LANG_VER_98
            #define CPP_LANG_VER CPP_LANG_VER_98

        #elif (CPP_LANG_VER == CPP_LANG_VER_11 || __cplusplus == 201103L)
            // cpp 11
            #define CPP_LANG_VER_NUM CPP_LANG_VER_11
            #define CPP_VER_SUPPORT CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11
            #define CPP_LANG_VER CPP_LANG_VER_11

        #elif (CPP_LANG_VER == CPP_LANG_VER_14 || __cplusplus == 201402L)
            // cpp 14
            #define CPP_LANG_VER_NUM CPP_LANG_VER_14
            #define CPP_VER_SUPPORT CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11 | CPP_LANG_VER_14
            #define CPP_LANG_VER CPP_LANG_VER_14

        #elif (CPP_LANG_VER == CPP_LANG_VER_17 || __cplusplus == 201703L)
            // cpp 17
            #define CPP_LANG_VER_NUM CPP_LANG_VER_17
            #define CPP_VER_SUPPORT \
                CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11 | CPP_LANG_VER_14 | CPP_LANG_VER_17
            #define CPP_LANG_VER CPP_LANG_VER_17

        #elif (CPP_LANG_VER == CPP_LANG_VER_20 || __cplusplus == 202002L)
            // cpp 20
            #define CPP_LANG_VER_NUM CPP_LANG_VER_20
            #define CPP_VER_SUPPORT \
                CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11 | CPP_LANG_VER_14 | CPP_LANG_VER_17 \
                    | CPP_LANG_VER_20
            #define CPP_LANG_VER CPP_LANG_VER_20

        #elif (CPP_LANG_VER == CPP_LANG_VER_23 || __cplusplus > 202002L)
            // cpp 23
            #define CPP_LANG_VER_NUM CPP_LANG_VER_23
            #define CPP_VER_SUPPORT \
                CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11 | CPP_LANG_VER_14 | CPP_LANG_VER_17 \
                    | CPP_LANG_VER_20 | CPP_LANG_VER_23
            #define CPP_LANG_VER CPP_LANG_VER_23

        #else
            #define CPP_LANG_VER_NUM 0
            #define CPP_VER_SUPPORT 0
            #define CPP_LANG_VER 0
        #endif
    #else
        // try to get the correct c++ version
        #if defined(__has_include)
            #if __has_include(<yvals_core.h>)
                #include <yvals_core.h>
            #elif __has_include(<yvals.h>)
                #include <yvals.h>
            #endif
        #endif

        #if (CPP_LANG_VER == CPP_LANG_VER_23 || __cplusplus > 202002L || _HAS_CXX23)
            #define CPP_LANG_VER_NUM CPP_LANG_VER_23
            #define CPP_VER_SUPPORT \
                CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11 | CPP_LANG_VER_14 | CPP_LANG_VER_17 \
                    | CPP_LANG_VER_20 | CPP_LANG_VER_23
            #define CPP_LANG_VER CPP_LANG_VER_23
        #elif (CPP_LANG_VER == CPP_LANG_VER_20 || __cplusplus == 202002L || _HAS_CXX20)
            #define CPP_LANG_VER_NUM CPP_LANG_VER_20
            #define CPP_VER_SUPPORT \
                CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11 | CPP_LANG_VER_14 | CPP_LANG_VER_17 \
                    | CPP_LANG_VER_20
            #define CPP_LANG_VER CPP_LANG_VER_20
        #elif (CPP_LANG_VER == CPP_LANG_VER_17 || __cplusplus == 201703L || _HAS_CXX17)
            #define CPP_LANG_VER_NUM CPP_LANG_VER_17
            #define CPP_VER_SUPPORT \
                CPP_LANG_VER_PRE98 | CPP_LANG_VER_98 | CPP_LANG_VER_11 | CPP_LANG_VER_14 | CPP_LANG_VER_17
            #define CPP_LANG_VER CPP_LANG_VER_17
        #else
            #define CPP_LANG_VER_NUM 0
            #define CPP_VER_SUPPORT 0
            #define CPP_LANG_VER 0
        #endif
    #endif
#endif

// specific feature definitions
#define USE_FORMAT_LIBRARY ((CPP_VER_SUPPORT & CPP_LANG_VER_20) > 0)
