#pragma once

namespace tictactoe {

enum class Mark {
    Empty,
    X,
    O
};

enum class GameState {
    InProgress,
    Won,
    Draw
};

} // namespace tictactoe
