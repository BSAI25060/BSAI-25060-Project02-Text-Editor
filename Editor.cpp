#include "Editor.h"
#include <vector>
#include <fstream>
#include <conio.h>
#include <string>
#include <Windows.h>

Editor::Editor()
{
	CurrentChapter = 0;
	CurrentColumn = 0;
	CurrentLine = 0;
	CurrentParagraph = 0;
	CurrentSection = 0;
	CurrentMode = Normal;
	ShowLineNumbers = false;
	Selecting = false;
	SelectStartLine = 0;
	SelectStartColumn = 0;
	SelectEndLine = 0;
	SelectEndColumn = 0;
	SearchChapter = 0;
	SearchSection = 0;
	SearchParagraph = 0;
	SearchLine = 0;
	SearchColumn = -1;
	ClipBoardIsLine = true;
	LastCommand = '\0';
	LoadFile();
	if (CurrentDocument.GetChapterCount() == 0)
	{
		Chapter Ch;
		Section Sec;
		Paragraph ParaG;
		Line L;
		ParaG.Addline(L);
		Sec.AddParagraph(ParaG);
		Ch.AddSection(Sec);
		CurrentDocument.AddChapter(Ch);
	}
	ConsoleSize(ConsoleRows, ConsoleColumns);
	MaxLineLength = ConsoleColumns - 10;
	if (MaxLineLength < 10)
		MaxLineLength = 10;
}

void Editor::SaveFile()
{
	ofstream Fout("Document.txt");
	for (int i = 0; i < CurrentDocument.GetChapterCount(); i++)
	{
		for (int j = 0; j < CurrentDocument.GetChapter(i).GetSectionCount(); j++)
		{
			for (int k = 0; k < CurrentDocument.GetChapter(i).GetSection(j).GetParagraphCount(); k++)
			{
				for (int l = 0; l < CurrentDocument.GetChapter(i).GetSection(j).GetParagraph(k).GetLineCount(); l++)
				{
					Fout << CurrentDocument.GetChapter(i).GetSection(j).GetParagraph(k).GetLine(l).getText();
					Fout << endl;
				}
				Fout << endl;
			}
		}
	}
	Fout.close();
}

void Editor::LoadFile()
{
	ifstream Fin("Document.txt");
	CurrentDocument.Clear();
	Chapter NewChapter;
	Section NewSection;
	Paragraph NewParagraph;
	string Text;
	while (getline(Fin, Text))
	{
		if (Text == "")
		{
			if (NewParagraph.GetLineCount() > 0)
			{
				NewSection.AddParagraph(NewParagraph);
				NewParagraph.Clear();
			}
		}
		else
		{
			Line NewLine;
			for (int i = 0; i < Text.length(); i++)
			{
				NewLine.AppendCharacter(Text[i]);
			}
			NewParagraph.Addline(NewLine);
		}
	}
	if (NewParagraph.GetLineCount() > 0)
	{
		NewSection.AddParagraph(NewParagraph);
	}
	if (NewSection.GetParagraphCount() > 0)
	{
		NewChapter.AddSection(NewSection);
	}
	if (NewChapter.GetSectionCount() > 0)
	{
		CurrentDocument.AddChapter(NewChapter);
	}
	Fin.close();
}

void Editor::DrawUI()
{
	system("cls");
	int LastRow = ConsoleRows - 1;
	int LastColumn = ConsoleColumns - 1;
	int FRow = ConsoleRows - 2;
	int SRow = ConsoleRows - 3;
	color(11);
	LineFunction(0, 0, 0, LastColumn);
	LineFunction(LastRow, 0, LastRow, LastColumn);
	LineFunction(0, 0, LastRow, 0);
	LineFunction(0, LastColumn, LastRow, LastColumn);
	LineFunction(2, 0, 2, LastColumn);
	LineFunction(SRow, 0, SRow, LastColumn);
	gotoRowCol(1, ConsoleColumns / 2 - 6);
	cout << "TEXT EDITOR ";
	color(14);
	gotoRowCol(FRow, 2);
	cout << "Mode: ";
	if (CurrentMode == Normal)
		cout << "Normal ";
	else if (CurrentMode == Insert)
		cout << "Insert ";
	else
		cout << "Command ";
	cout << "    ";
	cout << "Line: " << CurrentLine + 1;
	cout << "   Column: " << CurrentColumn + 1;
	color(7);
}

