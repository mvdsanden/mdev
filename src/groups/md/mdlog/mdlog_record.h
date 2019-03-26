// log_record.h                                                       -*-c++-*-
#ifndef __INCLUDED_LOG_RECORD
#define __INCLUDED_LOG_RECORD

#include <io_buffer.h>
#include <io_buffervector.h>
#include <mem_allocator.h>

#include <chrono>

namespace MvdS {
  namespace log {

			   // ============
			   // Class Record
			   // ============
    
    class Record {
      // Provides log record.
      //
      // TODO: Use FixedBufferPoolAllocator for the IO buffers.
      
    public:

      // PUBLIC TYPES
      
      typedef std::chrono::steady_clock::time_point time_point;
      
    private:

      // PRIVATE TYPES

      enum {
	k_defaultBufferSize = 256
      };
      
      // PRIVATE DATA
      
      Level            d_level;       // Log level.
      time_point       d_timePoint;   // Log time.
      io::BufferVector d_buffers;     // Backing buffers.
      mem::Allocator  *d_allocator_p;
      
    public:

      Record(Level             level,
	     const time_point& timePoint,
	     mem::Allocator   *allocator)
	: d_level(level)
	, d_timePoint(timePoint)
	, d_buffers(k_defaultBufferSize, allocator)
	, d_allocator_p(allocator)
      {}

      ~Record()
      {}
      
      // ACCESSORS

      io::Buffer *nextBuffer()
      // Allocate a new buffer and appends it to the buffer vector. Return the
      // newly allocated buffer.
      {
	d_buffers.emplace_back();
	return d_buffer.back();
      }
      
      Level level() const
      // Return log level for this record.
      {
	return d_level;
      }

      const time_point& timePoint() const
      // Return log time.
      {
	return d_timePoint;
      }
      
      const io::BufferVector& buffers() const
      // Return the buffers encapsulating the record data.
      {
	return d_buffers;
      }      
      
    };

    class RecordStreamBuffer : public std::streambuf {
      // Provides a streambuf for writing to a Record object.

      // DATA
      
      Record     *d_record;   // The record object to write to.
      io::Buffer *d_buffer;   // The current output buffer.
      size_t      d_position; // Current position in the output buffer.

      // PRIVATE MANIPULATORS
      
      void nextBuffer()
      // Tells the record to allocate and push a new buffer for output.
      {
	d_buffer   = d_record->nextBuffer();
	d_position = 0;
      }
      
    protected:

      // PROTECTED MANIPULATORS
      
      virtual std::streamsize xsputn(const char_type* src, std::streamsize size)
      // Copy the specifried 'src' buffer with the specified 'size' to the
      // record. Return number of characters written.
      {
	const char_type *srcEnd = src + size;
	char_type *dst          = d_buffer.begin() + d_position;

	while (srcEnd != src) {
	  size_t count = std::min(d_buffer.end() - dst, srcEnd - src);
	  
	  memcpy(dst, src, count);
	  
	  dst += count;
	  src += count;

	  if (d_buffer.end() == dst) {
	    nextBuffer();
	    dst = d_buffer.begin() + d_position;
	  }
	}

	d_position = dst - d_buffer.begin();
      }
      
    public:

      // CREATORS
      
      RecordStreamBuffer(Record *record);
      // Create RecordStreamBuffer that writes to the specified 'record'.

      ~RecordStreamBuffer();
      
    };

  }
}


#endif // __INCLUDED_LOG_RECORD
