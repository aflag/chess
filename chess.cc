#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <set>

#include "board.h"
#include "position.h"
#include "color.h"
#include "move.h"
#include "engine.h"

const int kDepth = 4;
const auto kUtility = SmartUtility;

Move ReadHumanMove(const std::vector<Move>& valid_moves) {
  while (true) {
    std::cout << "Your move: ";
    std::string from, to;
    std::cin >> from >> to;
    auto move = Move::FromString(from, to);
    if (!move.has_value()) {
      std::cout << "Invalid syntax." << std::endl;
      continue;
    }
    if (std::find(valid_moves.begin(), valid_moves.end(), *move) ==
        valid_moves.end()) {
      std::cout << "Invalid move. Valids: " << std::endl;
      for (const Move& valid_move : valid_moves) {
        std::cout << "  " << valid_move.String() << std::endl;
      }
      continue;
    }
    return *move;
  }
}

Move ChooseAiMove(
  Board& board,
  Color color,
  int depth,
  double (*utility)(Board& board, Color attackingcolor),
  Cache& cache
) {
  auto t0 = std::chrono::high_resolution_clock::now();
  auto valid_moves = ComputeUtility(board, color, depth, utility, cache);
  std::chrono::duration<double, std::milli> delta = std::chrono::high_resolution_clock::now() - t0;
  std::cout << "# Move found in: " << (delta.count() / 1000.0) << "s" << std::endl;
  std::sort(valid_moves.begin(), valid_moves.end(), ColorfulCompare(color));
  for (Move move : valid_moves) {
    std::cout << "# " << move.From().String() << " " << move.To().String() << " " << move.Utility() << std::endl;
  }
  return valid_moves.back();
}

int main(int argc, char *argv[]) {
  Cache cache;

  if (argc > 1 && !strcmp(argv[1], "ascii")) {
    srand(unsigned(time(nullptr)));
    Board board;
    while (true) {
      board.Print();
      auto valid_human_moves = board.GetMoves();
      switch (board.GetGameOutcome()) {
        case kCheckmate:
          std::cout << "You lose!" << std::endl;
          return 0;
        case kDraw:
          std::cout << "Draw" << std::endl;
          return 0;
        case kInProgress:
          break;
      }
      Move human_move = ReadHumanMove(valid_human_moves);
      board.DoMove(human_move);
      board.NewTurn();
      board.Print();
      auto valid_ai_moves = board.GetMoves();
      switch (board.GetGameOutcome()) {
        case kCheckmate:
          std::cout << "You win!" << std::endl;
          return 0;
        case kDraw:
          std::cout << "Draw" << std::endl;
          return 0;
        case kInProgress:
          break;
      }
      Move ai_move = ChooseAiMove(board, kBlack, kDepth, kUtility, cache);
      std::cout << "AI played: " << ai_move.String() << std::endl;
      board.DoMove(ai_move);
      board.NewTurn();
    }
  } else {
    std::cout.setf(std::ios::unitbuf);
    std::string line;
    std::getline(std::cin, line);
    if (line != "xboard") {
      return 1;
    }
    std::set<std::string> ignored = {
      "new",
      "random",
      "level",
      "post"
      "hard",
      "time",
      "otim",
      "post",
      "hard",
      "accepted",
      "force",
      "computer",
    };
    Board board;
    Color mycolor = kBlack;
    bool first_move = true;

    while (std::getline(std::cin, line)) {
      std::string command = line.substr(0, line.find(" "));
      if (command == "quit") {
        return 0;
      } else if (command == "protover") {
        std::cout << "feature reuse=0 sigint=0 sigterm=0" << std::endl;
      } else if (command == "white") {
        mycolor = kWhite;
      } else if (command == "black") {
        mycolor = kBlack;
      } else if (command == "go" && first_move) {
        auto valid_ai_moves = board.GetMoves();
        Move ai_move = ChooseAiMove(board, mycolor, kDepth, kUtility, cache);
        board.DoMove(ai_move);
        board.NewTurn();

        std::cout << "move " << ai_move.XboardString() << std::endl;
      } else if (ignored.find(command) != ignored.end()) {
        // ignore
      } else {
        // not a known command, must be a move
        first_move = false;
        auto human_move = Move::FromXboardString(command);
        if (!human_move.has_value()) {
          std::cout << "Ilegal move: " << command << std::endl;
          continue;
        }
        board.DoMove(*human_move);
        board.NewTurn();

        auto valid_ai_moves = board.GetMoves();
        switch (board.GetGameOutcome()) {
          case kCheckmate:
            std::cout << "1-0 {White mates}" << std::endl;
            continue;
          case kDraw:
            std::cout << "1/2-1/2 {draw}" << std::endl;
            continue;
          case kInProgress:
            break;
        }

        Move ai_move = ChooseAiMove(board, mycolor, kDepth, kUtility, cache);
        board.DoMove(ai_move);
        board.NewTurn();

        std::cout << "move " << ai_move.XboardString() << std::endl;

        auto valid_human_moves = board.GetMoves();
        switch (board.GetGameOutcome()) {
          case kCheckmate:
            std::cout << "0-1 {Black mates}" << std::endl;
            continue;
          case kDraw:
            std::cout << "1/2-1/2 {draw}" << std::endl;
            continue;
          case kInProgress:
            break;
        }
      }
    }
  }

  return 0;
}
