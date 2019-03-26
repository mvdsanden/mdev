// mdmt_fixedqueue.t.cpp                                               -*-c++-*-
#include <mdmt_fixedqueue.h>

#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace MvdS;
using namespace MvdS::mdmt;

typedef FixedQueue<int>::Configuration Conf;
typedef FixedQueue<int> Obj;

size_t g_errorCount = 0;

#define ASSERT(x) if (!(x)) { ++g_errorCount; cerr << "Error on line " << __LINE__ << " assert(" << #x << ")." << endl; }

#define P(x) { cerr << #x << " = " << x << "\n"; }

int main(int argc, char *argv[])
{

  int testcase = (argc > 1?std::atoi(argv[1]):0);

  bool verbose = argc > 1;
  bool veryVerbose = argc > 2;
  bool veryVeryVerbose = argc > 3;

  (void)verbose;
  (void)veryVerbose;
  (void)veryVeryVerbose;
  
  Conf config;
  config.d_size = 16;

  
  switch (testcase) {
  case 0: // ALWAYS LAST TEST CASE

  case 15:
    {

      Obj o(config);

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(1s); o.stop(); cerr << "STOPPED!" << endl; });

      int result;
      
      ASSERT(Obj::e_stopped == o.popWaitUntil(&result, chrono::steady_clock::now() + 3s));

      ASSERT((chrono::steady_clock::now() - t0) > 1s);
      ASSERT((chrono::steady_clock::now() - t0) < 3s);
      
      t.join();

    } break;

    
  case 14:
    {

      Obj o(config);

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(3s); ASSERT(o.tryPush(100)); cerr << "PUSHED!" << endl; });

      int result;
      (void)result;
      
      ASSERT(Obj::e_timeout == o.popWaitUntil(&result, chrono::steady_clock::now() + 1s));

      ASSERT((chrono::steady_clock::now() - t0) > 1s);
      ASSERT((chrono::steady_clock::now() - t0) < 3s);
      
      t.join();

    } break;

    
  case 13:
    {

      Obj o(config);

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(1s); o.stop(); cerr << "STOPPED!" << endl; });

      int result;
      (void)result;
      
      ASSERT(!o.popWait(&result));

      ASSERT((chrono::steady_clock::now() - t0) > 1s);
      
      t.join();

    } break;

    
  case 12:
    {

      Obj o(config);

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(1s); ASSERT(o.tryPush(100)); cerr << "PUSHED!" << endl; });

      int result;
      
      ASSERT(o.popWait(&result));
      ASSERT(100 == result);

      ASSERT((chrono::steady_clock::now() - t0) > 1s);
      
      t.join();

    } break;

    
  case 11:
    {

      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.pushWait(i));
      }

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(1s); o.stop(); cerr << "STOPPED!" << endl; });

      ASSERT(Obj::e_stopped == o.pushWaitUntil(-1, chrono::steady_clock::now() + 2s));

      ASSERT((chrono::steady_clock::now() - t0) < 2s);
      
      t.join();

    } break;
    
  case 10:
    {

      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.pushWait(i));
      }

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(5s); int result; (void)result; ASSERT(o.tryPop(&result)); cerr << "POPPED!" << endl; });

      ASSERT(Obj::e_timeout == o.pushWaitUntil(-1, chrono::steady_clock::now() + 2s));

      ASSERT((chrono::steady_clock::now() - t0) < 5s);
      ASSERT((chrono::steady_clock::now() - t0) > 2s);
      
      t.join();

    } break;

  case 9:
    {

      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.pushWait(i));
      }

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(1s); o.stop(); cerr << "STOPPED!" << endl; });

      ASSERT(!o.pushWait(-1));

      ASSERT((chrono::steady_clock::now() - t0) > 1s);
      
      t.join();

    } break;

    
  case 8:
    {

      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.pushWait(i));
      }

      auto t0 = chrono::steady_clock::now();
      
      std::thread t([&o](){ this_thread::sleep_for(1s); int result; (void)result; ASSERT(o.tryPop(&result)); cerr << "POPPED!" << endl; });

      ASSERT(o.pushWait(-1));

      ASSERT((chrono::steady_clock::now() - t0) > 1s);
      
      t.join();

    } break;
    
    
  case 7:
    {

      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.pushWait(i));
      }
      
    } break;

    
  case 6:
    {      
      Obj o(config);

      for (size_t j = 0; j < 1000; ++j) {
      
	for (size_t i = 0; i < config.d_size; ++i) {
	  ASSERT(o.tryPush(j * config.d_size + i));
	}
      
	ASSERT(!o.tryPush(-1));

	int result;

	for (size_t i = 0; i < config.d_size; ++i) {
	  ASSERT(o.tryPop(&result));

	  if (veryVerbose) {
	    P(j);
	    P(i);
	    P(j * config.d_size + i);
	    P(result);
	  }
	  
	  ASSERT(static_cast<size_t>(result) == (j * config.d_size + i))
	}
	
	ASSERT(!o.tryPop(&result));
      }
    } break;

    
  case 5:
    {      
      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.tryPush(1));
      }
      
      ASSERT(!o.tryPush(1));

      int result;
      (void)result;
      
      ASSERT(o.tryPop(&result));
      ASSERT(o.tryPush(1));
    } break;
    
    
  case 4:
    {      
      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.tryPush(1));
      }
      
      ASSERT(!o.tryPush(1));
    } break;

    
  case 3:
    {      
      Obj o(config);

      for (size_t i = 0; i < config.d_size; ++i) {
	ASSERT(o.tryPush(1));
      }
      
      ASSERT(!o.empty());
      ASSERT(o.full());      
    } break;

    
  case 2:
    {      
      Obj o(config);

      ASSERT(o.tryPush(1));
      
      ASSERT(!o.empty());
      ASSERT(!o.full());
      
    } break;

    
  case 1:
    {      
      Obj o(config);
     
      ASSERT(o.empty());
      ASSERT(!o.full());
      
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
