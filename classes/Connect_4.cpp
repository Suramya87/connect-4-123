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
    _gameOver = false;
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
// bool Connect_4::actionForEmptyHolder(BitHolder& holder)
// {
//     int clickedColumn = -1;

//     // Find which column this holder belongs to
//     _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
//         if (square == &holder) {
//             clickedColumn = x;
//         }
//     });

//     if (clickedColumn < 0)
//         return false;

//     // Apply gravity: find lowest empty square in that column
//     for (int row = ROWS - 1; row >= 0; row--) {
//         ChessSquare* square = _grid->getSquare(clickedColumn, row);
//         if (!square->bit()) {
//             Bit* bit = PieceForPlayer(
//                 getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER
//             );
//             bit->setPosition(square->getPosition());
//             square->setBit(bit);
//             endTurn();
//             return true;
//         }
//     }

//     return false; // column full
// }

bool Connect_4::actionForEmptyHolder(BitHolder& holder)
{
    if (_gameOver)
        return false;

    int clickedColumn = -1;

    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        if (square == &holder) {
            clickedColumn = x;
        }
    });

    if (clickedColumn < 0)
        return false;

    for (int row = ROWS - 1; row >= 0; row--) {
        ChessSquare* square = _grid->getSquare(clickedColumn, row);
        if (!square->bit()) {

            Bit* bit = PieceForPlayer(
                getCurrentPlayer()->playerNumber() == 0
                    ? HUMAN_PLAYER
                    : AI_PLAYER
            );

            bit->setPosition(square->getPosition());
            square->setBit(bit);

            // 🔴 detect win BEFORE turn advances
            if (checkForWinner() || checkForDraw()) {
                _gameOver = true;   // lock the game
                endTurn();          // engine prints correct winner
                return true;
            }

            endTurn();
            return true;
        }
    }

    return false;
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
    // Horizontal
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x <= COLS - 4; x++) {
            int i = y * COLS + x;
            Player* p = ownerAt(i);
            if (p &&
                p == ownerAt(i + 1) &&
                p == ownerAt(i + 2) &&
                p == ownerAt(i + 3))
                return p;
        }
    }

    // Vertical
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y <= ROWS - 4; y++) {
            int i = y * COLS + x;
            Player* p = ownerAt(i);
            if (p &&
                p == ownerAt(i + COLS) &&
                p == ownerAt(i + 2 * COLS) &&
                p == ownerAt(i + 3 * COLS))
                return p;
        }
    }

    // Diagonal down-right (\)
    for (int y = 0; y <= ROWS - 4; y++) {
        for (int x = 0; x <= COLS - 4; x++) {
            int i = y * COLS + x;
            Player* p = ownerAt(i);
            if (p &&
                p == ownerAt(i + COLS + 1) &&
                p == ownerAt(i + 2 * (COLS + 1)) &&
                p == ownerAt(i + 3 * (COLS + 1)))
                return p;
        }
    }

    // Diagonal up-right (/)
    for (int y = 3; y < ROWS; y++) {
        for (int x = 0; x <= COLS - 4; x++) {
            int i = y * COLS + x;
            Player* p = ownerAt(i);
            if (p &&
                p == ownerAt(i - COLS + 1) &&
                p == ownerAt(i - 2 * COLS + 2) &&
                p == ownerAt(i - 3 * COLS + 3))
                return p;
        }
    }

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
// Update AI
//

// Place a piece in a column, respecting gravity
bool Connect_4::dropPiece(int column, int player) {
    for (int row = ROWS - 1; row >= 0; row--) {
        ChessSquare* square = _grid->getSquare(column, row);
        if (!square->bit()) {
            Bit* bit = PieceForPlayer(player - 1);
            bit->setPosition(square->getPosition());
            square->setBit(bit);
            return true;
        }
    }
    return false;
}


// Simulation-only drop
void Connect_4::dropPieceSim(std::string &state, int column, int player) {
    for (int row = ROWS - 1; row >= 0; row--) {
        int idx = row * COLS + column;
        if (state[idx] == '0') {
            state[idx] = '0' + player;
            break;
        }
    }
}



