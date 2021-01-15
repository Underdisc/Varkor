include_directories("../")
add_custom_target(tests)

function(AddTest target)
  add_executable(${target})
  target_sources(${target} PRIVATE ${ARGN})
  set_property(TARGET ${target} PROPERTY RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_SOURCE_DIR}/working/test")
  add_dependencies(tests ${target})
endfunction()

AddTest(core_ComponentTable
  core/ComponentTable.cc
  ../core/ComponentTable.cc
  ../core/Types.cc
  ../debug/MemLeak.cc
  ../Error.cc)

AddTest(core_Space
  core/Space.cc
  ../core/ComponentTable.cc
  ../core/Space.cc
  ../core/Types.cc
  ../debug/MemLeak.cc
  ../Error.cc)

Addtest(ds_Map
  ds/Map.cc
  ../Error.cc)

Addtest(ds_RbTree
  ds/RbTree.cc
  ds/TestType.cc
  ../debug/MemLeak.cc
  ../Error.cc)

AddTest(ds_Vector
  ds/Vector.cc
  ds/TestType.cc
  ../debug/MemLeak.cc
  ../Error.cc)

AddTest(math_Complex
  math/Complex.cc)

AddTest(math_Matrix
  math/Matrix.cc
  ../Error.cc)

AddTest(math_Matrix4
  math/Matrix4.cc
  ../Error.cc
  ../math/Matrix4.cc
  ../math/Quaternion.cc)

AddTest(math_Quaternion
  math/Quaternion.cc
  ../Error.cc
  ../math/Quaternion.cc)

AddTest(math_Vector
  math/Vector.cc
  ../Error.cc)
