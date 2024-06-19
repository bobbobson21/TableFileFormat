
#include <iostream>
#include "TblFileFormat\TblFile.h"

int main() //to do : wite files with the file writer, OpenRM(vector<string>), WriteReadMany,
{
    std::cout << "Hello World!\n";

    TblFile TblFileWrite = TblFile();
    std::map<std::string, std::string> Map = std::map<std::string, std::string>();
    std::map<std::string, std::string> AltMap = std::map<std::string, std::string>();

    Map["test.txt"] = "@2345678";
    Map["best.txt"] = "1@345678";
    Map["rest.txt"] = "12@45678";
    Map["vest.txt"] = "123@5678";
    Map["verness.txt"] = "1234@678";

    TblContainer MainTable = TblContainer();
    TblContainer Blessed = TblContainer();
    TblContainer Nessed = TblContainer();

    TblContainer Test = TblContainer();
    TblContainer Nest = TblContainer();
    TblContainer Vest = TblContainer();

    Nessed.SetData("@234");
    Test.SetData("1@34");
    Nest.SetData("12@4");
    Vest.SetData("123@");

    Blessed.Add("Nessed", Nessed);
    MainTable.Add("Blessed", Blessed);
    MainTable.Add("Test", Test);
    MainTable.Add("Nest", Nest);
    MainTable.Add("Vest", Vest);

    TblFileWrite.OpenW("TableData.tbl", TblCompactionLevel::Tbl_CompactionLevel_Medium);
    TblFileWrite.WriteTable(MainTable);
    TblFileWrite.CloseW();

    TblFileWrite.OpenWM();
    TblFileWrite.OpenR("TableData.tbl");
    TblFileWrite.ReadWriteManyExtract("C:\\Users\\dsbkh\\OneDrive\\Desktop\\github projects\\TableFileFormat\\AsApplcation\\test\\");
    TblFileWrite.CloseR();
    TblFileWrite.CloseWM();
}

