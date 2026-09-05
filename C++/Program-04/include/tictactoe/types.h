#pragma once

#include <cstdint>
#include <format>

namespace tictactoe {

enum class Mark { Empty, X, O };

enum class GameState { InProgress, Won, Draw };

struct Position {
    std::size_t row{};
    std::size_t col{};
    friend constexpr bool operator==(Position, Position) = default;
};

constexpr char glyph(Mark mark) {
    using enum Mark;
    switch (mark) {
    case X:
        return 'X';
    case O:
        return 'O';
    case Empty:
        return '.';
    default:
        return '?';
    }
}

} // namespace tictactoe

template <> struct std::formatter<tictactoe::Mark> : std::formatter<char> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const tictactoe::Mark& mark, std::format_context& ctx) const {
        return std::formatter<char>::format(tictactoe::glyph(mark), ctx);
    }
};