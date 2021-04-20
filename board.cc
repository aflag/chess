#include "board.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <list>

#include "bishop.h"
#include "color.h"
#include "king.h"
#include "knight.h"
#include "move.h"
#include "pawn.h"
#include "piece.h"
#include "position.h"
#include "queen.h"
#include "rook.h"

namespace {

void SetUpRookKnightBishop(Color color, int others, int rook, int knight,
                           int bishop, std::unique_ptr<Piece> (&board)[8][8]) {
  board[rook][others] = std::make_unique<Rook>(color);
  board[knight][others] = std::make_unique<Knight>(color);
  board[bishop][others] = std::make_unique<Bishop>(color);
}

void SetUpColor(Color color, int others, int pawns,
                std::unique_ptr<Piece> (&board)[8][8]) {
  SetUpRookKnightBishop(color, others, 0, 1, 2, board);
  SetUpRookKnightBishop(color, others, 7, 6, 5, board);
  board[3][others] = std::make_unique<Queen>(color);
  board[4][others] = std::make_unique<King>(color);
  for (int x = 0; x <= 7; ++x) {
    board[x][pawns] = std::make_unique<Pawn>(color);
  }
}

std::vector<Move> GetMovesWithPossibleCheck(const Board& board, Color color) {
  std::vector<Move> moves;
  for (int x = 0; x <= 7; ++x) {
    for (int y = 0; y <= 7; ++y) {
      Position from(x, y);
      auto piece = board.GetPiece(from);
      if (!piece || piece->GetColor() != color) {
        continue;
      }
      std::vector<Position> tos = piece->GetMoves(board, from);
      bool is_pawn = dynamic_cast<const Pawn*>(piece) != nullptr;
      for (const auto to : tos) {
        if (is_pawn && (to.Y() == 0 || to.Y() == 7)) {
          moves.emplace_back(from, to, kBishop);
          moves.emplace_back(from, to, kKnight);
          moves.emplace_back(from, to, kQueen);
          moves.emplace_back(from, to, kRook);
        } else {
          moves.emplace_back(from, to, std::nullopt);
        }
      }
    }
  }
  return moves;
}

std::unique_ptr<Piece>& GetMutablePiece(std::unique_ptr<Piece> (&board)[8][8],
                                        Position position) {
  return board[position.X()][position.Y()];
}

}  // namespace

Board::Board() : current_player_(kWhite), turn_(0), cached_turn_(-1) {
  SetUpColor(kWhite, 0, 1, board_);
  SetUpColor(kBlack, 7, 6, board_);
}

// The cache is not copied because it's probably irrelevant anyway
Board::Board(const Board& b) : current_player_(b.current_player_), turn_(b.turn_), cached_turn_(-1) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (b.board_[i][j] != nullptr) {
        board_[i][j] = std::unique_ptr<Piece>(b.board_[i][j]->Clone());
      } else {
        board_[i][j] = nullptr;
      }
    }
  }
}

Board::Board(std::vector<std::tuple<Position, std::unique_ptr<Piece>>>& positions, Color current_player) : current_player_(current_player), turn_(0), cached_turn_(-1) {
  for (auto item = positions.begin(); item != positions.end(); ++item) {
    Position& pos = std::get<0>(*item);
    board_[pos.X()][pos.Y()] = std::move(std::get<1>(*item));
  }
}

void Board::Print(std::ostream& out) const {
  for (int y = 7; y >= 0; --y) {
    out << y + 1 << " ";
    for (int x = 0; x <= 7; ++x) {
      const Piece* piece = GetPiece({x, y});
      out << "\e[48;5;" << ((x + y) % 2 == 0 ? "94" : "208") << "m"
                << (piece == nullptr ? " " : piece->String());
    }
    out << "\e[0m";
    out << std::endl;
  }
  out << "  abcdefgh" << std::endl;
}

int Board::CountTargetedSquares(Color color) {
  return (int) GetMovesInternal(color).size();
}

std::vector<Move> Board::GetMoves() {
  if (cached_turn_ != turn_) {
    cached_turn_ = turn_;
    cached_moves_ = GetMovesInternal(current_player_);
  }
  return cached_moves_;
}

std::vector<Move> Board::GetMovesInternal(Color color) {
  std::vector<Move> moves = GetMovesWithPossibleCheck(*this, color);
  for (auto i = moves.begin(); i != moves.end();) {
    std::unique_ptr<Piece>& from = GetMutablePiece(board_, i->From());
    std::unique_ptr<Piece>& to = GetMutablePiece(board_, i->To());
    std::unique_ptr<Piece> to_copy = std::move(to);
    to = std::move(from);
    bool is_check = IsCheck(color);
    from = std::move(to);
    to = std::move(to_copy);
    if (is_check) {
      i = moves.erase(i);
      continue;
    }
    ++i;
  }
  return moves;
}

GameOutcome Board::GetGameOutcome() {
  std::vector<Move> moves = GetMoves();
  if (!moves.empty()) {
    return kInProgress;
  } else if (IsCheck(current_player_)) {
    return kCheckmate;
  } else {
    return kDraw;
  }
}

