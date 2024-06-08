
#include <iostream>
#include "TblFileFormat\TblFile.h"

int main()
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

    TblFileWrite.OpenW("");
    TblFileWrite.WriteMap(Map);
    TblFileWrite.CloseW();

    TblFileWrite.OpenR("");
    Map = TblFileWrite.ReadMap();
    TblFileWrite.CloseR();
    //TblFileWrite.WriteMap(Map);
}

