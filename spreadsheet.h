
/* 
 * Authors: Riley Anderson, Brent Bagley, Ryan Farr, Nathan Rollins
 * Last Modified: 04/24/2015
 * Version 1.0
 */

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <string>
#include <map>
#include <vector>
#include <boost/regex.hpp>
#include <stack>
#include <iostream>

/* Class: spreadsheet
 *
 * Description: Aggregate DS which stores the names and values of
 *              cells in a spreadsheet. Helper class for spreadsheet_server.
 *              Stores all dependencies to determine circular dependencies.
 *
 * Public Functions:
 *   constructor:       sets name of spreadsheet
 *   destructor:        deletes all stored data from spreadsheet
 *   get_name:          rerurns name of spreadsheet
 *   get_cell:          returns contents of specified cell
 *   set_cell:          sets contents of specified cell
 *   get_data_map:      returns the data map to store spreadsheet
 *   undo:              undoes last cell change
 *   display_contents:  display current spreadsheet -- only for testing
 *   num_cells:         returns the number of stored cells currently 
 *
 * Private Functions:
 *   has_dependency:    tells if circular dependencies exist
 */
class spreadsheet
{
  /* Class: cellChange
   *
   * Description: Aggregate to store the name and contents of a cell
   *              that has been changed. Stored on a stack for 'undo'
   *
   * Public Functions:
   *   constructor: sets the name and contents to be stored
   */
  class cellChange
  {
  public:
    cellChange(std::string name, std::string change);
    std::string cell_name;
    std::string cell_change;
  };

 public:
  spreadsheet(std::string name); //Constructor, pass in name of spreadsheet
  ~spreadsheet();
  std::string get_name();        //Getter for the string name
  std::string get_cell(std::string cellName);                    //Getter for contents of cell
  int set_cell(std::string cellName, std::string cellContents); //Setter for contents of cell
  std::map<std::string, std::string>* get_data_map();
  int undo(std::string * cell_name, std::string * cell_change);
  void display_contents(); //Note: just for testing
  int num_cells();

 private:
  int has_dependency (std::string c1, std::string c2);
  std::string name; //Name of spreadsheet
  mutable std::map<std::string, std::string>* data; //String for cell names corresponding to their cell contents
  std::map<std::string, std::vector<std::string> >* dependencies; //cell names to list of cell names that it relies on
  std::stack<cellChange>* changes;
};

#endif
