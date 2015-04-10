

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<string>
#include<map>
class spreadsheet
{
 public:
  spreadsheet(std::string name);
  std::string get_name();
  std::string get_cell(std::string cellName);
  void set_cell(std::string cellName, std::string cellContents);

 private:
  std::string name;
  std::map<std::string, std::string> data;
};

#endif
