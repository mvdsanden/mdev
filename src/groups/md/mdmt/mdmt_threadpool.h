// mdmt_threadpool.h                                                   -*-c++-*-
#ifndef __INCLUDED_MDMT_THREADPOOL
#define __INCLUDED_MDMT_THREADPOOL

#include <mdlog_logger.h>
#include <mdmem_allocator.h>

#include <atomic>
#include <functional>
#include <iosfwd>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace MvdS {
namespace mdmt {

typedef std::function<void()> ThreadPoolJob;
// ThreadPool job type.

// =============================
// Class ThreadPoolConfiguration
// =============================

struct ThreadPoolConfiguration
{
  // Provides configuration for a thread pool
};

// ========================
// Struct ThreadPoolMetrics
// ========================

struct ThreadPoolMetrics
{
  // Structure for keeping metrics on a thread pool.

  // DATA

  std::atomic<size_t> d_pendingJobsCount         = 0;
  std::atomic<size_t> d_processingJobsCount      = 0;
  std::atomic<size_t> d_totalJobsProcessed       = 0;
  std::atomic<size_t> d_threadCount              = 0;
  std::atomic<size_t> d_totalThreadIncreaseCount = 0;
  std::atomic<size_t> d_totalThreadDecreaseCount = 0;

  // MANIPULATORS

  void enqueueJob() { ++d_pendingJobsCount; }

  void beginJob()
  {
    --d_pendingJobsCount;
    ++d_processingJobsCount;
  }

  void endJob()
  {
    --d_processingJobsCount;
    ++d_totalJobsProcessed;
  }

  void threadIncrease()
  {
    ++d_threadCount;
    ++d_totalThreadIncreaseCount;
  }

  void threadDecrease()
  {
    --d_threadCount;
    ++d_totalThreadDecreaseCount;
  }

  // ACCESSORS

  bool isFullyLoaded() const
  // Return true if all threads are fully loaded or there are more pending
  // jobs than active threads.
  {
    MDLOG_SET_CATEGORY("mdmt::ThreadPoolMetrics::isFullyLoaded");
    const size_t threadsWorking = d_processingJobsCount.load();
    const size_t threadCount    = d_threadCount.load();
    const size_t pendingJobs    = d_pendingJobsCount.load();

    MDLOG_TRACE << "threadsWorking = " << threadsWorking
                << ", threadCount = " << threadCount
                << ", pendingJobs = " << pendingJobs << MDLOG_END;

    return (threadsWorking + pendingJobs > threadCount);
  }

  void print(std::ostream &stream,
             size_t        level          = 0,
             size_t        spacesPerLevel = 2) const;
  // Print metrics to the specified 'stream'.
};

// ====================
// Class ThreadPoolBase
// ====================

class ThreadPoolBase
{
  // Base for the ThreadPool templated class.

private:
  virtual void threadMain() = 0;
  // Pure virtual thread mainloop method.

protected:
  // PROTECTED TYPES

  typedef std::unordered_map<std::thread::id, std::unique_ptr<std::thread>>
      ThreadMap;

  // PROTECTED DATA

  size_t d_maximumThreadCount;
  // Maximum number of threads that will be used by the thread pool.

  size_t d_minimumThreadCount;
  // Minimum number of threads that will be used by the thread poool.

  ThreadMap d_threads;
  // Maps thread ids to thread objects.

  std::mutex d_mutex;
  // Mutex for the threads map.

  ThreadPoolMetrics d_metrics;
  // Metrics on threadpool queues and operations.

  mdmem::Allocator *d_allocator_p;

  // PROTECTED CREATORS

  ThreadPoolBase(
      size_t                         maximumThreadCount,
      size_t                         minimumThreadCount = 0,
      const ThreadPoolConfiguration &config    = ThreadPoolConfiguration(),
      mdmem::Allocator *             allocator = 0)
      : d_maximumThreadCount(maximumThreadCount)
      , d_minimumThreadCount(std::min(minimumThreadCount, maximumThreadCount))
      , d_threads()
      , d_allocator_p(allocator)
  {}

