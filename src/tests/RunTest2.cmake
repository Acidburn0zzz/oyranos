SET( ENV{OY_MODULE_PATH} "${CMAKE_BINARY_DIR}/../../src/modules" )
SET( ENV{XDG_DATA_DIRS} "${CMAKE_SOURCE_DIR}/../../..:${CMAKE_SOURCE_DIR}/../../../oyranos" )
EXECUTE_PROCESS( COMMAND valgrind "${CMAKE_CURRENT_BINARY_DIR}/test2")
