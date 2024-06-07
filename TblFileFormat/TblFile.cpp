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
			HeaderReturnData.push_back(TblHeaderDataResult());
			std::vector<std::string> CurrentHeaderDataLine = SplitStringIntoVector(HeaderDataSplited[i], ",");
			int CurrentHeaderDataResultIndex = HeaderReturnData.size() - 1;

			HeaderReturnData[CurrentHeaderDataResultIndex].Key = CurrentHeaderDataLine[0]; //where am I and what am I
			HeaderReturnData[CurrentHeaderDataResultIndex].StartPos = stoi(CurrentHeaderDataLine[1]); //where are my contents
			HeaderReturnData[CurrentHeaderDataResultIndex].EndPos = stoi(CurrentHeaderDataLine[2]); //where are my contents + how big are they
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

	while (true)
	{
		char NewLetter;
		FileReader.read(&NewLetter, 1);

		if (NewLetter == '}') //found start of header //do this after adding letters to header string so we dont add it to header data string
		{
			return HeaderLengthCount;
		}

		HeaderLengthCount++;
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

void TblFile::OpenW(char* Path)
{
	FileWrite.open(Path, std::ios::binary);
}

void TblFile::OpenW(std::string Path)
{
	FileWrite.open(Path, std::ios::binary);
}


void TblFile::CloseR()
{
	FileReader.close();
}

void TblFile::CloseW()
{
	FileWrite.close();
}


std::map<std::string, std::string> TblFile::ReadMap()
{
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

TblTree TblFile::ReadTree()
{
	TblTree Root = TblTree();
	std::map<std::string, std::string> Map = TblFile::ReadMap();

	for (std::map<std::string, std::string>::iterator i = Map.begin(); i != Map.end(); ++i)
	{
		TblTree *LastNode = &Root;
		std::vector<std::string> Path = SplitStringIntoVector(i->first, "/");

		for (int o = 0; o < Path.size(); o++)
		{
			if (LastNode->HasSubTable(Path[o]) == false)
			{
				TblTree NextNode = TblTree();
				LastNode->Set(Path[o], NextNode);
			}
			LastNode = &LastNode->operator[](Path[o]);
		}

		LastNode->SetData(i->second);
	}

	return Root;
}

void TblFile::ReadExtract(std::string Path)
{
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

		system(("echo " + i->second + " >> " + (std::string(1, '"')) + CurrentPath + PathSplited[PathSplited.size() - 1] + (std::string(1, '"'))).c_str());
	}
}

void TblFile::WriteMap(std::map<std::string, std::string> Map)
{
	std::vector<std::string> FileDataOut = std::vector<std::string>();

	FileDataOut.push_back("{");
	FileDataOut.push_back("\n\n");

	int CurrentFileByteCout = 3; //new lines also this dosent take into a count the header data
	for (std::map<std::string, std::string>::iterator i = Map.begin(); i != Map.end(); ++i)
	{
		FileDataOut.push_back(i->second);

		std::string NewHeaderContent = (std::string(1, '"')) + i->first + (std::string(1, '"')) + ", " + std::to_string(CurrentFileByteCout) + ", " + std::to_string(CurrentFileByteCout + i->second.size()) + ";";
		FileDataOut[0] += NewHeaderContent;

		CurrentFileByteCout += i->second.size(); //+ 5; //add due to new lines
	}
	FileDataOut[0][FileDataOut[0].size() -1] = '}'; //ends header write

	std::string FileDataOutFinalStage = "";
	for (size_t i = 0; i < FileDataOut.size(); i++) //compresses vector of strings to single string
	{
		FileDataOutFinalStage += FileDataOut[i];
	}

	FileWrite.clear();
	FileWrite << FileDataOutFinalStage;
}
