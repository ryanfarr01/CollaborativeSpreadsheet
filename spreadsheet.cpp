
#include "spreadsheet.h"
#include <iostream> //DELETE THIS. This is only for testing /!\

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
 * Function: get_data_map
 *
 * Parameter: none
 * Returns map of spreadsheet data
 */
std::map<std::string, std::string> spreadsheet::get_data_map() 
{
    return this->data;
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
 * Function: has_circular_dependency
 * Parameters: check if c1 depends on c2
 * 
 * Return: 1 if there exists a dependency, 0 otherwise
 */
int spreadsheet::has_dependency(std::string c1, std::string c2)
{
  if(data.count(c2) != 0)
  {
    for(std::vector<std::string>::iterator it = dependencies[c2].begin(); it != dependencies[c2].end(); it++)
    {
      if(*it == c1) { return 1; }
      if(has_dependency(c1, *it)) { return 1; } 
    }
  }

  return 0;
}

/*
 * Function: set_cell
 *
 * parameters: the name of the cell and the contents you want associated with it
 * Returns: 0 if there was a circular dependency or 1 otherwise
 */
int spreadsheet::set_cell(std::string cellName, std::string cellContents)
{
  std::string originalContents = cellContents;
  cellName[0] = toupper(cellName[0]);
  std::vector<std::string> temp_depends;

  //Erase white space
  std::string::size_type position = 0;
  while((position = cellContents.find(" ", position)) != std::string::npos)
  {
    cellContents.erase(position, 1);
  }

  //Check for contents
  if(cellContents[0] == '=')
  {
    //Get all instances of cell names
    //Check if would cause circular dependency
    //Set cell contents or don't and return 1 or 0, respectively
    boost::regex reg("[a-zA-Z][1-9][1-9]?");
    
    boost::sregex_token_iterator it(cellContents.begin(), cellContents.end(), reg, 0);
    boost::sregex_token_iterator end;

    std::string t;
    for( ; it != end; ++it)
    {
      t = *it;
      t[0] = toupper(t[0]); //Convert to uppercase
	
      temp_depends.push_back(t);
    }

    //Testing /!\ *************************************
    std::cout << "setting cell:" << cellName << " to:" << cellContents << std::endl;
    std::cout << "With dependencies: " << std::endl;
    for(std::vector<std::string>::iterator itt = temp_depends.begin(); itt != temp_depends.end(); itt++)
      std::cout << *itt << std::endl;
    //End testing /!\ *********************************

    for(std::vector<std::string>::iterator itt = temp_depends.begin(); itt != temp_depends.end(); itt++)
    {
      if(has_dependency(cellName, *itt)) 
	 return 0;
    }

    data[cellName] = cellContents;
    dependencies[cellName] = temp_depends;
    return 1;
  }

  //Case of it not being an equation
  //make cell dependent on nothing
  //Set cell contents and return 1
  if(dependencies.count(cellName) != 0)
     dependencies.erase(cellName);

  data[cellName] = originalContents;  

  return 1;
}

