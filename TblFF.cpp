
#include <iostream>
#include "TblFileFormat\TblFile.h"

int main()
{
    std::cout << "Hello World!\n";

    TblFile TblFileWrite = TblFile();
    std::map<std::string, std::string> Map = std::map<std::string, std::string>();

    Map["test.txt"] = "12345678";
    Map["best.txt"] = "12345678";
    Map["rest.txt"] = "12345678";
    Map["vest.txt"] = "12345678";
    Map["verness.txt"] = "12345678";

    TblFileWrite.OpenW("");
    TblFileWrite.WriteMap(Map);
    //TblFileWrite.WriteMap(Map);
}

