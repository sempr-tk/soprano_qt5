# - Find MySQL
# Find the MySQL includes and client library
# This module defines
#  MYSQL_INCLUDE_DIR, where to find mysql.h
#  MYSQL_LIBRARIES, the libraries needed to use MySQL.
#  MYSQL_FOUND, If false, do not try to use MySQL.

if ( MYSQL_INCLUDE_DIR AND MYSQL_LIBRARIES )

  # in cache already
  message(STATUS "Found Mysql: ${MYSQL_LIBRARIES} [${MYSQL_INCLUDE_DIR}] (cached)")
  set(TSTORE_FOUND TRUE)

else ( MYSQL_INCLUDE_DIR AND MYSQL_LIBRARIES )
    find_file(MYSQL_CONFIG mysql_config)

    if (MYSQL_CONFIG)
        set(MYSQL_FOUND TRUE)

        execute_process(COMMAND ${MYSQL_CONFIG} --include OUTPUT_VARIABLE MYSQL_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND ${MYSQL_CONFIG} --libs OUTPUT_VARIABLE MYSQL_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)

	# strip the -I and -l (cmake does not like those) and put the -LXXX into CFLAGS
	STRING(REPLACE "-I" "" MYSQL_INCLUDE_DIR ${MYSQL_INCLUDE_DIR})
	STRING(REGEX MATCHALL "-L[^ ]*" MYSQL_CFLAGS ${MYSQL_LIBRARIES})
	STRING(REPLACE "-L" " -L" MYSQL_CFLAGS ${MYSQL_CFLAGS})
	STRING(REGEX MATCHALL "-l[^ ]*" MYSQL_LIBRARIES ${MYSQL_LIBRARIES})
	STRING(REPLACE "-l" " " MYSQL_LIBRARIES ${MYSQL_LIBRARIES})

        message(STATUS "MySQL found.")
        message(STATUS "MySQL include dir: ${MYSQL_INCLUDE_DIR}")
        message(STATUS "MySQL libs: ${MYSQL_LIBRARIES}")
        message(STATUS "MySQL cflags: ${MYSQL_CFLAGS}")
    else (MYSQL_CONFIG)
        set(MYSQL_FOUND FALSE)
        message(STATUS "MySQL not found.")
    endif (MYSQL_CONFIG)

endif ( MYSQL_INCLUDE_DIR AND MYSQL_LIBRARIES )