void Editor::Update()
{
	int FRow = ConsoleRows - 2;
	color(14);
	gotoRowCol(FRow, 2);
	cout << "Mode: ";
	if (CurrentMode == Normal)
		cout << "Normal ";
	else if (CurrentMode == Insert)
		cout << "Insert ";
	else
		cout << "Command ";
	cout << "    ";
	cout << "Line: " << CurrentLine + 1;
	cout << "   Column: " << CurrentColumn + 1;
	color(7);
	gotoRowCol(3 + CurrentLine, TextStartColumn() + CurrentColumn);
}

void Editor::LineChecker()
{
	Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
	Line& Current = P.GetLine(CurrentLine);
	if (Current.getLength() <= MaxLineLength)
	{
		return;
	}
	Line Newline;
	while (Current.getLength() > MaxLineLength)
	{
		Newline.AppendCharacter(Current.getCharacter(MaxLineLength));
		Current.DeleteCharacter(MaxLineLength);
	}
	P.InsertLine(CurrentLine + 1, Newline);
	if (CurrentColumn > MaxLineLength)
	{
		CurrentLine++;
		CurrentColumn = Newline.getLength();
	}
}

void Editor::DisplayDocument()
{
	DrawUI();
	Paragraph& ParaG = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
	for (int i = 0; i < ParaG.GetLineCount(); i++)
	{
		gotoRowCol(3 + i, 4);
		if (ShowLineNumbers)
		{
			int LineNumber = i + 1;
			if (LineNumber < 10)
			{
				cout << "  " << LineNumber << " ";
			}
			else if (LineNumber < 100)
				cout << " " << LineNumber << " ";
			else
				cout << LineNumber << " ";
		}
		cout << ParaG.GetLine(i).getText();
	}
}

void Editor::ReplaceWord(string OldWord, string NewWord)
{
	Undo.push_back(CurrentDocument);
	Redo.clear();
	for (int i = 0; i < CurrentDocument.GetChapterCount(); i++)
	{
		Chapter& Ch = CurrentDocument.GetChapter(i);
		for (int j = 0; j < Ch.GetSectionCount(); j++)
		{
			Section& Sec = Ch.GetSection(j);
			for (int k = 0; k < Sec.GetParagraphCount(); k++)
			{
				Paragraph& ParaG = Sec.GetParagraph(k);
				for (int l = 0; l < ParaG.GetLineCount(); l++)
				{
					Line& Current = ParaG.GetLine(l);
					for (int m = 0; m < Current.getLength(); m++)
					{
						bool Match = true;
						for (int n = 0; n < OldWord.length(); n++)
						{
							if (m + n >= Current.getLength())
							{
								Match = false;
								break;
							}
							if (Current.getCharacter(m + n) != OldWord[n])
							{
								Match = false;
								break;
							}
						}
						if (Match)
						{
							for (int n = 0; n < OldWord.length(); n++)
							{
								Current.DeleteCharacter(m);
							}
							for (int n = 0; n < NewWord.length(); n++)
							{
								Current.InsertCharacter(m + n, NewWord[n]);
							}
							m = m + NewWord.length() - 1;
						}
					}
				}
			}
		}
	}
}

void Editor::Run()
{
	DrawUI();
	DisplayDocument();
	Running = true;
	while (Running)
	{
		if (CurrentMode == Normal)
		{
			NormalMode();
		}
		else if (CurrentMode == Insert)
		{
			InsertMode();
		}
		else
			CommandMode();
	}
}

