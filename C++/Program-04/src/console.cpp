#include "tictactoe/console.h"

#include "tictactoe/types.h"

#include <format>
#include <iostream>

namespace tictactoe {

std::string Console::render(bool isValidMove) {
    const auto& board = game_.board();

    constexpr std::string_view separator = "---+---+---";
    constexpr std::string_view input = "Enter row col: ";
    constexpr std::string_view invalid = "(Invalid move)";

    std::string frame = std::format(
        R"( {} | {} | {}
{}
 {} | {} | {}
{}
 {} | {} | {}
{} to move {}
{})",   board.at({0, 0}), board.at({0, 1}), board.at({0, 2}), separator, board.at({1, 0}),
        board.at({1, 1}), board.at({1, 2}), separator, board.at({2, 0}), board.at({2, 1}),
        board.at({2, 2}), game_.currentPlayer(), isValidMove ? "" : invalid, input);

    return frame;
}

void Console::play() {
    Position pos;
    auto isValidMove = true;
    while (game_.isInProgress()) {
        clearScreen();
        std::cout << render(isValidMove);
        std::cin >> pos.row >> pos.col;
        isValidMove = game_.makeMove(pos);
    }

    if (game_.hasWinner()) {
        std::cout << glyph(game_.winner()) << " Won!!!\n";
    } else {
        std::cout << "Draw!\n";
    }
}

void Console::clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

} // namespace tictactoe