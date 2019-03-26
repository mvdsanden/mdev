// mdmem_allocator.t.cpp                                                -*-c++-*-
#include <mdmem_allocator.h>

#include <mdmem_allocatorutil.h>

#include <iostream>
#include <string>

using namespace std;
using namespace MvdS;
using namespace MvdS::mdmem;

typedef Allocator Obj;

size_t g_errorCount = 0;

#define ASSERT(x) if (!(x)) { ++g_errorCount; cerr << "Error on line " << __LINE__ << " assert(" << #x << ")." << endl; }

int main(int argc, char *argv[])
{

  int testcase = (argc > 1?std::atoi(argv[1]):0);

  bool verbose = argc > 1;
  bool veryVerbose = argc > 2;
  bool veryVeryVerbose = argc > 3;

  (void)verbose;
  (void)veryVerbose;
  (void)veryVeryVerbose;

  switch (testcase) {
  case 0: // ALWAYS LAST TEST CASE

  case 2:
    {
      Obj *o = AllocatorUtil::defaultAllocator();

      char *data = reinterpret_cast<char*>(o->allocate(16));

      data[0] = 'a';
      data[1] = 'b';
      data[2] = 'c';
      data[3] = '\0';

      ASSERT("abc" == std::string(data));

      o->deallocate(data, 16);
      
    } break;
    
  case 1:
    {

      Obj *o = AllocatorUtil::defaultAllocator();

      ASSERT(o);
      
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