void Editor::NormalMode()
{
	char Input = _getch();
	switch (Input)
	{
	case 'i':
		CurrentMode = Insert;
		Update();
		break;
	case ':':
		CurrentMode = Command;
		Update();
		break;
	case 'h':
		if (CurrentColumn > 0)
		{
			CurrentColumn--;
			Update();
		}
		break;
	case 'l':
	{
		Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
		if (CurrentColumn < Current.getLength() - 1)
		{
			CurrentColumn++;
			Update();
		}
		break;
	}
	case 'j':
	{
		Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
		if (CurrentLine < P.GetLineCount() - 1)
		{
			CurrentLine++;
			CurrentColumn = 0;
			Update();
		}
		break;
	}
	case 'k':
		if (CurrentLine > 0)
		{
			CurrentLine--;
			Update();
		}
		CurrentColumn = 0;
		break;
	case 'w':
	{
		Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
		while (CurrentColumn < Current.getLength() and Current.getCharacter(CurrentColumn) != ' ')
		{
			CurrentColumn++;
			Update();
		}
		while (CurrentColumn < Current.getLength() and Current.getCharacter(CurrentColumn) == ' ')
		{
			CurrentColumn++;
			Update();
		}
		break;
	}
	case 'b':
	{
		Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
		while (CurrentColumn > 0 and Current.getCharacter(CurrentColumn - 1) == ' ')
		{
			CurrentColumn--;
			Update();
		}
		while (CurrentColumn > 0 and Current.getCharacter(CurrentColumn - 1) != ' ')
		{
			CurrentColumn--;
			Update();
		}
		break;
	}
	case '0':
		CurrentColumn = 0;
		Update();
		break;
	case '$':
	{
		Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
		CurrentColumn = Current.getLength() > 0 ? Current.getLength() - 1 : 0;
		Update();
		break;
	}
	case 'g':
	{
		char Next = _getch();
		if (Next == 'g')
		{
			CurrentChapter = 0;
			CurrentColumn = 0;
			CurrentLine = 0;
			CurrentParagraph = 0;
			CurrentSection = 0;
			DisplayDocument();
			Update();
		}
		break;
	}
	case 'G':
	{
		CurrentChapter = CurrentDocument.GetChapterCount() - 1;
		CurrentSection = CurrentDocument.GetChapter(CurrentChapter).GetSectionCount() - 1;
		CurrentParagraph = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraphCount() - 1;
		Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
		CurrentLine = P.GetLineCount() - 1;
		int Length = P.GetLine(CurrentLine).getLength();
		CurrentColumn = Length > 0 ? Length - 1 : 0;
		DisplayDocument();
		Update();
		break;
	}
	case 'd':
	{
		char Next = _getch();
		if (Next == 'd')
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
			P.DeleteLine(CurrentLine);
			if (P.GetLineCount() == 0)
				P.Addline(Line());
			if (CurrentLine >= P.GetLineCount())
				CurrentLine = P.GetLineCount() - 1;
			CurrentColumn = 0;
			DisplayDocument();
			Update();
		}
		break;
	}
	case 'D':
	{
		Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
		Undo.push_back(CurrentDocument);
		Redo.clear();
		int OldLength = Current.getLength();
		while (CurrentColumn < Current.getLength())
		{
			Current.DeleteCharacter(CurrentColumn);
		}
		gotoRowCol(3 + CurrentLine, TextStartColumn());
		cout << Current.getText();
		for (int i = Current.getLength(); i < OldLength; i++)
		{
			cout << " ";
		}
		Update();
		break;
	}
	case 'y':
	{
		char Next = _getch();
		if (Next == 'y')
		{
			ClipBoard.clear();
			ClipBoard.push_back(CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine));
			ClipBoardIsLine = true;
		}
		break;
	}
	case 'p':
	{
		if (!ClipBoard.empty())
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).InsertLine(CurrentLine + 1, ClipBoard[0]);
			DisplayDocument();
			Update();
		}
		break;
	}
	case 'P':
	{
		if (!ClipBoard.empty())
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).InsertLine(CurrentLine, ClipBoard[0]);
			DisplayDocument();
			Update();
		}
		break;
	}
	case '~':
	{
		Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
		char CH = Current.getCharacter(CurrentColumn);
		if (CH >= 'a' and CH <= 'z')
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			Current.SetCharacter(CurrentColumn, CH - 32);
		}
		else if (CH >= 'A' and CH <= 'Z')
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			Current.SetCharacter(CurrentColumn, CH + 32);
		}
		if (ShowLineNumbers)
		{
			gotoRowCol(3 + CurrentLine, 8);
		}
		else
			gotoRowCol(3 + CurrentLine, 4);
		cout << Current.getText() << " ";
		Update();
		break;
	}
	case 'u':
	{
		if (!Undo.empty())
		{
			Redo.push_back(CurrentDocument);
			CurrentDocument = Undo.back();
			Undo.pop_back();
			Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
			if (CurrentLine >= P.GetLineCount())
				CurrentLine = P.GetLineCount() - 1;
			DisplayDocument();
			Update();
		}
		break;
	}
	case 18:
	{
		if (!Redo.empty())
		{
			Undo.push_back(CurrentDocument);
			CurrentDocument = Redo.back();
			Redo.pop_back();
			Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
			if (CurrentLine >= P.GetLineCount())
			{
				CurrentLine = P.GetLineCount() - 1;
			}
			DisplayDocument();
			Update();
		}
		break;
	}
	case 'x':
	{
		Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
		if (CurrentColumn < Current.getLength())
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			Current.DeleteCharacter(CurrentColumn);
			LastCommand = 'x';
			if (ShowLineNumbers)
			{
				gotoRowCol(3 + CurrentLine, 8);
			}
			else
				gotoRowCol(3 + CurrentLine, 4);
			cout << Current.getText() << " ";
			Update();
		}
		break;
	}
	case '.':
	{
		if (LastCommand == 'x')
		{
			Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
			if (CurrentColumn < Current.getLength())
			{
				Undo.push_back(CurrentDocument);
				Redo.clear();
				Current.DeleteCharacter(CurrentColumn);
				LastCommand = 'x';
				gotoRowCol(3 + CurrentLine, 4);
				cout << Current.getText() << " ";
				Update();
			}
		}
		break;
	}
	case 4:
	{
		CurrentLine = CurrentLine + 10;
		if (CurrentLine >= CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLineCount())
		{
			CurrentLine = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLineCount() - 1;
		}
		Update();
		break;
	}
	case 21:
	{
		CurrentLine = CurrentLine - 10;
		if (CurrentLine < 0)
		{
			CurrentLine = 0;
		}
		Update();
		break;
	}
	}
}

