// Editor.cpp : Defines the entry point for the application.

#include "framework.h"
#include "Editor.h"
#include "FileHandler.h"
#include "FileView.h"
#include <commctrl.h>s

#define MAX_LOADSTRING 100
#define IDC_EDIT 200 
#define IDC_LISTVIEW 400
HWND hEdit;  // Globales Handle für das Edit-Control
HWND hListView;
int wmId;
int currentPage = 0;

// Global Variables:
HINSTANCE hInst; 
WCHAR szTitle[MAX_LOADSTRING]; 
WCHAR szWindowClass[MAX_LOADSTRING];  

// Forward declarations:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Lade globale Strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FIRSTWINDOWSAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Initialisiere die Anwendung
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FIRSTWINDOWSAPP));

    MSG msg;

    // Haupt-Nachrichtenschleife:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

// Registriert die Fensterklasse
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FIRSTWINDOWSAPP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FIRSTWINDOWSAPP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Speichert Instanzhandle und erstellt das Hauptfenster
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Speichern des Instanzhandles

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Fenster-Prozedur: Behandelt Nachrichten für das Hauptfenster
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Hole die Größe des Clientbereichs
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        // Erstelle das Edit-Control
        hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            0, 0, rcClient.right, rcClient.bottom, hWnd, (HMENU)IDC_EDIT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        // Erstelle das ListView-Control
        hListView = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            10, 50, rcClient.right - 20, rcClient.bottom - 60, hWnd, (HMENU)IDC_LISTVIEW,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        // Initialisiere ListView
        InitListView(hListView);

        // ListView initial unsichtbar
        ShowWindow(hListView, SW_HIDE);
    }
    break;

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // Skalierung des Edit-Controls
        if (hEdit)
        {
            MoveWindow(hEdit, 0, 0, width, height, TRUE);
        }

        // Skalierung des ListView-Controls
        if (hListView)
        {
            MoveWindow(hListView, 0, 0, width, height, TRUE);
        }
    }
    break;

    case WM_COMMAND:
    {
        wmId = LOWORD(wParam);
        HMENU hMenu = GetMenu(hWnd);
        // Menüoptionen bearbeiten
        if (wmId == IDM_CHART) {
            EnableMenuItem(hMenu, IDM_SAVE, MF_GRAYED); // Deaktivieren von "Speichern"
        }
        else {
            EnableMenuItem(hMenu, IDM_SAVE, MF_ENABLED); // Aktivieren von "Speichern"
        }
        DrawMenuBar(hWnd);
        // Menübefehle verarbeiten
        switch (wmId)
        {
        case IDM_NEW:
        {
            // Neues Dokument erstellen
            int response = MessageBox(hWnd, L"Möchten Sie die Datei speichern?", L"Speichern", MB_YESNOCANCEL | MB_ICONQUESTION);

            if (response == IDYES) {
                HandleSaveCommand(hWnd, hEdit);
                SetWindowTextW(hEdit, L"");
                ListView_DeleteAllItems(hListView);
                ShowWindow(hEdit, SW_SHOW);
                ShowWindow(hListView, SW_HIDE);
                wcscpy_s(openedFilePath, L"");
            }
            else if (response == IDNO) {
                SetWindowTextW(hEdit, L"");
                ListView_DeleteAllItems(hListView);
                ShowWindow(hEdit, SW_SHOW);
                ShowWindow(hListView, SW_HIDE);
                wcscpy_s(openedFilePath, L"");
            }
        }
            
            break;
        case IDM_OPEN:
            // Datei öffnen
            ShowWindow(hEdit, SW_SHOW);
            ShowWindow(hListView, SW_HIDE);
            OpenTextFile(hWnd, hEdit);
            break;
        case IDM_SAVE:
            // Datei speichern
            ShowWindow(hEdit, SW_SHOW);
            ShowWindow(hListView, SW_HIDE);
            HandleSaveCommand(hWnd, hEdit);
            break;
        case IDM_CHART:
            // Wechsel zu ListView-Ansicht
            ShowWindow(hEdit, SW_HIDE);
            ShowWindow(hListView, SW_SHOW);
            FillListViewWithContent(hListView);
            currentPage = IDM_SAVE;
            break;
        case IDM_HEX:
            // Hex-Darstellung anzeigen
            ShowWindow(hEdit, SW_SHOW);
            ShowWindow(hListView, SW_HIDE);
            PrintHex(hWnd, hEdit);
            currentPage = IDM_HEX;
            break;
        case IDM_BINARY:
            // Binär-Darstellung anzeigen
            ShowWindow(hEdit, SW_SHOW);
            ShowWindow(hListView, SW_HIDE);
            PrintBinary(hWnd, hEdit);
            currentPage = IDM_BINARY;
            break;
        case IDM_CHAR:
            // Zeichen-Darstellung anzeigen
            ShowWindow(hEdit, SW_SHOW);
            ShowWindow(hListView, SW_HIDE);
            PrintString(hWnd, hEdit);
            currentPage = IDM_CHAR;
            break;
        case IDM_ABOUT:
            // "Über"-Dialog anzeigen
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
        {
            // Anwendung beenden
            int exitResponse = MessageBox(hWnd, L"Möchten Sie die Datei speichern?", L"Speichern", MB_YESNOCANCEL | MB_ICONQUESTION);

            if (exitResponse == IDYES) {
                HandleSaveCommand(hWnd, hEdit);
                DestroyWindow(hWnd);
            }
            else if (exitResponse == IDNO) {
                DestroyWindow(hWnd);
            }
        }
            
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Handler für das "Über"-Dialogfenster
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
