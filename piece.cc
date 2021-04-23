#include "piece.h"

#include <string>
#include <vector>

#include "common.h"
#include "board.h"
#include "color.h"
#include "move.h"
#include "position.h"

Piece::Piece(Color color) : color_(color) {}

Color Piece::GetColor() const { return color_; }

void Piece::NewTurn() {}

void Piece::DoMove(unused Board& board, unused const Move& move) {}
