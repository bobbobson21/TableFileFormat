#include "TblFile.h"

std::vector<std::string> TblFile::SplitStringIntoVector(std::string Str, std::string Spl)
{
	Str += Spl; //has to add a spl to the end or it will not return last split
	std::vector<std::string> ToReturn = std::vector<std::string>();

	while (Str.find_first_of(Spl) != std::string::npos) //keep spliting untill there are no more splits that can be made
	{
		ToReturn.push_back(Str.substr(0, Str.find_first_of(Spl))); //new split
		Str = Str.substr(Str.find_first_of(Spl) + 1, Str.length() - Str.find_first_of(Spl)); //so we dont repeat our selfs endlessly
	}

	return ToReturn;
}

std::string TblFile::RemoveWhitespaceChars(std::string RemoveIn, char ToggleChar) //this is useful for parsing thougth the header data
{
	bool IsRunning = true;

	for (int i = 0; i < RemoveIn.size(); i++)
	{
		if (RemoveIn[i] == ToggleChar) //stops remove in substring
		{
			IsRunning = !IsRunning;
		}

		if (IsRunning == true)
		{
			if (RemoveIn[i] == '\n' || RemoveIn[i] == '\r' || RemoveIn[i] == '\t' || RemoveIn[i] == ' ') //what needs to be removed
			{
				RemoveIn.erase(RemoveIn.begin() + i); //removal
				
				i--; //backtracks
				if (i < 0) { i = 0; } //stops backtracking to far
				if (RemoveIn.size() <= 0) { return RemoveIn; } //stops going to far ahead after
			}
		}
	}
	
	return RemoveIn;
}


std::vector<TblHeaderDataResult> TblFile::ReadHeaderData()
{
	std::string HeaderDataString = "";
	bool StartReadingHeaderData = false;

	while (true)
	{
		char NewLetter;
		FileReader.read(&NewLetter, 1);

		if (NewLetter == '}') //found end of header //do this befor adding letters to header string so we dont add } to header data string
		{
			if (StartReadingHeaderData == true)
			{
				StartReadingHeaderData = false;
				break;
			}
			else
			{
				throw "Invalid Header data. File may not be using the tbl file format."; //wtf how not use correct file format
			}
		}

		if (StartReadingHeaderData == true)
		{
			HeaderDataString += NewLetter;
		}

		if (NewLetter == '{') //found start of header //do this after adding letters to header string so we dont add it to header data string
		{
			StartReadingHeaderData = true; 
		}
	}

	HeaderDataString = RemoveWhitespaceChars(HeaderDataString);
	std::vector<TblHeaderDataResult> HeaderReturnData;
	std::vector<std::string> HeaderDataSplited = SplitStringIntoVector(HeaderDataString, ";");

	for (size_t i = 0; i < HeaderDataSplited.size(); i++)
	{
		try
		{
			
			std::vector<std::string> CurrentHeaderDataLine = SplitStringIntoVector(HeaderDataSplited[i], ",");

			if (CurrentHeaderDataLine.size() >= 3)
			{
				HeaderReturnData.push_back(TblHeaderDataResult());
				int CurrentHeaderDataResultIndex = HeaderReturnData.size() - 1;

				HeaderReturnData[CurrentHeaderDataResultIndex].Key = CurrentHeaderDataLine[0]; //where am I and what am I
				HeaderReturnData[CurrentHeaderDataResultIndex].StartPos = stoi(CurrentHeaderDataLine[1]); //where are my contents
				HeaderReturnData[CurrentHeaderDataResultIndex].EndPos = stoi(CurrentHeaderDataLine[2]); //where are my contents + how big are they
			}
		}
		catch(...)
		{
			throw "Invalid Header data. File may not be using the tbl file format.";
		}
	}

	return HeaderReturnData;
}

