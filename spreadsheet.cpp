/* 
 * Authors: Riley Anderson, Brent Bagley, Ryan Farr, Nathan Rollins
 * Last Modified: 04/24/2015
 * Version 1.0
 */

#include "spreadsheet.h"

/* Constructor
 *
 * Parameter: name of spreadsheet
 * Simply sets the name and clears all data in the map storage
 */
spreadsheet::spreadsheet(std::string name)
{
  this->name = name;
  data = new std::map<std::string, std::string>();
  data->clear();
  
  dependencies = new std::map<std::string, std::vector<std::string> >();
  dependencies->clear();

  changes = new std::stack<cellChange>();
}

/* Function: spreadsheet destructor
 * Params: none
 * Return: void
 *
 * Description: Destroys all data structures stored on heap
 */
spreadsheet::~spreadsheet()
{
  delete data;
  delete dependencies;
  delete changes;
}

/* Function: get_name
 *
 * Parameter: none
 * Returns name of the spreadsheet
 */
std::string spreadsheet::get_name()
{
  return this->name;
}

/* Function: get_data_map
 *
 * Parameter: none
 * Returns map of spreadsheet data
 */
std::map<std::string, std::string>* spreadsheet::get_data_map() 
{
  return this->data;
}

/* Function: get_cell
 *
 * Parameter: name of cell whose contents you want returned
 * Returns the cell contents if they exist. Otherwise the empty string is returend
 * Note: returns "" if contents do not exist
 */
std::string spreadsheet::get_cell(std::string cellName)
{
  if(data->count(cellName) != 0)
  {
    return (*data)[cellName];
  }
  return "";
}

/* Function: has_circular_dependency
 * Parameters: check if c1 depends on c2
 * 
 * Return: 1 if there exists a dependency, 0 otherwise
 */
int spreadsheet::has_dependency(std::string c1, std::string c2)
{
  if((*dependencies).count(c2) != 0)
  {
    for(std::vector<std::string>::iterator it = (*dependencies)[c2].begin(); it != (*dependencies)[c2].end(); it++)
    {
      if(*it == c1) { return 1; }
      if(has_dependency(c1, *it)) { return 1; } 
    }
  }

  return 0;
}

/* Function: set_cell
 *
 * parameters: the name of the cell and the contents you want associated with it
 * Returns: 0 if there was a circular dependency or 1 otherwise
 */
int spreadsheet::set_cell(std::string cellName, std::string cellContents)
{
  std::cout << "In " << name << "'s set_cell with cell: " << cellName << ", and contents: " << cellContents << std::endl;

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

    for(std::vector<std::string>::iterator itt = temp_depends.begin(); itt != temp_depends.end(); itt++)
    {
      if(*itt == cellName)
	return 0;

      if(has_dependency(cellName, *itt)) 
	 return 0;
    }
    
    cellChange c(cellName, (*data)[cellName]);
    (*data)[cellName] = cellContents;
    (*dependencies)[cellName] = temp_depends;
    changes->push(c);
    return 1;
  }

  //Case of it not being an equation
  //make cell dependent on nothing
  //Set cell contents and return 1
  if((*dependencies).count(cellName) != 0)
    (*dependencies).erase(cellName);
  
  cellChange c(cellName, (*data)[cellName]);
  changes->push(c);
  (*data)[cellName] = originalContents;

  return 1;
}

//NOTE: returns integer 1 if it worked or 0 if it didn't
//Returns the cell name and cell changes so that we know what to send back to the other clients
int spreadsheet::undo( std::string * cell_name, std::string * cell_change )
{
  if(!changes->empty())
  {
    cellChange c = changes->top();
    changes->pop();
    (*cell_name) = c.cell_name;
    (*cell_change) = c.cell_change;
    
    if(set_cell(c.cell_name, c.cell_change))
    {
      changes->pop();
      return 1;
    }
  }

  (*cell_name) = "";
  (*cell_change) = "";
  return 0;
}

/* Function: num_cells
 * Params: none
 * Return: number of stored cells
 *
 * Description: Returns number of stored cells
 */
int spreadsheet::num_cells()
{
  return (*data).size();
}

/* Function: display_contents
 * Params: none
 * Return: void
 *
 * Description: Displays all cells and cell contents of spreadsheet
 * NOTE: used only for testing
 */
void spreadsheet::display_contents()
{
  for(std::map<std::string, std::string>::const_iterator it = (*data).begin(); it != (*data).end(); it++)
  {
    std::cout << "Cell: " << it->first << " Contents: " << it->second << std::endl;
  }
}

/* Function: cellChange constructor
 * Params: cell name and contents
 * Return: void
 *
 * Description: constructor for cellChange class, stores name and contents passed in
 */
spreadsheet::cellChange::cellChange(std::string name, std::string change)
{
  this->cell_name = name;
  this->cell_change = change;
}

