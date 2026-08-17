#pragma once

class Line
{
private:
	char* Text;
	int Length;
	int Capacity;
	void Resize();
public:
	Line();
	Line(const char* Data);
	Line(const Line& Other);
	~Line();
	Line& operator=(const Line& Other);
	void InsertCharacter(int Index, char Character);
	void DeleteCharacter(int Index);
	void AppendCharacter(char Character);
	char getCharacter(int Index) const;
	int getLength() const;
	const char* getText() const;
	void Clear();
	void SetCharacter(int Index, char Character);
};