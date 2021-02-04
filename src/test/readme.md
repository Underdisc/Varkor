# Varkor Tests

This directory is for the various tests that are used while working on and testing the functionality of Varkor's code.

### Running Tests

All of the possible test targets can be found in the CMakeLists.txt file in this directory.

```
AddTest(ds_Vector
  ds/Vector.cc
  ds/TestType.cc
  ../debug/MemLeak.cc
  ../Error.cc)
```

This is an example of such a test. `ds_Vector` will be added as a build target. This target can be passed to the generator to build that test. For example, this is the command used to build the `ds_Vector` test with Ninja.

```
ninja ds_Vector
```

Similar to Varkor, exectubles are placed in a specific directory. For Varkor, this is `{repo_root}/working`. For test executables, this is `{repo_root}/working/test`. After building the `ds_Vector` test, the executable can be found here.

```
{repo_root}/working/test/ds_Vector.exe
```

### Testing Tests

Files containing the output of every test can be found in `{repo_root}/working/test`. All output files are named with the following format, `{target}_out.txt`. `ds_Vector_out.txt` is an example of such a file.

To perform a test to make sure that the output hasn't changed, build the target being tested, output the result of the test into a new file called `{target}_out_diff.txt`, and diff this file against `{target}_out.txt`. The file should be called `{target}_out_diff.txt` because these files are gitignored. Here is an example of what this would like like for the `ds_Vector` target with Ninja.

```
cd build/directory
ninja ds_Vector
cd {repo_root}/working/test
ds_Vector.exe > ds_Vector_out_diff.txt
diff ds_Vector_out.txt ds_Vector_out_diff.txt
```

### Adding Tests

Adding a test only involves creating the test driver, adding a target to the CMakeLists.txt file in this directory, and creating a test ouptut file in `{repo_root}/working/test`. For example, let's say we added a new type for math that resides in `{repo_root}/src/math/NewType.cc` that we want to test. First we create the driver file.

```
cd {repo_root}/src/test/math
touch NewType.cc
```

Now that the driver file has been created, we need to add the test as a build target to the CMakeLists.txt file in this directory.

```
AddTest(math_NewType
  math/NewType.cc
  ../math/NewType.cc)
```

You'll probably notice that there are a lot of targets that also require `../Error.cc` because they use functionality from that source file. You can add whatever source files are needed to build the target to the list of source files.

The last thing to do is actually write the test driver and create the output file. The tests are really simple. There is no special testing framework shennanigans to deal with. Take a look at some of the existing tests to see how they are structured.

Once the test is complete, create the output file for the test.

```
math_NewType.exe > math_NewType_out.txt
```

### Advice For Quick Testing

Inside of `{repo_root}/working/test` you will find `btest.bat`. This script is for quickly building and running tests because no one should have to repeat the steps above for running and testing tests. There are no shell scripts at the moment because of lack of linux support. To use the script, read the comments at the top of the file to understand its usage. You will have to set the environment variables specified in `btest.bat` in `{repo_root}/working/buildSpecs.bat`.

### Organization Details

The location of the test driver source is supposed to reflect the location of the file being tested. The name of the build target for the test also reflects this. For example, here is a target, existing test driver, and the file being tested.

```
target: math_Matrix
driver: src/test/math/Matrix.cc
source: src/math/Matrix.cc
```

This is just for organization purposes, but it has some benefits. If you know the name of the file being tested, you can quickly look in the test directory to see if a test driver exists for it. If it does exist, you already know the name of the target because you know where the test driver is.

### Use Cases For Testing

The primary purpose of this is for rapid fire testing while implementing things. By doing this, we don't need to worry about the rest of the source in Varkor and instead can focus specifically on the thing being tested. That being said, every file in Varkor doesn't necessarily have a test because some things are much harder to test than others. `Input.cc` and `Framer.cc` are examples of such things.
