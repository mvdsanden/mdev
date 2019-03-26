// mdmem_allocator.h                                                    -*-c++-*-
#ifndef __INCLUDED_MDMEM_ALLOCATOR
#define __INCLUDED_MDMEM_ALLOCATOR

#include <experimental/memory_resource>

namespace MvdS {
  namespace mdmem {

    typedef std::experimental::pmr::memory_resource Allocator;

  }
}


#endif // __INCLUDED_MDMEM_ALLOCATOR
