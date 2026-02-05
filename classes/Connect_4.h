#pragma once
#include "Game.h"
#include <vector>
#include <string>

// Header for Connect 4 game

class Connect_4 : public Game {
public:
    Connect_4();
    ~Connect_4();

    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;

    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;

    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

    void        stopGame() override;

    void        updateAI() override;
    bool        gameHasAI() override { return true; }

    Grid*       getGrid() override { return _grid; }

private:
    Bit*        PieceForPlayer(int playerNumber);
    Player*     ownerAt(int index) const;

    // AI helpers using your existing naming
    bool        dropPiece(int column, int player);                   // make a move in a column
    void        undoPiece(std::string &state, int column);          // undo a move in a column
    bool        checkWin(const std::string &state, int player) const; // check if a player wins
    int         evaluateState(const std::string &state, int player) const; // evaluate board
    std::vector<int> getValidMoves(const std::string &state) const; // list of playable columns
    int         negamax(std::string &state, int depth, int playerColor); // recursive AI

    Grid*       _grid;
    int         _maxDepth = 6;
    bool        _gameOver = false;
};
