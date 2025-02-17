#ifndef test_ds_Test_h
#define test_ds_Test_h

#include "test/Test.h"
#include "test/ds/TestType.h"

#define RunDsTest(function) \
  TestType::ResetCounts();  \
  RunTest(function);

#endif
