#ifndef PAWN_H_
#define PAWN_H_

#include <string>
#include <vector>

#include "board.h"
#include "piece.h"
#include "position.h"

class Pawn : public Piece {
 public:
  using Piece::Piece;

  std::string String() const override;

  std::vector<Position> GetMoves(const Board& board, Position from) const override;

  bool Double() const;

 private:
  bool double_;
};

#endif  // PAWN_H_