const Piece* Board::GetPiece(Position position) const {
  return board_[position.X()][position.Y()].get();
}

std::optional<Position> Board::FindKing(Color color) const {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j <  8; ++j) {
      const King* king = dynamic_cast<const King*>(board_[i][j].get());
      if (king && king->GetColor() == color) {
        return Position(i, j);
      }
    }
  }
  return {};
}

bool Board::IsCheck(Color color) const {
  auto kingpos = FindKing(color);
  if (!kingpos.has_value()) {
    return false;
  }
  Bishop bishop(color);
  for (auto pos : bishop.GetMoves(*this, kingpos.value())) {
    const Piece *p = this->GetPiece(pos);
    if (dynamic_cast<const Bishop*>(p) != nullptr) {
      return true;
    } else if (dynamic_cast<const Queen*>(p) != nullptr) {
      return true;
    }
  }
  Rook rook(color);
  for (auto pos : rook.GetMoves(*this, kingpos.value())) {
    const Piece *p = this->GetPiece(pos);
    if (dynamic_cast<const Rook*>(p) != nullptr) {
      return true;
    } else if (dynamic_cast<const Queen*>(p) != nullptr) {
      return true;
    }
  }
  Knight knight(color);
  for (auto pos : knight.GetMoves(*this, kingpos.value())) {
    const Piece *p = this->GetPiece(pos);
    if (dynamic_cast<const Knight*>(p) != nullptr) {
      return true;
    }
  }
  std::vector<Position> pawns;
  if (color == kWhite) {
    int x, y;
    x = kingpos.value().X() + 1;
    y = kingpos.value().Y() + 1;
    if (x < 8 && y < 8) {
      pawns.emplace_back(x, y);
    }
    x = kingpos.value().X() - 1;
    y = kingpos.value().Y() + 1;
    if (x >= 0 && y < 8) {
      pawns.emplace_back(x, y);
    }
  } else {
    int x, y;
    x = kingpos.value().X() + 1;
    y = kingpos.value().Y() - 1;
    if (x < 8 && y >= 0) {
      pawns.emplace_back(x, y);
    }
    x = kingpos.value().X() - 1;
    y = kingpos.value().Y() - 1;
    if (x >= 0 && y >= 0) {
      pawns.emplace_back(x, y);
    }
  }
  for (auto pos : pawns) {
    const Piece *p = this->GetPiece(pos);
    if (dynamic_cast<const Pawn*>(p) != nullptr && p->GetColor() == Other(color)) {
      return true;
    }
  }
  return false;
}

void Board::DoMove(const Move& move) {
  std::unique_ptr<Piece>& from = GetMutablePiece(board_, move.From());
  std::unique_ptr<Piece>& to = GetMutablePiece(board_, move.To());
  to = std::move(from);
  from = nullptr;
  to->DoMove(*this, move);
}

void Board::NewTurn() {
  for (int x = 0; x <= 7; ++x) {
    for (int y = 0; y <= 7; ++y) {
      Piece* piece = board_[x][y].get();
      if (piece != nullptr && piece->GetColor() == current_player_) {
        piece->NewTurn();
      }
    }
  }
  ++turn_;
  current_player_ = Other(current_player_);
}

void Board::Set(Position position, std::unique_ptr<Piece> piece) {
  GetMutablePiece(board_, position) = std::move(piece);
}

std::list<const Piece*> Board::GetPieces() const {
  std::list<const Piece*> pieces;
  for (int x = 0; x <= 7; ++x) {
    for (int y = 0; y <= 7; ++y) {
      Piece* piece = board_[x][y].get();
      if (piece != nullptr) {
        pieces.push_back(piece);
      }
    }
  }
  return pieces;
}

std::string Board::Hash() const {
  std::string hash;
  hash.reserve(128);
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      const Pawn *p;
      const King *k;
      const Rook *r;
      const Knight *n;
      const Bishop *b;
      const Queen *q;
      if ((p = dynamic_cast<const Pawn*>(board_[i][j].get())) != nullptr) {
        hash += p->GetColor() == kWhite ? "P" : "p";
      } else if ((n = dynamic_cast<const Knight*>(board_[i][j].get())) != nullptr) {
        hash += n->GetColor() == kWhite ? "N" : "n";
      } else if ((b = dynamic_cast<const Bishop*>(board_[i][j].get())) != nullptr) {
        hash += b->GetColor() == kWhite ? "B" : "b";
      } else if ((r = dynamic_cast<const Rook*>(board_[i][j].get())) != nullptr) {
        hash += r->GetColor() == kWhite ? "R" : "r";
        if (!r->Moved()) {
          hash += "'";
        }
      } else if ((q = dynamic_cast<const Queen*>(board_[i][j].get())) != nullptr) {
        hash += q->GetColor() == kWhite ? "Q" : "q";
      } else if ((k = dynamic_cast<const King*>(board_[i][j].get())) != nullptr) {
        hash += k->GetColor() == kWhite ? "K" : "k";
        if (!k->Moved()) {
          hash += "'";
        }
      } else {
        hash += ".";
      }
    }
  }
  return hash + "_" + ColorToString(current_player_);
}

Color Board::CurrentPlayer() const { return current_player_; }