#include <iostream>
#include <Windows.h>
#include <vector>
#include <fstream>

#include "employee.h"
#include "utils.h"

DWORD WINAPI server(LPVOID arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    HANDLE hPipe = CreateNamedPipe(L"\\\\.\\pipe\\my_pipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        args->number_of_clients,
        0,
        0,
        INFINITE,
        NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error creating pipe: " << GetLastError() << '\n';
        return -1;
    }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    std::wstring cmd = L"Client.exe";
    if (!CreateProcess(NULL, (LPWSTR)cmd.c_str(), NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        std::cerr << "Error creating process: " << GetLastError() << '\n';
        return -1;
    }
    
    if (ConnectNamedPipe(hPipe, NULL)) {
        int id;
        DWORD bytes_read, bytes_written;
        employee empl;
        std::fstream s;
        char mode;

        while (true) {
            if (!ReadFile(hPipe, &mode, 1, &bytes_read, NULL) || bytes_read != 1) {
                std::cerr << "Error reading from pipe: " << GetLastError() << '\n';
                return -1;
            }
            if (mode != 'm' && mode != 'r') {
                break;
            }
            if (!ReadFile(hPipe, &id, sizeof(int), &bytes_read, NULL)) {
                std::cerr << "Error reading from pipe: " << GetLastError() << '\n';
                return -1;
            }

            switch (mode) {
            case 'r':
                WaitForSingleObject(args->read_events[id], INFINITE);
                ResetEvent(args->modify_events[id]);
                SetEvent(args->read_events[id]);

                WaitForSingleObject(args->mutexes[id], INFINITE);

                ++args->read_section_counter[id];

                ReleaseMutex(args->mutexes[id]);

                s.open(args->filename, std::ios_base::in | std::ios_base::binary);
                s.seekg(id * sizeof(employee), std::ios_base::_Seekbeg);
                s.read((char*)&empl, sizeof(employee));
                s.close();
                if (!WriteFile(hPipe, &empl, sizeof(employee), &bytes_written, NULL) || bytes_written != sizeof(employee)) {
                    std::cerr << "Error writing to pipe: " << GetLastError() << '\n';
                    return -1;
                }

                WaitForSingleObject(args->mutexes[id], INFINITE);

                if (--args->read_section_counter[id] <= 0) {
                    SetEvent(args->modify_events[id]);
                }

                ReleaseMutex(args->mutexes[id]);
                break;
            case 'm':
                WaitForSingleObject(args->read_events[id], INFINITE);
                WaitForSingleObject(args->modify_events[id], INFINITE);
                
                s.open(args->filename, std::ios_base::in | std::ios_base::binary);
                s.seekg(id * sizeof(employee), std::ios_base::_Seekbeg);
                s.read((char*)&empl, sizeof(employee));
                s.close();
                if (!WriteFile(hPipe, &empl, sizeof(employee), &bytes_written, NULL) || bytes_written != sizeof(employee)) {
                    std::cerr << "Error writing to pipe: " << GetLastError() << '\n';
                    return -1;
                }

                if (!ReadFile(hPipe, &empl, sizeof(employee), &bytes_read, NULL)) {
                    std::cerr << "Error reading from pipe: " << GetLastError() << '\n';
                    return -1;
                }
                s.open(args->filename, std::ios_base::out | std::ios_base::in | std::ios_base::binary);
                s.seekg(id * sizeof(employee));
                s.write((char*)&empl, sizeof(employee));
                s.close();

                SetEvent(args->read_events[id]);
                SetEvent(args->modify_events[id]);
                break;
            }
        }
    } else {
        std::cerr << "error connecting to client: " << GetLastError() << '\n';
        return -1;
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

int main() {

    std::string filename;
    std::cout << "Enter filename: ";
    std::cin >> filename;
    std::fstream fstream(filename, std::ios_base::out | std::ios_base::binary);
    int rnum;
    std::cout << "Enter number of records: ";
    std::cin >> rnum;
    std::cout << "Enter records:(name, hours)\n";
    std::vector<HANDLE> e_read(rnum);
    std::vector<HANDLE> e_modify(rnum);
    employee temp;
    for (int i = 0; i < rnum; ++i) {
        std::cin >> temp.name >> temp.hours;
        temp.num = i;
        fstream.write((char*)&temp, sizeof(employee));
        e_read[i] = CreateEvent(NULL, false, true, L"");
        e_modify[i] = CreateEvent(NULL, false, true, L"");

    }
    fstream.close();
    fstream.open(filename, std::ios_base::in | std::ios_base::binary);
    std::cout << "File contains: \n";
    for (int i = 0; i < rnum; ++i) {
        fstream.read((char*)&temp, sizeof(temp));
        std::cout << temp << '\n';
    }
    fstream.close();

    size_t n;
    std::cout << "enter number of clients: ";
    std::cin >> n;
    std::vector<HANDLE> threads(n);
    std::vector<HANDLE> mutexes(rnum);
    for (int i = 0; i < rnum; ++i) {
        mutexes[i] = CreateMutex(NULL, false, L"");
    }
    ThreadArgs args(n, filename, e_read, e_modify, mutexes);
    for (int i = 0; i < n; ++i) {
        threads[i] = CreateThread(NULL, NULL, server, &args, 0, NULL);
    }

    WaitForMultipleObjects(n, &threads[0], true, INFINITE);

    fstream.open(filename, std::ios_base::in | std::ios_base::binary);
    std::cout << "Modified file contains: \n";
    for (int i = 0; i < rnum; ++i) {
        fstream.read((char*)&temp, sizeof(temp));
        std::cout << temp << '\n';
    }
    fstream.close();

    for (HANDLE h : threads) {
        CloseHandle(h);
    }

    for (int i = 0; i < rnum; ++i) {
        CloseHandle(mutexes[i]);
        CloseHandle(e_read[i]);
        CloseHandle(e_modify[i]);
    }
}