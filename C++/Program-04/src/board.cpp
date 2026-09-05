#include "tictactoe/board.h"

#include <algorithm>
#include <ranges>

namespace tictactoe {

bool Board::place(Position position, Mark mark) {
    if (!isValidMove(position) || mark == Mark::Empty) {
        return false;
    }

    auto [row, col] = position;

    if (cells_[row][col] != Mark::Empty) {
        return false;
    }

    cells_[row][col] = mark;

    return true;
}

bool Board::full() const {
    return std::ranges::none_of(cells_ | std::views::join,
                                [](Mark mark) { return mark == Mark::Empty; });
}

Mark Board::at(Position position) const {
    return cells_[position.row][position.col];
}

} // namespace tictactoe
