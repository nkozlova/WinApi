#pragma once


#ifdef PALINDROME_COUNTER_LIB
#define PALINDROME_COUNTER_LIB extern "C" __declspec(dllexport)

#else
#define PALINDROME_COUNTER_LIB extern "C" __declspec(dllimport)

#endif


// Считаем количество палиндромов в тексте text
PALINDROME_COUNTER_LIB int PalindromeCounter( const wchar_t* text );
