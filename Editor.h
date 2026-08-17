#pragma once
#include "Document.h"
#include <iostream>
#include <vector>
#include <Windows.h>

class Editor
{
private:
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
	Document CurrentDocument;
	int CurrentChapter, CurrentSection, CurrentParagraph, CurrentLine, CurrentColumn, ConsoleRows, ConsoleColumns;
	string SearchWord;
	int SearchChapter, SearchSection, SearchParagraph, SearchLine, SearchColumn;
	enum Mode
	{
		Normal, Insert, Command
	};
	Mode CurrentMode;
	vector<Line> ClipBoard;
	vector<Document> Undo;
	vector<Document> Redo;
	bool Running;
	bool ShowLineNumbers;
	bool Selecting;
	bool ClipBoardIsLine;
	int SelectStartLine, SelectStartColumn, SelectEndLine, SelectEndColumn, MaxLineLength;
	string LastSearch;
	char LastCommand;
	void DrawUI();
	void Update();
	void LineChecker();
public:
	Editor();
	void Run();
	void NormalMode();
	void InsertMode();
	void CommandMode();
	void ConsoleSize(int& Rows, int& Columns);
	void SaveFile();
	void LoadFile();
	void DisplayDocument();
	void ReplaceWord(string OldWord, string NewWord);
	bool FindWord(string Word, bool Forward);
	void HighlightSelectedText();
	int TextStartColumn();
};