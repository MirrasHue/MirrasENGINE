#pragma once

// Don't include threading libraries here, use RUN_UPDATE_THREAD to add them accordingly
#ifdef RUN_UPDATE_THREAD
    #define ASYNC_UPDATE(...) __VA_ARGS__
    #define NO_ASYNC_UPDATE(...) // For when we need branching
#else
    #define ASYNC_UPDATE(...)
    #define NO_ASYNC_UPDATE(...) __VA_ARGS__
#endif
