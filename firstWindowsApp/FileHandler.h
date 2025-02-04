#pragma once
#include <windows.h>
#include <string>

extern std::wstring fileContent;
extern wchar_t openedFilePath[MAX_PATH];
// Deklaration der Funktion
void OpenTextFile(HWND hWnd, HWND hEdit);
void SaveTextFile(HWND hwnd, HWND hEdit, const wchar_t* filePath);
bool OpenSaveFileDialog(HWND hwnd, wchar_t* szFilePath, size_t maxLength);
void HandleSaveCommand(HWND hwnd, HWND hEdit);