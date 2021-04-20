#ifndef BOARD_H_
#define BOARD_H_

#include <memory>
#include <vector>
#include <list>
#include <iostream>

#include "color.h"
#include "move.h"
#include "piece.h"
#include "position.h"

enum GameOutcome { kInProgress, kDraw, kCheckmate };

class Board {
 public:
  Board();
  Board(const Board& b);
  Board(std::vector<std::tuple<Position, std::unique_ptr<Piece>>>& positions, Color current_player);
  void Print(std::ostream& out = std::cout) const;
  std::vector<Move> GetMoves();
  int CountTargetedSquares(Color color);
  const Piece* GetPiece(Position position) const;
  std::list<const Piece*> GetPieces() const;
  bool IsCheck(Color color) const;
  void DoMove(const Move& move);
  void NewTurn();
  void Set(Position position, std::unique_ptr<Piece> piece);
  std::optional<Position> FindKing(Color color) const;
  std::string Hash() const;
  GameOutcome GetGameOutcome();
  Color CurrentPlayer() const;

 private:
  std::vector<Move> GetMovesInternal(Color color);
  void DoMoveInternal(const Move& move);

  std::unique_ptr<Piece> board_[8][8];
  Color current_player_;
  int turn_;
  std::vector<Move> cached_moves_;
  int cached_turn_;
};

#endif  // BOARD_H_
