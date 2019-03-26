// mdmem_testallocator.h                                                -*-c++-*-
#ifndef __INCLUDED_MDMEM_TESTALLOCATOR
#define __INCLUDED_MDMEM_TESTALLOCATOR

#include <mdmem_allocator.h>
#include <mdmem_allocatorutil.h>

#include <atomic>

namespace MvdS {
  namespace mdmem {

    class TestAllocator : public Allocator {
      // Provides a test allocator.

      // DATA
      
      std::atomic<size_t> d_allocationCount;
      std::atomic<size_t> d_deallocationCount;

      std::atomic<size_t> d_totalBytesAllocated;
      std::atomic<size_t> d_totalBytesDeallocated;

      mdmem::Allocator *d_allocator_p;

      // PRIVATE MANIPULATORS
      
      virtual void *do_allocate(std::size_t bytes, std::size_t alignment)
      {
	++d_allocationCount;
	d_totalBytesAllocated += bytes;
	return d_allocator_p->allocate(bytes, alignment);
      }

      virtual void do_deallocate(void *p, std::size_t bytes, std::size_t alignment)
      {
	++d_deallocationCount;
	d_totalBytesDeallocated += bytes;
	return d_allocator_p->deallocate(p, bytes, alignment);
      }

      virtual bool do_is_equal(const std::experimental::pmr::memory_resource& other) const noexcept
      {
	return this == &other;
      }
      
    public:

      // CREATORS
      
      TestAllocator(Allocator *allocator = 0)
	// Create test allocator that used the specified 'allocator' for
	// memory allocation.
	: d_allocationCount(0)
	, d_deallocationCount(0)
	, d_totalBytesAllocated(0)
	, d_totalBytesDeallocated(0)
	, d_allocator_p(AllocatorUtil::defaultAllocator(allocator))
      {
      }

      ~TestAllocator()
      {
      }

      // ACCESSORS
      
      size_t allocationCount() const
      // Return number of allocations.
      {
	return d_allocationCount.load();
      }

      size_t deallocationCount() const
      // Return number of deallocations.
      {
	return d_deallocationCount.load();
      }

      size_t totalBytesAllocated() const
      // Return total bytes allocated.
      {
	return d_totalBytesAllocated;
      }

      size_t totalBytesDeallocated() const
      // Return total bytes deallocated.
      {
	return d_totalBytesDeallocated;
      }
      
    };

  }
}


#endif // __INCLUDED_MDMEM_TESTALLOCATOR