void Editor::HighlightSelectedText()
{
	Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
	int Start = -1;
	int End = -1;
	if (Selecting)
	{
		Start = (SelectStartColumn < SelectEndColumn) ? SelectStartColumn : SelectEndColumn;
		End = (SelectStartColumn > SelectEndColumn) ? SelectStartColumn : SelectEndColumn;
	}
	if (ShowLineNumbers)
	{
		gotoRowCol(3 + CurrentLine, 8);
	}
	else
		gotoRowCol(3 + CurrentLine, 4);
	for (int i = 0; i < Current.getLength(); i++)
	{
		if (Selecting and i >= Start and i <= End)
			color(112);
		else
			color(7);
		cout << Current.getCharacter(i);
	}
	color(7);
	cout << " ";
	if (ShowLineNumbers)
	{
		gotoRowCol(3 + CurrentLine, 8 + CurrentColumn);
	}
	else
		gotoRowCol(3 + CurrentLine, 4 + CurrentColumn);
}

int Editor::TextStartColumn()
{
	if (ShowLineNumbers)
		return 8;
	else
		return 4;
}

void Editor::InsertMode()
{
	char Input = _getch();
	if (Input == 0 or Input == -32)
	{
		char Special = _getch();
		if (Special == 77 or Special == 116)
		{
			bool ShiftHeld = (Special == 116);
			if (ShiftHeld and !Selecting)
			{
				Selecting = true;
				SelectStartLine = CurrentLine;
				SelectStartColumn = CurrentColumn;
			}
			if (CurrentColumn < CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine).getLength())
			{
				CurrentColumn++;
				Update();
			}
			if (ShiftHeld)
			{
				SelectEndLine = CurrentLine;
				SelectEndColumn = CurrentColumn;
			}
			else
				Selecting = false;
			HighlightSelectedText();
			return;
		}
		if (Special == 75 or Special == 115)
		{
			bool ShiftHeld = (Special == 115);
			if (ShiftHeld and !Selecting)
			{
				Selecting = true;
				SelectStartLine = CurrentLine;
				SelectStartColumn = CurrentColumn;
			}
			if (CurrentColumn > 0)
			{
				CurrentColumn--;
				Update();
			}
			if (ShiftHeld)
			{
				SelectEndLine = CurrentLine;
				SelectEndColumn = CurrentColumn;
			}
			else
				Selecting = false;
			HighlightSelectedText();
			return;
		}
		if (Special == 72)
		{
			Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
			if (CurrentLine > 0)
			{
				CurrentLine--;
				int Length = P.GetLine(CurrentLine).getLength();
				if (CurrentColumn > Length)
					CurrentColumn = Length;
				Update();
			}
		}
		if (Special == 80)
		{
			Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
			if (CurrentLine < P.GetLineCount() - 1)
			{
				CurrentLine++;
				int Length = P.GetLine(CurrentLine).getLength();
				if (CurrentColumn > Length)
					CurrentColumn = Length;
				Update();
			}
		}
		if (Special == 83)
		{
			if (CurrentColumn < CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine).getLength())
			{
				Undo.push_back(CurrentDocument);
				Redo.clear();
				Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
				Current.DeleteCharacter(CurrentColumn);
				gotoRowCol(3 + CurrentLine, TextStartColumn());
				cout << Current.getText() << " ";
				gotoRowCol(3 + CurrentLine, TextStartColumn() + CurrentColumn);
				Update();
			}
		}
		if (Special == 15)
		{
			Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
			if (Current.getLength() >= 4)
			{
				bool Spaces = true;
				for (int i = 0; i < 4; i++)
				{
					if (Current.getCharacter(i) != ' ')
					{
						Spaces = false;
					}
				}
				if (Spaces)
				{
					Undo.push_back(CurrentDocument);
					Redo.clear();
					for (int i = 0; i < 4; i++)
					{
						Current.DeleteCharacter(0);
					}
					CurrentColumn = (CurrentColumn >= 4) ? CurrentColumn - 4 : 0;
					gotoRowCol(3 + CurrentLine, 4);
					cout << Current.getText() << "    ";
					Update();
				}
			}
		}
		return;
	}
	if (Input == 3)
	{
		if (Selecting)
		{
			int Start = (SelectStartColumn < SelectEndColumn) ? SelectStartColumn : SelectEndColumn;
			int End = (SelectStartColumn > SelectEndColumn) ? SelectStartColumn : SelectEndColumn;
			Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
			Line Temp;
			for (int i = Start; i <= End and i < Current.getLength(); i++)
			{
				Temp.AppendCharacter(Current.getCharacter(i));
			}
			ClipBoard.clear();
			ClipBoard.push_back(Temp);
			ClipBoardIsLine = false;
			Selecting = false;
		}
		return;
	}
	if (Input == 24)
	{
		if (Selecting)
		{
			int Start = (SelectStartColumn < SelectEndColumn) ? SelectStartColumn : SelectEndColumn;
			int End = (SelectStartColumn > SelectEndColumn) ? SelectStartColumn : SelectEndColumn;
			Line& Current = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph).GetLine(CurrentLine);
			Line Temp;
			for (int i = Start; i <= End and i < Current.getLength(); i++)
			{
				Temp.AppendCharacter(Current.getCharacter(i));
			}
			ClipBoard.clear();
			ClipBoard.push_back(Temp);
			ClipBoardIsLine = false;
			Undo.push_back(CurrentDocument);
			Redo.clear();
			int OldLength = Current.getLength();
			for (int i = Start; i <= End and Start < Current.getLength(); i++)
			{
				Current.DeleteCharacter(Start);
			}
			CurrentColumn = Start;
			Selecting = false;
			gotoRowCol(3 + CurrentLine, 4);
			cout << Current.getText();
			for (int i = Current.getLength(); i < OldLength; i++)
			{
				cout << " ";
			}
			Update();
		}
		return;
	}
	if (Input == 22)
	{
		if (!ClipBoard.empty())
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
			if (ClipBoardIsLine)
				P.InsertLine(CurrentLine + 1, ClipBoard[0]);
			else
			{
				Line& Current = P.GetLine(CurrentLine);
				Line& F = ClipBoard[0];
				for (int i = 0; i < F.getLength(); i++)
				{
					Current.InsertCharacter(CurrentColumn + i, F.getCharacter(i));
				}
				CurrentColumn += F.getLength();
				LineChecker();
			}
			DisplayDocument();
			Update();
		}
		return;
	}
	switch (Input)
	{
	case 27:
	{
		CurrentMode = Normal;
		Selecting = false;
		Update();
		break;
	}
	case 8:
	{
		Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
		if (CurrentColumn > 0)
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			Line& Current = P.GetLine(CurrentLine);
			Current.DeleteCharacter(CurrentColumn - 1);
			CurrentColumn--;
			gotoRowCol(3 + CurrentLine, TextStartColumn());
			cout << Current.getText() << " ";
			gotoRowCol(3 + CurrentLine, TextStartColumn() + CurrentColumn);
			Update();
		}
		else if (CurrentLine > 0)
		{
			Undo.push_back(CurrentDocument);
			Redo.clear();
			Line& Previous = P.GetLine(CurrentLine - 1);
			Line& Current = P.GetLine(CurrentLine);
			CurrentColumn = Previous.getLength();
			for (int i = 0; i < Current.getLength(); i++)
			{
				Previous.AppendCharacter(Current.getCharacter(i));
			}
			P.DeleteLine(CurrentLine);
			CurrentLine--;
			LineChecker();
			DisplayDocument();
			Update();
		}
		break;
	}
	case 13:
	{
		Undo.push_back(CurrentDocument);
		Redo.clear();
		Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
		Line& Current = P.GetLine(CurrentLine);
		Line NewLine;
		while (CurrentColumn < Current.getLength())
		{
			NewLine.AppendCharacter(Current.getCharacter(CurrentColumn));
			Current.DeleteCharacter(CurrentColumn);
		}
		P.InsertLine(CurrentLine + 1, NewLine);
		CurrentLine++;
		CurrentColumn = 0;
		DisplayDocument();
		Update();
		break;
	}
	case 9:
	{
		Undo.push_back(CurrentDocument);
		Redo.clear();
		Paragraph& P1 = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
		for (int i = 0; i < 4; i++)
		{
			P1.GetLine(CurrentLine).InsertCharacter(CurrentColumn, ' ');
			CurrentColumn++;
		}
		LineChecker();
		DisplayDocument();
		Update();
		break;
	}
	}
	if (Input >= 32 and Input <= 126)
	{
		Undo.push_back(CurrentDocument);
		Redo.clear();
		Paragraph& P = CurrentDocument.GetChapter(CurrentChapter).GetSection(CurrentSection).GetParagraph(CurrentParagraph);
		int LinesBefore = P.GetLineCount();
		P.GetLine(CurrentLine).InsertCharacter(CurrentColumn, Input);
		CurrentColumn++;
		LineChecker();
		if (P.GetLineCount() != LinesBefore)
		{
			DisplayDocument();
		}
		else
		{
			Line& Current = P.GetLine(CurrentLine);
			gotoRowCol(3 + CurrentLine, TextStartColumn());
			cout << Current.getText();
			gotoRowCol(3 + CurrentLine, TextStartColumn() + CurrentColumn);
		}
		Update();
	}
}

