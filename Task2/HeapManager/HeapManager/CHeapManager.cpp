#include "stdafx.h"
#include "CHeapManager.h"


CHeapManager::CHeapManager() : heap( NULL ), sizeOfHeap( 0 ) {
	GetSystemInfo( &systemInfo );
}

CHeapManager::~CHeapManager() {
	Destroy();
}


/*
Резервируем непрерывный регион памяти размером maxSize.
Части этого региона, размером minSize, необходимо передать физическую память (commit).
(При работе с памятью необходимо учитывать её страничное представление, т.е. округлять minSize вверх до размера страницы (4Кб))
*/
void CHeapManager::Create( int minSize, int maxSize ) {
	if( minSize > maxSize ) {
		std::wcout << L"Значения minSize и maxSize не корректны" << std::endl;
		return;
	}

	// Окоугляем значения maxSize и minSize до целого количества размера страниц
	maxSize = round( maxSize, systemInfo.dwPageSize );
	minSize = round( minSize, systemInfo.dwPageSize );

	if( heap != NULL ) {
		std::wcout << L"Heap уже создана" << std::endl;
		return;
	}

	// Резервируем память для кучи размером maxSize
	heap = VirtualAlloc( NULL, maxSize, MEM_RESERVE, PAGE_READWRITE );
	if( heap == NULL ) {
		std::wcout << L"Heap уже создана" << std::endl;
		return;
	}

	// Предоставляем фрагменту minSize физическую память
	LPVOID commitResult = VirtualAlloc( heap, minSize, MEM_COMMIT, PAGE_READWRITE );
	if( commitResult == NULL ) {
		VirtualFree( heap, maxSize, MEM_RELEASE );
		heap = NULL;
		std::wcout << L"Не получилось выделить физическую память размером minSize" << std::endl;
		return;
	}
	sizeOfHeap = maxSize;

	// Инициализируем страницы количеством блоков на каждой из них (пока на каждой странице 0 блоков)
	pages.assign( sizeOfHeap / systemInfo.dwPageSize + 1, 0 );

	// Добавляем свободны блок (он пока один и занимает все пространство)
	addFreeBlock( Block( heap, sizeOfHeap ) );
}


/*
- Выдаем блоки кратные 4 байтам.
- Передаем физическую память только по мере необходимости. 
- Записываем в начало выделенной памяти аллоцируемый размер (для Free).
- Объединяем свободные блоки. 
- Храним несколько set/hash_table свободных блоков памяти разного размера.
*/
void* CHeapManager::Alloc( int size ) {
	size = round( size + sizeof( Heading ), 4 );

	Block suitableBlock = findSuitableFreeBlock( size );	// Поиск подходящего свободного блока
	if( suitableBlock.addr == NULL ) {
		std::wcout << L"Сейчас нельзя выделить память такого размера" << std::endl;
		return static_cast<LPVOID>( Block( 0, 0 ).addr );
	}

	Block newBlock = biteOfNewBlock( suitableBlock, size ); // Забираем себе кусок от найденного блока

	allocateBlock( newBlock );	// Аллоцируем себе блок
	return static_cast<LPVOID>( static_cast<byte*>( newBlock.addr ) + sizeof( Heading ) );	// Берем тело блока
}


/*
Освобождение памяти -- возврат системе далее не используемых страниц физической памяти
*/
void CHeapManager::Free( void* mem ) {
	Heading* heading = ( Heading* )( static_cast<byte*>( mem ) - sizeof( Heading ) );	// Заголовок удаляемого блока
	Block curBlock = Block( heading, heading->blockSize );	// Текущий блок
	//	Соседи блока
	Heading* left = heading->prev;
	Heading* right = ( Heading* )getNext( curBlock );

	int newSize = heading->blockSize;
	LPVOID newFreeBlockAddr = heading;	// Новый свободный блок

	if( left != NULL ) {
		// Слева примыкает свободный блок
		newFreeBlockAddr = left;
		newSize += ( byte* )heading - ( byte* )left;
		removeFreeBlock( Block( left, ( byte* )heading - ( byte* )left ) );
	}
	if( isFree( right ) ) {
		int sizeRight = freeAddresses.at( right );
		newSize += sizeRight;
		removeFreeBlock( Block( right, sizeRight ) );
	}

	// Добавляем свободный блок и обновляем страницы
	addFreeBlock( Block( newFreeBlockAddr, newSize ) ); 
	updatePages( curBlock, PAGES_UNSUBSCRIBE );
}


/*
Необходимо вернуть системе всю физическую память и освободить зарезервированный регион. 
В случае, если остались аллоцированные участки, необходимо вывести их адреса и размер.
*/
void CHeapManager::Destroy() {
	LPVOID cur = heap;
	while( cur != NULL ) {
		if( !isFree( cur ) ) {
			Heading* heading = ( Heading* )cur;
			std::wcout << L"Осталась память размером: " << heading->blockSize - sizeof( Heading ) << L" по адресу " << heading << std::endl;
			Free( static_cast<byte*>( cur ) + sizeof( Heading ) );
			cur = heap;
		}
		else {
			cur = getNext( Block( cur, freeAddresses.at( cur ) ) );
		}
	}
}


