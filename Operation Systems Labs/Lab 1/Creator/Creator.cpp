#include <iostream>
#include <fstream>
#include "..\Employee.h"
using namespace std;

//	Утилита Creator создает бинарный файл
//	имя которого и количество записей в котором получает через командную строку
// 
//1. Утилита получает через командную строку следующие данные :
//		1.1.Имя исходного бинарного файла.
//		1.2.Имя файла отчета.
//		1.3.Оплата за час работы.
//2. Отчет содержит следующие строки :
//		2.1.Заголовок : Отчет по файлу «имя исходного бинарного файла».
//		2.2.Строка : Номер сотрудника, имя сотрудника, часы, зарплата.
//		2.3.Список сотрудников по строкам.Каждая строка содержит информацию из
//		соответствующей записи бинарного файла, а последний столбец строки -
//		начисленную зарплату = кол - ву отработанных часов * оплату за час работы.

int main(int argc, char* argv[]) {
	ofstream out(argv[0], ios::binary);
	int countOfRec = atoi(argv[1]);
	for (int i = 0; i < countOfRec; i++) {
		employee empl;
		cout << "Input employee id: ";
		cin >> empl.num;
		cout << "Input employee name: ";
		cin >> empl.name;
		cout << "Input worked hours: ";
		cin >> empl.hours;
		out.write((char*)&empl, sizeof(employee));
	}
	out.close();
}