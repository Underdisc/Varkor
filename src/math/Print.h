#include <cmath>
#include <iomanip>
#include <ostream>
#include <sstream>

#include "math/Constants.h"
#include "math/Matrix.h"
#include "math/Quaternion.h"

template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, const Math::Matrix<T, N>& matrix) {
  // We find the longest number in each column so that all columns are aligned
  // when printed to console. Do note that this does not work if any formatting
  // specifiers are applied to os.
  int longestWidths[N];
  for (int c = 0; c < N; ++c) {
    longestWidths[c] = 1;
    for (int r = 0; r < N; ++r) {
      std::stringstream ss;
      ss << matrix[r][c];
      int width = (int)ss.str().size();
      if (width > longestWidths[c]) {
        longestWidths[c] = width;
      }
    }
  }

  for (int r = 0; r < N; ++r) {
    if (r == 0 || r == N - 1) {
      os << "+ ";
    }
    else {
      os << "| ";
    }
    for (int c = 0; c < N; ++c) {
      os << std::setw(longestWidths[c]) << std::right << matrix[r][c];
      if (c < N - 1) {
        os << ", ";
      }
    }
    if (r == 0 || r == N - 1) {
      os << " +\n";
    }
    else {
      os << " |\n";
    }
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
  bool firstValue = true;
  auto printValue = [&os, &firstValue](float value, const char* basis) {
    if (std::fabs(value) < Math::nEpsilon) {
      return;
    }
    if (firstValue) {
      os << value << basis;
      firstValue = false;
      return;
    }
    if (value < 0.0f) {
      os << " - ";
      value *= -1.0f;
    }
    else {
      os << " + ";
    }
    os << value << basis;
  };
  printValue(q.mA, "");
  printValue(q.mB, "i");
  printValue(q.mC, "j");
  printValue(q.mD, "k");
  return os;
}
