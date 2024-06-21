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
	static  std::string RemoveTillOrReplaceBasedOnCharCount(std::string EditIn, int CharCount, char CharToCountOrReplace = '\\', char CountCharReplace = '/' );

	std::vector<TblHeaderDataResult> ReadHeaderData();
	int ReadHeaderLength();

	TblCompactionLevel CurrentCompactionLevel = Tbl_CompactionLevel_Low;

	std::vector<std::string> ReadManyFilePaths;

public:
	void OpenR(char *Path);
	void OpenR(std::string Path);
	void OpenRM(std::vector<std::string> Paths);
	void OpenRM(std::vector<char*> Paths);

	void OpenW(char* Path, TblCompactionLevel CompactLevel = Tbl_CompactionLevel_Low);
	void OpenW(std::string Path, TblCompactionLevel CompactLevel = Tbl_CompactionLevel_Low);
	void OpenWM(); //write many and no you dont control what files it writes it will do that for you
	
	void CloseR();
	void CloseW();
	void CloseWM(); //fun fact this is excectly the same as OpenWM so if you wanted to you use this to open and close but thats wieard
	void CloseRM();

	std::map<std::string, std::string> ReadMap();
	TblContainer ReadTable();
	void ReadWriteManyExtract(std::string Path);
	void ReadWriteManyExtractInChuncks(std::string Path, int ChunkVolumeSize = 2000000); //this is to be used for extracting stored files

	void WriteMap(std::map<std::string, std::string> Map);
	void WriteTable(TblContainer Table);
	void WriteReadManyCombineInChuncks(int RootIndex = -1, int ChunkVolumeSize = 2000000); //Combines many files into one tbl file also note the root index tells us how may folders should be discareded from the start of all the paths  so if set to 3 and the file pat is something like c:/a/b/c/d/e/file the math it will encode into the file will be d/e/file and if set to -1 it will only be just file
	void WriteReadManyCombine(int RootIndex = -1); //Combines many files into one tbl file also note the root index tells us how may folders should be discareded from the start of all the paths  so if set to 3 and the file pat is something like c:/a/b/c/d/e/file the math it will encode into the file will be d/e/file and if set to -1 it will only be just file
};


