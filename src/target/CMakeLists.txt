# Create the target for the standalone version of Varkor.
add_executable(varkorStandalone)
target_sources(varkorStandalone PRIVATE VarkorStandalone.cc)
target_link_libraries(varkorStandalone varkor)

# Create the test viewer target.
add_executable(testViewer)
target_sources(testViewer PRIVATE
  TestViewer.cc
  ../test/math/Hull.cc
  ../test/math/Intersection.cc
  ../test/math/Triangle.cc)
target_link_libraries(testViewer varkor)
target_compile_definitions(testViewer PUBLIC RemoveTestEntryPoint)