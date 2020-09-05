# Varkor Tests

This directory is for various tests that are used while working on and testing the functionality of code in Varkor's source.

When building the generator for Varkor with CMake, you can add a flag for building test executables.

```
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel -Dtest=yes ../../
```

The `-Dtest=yes` flag will create a generator for building the test executable. To specify the test you want to build, you will need to add a CMakeLists.txt file to this directory and add the following to it.

```
target_sources(test PRIVATE
  ../error.cc
  <test_file_to_build>)
```

Replace `<test_file_to_build>` with the desired test file and it will be used when building. When you do build the test, the output will show up in `{repo_root}/working` as `test.exe`

When performing normal builds of Varkor, `test` will be set to `no` by default instead of `yes`, and this entire test directory will be ignored.

### Use Cases For Testing

The primary purpose of this is for rapid fire testing while implementing things. By doing this, we don't need to worry about the rest of the source in Varkor and instead can focus on one specific thing. This is by no means a fully thought out testing framework, it's just a little system to make implementing features (mostly math stuff) fast and enjoyable.
