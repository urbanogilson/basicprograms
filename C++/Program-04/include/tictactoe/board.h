#pragma once

#include "tictactoe/types.h"

#include <array>
#include <cstddef>

namespace tictactoe {

class Board {
  public:
    static constexpr std::size_t Size = 3;

    [[nodiscard]] bool place(Position position, Mark mark);

    [[nodiscard]] Mark at(Position position) const;

    [[nodiscard]] bool full() const;

  private:
    [[nodiscard]] bool isValidMove(Position position) const {
        if (position.row >= Size || position.col >= Size) {
            return false;
        }

        return true;
    }

    std::array<std::array<Mark, Size>, Size> cells_{{{Mark::Empty, Mark::Empty, Mark::Empty},
                                                     {Mark::Empty, Mark::Empty, Mark::Empty},
                                                     {Mark::Empty, Mark::Empty, Mark::Empty}}};
};

} // namespace tictactoe
