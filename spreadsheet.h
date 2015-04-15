

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <string>
#include <map>
#include <boost/regex.hpp>

class spreadsheet
{
 public:
  spreadsheet(std::string name); //Constructor, pass in name of spreadsheet
  std::string get_name();        //Getter for the string name
  std::string get_cell(std::string cellName);                    //Getter for contents of cell
  int set_cell(std::string cellName, std::string cellContents); //Setter for contents of cell
  std::map<std::string, std::string> get_data_map();

 private:
  std::string name; //Name of spreadsheet
  std::map<std::string, std::string> data; //String for cell names corresponding to their cell contents
};

#endif
