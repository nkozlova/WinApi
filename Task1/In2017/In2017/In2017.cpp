// In2017.cpp: определяет точку входа для консольного приложения.

// Автор: Козлова Надежда

#include "stdafx.h"
#include <time.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <atlstr.h>
#include <cstdlib>


void printErrorMessage(DWORD error);
void freeMemory(std::vector<LPVOID>& ptrs);

int СountOfRecursionDepth;

void testEmptyFunction();
void testComplexFunction();

bool hasCreateFile(LPCTSTR fileName);
int getLengthOfFile(LPCTSTR fileName);


void task1(DWORD allocatorType);
void task2();
void task3();
void task3_();
void task4();
void task5();
void task6();
void task8();


int main()
{
	srand(time(NULL));
	setlocale(LC_ALL, "Russian");

	//task1(MEM_COMMIT);		
	/*
	Выделено 2032414720 битов памяти
	Последняя ошибка: Error(8): Недостаточно памяти для обработки команды
	*/
	//task1(MEM_RESERVE);
	/*
	Выделено 2029305856 битов памяти
	Последняя ошибка: Error(8): Недостаточно памяти для обработки команды
	*/
	//task2();
	/*
	Выделено 1358561280 битов памяти
	Последняя ошибка: Error(8): Недостаточно памяти для обработки команды
	*/
	//task3();
	/*
	Хэндлов создано: 16711591
	Последняя ошибка: Error(1450):

	В ProcessExplore: 16711680
	*/
	//task3_();
	/*
	Количество созданных объектов: 16711592
	Последняя ошибка: Error(1450): Недостаточно системных ресурсов для завершения операции
	*/
	//task4();
	/*
	Количество ручек: 10000
	Последняя ошибка: Error(0): Операция успешно завершена
	*/
	//task5();
	/*
	1) Задача упала на вложенности 4860 при размере стека 1 МБ    => 215,8 байт
	2) Задача упала на вложенности 24640 при размере стека 5 МБ   => 212,8 байт
	3) Задача упала на вложенности 49380 при размере стека 10 МБ  => 212,3 байт

	Тогда в среднем вызов пустой функции занимает 214 байт
	*/
	//task6();
	/*
	1) Задача упала на вложенности 4160 при размере стека 1 МБ    => 252,1 байт
	2) Задача упала на вложенности 21060 при размере стека 5 МБ   => 248,9 байт
	3) Задача упала на вложенности 42200 при размере стека 10 МБ  => 248,5 байт

	Тогда в среднем вызов пустой функции занимает 250 байт
	*/
	//task8();
	/*
	Нельзя создать файл или папку с длиной пути 251
	*/

	system("pause");
	return 0;
}


// Task 1: Определение максимально выделяемого объема памяти
void task1(DWORD allocatorType) {
	LPVOID ptrToAllocate;	// Указатель для выделяемой памяти
	std::vector<LPVOID> ptrsToAllocatedMemory;	// Указатели на выделяемую память
	ptrsToAllocatedMemory.reserve(10000);

	const int SmallSize = 4096;	// Малый размер выделяемого блока (минимальный)
	const int AverageSize = 4096 * 20;	// Средний размер выделяемого блока
	const int BigSize = 4096 * 1000;	// Большой размер выделяемого блока
	
	int numberOfMemory = 0;	// Количество выделенной памяти

	for( int size : {BigSize, AverageSize, SmallSize} ) {
		// Выделяем максимально возможное количество блоков памяти
		// сначала большого размера, среднего, малого
		while( true ) {
			// Попытка выделить блок памяти размером size
			ptrToAllocate = VirtualAlloc(NULL, size, allocatorType, PAGE_READWRITE);

			if( ptrToAllocate != NULL ) {
				ptrsToAllocatedMemory.push_back(ptrToAllocate);	// Запоминаем адрес выделенной памяти для дальнейшего ее освобождения
				numberOfMemory += size;
			} else {
				break;	// Не получилось выделить нужное количетво памяти -- выходим из цикла 
			}
		}
	}

	DWORD error = GetLastError();
	freeMemory(ptrsToAllocatedMemory);

	std::cout << "Выделено " << numberOfMemory << " битов памяти" << std::endl;
	std::cout << "Последняя ошибка: ";
	printErrorMessage(error);
}

