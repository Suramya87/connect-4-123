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
    // bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "yellow.png" : "red.png");
    // bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    bit->LoadTextureFromFile(playerNumber == 0 ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber));
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
        setAIPlayer(_aiGoesFirst ? 0 : 1);
    }

    startGame();
}


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
            // Just use the current player's number directly
            Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
            
            bit->setPosition(square->getPosition());
            square->setBit(bit);

            if (checkForWinner() || checkForDraw()) {
                _gameOver = true;
                endTurn();
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

    // Diagonal Right 
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

    // Diagonal Left 
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

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            ChessSquare* sq = _grid->getSquare(x, y);
            if (sq && sq->bit()) {
                s[y * COLS + x] =
                    '0' + (sq->bit()->getOwner()->playerNumber() + 1);
            }
        }
    }
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
// Place a piece in a column
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


// Simulation only drop

void Connect_4::dropPieceSim(std::string &state, int column, int player) {
    for (int row = ROWS - 1; row >= 0; row--) {  
        int idx = row * COLS + column;
        if (state[idx] == '0') {
            state[idx] = '0' + player;
            break;  
        }
    }
}



void Connect_4::undoPiece(std::string &state, int column)
{
    for (int row = 0; row < ROWS; row++) {  
        int idx = row * COLS + column;
        if (state[idx] != '0') {
            state[idx] = '0';
            return;  
        }
    }
}

std::vector<int> Connect_4::getValidMoves(const std::string &state) const
{
    static const int order[] = {3, 2, 4, 1, 5, 0, 6};
    std::vector<int> moves;

    for (int col : order) {
        int topIndex = col; // row 0 * COLS + col
        if (state[topIndex] == '0')
            moves.push_back(col);
    }

    return moves;
}



void Connect_4::stateToBitboards(const std::string &state, uint64_t bb[2]) const
{
    bb[0] = 0;
    bb[1] = 0;
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            int idx = r * COLS + c;
            int pos = r + c * (ROWS + 1);  // Bitboard position
            
            if (state[idx] == '1') {  // Human
                bb[0] |= (1ULL << pos);
            } else if (state[idx] == '2') {  // AI
                bb[1] |= (1ULL << pos);
            }
        }
    }
}

bool Connect_4::checkWinBitboard(uint64_t board) const
{
    // Horizontal check
    uint64_t m = board & (board >> (ROWS + 1));
    if (m & (m >> (2 * (ROWS + 1)))) return true;
    
    // Vertical check
    m = board & (board >> 1);
    if (m & (m >> 2)) return true;
    
    // Diagonal left
    m = board & (board >> ROWS);
    if (m & (m >> (2 * ROWS))) return true;
    
    // Diagonal right
    m = board & (board >> (ROWS + 2));
    if (m & (m >> (2 * (ROWS + 2)))) return true;
    
    return false;
}

int Connect_4::evaluateBitboard(uint64_t myBoard, uint64_t oppBoard) const
{
    int score = 0;
    
    for (int r = 0; r < ROWS; r++) {
        int pos = r + 3 * (ROWS + 1);
        if (myBoard & (1ULL << pos)) score += 3;
        if (oppBoard & (1ULL << pos)) score -= 3;
    }
    
    uint64_t combined = myBoard | oppBoard;
    
    int myThreats = std::popcount(myBoard);
    int oppThreats = std::popcount(oppBoard);

    
    score += (myThreats - oppThreats) * 2;
    
    return score;
}



int Connect_4::negamax(std::string &state, int depth, int alpha, int beta, int player)
{
    int opponent = (player == AI_PLAYER + 1) ? HUMAN_PLAYER + 1 : AI_PLAYER + 1;
    
    // Convert to bitboards once
    uint64_t bb[2];
    stateToBitboards(state, bb);
    
    // player is 1 or 2, array indices are 0 or 1
    int myIdx = player - 1;
    int oppIdx = 1 - myIdx;

    // Check if opponent just won (they made the last move)
    if (checkWinBitboard(bb[oppIdx]))
        return -100000 + (_maxDepth - depth);
    
    std::vector<int> moves = getValidMoves(state);
    if (depth == 0 || moves.empty())
        return evaluateBitboard(bb[myIdx], bb[oppIdx]);

    int bestVal = -1000000;
    
    for (int col : moves) {
        dropPieceSim(state, col, player);
        int val = -negamax(state, depth - 1, -beta, -alpha, opponent);
        undoPiece(state, col);

        bestVal = std::max(bestVal, val);
        alpha = std::max(alpha, val);
        if (alpha >= beta) break;
    }

    return bestVal;
}


