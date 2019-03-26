// io_testallocator.t.cpp                                             -*-c++-*-
#include <mdmem_testallocator.h>

#include <mdmem_testallocator.h>

#include <iostream>
#include <thread>

using namespace std;
using namespace MvdS;
using namespace MvdS::mdmem;

size_t g_errorCount = 0;

#define ASSERT(x) if (!(x)) { ++g_errorCount; cerr << "Error on line " << __LINE__ << " assert(" << #x << ")." << endl; }


int main(int argc, char *argv[])
{

  int testcase = (argc > 0?std::atoi(*argv):0);

  bool verbose = argc > 1;
  bool veryVerbose = argc > 2;
  bool veryVeryVerbose = argc > 3;

  (void)verbose;
  (void)veryVerbose;
  (void)veryVeryVerbose;
  
  
  switch (testcase) {
  case 0: // ALWAYS LAST TEST CASE

    
  case 1:
    {
      TestAllocator ta;
      ASSERT(0 == ta.allocationCount());
      ASSERT(0 == ta.deallocationCount());
      ASSERT(0 == ta.totalBytesAllocated());
      ASSERT(0 == ta.totalBytesDeallocated());

      char *test = reinterpret_cast<char*>(ta.allocate(5));

      ta.deallocate(test, 5);

      ASSERT(1 == ta.allocationCount());
      ASSERT(1 == ta.deallocationCount());
      ASSERT(5 == ta.totalBytesAllocated());
      ASSERT(5 == ta.totalBytesDeallocated());
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
