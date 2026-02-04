#pragma once 
#include "Game.h"

// Header for connect 4 game 

// Game class

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

    // AI helpers
    bool        dropPiece(int column, int player);
    bool        checkWin(const std::string& state, int player) const;
    int         evaluateState(const std::string& state, int player) const;
    std::vector<int> getValidMoves(const std::string& state) const;

    int         negamax(std::string& state, int depth, int playerColor);

    Grid*       _grid;
    int         _maxDepth = 6;
};


