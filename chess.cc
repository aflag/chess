#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <vector>
#include <cstring>
#include <set>

#include "board.h"
#include "position.h"
#include "color.h"
#include "move.h"
#include "engine.h"

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

Move ChooseAiMove(Board& board, Color color, int depth, double (*utility)(Board& board, GameOutcome outcome, Color attackingcolor)) {
  auto t0 = std::chrono::high_resolution_clock::now();
  auto valid_moves = ComputeUtility(board, color, depth, utility);
  std::chrono::duration<double, std::milli> delta = std::chrono::high_resolution_clock::now() - t0;
  std::cout << "# Move found in: " << (delta.count() / 1000.0) << "s" << std::endl;
  std::sort(valid_moves.begin(), valid_moves.end(), ColorfulCompare(color));
  for (Move move : valid_moves) {
    std::cout << "# " << move.From().String() << " " << move.To().String() << " " << move.Utility() << std::endl;
  }
  return valid_moves.back();
}

int main(int argc, char *argv[]) {
  if (argc > 1 && !strcmp(argv[1], "ascii")) {
    srand(unsigned(time(nullptr)));
    Board board;
    while (true) {
      board.NewTurn(kWhite);
      board.Print();
      auto valid_human_moves = board.GetMoves(kWhite);
      switch (GetGameOutcome(!valid_human_moves.empty(), board.IsCheck(kWhite))) {
      case kCheckmate:
        std::cout << "You lose!" << std::endl;
        return 0;
      case kDraw:
        std::cout << "Stalemate draw." << std::endl;
        return 0;
      }
      Move human_move = ReadHumanMove(valid_human_moves);
      board.DoMove(human_move);
      board.NewTurn(kBlack);
      board.Print();
      auto valid_ai_moves = board.GetMoves(kBlack);
      switch (GetGameOutcome(!valid_ai_moves.empty(), board.IsCheck(kBlack))) {
      case kCheckmate:
        std::cout << "You win!" << std::endl;
        return 0;
      case kDraw:
        std::cout << "Stalemate draw." << std::endl;
        return 0;
      }
      Move ai_move = ChooseAiMove(board, kBlack, 4, SmartUtility);
      std::cout << "AI played: " << ai_move.String() << std::endl;
      board.DoMove(ai_move);
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
        board.NewTurn(mycolor);
        auto valid_ai_moves = board.GetMoves(mycolor);
        Move ai_move = ChooseAiMove(board, mycolor, 4, SmartUtility);
        board.DoMove(ai_move);
        std::cout << "move " << ai_move.XboardString() << std::endl;
        board.NewTurn(Other(mycolor));
      } else if (ignored.find(command) != ignored.end()) {
        // ignore
      } else {
        // not a known command, must be a move
        if (first_move && Other(mycolor) == kWhite) {
          board.NewTurn(kWhite);
        }
        first_move = false;
        auto human_move = Move::FromXboardString(command);
        if (!human_move.has_value()) {
          std::cout << "Ilegal move: " << command << std::endl;
          continue;
        }
        board.DoMove(*human_move);

        board.NewTurn(mycolor);
        auto valid_ai_moves = board.GetMoves(mycolor);
        switch (GetGameOutcome(!valid_ai_moves.empty(), board.IsCheck(mycolor))) {
        case kCheckmate:
          std::cout << "1-0 {White mates}" << std::endl;
          continue;
        case kDraw:
          std::cout << "1/2-1/2 {Stalemate}" << std::endl;
          continue;
        }

        Move ai_move = ChooseAiMove(board, mycolor, 4, SmartUtility);
        board.DoMove(ai_move);
        std::cout << "move " << ai_move.XboardString() << std::endl;

        board.NewTurn(Other(mycolor));
        auto valid_human_moves = board.GetMoves(Other(mycolor));
        switch (GetGameOutcome(!valid_human_moves.empty(), board.IsCheck(Other(mycolor)))) {
        case kCheckmate:
          std::cout << "0-1 {Black mates}" << std::endl;
          continue;
        case kDraw:
          std::cout << "1/2-1/2 {Stalemate}" << std::endl;
          continue;
        }
      }
    }
  }
  return 0;
}
