#include <iostream>
#include <Windows.h>

struct thread_info {
    int size;
    int* array;
    int maker_index;
    HANDLE stop;
    HANDLE start;
    HANDLE* terminate_or_continue;
};

CRITICAL_SECTION cs;

void set_zeros(int* array, int size, int thread_index) {
    for (int i = 0; i < size; ++i) {
        if (array[i] == thread_index) { array[i] = 0; }
    }
}

DWORD WINAPI maker(LPVOID params) {
    thread_info info = *((thread_info*)params);
    bool end_thread = false;
    int number_of_marked_elements = 0;
    srand(info.maker_index);

    WaitForSingleObject(info.start, INFINITE);

    while (!end_thread) {
        int ind = rand() % info.size;
        EnterCriticalSection(&cs);
        if (info.array[ind] == 0) {
            Sleep(5);
            info.array[ind] = info.maker_index;
            LeaveCriticalSection(&cs);
            number_of_marked_elements++;
            Sleep(5);
        }
        else {
            printf("%s %d%s %d%s %d\n","Thread",info.maker_index,", number of marked elements:",number_of_marked_elements,", can't mark element with index",ind);
            LeaveCriticalSection(&cs);
            SetEvent(info.stop);
            //waiting for main response
            int k = WaitForMultipleObjects(2, info.terminate_or_continue, FALSE, INFINITE) - WAIT_OBJECT_0;
            if (k== 0) {
                end_thread = true;
            }
        }
    }
    set_zeros(info.array, info.size, info.maker_index);
    return 0;
}

void print_array(int* array, int size) {
    printf("%s ","Array:");
    for (int i = 0; i < size; ++i) {
        printf(" %d",array[i]);
    }
    printf("\n");
}

int main() {
    InitializeCriticalSection(&cs);

    int size;
    printf("%s","Enter array size:");
    scanf("%d",&size);
    int* array = new int[size];
    for (int i = 0; i < size; ++i) {
        array[i] = 0;
    }
    printf("%s ","Enter number marker of threads:");
    int num_threads;
    scanf("%d",&num_threads);

    HANDLE* threads = new HANDLE[num_threads];
    thread_info* information = new thread_info[num_threads];
    bool* terminated_threads = new bool[num_threads];
    HANDLE start_work = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE* stopped_threads = new HANDLE[num_threads];

    for (int i = 0; i < num_threads; ++i) {
        information[i].array = array;
        information[i].size = size;
        information[i].maker_index = i + 1;
        information[i].start = start_work;
        stopped_threads[i] = information[i].stop = CreateEvent(NULL, TRUE, FALSE, NULL);
        information[i].terminate_or_continue = new HANDLE[2];
        information[i].terminate_or_continue[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
        information[i].terminate_or_continue[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
        threads[i] = CreateThread(NULL, 0, maker, (LPVOID)(&information[i]), NULL, NULL);
        terminated_threads[i] = false;
    }
    SetEvent(start_work);

    int ended_threads = 0;

    while (ended_threads != num_threads) {
        WaitForMultipleObjects(num_threads, stopped_threads, TRUE, INFINITE);
        print_array(array, size);
        bool is_thread_terminated = false;
        while (!is_thread_terminated) {
            int thread_to_terminate_ind;
            printf("%s ","Enter index of thread to be terminated(starts with 1):");
            scanf("%d",&thread_to_terminate_ind);
            thread_to_terminate_ind--;
            if (thread_to_terminate_ind >= num_threads || thread_to_terminate_ind < 0) {
                printf("%s\n","No thread with such index");
                continue;
            }
            if (terminated_threads[thread_to_terminate_ind]) {
                printf("%s\n","Thread is already terminated");
            }
            else {
                SetEvent(information[thread_to_terminate_ind].terminate_or_continue[0]);
                WaitForSingleObject(threads[thread_to_terminate_ind], INFINITE);
                print_array(array, size);
                terminated_threads[thread_to_terminate_ind] = true;
                is_thread_terminated = true;
                ended_threads++;
            }
        }

        for (int j = 0; j < num_threads; ++j) {
            if (!terminated_threads[j]) {
                ResetEvent(information[j].stop);
                SetEvent(information[j].terminate_or_continue[1]);
            }
        }
    }
    printf("%s\n","All threads are terminated");

    CloseHandle(start_work);
    for (int i = 0; i < num_threads; ++i) {
        CloseHandle(threads[i]);
        CloseHandle(stopped_threads[i]);
        CloseHandle(information[i].terminate_or_continue[0]);
        CloseHandle(information[i].terminate_or_continue[1]);
    }
    for (int i = 0; i < num_threads; ++i) {
        delete[] information[i].terminate_or_continue;
    }
    delete[] information;
    delete[] terminated_threads;
    delete[] array;
    delete[] threads;
    delete[] stopped_threads;

    DeleteCriticalSection(&cs);

    return 0;
}