void Connect_4::updateAI()
{
    if (_gameOver) return;
    
    if (!getCurrentPlayer()->isAIPlayer()) {
        _debugInfo = "Not AI's turn yet. Current player: " + std::to_string(getCurrentPlayer()->playerNumber()) + "\n";
        return;
    }
    
    std::string state = stateString();
    
    _debugInfo = "=== AI TURN ===\n";
    _debugInfo += "AI_PLAYER: " + std::to_string(AI_PLAYER) + "\n";
    _debugInfo += "HUMAN_PLAYER: " + std::to_string(HUMAN_PLAYER) + "\n";
    _debugInfo += "Current player: " + std::to_string(getCurrentPlayer()->playerNumber()) + "\n";
    
    _debugInfo += "State:\n";
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            _debugInfo += state[r * COLS + c];
            _debugInfo += " ";
        }
        _debugInfo += "\n";
    }
    
    // Convert to bitboards for faster checks
    uint64_t bb[2];
    stateToBitboards(state, bb);
    
    bool aiWon = checkWinBitboard(bb[1]);  // AI is index 1
    bool humanWon = checkWinBitboard(bb[0]);  // Human is index 0
    _debugInfo += "AI won check: " + std::to_string(aiWon) + "\n";
    _debugInfo += "Human won check: " + std::to_string(humanWon) + "\n";
    
    int evalScore = evaluateBitboard(bb[1], bb[0]);  // AI vs Human
    _debugInfo += "Base eval score: " + std::to_string(evalScore) + "\n";
    
    std::vector<int> validMoves = getValidMoves(state);
    _debugInfo += "Valid moves: ";
    for (int col : validMoves) {
        _debugInfo += std::to_string(col) + " ";
    }
    _debugInfo += "\n\n";
    
    int bestCol = -1;
    int bestScore = -100000000;
    
    // Evaluate each move using negamax
    for (int col : validMoves) {
        std::string temp = state;
        
        _debugInfo += "--- Testing col " + std::to_string(col) + " ---\n";
        
        dropPieceSim(temp, col, AI_PLAYER + 1);
        
        _debugInfo += "After drop:\n";
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                _debugInfo += temp[r * COLS + c];
                _debugInfo += " ";
            }
            _debugInfo += "\n";
        }
        
        // Check for immediate win using bitboards
        uint64_t tempBB[2];
        stateToBitboards(temp, tempBB);
        
        if (checkWinBitboard(tempBB[1])) {  // AI wins
            _debugInfo += "THIS IS A WINNING MOVE!\n";
            bestCol = col;
            bestScore = 100000;
            break;
        }
        
        // Check if human can win on next turn
        for (int testCol : getValidMoves(temp)) {
            std::string test2 = temp;
            dropPieceSim(test2, testCol, HUMAN_PLAYER + 1);
            
            uint64_t test2BB[2];
            stateToBitboards(test2, test2BB);
            
            if (checkWinBitboard(test2BB[0])) {  // Human wins
                _debugInfo += "WARNING: Human can win at col " + std::to_string(testCol) + "\n";
            }
        }
        
        // Use negamax to evaluate this position
        int score = -negamax(
            temp,
            _maxDepth - 1,
            -100000000,
            100000000,
            HUMAN_PLAYER + 1
        );
        
        _debugInfo += "Negamax score: " + std::to_string(score) + "\n";
        
        // Also show simple eval for comparison
        int simpleEval = evaluateBitboard(tempBB[1], tempBB[0]);
        _debugInfo += "Simple eval: " + std::to_string(simpleEval) + "\n\n";
        
        if (score > bestScore) {
            bestScore = score;
            bestCol = col;
        }
    }
    
    _debugInfo += "=== DECISION ===\n";
    _debugInfo += "Best col: " + std::to_string(bestCol) + "\n";
    _debugInfo += "Best score: " + std::to_string(bestScore) + "\n";
    
    // Execute move
    if (bestCol != -1) {
        _debugInfo += "Executing move at column " + std::to_string(bestCol) + "\n";
        
        for (int row = ROWS - 1; row >= 0; row--) {
            ChessSquare* square = _grid->getSquare(bestCol, row);
            if (square && !square->bit()) {
                Bit* bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
                bit->setPosition(square->getPosition());
                square->setBit(bit);
                
                _debugInfo += "Piece placed at row " + std::to_string(row) + "\n";
                
                if (checkForWinner() || checkForDraw()) {
                    _gameOver = true;
                }
                
                endTurn();
                return;
            }
        }
        
        _debugInfo += "ERROR: Could not place piece!\n";
    }
}
