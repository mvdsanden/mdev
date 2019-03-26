// mdmt_fixedqueue.h                                                   -*-c++-*-
#ifndef __INCLUDED_MDMT_FIXEDQUEUE
#define __INCLUDED_MDMT_FIXEDQUEUE

#include <mdmem_allocator.h>
#include <mdmem_allocatorutil.h>

#include <condition_variable>
#include <mutex>

namespace MvdS {
namespace mdmt {

// ================
// Class FixedQueue
// ================

template <typename ValueType>
class FixedQueue
{
  // Provides an efficient thread-enabled fixed size queue.

public:
  // PUBLIC TYPES

  struct Configuration
  {
    size_t d_size;

    Configuration()
        : d_size(16)
    {}
  };

private:
  // PRIVATE DATA
  mutable std::mutex      d_mutex;
  size_t                  d_capacity;
  ValueType *             d_data;
  std::condition_variable d_cond;
  size_t                  d_start;
  std::atomic<size_t>     d_count;
  std::atomic<bool>       d_stopped;
  mdmem::Allocator *      d_allocator_p;

  void allocateData()
  {
    d_data = reinterpret_cast<ValueType *>(d_allocator_p->allocate(
        sizeof(ValueType) * d_capacity, alignof(ValueType)));
  }

  void deallocateData()
  {
    d_allocator_p->deallocate(
        d_data, sizeof(ValueType) * d_capacity, alignof(ValueType));
  }

  void push(const ValueType &value)
  {
    size_t index = (d_start + d_count++) % d_capacity;
    new (d_data + index) ValueType(value);
  }

  void pop(ValueType *value)
  {
    size_t index = d_start % d_capacity;
    *value       = d_data[index];
    d_start      = (d_start + 1) % d_capacity;
    --d_count;
  }

public:
  // PUBLIC TYPES

  // TODO: move this outside of the templated class.
  enum TimedWaitResult
  {
    e_success = 0,
    e_stopped = 1,
    e_timeout = 2
  };

  // CREATORS

  FixedQueue(const Configuration &config, mdmem::Allocator *allocator = 0)
      : d_capacity(config.d_size)
      , d_data(nullptr)
      , d_start(0)
      , d_count(0)
      , d_stopped(false)
      , d_allocator_p(mdmem::AllocatorUtil::defaultAllocator(allocator))
  {
    allocateData();
  }

  ~FixedQueue() { deallocateData(); }

  // MANIPULATORS

  void start() { d_stopped.store(false); }

  void stop()
  {
    d_stopped.store(true);
    d_cond.notify_one();
  }

  bool tryPush(const ValueType &value)
  {
    {
      std::lock_guard<std::mutex> lk(d_mutex);

      if (full()) {
        return false;
      }

      push(value);
    }
    d_cond.notify_one();

    return true;
  }

  bool pushWait(const ValueType &value)
  {
    {
      std::unique_lock<std::mutex> lk(d_mutex);

      d_cond.wait(lk, [this]() { return !full() || d_stopped; });

      if (full()) {
        return false;
      }

      push(value);
    }
    d_cond.notify_one();

    return true;
  }

  template <class Clock, class Duration>
  TimedWaitResult
  pushWaitUntil(const ValueType &                               value,
                const std::chrono::time_point<Clock, Duration> &time)
  {
    {
      std::unique_lock<std::mutex> lk(d_mutex);

      bool result = d_cond.wait_until(
          lk, time, [this]() { return !full() || d_stopped; });

      if (d_stopped) {
        return e_stopped;
      } else if (!result) {
        return e_timeout;
      }

      push(value);
    }
    d_cond.notify_one();

    return e_success;
  }

  bool tryPop(ValueType *value)
  {
    {
      std::lock_guard<std::mutex> lk(d_mutex);

      if (empty()) {
        return false;
      }

      pop(value);
    }
    d_cond.notify_one();

    return true;
  }

  bool popWait(ValueType *value)
  {
    {
      std::unique_lock<std::mutex> lk(d_mutex);

      d_cond.wait(lk, [this]() { return !empty() || d_stopped; });

      if (empty()) {
        return false;
      }

      pop(value);
    }
    d_cond.notify_one();

    return true;
  }

  template <class Clock, class Duration>
  TimedWaitResult
  popWaitUntil(ValueType *                                     value,
               const std::chrono::time_point<Clock, Duration> &time)
  {
    {
      std::unique_lock<std::mutex> lk(d_mutex);

      bool result = d_cond.wait_until(
          lk, time, [this]() { return !empty() || d_stopped; });

      if (d_stopped) {
        return e_stopped;
      } else if (!result) {
        return e_timeout;
      }

      pop(value);
    }
    d_cond.notify_one();

    return e_success;
  }

  // ACCESSORS

  bool empty() const
  {
    // TODO: optimize for memory ordering.
    return 0 == d_count.load();
  }

  bool full() const
  {
    // TODO: optimize for memory ordering.
    return d_count.load() == d_capacity;
  }
};

} // namespace mdmt
} // namespace MvdS

#endif // __INCLUDED_MDMT_FIXEDQUEUE
