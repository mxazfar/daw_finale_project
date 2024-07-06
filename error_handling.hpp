#ifndef ERR_HND_HPP
#define ERR_HND_HPP

#include <iostream>
#include <Windows.h>

std::string GetExceptionString(DWORD exceptionCode);
LONG WINAPI UnhandledException(EXCEPTION_POINTERS* exceptionInfo);

#endif