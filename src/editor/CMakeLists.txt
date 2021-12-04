target_sources(${varkorTarget} PRIVATE
  Camera.cc
  CoreInterface.cc
  Editor.cc
  ErrorInterface.cc
  FileInterface.cc
  FramerInterface.cc
  HookInterface.cc
  InspectorInterface.cc
  Interface.cc
  OverviewInterface.cc
  Utility.cc
  WindowInterface.cc)
add_subdirectory(hook)