int TblFile::ReadHeaderLength()
{
	int HeaderLengthCount = 0;
	bool StartReadingHeaderLength = false;

	while (true)
	{
		char NewLetter;
		FileReader.read(&NewLetter, 1);

		if (NewLetter == '{') //found the start of the header
		{
			StartReadingHeaderLength = true;
		}

		if (StartReadingHeaderLength == true)
		{
			HeaderLengthCount++;
		}

		if (NewLetter == '}') //found end of headers size
		{
			return HeaderLengthCount;
		}
	}
}


void TblFile::OpenR(char* Path)
{
	FileReader.open(Path, std::ios::binary);
}

void TblFile::OpenR(std::string Path)
{
	FileReader.open(Path, std::ios::binary);
}

void TblFile::OpenRM(std::vector<std::string> Paths)
{
	ReadManyFilePaths = Paths;

	if (FileReader.is_open() == true)
	{
		FileReader.close();
	}
}

void TblFile::OpenRM(std::vector<char*> Paths)
{
	std::vector<std::string> OutPaths = std::vector<std::string>();

	for (int i = 0; i < Paths.size(); i++)
	{
		OutPaths.push_back("");

		for (int o = 0; Paths[i][o] != '\0'; o++)
		{
			OutPaths[i] += Paths[i][o];
		}

		delete[] Paths[i];
	}

	OpenRM(OutPaths);
}


void TblFile::OpenW(char* Path, TblCompactionLevel CompactLevel)
{
	FileWrite.open(Path, std::ios::binary);
	CurrentCompactionLevel = CompactLevel;
}

void TblFile::OpenW(std::string Path, TblCompactionLevel CompactLevel)
{
	FileWrite.open(Path, std::ios::binary);
	CurrentCompactionLevel = CompactLevel;
}

void TblFile::OpenWM()
{
	if (FileWrite.is_open() == true)
	{
		FileWrite.close();
	}
}


void TblFile::CloseR()
{
	FileReader.close();
}

void TblFile::CloseW()
{
	FileWrite.close();
}

void TblFile::CloseWM()
{
	OpenWM();
}

void TblFile::CloseRM()
{
	ReadManyFilePaths.clear();

	if (FileReader.is_open() == true)
	{
		FileReader.close();
	}
}


std::map<std::string, std::string> TblFile::ReadMap()
{
	FileReader.seekg(std::ios::beg);
	int HeaderLength = ReadHeaderLength();

	FileReader.seekg(std::ios::beg); //so we are back to the header
	std::vector<TblHeaderDataResult> Results = ReadHeaderData();

	std::map<std::string, std::string> ReturnData = std::map<std::string, std::string>();

	for (int i = 0; i < Results.size(); i++)
	{
		char* TempDataDelete = new char[Results[i].EndPos - Results[i].StartPos]; //creates space to read file info into
		std::string ResultName = Results[i].Key.substr(1, Results[i].Key.size() - 2);

		FileReader.seekg(Results[i].StartPos + HeaderLength);
		FileReader.read(TempDataDelete, Results[i].EndPos - Results[i].StartPos); //reads out file to valid format

		ReturnData[ResultName] = "";
		for (size_t o = 0; o < Results[i].EndPos - Results[i].StartPos; o++) //converts to desiered format
		{
			ReturnData[ResultName] += TempDataDelete[o];
		}

		delete[] TempDataDelete; //deletes temp data
	}
	return ReturnData;
}

TblContainer TblFile::ReadTable()
{
	TblContainer Root = TblContainer();
	std::map<std::string, std::string> Map = TblFile::ReadMap();

	for (std::map<std::string, std::string>::iterator i = Map.begin(); i != Map.end(); ++i)
	{
		TblContainer *LastNode = &Root;
		std::vector<std::string> Path = SplitStringIntoVector(i->first, "/");

		for (int o = 0; o < Path.size(); o++)
		{
			if (LastNode->HasSubTable(Path[o]) == false)
			{
				TblContainer NextNode = TblContainer();
				LastNode->Add(Path[o], NextNode);
			}
			LastNode = &LastNode->operator[](Path[o]);
		}

		LastNode->SetData(i->second);
	}

	return Root;
}

