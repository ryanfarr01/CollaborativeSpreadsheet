
#include "spreadsheet.h"

spreadsheet::spreadsheet(std::string name)
{
  this->name = name;
  this->data.clear();
}

std::string spreadsheet::get_name()
{
  return this->name;
}

/*
 *Note: returns "" if contents do not exist
 */
std::string spreadsheet::get_cell(std::string cellName)
{
  if(data.count(cellName) != 0)
  {
    return data[cellName];
  }
  return "";
}

void spreadsheet::set_cell(std::string cellName, std::string cellContents)
{
  data[cellName] = cellContents;
}
