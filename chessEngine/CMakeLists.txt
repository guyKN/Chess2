cmake_minimum_required(VERSION 3.16)
project(Chess)

set(CMAKE_CXX_STANDARD 17)

option(WASM "Compile to WASM, rather than the command line and UCI" OFF)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    message(STATUS "GNU compiler! ")
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "release mode")
    add_compile_definitions(FOR_RELEASE=1)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "-O3")
        set(CMAKE_CXX_FLAGS "-march=native")
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "debug mode.")
    add_compile_definitions(FOR_RELEASE=0)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "-g")
    endif()
else()
    message( FATAL_ERROR "Must be debug or release." )
endif()

if(WASM)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../firebase/public/js/webAssembly")
    add_compile_definitions(TO_WASM)
    include_directories(include)
    file(GLOB SOURCES src/*.cpp)
    add_executable(chessWasm ${SOURCES})

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
                _WASM_doMoveSequence,                                                                    \
                _WASM_runTest                                                                            \
                ]'")
else(WASM)
    message(STATUS "inside of else")
    if(CMAKE_COMPILER_IS_GNUCC)
        add_compile_options(-m64)
    endif()
    add_compile_definitions(FOR_UCI)
    include_directories(include)
    file(GLOB SOURCES src/*.cpp)
    add_executable(chessUci ${SOURCES})
endif(WASM)

if(WASM)
endif(WASM)
