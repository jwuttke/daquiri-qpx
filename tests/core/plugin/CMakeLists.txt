set(dir ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
  ${dir}/container.cpp
  ${dir}/pattern.cpp
  ${dir}/setting_metadata.cpp
  ${dir}/setting.cpp
  ${dir}/plugin.cpp
  )

set(${this_target}_sources ${${this_target}_sources} ${SOURCES} PARENT_SCOPE)
