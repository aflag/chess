#define BOOST_TEST_MODULE engine tests
#include <boost/test/included/unit_test.hpp>
#include "engine.h"
#include "king.h"
#include "pawn.h"
#include "rook.h"

BOOST_AUTO_TEST_CASE(TestCaptureFreePawn) {
  Cache cache;
  std::vector<std::tuple<Position, std::unique_ptr<Piece>>> positions;
  positions.emplace_back(Position(5, 0), std::make_unique<King>(kWhite));
  positions.emplace_back(Position(2, 4), std::make_unique<Rook>(kWhite));
  positions.emplace_back(Position(2, 1), std::make_unique<Pawn>(kBlack));
  positions.emplace_back(Position(7, 7), std::make_unique<King>(kBlack));
  Board b(positions, kWhite);

  auto moves = ComputeUtility(b, kWhite, 2, SmartUtility, cache);
  auto best = std::max_element(moves.begin(), moves.end(), ColorfulCompare(kWhite));

  BOOST_REQUIRE_EQUAL(moves.size(), 18);

  // rook
  BOOST_CHECK_EQUAL(best->From().X(), 2);
  BOOST_CHECK_EQUAL(best->From().Y(), 4);
  // captures pawn
  BOOST_CHECK_EQUAL(best->To().X(), 2);
  BOOST_CHECK_EQUAL(best->To().Y(), 1);
}

BOOST_AUTO_TEST_CASE(TestFindsMate) {
  std::vector<std::tuple<Position, std::unique_ptr<Piece>>> positions;
  positions.emplace_back(Position(5, 0), std::make_unique<King>(kWhite));
  positions.emplace_back(Position(0, 2), std::make_unique<Rook>(kWhite));
  positions.emplace_back(Position(1, 1), std::make_unique<Rook>(kWhite));
  positions.emplace_back(Position(7, 7), std::make_unique<King>(kBlack));
  Cache cache;
  Board b(positions, kWhite);

  auto moves = ComputeUtility(b, kWhite, 2, SmartUtility, cache);
  auto best = std::max_element(moves.begin(), moves.end(), ColorfulCompare(kWhite));

  BOOST_REQUIRE_EQUAL(moves.size(), 33);

  // finds mate
  BOOST_CHECK(std::isinf(best->Utility()));
}

void PlayAGame(Board& board) {
  int depth = 2;
  Cache cache;
  while (true) {
    auto moves = ComputeUtility(board, board.CurrentPlayer(), depth, MaterialisticUtility, cache);
    auto best = std::max_element(moves.begin(), moves.end(), ColorfulCompare(board.CurrentPlayer()));
    board.DoMove(*best);
    board.NewTurn();
    if (board.GetGameOutcome() != kInProgress) {
      break;
    }
  }
}

BOOST_AUTO_TEST_CASE(TestRegressionTest) {
  Board board;
  PlayAGame(board);
  BOOST_CHECK_EQUAL(board.Hash(), "R'P..r.pr'NP....pnBP....pbQP....pqK..P..pkBP....pbN.P...pnR.P.p..._black");
  BOOST_CHECK_EQUAL(board.GetGameOutcome(), kDraw);
}