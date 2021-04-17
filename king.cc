#include "king.h"

#include <string>
#include <vector>

#include "board.h"
#include "move.h"
#include "movement.h"
#include "piece.h"
#include "position.h"
#include "rook.h"

namespace {

void GetRegularMoves(const Board& board, Position from, Color color,
                     std::vector<Position>& moves) {
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      if (x == 0 && y == 0) {
        continue;
      }
      GetMove(board, from, color, x, y, moves);
    }
  }
}

// TODO: Check if the king is in check in any position in the way.
void GetCastlingMoves(const Board& board, Position from, Color color,
                      std::vector<Position>& moves) {
  for (int direction = -1; direction <= 1; direction += 2) {
    for (int size = 1; true; ++size) {
      int x = direction * size;
      auto position = from.Move(x, 0);
      if (!position.has_value()) {
        break;
      }
      auto piece = board.GetPiece(*position);
      if (piece == nullptr) {
        continue;
      }
      auto rook = dynamic_cast<const Rook*>(piece);
      if (rook == nullptr || rook->Moved()) {
        break;
      }
      moves.push_back(*from.Move(direction * 2, 0));
      break;
    }
  }
}

}  // namespace

std::string King::String() const { return GetColor() == kWhite ? "♔" : "♚"; }

std::vector<Position> King::GetMoves(const Board& board, Position from) const {
  std::vector<Position> moves;
  GetRegularMoves(board, from, GetColor(), moves);
  if (!Moved()) {
    GetCastlingMoves(board, from, GetColor(), moves);
  }
  return moves;
}

void King::DoMove(Board& board, const Move& move) {
  moved_ = true;
  int y = move.From().Y();
  int diff = move.To().X() - move.From().X();
  if (diff == 2) {
    board.DoMove({{7, y}, {5, y}, std::nullopt});
  } else if (diff == -2) {
    board.DoMove({{0, y}, {3, y}, std::nullopt});
  }
}

bool King::Moved() const { return moved_; }

int King::Value() const {
  return 1;  // this is all pieces plus 1
}

Piece* King::Clone() const {
  return new King(*this);
}