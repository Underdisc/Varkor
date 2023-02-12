#include "Result.h"

std::ostream& operator<<(std::ostream& os, const Result& rhs)
{
  if (rhs.Success()) {
    os << "Success";
  }
  else {
    os << rhs.mError;
  }
  return os;
}