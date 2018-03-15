#pragma once
#include "CHeapManager.h"
#include <windows.h>
#include <random>


static const int MAX_SIZE = 256 * 1024;

static HANDLE standartHeap;
static CHeapManager myHeap;

static const size_t LOWER[3] = { 0, 4 * 1024, 128 * 1024 };
static const size_t UPPER[3] = { 4 * 1024, 128 * 1024, MAX_SIZE };


int GenerateSize() {
	int type = rand() % 3;
	int size = LOWER[type] + rand() % (UPPER[type] - LOWER[type]);
	return size;
}


class ComparatorHeapManager {
public:
	static void CreateHeap( size_t size ) {
		standartHeap = HeapCreate( 0, 256 * 1024, size );
	}

	void* operator new( size_t size ) {
		return HeapAlloc( standartHeap, 0, size * GenerateSize() );
	}

	void operator delete( void* ptr ) {
		HeapFree( standartHeap, 0, ptr );
	}

	static void DestroyHeap() {
		HeapDestroy( standartHeap );
	}
};


class ComparatorCHeapManager {
public:
	static void CreateHeap( size_t size ) {
		myHeap.Create( 256 * 1024, size );
	}

	void* operator new( size_t size ) {
		return myHeap.Alloc( size * GenerateSize() );
	}

	void operator delete( void* ptr ) {
		myHeap.Free( ptr );
	}

	static void DestroyHeap() {
		myHeap.Destroy();
	}
};
