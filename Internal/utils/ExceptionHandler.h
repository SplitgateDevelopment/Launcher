#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <format>
#include <fstream>

#pragma comment(lib, "DbgHelp.lib")

namespace fs = std::filesystem;

namespace ExceptionHandler
{
    enum class ExitModes
    {
        Silent = EXCEPTION_EXECUTE_HANDLER,
        Crash = EXCEPTION_CONTINUE_SEARCH,
    };

    ExitModes ExitMode = ExitModes::Silent;

    void Stacktrace(CONTEXT* context, std::stringstream& ss)
    {
        // Initialize symbols handler
        HANDLE currentProcess = GetCurrentProcess();
        SymInitialize(currentProcess, NULL, TRUE);

        // Get current thread and stack frame
        STACKFRAME64 stackFrame;
        memset(&stackFrame, 0, sizeof(STACKFRAME64));

        //https://github.com/JochenKalmbach/StackWalker/blob/master/Main/StackWalker/StackWalker.cpp
        DWORD machineType;

#ifdef _M_IX86
        machineType = IMAGE_FILE_MACHINE_I386;
        stackFrame.AddrPC.Offset = context->Eip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context->Ebp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context->Esp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
        //Should be this
        machineType = IMAGE_FILE_MACHINE_AMD64;
        stackFrame.AddrPC.Offset = context->Rip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context->Rsp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context->Rsp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
        machineType = IMAGE_FILE_MACHINE_IA64;
        stackFrame.AddrPC.Offset = context->StIIP;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context->IntSp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrBStore.Offset = context->RsBSP;
        stackFrame.AddrBStore.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context->IntSp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_ARM64
        machineType = IMAGE_FILE_MACHINE_ARM64;
        stackFrame.AddrPC.Offset = context->Pc;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context->Fp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context->Sp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

        // Walk the stack
        while (StackWalk64(machineType, currentProcess, GetCurrentThread(), &stackFrame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
            // Get module name
            DWORD64 moduleBase = SymGetModuleBase64(currentProcess, stackFrame.AddrPC.Offset);
            char moduleName[MAX_PATH];

            if (GetModuleFileNameA((HMODULE)moduleBase, moduleName, MAX_PATH) != 0) {
                // Get symbol name
                DWORD64 displacement = 0;
                char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
                PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol->MaxNameLen = MAX_SYM_NAME;

                fs::path modulePath(moduleName);
                std::string moduleFileName = modulePath.filename().string();

                if (SymFromAddr(currentProcess, stackFrame.AddrPC.Offset, &displacement, symbol)) {
                    // Append module name and symbol to the stack trace
                    ss << "  " << moduleFileName << "!" << symbol->Name;
                    // Append source file location
                    IMAGEHLP_LINE64 line;
                    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                    DWORD dis;
                    if (SymGetLineFromAddr64(currentProcess, stackFrame.AddrPC.Offset, &dis, &line)) {
                        ss << "    [" << line.FileName << ":" << line.LineNumber << "]";
                    };
                    ss << std::endl;
                }
                else {
                    // Append module name and address to the stack trace
                    ss << "  " << moduleFileName << " + " << stackFrame.AddrPC.Offset - moduleBase << std::endl;
                }
            }
            else {
                // Append address to the stack trace
                ss << "  " << std::hex << stackFrame.AddrPC.Offset << std::endl;
            }
        }

        SymCleanup(currentProcess);
    }

    LONG WINAPI UnhandledHandler(EXCEPTION_POINTERS* ExceptionInfo) {
        DWORD exceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;

        Logger::Log("ERROR", "Exception thrown");
        Logger::Log("ERROR", std::format("Exception Code: 0x{:x}", exceptionCode));


        auto now = std::chrono::system_clock::now();
        std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
        tm timeZone;
        localtime_s(&timeZone, &timestamp);

        std::stringstream timeStringStream;
        timeStringStream << std::put_time(&timeZone, "%Y-%m-%d-%H-%M-%S");

        fs::path crashesFolder = SettingsHelper::GetAppPath() / "Crashes" / timeStringStream.str();
        if (!fs::exists(crashesFolder)) {
            fs::create_directories(crashesFolder);
        }

        fs::path stackFilePath = crashesFolder / "StackTrace.log";

        Logger::Log("ERROR", std::format("Crash stack trace located at: {}", stackFilePath.string()));

        std::ofstream crashLogFile(stackFilePath, std::ios::out);
        if (!crashLogFile.is_open()) {
            Logger::Log("ERROR", "Failed to open crash log file for writing");
            return static_cast<LONG>(ExitMode);
        };

        std::stringstream crashStream;
        crashStream << std::format("Exception: (0x{:x})\n", exceptionCode);

        crashStream << "== Stack Trace ==\n";
        Stacktrace(ExceptionInfo->ContextRecord, crashStream);

        crashLogFile << crashStream.str();
        crashLogFile.close();

        Logger::Log("INFO", "Deleted settings to prevent further errors");
        SettingsHelper::Delete();

        return static_cast<LONG>(ExitMode);
    };

    void Init(ExitModes pExitMode = ExitModes::Silent) {
        ExitMode = pExitMode;

        SetUnhandledExceptionFilter(UnhandledHandler);
    }

    void Disable() {
        SetUnhandledExceptionFilter(NULL);
    }
};