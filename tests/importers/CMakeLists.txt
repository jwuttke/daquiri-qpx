set(dir ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
  ${dir}/ImporterAVA.cpp
  ${dir}/ImporterCHN.cpp
  ${dir}/ImporterCNF.cpp
  ${dir}/ImporterMCA.cpp
  ${dir}/ImporterN42.cpp
  ${dir}/ImporterSPC.cpp
  ${dir}/ImporterTKA.cpp
  ${dir}/string_to_chans.cpp
  )

set(${this_target}_sources ${${this_target}_sources} ${SOURCES} PARENT_SCOPE)
