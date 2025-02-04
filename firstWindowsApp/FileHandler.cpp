#include "FileHandler.h"
#include <commdlg.h>  // Für das Öffnen eines Datei-Dialogs
#include <fstream>    // Für das Lesen der Datei
#include <sstream>    // Für das Umwandeln des Textes
#include <string>
#include <iostream>
#include <iomanip>
#include "framework.h"
#include "Editor.h"
#include "FileHandler.h"
#include "FileView.h"
#include <commctrl.h>

using namespace std;

// Globale Variablen
wstring fileContent; 
wchar_t openedFilePath[MAX_PATH] = L"";

// Funktion zum Öffnen einer Textdatei und Anzeigen ihres Inhalts im Edit-Feld
void OpenTextFile(HWND hWnd, HWND hEdit)
{
    OPENFILENAME ofn;
    wchar_t szFile[MAX_PATH] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Textdateien (*.txt)\0*.txt\0Alle Dateien\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = L"txt";

    // Öffne den Datei-Dialog
    if (GetOpenFileName(&ofn))
    {
        wifstream file(szFile);
        if (!file)
        {
            MessageBox(hWnd, L"Fehler beim Öffnen der Datei!", L"Fehler", MB_OK | MB_ICONERROR);
            return;
        }

        // Lese den Inhalt der Datei in einen wstringstream und speichere ihn in fileContent
        wstringstream buffer;
        buffer << file.rdbuf();
        file.close();
        fileContent = buffer.str();

        // Setze den Inhalt der Datei in das Edit-Control
        SetWindowTextW(hEdit, fileContent.c_str());

        // Speichere den Pfad der geöffneten Datei
        wcsncpy_s(openedFilePath, MAX_PATH, ofn.lpstrFile, _TRUNCATE);
    }
}

// Funktion zum Konvertieren eines Hex-Strings in lesbaren Text
string ConvertHexToText(string& hex) {
    string result;
    istringstream iss(hex);
    string byte;

    // Iteriere über jedes Byte im Hex-String und wandle es in ein Zeichen um
    while (iss >> setw(2) >> byte) {
        char chr = static_cast<char>(stoi(byte, nullptr, 16));
        result += chr;
    }

    return result;
}

// Funktion zum Konvertieren eines Binär-Strings in lesbaren Text
string ConvertBinaryToText(const std::string& binary) {
    string result;
    istringstream iss(binary);
    string byte;

    // Iteriere über jedes Byte im Binär-String und wandle es in ein Zeichen um
    while (iss >> byte) {
        if (byte.size() == 8) {
            char chr = static_cast<char>(stoi(byte, nullptr, 2));
            result += chr;
        }
    }

    return result;
}

// Funktion zum Speichern des Texts im Edit-Control in eine Datei
void SaveTextFile(HWND hwnd, HWND hEdit, const wchar_t* filePath)
{
    DWORD dwSize = GetWindowTextLengthW(hEdit);
    if (dwSize == 0) return; // Keine Textdaten, daher abbrechen

    wchar_t* buffer = new wchar_t[dwSize + 1]; // Puffer für den Text
    GetWindowTextW(hEdit, buffer, dwSize + 1); // Text aus dem Edit-Control lesen

    // Umwandlung von wchar_t (UTF-16) nach std::string (UTF-8)
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL, NULL);
    string textContent(utf8Size, 0);
    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &textContent[0], utf8Size, NULL, NULL);

    delete[] buffer;

    // Wenn der Inhalt Hex oder Binär ist, konvertiere ihn in lesbaren Text
    if (currentPage == IDM_HEX) {
        textContent = ConvertHexToText(textContent);
    }
    else if (currentPage == IDM_BINARY) {
        textContent = ConvertBinaryToText(textContent);
    }

    // Öffne die Datei zum Schreiben
    HANDLE hFile = CreateFileW(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD written;
        WriteFile(hFile, textContent.c_str(), textContent.length(), &written, NULL);
        CloseHandle(hFile);
    }

    // Konvertiere den Text zurück nach UTF-16 und speichere ihn in fileContent
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, textContent.c_str(), -1, NULL, 0);
    fileContent.resize(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, textContent.c_str(), -1, &fileContent[0], size_needed);
}

// Funktion zum Öffnen des "Speichern unter"-Dialogs für das Speichern der Datei
bool OpenSaveFileDialog(HWND hwnd, wchar_t* szFilePath, size_t maxLength)
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFilePath;
    ofn.nMaxFile = (DWORD)maxLength;
    ofn.lpstrFilter = L"Textdateien (*.txt)\0*.txt\0Alle Dateien\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = L"Datei speichern unter...";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    // Zeige den Dialog an und prüfe, ob der Benutzer eine Datei ausgewählt hat
    if (GetSaveFileName(&ofn))
    {
        return true;
    }
    return false;
}

// Handler-Funktion für den 'Speichern'-Befehl
void HandleSaveCommand(HWND hwnd, HWND hEdit)
{
    if (openedFilePath[0] == L'\0') // Keine Datei geöffnet, daher "Speichern unter" Dialog
    {
        wchar_t filePath[MAX_PATH] = L"";
        if (OpenSaveFileDialog(hwnd, filePath, MAX_PATH))
        {
            // Speichere den Inhalt in die ausgewählte Datei
            SaveTextFile(hwnd, hEdit, filePath);
        }
        else
        {
            MessageBox(hwnd, L"Speichern abgebrochen", L"Fehler", MB_OK | MB_ICONERROR);
        }
    }
    else // Eine Datei ist geöffnet, daher im gleichen Pfad speichern
    {
        SaveTextFile(hwnd, hEdit, openedFilePath);
    }
}
