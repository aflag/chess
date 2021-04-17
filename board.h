#ifndef BOARD_H_
#define BOARD_H_

#include <memory>
#include <vector>
#include <list>

#include "color.h"
#include "move.h"
#include "piece.h"
#include "position.h"

class Board {
 public:
  class Snapshot {
    Snapshot(Piece* board[8][8]);
    std::unique_ptr<Piece> board_[8][8];
    friend Board;
  };
  Board();
  void Print() const;
  std::vector<Move> GetMoves(Color color);
  const Piece* GetPiece(Position position) const;
  std::list<const Piece*> GetPieces() const;
  bool IsCheck(Color color) const;
  void DoMove(const Move& move);
  std::unique_ptr<Snapshot> TakeSnapshot() const;
  void RecoverSnapshot(const Snapshot& snapshot);
  void NewTurn(Color color);
  void Set(Position position, std::unique_ptr<Piece> piece);
  std::optional<Position> FindKing(Color color) const;
  std::string Hash() const;
  void Clear();
  void Setup(std::vector<std::tuple<Position, std::unique_ptr<Piece>>>& positions);

 private:
  std::unique_ptr<Piece> board_[8][8];
};

#endif  // BOARD_H_
