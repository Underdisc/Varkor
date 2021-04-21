add_custom_target(tests)

function(AddTest target)
  add_executable(${target})
  target_sources(${target} PRIVATE ${ARGN})
  target_include_directories(${target} PRIVATE "../" "../../ext/local")
  target_link_libraries(${target} ${glfw} ${OPENGL_gl_LIBRARY})
  set_property(TARGET ${target} PROPERTY RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_SOURCE_DIR}/working/test")
  add_dependencies(tests ${target})

  if("${testNames}" STREQUAL "")
    set(testNames "${target}" PARENT_SCOPE)
  else()
    set(testNames "${testNames}\n${target}" PARENT_SCOPE)
  endif()
endfunction()

Addtest(ds_Map
  ds/Map.cc
  ds/TestType.cc
  ../debug/MemLeak.cc
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

AddTest(gfx_Shader
  gfx/Shader.cc
  ../gfx/Shader.cc
  ../Error.cc
  ../Viewport.cc
  ../debug/MemLeak.cc
  ../math/Matrix4.cc
  ../../ext/local/glad/glad.c)

AddTest(math_Complex
  math/Complex.cc)

AddTest(math_Geometry
  math/Geometry.cc
  ../Error.cc
  ../math/Geometry.cc)

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

AddTest(world_Space
  world/Space.cc
  world/Print.cc
  ../world/Space.cc
  ../world/Table.cc
  ../world/Types.cc
  ../debug/MemLeak.cc
  ../Error.cc)

AddTest(world_Table
  world/Table.cc
  world/Print.cc
  ../world/Space.cc
  ../world/Table.cc
  ../world/Types.cc
  ../debug/MemLeak.cc
  ../Error.cc)

set(testNames "${testNames}" PARENT_SCOPE)
