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

# Create all of the targets for different Varkor projects.
function(AddProject targetName directory)
  project(${targetName})
  add_executable(${targetName})
  add_subdirectory(${directory}/src ${targetName}Obj)
  target_compile_features(${targetName} PRIVATE cxx_std_20)
  target_include_directories(${targetName} PUBLIC ${directory}/src)
  target_compile_definitions(${targetName}
    PUBLIC PROJECT_DIRECTORY="${directory}")
  target_link_libraries(${targetName} varkor)
  add_externals(${targetName})
endfunction()

include("../../projects.cmake" OPTIONAL)