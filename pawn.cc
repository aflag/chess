#include "pawn.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "bishop.h"
#include "board.h"
#include "color.h"
#include "knight.h"
#include "move.h"
#include "piece.h"
#include "position.h"
#include "queen.h"
#include "rook.h"

namespace {

int Forward(Color color) { return color == kWhite ? 1 : -1; }

void GetRegularMoves(const Board& board, Position from, Color color,
                     std::vector<Position>& moves) {
  auto next = from.Move(0, Forward(color));
  if (!next.has_value()) {
    std::cerr << "Pawn in the last row";
    exit(1);
  }
  if (board.GetPiece(*next)) {
    return;
  }
  moves.push_back(*next);
  if ((color == kWhite && from.Y() != 1) || (color == kBlack && from.Y() != 6)) {
    return;
  }
  auto two = next->Move(0, Forward(color));
  if (!two) {
    return;
  }
  if (board.GetPiece(*two)) {
    return;
  }
  moves.push_back(*two);
}

void GetCaptureMoves(const Board& board, Position from, Color color,
                     std::vector<Position>& moves) {
  for (int side = -1; side <= 1; side += 2) {
    auto to = from.Move(side, Forward(color));
    if (!to.has_value()) {
      continue;
    }
    auto toPiece = board.GetPiece(*to);
    if (toPiece == nullptr || toPiece->GetColor() == color) {
      continue;
    }
    moves.push_back(*to);
  }
}

// void GetEnPassantMoves(const Board& board, Position from, Color color,
//                        std::vector<Position>& moves) {
//   for (int side = -1; side <= 1; side += 2) {
//     auto onSide = from.Move(side, 0);
//     if (!onSide) {
//       continue;
//     }
//     auto piece = board.GetPiece(*onSide);
//     if (!piece || piece->GetColor() == color) {
//       continue;
//     }
//     auto pawn = dynamic_cast<const Pawn*>(piece);
//     if (!pawn || !pawn->Double()) {
//       continue;
//     }
//     auto to = from.Move(side, Forward(color));
//     if (!to) {
//       std::cerr << "Pawn in first row";
//       exit(1);
//     }
//     moves.push_back(*to);
//   }
// }

}  // namespace

std::string Pawn::String() const { return GetColor() == kWhite ? "♙" : "♟"; }

std::vector<Position> Pawn::GetMoves(const Board& board, Position from) const {
  std::vector<Position> moves;
  GetRegularMoves(board, from, GetColor(), moves);
  GetCaptureMoves(board, from, GetColor(), moves);
  // GetEnPassantMoves(board, from, GetColor(), moves);
  return moves;
}

bool Pawn::Double() const { return double_; }

void Pawn::NewTurn() {
  double_ = false;
}

void Pawn::DoMove(Board& board, const Move& move) {
  Position to = move.To();
  int to_y = to.Y();
  if (abs(to_y - move.From().Y()) == 2) {
    double_ = true;
    return;
  }
  if (to_y == 7 || to_y == 0) {
    Promotion promote_to = move.PromoteTo().value_or(kQueen);
    switch (promote_to) {
      case kBishop:
        board.Set(to, std::make_unique<Bishop>(GetColor()));
        break;
      case kKnight:
        board.Set(to, std::make_unique<Knight>(GetColor()));
        break;
      case kQueen:
        board.Set(to, std::make_unique<Queen>(GetColor()));
        break;
      case kRook:
        board.Set(to, std::make_unique<Rook>(GetColor()));
        break;
      default:
        board.Set(to, std::make_unique<Queen>(GetColor()));
        break;
    }
  }
}

int Pawn::Value() const {
  return 1;
}

Piece* Pawn::Clone() const {
  return new Pawn(*this);
}