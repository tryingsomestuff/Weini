#include "logger.hpp"

std::ostream  * LogIt::_os = &std::cout;

std::mutex LogIt::_mutex;