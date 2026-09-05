#include "tictactoe/game.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <limits>

namespace {

using tictactoe::GameState;
using tictactoe::Mark;

class Game : public ::testing::Test {
  protected:
    // Plays a move that is expected to be accepted, so sequences below stay readable.
    void play(std::size_t row, std::size_t col) {
        EXPECT_TRUE(game.makeMove({row, col}))
            << "move (" << row << ", " << col << ") was rejected";
    }

    tictactoe::Game game;
};

TEST_F(Game, BasicMove) {
    EXPECT_TRUE(game.makeMove({0, 0}));
    EXPECT_EQ(game.state(), GameState::InProgress);
}

TEST_F(Game, StartsInProgress) {
    EXPECT_EQ(game.state(), GameState::InProgress);
}

TEST_F(Game, OccupiedCellIsRejected) {
    play(0, 0);
    EXPECT_FALSE(game.makeMove({0, 0}));
    EXPECT_EQ(game.state(), GameState::InProgress);
}

TEST_F(Game, OutOfRangeMoveIsRejected) {
    constexpr std::size_t Huge = std::numeric_limits<std::size_t>::max();
    EXPECT_FALSE(game.makeMove({3, 0}));
    EXPECT_FALSE(game.makeMove({0, 3}));
    EXPECT_FALSE(game.makeMove({Huge, Huge}));
    EXPECT_EQ(game.state(), GameState::InProgress);
}

// X wins the top row. Reaching a win at all proves the players alternate:
// without alternation the three cells would not share a mark.
TEST_F(Game, RowWin) {
    play(0, 0); // X
    play(1, 0); // O
    play(0, 1); // X
    play(1, 1); // O
    play(0, 2); // X
    EXPECT_EQ(game.state(), GameState::Won);
}

TEST_F(Game, ColumnWin) {
    play(0, 0); // X
    play(0, 1); // O
    play(1, 0); // X
    play(1, 1); // O
    play(2, 0); // X
    EXPECT_EQ(game.state(), GameState::Won);
}

TEST_F(Game, DiagonalWin) {
    play(0, 0); // X
    play(0, 1); // O
    play(1, 1); // X
    play(0, 2); // O
    play(2, 2); // X
    EXPECT_EQ(game.state(), GameState::Won);
}

TEST_F(Game, AntiDiagonalWin) {
    play(0, 2); // X
    play(0, 0); // O
    play(1, 1); // X
    play(0, 1); // O
    play(2, 0); // X
    EXPECT_EQ(game.state(), GameState::Won);
}

// O wins, so a win is not hard-wired to whoever moves first.
TEST_F(Game, SecondPlayerCanWin) {
    play(0, 0); // X
    play(1, 0); // O
    play(0, 1); // X
    play(1, 1); // O
    play(2, 2); // X
    play(1, 2); // O wins the middle row
    EXPECT_EQ(game.state(), GameState::Won);
}

TEST_F(Game, NoWinnerBeforeThirdMark) {
    play(0, 0); // X
    play(1, 0); // O
    play(0, 1); // X
    EXPECT_EQ(game.state(), GameState::InProgress);
}

// A full board with no line: X O X / X O O / O X X
TEST_F(Game, FullBoardWithoutLineIsADraw) {
    play(0, 0); // X
    play(0, 1); // O
    play(0, 2); // X
    play(1, 1); // O
    play(1, 0); // X
    play(1, 2); // O
    play(2, 1); // X
    play(2, 0); // O
    play(2, 2); // X
    EXPECT_EQ(game.state(), GameState::Draw);
}

TEST_F(Game, MovesAfterAWinAreRejected) {
    play(0, 0); // X
    play(1, 0); // O
    play(0, 1); // X
    play(1, 1); // O
    play(0, 2); // X wins
    ASSERT_EQ(game.state(), GameState::Won);

    EXPECT_FALSE(game.makeMove({2, 2}));
    EXPECT_EQ(game.state(), GameState::Won);
}

TEST_F(Game, MovesAfterADrawAreRejected) {
    play(0, 0);
    play(0, 1);
    play(0, 2);
    play(1, 1);
    play(1, 0);
    play(1, 2);
    play(2, 1);
    play(2, 0);
    play(2, 2);
    ASSERT_EQ(game.state(), GameState::Draw);

    EXPECT_FALSE(game.makeMove({0, 0}));
    EXPECT_EQ(game.state(), GameState::Draw);
}

// A rejected move must not hand the turn to the other player: if it did,
// the marks would land on the wrong squares and X would never get the row.
TEST_F(Game, RejectedMoveDoesNotConsumeATurn) {
    play(0, 0);                          // X
    EXPECT_FALSE(game.makeMove({0, 0})); // O tries an occupied cell
    EXPECT_FALSE(game.makeMove({9, 9})); // O tries an out-of-range cell
    play(1, 0);                          // O
    play(0, 1);                          // X
    play(1, 1);                          // O
    play(0, 2);                          // X
    EXPECT_EQ(game.state(), GameState::Won);
}

} // namespace
