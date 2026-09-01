#include "tictactoe/board.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <limits>

namespace {

using tictactoe::Mark;

class Place : public ::testing::Test {
  protected:
    tictactoe::Board board;
};

TEST_F(Place, ValidMoves) {
    EXPECT_TRUE(board.place(0, 0, Mark::X));
    EXPECT_TRUE(board.place(0, 1, Mark::O));
}

TEST_F(Place, EveryCellIsPlayable) {
    for (std::size_t row = 0; row < tictactoe::Board::Size; ++row) {
        for (std::size_t col = 0; col < tictactoe::Board::Size; ++col) {
            EXPECT_TRUE(board.place(row, col, Mark::X))
                << "cell (" << row << ", " << col << ") was rejected";
        }
    }
}

TEST_F(Place, SameMarkOnSeveralCells) {
    EXPECT_TRUE(board.place(0, 0, Mark::X));
    EXPECT_TRUE(board.place(1, 1, Mark::X));
    EXPECT_TRUE(board.place(2, 2, Mark::X));
}

TEST_F(Place, NoOverlap) {
    EXPECT_TRUE(board.place(0, 0, Mark::X));
    EXPECT_FALSE(board.place(0, 0, Mark::O));
}

TEST_F(Place, NoErase) {
    EXPECT_TRUE(board.place(0, 0, Mark::X));
    EXPECT_FALSE(board.place(0, 0, Mark::Empty));
}

TEST_F(Place, EmptyMarkRejectedOnFreeCell) {
    EXPECT_FALSE(board.place(0, 0, Mark::Empty));
}

TEST_F(Place, Boundaries) {
    EXPECT_FALSE(board.place(3, 2, Mark::X));
    EXPECT_FALSE(board.place(2, 3, Mark::O));
    EXPECT_FALSE(board.place(3, 3, Mark::X));

    constexpr std::size_t Huge = std::numeric_limits<std::size_t>::max();
    EXPECT_FALSE(board.place(Huge, 0, Mark::X));
    EXPECT_FALSE(board.place(0, Huge, Mark::O));
}

TEST_F(Place, LastCellIsInRange) {
    EXPECT_TRUE(board.place(tictactoe::Board::Size - 1, tictactoe::Board::Size - 1, Mark::X));
}

TEST_F(Place, RejectedMoveLeavesCellFree) {
    EXPECT_FALSE(board.place(0, 0, Mark::Empty));
    EXPECT_TRUE(board.place(0, 0, Mark::X));
}

} // namespace
