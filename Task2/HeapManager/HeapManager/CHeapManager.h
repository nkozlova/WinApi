#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <map>


struct Heading {
	Heading( Heading* p, int s ) : prev( p ), blockSize( s ) {}

	Heading* prev;
	int blockSize;
};


struct Block {
	LPVOID addr;	// Адрес блока
	size_t size;	// Его размер (без учета заголовков)

	Block( LPVOID a, int s ) : addr( a ), size( s ) {}

	bool operator<( Block const & b ) const {
		return this->addr < b.addr;
	}
};


struct BlockComparator {
	bool operator()( Block const & a, Block const & b ) const {
		return ( a.size < b.size ) || ( a.size == b.size && a.addr < b.addr );
	}
};


class CHeapManager {
	LPVOID heap;	// Адрес начала кучи
	int sizeOfHeap;	// Размер

	static const int NUMBER_OF_SIZE_TYPES = 3;	// Количество разменных классов по размеру блоков
	static constexpr int SIZE_TYPES_LOWER_BOUNDS[NUMBER_OF_SIZE_TYPES] = { 0, 4 * 1024, 128 * 1024 }; // Нижние границы для них
	// Храним несколько set/hash_table свободных блоков памяти разного размера (блоки меньше 4 КБ, от 4 до 128 КБ, больше 128 КБ).

	static const int PAGES_SUBSCRIBE = 1;	// Флаг для заполнения страницы
	static const int PAGES_UNSUBSCRIBE = -1;	// Флаг для освобождения страницы

	_SYSTEM_INFO systemInfo;

	std::set<Block, BlockComparator> freeBlocks[NUMBER_OF_SIZE_TYPES];	// Набор адресов свободных блоков разных разменных классов
	std::map<LPVOID, int> freeAddresses;

	std::vector<int> pages;	// Массив целых чисел по количеству страниц, где храним кол-во занятых блоков, 
							// лежащих на этой странице
							// (Для передачи физической памяти только при необходимости)

public:
	CHeapManager();
	~CHeapManager();

	void Create( int minSize, int maxSize );
	void* Alloc( int size );
	void Free( void* mem );
	void Destroy();

	void Describe();

private:
	static int round( int value, int roundTo );
	static int getSizeType( int size );

	LPVOID getNext( const Block ) const;
	void addFreeBlock( const Block );
	Block findSuitableFreeBlock( int size );
	Block biteOfNewBlock( const Block block, int size );
	void removeFreeBlock( const Block );
	void allocateBlock( const Block );

	void updatePages( const Block block, int operation );

	bool isFree( LPVOID addr ) const;
};
