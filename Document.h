#pragma once
#include <vector>
#include <iostream>
#include "Chapter.h"
#include <Windows.h>

class Document
{
private:
	vector<Chapter> Chapters;
public:
	Document();
	void AddChapter(const Chapter& NewChapter);
	void InsertChapter(int Index, const Chapter& NewChapter);
	void DeleteChapter(int Index);
	Chapter& GetChapter(int Index);
	int GetChapterCount() const;
	void Clear();
};

