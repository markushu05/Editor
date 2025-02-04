#include <windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "FileHandler.h"
#include <commctrl.h>

using namespace std;

// Initialisiert das ListView mit den erforderlichen Spalten
void InitListView(HWND hwndListView)
{
    LVCOLUMN lvColumn;
    ZeroMemory(&lvColumn, sizeof(lvColumn));
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvColumn.pszText = const_cast<LPWSTR>(L"Index");
    lvColumn.cx = 100;
    ListView_InsertColumn(hwndListView, 0, &lvColumn);

    lvColumn.pszText = const_cast<LPWSTR>(L"Hex (8 Bytes)");
    lvColumn.cx = 150;
    ListView_InsertColumn(hwndListView, 1, &lvColumn);

    lvColumn.pszText = const_cast<LPWSTR>(L"Binär (8 Bytes)");
    lvColumn.cx = 150;
    ListView_InsertColumn(hwndListView, 2, &lvColumn);

    lvColumn.pszText = const_cast<LPWSTR>(L"String");
    lvColumn.cx = 100;
    ListView_InsertColumn(hwndListView, 3, &lvColumn);
}

// Füllt das ListView mit dem Inhalt des Textes in verschiedenen Darstellungen
void FillListViewWithContent(HWND hwndListView)
{
    ListView_DeleteAllItems(hwndListView);

    if (fileContent.empty()) {
        MessageBox(hwndListView, L"Der Eingabetext ist leer.", L"Warnung", MB_OK | MB_ICONWARNING);
        return;
    }

    size_t textLength = fileContent.length();

    for (size_t i = 0; i < textLength; i += 8)
    {
        LVITEM lvItem;
        ZeroMemory(&lvItem, sizeof(lvItem));
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i / 8;
        lvItem.iSubItem = 0;
        lvItem.pszText = _wcsdup(to_wstring(i).c_str());
        ListView_InsertItem(hwndListView, &lvItem);

        wstringstream hexStream;
        hexStream.str(L"");
        for (size_t j = 0; j < 8 && (i + j) < textLength; ++j)
        {
            wchar_t ch = fileContent[i + j];
            hexStream << setw(2) << setfill(L'0') << hex << uppercase << (int)ch;
        }
        ListView_SetItemText(hwndListView, lvItem.iItem, 1, _wcsdup(hexStream.str().c_str()));

        wstringstream binStream;
        binStream.str(L"");
        for (size_t j = 0; j < 8 && (i + j) < textLength; ++j)
        {
            wchar_t ch = fileContent[i + j];
            for (int bit = 7; bit >= 0; --bit) {
                binStream << ((ch & (1 << bit)) ? L"1" : L"0");
            }
        }
        ListView_SetItemText(hwndListView, lvItem.iItem, 2, _wcsdup(binStream.str().c_str()));

        wstring strContent = L"";
        for (size_t j = 0; j < 8 && (i + j) < textLength; ++j)
        {
            wchar_t ch = fileContent[i + j];
            strContent += (ch >= 32 && ch <= 126 ? ch : L'.');
        }
        ListView_SetItemText(hwndListView, lvItem.iItem, 3, _wcsdup(strContent.c_str()));
    }
}

// Zeigt den Inhalt als Hex-Darstellung im Edit-Feld
void PrintHex(HWND hwnd, HWND hEdit)
{
    if (fileContent.empty()) {
        MessageBox(hwnd, L"Der Eingabetext ist leer.", L"Warnung", MB_OK | MB_ICONWARNING);
        return;
    }

    wstring hexOutput;

    for (size_t i = 0; i < fileContent.length(); ++i)
    {
        wchar_t ch = fileContent[i];
        wstringstream hexStream;
        hexStream << setw(2) << setfill(L'0') << hex << (int)ch;
        wstring hexStr = hexStream.str();

        hexOutput += hexStr + L" ";
    }

    SetWindowTextW(hEdit, hexOutput.c_str());
}

// Zeigt den Inhalt als Binär-Darstellung im Edit-Feld
void PrintBinary(HWND hwnd, HWND hEdit)
{
    if (fileContent.empty()) {
        MessageBox(hwnd, L"Der Eingabetext ist leer.", L"Warnung", MB_OK | MB_ICONWARNING);
        return;
    }

    wstring binaryOutput;

    for (wchar_t ch : fileContent)
    {
        wstringstream binaryStream;
        for (int i = 7; i >= 0; --i)
        {
            binaryStream << ((ch >> i) & 1);
        }

        binaryOutput += binaryStream.str() + L" ";
    }

    SetWindowTextW(hEdit, binaryOutput.c_str());
}

// Zeigt den ursprünglichen Inhalt im Edit-Feld als Text
void PrintString(HWND hwnd, HWND hEdit)
{
    if (fileContent.empty()) {
        MessageBox(hwnd, L"Der Eingabetext ist leer.", L"Warnung", MB_OK | MB_ICONWARNING);
        return;
    }

    SetWindowTextW(hEdit, fileContent.c_str());
}
