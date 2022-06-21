#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>

using namespace std;

int main(int argc, char* argv[]) {
	HANDLE start = OpenEvent(EVENT_MODIFY_STATE, FALSE, (L"start" + to_wstring(atoi(argv[3]))).c_str());
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"mutex");
	HANDLE writeSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"writeSem");
	HANDLE readSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"readSem");

	SetEvent(start);
	string fileName = argv[1];
	fstream file;

	int req;

	while (true) {
		cout << "Enter\n1 - write to file\n0 - close" << "\n";
		cin >> req;
		if (req != 0 && req != 1) {
			cout << "Try again!" << "\n";
			continue;
		}
		if (req == 0) {
			break;
		}

		cin.ignore();

		string message;
		cout << "Enter message: ";

		getline(cin, message);

		WaitForSingleObject(writeSem, INFINITE);
		WaitForSingleObject(mutex, INFINITE);

		file.open(fileName, ios::binary | ios::app);
		file << message + "\n";
		file.close();

		ReleaseMutex(mutex);
		ReleaseSemaphore(readSem, 1, NULL);
	}

	return 0;
}