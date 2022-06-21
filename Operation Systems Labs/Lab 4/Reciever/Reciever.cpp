#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>
#include<vector>

using namespace std;

int main() {
	wstring fileName;
	cout << "Enter file name: ";
	wcin >> fileName;

	fstream file(fileName, ios::binary | ios::out);

	int messagesNumber;
	cout << "Enter number of messages: ";
	cin >> messagesNumber;

	int sendersNumber;
	cout << "Enter number of Senders:";
	cin >> sendersNumber;

	HANDLE* senders = new HANDLE[sendersNumber];
	HANDLE* events = new HANDLE[sendersNumber];

	HANDLE mutex = CreateMutex(NULL, FALSE, L"mutex");
	HANDLE writeSem = CreateSemaphore(NULL, messagesNumber, messagesNumber, L"writeSem");
	HANDLE readSem = CreateSemaphore(NULL, 0, messagesNumber, L"readSem");

	for (int i = 0; i < sendersNumber; ++i) {
		wstring args = L"Sender.exe " + fileName + L" " + to_wstring(messagesNumber) + L" " + to_wstring(i);

		HANDLE event = CreateEvent(NULL, FALSE, FALSE, (L"start" + to_wstring(i)).c_str());
		events[i] = event;

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);

		if (!CreateProcess(NULL, (LPWSTR)args.c_str(), NULL, NULL,
			FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
			return NULL;
		}

		senders[i] = pi.hProcess;

		if (senders[i] == NULL) {
			cout << "Error while creating process";
			return -1;
		}
	}

	WaitForMultipleObjects(sendersNumber, events, TRUE, INFINITE);

	ifstream fin{};
	string message;
	string tmpMessages;

	int req;

	while (true) {
		cout << "Enter: 1 to read or 0 to exit" << "\n";
		cin >> req;

		if (req != 0 && req != 1) {
			cout << "Error. Enter 1 or 0!";
			continue;
		}
		if (req == 0) {
			break;
		}
		WaitForSingleObject(readSem, INFINITE);
		WaitForSingleObject(mutex, INFINITE);

		fin.open(fileName, ios::binary);
		getline(fin, message);

		cout << "Message: " << message << '\n';

		while (getline(fin, message)) {
			tmpMessages += message + '\n';
		}
		fin.close();

		fstream fout(fileName, ios::binary | ios::out);
		fout << tmpMessages;

		ReleaseMutex(mutex);
		ReleaseSemaphore(writeSem, 1, NULL);

		tmpMessages = "";
	}

	for (int i = 0; i < sendersNumber; ++i) {
		CloseHandle(events[i]);
		CloseHandle(senders[i]);
	}
	delete[] events;
	delete[] senders;
	CloseHandle(mutex);
	CloseHandle(readSem);
	CloseHandle(writeSem);

	return 0;
}