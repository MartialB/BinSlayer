#include <iostream>
#include <dlfcn.h>
#include <errno.h>
#include "Dll.h"

Dll::Dll(std::string const &name)
	: name(name),
	  errorCode(0)
{
}

Dll::~Dll(void)
{
}

bool Dll::open(void)
{
	this->hdl = dlopen(this->name.c_str(), RTLD_NOW);
	if (this->hdl == NULL)
	{
		this->errorCode = errno;
		std::cout << dlerror() << std::endl;
		return false;
	}
	return true;
}

bool Dll::close(void)
{
	int	retCode = 0;

	retCode = dlclose(this->hdl);
	if (retCode)
	{
		this->errorCode = retCode;
		return false;
	}
	return true;
}

void * Dll::getSymbole(std::string const &Symbole)
{
	void * ret = dlsym(this->hdl, Symbole.c_str());

	if (ret == NULL)
	{
		this->errorCode = errno;
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
	return this->name;
}
