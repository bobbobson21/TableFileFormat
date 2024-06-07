#include "TblTree.h"

TblTree& TblTree::operator[](std::string Str)
{
    return Children[Str];
}

TblTree& TblTree::operator[](int Index)
{
    return Children[std::to_string(Index)];
}

void TblTree::operator>>(std::string& Str)
{
    Str = Data;
}

void TblTree::operator<<(std::string& Str)
{
    Str = Data;
}

std::string TblTree::operator () ()
{
    return Data;
}

bool TblTree::HasSubTable(int Index)
{
    return HasSubTable(std::to_string(Index));
}

bool TblTree::HasSubTable(std::string Str)
{
    return (Children.count(Str) > 0);
}


void TblTree::Set(std::string Name, TblTree& Child)
{
    Children[Name] = Child;
}

void TblTree::SetData(std::string NewData)
{
    Data = NewData;
}
