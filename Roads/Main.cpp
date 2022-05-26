#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
using namespace std;

struct Point {
	int x = 0, y = 0;
};
class Graph {
public:
	bool** roads;	
	Point* coords;	
	int	p, 
		d;	

	Graph(string path) {
		int x, y, from, to;
		ifstream file(path);
		if (file) {
			file >> p;
			roads = new bool*[p];
			for (int i = 0; i < p; i++) roads[i] = new bool[p];
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < p; j++) {
					roads[i][j] = false;
				}
			}
			coords = new Point[p];
			for (int i = 0; i < p; i++) {
				file >> x >> y;
				coords[i] = { x,y };
			}
			file >> d;
			for (int i = 0; i < d; i++) {
				file >> from >> to;
				roads[from - 1][to - 1] = true;
				roads[to - 1][from - 1] = true;
			}

			file.close();
		}
		else {
			MessageBox(NULL, L"Не удается открыть файл \"graph.txt\"! Проверьте наличие файла", L"Ошибка", 0);
			exit(0);
		}

	}
	//----------------------------------------------------------------------------------------------------------------------------------------
	bool Deactivate(int from, int to) {
		if (from <= p && to <= p && from > 0 && to > 0) {
			int count = 0;
			stack<int> s;
			bool* used = new bool[p];

			roads[from - 1][to - 1] = false;
			roads[to - 1][from - 1] = false;
			for (int i = 0; i < p; i++) used[i] = false;
			s.push(0);

			do {
				int cur = s.top();
				s.pop();
				used[cur] = true;
				for (int i = 0; i < p; i++) {
					if (cur == i) continue;
					if (roads[cur][i] && !used[i]) {
						s.push(i);
						used[i] = true;
					}
				}
				count++;
			} while (!s.empty());
			delete[] used;

			if (count < p) {
				roads[from - 1][to - 1] = true;
				roads[to - 1][from - 1] = true;
				return false;
			}
			else {
				d--;
				return true;
			}
		}
		else return false;
	}
};

//----------------------------------------------------------------------------------------------------------------------------------------
#define IDC_BUTTON1 100
#define IDC_EDIT1 101
#define IDC_EDIT2 102
const int diametr = 20;


Graph g("graph.txt");
HWND hWnd;
HWND button1;
HWND edit1, edit2;
static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Road Manager");
HINSTANCE hInst; // Это адрес, по которому модуль размещен в памяти
HDC DC;


void DrawGraph() {
	DC = GetDC(hWnd);
	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));   // Идет   
	HPEN OldPen = (HPEN)SelectObject(DC, Pen);           // закраска
	HBRUSH Brush = CreateSolidBrush(RGB(255, 0, 0));        // фоновым
	HBRUSH OldBrush = (HBRUSH)SelectObject(DC, Brush);         // цветом

	for (int i = 0; i < g.p; i++) {
		int	fromX = g.coords[i].x,
					fromY = g.coords[i].y;

		Ellipse(DC, fromX, fromY, fromX + diametr, fromY + diametr);

		wstring out = to_wstring(i+1);
		TextOut(DC, fromX, fromY - diametr, out.c_str(), out.length());

		for (int j = i; j < g.p; j++) {
			int	toX = g.coords[j].x,
						toY = g.coords[j].y;
			if (g.roads[i][j]) {
				MoveToEx(DC, fromX + diametr / 2, fromY + diametr / 2, NULL);
				LineTo(DC, toX + diametr / 2, toY + diametr / 2);
			}
		}
	}

	SelectObject(DC, OldPen); // Относится к созданию теневого контекста для двойной буферизации
	DeleteObject(Pen); // Удаляем ненужные системные объекты
	SelectObject(DC, OldBrush);
	DeleteObject(Brush); // Удаляем ненужные системные объекты

	ReleaseDC(hWnd, DC);
}


void button_click() {
	TCHAR buf1[6];
	TCHAR buf2[6];
	int from, to;
	GetWindowText(edit1, buf1, lstrlen(buf1));
	GetWindowText(edit2, buf2, lstrlen(buf2));
	from = stoi(buf1);
	to = stoi(buf2);
	if (g.Deactivate(from, to)) {
		MessageBox(hWnd, L"Дорога закрыта", L"Удалось!", 0);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	else MessageBox(hWnd, L"Такой дороги нет", L"Увы!", 0);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_PAINT:
		DC = BeginPaint(hWnd, &ps);
		DrawGraph();
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		switch (wParam) {
		case IDC_BUTTON1:
			button_click();
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

//2D графика на основе WinApi C++
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	hInst = hInstance;
	//Заполняем структуру класса окна
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	// Регистрируем класс окна
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	hInst = hInstance;
	// Создаем основное окно приложения
	hWnd = CreateWindow(    
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,            // Стиль окна 
		CW_USEDEFAULT, CW_USEDEFAULT,   // Константы позволяют задать значения по умолчанию для размеров и позиций окна
		800, 600,
		NULL,                         // Указатель на родительское окно NULL 
		NULL,                         // Используется меню класса окна 
		hInst,                        // Указатель на текущее приложение
		NULL                          // Передается в качестве lParam в событие WM_CREATE
	);
	edit1 = CreateWindow(
		L"edit",
		L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		0, 0,
		50, 20,
		hWnd,
		(HMENU)IDC_EDIT1,
		hInst,
		NULL
	);
	edit2 = CreateWindow(
		L"edit",
		L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, // Стили окон
		50, 0,
		50, 20,
		hWnd,
		(HMENU)IDC_EDIT2,
		hInst,
		NULL
	);
	button1 = CreateWindow(
		L"button",
		L"Закрыть дорогу",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		100, 0,
		150, 20,
		hWnd,
		(HMENU)IDC_BUTTON1,
		hInst,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}
	// Показываем наше окно
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// Выполняем цикл обработки сообщений до закрытия приложения
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}