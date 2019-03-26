// mdio_buffer.t.cpp                                                    -*-c++-*-
#include <mdio_buffer.h>

#include <mdmem_testallocator.h>

#include <iostream>
#include <thread>

using namespace std;
using namespace MvdS;
using namespace MvdS::mdio;

size_t g_errorCount = 0;

#define ASSERT(x) if (!(x)) { ++g_errorCount; cerr << "Error on line " << __LINE__ << " assert(" << #x << ")." << endl; }


int main(int argc, char *argv[])
{

  int  testcase        = (argc > 0 ? std::atoi(*argv) : 0);
  bool verbose         = argc > 1;
  bool veryVerbose     = argc > 2;
  bool veryVeryVerbose = argc > 3;

  (void)verbose;
  (void)veryVerbose;
  (void)veryVeryVerbose;  
  
  switch (testcase) {
  case 0: // ALWAYS LAST TEST CASE

    
  case 1:
    {
     
      {	
	const char *test = "test";
	ConstBuffer cb0(test, 4);
      }

      {
	const char *test = "test";
	ConstBuffer cb0(test, 4);
	ASSERT(test == cb0.buffer());
	ASSERT(4 == cb0.size());
      }

      {
	ConstBuffer cb0("hello", 5);
	ASSERT(5 == cb0.size());
	ASSERT(std::string(cb0.buffer(), cb0.size()) == "hello");
      }

      {
	mdmem::TestAllocator ta;
	char *dtest = reinterpret_cast<char *>(ta.allocate(5));
	Buffer cb0(dtest, 5, &ta);
      }

      {
	mdmem::TestAllocator ta;
	char *dtest = reinterpret_cast<char *>(ta.allocate(5));
	Buffer cb0(dtest, 5, &ta);
	ASSERT(dtest == cb0.buffer());
	ASSERT(5 == cb0.size());
      }

      {
	mdmem::TestAllocator ta;
	Buffer cb0 = Buffer::copy("hello", 5, &ta);
	ASSERT(1 == ta.allocationCount());
	ASSERT(5 == cb0.size());
	ASSERT(std::string(cb0.buffer(), cb0.size()) == "hello");
      }

      {
	mdmem::TestAllocator ta;
	char *dtest = reinterpret_cast<char *>(ta.allocate(5));
	ASSERT(1 == ta.allocationCount());
	Buffer cb0 = Buffer::ref(dtest, 5);
	ASSERT(1 == ta.allocationCount());
	ASSERT(5 == cb0.size());
	ASSERT(dtest == cb0.buffer());
	ta.deallocate(dtest, 5);
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
