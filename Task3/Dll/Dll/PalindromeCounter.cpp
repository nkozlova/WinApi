// PalindromeCounter.cpp: определяет экспортированные функции для приложения DLL.
//

// Из этого модуля экспортируем
#define PALINDROME_COUNTER_LIB extern "C" __declspec( dllexport )

#include "stdafx.h"
#include <exception>
#include "PalindromeCounter.h"
#include <wchar.h>
#include <string>
#include <cctype>
#include <iostream>
#include <vector>
#include <numeric>


// Количество палиндромов в тексте
int PalindromeCounter( const wchar_t* text ) {
	int counterOfPalindrome = 0;
	int startOfWord = 0;
	int endOfWord = 0;
	int i = 0;

	while( text[i] != '\0' ) {
		if ( !iswalpha( text[i + 1] ) && iswalpha( text[startOfWord] ) ) {
			endOfWord = i;
			while (startOfWord < endOfWord && text[startOfWord] == text[endOfWord]) {
				startOfWord++;
				endOfWord--;
			}
			if (startOfWord >= endOfWord) {
				counterOfPalindrome++;
			}
			startOfWord = i + 2;
		} else if (!iswalpha(text[startOfWord])) {
			startOfWord++;
		}

		++i;
	}

	return counterOfPalindrome;
}
