
#include "game.h" 
bool placePiece(GameState& game, int row, int col)
{
    if (game.winner != EMPTY)
        return false;
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
        return false;
    if (game.board[row][col] != EMPTY)
        return false;

    game.moves.push_back(Move{row, col, game.currentPlayer});
    game.board[row][col] = game.currentPlayer;
    if (checkWin(game, row, col))
    {
        game.winner = game.currentPlayer;
        return true;
    }
    game.currentPlayer = game.currentPlayer == BLACK_PIECE ? WHITE_PIECE : BLACK_PIECE;
    return true;
}

bool undoLastMove(GameState& game)
{
    if (game.moves.empty())
        return false;
    const Move move = game.moves.back();
    game.board[move.row][move.col] = EMPTY;
    game.moves.pop_back();
    game.currentPlayer = move.piece;
    game.winner = EMPTY;
    return true;
}

bool checkWin(const GameState& game, int row, int col) 
{ 
    if (row < 0 || row >= BOARD_SIZE || 
        col < 0 || col >= BOARD_SIZE) 
        return false; 

    const PieceColor piece = game.board[row][col]; 
    if (piece == EMPTY) 
        return false; 

    const int directions[4][2] = { 
        {0, 1}, 
        {1, 0}, 
        {1, 1}, 
        {1, -1} 
    };

    for (int i = 0; i < 4; i++) 
    { 
        int count = 1; 

        
        for (int sign = -1; sign <= 1; sign += 2) 
        { 
            const int dr = directions[i][0] * sign; 
            const int dc = directions[i][1] * sign; 
            int r = row + dr; 
            int c = col + dc; 

            while (r >= 0 && r < BOARD_SIZE && 
                   c >= 0 && c < BOARD_SIZE && 
                   game.board[r][c] == piece) 
            { 
                count++; 
                r += dr; 
                c += dc; 
            }
        }

        if (count >= 5) 
            return true; 
    }

    return false; 
}
