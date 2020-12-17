#include <pch.h>
#include <type_traits>
#include "LibraryLoader.h"

using namespace salvation::rhi;
using namespace salvation::external;

///////////////////////////////////////////////////////////////////////////
// LibraryLoader
///////////////////////////////////////////////////////////////////////////
LibraryHandle LibraryLoader::Load(const wchar_t* pName)
{
    return LoadDynamicLibrary(pName);
}

void LibraryLoader::Unload(LibraryHandle handle)
{
    UnloadDynamicLibrary(handle);
}

template<typename T>
void LibraryLoader::LoadFunction(LibraryHandle libraryHdl, const char *pFnctName, T &fnctPtr)
{
    fnctPtr = reinterpret_cast<T>(GetFunctionAddress(libraryHdl, pFnctName));
}

#include INCLUDE_IMPLEMENTATION(LibraryLoader)
