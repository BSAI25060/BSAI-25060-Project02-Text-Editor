#pragma once
#include <vector>
#include <iostream>
#include "Section.h"
#include <Windows.h>

class Chapter
{
private:
	vector<Section> Sections;
public:
	Chapter();
	void AddSection(const Section& NewSection);
	void InsertSection(int Index, const Section& NewSection);
	void DeleteSection(int Index);
	Section& GetSection(int Index);
	int GetSectionCount() const;
	void Clear();
};

