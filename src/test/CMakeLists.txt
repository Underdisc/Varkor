include_directories("../")

function(AddTest target)
  add_executable(${target})
  target_sources(${target} PRIVATE ${ARGN})
endfunction()

AddTest(core_ComponentTable
  core/ComponentTable.cc
  ../core/ComponentTable.cc
  ../debug/MemLeak.cc)

AddTest(ds_Vector
  ds/Vector.cc
  ../Error.cc
  ../debug/MemLeak.cc)

AddTest(math_Complex
  math/Complex.cc)

AddTest(math_Matrix
  math/Matrix.cc
  ../Error.cc)

AddTest(math_Matrix4
  math/Matrix4.cc
  ../Error.cc
  ../math/Matrix4.cc)

AddTest(math_Quaternion
  math/Quaternion.cc
  ../Error.cc
  ../math/Quaternion.cc)

AddTest(math_Vector
  math/Vector.cc
  ../Error.cc)
