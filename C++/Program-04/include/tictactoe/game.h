#pragma once

#include "tictactoe/board.h"
#include "tictactoe/types.h"

namespace tictactoe {

class Game {
  public:
    [[nodiscard]] bool makeMove(std::size_t row, std::size_t col);
    [[nodiscard]] Board& board() const;
    [[nodiscard]] Mark currentPlayer() const;
    [[nodiscard]] GameState state() const {return state_;};

  private:
    Board board_;
    Mark currentPlayer_ = Mark::X;
    GameState state_ = GameState::InProgress;
    Mark winner_ = Mark::Empty;

    void updateState();
    bool hasWon(Mark mark) const;
};

} // namespace tictactoe