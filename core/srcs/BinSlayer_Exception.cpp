#include "BinSlayer_Exception.hh"

BinSlay::Error::Error(const std::string &text)
  : std::runtime_error(text)
{
}

BinSlay::Error::~Error() throw()
{
}
