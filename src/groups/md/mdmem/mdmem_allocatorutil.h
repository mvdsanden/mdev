// mdmem_allocatorutil.h                                                -*-c++-*-
#ifndef __INCLUDED_MDMEM_ALLOCATORUTIL
#define __INCLUDED_MDMEM_ALLOCATORUTIL

#include <mdmem_allocator.h>

#include <experimental/memory_resource>

namespace MvdS {
  namespace mdmem {
    
    class AllocatorUtil {
    public:

      static Allocator *defaultAllocator(Allocator *allocator = nullptr);
      // Return the default allocator. Optionally specify 'allocator' to be
      // returned instead of the default.

      static Allocator *nullAllocator();
      // Return allocator that throws on allocation and does nothing on
      // deallocation.
      
    };

    inline Allocator *AllocatorUtil::defaultAllocator(Allocator *allocator)
    {
      return (allocator
	      ? allocator
	      : std::experimental::pmr::get_default_resource());
    }

    inline Allocator *AllocatorUtil::nullAllocator()
    {
      return std::experimental::pmr::null_memory_resource();
    }
    
  }
}

#endif //  __INCLUDED_MDMEM_ALLOCATORUTIL
