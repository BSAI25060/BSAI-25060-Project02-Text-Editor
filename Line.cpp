#include "Line.h"
#include <iostream>
#include <Windows.h>

void Line::Resize()
{
	if (Capacity == 0)
		Capacity = 2;
	else
		Capacity = Capacity * 2;
	char* Temp = new char[Capacity + 1];
	for (int i = 0; i < Length; i++)
	{
		Temp[i] = Text[i];
	}
	Temp[Length] = '\0';
	delete[] Text;
	Text = Temp;
}

Line::Line()
{
	Capacity = 2;
	Length = 0;
	Text = new char[Capacity + 1];
	Text[0] = '\0';
}

Line::Line(const char* Data)
{
	int Count = 0;
	while (Data[Count] != '\0')
	{
		Count++;
	}
	Length = Count;
	Capacity = Length;
	Text = new char[Capacity + 1];
	for (int i = 0; i < Length; i++)
	{
		Text[i] = Data[i];
	}
	Text[Length] = '\0';
}

Line::Line(const Line& Other)
{
	this->Length = Other.Length;
	this->Capacity = Other.Capacity;
	Text = new char[Other.Capacity + 1];
	for (int i = 0; i < Other.Length; i++)
	{
		Text[i] = Other.Text[i];
	}
	Text[Length] = '\0';
}

Line::~Line()
{
	delete[] Text;
}

Line& Line::operator=(const Line& Other)
{
	if (this == &Other)
	{
		return *this;
	}
	delete[] Text;
	this->Length = Other.Length;
	this->Capacity = Other.Capacity;
	Text = new char[Other.Capacity + 1];
	for (int i = 0; i < Other.Length; i++)
	{
		Text[i] = Other.Text[i];
	}
	Text[Length] = '\0';
	return *this;
}

void Line::InsertCharacter(int Index, char Character)
{
	if (Index < 0 or Index > Length)
	{
		return;
	}
	if (Length + 1 >= Capacity)
	{
		Resize();
	}
	for (int i = Length; i >= Index; i--)
	{
		Text[i + 1] = Text[i];
	}
	Text[Index] = Character;
	Length++;
	Text[Length] = '\0';
}

void Line::DeleteCharacter(int Index)
{
	if (Index < 0 or Index >= Length)
	{
		return;
	}
	for (int i = Index; i < Length; i++)
	{
		Text[i] = Text[i + 1];
	}
	Length--;
	Text[Length] = '\0';
}

void Line::AppendCharacter(char Character)
{
	if (Length + 1 >= Capacity)
	{
		Resize();
	}
	Text[Length] = Character;
	Length++;
	Text[Length] = '\0';
}

char Line::getCharacter(int Index) const
{
	if (Index < 0 or Index >= Length)
	{
		return '\0';
	}
	return Text[Index];
}

int Line::getLength() const
{
	return Length;
}

const char* Line::getText() const
{
	return Text;
}

void Line::Clear()
{
	delete[] Text;
	Text = nullptr;
	Length = 0;
	Capacity = 0;
}

void Line::SetCharacter(int Index, char Character)
{
	if (Index >= 0 and Index < Length)
	{
		Text[Index] = Character;
	}
}
