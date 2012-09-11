#ifndef _DLLMANAGER_H_
# define _DLLMANAGER_H_

# include <map>
# include <string>

#ifdef _WIN32
# include "Windows/Dll.h"
#elif __linux__
# include "Linux/Dll.h"
#endif // _WIN32 linux

class DllManager
{
public:
	static DllManager* instance();
public:
	DllManager(void);
	~DllManager(void);

	bool OpenLibrary(std::string const &);
	bool CloseLibrary(std::string const &);
	int GetLastErrorCode(void) const;
	bool isGood(void) const;
	bool isLibraryOpen(std::string const &);

	template <typename T>
	T GetInstanceFromLibrary(std::string const &DllName, std::string const &Symbole)
	{
		T (*loader)(void);

		loader = reinterpret_cast <T (*)(void)> (this->DllList[DllName]->getSymbole(Symbole));
    if (loader)
		  return loader();
    return 0;
	}

	template <typename T>
	T GetSymboleFromLibrary(std::string const &DllName, std::string const &Symbole)
	{
		return reinterpret_cast <T> (this->DllList[DllName]->getSymbole(Symbole));
	}

private:
	int errorCode;
	std::map <std::string, IDll *>		DllList;
	static	DllManager *singleton;
};

#endif // _DLL_H_
