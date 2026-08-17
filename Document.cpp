#include "Document.h"
#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;

Document::Document()
{

}

void Document::AddChapter(const Chapter& NewChapter)
{
	Chapters.push_back(NewChapter);
}

void Document::InsertChapter(int Index, const Chapter& NewChapter)
{
	if (Index >= 0 and Index <= Chapters.size())
	{
		Chapters.insert(Chapters.begin() + Index, NewChapter);
	}
}

void Document::DeleteChapter(int Index)
{
	if (Index >= 0 and Index < Chapters.size())
	{
		Chapters.erase(Chapters.begin() + Index);
	}
}

Chapter& Document::GetChapter(int Index)
{
	return Chapters[Index];
}

int Document::GetChapterCount() const
{
	return Chapters.size();
}

void Document::Clear()
{
	Chapters.clear();
}