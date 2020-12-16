cmake_minimum_required(VERSION 3.16)
project(Chess)

set(CMAKE_CXX_STANDARD 17)

option(WASM "Compile to WASM, rather than the command line" OFF)
if(WASM)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/firebase/public/js/webAssembly")

    add_compile_definitions(TO_WASM)

    include_directories(include)
    file(GLOB SOURCES src/*.cpp)
    add_executable(chessWasm ${SOURCES} src/jsInterface.cpp include/jsInterface.h)

    set_target_properties(chessWasm PROPERTIES LINK_FLAGS "-s WASM=1 -s EXPORTED_FUNCTIONS='[       \
                _main,                                                                              \
                _pieceOn,                                                                           \
                _calculateMoves,                                                                    \
                _isLegalMoveStart,                                                                  \
                _calculateMovesFrom,                                                                \
                _isLegalMoveTo,                                                                     \
                _doMoveIfLegal,                                                                     \
                _currentPlayer,                                                                     \
                _printMoves,                                                                        \
                _initData,                                                                          \
                _resetBoard                                                                         \
                ]'")
else(WASM)
    add_compile_definitions(COMMAND_LINE)
    include_directories(include)
    file(GLOB SOURCES src/*.cpp)
    add_compile_definitions(TO_COMMAND_LINE)
    add_executable(chessAscii ${SOURCES})
endif(WASM)