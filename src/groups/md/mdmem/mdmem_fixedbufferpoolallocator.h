// mdmem_fixedbufferpoolallocator.h                                                -*-c++-*-
#ifndef __INCLUDED_MDMEM_FIXEDBUFFERPOOLALLOCATOR
#define __INCLUDED_MDMEM_FIXEDBUFFERPOOLALLOCATOR

#include <mdmem_allocator.h>
#include <mdmem_allocatorutil.h>

#include <atomic>

namespace MvdS {
  namespace mdmem {

    class FixedBufferPoolAllocator : public Allocator {
      // Provides an allocator the has a fixed size pool of fixed size buffers
      // which it uses to allocate from. If the allocation request is larger
      // than the size of the buffers or there are no free buffers in the pool
      // the provided backing allocator is used instead.

      // DATA
      std::pmr::vector<void *>  d_pool;
      size_t                    d_bufferSize;
      size_t                    d_alignment;
      std::atomic<size_t>       d_head;
      std::atomic<int>          d_rCount, d_wCount;
      size_t                    d_sizeMask;
      mdmem::Allocator           *d_allocator_p;

      // PRIVATE MANIPULATORS
      void *popBuffer()
      {
	// Note that there is no guarranteed that 'popBuffer' works immediately
	// after a 'pushBuffer', but we don't need that guarrantee here.
	int prev = d_rCount.fetch_sub(1);
	if (0 >= prev) {
	  // Queue was empty, repair and return false.
	  ++d_rCount;
	  return nullptr;
	}

	// If 'prev' > 0 we have reserved a pop index, so now we have all the
	// time in the world to claim it.
	
	size_t index = d_head.fetch_add(1) & d_sizeMask;
	void *result = d_pool[index];

	// Modulo the head so we don't overflow it.
	// TODO: optimize this by only do this when at 2*size or something like
	// that.
	d_head &= d_sizeMask;

	// We load the index, so now we can release it for writing.
	--d_wCount;
	return result;
      }

      bool pushBuffer(void *buffer)
      {
	int prev = d_wCount.fetch_add(1);
	if (d_pool.size() <= prev) {
	  // Queue was full, repair and return false.
	  --d_wCount;
	  return false;
	}

	size_t index = (d_head.load() + prev) & d_sizeMask;
	d_pool[index] = buffer;
	++d_rCount;
	return true;
      }
      
      virtual void *do_allocate(std::size_t bytes, std::size_t alignment)
      {
	void *result = (bytes == d_bufferSize
			&& alignment == d_alignment
			?popBuffer()
			:nullptr);

	if (nullptr == result) {
	  result = d_allocator_p->allocate(bytes, alignment);
	}

	return result;
      }

      virtual void do_deallocate(void *p, std::size_t bytes, std::size_t alignment)
      {
	if (bytes != d_bufferSize
	    || alignment != d_alignment
	    || pushBuffer(p)) {
	  d_allocator_p->deallocate(p, bytes, alignment);
	}
      }

      virtual bool do_is_equal(const std::experimental::pmr::memory_resource& other) const noexcept
      {
	return this == &other;
      }
      
    public:

      // CREATORS

      // TODO: make poolSize always 2^i.
      FixedBufferPoolAllocator(size_t     poolSize,
			       size_t     bufferSize,
			       size_t     alignment,
			       Allocator *allocator = 0)
	// Create FixedBufferPool allocator that used the specified 'allocator' for
	// memory allocation.
	: d_pool(poolSize, allocator)
	, d_bufferSize(bufferSize)
	, d_alignment(alignment)
	, d_allocator_p(AllocatorUtil::defaultAllocator(allocator))
      {
      }

      ~FixedBufferPoolAllocator()
      {
      }
      
      // ACCESSORS      
      
    };

  }
}


#endif // __INCLUDED_MDMEM_FIXEDBUFFERPOOLALLOCATOR
