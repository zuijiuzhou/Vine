#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "viGe" for configuration "Debug"
set_property(TARGET viGe APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(viGe PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/viGe.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/viGe.dll"
  )

list(APPEND _cmake_import_check_targets viGe )
list(APPEND _cmake_import_check_files_for_viGe "${_IMPORT_PREFIX}/lib/viGe.lib" "${_IMPORT_PREFIX}/bin/viGe.dll" )

# Import target "viCore" for configuration "Debug"
set_property(TARGET viCore APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(viCore PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/viCore.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/viCore.dll"
  )

list(APPEND _cmake_import_check_targets viCore )
list(APPEND _cmake_import_check_files_for_viCore "${_IMPORT_PREFIX}/lib/viCore.lib" "${_IMPORT_PREFIX}/bin/viCore.dll" )

# Import target "viDi" for configuration "Debug"
set_property(TARGET viDi APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(viDi PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/viDi.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/viDi.dll"
  )

list(APPEND _cmake_import_check_targets viDi )
list(APPEND _cmake_import_check_files_for_viDi "${_IMPORT_PREFIX}/lib/viDi.lib" "${_IMPORT_PREFIX}/bin/viDi.dll" )

# Import target "viSystem" for configuration "Debug"
set_property(TARGET viSystem APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(viSystem PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/viSystem.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/viSystem.dll"
  )

list(APPEND _cmake_import_check_targets viSystem )
list(APPEND _cmake_import_check_files_for_viSystem "${_IMPORT_PREFIX}/lib/viSystem.lib" "${_IMPORT_PREFIX}/bin/viSystem.dll" )

# Import target "viGraphics" for configuration "Debug"
set_property(TARGET viGraphics APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(viGraphics PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/viGraphics.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/viGraphics.dll"
  )

list(APPEND _cmake_import_check_targets viGraphics )
list(APPEND _cmake_import_check_files_for_viGraphics "${_IMPORT_PREFIX}/lib/viGraphics.lib" "${_IMPORT_PREFIX}/bin/viGraphics.dll" )

# Import target "viAppfw" for configuration "Debug"
set_property(TARGET viAppfw APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(viAppfw PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/viAppfw.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "Qt6::Core"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/viAppfw.dll"
  )

list(APPEND _cmake_import_check_targets viAppfw )
list(APPEND _cmake_import_check_files_for_viAppfw "${_IMPORT_PREFIX}/lib/viAppfw.lib" "${_IMPORT_PREFIX}/bin/viAppfw.dll" )

# Import target "viAppfwGui" for configuration "Debug"
set_property(TARGET viAppfwGui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(viAppfwGui PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/viAppfwGui.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "Qt6::Core;Qt6::Gui;Qt6::Widgets"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/viAppfwGui.dll"
  )

list(APPEND _cmake_import_check_targets viAppfwGui )
list(APPEND _cmake_import_check_files_for_viAppfwGui "${_IMPORT_PREFIX}/lib/viAppfwGui.lib" "${_IMPORT_PREFIX}/bin/viAppfwGui.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