void Editor::CommandMode()
{
	string Command;
	getline(cin, Command);
	if (Command == "w")
	{
		SaveFile();
	}
	else if (Command == "q")
	{
		Running = false;
	}
	else if (Command == "wq")
	{
		SaveFile();
		Running = false;
	}
	else if (Command == "q!")
	{
		Running = false;
	}
	else if (Command == "new")
	{
		Undo.push_back(CurrentDocument);
		Redo.clear();
		CurrentDocument.Clear();
		Chapter Ch;
		Section Sec;
		Paragraph ParaG;
		Line L;
		ParaG.Addline(L);
		Sec.AddParagraph(ParaG);
		Ch.AddSection(Sec);
		CurrentDocument.AddChapter(Ch);
		CurrentChapter = 0;
		CurrentSection = 0;
		CurrentParagraph = 0;
		CurrentLine = 0;
		CurrentColumn = 0;
		DisplayDocument();
	}
	else if (Command.substr(0, 3) == "set")
	{
		int SpacePosition = Command.find(' ');
		auto pos = find(Command.begin(), Command.end(), ' ');
		string Option = (!Command.empty() and pos != Command.end()) ? string(pos + 1, Command.end()) : "";
		if (Option == "number")
			ShowLineNumbers = true;
		else if (Option == "nonumber")
			ShowLineNumbers = false;
		DisplayDocument();
	}
	else if (Command.size() > 0 and Command[0] == '/')
	{
		SearchWord = Command.substr(1);
		FindWord(SearchWord, true);
		DisplayDocument();
		Update();
	}
	else if (Command.size() > 0 and Command[0] == '?')
	{
		SearchWord = Command.substr(1);
		FindWord(SearchWord, false);
		DisplayDocument();
		Update();
	}
	else if (Command == "N")
	{
		FindWord(SearchWord, false);
		DisplayDocument();
		Update();
	}
	else if (Command == "n")
	{
		FindWord(SearchWord, true);
		DisplayDocument();
		Update();
	}
	else if (Command.substr(0, 7) == "replace")
	{
		if (Command.length() > 8)
		{
			string Rest = Command.substr(8);
			auto It = Rest.begin();
			while (It != Rest.end() and *It != ' ')
				++It;
			if (It != Rest.end())
			{
				ReplaceWord(Rest.substr(0, It - Rest.begin()), Rest.substr((It - Rest.begin()) + 1));
				DisplayDocument();
			}
		}
	}
	else if (Command.substr(0, 2) == "%s" and Command.size() > 2 and Command[2] == '/')
	{
		if (Command.length() >= 3)
		{
			string Rest = Command.substr(3);
			auto I = Rest.begin();
			while (I != Rest.end() and *I != '/')
			{
				++I;
			}
			if (I != Rest.end())
			{
				string OldWord = Rest.substr(0, I - Rest.begin());
				auto Start = I;
				if (Start != Rest.end())
				{
					++Start;
					auto I2 = Start;
					while (I2 != Rest.end() and *I2 != '/')
						++I2;
					string NewWord = Rest.substr(Start - Rest.begin(), I2 - Start);
					if (!OldWord.empty())
					{
						ReplaceWord(OldWord, NewWord);
						DisplayDocument();
					}
				}
			}
		}
	}
	CurrentMode = Normal;
	Update();
}

