#include "PalindromeCounter.h"
#include <Windows.h>
#include "stdafx.h"
#include <exception>
#include <string>
#include <cctype>
#include <iostream>


// ‘ункци€, вычисл€юща€ количество палиндромов в тексте
int PalindromeCounter(const wchar_t* text) {
	int counterOfPalindrome = 0;
	int startOfWord = 0;
	int endOfWord = 0;
	int i = 0;

	while (text[i] != '\0') {
		if (!iswalpha(text[i + 1]) && iswalpha(text[startOfWord])) {
			endOfWord = i;
			while (startOfWord < endOfWord && text[startOfWord] == text[endOfWord]) {
				startOfWord++;
				endOfWord--;
			}
			if (startOfWord >= endOfWord) {
				counterOfPalindrome++;
			}
			startOfWord = i + 2;
		}
		else if (!iswalpha(text[startOfWord])) {
			startOfWord++;
		}

		++i;
	}

	return counterOfPalindrome;
}