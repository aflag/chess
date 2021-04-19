cmake_minimum_required(VERSION 3.10)

project(chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(SOURCES
  bishop.cc
  board.cc
  color.cc
  engine.cc
  king.cc
  knight.cc
  move.cc
  movement.cc
  pawn.cc
  piece.cc
  position.cc
  queen.cc
  rook.cc
)

add_executable(chess ${SOURCES} chess.cc)

enable_testing()
add_executable(engine_test ${SOURCES} engine_test.cc)
add_test(board_test board_test)

add_executable(board_test ${SOURCES} board_test.cc)
add_test(engine_test engine_test)