#include "tictactoe/game.h"

#include <algorithm>
#include <ranges>

namespace tictactoe {

bool Game::makeMove(Position position) {
    if (state_ != GameState::InProgress) {
        return false;
    }

    if (!board_.place(position, currentPlayer_)) {
        return false;
    }

    updateState();

    if (state_ == GameState::InProgress) {
        currentPlayer_ = currentPlayer_ == Mark::X ? Mark::O : Mark::X;
    }

    return true;
}

using Line = std::array<Position, Board::Size>;

consteval std::array<Line, 2 * Board::Size + 2> makeWinLines() {
    return {{
        {{{0, 0}, {0, 1}, {0, 2}}}, // Row 0
        {{{1, 0}, {1, 1}, {1, 2}}}, // Row 1
        {{{2, 0}, {2, 1}, {2, 2}}}, // Row 2
        {{{0, 0}, {1, 0}, {2, 0}}}, // Col 0
        {{{0, 1}, {1, 1}, {2, 1}}}, // Col 1
        {{{0, 2}, {1, 2}, {2, 2}}}, // Col 2
        {{{0, 0}, {1, 1}, {2, 2}}}, // Diagonal 1
        {{{0, 2}, {1, 1}, {2, 0}}}  // Diagonal 2
    }};
}
inline constexpr auto WinLines = makeWinLines();

bool Game::hasWon(Mark mark) const {
    return std::ranges::any_of(WinLines, [&](const Line& line) {
        return std::ranges::all_of(line, [&](Position pos) { return board_.at(pos) == mark; });
    });
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
