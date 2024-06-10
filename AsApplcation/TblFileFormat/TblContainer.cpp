#include "TblContainer.h"

TblContainer::TblContainer()
{
}

TblContainer::TblContainer(std::map<std::string, TblContainer> NewChildren)
{
    Children = NewChildren;
}


TblContainer& TblContainer::operator[](std::string Str)
{
    return Children[Str];
}

TblContainer& TblContainer::operator[](int Index)
{
    return Children[std::to_string(Index)];
}

void TblContainer::operator>>(std::string& Str)
{
    Str = Data;
}

void TblContainer::operator<<(std::string& Str)
{
    Str = Data;
}

void TblContainer::operator=(std::string& Str)
{
    Data = Str;
}

std::string TblContainer::operator () ()
{
    return Data;
}


bool TblContainer::HasSubTable(int Index)
{
    return HasSubTable(std::to_string(Index));
}

bool TblContainer::HasSubTable(std::string Str)
{
    return (Children.count(Str) > 0);
}


void TblContainer::Add(std::string Name, TblContainer& Child)
{
    Children[Name] = Child;
}

void TblContainer::SetData(std::string NewData)
{
    Data = NewData;
}

std::string TblContainer::GetData()
{
    return Data;
}

std::map<std::string, TblContainer> TblContainer::GetCopyOfChildren()
{
    return Children;
}
