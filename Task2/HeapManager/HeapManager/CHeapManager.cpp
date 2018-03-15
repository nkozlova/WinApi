#include "stdafx.h"
#include "CHeapManager.h"


CHeapManager::CHeapManager() : heap( NULL ), sizeOfHeap( 0 ) {
	GetSystemInfo( &systemInfo );
}

CHeapManager::~CHeapManager() {
	Destroy();
}


/*
����������� ����������� ������ ������ �������� maxSize.
����� ����� �������, �������� minSize, ���������� �������� ���������� ������ (commit).
(��� ������ � ������� ���������� ��������� � ���������� �������������, �.�. ��������� minSize ����� �� ������� �������� (4��))
*/
void CHeapManager::Create( int minSize, int maxSize ) {
	if( minSize > maxSize ) {
		std::wcout << L"�������� minSize � maxSize �� ���������" << std::endl;
		return;
	}

	// ��������� �������� maxSize � minSize �� ������ ���������� ������� �������
	maxSize = round( maxSize, systemInfo.dwPageSize );
	minSize = round( minSize, systemInfo.dwPageSize );

	if( heap != NULL ) {
		std::wcout << L"Heap ��� �������" << std::endl;
		return;
	}

	// ����������� ������ ��� ���� �������� maxSize
	heap = VirtualAlloc( NULL, maxSize, MEM_RESERVE, PAGE_READWRITE );
	if( heap == NULL ) {
		std::wcout << L"Heap ��� �������" << std::endl;
		return;
	}

	// ������������� ��������� minSize ���������� ������
	LPVOID commitResult = VirtualAlloc( heap, minSize, MEM_COMMIT, PAGE_READWRITE );
	if( commitResult == NULL ) {
		VirtualFree( heap, maxSize, MEM_RELEASE );
		heap = NULL;
		std::wcout << L"�� ���������� �������� ���������� ������ �������� minSize" << std::endl;
		return;
	}
	sizeOfHeap = maxSize;

	// �������������� �������� ����������� ������ �� ������ �� ��� (���� �� ������ �������� 0 ������)
	pages.assign( sizeOfHeap / systemInfo.dwPageSize + 1, 0 );

	// ��������� �������� ���� (�� ���� ���� � �������� ��� ������������)
	addFreeBlock( Block( heap, sizeOfHeap ) );
}


/*
- ������ ����� ������� 4 ������.
- �������� ���������� ������ ������ �� ���� �������������. 
- ���������� � ������ ���������� ������ ������������ ������ (��� Free).
- ���������� ��������� �����. 
- ������ ��������� set/hash_table ��������� ������ ������ ������� �������.
*/
void* CHeapManager::Alloc( int size ) {
	size = round( size + sizeof( Heading ), 4 );

	Block suitableBlock = findSuitableFreeBlock( size );	// ����� ����������� ���������� �����
	if( suitableBlock.addr == NULL ) {
		std::wcout << L"������ ������ �������� ������ ������ �������" << std::endl;
		return static_cast<LPVOID>( Block( 0, 0 ).addr );
	}

	Block newBlock = biteOfNewBlock( suitableBlock, size ); // �������� ���� ����� �� ���������� �����

	allocateBlock( newBlock );	// ���������� ���� ����
	return static_cast<LPVOID>( static_cast<byte*>( newBlock.addr ) + sizeof( Heading ) );	// ����� ���� �����
}


/*
������������ ������ -- ������� ������� ����� �� ������������ ������� ���������� ������
*/
void CHeapManager::Free( void* mem ) {
	Heading* heading = ( Heading* )( static_cast<byte*>( mem ) - sizeof( Heading ) );	// ��������� ���������� �����
	Block curBlock = Block( heading, heading->blockSize );	// ������� ����
	//	������ �����
	Heading* left = heading->prev;
	Heading* right = ( Heading* )getNext( curBlock );

	int newSize = heading->blockSize;
	LPVOID newFreeBlockAddr = heading;	// ����� ��������� ����

	if( left != NULL ) {
		// ����� ��������� ��������� ����
		newFreeBlockAddr = left;
		newSize += ( byte* )heading - ( byte* )left;
		removeFreeBlock( Block( left, ( byte* )heading - ( byte* )left ) );
	}
	if( isFree( right ) ) {
		int sizeRight = freeAddresses.at( right );
		newSize += sizeRight;
		removeFreeBlock( Block( right, sizeRight ) );
	}

	// ��������� ��������� ���� � ��������� ��������
	addFreeBlock( Block( newFreeBlockAddr, newSize ) ); 
	updatePages( curBlock, PAGES_UNSUBSCRIBE );
}


/*
���������� ������� ������� ��� ���������� ������ � ���������� ����������������� ������. 
� ������, ���� �������� �������������� �������, ���������� ������� �� ������ � ������.
*/
void CHeapManager::Destroy() {
	LPVOID cur = heap;
	while( cur != NULL ) {
		if( !isFree( cur ) ) {
			Heading* heading = ( Heading* )cur;
			std::wcout << L"�������� ������ ��������: " << heading->blockSize - sizeof( Heading ) << L" �� ������ " << heading << std::endl;
			Free( static_cast<byte*>( cur ) + sizeof( Heading ) );
			cur = heap;
		}
		else {
			cur = getNext( Block( cur, freeAddresses.at( cur ) ) );
		}
	}
}


