#pragma once

#include <cstddef>
#include <thread>

#ifdef _MSC_VER
#  include <intrin.h>
#endif

#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Hints that the current thread is in a spin-wait loop.
 * @details It helps reduce power consumption and improves performance on hyper-threaded
 * processors by preventing the CPU from consuming unnecessary cycles while waiting.
 * It is particularly useful in low-contention scenarios, where the thread is likely to quickly
 * acquire the lock or condition it's waiting for, avoiding an expensive context switch.
 * On modern x86 processors, this instruction can be invoked using  `__builtin_ia32_pause()` in
 * GCC/Clang or `_mm_pause()` in MSVC. In non-x86 architectures, alternative mechanisms such as
 * yielding the CPU may be used instead.
 *
 * @ingroup core_threading
 * @see yield(std::size_t)
 */
inline void yield()
{
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
  // x86 and x86_64: Use the PAUSE instruction
#  ifdef _MSC_VER
  // Microsoft Visual C++
  _mm_pause();
#  elif defined(__GNUC__) || defined(__clang__)
  // GCC and Clang
  __builtin_ia32_pause();
#  else
  asm volatile("pause" ::: "memory");
#  endif

#elif defined(__aarch64__) || defined(__arm__)
  // ARM and AArch64: Use the YIELD instruction
#  if defined(__GNUC__) || defined(__clang__)
  asm volatile("yield" ::: "memory");
#  endif

#else
  // Fallback: Portable yield for unknown architectures
  std::this_thread::yield();
#endif
}

/**
 * @brief Repeats yield() for a fixed number of iterations.
 * @param count Number of pause/yield iterations to perform.
 * @ingroup core_threading
 * @see yield()
 */
inline void yield(std::size_t count)
{
  while(count-- > 0)
    yield();
}

RADAR_COMMON_NAMESPACE_END
