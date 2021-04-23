#include "rook.h"

#include <string>
#include <vector>

#include "common.h"
#include "board.h"
#include "color.h"
#include "movement.h"
#include "piece.h"
#include "position.h"

std::string Rook::String() const { return GetColor() == kWhite ? "♖" : "♜"; }

std::vector<Position> Rook::GetMoves(const Board& board, Position from) const {
  std::vector<Position> moves;
  GetRookMoves(board, from, GetColor(), moves);
  return moves;
}

void Rook::DoMove(unused Board& board, unused const Move& move) { moved_ = true; }

bool Rook::Moved() const { return moved_; }

void GetRookMoves(const Board& board, Position from, Color color,
                  std::vector<Position>& moves) {
  for (int direction_x = -1; direction_x <= 1; direction_x += 2) {
    GetLinearMoves(board, from, color, direction_x, 0, moves);
  }
  for (int direction_y = -1; direction_y <= 1; direction_y += 2) {
    GetLinearMoves(board, from, color, 0, direction_y, moves);
  }
}

int Rook::Value() const {
  return 5;
}

Piece* Rook::Clone() const {
  return new Rook(*this);
}