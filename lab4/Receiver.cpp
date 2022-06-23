#include<windows.h>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <string>
using namespace std;

int main()
{
    cout << "File name:\n";
    string file;
    cin >> file;
    ofstream out(file, ios::binary | ios::out);
    out.close();
    ifstream in(file, ios::binary | ios::in);
    cout << "Records maximum number:\n";
    int record;
    cin >> record;
    cout << "Processes number:\n";
    int process;
    cin >> process;

    HANDLE semProcess = CreateSemaphore(NULL, -process, 0, L"ready");
    HANDLE semRec = CreateSemaphore(NULL, record, record, L"isFilled");

    STARTUPINFO* si = new STARTUPINFO[process];
    PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[process];
    ZeroMemory(si, process * sizeof(si[0]));
    for (int i = 0; i < process; i++) {
        si[i].cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("Sender.exe " + file + " " + to_string(process)).c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &(si[i]), &(pi[i]));
    }

    WaitForSingleObject(semProcess, INFINITE);

    HANDLE hEvent = CreateEvent(NULL, true, false, L"message");

    while (true) {
        cout << "1 - read a message\n2 - exit\n";
        int answer;
        cin >> answer;
        if (answer == 1) {
            unsigned strLen;
            in.read((char*)&strLen, sizeof(unsigned));
            if (in.eof()) {
                cout << "There are no messages.\n";
                in.clear();
                in.close();
                return 0;
            }
            else {
                string message;
                message.resize(strLen);
                in.read((char*)message.data(), strLen);
                ReleaseSemaphore(semRec, 1, NULL);
                cout << message << '\n';
            }
        }
        else {
            in.close();
            return 0;
        }
    }
}