// Task 2: Искусственная фрагментация памяти
void task2() {
	LPVOID ptrToAllocate;  // Указатель для выделяемой памяти
	std::vector<LPVOID> ptrsToAllocatedMemory;	// Указатели на выделяемую память
	ptrsToAllocatedMemory.reserve(10000);

	int reserveSize;
	int commitSize;
	bool isPageGuard;  // Выставлять ли флаг PAGE_GUARD -- значения {0, 1}
	bool tryCommit = 0;

	int numberOfMemory = 0;  // Количество выделенной памяти

	while( true ) {
		reserveSize = 4096 * (rand() % 1023 + 1);
		commitSize = 4096 * (rand() % 1023 + 1);

		isPageGuard = static_cast<bool>(rand() % 2);
		tryCommit = static_cast<bool>(rand() % 2);

		if( isPageGuard & tryCommit ) {
			ptrToAllocate = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE);
			ptrToAllocate = VirtualAlloc(NULL, commitSize, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD);
		} else if( isPageGuard ) {
			ptrToAllocate = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE | PAGE_GUARD);
		} else if (tryCommit) {
			ptrToAllocate = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE);
			ptrToAllocate = VirtualAlloc(NULL, commitSize, MEM_COMMIT, PAGE_READWRITE);
		} else {
			ptrToAllocate = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE);
		}

		if( ptrToAllocate != NULL ) {
			ptrsToAllocatedMemory.push_back(ptrToAllocate);	// Запоминаем адрес выделенной памяти для дальнейшего ее освобождения
		} else {
			break;	// Не получилось выделить нужное количетво памяти -- выходим из цикла 
		}
		numberOfMemory += reserveSize;
	}

	DWORD error = GetLastError();

	int a;
	std::cin >> a;

	freeMemory(ptrsToAllocatedMemory);

	std::cout << "Выделено: " << numberOfMemory << " битов памяти" << std::endl;
	std::cout << "Последняя ошибка: ";
	printErrorMessage(error);
}

// Task 3: Максимальное количество хэндлов внутри одного процесса
void task3() {
	int numberOfHandle = 0;	// Счётчик хэндлов процесса

	HANDLE handle = CreateEvent(NULL, TRUE, TRUE, NULL);	// Создаем хэндл

	if( handle == NULL ) {
		exit(0);	// Не получилось
	}

	LPHANDLE currentDuplicatedHandle = NULL;
	std::vector<LPHANDLE> duplicatedHandles;
	duplicatedHandles.reserve(10000);

	while( true ) {
		if( !DuplicateHandle(GetCurrentProcess(), handle, GetCurrentProcess(), 
			currentDuplicatedHandle, 0, FALSE, DUPLICATE_SAME_ACCESS) ) 
		{
			break;	// Не получилось
		}
		duplicatedHandles.push_back(currentDuplicatedHandle);
		numberOfHandle++;
	}

	DWORD error = GetLastError();

	int a;
	std::cin >> a;

	CloseHandle(handle);
	duplicatedHandles.clear();

	std::cout << "Хэндлов создано: " << numberOfHandle << std::endl;
	std::cout << "Последняя ошибка: ";
	printErrorMessage(error);

	std::cin >> a;
}

// Task 3_: Максимальное количество объектов ядра
void task3_() {
	HANDLE handle;
	std::vector<HANDLE> kernelObjects;
	kernelObjects.reserve(10000);

	int numberOfKernelObjects = 0;  // Счётчик объектов ядра

	while( true ) {
		handle = CreateEvent(NULL, TRUE, TRUE, NULL);
		if( handle == NULL ) {
			break;	// Не получилось создать handle
		}
		kernelObjects.push_back(handle);
		numberOfKernelObjects++;
	}

	DWORD error = GetLastError();	// Последняя ошибка

	for( HANDLE handle : kernelObjects ) {
		CloseHandle(handle);
	}
	kernelObjects.clear();

	std::cout << "Количество созданных объектов: " << numberOfKernelObjects << std::endl;
	std::cout << "Последняя ошибка: ";
	printErrorMessage(error);
}

