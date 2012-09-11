#ifndef _DLL_H_
# define _DLL_H_

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
	std::string		name;
	int				errorCode;
	void *			hdl;
};

#endif // _DLL_H_
