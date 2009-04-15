project(virtuoso)

include_directories(
  ${soprano_core_SOURCE_DIR}
  ${soprano_core_SOURCE_DIR}/query
  ${soprano_core_SOURCE_DIR}/vocabulary
  ${soprano_core_SOURCE_DIR}/util
  ${soprano_core_BINARY_DIR}
  ${QT_INCLUDES}
  ${soprano_SOURCE_DIR}
  ${virtuoso_BINARY_DIR}
  ${IODBC_INCLUDE_DIR}
)

set(SRC
  virtuosomodel.cpp
  virtuosobackend.cpp
  virtuosocontroller.cpp
  virtuosotools.cpp
  virtuosoqueryresultiteratorbackend.cpp
  virtuosoconfigurator.cpp
  odbcenvironment.cpp
  odbcconnection.cpp
  odbcqueryresult.cpp
  ${soprano_core_SOURCE_DIR}/sopranodirs.cpp
)

qt4_automoc(${SRC})

add_library(soprano_virtuosobackend MODULE ${SRC})

target_link_libraries(soprano_virtuosobackend
  soprano
  ${IODBC_LIBRARIES}
)

install(TARGETS soprano_virtuosobackend
  LIBRARY DESTINATION ${LIB_DESTINATION}/soprano
  RUNTIME DESTINATION bin/soprano
  ARCHIVE DESTINATION lib/soprano
  )

configure_file(virtuosobackend.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/virtuosobackend.desktop)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/virtuosobackend.desktop
  DESTINATION share/soprano/plugins
  )