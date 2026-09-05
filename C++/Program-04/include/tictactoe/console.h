#pragma once

#include "tictactoe/game.h"

#include <string>

namespace tictactoe {

class Console {
  public:
    std::string render(bool isValidMove);
    void play();

  private:
    void clearScreen();

    Game game_{};
};
} // namespace tictactoe