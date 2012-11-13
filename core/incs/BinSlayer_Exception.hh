#ifndef __BINSLAYER_EXCEPTION_HH__
# define __BINSLAYER_EXCEPTION_HH__

# include <iostream>
# include <exception>
# include <stdexcept>
# include <string>
# include <sstream>

// TODO: Level of error -> Critical / Debug / Warning

namespace BinSlay
{
  class Error : public std::runtime_error
  {
  public:
    explicit Error(const std::string &text);
    virtual ~Error() throw();
  };

}

#endif // !__BINSLAYER_EXCEPTION_HH__
