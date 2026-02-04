#include "Connect_4.h"

static const int ROWS = 6;
static const int COLS = 7;

Connect_4::Connect_4()
{
    _grid = new Grid(COLS, ROWS);
}

Connect_4::~Connect_4()
{
    delete _grid;
}

//
// make a piece
//
Bit* Connect_4::PieceForPlayer(const int playerNumber)
{
    Bit* bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void Connect_4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = COLS;
    _gameOptions.rowY = ROWS;

    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

//
// Player action (column-based, gravity enforced)
//
bool Connect_4::actionForEmptyHolder(BitHolder& holder)
{
    int clickedColumn = -1;

    // Find which column this holder belongs to
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        if (square == &holder) {
            clickedColumn = x;
        }
    });

    if (clickedColumn < 0)
        return false;

    // Apply gravity: find lowest empty square in that column
    for (int row = ROWS - 1; row >= 0; row--) {
        ChessSquare* square = _grid->getSquare(clickedColumn, row);
        if (!square->bit()) {
            Bit* bit = PieceForPlayer(
                getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER
            );
            bit->setPosition(square->getPosition());
            square->setBit(bit);
            endTurn();
            return true;
        }
    }

    return false; // column full
}


bool Connect_4::canBitMoveFrom(Bit&, BitHolder&)
{
    return false;
}

bool Connect_4::canBitMoveFromTo(Bit&, BitHolder&, BitHolder&)
{
    return false;
}

void Connect_4::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper for winner check
//
Player* Connect_4::ownerAt(int index) const
{
    int x = index % COLS;
    int y = index / COLS;

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit())
        return nullptr;

    return square->bit()->getOwner();
}

//
// winner detection (template)
//
Player* Connect_4::checkForWinner()
{
    std::string s = stateString();

    // TODO: horizontal, vertical, diagonal checks
    // follow same pattern as TicTacToe, just more cases

    return nullptr;
}

bool Connect_4::checkForDraw()
{
    bool draw = true;
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        if (!square->bit())
            draw = false;
    });
    return draw;
}

//
// state strings
//
std::string Connect_4::initialStateString()
{
    return std::string(ROWS * COLS, '0');
}

std::string Connect_4::stateString()
{
    std::string s(ROWS * COLS, '0');

    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        if (square->bit()) {
            s[y * COLS + x] =
                std::to_string(square->bit()->getOwner()->playerNumber() + 1)[0];
        }
    });

    return s;
}

void Connect_4::setStateString(const std::string& s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * COLS + x;
        int player = s[index] - '0';

        if (player)
            square->setBit(PieceForPlayer(player - 1));
        else
            square->setBit(nullptr);
    });
}

//
// AI entry point (simple placeholder, same style as TicTacToe)
//
void Connect_4::updateAI()
{
    BitHolder* bestMove = nullptr;
    std::string state = stateString();

    // pick first valid column (placeholder logic)
    for (int col = 0; col < COLS && !bestMove; col++) {
        auto square = _grid->getSquare(col, 0);
        if (!square->bit()) {
            bestMove = square;
        }
    }

    if (bestMove) {
        actionForEmptyHolder(*bestMove);
    }
}
