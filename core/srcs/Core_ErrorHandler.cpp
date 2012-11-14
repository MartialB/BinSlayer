#include "Core_ErrorHandler.hh"

BinSlay::Core_ErrorHandler::Core_ErrorHandler()
  : _current_status(BinSlay::Core_ErrorHandler::Status::OK)
{
}

BinSlay::Core_ErrorHandler::Status::status_e
BinSlay::Core_ErrorHandler::status() const
{
  return _current_status;
}

std::string const
BinSlay::Core_ErrorHandler::msg() const
{
  return _buf_error.str();
}
