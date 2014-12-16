// Implementation file for the -*- C++ -*- dynamic memory management header.

#include "new"

const std::nothrow_t std::nothrow = { };

using std::new_handler;
new_handler __new_handler;

new_handler
std::set_new_handler (new_handler handler) throw()
{
  new_handler prev_handler = __new_handler;
  __new_handler = handler;
  return prev_handler;
}

std::bad_alloc::~bad_alloc() throw() { }
