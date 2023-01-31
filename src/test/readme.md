# Varkor Tests
This directory is for the various tests that are used while working on and testing the functionality of Varkor's code.

### Testing Tests
Files containing the output of every test can be found in `working/test`. All output files are named with the following format, `{target}/out.txt`. `ds_Vector/out.txt` is an example of such a file. Inside of `script` you will find `btest.js`. This script is for quickly building and running tests. Read the script for details about its usage.

### Adding Tests
Adding a test involves creating the test driver, adding a target to the CMakeLists.txt file in this directory, and creating a test ouptut file in `working/test/{target}`. For example, let's say we added a new type for math that resides in `src/math/NewType.cc` that we want to test. First we create the driver file.
```
cd src/test/math
touch NewType.cc
```
Now that the driver file has been created, we need to add the test as a target to the CMakeLists.txt file in this directory.
```
AddTest(math_NewType math/NewType.cc)
```
The last thing to do is actually write the test driver and create the output file. The tests are really simple. There is no special testing framework shennanigans to deal with. Take a look at some of the existing tests to see how they are structured. Once the test is complete, create the output file for the test.
```
node btest <compiler> <configuration> math_NewType o
```
You'll notice that some of the test targets require files other than the main test driver. `AddTest(ds_Vector ds/Vector.cc ds/TestType.cc)` is an example of this. This is because these targets require that these files are also built in order to create the driver. These extra files can be added any time it is necessary to do so. The other files in Varkor's `src/` directory that are needed for the test target do not need to be accounted for because every test will link against the Varkor library.

### Organization Details
The name of the test target, location of the test driver, and the location of the test output is supposed to reflect the location of the source file being tested. For example, here is a target, driver, output, and the file being tested.
```
target: math_Matrix
driver: src/test/math/Matrix.cc
output: working/test/math_Matrix/out.txt
source: src/math/Matrix.cc
```
This is for organization, but it has other benefits. If you know the name of the file being tested, you can quickly look in the test directory to see if a test driver exists for it. If it does exist, you already know the name of the target because you know where the test driver is.

### Use Cases For Testing
The primary purpose of this is for rapid fire testing during implementation. By doing this, we don't need to worry about the rest of the source in Varkor and instead can focus specifically on the thing being tested. That being said, every file in Varkor doesn't necessarily have a test because some things are much harder to test than others. `Input.cc` and `Framer.cc` are examples of such things.
