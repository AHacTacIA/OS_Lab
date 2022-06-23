#include <windows.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "..//employee.h"
using namespace std;


int main(int argc, char* argv[])
{
    ifstream in(argv[1], ios::binary | ios::in);
    ofstream out(argv[2]);
    out << "Report on the file " << argv[1] << "\n";
    out << "Employee number, employee name, hours, salary\n";
    employee emp;
    while (!in.eof()) {
        in.read((char*)&emp, sizeof(emp));
        if (!in.eof())
            out << emp.num << ", " << emp.name << ", " << emp.hours << ", " << emp.hours * atof(argv[3]) << '\n';
    }
    out.close();
    in.close();
    return 0;
}