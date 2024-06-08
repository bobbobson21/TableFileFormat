#pragma once

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "TblTree.h"

struct TblHeaderDataResult
{
	std::string Key = "";
	int StartPos = 0;
	int EndPos = 0;
};

class TblFile
{
private:
	std::ifstream FileReader;
	std::ofstream FileWrite;

	static std::vector<std::string> SplitStringIntoVector(std::string Str, std::string Spl);
	static std::string RemoveWhitespaceChars(std::string RemoveIn, char ToggleChar ='"');
	std::vector<TblHeaderDataResult> ReadHeaderData();
	int ReadHeaderLength();

public:
	void OpenR(char *Path);
	void OpenR(std::string Path);
	void OpenW(char* Path);
	void OpenW(std::string Path);
	
	void CloseR();
	void CloseW();

	std::map<std::string, std::string> ReadMap();
	TblTree ReadTree();
	void ReadExtract(std::string Path);

	void WriteMap(std::map<std::string, std::string> Map);
};


