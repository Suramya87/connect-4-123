#pragma once
#include "Game.h"
#include <vector>
#include <string>
#include <bit>
#include <cstdint> 

class Connect_4 : public Game {
public:
    std::string getDebugInfo() const { return _debugInfo; }
    std::string _debugInfo;
    
    Connect_4();
    ~Connect_4();

    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;

    void        setUpBoard() override;
    Player* checkForWinner() override;
    bool        checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;

    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

    void        stopGame() override;
    void        updateAI() override;

    virtual bool gameHasAI() override { return _aiEnabled; }
    void setAIEnabled(bool enabled) { _aiEnabled = enabled; }

    Grid* getGrid() override { return _grid; }
    void        setAIGoesFirst(bool goesFirst) { _aiGoesFirst = goesFirst; }

private:
    bool _aiEnabled = true;   
    bool _aiGoesFirst = false;

    static constexpr int HUMAN_PLAYER = 0; 
    static constexpr int AI_PLAYER = 1;

    Bit* PieceForPlayer(int playerNumber);
    Player* ownerAt(int index) const;

    bool        dropPiece(int column, int player);
    void        dropPieceSim(std::string &state, int column, int player);
    void        undoPiece(std::string &state, int column);

    std::vector<int> getValidMoves(const std::string &state) const;
    int         negamax(std::string &state, int depth, int alpha, int beta, int player);

    uint64_t _bitboard[2]; 
    void stateToBitboards(const std::string &state, uint64_t bb[2]) const;
    bool checkWinBitboard(uint64_t board) const;
    int evaluateBitboard(uint64_t myBoard, uint64_t oppBoard) const;

    int getAIPlayerNumber() const { return _aiGoesFirst ? 0 : 1; }
    int getHumanPlayerNumber() const { return _aiGoesFirst ? 1 : 0; }

    Grid* _grid;
    int         _maxDepth = 5;
    bool        _gameOver = false;
};