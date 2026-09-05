#pragma once

#include "tictactoe/board.h"
#include "tictactoe/types.h"

namespace tictactoe {

class Game {
  public:
    [[nodiscard]] bool makeMove(Position position);
    [[nodiscard]] const Board& board() const { return board_; }
    [[nodiscard]] Mark currentPlayer() const { return currentPlayer_; };
    [[nodiscard]] GameState state() const { return state_; };
    [[nodiscard]] bool isInProgress() const { return state_ == GameState::InProgress; };
    [[nodiscard]] Mark winner() const { return winner_; };
    [[nodiscard]] bool hasWinner() const { return winner_ != Mark::Empty; };

  private:
    Board board_;
    Mark currentPlayer_ = Mark::X;
    GameState state_ = GameState::InProgress;
    Mark winner_ = Mark::Empty;

    void updateState();
    bool hasWon(Mark mark) const;
};

} // namespace tictactoe