#include <windows.h>
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    HANDLE hSemProcess = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"ready");
    HANDLE hSemRecord = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"isFilled");
    HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"message");
    ReleaseSemaphore(hSemProcess, 1, NULL);

    while (true) {
        cout << "1 - send a message\n2 - exit\n";
        int answer;
        cin >> answer;
        if (answer == 1) {
            if (WaitForSingleObject(hSemRecord, 0) != WAIT_OBJECT_0) {
                cout << "there is no space for a message. Wait...\n";
                WaitForSingleObject(hSemRecord, INFINITE);
            }

            cout << "message:\n";
            string message;
            string str;
            do {
                cin >> str;
                if (str.size() > 19) {
                    cout << "string length must be less than 20\n";
                }
            } while (str.size() > 19);

            message = str;
            unsigned strLen = message.size();
            ofstream out(argv[1], ios::binary | ios::out | ios::app);
            out.write((char*)&strLen, sizeof(unsigned));
            out.write((char*)message.c_str(), message.size());
            out.close();
            PulseEvent(hEvent);
        }
        else {
            return 0;
        }
    }
}