// Task 4: Практический предел создания GDIObjects для рисования
void task4() {
	HPEN penHandle;
	std::vector<HPEN> pens;
	pens.reserve(100000); // Заранее выделяем память под указатели

	int numberOfPenHandles = 0;  // Счётчик ручек

	while( true ) {
		penHandle = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
		if( penHandle == NULL ) {  
			break;	// Не получилось
		}
		pens.push_back(penHandle);
		numberOfPenHandles++;
	}

	DWORD error = GetLastError();

	for( HPEN hpen : pens ) {
		DeleteObject(hpen);
	}
	pens.clear();

	std::cout << "Количество ручек: " << numberOfPenHandles << std::endl;
	std::cout << "Последняя ошибка: ";
	printErrorMessage(error);
}

// Пустая функция для 5 задачи -- без локальных переменных
void testEmptyFunction() {
	СountOfRecursionDepth++;
	if( СountOfRecursionDepth % 20 == 0 ) {
		std::cout << "Глубина рекурсии: " << СountOfRecursionDepth << std::endl;
	}
	testEmptyFunction();
	std::wcout << std::endl;
}

// Task 5: Количество памяти, занимаемое вызовом пустой функции
void task5() {
	СountOfRecursionDepth = 0;
	testEmptyFunction();
}

// Функция с тремя локальными переменными для 6 задачи
void testComplexFunction() {
	int a = 4, b = 5, c = 6;
	СountOfRecursionDepth++;
	if( СountOfRecursionDepth % 20 == 0 ) { 
		std::cout << "Глубина рекурсии: " << СountOfRecursionDepth << std::endl;
	}
	testComplexFunction();
}

// Task 6: Количество памяти, занимаеме вызовом функции с параметрами
void task6() {
	СountOfRecursionDepth = 0;
	testComplexFunction();
}

// Task 8: Максимальный путь в файловой системе

void task8() {
	CreateDirectory(CString("C:\\test"), NULL);	// Новая директория, куда будем добавлять вложенные папки

	CString fileName = "f";
	CString dirName = "C:\\test\\a";	// Cоздаем все большую вложенность папок с короткими именами
	while( true ) {
		CreateDirectory(dirName, NULL);	// Создаем новую вложенную директорию

		// Проверяем можно ли создать в ней файл
		if( !hasCreateFile((LPCTSTR)(dirName + "\\" + fileName)) ) {
			std::cout << "Нельзя создать файл или папку с длиной пути " << getLengthOfFile(dirName + "\\" + fileName) << std::endl;
			break;
		}
		dirName.Append(CString("\\a"));
	}
	system("pause");

	system("del /S /Q C:\\test");	// удаляем созданное нами, /S - удалять рекурсивно, /Q - не просить подтверждения
	exit(0);
}

// Функция - индикатор, можно ли создать файл с данным именем
bool hasCreateFile(LPCTSTR fileName) {
	HANDLE file = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( file == INVALID_HANDLE_VALUE ) {
		return false;
	} else {
		CloseHandle(file);
		return true;
	}
}

// Функция длины полного пути файла 
int getLengthOfFile(LPCTSTR fileName) {
	const DWORD LenOfBuffer = 10000;
	TCHAR  buffer[LenOfBuffer] = TEXT("");	// Здесь будет полный путь
	TCHAR** filePart = { NULL };

	DWORD retval = GetFullPathName(fileName, LenOfBuffer, buffer, filePart);	// Полный путь

	return CString(buffer).GetLength();
}

// Функция для освобождения выделенной памяти
void freeMemory(std::vector<LPVOID>& ptrs) {
	for( LPVOID ptr : ptrs ) {
		VirtualFree(ptr, 0, MEM_RELEASE);
	}
	ptrs.clear();
}

// Функция вывода ошибки 
void printErrorMessage(DWORD error) {
	LPVOID buffer;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buffer,
		0, NULL);

	if( buffer != NULL ) {
		std::wcout << "Error (" << error << "): " << static_cast<LPCTSTR>(buffer) << std::endl;
		LocalFree(buffer);
	} else {
		std::wcout << "Нет сообщения для этой ошибки" << std::endl;
	}
}