void Editor::ConsoleSize(int& Rows, int& Columns)
{
	CONSOLE_SCREEN_BUFFER_INFO Info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
	Columns = Info.srWindow.Right - Info.srWindow.Left + 1;
	Rows = Info.srWindow.Bottom - Info.srWindow.Top + 1;
}

bool Editor::FindWord(string Word, bool Forward)
{
	int StartChapter = SearchChapter, StartSection = SearchSection, StartParagraph = SearchParagraph, Startline = SearchLine, StartColumn = SearchColumn;
	if (Forward)
	{
		for (int i = StartChapter; i < CurrentDocument.GetChapterCount(); i++)
		{
			Chapter& Ch = CurrentDocument.GetChapter(i);
			for (int j = (i == StartChapter ? StartSection : 0); j < Ch.GetSectionCount(); j++)
			{
				Section& S = Ch.GetSection(j);
				for (int k = (i == StartChapter and j == StartSection ? StartParagraph : 0); k < S.GetParagraphCount(); k++)
				{
					Paragraph& ParaG = S.GetParagraph(k);
					for (int l = (i == StartChapter and j == StartSection and k == StartParagraph ? Startline : 0); l < ParaG.GetLineCount(); l++)
					{
						Line& Current = ParaG.GetLine(l);
						int Start = (i == StartChapter and j == StartSection and k == StartParagraph and l == Startline) ? StartColumn + 1 : 0;
						for (int m = Start; m < Current.getLength(); m++)
						{
							bool Match = true;
							for (int n = 0; n < Word.length(); n++)
							{
								if (m + n >= Current.getLength())
								{
									Match = false;
									break;
								}
								if (Current.getCharacter(m + n) != Word[n])
								{
									Match = false;
									break;
								}
							}
							if (Match)
							{
								CurrentChapter = i;
								CurrentSection = j;
								CurrentParagraph = k;
								CurrentLine = l;
								CurrentColumn = m;
								SearchChapter = i;
								SearchSection = j;
								SearchParagraph = k;
								SearchLine = l;
								SearchColumn = m;
								return true;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (int i = StartChapter; i >= 0; i--)
		{
			Chapter& Ch = CurrentDocument.GetChapter(i);
			for (int j = (i == StartChapter ? StartSection : Ch.GetSectionCount() - 1); j >= 0; j--)
			{
				Section& S = Ch.GetSection(j);
				for (int k = (i == StartChapter and j == StartSection ? StartParagraph : S.GetParagraphCount() - 1); k >= 0; k--)
				{
					Paragraph& ParaG = S.GetParagraph(k);
					for (int l = (i == StartChapter and j == StartSection and k == StartParagraph ? Startline : ParaG.GetLineCount() - 1); l >= 0; l--)
					{
						Line& Current = ParaG.GetLine(l);
						int Start = (i == StartChapter and j == StartSection and k == StartParagraph and l == Startline) ? StartColumn - 1 : Current.getLength() - 1;
						for (int m = Start; m >= 0; m--)
						{
							bool Match = true;
							for (int n = 0; n < Word.length(); n++)
							{
								if (m + n >= Current.getLength())
								{
									Match = false;
									break;
								}
								if (Current.getCharacter(m + n) != Word[n])
								{
									Match = false;
									break;
								}
							}
							if (Match)
							{
								CurrentChapter = i;
								CurrentSection = j;
								CurrentParagraph = k;
								CurrentLine = l;
								CurrentColumn = m;
								SearchChapter = i;
								SearchSection = j;
								SearchParagraph = k;
								SearchLine = l;
								SearchColumn = m;
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}