/*
�������� ��������� CHeapManager
*/
void CHeapManager::Describe() {
	std::wcout << L"����� heap: " << heap << std::endl;
	std::wcout << L"������ heap: " << sizeOfHeap << L"(" << sizeOfHeap / systemInfo.dwPageSize << L" �������)" << std::endl;

	for( auto pair : freeAddresses ) {
		std::wcout << L"�������� " << pair.first << L" " << pair.second << std::endl;
	}

	std::wcout << std::endl << std::endl << std::endl;
}


// ��������� value �� �����, �������� roundTo
int CHeapManager::round( int value, int roundTo ) {
	if( value % roundTo == 0 ) {
		return value;
	}
	return roundTo * ( ( value / roundTo ) + 1 );
}


// ����������� ���������� ������
int CHeapManager::getSizeType( int size ) {
	int res = 0;
	for( int type = 0; type < NUMBER_OF_SIZE_TYPES; ++type ) {
		if( SIZE_TYPES_LOWER_BOUNDS[type] <= size ) {
			res = type;
		}
	}

	return res;
}


// ���������� ��������� ������
void CHeapManager::addFreeBlock( const Block block ) {
	freeBlocks[getSizeType( block.size )].insert( block );
	freeAddresses.insert( std::make_pair( block.addr, block.size ) );
	Heading* next = ( Heading* )getNext( block );	// ������� � ������������ ��������������� ����� ��������� prev

	if( next != NULL ) {
		next->prev = ( Heading* )block.addr;
	}
}


// ����� ����������� ���������� ����� � ������ ������ ������� size
Block CHeapManager::findSuitableFreeBlock( int size ) {
	Block minBlock = Block( 0, size );
	int minSizeType = getSizeType( size );	// ����������� ���������� ��� ��������
											// ���� �� ������ ����������� ����� ��������� �������, ����� ������ � ����� �������
	for( int type = minSizeType; type < NUMBER_OF_SIZE_TYPES; ++type ) {
		auto candidate = freeBlocks[type].lower_bound( minBlock );
		if( candidate != freeBlocks[type].end() ) {
			if( candidate->size < size ) {
				break;
			}
			return *candidate;
		}
	}
	return Block( 0, 0 );	// �� ���������� ����� ����� �����
}


// �������� ���� ���������� ������� �� ���������� ����� (���������� ���� �� ��������, �� �����������)
Block CHeapManager::biteOfNewBlock( const Block block, int size ) {
	if( block.size < size ) {
		return Block( 0, 0 );
	}

	removeFreeBlock( block );	// ������� ������� ��������� ����
							
	// ���� ����� ��������� ����� ��������� ��������� �����, �� ������� ����� ��������� ����
	if( block.size > size + sizeof( Heading ) ) {
		LPVOID newBlockAddr = static_cast<byte*>( block.addr ) + size;
		addFreeBlock( Block( newBlockAddr, block.size - size ) );
	}

	return Block( block.addr, size );
}


// �������� ���������� �����
void CHeapManager::removeFreeBlock( const Block block ) {
	freeBlocks[getSizeType( block.size )].erase( block );
	freeAddresses.erase( block.addr );
}


// ��������� �����
void CHeapManager::allocateBlock( const Block block ) {
	// ������������ ���� ������������� �������
	LPVOID comm = VirtualAlloc( block.addr, block.size, MEM_COMMIT, PAGE_READWRITE );
	if( comm == NULL ) {
		std::wcout << L"������ �� ���������" << std::endl;
		return;
	}

	updatePages( block, PAGES_SUBSCRIBE );	// ��������� pages
	Heading* heading = ( Heading* )block.addr;	// ����� ��������� ������

	heading->blockSize = block.size;
	heading->prev = 0;	// ����� block �� ����� ���� ���������� ����� (������, ��� ���� �������, ���� ������)
}


// ���������� ������� pages
void CHeapManager::updatePages( const Block block, int operation ) {
	if( operation != PAGES_SUBSCRIBE && operation != PAGES_UNSUBSCRIBE ) {
		return;
	}

	int offset = static_cast<byte*>( block.addr ) - static_cast<byte*>( heap );
	int startOfPage = offset / systemInfo.dwPageSize;
	int endOfPage = ( offset + block.size ) / systemInfo.dwPageSize;

	int startOfReleasing;
	bool isInReleasingFragment = false;

	for( int i = startOfPage; i <= endOfPage; ++i ) {
		pages[i] += operation;

		if( pages[i] == 0 && !isInReleasingFragment ) {
			startOfReleasing = i;
			isInReleasingFragment = true;
		}
		else if( pages[i] != 0 && isInReleasingFragment ) {
			isInReleasingFragment = false;

			// ����������� ������
			int size = ( i - startOfReleasing ) * systemInfo.dwPageSize;
			VirtualFree( static_cast<byte*>( heap ) + startOfReleasing * systemInfo.dwPageSize, size, MEM_DECOMMIT );			
		}
	}
}


// ���������� ����, ��������� ��������������� �� ��������� ��� ������� ����, ���� ��������� ���� ��� ���������
LPVOID CHeapManager::getNext( const Block block ) const {
	// ���� ��� ��������� ����, �� ���������� ����� ���
	if( static_cast<byte*>( block.addr ) - static_cast<byte*>( heap ) + block.size + sizeof( Heading ) <= sizeOfHeap ) {
		return static_cast<byte*>( block.addr ) + block.size;
	}
	else {
		return 0;
	}

}


// �������� �� ����������� �����, �������� �� ������� ������
bool CHeapManager::isFree( LPVOID addr ) const {
	return addr != NULL && freeAddresses.find( addr ) != freeAddresses.end();
}
