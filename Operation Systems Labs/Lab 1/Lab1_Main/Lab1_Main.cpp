#include <iostream>	//директива для подключения файла(библиотеки где хранится описание функций(cin, cout))
#include <Windows.h>//граф интерф, работа с файлами, работа с сетью и звуком
#include <string>
#include <fstream>
#include "../Employee.h"

using namespace std;//область кода в которой гарантируется уникальность

//Программа Main выполняет следующие действия :
//	1. Запрашивает с консоли имя бинарного файла и количество записей в этом файле.
//	2. Запускает утилиту Creator, которой передает через командную строку имя
//	создаваемого бинарного файла и количество записей в этом файле.
//	3. Ждет завершения работы утилиты Creator.
//	4. Выводит содержимое созданного бинарного файла на консоль.
//	5. Запрашивает с консоли :
//			a.Имя файла отчета.
//			b.Оплату за час работы.
//	6. Запускает утилиту Reporter, которой передает через командную строку :
//			a.Имя исходного бинарного файла.
//			b.Имя файла отчета.
//			c.Оплату за час работы.
//	7. Ждет завершения работы утилиты Reporter.
//	8. Выводит отчет на консоль.
//	9. Завершает свою работу.

//Для ожидания завершения работы процессов Creator и Reporter использовать функцию WaitForSingleObject


void run_creator(string fileName, int numOfRec) { //общий указатель
	const wchar_t* appName = L"..\\Release\\Creator.exe"; //указатель на char Creator
	string name_with_params = fileName + " " + to_string(numOfRec);

	STARTUPINFO si;					//	Структура
	PROCESS_INFORMATION piApp;		//	заполняется СreateProcess информацией о созданном окне (Дескриптор недавно созданного процесса,
									//	Дескриптор первичного потока недавно созданного процесса, два значения для идентификации)

	ZeroMemory(&si, sizeof(STARTUPINFO));	//заполнение блока памяти нулями можно использовать Сишный memset(любыми числами)
	si.cb = sizeof(STARTUPINFO);			// указание размера

	if (!CreateProcess(appName, (LPWSTR)name_with_params.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp)) {
		cout << "Error: process Creator hasn't been created!\n";
	}

	WaitForSingleObject(piApp.hProcess, INFINITE);

	CloseHandle(piApp.hThread);		//ДПр
	CloseHandle(piApp.hProcess);	//Дпот
}

void run_reporter(string fileName, string reportFileName, int moneyPerHour) {
	const wchar_t* appName = L"..\\Release\\Reporter.exe";
	string name_with_params = fileName + " " + reportFileName + " " + to_string(moneyPerHour);

	STARTUPINFO si;
	PROCESS_INFORMATION piApp;

	ZeroMemory(&si, sizeof(STARTUPINFO)); //заполнение блока памяти нулями можно использовать Сишный memset(любыми числами)
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(appName, (LPWSTR)name_with_params.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp)) {
		cout << "Error: process Reporter hasn't been created!\n";
	}

	//где второй параметр установить равным INFINITE, например
	//WaitForSingleObject(hCreator, INFINITE);
	//здесь hCreator – дескриптор процесса Creator.
	WaitForSingleObject(piApp.hProcess, INFINITE);	//HANDLE hHandle, -- дескриптор объекта
													//DWORD dwMilliseconds -- интервал ожидания в миллисекундах

	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);
}

int main()
{
	setlocale(LC_ALL, "Russian");
	cout << "Input name of binary file: ";
	string fileName;
	cin >> fileName;

	cout << "Enter the number of records: ";
	int numOfRec;
	cin >> numOfRec;

	run_creator(fileName, numOfRec);

	ifstream binFile(fileName, ios::binary);
	employee record;
	while (binFile.read((char*)&record, sizeof(employee))) {
		cout << record.num << "\t" << record.name << "\t" << record.hours << endl;
	}
	binFile.close();

	string reportFileName;
	cout << "Input name of report file: ";
	cin >> reportFileName;

	int moneyPerHour;
	cout << "Input money per hour: ";
	cin >> moneyPerHour;


	run_reporter(fileName, reportFileName, moneyPerHour);

	ifstream reportFile(reportFileName);
	string recordFromReport;
	while (getline(reportFile, recordFromReport)) {
		cout << recordFromReport << endl;
	}
	reportFile.close();
	system("pause");
	return 0;
}