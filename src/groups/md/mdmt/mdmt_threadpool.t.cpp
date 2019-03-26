// mdmt_threadpool.t.cpp                                               -*-c++-*-
#include <mdmt_threadpool.h>
#include <mdmt_fixedqueue.h>

#include <iostream>
#include <thread>

using namespace std;
using namespace MvdS;
using namespace MvdS::mdmt;

typedef ThreadPool<FixedQueue<std::function<void()>>> Obj;
typedef ThreadPoolConfiguration Conf;

size_t g_errorCount = 0;

#define ASSERT(x) if (!(x)) { ++g_errorCount; cerr << "Error on line " << __LINE__ << " assert(" << #x << ")." << endl; }


size_t g_executionCount = 0;

thread_local size_t g_localInt = 0;

int main(int argc, char *argv[])
{

  int testcase = (argc > 0?std::atoi(*argv):0);

  bool verbose = argc > 1;
  bool veryVerbose = argc > 2;
  bool veryVeryVerbose = argc > 3;

  (void)verbose;
  (void)veryVerbose;
  (void)veryVeryVerbose;
  
  auto job = [](){ ++g_executionCount; };
  auto slowJob = [](){ ++g_executionCount; this_thread::sleep_for(100ms); };
  
  switch (testcase) {
  case 0: // ALWAYS LAST TEST CASE

  case 3:
    {
      Conf conf;
      Obj o(5, 0, conf);

      o.start();

      o.enqueue([]{ g_localInt = 5; this_thread::sleep_for(100ms); });
      o.enqueue([]{ g_localInt = 10; this_thread::sleep_for(100ms); });

      this_thread::sleep_for(1s);

      o.enqueue([]{ cerr << this_thread::get_id() << ": g_localInt = " << g_localInt << endl; this_thread::sleep_for(100ms); });
      o.enqueue([]{ cerr << this_thread::get_id() << ": g_localInt = " << g_localInt << endl; this_thread::sleep_for(100ms); });

      this_thread::sleep_for(1s);
      
      o.stop();

      cerr << "g_localInt = " << g_localInt << endl;
      
    } break;
    
  case 2:
    {

      Conf conf;
      Obj o(5, 0, conf);

      o.start();
      
      ASSERT(o.enqueue(job));
      ASSERT(o.metrics().d_threadCount == 1);
      
      this_thread::sleep_for(3s);

      ASSERT(o.metrics().d_threadCount == 0);

      ASSERT(o.enqueue(slowJob));
      ASSERT(o.metrics().d_threadCount == 1);
      ASSERT(o.enqueue(slowJob));
      ASSERT(o.metrics().d_threadCount == 2);
      ASSERT(o.enqueue(slowJob));
      ASSERT(o.metrics().d_threadCount == 3);
      ASSERT(o.enqueue(slowJob));
      ASSERT(o.metrics().d_threadCount == 4);
      ASSERT(o.enqueue(slowJob));
      ASSERT(o.metrics().d_threadCount == 5);
      ASSERT(o.enqueue(slowJob));
      ASSERT(o.metrics().d_threadCount == 5);

      this_thread::sleep_for(3s);

      ASSERT(o.metrics().d_threadCount == 0);
      
      o.stop();

      ASSERT(7 == g_executionCount);

      if (verbose) {
	cerr << o.metrics() << endl;
      }
      
    } break;
    
  case 1:
    {

      Conf conf;
      Obj o(5, 0, conf);

      o.start();
      
      ASSERT(o.enqueue(job));

      this_thread::sleep_for(1s);
      
      o.stop();

      ASSERT(1 == g_executionCount);

      if (verbose) {
	cerr << o.metrics() << endl;
      }
      
    } break;

  default:
    cerr << "Unknown testcase: " << testcase << endl;
    return -1;
    
  };

  if (g_errorCount) {
    cerr << "There were error." << endl;
  }
  
  return static_cast<int>(g_errorCount);
}
