// mdlog_logger.h                                                       -*-c++-*-
#ifndef __INCLUDED_MDLOG_LOGGER
#define __INCLUDED_MDLOG_LOGGER

#include <iostream>

#define MDLOG_SET_CATEGORY(x) const std::string __mdlog_logger_category = x

#define MDLOG_PRINT_HEADER(level) "[" level "] " <<  __mdlog_logger_category << ": "
#define MDLOG_INFO std::cerr << MDLOG_PRINT_HEADER("INFO")
#define MDLOG_DEBUG std::cerr << MDLOG_PRINT_HEADER("DEBUG")
#define MDLOG_TRACE std::cerr << MDLOG_PRINT_HEADER("TRACE")
#define MDLOG_ERROR std::cerr << MDLOG_PRINT_HEADER("ERROR")

#define MDLOG_END std::endl

namespace MvdS {
namespace mmt {

}
}

#endif // __INCLUDED_MDLOG_LOGGER
