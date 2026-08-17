#pragma once
#include <vector>
#include <iostream>
#include "Paragraph.h"
#include <Windows.h>

class Section
{
private:
	vector<Paragraph> Paragraphs;
public:
	Section();
	void AddParagraph(const Paragraph& NewParagraph);
	void InsertParagraph(int Index, const Paragraph& NewParagraph);
	void DeleteParagraph(int Index);
	Paragraph& GetParagraph(int Index);
	int GetParagraphCount() const;
	void Clear();
};

