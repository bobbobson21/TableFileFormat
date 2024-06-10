#pragma once

#include <string>
#include <map>

class TblContainer
{
private:
	std::string Data = "nil";
	std::map<std::string, TblContainer> Children;

public:
	TblContainer();
	TblContainer(std::map<std::string, TblContainer> NewChildren);

	TblContainer& operator [] (std::string Str);
	TblContainer& operator [] (int Index);

	void operator >> (std::string &Str);
	void operator << (std::string& Str);
	void operator = (std::string& Str);
	std::string operator () ();

	bool HasSubTable(int Index);
	bool HasSubTable(std::string Str);

	void Add(std::string Name, TblContainer& Child);
	void SetData(std::string NewData);
	std::string GetData();

	std::map<std::string, TblContainer> GetCopyOfChildren();
};

