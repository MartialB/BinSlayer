#ifndef _IDll_H_
# define _IDll_H_

# include <string>

#ifdef _WIN32
	typedef HANDLE	SYMBOLE;
#else
	typedef void *	SYMBOLE;
#endif // _WIN32

class IDll
{
public:
	virtual bool open(void) = 0;
	virtual bool close(void) = 0;
	virtual SYMBOLE getSymbole(std::string const &) = 0;
	virtual int	getErrorCode(void) const = 0;
	virtual std::string const & getType(void) const = 0;
};

#endif // _IDll_H_