void TblFile::ReadWriteManyExtract(std::string Path)
{
	if (Path[Path.size() - 1] != '\\')
	{
		Path += "\\";
	}

	std::map<std::string, std::string> FileDataOut = ReadMap();

	for (std::map<std::string, std::string>::iterator i = FileDataOut.begin(); i != FileDataOut.end(); ++i)
	{
		std::vector<std::string> PathSplited = SplitStringIntoVector(i->first, "/");

		std::string CurrentPath = Path;

		for (int o = 0; o < PathSplited.size() -1; o++)
		{
			CurrentPath += PathSplited[o] + "\\";
			system(("mkdir " + (std::string(1, '"')) + CurrentPath + (std::string(1, '"'))).c_str());
		}

		FileWrite.open(CurrentPath + PathSplited[PathSplited.size() - 1]);
		FileWrite.clear();

		FileWrite.write(i->second.c_str(), i->second.size());
		FileWrite.close();
	}
}

void TblFile::ReadWriteManyExtractInChuncks(std::string Path, int ChunkVolumeSize) //this is to be used for extracting stored files
{
	if (Path[Path.size() - 1] != '\\')
	{
		Path += "\\";
	}

	FileReader.seekg(std::ios::beg);
	int HeaderLength = ReadHeaderLength();

	FileReader.seekg(std::ios::beg); //so we are back to the header
	std::vector<TblHeaderDataResult> Results = ReadHeaderData();

	for (int i = 0; i < Results.size(); i++) //makes all the folders to put files into
	{
		std::vector<std::string> PathSplited = SplitStringIntoVector(Results[i].Key, "/");

		std::string CurrentPath = Path; //path of last folder created ... used to make any folder to go in it
		for (int o = 0; o < PathSplited.size() - 1; o++) //dose the make foldey part
		{
			CurrentPath += PathSplited[o] + "\\";
			system(("mkdir " + (std::string(1, '"')) + CurrentPath + (std::string(1, '"'))).c_str()); //dose the real make foldy thingy
		}
	}

	for (int i = 0; i < Results.size(); i++)
	{
		int TotalSize = Results[i].EndPos - Results[i].StartPos;
		char* TempDataDelete = new char[min(TotalSize, ChunkVolumeSize)]; //creates space to read file info into

		FileReader.seekg(Results[i].StartPos + HeaderLength);

		FileWrite.open(Path + Results[i].Key.substr(1, Results[i].Key.size() -2), std::ios_base::app); //creates file to write to
		FileWrite.clear(); //if file already exsist empty it

		while (TotalSize > 0)
		{

			FileReader.read(TempDataDelete, min(TotalSize, ChunkVolumeSize)); //reads out file to valid format
			FileWrite.write(TempDataDelete, min(TotalSize, ChunkVolumeSize));

			TotalSize = TotalSize - ChunkVolumeSize;
		}

		FileWrite.close();
		delete[] TempDataDelete; //deletes temp data
	}
}


