#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <dbghelp.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "dbghelp.lib")

void PrintStackTrace(CONTEXT* context)
{
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    SymInitialize(process, NULL, TRUE);

    CONTEXT context_copy = *context; // Make a copy of the context
    STACKFRAME64 stack_frame = {};
    stack_frame.AddrPC.Offset = context_copy.Rip;
    stack_frame.AddrPC.Mode = AddrModeFlat;
    stack_frame.AddrFrame.Offset = context_copy.Rbp;
    stack_frame.AddrFrame.Mode = AddrModeFlat;
    stack_frame.AddrStack.Offset = context_copy.Rsp;
    stack_frame.AddrStack.Mode = AddrModeFlat;

    std::cout << "Stack trace:" << std::endl;

    while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, 
                       process, 
                       thread, 
                       &stack_frame, 
                       &context_copy, 
                       NULL, 
                       SymFunctionTableAccess64, 
                       SymGetModuleBase64, 
                       NULL))
    {
        DWORD64 displacement = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        if (SymFromAddr(process, stack_frame.AddrPC.Offset, &displacement, symbol))
        {
            std::cout << symbol->Name << " + " 
                      << displacement << " (" 
                      << std::hex << stack_frame.AddrPC.Offset << std::dec << ")" 
                      << std::endl;
        }
        else
        {
            std::cout << "(No symbol) (" 
                      << std::hex << stack_frame.AddrPC.Offset << std::dec << ")" 
                      << std::endl;
        }
    }

    SymCleanup(process);
}


std::string GetExceptionString(DWORD exceptionCode) {
    switch(exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
        default:                                 return "UNKNOWN_EXCEPTION";
    }
}

LONG WINAPI UnhandledException(EXCEPTION_POINTERS* exceptionInfo) {
    std::cout << "Unhandled exception occurred:" << std::endl;
    std::cout << "Exception code: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionCode 
       << " (" << "Bruh" << ")" << std::endl;
    std::cout << "Exception address: 0x" << std::hex << exceptionInfo->ExceptionRecord->ExceptionAddress << std::endl;
    
    PrintStackTrace(exceptionInfo->ContextRecord);

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return EXCEPTION_EXECUTE_HANDLER;
}