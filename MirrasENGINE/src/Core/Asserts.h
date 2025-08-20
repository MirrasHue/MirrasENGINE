#pragma once

#ifdef DEBUG_BUILD

    #if defined(__GNUC__) || defined(__clang__)
        #define breakpoint() __builtin_trap()

    #elif defined(_MSC_VER)
        #define breakpoint() __debugbreak()

    #else
        #define breakpoint() asm("int3")
    #endif
    
#else
    #define breakpoint() std::exit(EXIT_FAILURE)
#endif

#ifndef DISTRIBUTION_BUILD
    #include "Core/Log.h"

    #define MIRR_ASSERT(expr, ...)                                    \
        do{                                                           \
            if(!(expr))                                               \
            {                                                         \
                LOG_ERROR("Assertion Failed: {}\nFile: {}, line {}",  \
                    #expr, __FILE__, __LINE__);                       \
                LOG_ERROR(__VA_ARGS__);                               \
                breakpoint();                                         \
            }                                                         \
        } while(false)

#else
    #define MIRR_ASSERT(expr, ...)
#endif

// In some cases we still need the runtime check in distribution builds for extra safety
// (mainly to protect against misuse by some user, or resources from the graphics API that fail to be created)
#define MIRR_ASSERT_RETURN(expr, ...)    \
    do{                                  \
        MIRR_ASSERT(expr, __VA_ARGS__);  \
        if(!(expr))                      \
            return;                      \
    } while(false)

#define MIRR_ASSERT_RETURN_VALUE(expr, value, ...)  \
    do{                                             \
        MIRR_ASSERT(expr, __VA_ARGS__);             \
        if(!(expr))                                 \
            return value;                           \
    } while(false)
