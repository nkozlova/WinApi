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
	LPVOID addr;	// ����� �����
	size_t size;	// ��� ������ (��� ����� ����������)

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
	LPVOID heap;	// ����� ������ ����
	int sizeOfHeap;	// ������

	static const int NUMBER_OF_SIZE_TYPES = 3;	// ���������� ��������� ������� �� ������� ������
	static constexpr int SIZE_TYPES_LOWER_BOUNDS[NUMBER_OF_SIZE_TYPES] = { 0, 4 * 1024, 128 * 1024 }; // ������ ������� ��� ���
	// ������ ��������� set/hash_table ��������� ������ ������ ������� ������� (����� ������ 4 ��, �� 4 �� 128 ��, ������ 128 ��).

	static const int PAGES_SUBSCRIBE = 1;	// ���� ��� ���������� ��������
	static const int PAGES_UNSUBSCRIBE = -1;	// ���� ��� ������������ ��������

	_SYSTEM_INFO systemInfo;

	std::set<Block, BlockComparator> freeBlocks[NUMBER_OF_SIZE_TYPES];	// ����� ������� ��������� ������ ������ ��������� �������
	std::map<LPVOID, int> freeAddresses;

	std::vector<int> pages;	// ������ ����� ����� �� ���������� �������, ��� ������ ���-�� ������� ������, 
							// ������� �� ���� ��������
							// (��� �������� ���������� ������ ������ ��� �������������)

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