/*
Описание состояния CHeapManager
*/
void CHeapManager::Describe() {
	std::wcout << L"Адрес heap: " << heap << std::endl;
	std::wcout << L"Размер heap: " << sizeOfHeap << L"(" << sizeOfHeap / systemInfo.dwPageSize << L" страниц)" << std::endl;

	for( auto pair : freeAddresses ) {
		std::wcout << L"Свободны " << pair.first << L" " << pair.second << std::endl;
	}

	std::wcout << std::endl << std::endl << std::endl;
}


// Округляет value до числа, кратного roundTo
int CHeapManager::round( int value, int roundTo ) {
	if( value % roundTo == 0 ) {
		return value;
	}
	return roundTo * ( ( value / roundTo ) + 1 );
}


// Определение разменного класса
int CHeapManager::getSizeType( int size ) {
	int res = 0;
	for( int type = 0; type < NUMBER_OF_SIZE_TYPES; ++type ) {
		if( SIZE_TYPES_LOWER_BOUNDS[type] <= size ) {
			res = type;
		}
	}

	return res;
}


// Добавление свободных блоков
void CHeapManager::addFreeBlock( const Block block ) {
	freeBlocks[getSizeType( block.size )].insert( block );
	freeAddresses.insert( std::make_pair( block.addr, block.size ) );
	Heading* next = ( Heading* )getNext( block );	// Обновим у последующего аллоцированного блока указатель prev

	if( next != NULL ) {
		next->prev = ( Heading* )block.addr;
	}
}


// Поиск подходящего свободного места в списке блоков размера size
Block CHeapManager::findSuitableFreeBlock( int size ) {
	Block minBlock = Block( 0, size );
	int minSizeType = getSizeType( size );	// Минимальный допустимый тип размеров
											// Если не найдем подходящего блока желаемого размера, будем искать в более крупных
	for( int type = minSizeType; type < NUMBER_OF_SIZE_TYPES; ++type ) {
		auto candidate = freeBlocks[type].lower_bound( minBlock );
		if( candidate != freeBlocks[type].end() ) {
			if( candidate->size < size ) {
				break;
			}
			return *candidate;
		}
	}
	return Block( 0, 0 );	// Не получилось найти такое место
}


// Отделяем блок требуемого размера от свободного блока (отделенный блок не свободен, не аллоцирован)
Block CHeapManager::biteOfNewBlock( const Block block, int size ) {
	if( block.size < size ) {
		return Block( 0, 0 );
	}

	removeFreeBlock( block );	// Удаляем текущий свободный блок
							
	// Если после отделения блока останется свободное место, то добавим новый свободный блок
	if( block.size > size + sizeof( Heading ) ) {
		LPVOID newBlockAddr = static_cast<byte*>( block.addr ) + size;
		addFreeBlock( Block( newBlockAddr, block.size - size ) );
	}

	return Block( block.addr, size );
}


// Удаление свободного блока
void CHeapManager::removeFreeBlock( const Block block ) {
	freeBlocks[getSizeType( block.size )].erase( block );
	freeAddresses.erase( block.addr );
}


// Выделение блока
void CHeapManager::allocateBlock( const Block block ) {
	// Обеспечиваем блок закоммиченной памятью
	LPVOID comm = VirtualAlloc( block.addr, block.size, MEM_COMMIT, PAGE_READWRITE );
	if( comm == NULL ) {
		std::wcout << L"Коммит не получился" << std::endl;
		return;
	}

	updatePages( block, PAGES_SUBSCRIBE );	// Обновляем pages
	Heading* heading = ( Heading* )block.addr;	// Берем заголовок адреса

	heading->blockSize = block.size;
	heading->prev = 0;	// Перед block не может быть свободного блока (значит, там либо занятый, либо ничего)
}


// Обновление страниц pages
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

			// Освобождаем память
			int size = ( i - startOfReleasing ) * systemInfo.dwPageSize;
			VirtualFree( static_cast<byte*>( heap ) + startOfReleasing * systemInfo.dwPageSize, size, MEM_DECOMMIT );			
		}
	}
}


// Возвращает блок, следующий непосредственно за указанным или нулевой блок, если указанный блок был последним
LPVOID CHeapManager::getNext( const Block block ) const {
	// Если это последний блок, то следующего точно нет
	if( static_cast<byte*>( block.addr ) - static_cast<byte*>( heap ) + block.size + sizeof( Heading ) <= sizeOfHeap ) {
		return static_cast<byte*>( block.addr ) + block.size;
	}
	else {
		return 0;
	}

}


// Проверка на свободность блока, лежащего по данному адресу
bool CHeapManager::isFree( LPVOID addr ) const {
	return addr != NULL && freeAddresses.find( addr ) != freeAddresses.end();
}
