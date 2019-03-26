// log_observer.h                                                     -*-c++-*-
#ifndef __INCLUDED_LOG_OBSERVER
#define __INCLUDED_LOG_OBSERVER

namespace MvdS {
  namespace log {

    class Record;

			  // ==============
			  // Class Observer
			  // ==============
    
    class Observer {
      // Provides the log observer base class.

    public:

      virtual void publish(const std::shared_ptr<Record>& record) = 0;
      // Publish the specified 'record' to the observer.
      
    };

  }
}


#endif // __INCLUDED_LOG_OBSERVER
