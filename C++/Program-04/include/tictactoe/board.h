#pragma once

#include "tictactoe/types.h"

#include <array>
#include <cstddef>

namespace tictactoe {

class Board {
  public:
    static constexpr std::size_t Size = 3;

    [[nodiscard]] bool place(std::size_t row, std::size_t col, Mark mark);

    [[nodiscard]] Mark at(std::size_t row, std::size_t col) const;

    [[nodiscard]] bool full() const;

  private:
    [[nodiscard]] bool isValidMove(std::size_t row, std::size_t col) const {
      if (row >= Size || col >= Size) {
        return false;
      }

      return true;
    }

    std::array<std::array<Mark, Size>, Size> cells_ {
        {
            {Mark::Empty, Mark::Empty, Mark::Empty},
            {Mark::Empty, Mark::Empty, Mark::Empty},
            {Mark::Empty, Mark::Empty, Mark::Empty}
        }
    };
};

} // namespace tictactoe
