project(vtkSlicer${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${OpenIGTLinkIO_INCLUDE_DIRS}
  ${OpenIGTLinkIF_ModuleLogic_INCLUDE_DIRS}
  ${OpenIGTLinkIF_ModuleMRML_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  vtkSlicerOpenIGTLinkCommand.cxx
  vtkSlicerOpenIGTLinkCommand.h
  vtkSlicerOpenIGTLinkRemoteLogic.cxx
  vtkSlicerOpenIGTLinkRemoteLogic.h
  )

set(${KIT}_TARGET_LIBRARIES
  ${ITK_LIBRARIES}
  vtkSlicer${MODULE_NAME}ModuleMRML
  ${OpenIGTLinkIO_LIBRARIES}
  vtkSlicerOpenIGTLinkIFModuleLogic
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  ${DISABLE_WRAP_PYTHON}
  )
