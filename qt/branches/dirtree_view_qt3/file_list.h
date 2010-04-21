#ifndef FILE_LIST_H
#define FILE_LIST_H
// this is a model in MVC design pattern

#include <vector>
#include <string>

class FileList
{
  public:
    FileList(){}
  private:
    std::vector<std::string> dirs_, files_;
    std::string cur_dir_; // fullpath
};


#endif
