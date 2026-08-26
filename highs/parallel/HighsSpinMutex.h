/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                       */
/*    This file is part of the HiGHS linear optimization suite           */
/*                                                                       */
/*    Available as open-source under the MIT License                     */
/*                                                                       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef HIGHS_SPIN_MUTEX_H_
#define HIGHS_SPIN_MUTEX_H_

#include <atomic>

#include "HConfig.h"

#if defined(HIGHS_HAVE_MM_PAUSE) && \
    (defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || \
     defined(_M_X64))
#include <immintrin.h>
#else
#include <thread>
#endif

class HighsSpinMutex {
  std::atomic<bool> flag{false};

 public:
  static void yieldProcessor() {
#if defined(HIGHS_HAVE_MM_PAUSE) && \
    (defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || \
     defined(_M_X64))
    _mm_pause();
#else
    std::this_thread::yield();
#endif
  }

  bool try_lock() { return !flag.exchange(true, std::memory_order_acquire); }

  void lock() {
    while (true) {
      if (!flag.exchange(true, std::memory_order_acquire)) return;

      while (flag.load(std::memory_order_relaxed)) yieldProcessor();
    }
  }

  void unlock() { flag.store(false, std::memory_order_release); }
};

#endif
