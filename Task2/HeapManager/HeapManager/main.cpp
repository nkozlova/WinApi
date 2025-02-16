// HeapManager.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "CHeapManager.h"
#include "ComparatorHeapManagers.h"
#include <time.h>
#include <ctime>
#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <atlstr.h>
#include <cstdlib>


void testMyHeap() {
	_SYSTEM_INFO systemInfo;
	GetSystemInfo( &systemInfo );
	size_t pageSize = systemInfo.dwPageSize;
	size_t heapSize = 8 * 1024;

	CHeapManager heapManager;
	heapManager.Create( pageSize, heapSize );
	LPVOID ptr1 = heapManager.Alloc( 5000 );
	heapManager.Describe();
	LPVOID ptr2 = heapManager.Alloc( 50 );
	heapManager.Describe();
	LPVOID ptr3 = heapManager.Alloc( 50000 );
	heapManager.Describe();
	LPVOID ptr4 = heapManager.Alloc( 100000 );
	heapManager.Describe();
	LPVOID ptr5 = heapManager.Alloc( 1000 );
	heapManager.Describe();
	heapManager.Destroy();
	heapManager.Describe();
	heapManager.Destroy();
	heapManager.Describe();

    return;
}


template<typename T>
void compareHeap( int numberOfAllocations ) {
	T::CreateHeap( numberOfAllocations * MAX_SIZE );
	std::vector<T*> ptrs;
	ptrs.resize( numberOfAllocations );

	unsigned int startMoment = clock();

	int startPush = 0, numberPush = 0, numberDel = 0, startDel = 0;
	for( int l = 0; l < 20; l++ ) {
		if( numberPush < numberOfAllocations ) {
			numberPush += rand() % ( numberOfAllocations - numberPush );
			for( int i = startPush; i < numberPush; ++i ) {
				ptrs[i] = new T;
			}
			startPush = numberPush;

			numberDel += rand() % ( numberPush - numberDel );
			for( int i = startDel; i < numberDel; ++i ) {
				delete ptrs[i];
			}
			startDel = numberDel;
		}
	}
	for( int i = startDel; i < numberPush; ++i ) {
		delete ptrs[i];
	}

	//std::cin.get();
	T::DestroyHeap();

	unsigned int finalMoment = clock();

	auto totalPeriod = finalMoment - startMoment;

	std::wcout << L"Общее время: " << totalPeriod << std::endl;
}

void сompareTime() {
	int numerOfAllocs = 20000;

	std::wcout << std::endl << L"Стандартная куча" << std::endl;
	compareHeap<ComparatorHeapManager>( numerOfAllocs );

	std::wcout << L"Моя куча" << std::endl;
	compareHeap<ComparatorCHeapManager>( numerOfAllocs );
}

int main() {
	srand( time( NULL ) );
	setlocale( LC_ALL, "Russian" );

	//testMyHeap();
	сompareTime();

	return 0;
}
