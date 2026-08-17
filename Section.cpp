#include "Section.h"
#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;

Section::Section()
{

}

void Section::AddParagraph(const Paragraph& NewParagraph)
{
	Paragraphs.push_back(NewParagraph);
}

void Section::InsertParagraph(int Index, const Paragraph& NewParagraph)
{
	if (Index >= 0 and Index <= Paragraphs.size())
	{
		Paragraphs.insert(Paragraphs.begin() + Index, NewParagraph);
	}
}

void Section::DeleteParagraph(int Index)
{
	if (Index >= 0 and Index < Paragraphs.size())
	{
		Paragraphs.erase(Paragraphs.begin() + Index);
	}
}

Paragraph& Section::GetParagraph(int Index)
{
	return Paragraphs[Index];
}

int Section::GetParagraphCount() const
{
	return Paragraphs.size();
}

void Section::Clear()
{
	Paragraphs.clear();
}
