#ifndef _DLL_H_
# define _DLL_H_

# include <Windows.h>

# include "../IDll.h"

class Dll : public IDll
{
public:
	Dll(std::string const &);
	virtual ~Dll(void);

	virtual bool open(void);
	virtual bool close(void);
	virtual SYMBOLE getSymbole(std::string const &);
	virtual int	getErrorCode(void) const;
	virtual std::string const & getType(void) const;

private:
	std::string		type;
	DWORD			errorCode;
	HMODULE			hdl;
};

#endif // _DLL_H_