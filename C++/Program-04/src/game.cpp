#include "tictactoe/game.h"

namespace tictactoe {

bool Game::makeMove(std::size_t row, std::size_t col) {
    if (state_ != GameState::InProgress) {
        return false;
    }

    if (!board_.place(row, col, currentPlayer_)) {
        return false;
    }

    updateState();

    if (state_ == GameState::InProgress) {
        currentPlayer_ = currentPlayer_ == Mark::X ? Mark::O : Mark::X;
    }

    return true;
}

bool Game::hasWon(Mark mark) const {
    // Rows
    for (std::size_t row = 0; row < Board::Size; ++row) {
        if (board_.at(row, 0) == mark && board_.at(row, 1) == mark && board_.at(row, 2) == mark) {
            return true;
        }
    }

    // Columns
    for (std::size_t col = 0; col < Board::Size; ++col) {
        if (board_.at(0, col) == mark && board_.at(1, col) == mark && board_.at(2, col) == mark) {
            return true;
        }
    }

    // Diagonal
    if (board_.at(0, 0) == mark && board_.at(1, 1) == mark && board_.at(2, 2) == mark) {
        return true;
    }

    // Other diagonal
    if (board_.at(0, 2) == mark && board_.at(1, 1) == mark && board_.at(2, 0) == mark) {
        return true;
    }

    return false;
}

void Game::updateState() {
    if (hasWon(currentPlayer_)) {
        state_ = GameState::Won;
        winner_ = currentPlayer_;
        return;
    }

    if (board_.full()) {
        state_ = GameState::Draw;
    }
}

} // namespace tictactoe
