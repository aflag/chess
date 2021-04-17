cmake_minimum_required(VERSION 3.10)

project(chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(chess
  bishop.cc
  board.cc
  chess.cc
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

enable_testing()
add_executable(
  tests

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

  engine_test.cc
)
add_test(tests tests)