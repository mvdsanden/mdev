// mdmt_threadpool.cpp                                                 -*-c++-*-
#include <mdmt_threadpool.h>

#include <iostream>

namespace MvdS {
namespace mdmt {

void ThreadPoolMetrics::print(std::ostream &stream, size_t level,
                              size_t spacesPerLevel) const {
#define P(x)                                                                   \
  {                                                                            \
    for (size_t i = 0; i < level * spacesPerLevel; ++i)                        \
      stream << " ";                                                           \
    stream << #x " = " << x.load(std::memory_order_relaxed) << "\n";           \
  }
  P(d_pendingJobsCount);
  P(d_processingJobsCount);
  P(d_totalJobsProcessed);
  P(d_threadCount);
  P(d_totalThreadIncreaseCount);
  P(d_totalThreadDecreaseCount);
#undef P
}

void ThreadPoolBase::removeThisThread() {
  MDLOG_SET_CATEGORY("ThreadPoolBase::removeThisThread");
  std::lock_guard<std::mutex> lk(d_mutex);
  auto i = d_threads.find(std::this_thread::get_id());
  if (d_threads.end() == i) {
    MDLOG_ERROR << "No thread with ID=" << std::this_thread::get_id() << MDLOG_END;
    return;
  }

  i->second->detach();
  d_threads.erase(i);

  d_metrics.threadDecrease();
}

void ThreadPoolBase::increaseThreads() {
  std::lock_guard<std::mutex> lk(d_mutex);
  if (d_threads.size() < d_maximumThreadCount) {
    std::unique_ptr<std::thread> t =
        std::make_unique<std::thread>(&ThreadPoolBase::threadMain, this);
    d_threads.emplace(std::make_pair(t->get_id(), std::move(t)));

    d_metrics.threadIncrease();
  }
}

void ThreadPoolBase::stopAllThreads() {
  ThreadMap threads; //(d_allocator_p);

  {
    std::lock_guard<std::mutex> lk(d_mutex);
    std::swap(d_threads, threads);
  }

  for (auto &&[id, thread] : threads) {
    if (std::this_thread::get_id() == id) {
      // TBD: This might not be safe when the pool is destroyed soon
      //      afterwards.
      thread->detach();
    } else {
      thread->join();
    }
  }
}

} // namespace mdmt
} // namespace MvdS
