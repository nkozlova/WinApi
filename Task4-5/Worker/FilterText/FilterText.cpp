// FilterText.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <assert.h>
#include <wchar.h>
#include <fstream>
#include <exception>
#include <codecvt>


#pragma warning(disable : 4996)

#define NUMBER_OF_PROCESSES 4

HANDLE terminationEvent[NUMBER_OF_PROCESSES], dataIsReadyEvent[NUMBER_OF_PROCESSES], workIsDoneEvent[NUMBER_OF_PROCESSES];


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

void CreateProcesses() {
	std::wstring dict = GetArgument(1, "Dictionary file path"), prog = L"Worker.exe";
	// Записываем разбитые кусочки текста сюда
	std::wstring partName[NUMBER_OF_PROCESSES];

	std::wstring arguments;
	for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
		partName[i] = L"..\\part" + std::to_wstring(i) + L".txt";
		arguments = prog + L" " + dict + L" " + partName[i] + L" " + std::to_wstring(i);

		std::wcout << arguments << std::endl;

		PROCESS_INFORMATION processInfo;
		STARTUPINFO startUpInfo = { sizeof(startUpInfo) };
		if (!CreateProcess(NULL, const_cast<LPWSTR>(arguments.c_str()), NULL, NULL, TRUE, NULL, NULL, NULL, &startUpInfo, &processInfo)) {
			std::wcout << L"Can't create process" << std::endl;
			exit(1);
		}
		arguments = L"";
	}
}

void SplitText() {
	std::wstring dict = GetArgument(1, "Dictionary file path"), prog = L"Worker.exe";
	std::wstring fileName = GetArgument(2, "Text file path");

	std::wifstream input(fileName, std::wifstream::in);
	input.imbue(std::locale(input.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

	input.seekg(0, std::wifstream::end);
	int offset = 0, size = input.tellg() / NUMBER_OF_PROCESSES / 2;
	input.seekg(0, std::wifstream::beg);

	// Записываем разбитые кусочки текста сюда
	std::wstring partName[NUMBER_OF_PROCESSES];

	if (input.good()) {
		for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
			//GetCurrentDirectory();
			
			partName[i] = L"..\\part" + std::to_wstring(i) + L".txt";

			std::wofstream output(partName[i], std::wofstream::out);
			output.imbue(std::locale(output.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

			while (!input.eof() && offset < (i + 1) * size) {
				std::wstring word;
				input >> word;
				offset += word.length() + 1;
				output << word + L" ";
			}
			output << L"\0";
			output.close();

			SetEvent(dataIsReadyEvent[i]);

		}
		input.close();
	} else {
		std::wcout << L"Failed to read text" << std::endl;
		exit(1);
	}
}

void JoinText() {
	std::wstring fileName = GetArgument(3, " Result file path");

	std::wofstream output(fileName, std::wofstream::out);
	output.imbue(std::locale(output.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

	// Записываем разбитые кусочки текста сюда
	std::wstring partName[NUMBER_OF_PROCESSES];

	for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
		partName[i] = L"..\\part" + std::to_wstring(i) + L".txt";

		std::wifstream input(partName[i], std::wifstream::in);
		input.imbue(std::locale(input.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

		while (!input.eof()) {
			std::wstring word;
			input >> word;
			output << word + L" ";
		}
	}
	output << L"\0";
	output.close();

}

int main(int argc, char* argv[]) {
	SECURITY_ATTRIBUTES sec;
	sec.nLength = sizeof(SECURITY_ATTRIBUTES);
	sec.bInheritHandle = TRUE;
	sec.lpSecurityDescriptor = NULL;
	
	for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
		terminationEvent[i] = CreateEvent(&sec, FALSE, FALSE, (L"Global\\TerminateEvent" + std::to_wstring(i)).c_str());
		dataIsReadyEvent[i] = CreateEvent(&sec, FALSE, FALSE, (L"Global\\DataIsReadyEvent" + std::to_wstring(i)).c_str());
		workIsDoneEvent[i] = CreateEvent(&sec, FALSE, FALSE, (L"Global\\WorkIsDoneEvent" + std::to_wstring(i)).c_str());

		std::wcout << dataIsReadyEvent[i] << L" Global\\DataIsReadyEvent" + std::to_wstring(i)  << std::endl;
	}
	std::wcout << std::endl;

	CreateProcesses();
	SplitText();
	// Ожидание конца всех действий
	WaitForMultipleObjects(NUMBER_OF_PROCESSES, workIsDoneEvent, TRUE, INFINITE);
	JoinText();

	for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
		SetEvent(terminationEvent[i]);
	}

	return 0;
}