
#include "spreadsheet.h"

/*
 * Constructor
 *
 * Parameter: name of spreadsheet
 * Simply sets the name and clears all data in the map storage
 */
spreadsheet::spreadsheet(std::string name)
{
  this->name = name;
  this->data.clear();
}

/*
 * Function: get_name
 *
 * Parameter: none
 * Returns name of the spreadsheet
 */
std::string spreadsheet::get_name()
{
  return this->name;
}

/*
 * Function: get_cell
 *
 * Parameter: name of cell whose contents you want returned
 * Returns the cell contents if they exist. Otherwise the empty string is returend
 * Note: returns "" if contents do not exist
 */
std::string spreadsheet::get_cell(std::string cellName)
{
  if(data.count(cellName) != 0)
  {
    return data[cellName];
  }
  return "";
}

/*
 * Function: set_cell
 *
 * parameters: the name of the cell and the contents you want associated with it
 * Returns: 0 if there was a circular dependency or 1 if succeeded
 */
int spreadsheet::set_cell(std::string cellName, std::string cellContents)
{
  cellContents.erase(' ');
  if(cellContents[0] == '=')
  {
    data[cellName] = cellContents;
    return 1;
  }
  data[cellName] = cellContents;
  // Patterns for individual tokens
  //std::string lpPattern = "\(";
  //std::string rpPattern = "\)";
  //std::string opPattern = "[\+\-*/]";
  //std::string varPattern = "[a-zA-Z_](?: [a-zA-Z_]|\d)*";
  //std::string doublePattern = "(?: \d+\.\d* | \d*\.\d+ | \d+ ) (?: [eE][\+-]?\d+)?";
  //std::string spacePattern = "\s+";
  
  boost::regex reg("test");

  return 0;
}
