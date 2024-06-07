#pragma once

#include <string>
#include <map>

class TblTree
{
private:
	std::string Data;
	std::map<std::string, TblTree> Children;

public:
	TblTree& operator [] (std::string Str);
	TblTree& operator [] (int Index);

	void operator >> (std::string &Str);
	void operator << (std::string& Str);
	std::string operator () ();

	bool HasSubTable(int Index);
	bool HasSubTable(std::string Str);

	void Set(std::string Name, TblTree& Child);
	void SetData(std::string NewData);
};

