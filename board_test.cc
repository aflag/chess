#define BOOST_TEST_MODULE board tests
#include <boost/test/included/unit_test.hpp>

#include "king.h"
#include "rook.h"
#include "board.h"
#include "move.h"

BOOST_AUTO_TEST_CASE(TestCastleKeepsTurn) {
  std::vector<std::tuple<Position, std::unique_ptr<Piece>>> positions;
  positions.emplace_back(Position(4, 0), std::make_unique<King>(kWhite));
  positions.emplace_back(Position(7, 0), std::make_unique<Rook>(kWhite));
  positions.emplace_back(Position(7, 7), std::make_unique<King>(kBlack));
  Board b(positions, kWhite);

  auto move = Move::FromXboardString("e1g1");
  b.DoMove(move.value());
  b.NewTurn();

  BOOST_CHECK_EQUAL(b.CurrentPlayer(), kBlack);
}