#pragma once
#include <vector>
#include <iostream>
#include "Line.h"	
#include <Windows.h>
using namespace std;

class Paragraph
{
private:
	vector<Line> Lines;
public:
	Paragraph();
	void Addline(const Line& NewLine);
	void InsertLine(int Index, const Line& NewLine);
	void DeleteLine(int Index);
	Line& GetLine(int Index);
	int GetLineCount() const;
	void Clear();
};

