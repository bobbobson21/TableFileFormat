#pragma once

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "TblContainer.h"

enum TblCompactionLevel
{
	Tbl_CompactionLevel_Low, //easy for humans to understand where  things are and what thing there are
	Tbl_CompactionLevel_Medium, //harder for humans to understand
	Tbl_CompactionLevel_High, //very hard for humans to understand
};

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

	TblCompactionLevel CurrentCompactionLevel = Tbl_CompactionLevel_Low;

	void WriteLowCompaction(std::map<std::string, std::string> Map);
	void WriteMediumCompaction(std::map<std::string, std::string> Map);
	void WriteHighCompaction(std::map<std::string, std::string> Map);

public:
	void OpenR(char *Path);
	void OpenR(std::string Path);
	void OpenW(char* Path, TblCompactionLevel CompactLevel = Tbl_CompactionLevel_Low);
	void OpenW(std::string Path, TblCompactionLevel CompactLevel = Tbl_CompactionLevel_Low);
	
	void CloseR();
	void CloseW();

	std::map<std::string, std::string> ReadMap();
	TblContainer ReadTable();
	void ReadExtract(std::string Path);

	void WriteMap(std::map<std::string, std::string> Map);
	void WriteTable(TblContainer Table);

};