void TblFile::WriteMap(std::map<std::string, std::string> Map)
{
	std::vector<std::string> FileDataOut = std::vector<std::string>();

	FileDataOut.push_back("{");

	int CurrentFileByteCout = 0; //new lines also this dosent take into a count the header data

	switch (CurrentCompactionLevel)
	{
	case Tbl_CompactionLevel_Low: default:
		FileDataOut.push_back("\n\n");
		CurrentFileByteCout = 2;
		break;
	case Tbl_CompactionLevel_Medium:
		FileDataOut.push_back("\n\n");
		CurrentFileByteCout = 2;
		break;
	case Tbl_CompactionLevel_High:
		CurrentFileByteCout = 0;
		break;
	}

	for (std::map<std::string, std::string>::iterator i = Map.begin(); i != Map.end(); ++i)
	{
		FileDataOut.push_back(i->second);

		std::string NewHeaderContent = "";

		switch (CurrentCompactionLevel)
		{
		case Tbl_CompactionLevel_Low: default:
			NewHeaderContent = (std::string(1, '"')) + i->first + (std::string(1, '"')) + ", " + std::to_string(CurrentFileByteCout) + ", " + std::to_string(CurrentFileByteCout + i->second.size()) + "; ";
			break;
		case Tbl_CompactionLevel_Medium:
			NewHeaderContent = (std::string(1, '"')) + i->first + (std::string(1, '"')) + ", " + std::to_string(CurrentFileByteCout) + ", " + std::to_string(CurrentFileByteCout + i->second.size()) + "; ";
			break;
		case Tbl_CompactionLevel_High:
			NewHeaderContent = (std::string(1, '"')) + i->first + (std::string(1, '"')) + "," + std::to_string(CurrentFileByteCout) + "," + std::to_string(CurrentFileByteCout + i->second.size()) + ";";
			break;
		}

		FileDataOut[0] += NewHeaderContent;

		switch (CurrentCompactionLevel)
		{
		case Tbl_CompactionLevel_Low: default:
			CurrentFileByteCout += i->second.size() + 2;
			FileDataOut.push_back("\n\n");
			break;
		case Tbl_CompactionLevel_Medium:
			CurrentFileByteCout += i->second.size();
			break;
		case Tbl_CompactionLevel_High:
			CurrentFileByteCout += i->second.size();
			break;
		}
	}

	switch (CurrentCompactionLevel)
	{
	case Tbl_CompactionLevel_Low: default:
		FileDataOut[0][FileDataOut[0].size() - 2] = '}'; //ends header write
		FileDataOut[0].erase(FileDataOut[0].begin() + FileDataOut[0].size() - 1);  //gets rid of pointless space
		break;
	case Tbl_CompactionLevel_Medium:
		FileDataOut[0][FileDataOut[0].size() - 2] = '}'; //ends header write
		FileDataOut[0].erase(FileDataOut[0].begin() + FileDataOut[0].size() - 1);  //gets rid of pointless space
		break;
	case Tbl_CompactionLevel_High:
		FileDataOut[0][FileDataOut[0].size() - 1] = '}'; //ends header write
		FileDataOut[0].erase(FileDataOut[0].begin() + FileDataOut[0].size() - 1);  //gets rid of pointless space
		break;
	}

	std::string FileDataOutFinalStage = "";
	for (size_t i = 0; i < FileDataOut.size(); i++) //compresses vector of strings to single string
	{
		FileDataOutFinalStage += FileDataOut[i];
	}

	FileWrite.clear();
	FileWrite << FileDataOutFinalStage;
}

void TblFile::WriteTable(TblContainer Table)
{
	std::map<std::string, TblContainer> TableChildren = Table.GetCopyOfChildren();
	std::string PathAsSingleString = "";

	static std::map<std::string, std::string> TableMap;
	static std::vector<std::string> PathAsVec;
	static int Indentatation = 0;

	for (int i = 0; i < PathAsVec.size(); i++) //get my tree location into a single string
	{
		PathAsSingleString += PathAsVec[i] + "/";
	}


	Indentatation = Indentatation + 1; //how far down the stack are we
	for (std::map<std::string, TblContainer>::iterator i = TableChildren.begin(); i != TableChildren.end(); ++i)
	{
		if (i->second() != "nil")
		{
			TableMap[PathAsSingleString+i->first] = i->second();
		}

		PathAsVec.push_back(i->first);
		WriteTable(i->second);
		PathAsVec.erase(PathAsVec.begin() + (PathAsVec.size() - 1));
	}
	Indentatation = Indentatation - 1;

	if (Indentatation == 0) //everything has been intergrated into a single map write it
	{
		WriteMap(TableMap);

		PathAsVec.clear();
		TableMap.clear();
	}
}

void TblFile::WriteReadManyCombine()
{
	//well be done tomarrow
}