// Undo the last piece in a column (for recursion)
void Connect_4::undoPiece(std::string &state, int column) {
    for (int row = 0; row < ROWS; row++) {
        int idx = row * COLS + column;
        if (state[idx] != '0') {
            state[idx] = '0';
            break;
        }
    }
}
// Check if a given player has won
bool Connect_4::checkWin(const std::string &state, int player) const {
    char pChar = '0' + player;

    // Horizontal
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c <= COLS - 4; c++) {
            if (state[r * COLS + c] == pChar &&
                state[r * COLS + c + 1] == pChar &&
                state[r * COLS + c + 2] == pChar &&
                state[r * COLS + c + 3] == pChar)
                return true;
        }
    }

    // Vertical
    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r <= ROWS - 4; r++) {
            if (state[r * COLS + c] == pChar &&
                state[(r + 1) * COLS + c] == pChar &&
                state[(r + 2) * COLS + c] == pChar &&
                state[(r + 3) * COLS + c] == pChar)
                return true;
        }
    }

    // Diagonal down-right (\)
    for (int r = 0; r <= ROWS - 4; r++) {
        for (int c = 0; c <= COLS - 4; c++) {
            if (state[r * COLS + c] == pChar &&
                state[(r + 1) * COLS + c + 1] == pChar &&
                state[(r + 2) * COLS + c + 2] == pChar &&
                state[(r + 3) * COLS + c + 3] == pChar)
                return true;
        }
    }

    // Diagonal up-right (/)
    for (int r = 3; r < ROWS; r++) {
        for (int c = 0; c <= COLS - 4; c++) {
            if (state[r * COLS + c] == pChar &&
                state[(r - 1) * COLS + c + 1] == pChar &&
                state[(r - 2) * COLS + c + 2] == pChar &&
                state[(r - 3) * COLS + c + 3] == pChar)
                return true;
        }
    }

    return false;
}

// Simple evaluation function
int Connect_4::evaluateState(const std::string &state, int player) const {
    int score = 0;
    int opponent = (player == AI_PLAYER + 1 ? HUMAN_PLAYER + 1 : AI_PLAYER + 1);

    auto countLine = [&](int r, int c, int dr, int dc) -> int {
        int playerCount = 0, oppCount = 0;
        for (int i = 0; i < 4; i++) {
            char cell = state[(r + i * dr) * COLS + (c + i * dc)];
            if (cell == '0' + player) playerCount++;
            else if (cell == '0' + opponent) oppCount++;
        }
        if (playerCount > 0 && oppCount == 0) return playerCount;
        if (oppCount > 0 && playerCount == 0) return -oppCount;
        return 0;
    };

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (c <= COLS - 4) score += countLine(r, c, 0, 1); // horizontal
            if (r <= ROWS - 4) score += countLine(r, c, 1, 0); // vertical
            if (r <= ROWS - 4 && c <= COLS - 4) score += countLine(r, c, 1, 1); // diag 
            if (r >= 3 && c <= COLS - 4) score += countLine(r, c, -1, 1); // diag /
        }
    }

    return score;
}

std::vector<int> Connect_4::getValidMoves(const std::string &state) const {
    std::vector<int> moves;
    for (int col = 0; col < COLS; col++) {
        if (state[col] == '0') moves.push_back(col); // top row empty
    }
    return moves;
}

int Connect_4::negamax(std::string &state, int depth, int playerColor) {
    if (checkWin(state, AI_PLAYER + 1)) return 1000 - (_maxDepth - depth);
    if (checkWin(state, HUMAN_PLAYER + 1)) return -1000 + (_maxDepth - depth);
    if (depth == 0 || getValidMoves(state).empty()) return evaluateState(state, AI_PLAYER + 1);

    int bestVal = -10000;

    for (int col : getValidMoves(state)) {
        dropPieceSim(state, col, playerColor);  // simulate on string only
        int val = -negamax(state, depth - 1, playerColor == AI_PLAYER + 1 ? HUMAN_PLAYER + 1 : AI_PLAYER + 1);
        undoPiece(state, col);                   // undo in string

        bestVal = std::max(bestVal, val);
    }

    return bestVal;
}




void Connect_4::updateAI() {
    if (_gameOver) return;

    std::string state = stateString();
    int bestScore = -10000;
    int bestCol = -1;

    for (int col : getValidMoves(state)) {
        std::string tempState = state;                 // copy
        dropPieceSim(tempState, col, AI_PLAYER + 1);  // simulate AI move
        int score = -negamax(tempState, _maxDepth - 1, HUMAN_PLAYER + 1);

        if (score > bestScore) {
            bestScore = score;
            bestCol = col;
        }
    }

    // Place on the real board just once
    if (bestCol != -1) {
        for (int row = ROWS - 1; row >= 0; row--) {
            ChessSquare* square = _grid->getSquare(bestCol, row);
            if (!square->bit()) {
                actionForEmptyHolder(*square); // only one piece
                break;
            }
        }
    }
}



// void Connect_4::updateAI()
// {

//         if (_gameOver)
//         return;
//     BitHolder* bestMove = nullptr;
//     std::string state = stateString();

//     // pick first valid column (placeholder logic)
//     for (int col = 1; col < COLS && !bestMove; col++) {
//         auto square = _grid->getSquare(col, 0);
//         if (!square->bit()) {
//             bestMove = square;
//         }
//     }

//     if (bestMove) {
//         actionForEmptyHolder(*bestMove);
//     }
// }
