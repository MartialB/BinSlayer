#include <iostream>
#include "DLLmanager.h"

#ifdef _WIN32
const std::string dll_extension = ".dll";
#elif __linux__
# include <errno.h>
const std::string dll_extension = ".so";
#endif // !WIN32 !linux

DllManager::DllManager(void)
{
}

DllManager::~DllManager(void)
{
  if (singleton) delete singleton;
  for (auto it_dll = DllList.begin(); it_dll != DllList.end(); ++it_dll) {
    it_dll->second->close();
    delete it_dll->second;
  }
}

DllManager* DllManager::instance()
{
  // Singleton: carefull!
  if (singleton == 0)
    singleton = new DllManager();
  return singleton;
}

bool DllManager::OpenLibrary(std::string const &DllName)
{
  std::string rDllName;
  
  if (this->DllList.find(DllName) != this->DllList.end())
    return true;
  rDllName += "lib"; rDllName += DllName; rDllName += dll_extension;
  IDll * dll = new Dll(rDllName);
  if (dll->open() == false)
    {
#ifdef _WIN32
      this->errorCode = GetLastError();
#elif __linux__
      this->errorCode = errno;
#endif // _WIN32 __linux__
      return false;
    }
  this->DllList[DllName] = dll;
  return true;
}

bool DllManager::isLibraryOpen(std::string const &DllName)
{
  return this->DllList.count(DllName) != 0;
}

bool DllManager::CloseLibrary(std::string const &path)
{
  this->DllList[path]->close();
  return true;
}

bool DllManager::isGood(void) const
{
  return this->errorCode ? true : false;
}

int DllManager::GetLastErrorCode(void) const
{
  return this->errorCode;
}

DllManager* DllManager::singleton = nullptr;

