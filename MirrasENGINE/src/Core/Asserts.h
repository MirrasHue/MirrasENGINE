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

    #define MIRR_ASSERT_CORE(expr, msg)                                         \
        do{                                                                     \
            if(!(expr))                                                         \
            {                                                                   \
                ENGINE_LOG_ERROR("Assertion Failed: {}\n{}\nFile: {}, line {}", \
                    #expr, msg, __FILE__, __LINE__);                            \
                breakpoint();                                                   \
            }                                                                   \
        } while(false)

    #define MIRR_ASSERT(expr, msg)                                       \
        do{                                                              \
            if(!(expr))                                                  \
            {                                                            \
                LOG_ERROR("Assertion Failed: {}\n{}\nFile: {}, line {}", \
                    #expr, msg, __FILE__, __LINE__);                     \
                breakpoint();                                            \
            }                                                            \
        } while(false)

#else
    #define MIRR_ASSERT_CORE(expr, msg)
    #define MIRR_ASSERT(expr, msg)
#endif
