target_sources(${varkorTarget} PRIVATE
  AssetInterfaces.cc
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
add_subdirectory(gizmos)
add_subdirectory(hook)
