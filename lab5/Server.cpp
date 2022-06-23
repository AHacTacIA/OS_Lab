#include <iostream>
#include <windows.h>
#include <fstream>
#include <codecvt>
#include <string>
#include "employee.h"
#include "message.h"

using namespace std;

int procNum, recordsNum;
    fstream file;
    employee emp;
    HANDLE* processes;
    const wchar_t* pipeName = L"\\\\.\\pipe\\named_pipe";

    void thread(HANDLE hNamedPipe);

int main()
{
    

    string f1;
    cout << "File name\n";
    cin >> f1;
    cout << "Enter the number of employees\n";
    cin >> recordsNum;
    file = fstream(f1, ios::trunc | ios::binary | ios::in | ios::out);


    HANDLE* readAccess = new HANDLE[recordsNum];
    HANDLE* writeAccess = new HANDLE[recordsNum];

    for (int i = 0; i < recordsNum; i++) {
        cout << "Enter employee number\n";
        cin >> emp.num;
        cout << "Enter employee name\n";
        cin >> emp.name;
        cout << "Enter the number of hours worked\n";
        cin >> emp.hours;
        file.write((char*)&emp, sizeof(emp));
        readAccess[i] = CreateEvent(NULL, 1, 1, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("r" + to_string(emp.num)).c_str());
        writeAccess[i] = CreateEvent(NULL, 1, 1, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("w" + to_string(emp.num)).c_str());
    }
    cout << "\n" << f1 << "\n";
    file.clear();
    file.seekg(0);
    for (int i = 0; i < recordsNum; i++) {
        file.read((char*)&emp, sizeof(emp));
        cout << emp.num << "\n" << emp.name << '\n' << emp.hours << "\n\n";
    }


    cout << "Enter the number of processes\n";
    cin >> procNum;

    STARTUPINFO* si = new STARTUPINFO[procNum];
    PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[procNum];
    processes = new HANDLE[procNum];
    ZeroMemory(si, procNum * sizeof(si[0]));
    for (int i = 0; i < procNum; i++) {
        si[i].cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("Client.exe").c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &(si[i]), &(pi[i]));
        processes[i] = pi[i].hProcess;
    }

    DWORD* idthread = new DWORD[procNum];
    HANDLE* threads = new HANDLE[procNum];
    for (int i = 0; i < procNum; i++) {
        HANDLE hNamedPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, procNum, 0, 0, INFINITE, (LPSECURITY_ATTRIBUTES)NULL);
        threads[i] = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(thread), (void*)hNamedPipe, 0, &idthread[i]);
    }

    WaitForMultipleObjects(procNum, threads, true, INFINITE);

    system("pause");
}

void thread(HANDLE hNamedPipe) {
    ConnectNamedPipe(hNamedPipe, NULL);

    while (WaitForMultipleObjects(procNum, processes, true, 0)) {
        message msg;
        DWORD dwRead;
        ReadFile(hNamedPipe, (char*)&msg, sizeof(msg), &dwRead, NULL);
        file.clear();
        file.seekg(0);
        int pos;
        for (int i = 0; i < recordsNum; i++) {
            pos = file.tellg();
            file.read((char*)&emp, sizeof(emp));
            if (emp.num == msg.id) {
                break;
            }
        }
        WriteFile(hNamedPipe, &emp, sizeof(emp), &dwRead, NULL);
        if (msg.type == 'w') {
            ReadFile(hNamedPipe, &emp, sizeof(emp), &dwRead, NULL);
            for (int i = 0; i < recordsNum; i++) {
                file.clear();
                file.seekg(pos);
                file.write((char*)&emp, sizeof(emp));
            }
        }
    }
}