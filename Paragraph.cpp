#include "Paragraph.h"
#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;

Paragraph::Paragraph()
{

}

void Paragraph::Addline(const Line& NewLine)
{
	Lines.push_back(NewLine);
}

void Paragraph::InsertLine(int Index, const Line& NewLine)
{
	if (Index >= 0 and Index <= Lines.size())
	{
		Lines.insert(Lines.begin() + Index, NewLine);
	}
}

void Paragraph::DeleteLine(int Index)
{
	if (Index >= 0 and Index < Lines.size())
	{
		Lines.erase(Lines.begin() + Index);
	}
}

Line& Paragraph::GetLine(int Index)
{
	return Lines[Index];
}

int Paragraph::GetLineCount() const
{
	return Lines.size();
}

void Paragraph::Clear()
{
	Lines.clear();
}