#include <windows.h>
#include <fstream>
#include <iostream>
#include "..//employee.h"
using namespace std;


int main(int argc, char* argv[])
{
    ofstream out(argv[1], ios::binary | ios::out);
    employee emp;
    for (int i = 0; i < atof(argv[2]); i++) {
        cout << "Enter employee number\n";
        cin >> emp.num;
        cout << "Enter the employee's name\n";
        cin >> emp.name;
        cout << "Enter the number of hours worked\n";
        cin >> emp.hours;
        out.write((char*)&emp, sizeof(emp));
    }
    out.close();
    return 0;
}