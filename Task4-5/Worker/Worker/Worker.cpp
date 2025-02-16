// Worker.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <fstream>
#include <set>
#include <iostream>
#include <string>
#include <exception>
#include <codecvt>
#include <assert.h>


std::set<std::wstring> Dictionary;	// Словарь запрещенных слов

HANDLE terminationEvent, dataIsReadyEvent, workIsDoneEvent;

// Взятие i-го аргумента из командной строки (попытка взятия)
std::wstring GetArgument(size_t index, const std::string &what) {
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argc < index + 1) {
		std::wcout << L"Haven't enough arguments" << std::endl;
		exit(1);
	}
	return std::wstring(argv[index]);
}

// Чтение словаря запрещенных слов
void SetDictionary(const std::wstring &fileName) {
	std::wifstream input(fileName, std::wifstream::in);
	// Чтобы unicode файл читался как unicode файл
	input.imbue(std::locale(input.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

	if (input.good()) {
		while (!input.eof()) {
			std::wstring word;
			input >> word;
			Dictionary.insert(word);
		}
	} else {
		std::wcout << L"Failed to read dictionary" << std::endl;
		exit(1);
	}

	input.close();
}

void DoJob() {
	std::wstring fileName = GetArgument(2, "Text file path");
	std::wstring text;	// Отфильтрованный текст

	std::wifstream input(fileName, std::wifstream::in);
	// Чтобы unicode файл читался как unicode файл
	input.imbue(std::locale(input.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

	if (input.good()) {
		while (!input.eof()) {
			std::wstring word;
			input >> word;
			if (Dictionary.find(word) == Dictionary.end()) {
				text.append(L" ").append(word);
			}
		}
	} else {
		std::wcout << L"Failed to read text" << std::endl;
		exit(1);
	}
	input.close();

	// Записываем отфильтрованный текст обратно в файл
	std::wofstream output(fileName, std::wofstream::out);
	output.imbue(std::locale(output.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

	output << text;
	output.close();

	SetEvent(workIsDoneEvent);
	//SetEvent(terminationEvent);
}


int main(int argc, char* argv[]) {

	int i = atoi(argv[3]);

	for (int j = 0; j < argc; j++) {
		std::wcout << argv[j] << std::endl;
	}

	SetDictionary(GetArgument(1, "Dictionary file path"));

	terminationEvent = CreateEvent(NULL, FALSE, FALSE, (L"Global\\TerminateEvent" + std::to_wstring(i)).c_str());
	dataIsReadyEvent = CreateEvent(NULL, FALSE, FALSE, (L"Global\\DataIsReadyEvent" + std::to_wstring(i)).c_str());
	workIsDoneEvent = CreateEvent(NULL, FALSE, FALSE, (L"Global\\WorkIsDoneEvent" + std::to_wstring(i)).c_str());

	std::wcout << dataIsReadyEvent << L" Global\\DataIsReadyEvent" + std::to_wstring(i) << std::endl;

	//SetEvent(dataIsReadyEvent);

	HANDLE waitedEvents[] = { dataIsReadyEvent, terminationEvent };

	while (true) {
		DWORD waitResult = WaitForMultipleObjects(2, waitedEvents, FALSE, INFINITE);
		switch (waitResult) {
		case WAIT_FAILED:
			std::wcout << L"Fail" << std::endl;
			break;
		case WAIT_TIMEOUT:
			std::wcout << L"Time out" << std::endl;
			break;
		case WAIT_OBJECT_0 + 0:
			// Появилсь работа
			std::wcout << L"Do some job" << std::endl;
			DoJob();
			break;
		case WAIT_OBJECT_0 + 1:
			// Завершение работы
			std::wcout << L"Terminate" << std::endl;
			ExitProcess(0);
			break;
		}
	}
}
