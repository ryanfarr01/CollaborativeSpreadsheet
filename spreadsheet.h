

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <string>
#include <map>
#include <vector>
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
  int has_dependency (std::string c1, std::string c2);
  std::string name; //Name of spreadsheet
  mutable std::map<std::string, std::string> data; //String for cell names corresponding to their cell contents
  std::map<std::string, std::vector<std::string> > dependencies; //cell names to list of cell names that it relies on
};

#endif
