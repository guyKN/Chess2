cmake_minimum_required(VERSION 3.16)
project(Chess)

set(CMAKE_CXX_STANDARD 17)

option(WASM "Compile to WASM, rather than the command line" OFF)
option(GENERATE_SEEDS "generate seeds for bishop rook and queen movement, rather than using the pre-existing ones" ON)
option(FOR_RELEASE "optimize code for release" OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
if(FOR_RELEASE)
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS "-O3")
else()
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "-g")
endif()
    if(GENERATE_SEEDS)
    add_compile_definitions(GENERATE_SEEDS)
else(GENERATE_SEEDS)
    add_compile_definitions(USE_SEEDS)
endif(GENERATE_SEEDS)

if(WASM)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/firebase/public/js/webAssembly")
    add_compile_definitions(TO_WASM)

    include_directories(include)
    file(GLOB SOURCES src/*.cpp)
    add_executable(chessWasm ${SOURCES} src/jsInterface.cpp include/jsInterface.h)

    set_target_properties(chessWasm PROPERTIES LINK_FLAGS "-s WASM=1                                     \
                -s EXPORTED_FUNCTIONS='[                                                                 \
                _main,                                                                                   \
                _WASM_pieceOn,                                                                           \
                _WASM_calculateMoves,                                                                    \
                _WASM_isLegalMoveStart,                                                                  \
                _WASM_calculateMovesFrom,                                                                \
                _WASM_isLegalMoveTo,                                                                     \
                _WASM_doMoveIfLegal,                                                                     \
                _WASM_checkWinner,                                                                       \
                _WASM_doAiMove,                                                                          \
                _WASM_currentPlayer,                                                                     \
                _WASM_printMoves,                                                                        \
                _WASM_isThreatTo,                                                                        \
                _WASM_initData,                                                                          \
                _WASM_resetBoard,                                                                        \
                _WASM_printBitboards,                                                                    \
                _WASM_undoMove,                                                                          \
                _WASM_gotoPos,                                                                           \
                _WASM_runTest                                                                            \
                ]'")
else(WASM)
    add_compile_options(-m64)
    add_compile_definitions(COMMAND_LINE)
    include_directories(include)
    file(GLOB SOURCES src/*.cpp)
    add_compile_definitions(TO_COMMAND_LINE)
    add_executable(chessAscii ${SOURCES})
endif(WASM)

#if(WASM)
#endif(WASM)
