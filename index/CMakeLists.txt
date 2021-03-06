cmake_policy(SET CMP0003 NEW)

project(sopranoindex)

include_directories(
  ${soprano_SOURCE_DIR}/soprano
  ${soprano_SOURCE_DIR}
  ${CLUCENE_INCLUDE_DIR}
  ${CLUCENE_LIBRARY_DIR}
)

set(SOPRANO_INDEX_SRCS
  cluceneindex.cpp
  clucenedocumentwrapper.cpp
  cluceneutils.cpp
  indexfiltermodel.cpp
  tstring.cpp
  indexqueryhit.cpp
  indexqueryhititeratorbackend.cpp
  queryhitwrapperresultiteratorbackend.cpp
)

configure_file(clucene-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/clucene-config.h)

# CLucene requires exception support and has no support for visibility=hidden
# so we must use the default (i.e. public) value for -fvisibility
if(NOT WIN32)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
        if(__SOPRANO_HAVE_GCC_VISIBILITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
        endif()
        if(__SOPRANO_HAVE_GCC_INLINE_VISIBILITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-visibility-inlines-hidden")
        endif()
    endif()
endif()


add_library(sopranoindex SHARED ${SOPRANO_INDEX_SRCS})

target_link_libraries(
  sopranoindex
  soprano
  ${CLUCENE_LIBRARIES}
)
if(QT5_BUILD)
  target_link_libraries(sopranoindex ${Qt5Core_LIBRARIES})
else()
  target_link_libraries(sopranoindex ${QT_QTCORE_LIBRARY})
endif()

set_target_properties(
  sopranoindex
  PROPERTIES
  VERSION 1.1.0
  SOVERSION ${SOPRANO_GENERIC_SOVERSION}
  DEFINE_SYMBOL MAKE_SOPRANO_INDEX_LIB
  INSTALL_NAME_DIR ${LIB_INSTALL_DIR}
)

install(TARGETS sopranoindex EXPORT SopranoExport ${INSTALL_TARGETS_DEFAULT_ARGS})

install(
  FILES
  cluceneindex.h
  indexfiltermodel.h
  indexqueryhit.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/soprano
  COMPONENT Devel
)
