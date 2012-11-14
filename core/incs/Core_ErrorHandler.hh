#ifndef __CORE_ERROR_HANDLER_HH__
# define __CORE_ERROR_HANDLER_HH__

# include <sstream>
# include <cstdio>

#define IF_NULLPTR(object, type, fmt, ...)				\
  if (!(object)) {							\
    char buffer[256];							\
    sprintf(buffer, fmt, ##__VA_ARGS__);				\
    this->_buf_error << buffer << std::endl				\
		     << "  File: " << __FILE__ << std::endl		\
		     << "  Function: " << __FUNCTION__ << std::endl	\
		     << "  Line: " << __LINE__ ;			\
    this->_current_status = type;					\
  }									\

#define IF_NULLPTR_RETURN_NULL(object, type, fmt, ...)			\
  if (!(object)) {							\
    char buffer[256];							\
    sprintf(buffer, fmt, ##__VA_ARGS__);				\
    this->_buf_error << buffer << std::endl				\
		     << "  File: " << __FILE__ << std::endl		\
		     << "  Function: " << __FUNCTION__ << std::endl	\
		     << "  Line: " << __LINE__ ;			\
    this->_current_status = type;					\
    return nullptr;							\
  }									\

#define IF_NULLPTR_RETURN_FALSE(object, type, fmt, ...)			\
  if (!(object)) {							\
    char buffer[256];							\
    sprintf(buffer, fmt, ##__VA_ARGS__);				\
    this->_buf_error << buffer << std::endl				\
		     << "  File: " << __FILE__ << std::endl		\
		     << "  Function: " << __FUNCTION__ << std::endl	\
		     << "  Line: " << __LINE__ ;			\
    this->_current_status = type;					\
    return false;							\
  }									\

namespace BinSlay
{
  class Core_ErrorHandler
  {
  public:
    class Status
    {
    public:
      enum status_e
	{
	  OK = 0,
	  DLL_LOADING_FAIL,
	  SYMBOL_LOADING_FAIL,
	  BINARY_LOADING_FAIL,
	  BAD_ALLOC
	};
    };

  public:
    Core_ErrorHandler();
    Status::status_e status() const;
    std::string const msg() const;

  protected:
    Status::status_e   		_current_status;
    std::stringstream		_buf_error;
  };
}

#endif // !__CORE_ERROR_HANDLER_HH__
