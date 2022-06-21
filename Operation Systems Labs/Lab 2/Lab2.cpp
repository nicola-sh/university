#include <iostream>
#include <Windows.h>

using namespace std;
//	Написать программу для консольного процесса, который состоит из трех потоков : main,
//	min_max и average.
//	Поток main должен выполнить следующие действия :
//		1. Создать массив целых чисел, размерность и элементы которого вводятся с консоли.
//		2. Создать потоки min_max и average.
//		3. Дождаться завершения потоков min_max и average.
//		4. Заменить максимальный и минимальный элементы массива на среднее значение элементов
//	массива.Вывести полученные результаты на консоль.
//		5. Завершить работу.
//	Поток min_max должен выполнить следующие действия :
//		1. Найти минимальный и максимальный элементы массива и вывести их на консоль.После
//			каждого сравнения элементов «спать» 7 миллисекунд.
//		2. Завершить свою работу.
//			Поток average должен выполнить следующие действия :
//		1. Найти среднее арифметическое значение элементов массива и вывести его на консоль.
//			После каждой операции суммирования элементов «спать» 12 миллисекунд.
//	2. Завершить свою работу.
//Примечания.
//		1. Для ожидания завершения работы потоков min_max и average использовать функцию :
//		DWORD WaitForSingleObject(
//			HANDLE hHandle, // дескриптор объекта
//			DWORD dwMilliseconds // интервал ожидания в миллисекундах
//		);
//	где второй параметр установить равным INFINITE.Например
//	WaitForSingleObject(hAverage, INFINITE); // ждать завершения потока
//	Здесь hAverage – дескриптор потока average.
//		2. Для засыпания использовать функцию :
//		VOID Sleep(
//			DWORD dwMilliseconds // миллисекунды
//		);
//	Например,
//	Sleep(12); // спать 12 миллисекунд


DWORD WINAPI min_max(LPVOID array) { //(ук)
	int* arr = (int*)array;
	int size = arr[0];
	int min = arr[1], max = arr[1];
	for (int i = 2; i <= size; i++) {
		if (arr[i] < min) {
			min = arr[i];
		}
		Sleep(7);
		if (arr[i] > max) {
			max = arr[i];
		}
		Sleep(7);
	}
	cout << "minimal and maximum elements: " << min << "   " << max << endl << endl;
	arr[size + 1] = min;
	arr[size + 2] = max;
	return 0;
}

DWORD WINAPI average(LPVOID array) {
	int* arr = (int*)array;
	int size = arr[0];
	int sum = 0;
	for (int i = 1; i <= size; i++) {
		sum += arr[i];
		Sleep(12);
	}
	arr[size + 3] = sum / size;
	cout << "average elements: " << arr[size + 3] << endl;
	return 0;
}

int main() {
	int* array;
	int n;
	cin >> n;
	array = new int[n + 4];
	array[0] = n;
	for (int i = 1; i <= n; i++) {
		cin >> array[i];
	}
	HANDLE hMin_max, hAverage;
	DWORD IDMin_max, IDAverage;
	hMin_max = CreateThread(NULL, 0, min_max, (void*)array, NULL, &IDMin_max);
	hAverage = CreateThread(NULL, 0, average, (void*)array, NULL, &IDAverage);
	WaitForSingleObject(hMin_max, INFINITE);
	WaitForSingleObject(hAverage, INFINITE);
	CloseHandle(hMin_max);
	CloseHandle(hAverage);
	for (int i = 1; i <= n; i++) {
		if (array[i] == array[n + 1] || array[i] == array[n + 2]) {
			array[i] = array[n + 3];
		}
	}
	for (int i = 1; i <= n; i++) {
		cout << array[i] << " ";
	}
	system("pause");
	return 0;
}