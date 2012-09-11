#include "Dll.h"

Dll::Dll(std::string const &type)
	: type(type),
	  errorCode(0)
{
}

Dll::~Dll(void)
{
}

bool Dll::open(void)
{
	this->hdl = LoadLibraryEx(this->type.c_str(), NULL, 0);
	if (this->hdl == NULL)
	{
		this->errorCode = GetLastError();
		return false;
	}
	return true;
}

bool Dll::close(void)
{
	BOOL	retCode = true;

	retCode = FreeLibrary(this->hdl);
	if (retCode == false)
	{
		this->errorCode = GetLastError();
		return false;
	}
	return true;
}

# include <iostream>

void * Dll::getSymbole(std::string const &Symbole)
{
	void * ret = GetProcAddress(this->hdl, Symbole.c_str());

	if (ret == NULL)
	{
		this->errorCode = GetLastError();
		std::cerr << this->errorCode << std::endl;
		return NULL;
	}
	return ret;
}

int	Dll::getErrorCode(void) const
{
	return this->errorCode;
}

std::string const & Dll::getType(void) const
{
	return this->type;
}