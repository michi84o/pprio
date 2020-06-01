// pprio.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

bool abortProgram = false;
void PrintUsage();
BOOL WINAPI consoleHandler(DWORD signal);

int main(int argc, char* argv[])
{
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    std::cout << "pprio - Process Priority Changer" << std::endl << std::endl;

    std::string processName = "";
    bool boost = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = std::string(argv[i]);
        if (arg == "-p")
        {
            if (argc > i++)
            {
                processName = argv[i];
            }
        }     
        else if (arg == "-h")
        {
            boost = true;
        }
    }    

    if (processName == "")
    {
        std::cout << "You can specify a program name by using the command line parameter '-p'." << std::endl;
        std::cout << "For example:" << std::endl;
        std::cout << "\tpprio -p Discord.exe" << std::endl;
        std::cout << "Since you didn't do that, what program do you like to monitor?" << std::endl;

        std::cin >> processName;

        while (processName == "" && !abortProgram)
        {
            std::cout << "You did not input anything, please try again or press Ctrl+C to exit:" << std::endl;
            std::cin >> processName;
        }        
        std::cout << std::endl;
    }

    if (abortProgram) return -1;

    std::string prio = "Above normal";
    if (boost) prio = "High";
    DWORD prioValue = ABOVE_NORMAL_PRIORITY_CLASS;
    if (boost) prioValue = HIGH_PRIORITY_CLASS;


    std::cout << "Monitoring '"<< processName << "'" << std::endl << std::endl;
    std::cout << "Target priority: " << prio << std::endl << std::endl;

    HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBuffInfo;
    GetConsoleScreenBufferInfo(stdHandle, &screenBuffInfo);

    std::ios coutState(NULL);
    coutState.copyfmt(std::cout);

    std::wstring pName = std::wstring(processName.begin(), processName.end());

    int lastLineCount = 0;
    while (!abortProgram)
    {        
        bool fail = false;
        int lineCount = 0;

        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(snapshot, &entry) == TRUE)
        {
            SetConsoleCursorPosition(stdHandle, screenBuffInfo.dwCursorPosition);

            while (Process32Next(snapshot, &entry) == TRUE)
            {
                if (_wcsicmp(entry.szExeFile, pName.c_str()) == 0)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                    
                    DWORD pid = GetProcessId(hProcess);
                    ++lineCount;
                    std::cout << std::left << std::setw(6) << std::setfill(' ');
                    std::cout << pid;                    

                    if (!SetPriorityClass(hProcess, prioValue))
                    {
                        fail = true;
                        std::cout << "FAIL" << std::endl;                                    
                    }
                    std::cout << "OK" << std::endl;                                        
                    std::cout.copyfmt(coutState);

                    CloseHandle(hProcess);
                }
            }
        }

        CloseHandle(snapshot);
                
        int currentLineCount = lineCount;
        while (currentLineCount++ < lastLineCount)
        {
            std::cout << "                " << std::endl;
        }
        lastLineCount = lineCount;

        Sleep(5000);
    }    

    return 0;
}

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT)
    {
        abortProgram = true;
        exit(-1);
    }

    return TRUE;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
