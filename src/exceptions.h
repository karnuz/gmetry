#include <exception>


class dimension_not_handled_exception: public exception
{
  virtual const char* what() const throw()
  {
    return "Dimension Not Handled";
  }
} dimension_not_handled_exception;

class point_out_of_boundary_exception: public exception
{
  virtual const char* what() const throw()
  {
    return "Point not in Boundary";
  }
} point_out_of_boundary_exception;
