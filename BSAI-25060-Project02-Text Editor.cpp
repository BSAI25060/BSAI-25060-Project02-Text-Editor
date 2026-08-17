#include <iostream>
#include "Line.h"
#include "Paragraph.h" 
#include "Section.h"
#include "Chapter.h"
#include "Document.h"
#include "Editor.h"
#include <Windows.h>
#include <conio.h>
using namespace std;

void getRowColbyLeftClick(int& rpos, int& cpos)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD Events;
    INPUT_RECORD InputRecord;
    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT |
        ENABLE_EXTENDED_FLAGS);
    do
    {
        ReadConsoleInput(hInput, &InputRecord, 1, &Events);
        if (InputRecord.Event.MouseEvent.dwButtonState ==

            FROM_LEFT_1ST_BUTTON_PRESSED)

        {
            cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
            rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
            break;
        }
    } while (true);
}

void gotoRowCol(int rpos, int cpos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = cpos;
    scrn.Y = rpos;
    SetConsoleCursorPosition(hOuput, scrn);
}

void color(int k)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, k);
}

void hideConsoleCursor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void LineFunction(int P1row, int P1column, int P2row, int P2column, char sym = -37, float Delta = 0.001)
{
    int Row, Column;
    for (float Alpha = 0; Alpha <= 1; Alpha += Delta)
    {
        Row = P1row * Alpha + P2row * (1 - Alpha);
        Column = P1column * Alpha + P2column * (1 - Alpha);

        if (P1row == P2row)
            Row = P1row;
        if (P1column == P2column)
            Column = P1column;
        gotoRowCol(Row, Column);
        cout << sym;
    }
}

int main()
{
    cout << "PRESS A KEY TO START TEXT EDITOR...!!!" << endl;
    _getch();
    system("cls");
    Editor E;
    E.Run();
    return 0;
}
