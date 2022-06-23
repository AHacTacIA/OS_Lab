#include<windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <codecvt>
#include "employee.h"
using namespace std;


int main()
{
	STARTUPINFO si1;
	PROCESS_INFORMATION pi1;
	ZeroMemory(&si1, sizeof(STARTUPINFO));
	si1.cb = sizeof(STARTUPINFO);
	string str1 = "Creator.exe ";

	STARTUPINFO si2;
	PROCESS_INFORMATION pi2;
	ZeroMemory(&si2, sizeof(STARTUPINFO));
	si2.cb = sizeof(STARTUPINFO);
	string str2 = "Reporter.exe ";

	char file1[20];
	char file2[20];
	int recNum;
	cout << "Enter the name of the binary file\n";
	cin >> file1;
	cout << "Enter the number of records\n";
	cin >> recNum;
	CreateProcess(NULL, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(str1 + file1 + " " + to_string(recNum)).c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si1, &pi1);
	WaitForSingleObject(pi1.hProcess, INFINITE);
	CloseHandle(pi1.hThread);
	CloseHandle(pi1.hProcess);

	ifstream fin(file1, ios::binary | ios::in);
	employee emp;
	while (!fin.eof()) {
		fin.read((char*)&emp, sizeof(emp));
		if (!fin.eof())
			cout << emp.num << ", " << emp.name << ", " << emp.hours << '\n';
	}

	cout << "Enter report file name\n";
	cin >> file2;
	double payment;
	cout << "Enter pay per hour of work\n";
	cin >> payment;
	CreateProcess(NULL, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(str2 + file1 + " " + file2 + " " + to_string(payment)).c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si2, &pi2);
	WaitForSingleObject(pi2.hProcess, INFINITE);
	CloseHandle(pi2.hThread);
	CloseHandle(pi2.hProcess);
	fin.close();
	ifstream fin1(file2);
	char ch;
	while (!fin1.eof()) {
		fin1.read(&ch, 1);
		if (!fin1.eof()) {
			cout << ch;
		}
	}
	fin1.close();
	return 0;
}