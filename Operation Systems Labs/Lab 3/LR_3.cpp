#include <windows.h>
#include <iostream>
#include<vector>
#include<string>
HANDLE *hOutEvent, hAddEvent;
HANDLE hSemaphore;

struct fun {
	int i=0;
	int size=0;
	int* vec=NULL;
};

int* flags = NULL;

DWORD WINAPI marker(LPVOID aa) {
	ReleaseSemaphore(hSemaphore, 1, NULL);
	WaitForSingleObject(hSemaphore, INFINITE);
	fun* a = (fun*)aa;

	//	std::cout << a->i << "\n" ;
	std::vector<int> positions;
   start: std::srand(a->i);
	while (true) {
		int k = std::rand() % a->size;
		if (a->vec[k] == 0) {
			Sleep(5);
			a->vec[k] = a->i;
			Sleep(5);
			if(std::find(positions.begin(), positions.end(), k) == positions.end()) 	positions.push_back(k);
		}
		else {
			std::string s = "number of thread : " + std::to_string(a->i) + " positions.size:  " + std::to_string(positions.size()) +" position wrong: "+ std::to_string(k) + "\n";
			std::cout <<s;
			break;
		}

	}
	flags[a->i] = 1;
	while (true) {
		Sleep(200);
		if (flags[a->i] == 2) break;
		if (flags[a->i] == 0) goto start;

	}
	for (int i = 0; i < positions.size(); i++) {
		a->vec[positions[i]] = 0;
	}
	flags[a->i] = 1;
	return 0;
}

int main() {

	int size;
	std::cout << "Write size of array";
	std::cin >> size;

	int* vec = new int[size];
	for (int i = 0; i < size; i++) {
		vec[i] = 0;
	}
	int number;
	std::cout << "Write number of threads marker";
	std::cin >> number;
	hSemaphore = CreateSemaphore(NULL, 0, number, NULL);

	flags = new int[number];
	for (int i = 0; i < number; i++) {
		flags[i] = 0;
	}

    HANDLE* hThread = new HANDLE[number];
	for (int i = 0; i < number; i++) {
		struct fun* a = new fun;
		a->i = i;
		a->size = size;
		a->vec = vec;
		HANDLE hThread_i;
		DWORD IDThread_i;
		hThread_i = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)marker, a, 0, &IDThread_i);
		hThread[i] = hThread_i;
		delete a;
	}
	
	while (true) {
		int number_stop = 0;
       for (int i = 0; i <number; i++) {
	       if (flags[i] == 1) number_stop++;
	    }
       if (number_stop == number) break;
	}
	

	for (int i = 0; i < size; i++) {
		std::cout << vec[i] << " ";
	}

    int number_stop;
	std::cout << "Press number under " << number<<std::endl;
	std::cin >> number_stop;
	flags[number_stop] = 2;
	while (true) {
		int number_stop = 0;
		for (int i = 0; i < number; i++) {
			if (flags[i] == 1) number_stop++;
		}
		if (number_stop == number) break;
	}
	for (int i = 0; i < size; i++) {
		std::cout << vec[i] << " ";
	}

	for (int i = 0; i < number; i++) {
		if (i != number_stop) flags[i]=0;
	}
	
	delete[] vec;
	
	return 0;
}