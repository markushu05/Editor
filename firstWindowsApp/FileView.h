#pragma once
#include <windows.h>
#include <string>

// Funktionsdeklarationen
void PrintHex(HWND hwnd, HWND hEdit);
void PrintBinary(HWND hwnd, HWND hEdit);
void PrintString(HWND hwnd, HWND hEdit);
void FillListViewWithContent(HWND hwndListView);
void InitListView(HWND hwndListView);