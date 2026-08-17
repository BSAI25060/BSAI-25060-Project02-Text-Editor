#include "Chapter.h"
#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;

Chapter::Chapter()
{

}

void Chapter::AddSection(const Section& NewSection)
{
	Sections.push_back(NewSection);
}

void Chapter::InsertSection(int Index, const Section& NewSection)
{
	if (Index >= 0 and Index <= Sections.size())
	{
		Sections.insert(Sections.begin() + Index, NewSection);
	}
}

void Chapter::DeleteSection(int Index)
{
	if (Index >= 0 and Index < Sections.size())
	{
		Sections.erase(Sections.begin() + Index);
	}
}

Section& Chapter::GetSection(int Index)
{
	return Sections[Index];
}

int Chapter::GetSectionCount() const
{
	return Sections.size();
}

void Chapter::Clear()
{
	Sections.clear();
}