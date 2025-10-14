#pragma once

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__has_builtin)
#if __has_builtin(__builtin_debugtrap)
#define DEBUG_BREAK() __builtin_debugtrap()
#else
#define DEBUG_BREAK() raise(SIGTRAP)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() raise(SIGTRAP)
#else
#ifdef _WIN32
#include <windows.h>
#define DEBUG_BREAK() DebugBreak()
#else
#define DEBUG_BREAK() raise(SIGTRAP)
#endif
#endif

#ifndef NDEBUG
#define FUZZ_ASSERT(cond, stream, fmt, ...)                                  \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf((stream), "Assertion failed: (%s) %s:%d: " fmt "\n", #cond,      \
              __FILE__, __LINE__, ##__VA_ARGS__);                              \
      fflush((stream));                                                        \
      DEBUG_BREAK();                                                           \
    }                                                                          \
  } while (0)
#else
#define FUZZ_ASSERT(cond, stream, fmt, ...) ((void)0)
#endif

// /* Always-on variant */
// #define ASSERT_STREAM_ALWAYS(cond, stream, fmt, ...) \
//   do { \
//     if (!(cond)) { \
//       fprintf((stream), "Assertion failed (always): (%s) %s:%d: " fmt "\n", \
//               #cond, __FILE__, __LINE__, ##__VA_ARGS__); \
//       fflush((stream)); \
//       DEBUG_BREAK(); \
//     } \
//   } while (0)
