// mdio_buffer.h                                                        -*-c++-*-
#ifndef __INCLUDED_MDIO_BUFFER
#define __INCLUDED_MDIO_BUFFER

#include <mdmem_allocator.h>
#include <mdmem_allocatorutil.h>

#include <cstring>

namespace MvdS {
namespace mdio {

// ==================
// Class: ConstBuffer
// ==================
  
class ConstBuffer
{
  // Provides a const data buffer.

protected:
  // PROTECTED DATA

  const char *d_buffer;
  size_t      d_size;

public:
  // CREATORS

  ConstBuffer(const char *buffer, size_t size)
      // Create constant buffer containing a reference to the specified
      // 'buffer' with the specified 'size'.
      : d_buffer(buffer)
      , d_size(size)
  {}

  ~ConstBuffer() {}

  // ACCESSORS

  const char *buffer() const
  // Return the buffer.
  {
    return d_buffer;
  }

  size_t size() const
  // Return the size.
  {
    return d_size;
  }
};

// ======================
// Class: TemporaryBuffer
// ======================

class TemporaryBuffer : public ConstBuffer
{
  // Provide a temporary buffer that can be assigned, but does not deallocate
  // anything. This is used to return a buffer from a function, without the
  // temporary copy causing the deallocation to be called.
  friend class Buffer;

  mdmem::Allocator *d_allocator_p;

  // PRIVATE CREATORS

  TemporaryBuffer(char *buffer, size_t size, mdmem::Allocator *allocator = 0)
      // Create buffer with a reference to the specified 'buffer' and the
      // specified 'size'. Optionally deallocate using the specified
      // 'allocator', otherwise deallocate using the default allocator.
      : d_allocator_p(mdmem::AllocatorUtil::defaultAllocator(allocator))
      , ConstBuffer(const_cast<const char *>(buffer), size)
  {}

public:
  // CREATORS
  
  virtual ~TemporaryBuffer() {}
    
};

// =============
// Class: Buffer
// =============

class Buffer : public TemporaryBuffer
{
  // Provides a data buffer.

public:
  // CLASS METHODS

  static TemporaryBuffer
  copy(const char *buffer, size_t size, mdmem::Allocator *allocator)
  // Create a buffer object containing a copy of the specified 'buffer' with
  // the specified 'size'. Optionally the specified 'allocator' is used for
  // memory allocation.
  {
    allocator       = mdmem::AllocatorUtil::defaultAllocator(allocator);
    char *newBuffer = reinterpret_cast<char *>(allocator->allocate(size));
    memcpy(newBuffer, buffer, size);
    return TemporaryBuffer(newBuffer, size, allocator);
  }

  static TemporaryBuffer ref(char *buffer, size_t size)
  {
    return TemporaryBuffer(buffer, size, mdmem::AllocatorUtil::nullAllocator());
  }

  // CREATORS

  Buffer(mdmem::Allocator *allocator = 0)
      // Create empty buffer.
      : TemporaryBuffer(0, 0, allocator)
  {}

  Buffer(char *buffer, size_t size, mdmem::Allocator *allocator = 0)
      // Create buffer with a reference to the specified 'buffer' and the
      // specified 'size'. Optionally deallocate using the specified
      // 'allocator', otherwise deallocate using the default allocator.
      : TemporaryBuffer(buffer, size, allocator)
  {}

  Buffer(const TemporaryBuffer& buffer)
    // Copy from the specified 'buffer'.
    : TemporaryBuffer(buffer)
  {
  }

  ~Buffer() { d_allocator_p->deallocate(buffer(), size()); }

  // MANIPULATORS

  // ACCESSORS

  char *buffer()
  // Return the buffer.
  {
    return const_cast<char *>(d_buffer);
  }


  };

} // namespace mdio
} // namespace MvdS

#endif // __INCLUDED_MDIO_BUFFER
