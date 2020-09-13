#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_


#include <exception>

using namespace std;

class NormalZero_Exception: public std::exception
{
  
  virtual const char* what() const throw()
  {
    return "Normal is zero.\n";
  }
};

class CannotSplitNode_Exception: public std::exception
{
  
  virtual const char* what() const throw()
  {
    return "Cannot split node.\n";
  }
};


class dimension_not_handled_exception: public exception
{
  virtual const char* what() const throw()
  {
    return "Dimension Not Handled";
  }
};

class point_out_of_boundary_exception: public exception
{
  virtual const char* what() const throw()
  {
    return "Point not in Boundary";
  }
};

class no_point_in_quad_exception: public exception
{
  virtual const char* what() const throw()
  {
    return "No Point In Quad.\n";
  }
};


class ShaderException: public exception
{
  const char* error_message;

public:
  virtual const char* what() const throw()
  {
    return error_message;
  }
};



class ObjImporter_Exception: public exception
{
  
  virtual const char* what() const throw()
  {
    return "Something went wrong in ObjImporter.\n";
  }
};




#endif


/*
class ObjImporter_Exception : public std::exception {
  const char* file;
  int line;
  const char* func;
  const char* info;
  
public:
  ObjImporter_Exception(const char* msg, const char* file_, int line_, const char* func_, const char* info_ = "") : std::exception(msg),
														    file (file_),
														    line (line_),
														    func (func_),
														    info (info_)
  {
  }
  
  const char* get_file() const { return file; }
  int get_line() const { return line; }
  const char* get_func() const { return func; }
  const char* get_info() const { return info; }
  
} ObjImporter_Exception;
*/
