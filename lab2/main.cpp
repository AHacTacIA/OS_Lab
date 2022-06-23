#include <windows.h>
#include <iostream>

struct info {
    int min;
    int max;
    int* array = NULL;
    int size = 0;
    double average;
};

DWORD WINAPI Min_max(LPVOID a) {
info* mm = (info*)a;
int min,max;
min = 0;
max = 0;
for (int i = 1; i < mm->size; i++)
{
if (mm->array[max] < mm->array[i]) {
max = i;
}Sleep(7);
if (mm->array[min] > mm->array[i]) {
min =i;
}Sleep(7);
mm->min=min;
mm->max=max;
}
printf("%s %d\n%s %d\n","Min:",mm->array[mm->min],"Max:",mm->array[mm->max]);
return 0;
}

DWORD WINAPI Average(LPVOID a) {
info* av = (info*)a;
int sum=0;
for (int i = 0; i < av->size; i++) {
sum += av->array[i];
Sleep(12);
}
av->average = (double)sum / (av->size);
printf("Average: %f\n", av->average);
return 0;
}

int main()
{
    int size;
    printf("Input array size:");
    scanf("%d", &size);
    int* array = new int[size];
    printf("Input elements of array:");
    for (int i = 0; i < size; i++) {
        scanf("%d", &array[i]);
    }

    struct info* a = new info;
    a->array = array;
    a->size = size;

    DWORD IDThread_mm;
    HANDLE hThread_mm = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Min_max, (LPVOID)a, 0, &IDThread_mm);
    if (hThread_mm == NULL)
        return GetLastError();

    DWORD IDThread_av;
    HANDLE hThread_av = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Average, (LPVOID)a, 0, &IDThread_av);
    if (hThread_av == NULL)
        return GetLastError();

    WaitForSingleObject(hThread_mm, INFINITE);
    WaitForSingleObject(hThread_av, INFINITE);

    CloseHandle(hThread_mm);
    CloseHandle(hThread_av);

    array[a->max]=a->average;
    array[a->min]=a->average;
    for (int i = 0; i < size; i++) {
        printf("%d ",array[i]);
    }
    delete[] array;
    delete a;
    return 0;
}

