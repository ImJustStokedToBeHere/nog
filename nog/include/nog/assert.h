#pragma once
#include <cstdlib>
#include <iostream>
// specific feature definitions
#ifndef NDEBUG
    #define NOG_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                std::cerr << "Assertion failed: (" << #condition << "), " \
                          << "file " << __FILE__ << ", line " << __LINE__ << ".\n" \
                          << "Message: " << message << std::endl; \
                std::abort(); \
            } \
        } while (false)
#else
    #define NOG_ASSERT(condition, message) ((void)0)
#endif
