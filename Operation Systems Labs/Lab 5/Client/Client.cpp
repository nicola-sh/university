#include <iostream>
#include <Windows.h>

#include "../Server/employee.h"
#include "../Server/utils.h"

int main() {
    HANDLE hPipe;

    while (true) {
        hPipe = CreateFile(L"\\\\.\\pipe\\my_pipe", GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }
        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "Could not open pipe: " << GetLastError();
            system("pause");
            return -1;
        }
        if (!WaitNamedPipe(L"\\\\.\\pipe\\my_pipe", 20000)) {
            std::cerr << "could not open pipe: 20 second wait timeout\n";
            system("pause");
            return -1;
        }
    }
    char input;
    DWORD bytes_written, bytes_read;
    employee empl;
    int id;
    std::cout << "Press m for modification, r for read, q to exit process: ";

    while (std::cin >> input) {
        if (!WriteFile(hPipe, &input, 1, &bytes_written, NULL)) {
            std::cerr << "Error writing to pipe: " << GetLastError();
            return -1;
        }
        if (input != 'm' && input != 'r') {
            break;
        }
        std::cout << "Enter id: ";
        std::cin >> id;
        if (!WriteFile(hPipe, &id, sizeof(int), &bytes_written, NULL) || bytes_written != sizeof(int)) {
            std::cerr << "Error writing to pipe: " << GetLastError();
            return -1;
        }
        if (!ReadFile(hPipe, &empl, sizeof(employee), &bytes_read, NULL) || bytes_read != sizeof(employee)) {
            std::cerr << "Error reading from pipe: " << GetLastError();
            return -1;
        }
        std::cout << empl << '\n';
        if (input == 'm') {
            std::cout << "Enter new values(name, hours):\n";
            std::cin >> empl.name >> empl.hours;
            if (!WriteFile(hPipe, &empl, sizeof(employee), &bytes_written, NULL)) {
                std::cerr << "Error writing to pipe: " << GetLastError();
                return -1;
            }
        }

        std::cout << "Press m for modification, r for read, q to exit process: ";
    }
    CloseHandle(hPipe);
}