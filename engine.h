#ifndef ENGINE_H_
#define ENGINE_H_
#include <vector>

#include "board.h"
#include "color.h"
#include "cache.h"

std::vector<Move> ComputeUtility(
  Board board,
  Color mycolor,
  int depth,
  double (*utility)(Board& board, Color attackingcolor),
  Cache& cache
);

double MaterialisticUtility(Board& board, Color attackingcolor);
double SmartUtility(Board& board, Color attackingcolor);

class ColorfulCompare {
 public:
  ColorfulCompare(Color color);
  bool operator()(const Move& a, const Move& b);

 private:
  int multiplier_;
};

#endif