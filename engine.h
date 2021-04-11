#ifndef ENGINE_H_
#define ENGINE_H_
#include <vector>

#include "board.h"
#include "color.h"
#include "cache.h"

enum GameOutcome { kInProgress, kDraw, kCheckmate };

GameOutcome GetGameOutcome(bool has_valid_moves, bool isCheck);

std::vector<Move> ComputeUtility(
  Board& board,
  Color mycolor,
  int depth,
  double (*utility)(Board& board, GameOutcome outcome, Color attackingcolor),
  Cache& cache
);

double MaterialisticUtility(Board& board, GameOutcome outcome, Color attackingcolor);
double SmartUtility(Board& board, GameOutcome outcome, Color attackingcolor);

class ColorfulCompare {
 public:
  ColorfulCompare(Color color);
  bool operator()(const Move& a, const Move& b);

 private:
  int multiplier_;
};

#endif