  // PROTECTED MANIPULATORS

  void removeThisThread();
  // Remove the current thread from the thread map.

  void increaseThreads();
  // Increase the number of threads by one.

  void stopAllThreads();
  // Stop all threads.

  void checkLoad()
  // Checks the load of the thread pool and adds a thread if necessary.
  {
    if (d_metrics.d_threadCount < d_maximumThreadCount &&
        d_metrics.isFullyLoaded()) {
      increaseThreads();
    }
  }

public:
  // PUBLIC ACCESSORS

  const ThreadPoolMetrics &metrics() const
  // Return the thread pool metrics.
  {
    return d_metrics;
  }
};

// ================
// Class ThreadPool
// ================

template <class QueueType>
class ThreadPool : public ThreadPoolBase
{
  // Provides a thread pool for enqueuing jobs to multiple threads.

public:
  // PUBLIC TYPES

  typedef typename QueueType::Configuration QueueConfig;

private:
  // PRIVATE DATA
  QueueType d_queue;
  // The queue used by the thread pool.

  // PRIVATE MANIPULATORS

  virtual void threadMain()
  {
    MDLOG_SET_CATEGORY("mdmt::ThreadPool::threadMain");
    MDLOG_TRACE << "Starting thread " << std::this_thread::get_id()
                << MDLOG_END;

    while (true) {
      ThreadPoolJob job;
      auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(2);

      const auto result = d_queue.popWaitUntil(&job, timeout);

      if (QueueType::e_success == result) {
        d_metrics.beginJob();
        job();
        d_metrics.endJob();
        continue;
      }

      if (QueueType::e_timeout == result) {
        removeThisThread();
      }

      break;
    }

    MDLOG_TRACE << "Stopping thread " << std::this_thread::get_id()
                << MDLOG_END;
  }

public:
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  // CREATORS

  ThreadPool(size_t                         maximumThreadCount,
             size_t                         minimumThreadCount = 0,
             const ThreadPoolConfiguration &config = ThreadPoolConfiguration(),
             const QueueConfig &            queueConfig = QueueConfig(),
             mdmem::Allocator *             allocator   = 0)
      // Create thread pool with the specified 'maximumThreadCount' and the
      // specified 'minimumThreadCount'. Optionally with the specified
      // 'config' and the specified 'queueConfig'. Optionally the provided
      // 'allocator' is used for memory allocations.
      : ThreadPoolBase(
            maximumThreadCount, minimumThreadCount, config, allocator)
      , d_queue(queueConfig)
  {}

  ~ThreadPool()
  // Destroy thread pool. Behavior is undefined if the thread pool is
  // destroyed from within one of its own threads.
  {
    stop();
  }

  // MANIPULATORS

  void start()
  // Start the thread pool. Behavior is undefined unless this is called after
  // construction or after a call to 'stop'. Note that this method is not
  // guarranteed to be thread-safe.
  {
    d_queue.start();
  }

  void stop()
  // Stop the thread pool. Behavior is undefined unless 'start' was called
  // earlier. Note that this method is not guarranteed to be thread-safe.
  {
    d_queue.stop();
    stopAllThreads();
  }

  bool enqueue(const ThreadPoolJob &job)
  // Enqueue the specified 'job' to the thread pool. This method is thread
  // safe and can be called from multiple threads concurently.
  {
    MDLOG_SET_CATEGORY("mdmt::ThreadPool::enqueue");

    d_metrics.enqueueJob();

    if (!d_queue.tryPush(job)) {
      return false;
    }

    checkLoad();

    return true;
  }
};

} // namespace mdmt
} // namespace MvdS

// ------------------------
// Class: ThreadPoolMetrics
// ------------------------

inline std::ostream &operator<<(std::ostream &                       stream,
                                const MvdS::mdmt::ThreadPoolMetrics &metrics)
{
  metrics.print(stream);
  return stream;
}

#endif //  __INCLUDED_MDMT_THREADPOOL
