# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/Horizon_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Horizon_autogen.dir/ParseCache.txt"
  "Horizon_autogen"
  )
endif()
