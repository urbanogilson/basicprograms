#include "tictactoe/board.h"

namespace tictactoe {

bool Board::place(std::size_t row, std::size_t col, Mark mark) {
    if (!isValidMove(row, col) || mark == Mark::Empty) {
        return false;
    }

    if (cells_[row][col] != Mark::Empty) {
        return false;
    }

    cells_[row][col] = mark;

    return true;
}

bool Board::full() const {
    for(const auto & col : cells_){
        for(const auto & mark: col) {
            if (mark == Mark::Empty) {
                return false;
            }
        }
    }

    return true;
}

Mark Board::at(std::size_t row, std::size_t col) const {
    return cells_[row][col];
}

} // namespace tictactoe
