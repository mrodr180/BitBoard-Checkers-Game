#include <math.h>
#include <stdio.h>

unsigned int setBits(unsigned int value, int position);
unsigned int setBitZero(unsigned int value, int position);
unsigned int toggleBit(unsigned int value, int position);
unsigned getBitValue(unsigned int value, int position);
int countBits(unsigned int value);
unsigned int shiftLeft(unsigned int value, int positions);
unsigned int shiftRight(unsigned int value, int positions);
void printBinary(unsigned int value);
void printHex(unsigned int value);

typedef struct {
    unsigned long long red_pieces;
    unsigned long long black_pieces;
    unsigned long long red_kings;
    unsigned long long black_kings;
    int current_turn;
} GameState;

const int boardSize = 8;

const unsigned long long validSpace = 0xAA55AA55AA55AA55ull;
const unsigned long long redKingRow = 0xFF00000000000000ull;
const unsigned long long blackKingRow = 0x00000000000000FFull;

void printBoard(const GameState *game) {
    printf("\n");
    for (int row = 7; row >= 0; row--) {
        printf("%d |", row);
        for (int col = 0; col < boardSize; col++) {
            int position = row * 8 + col;
            unsigned long long mask = 1ull << position;

            if (!(validSpace & mask)) {
                printf("  ");
                continue;
            }
            if (game->red_pieces & mask)
                printf(" r ");
            else if (game->black_pieces & mask)
                printf(" b ");
            else if (game->red_kings & mask)
                printf(" R ");
            else if (game->black_kings & mask)
                printf(" B ");
            else
                printf(" . ");
            }
        printf("\n");
        }
    printf("    0 1  2 3  4 5  6 7\n\n");
    }

void initializeGame(GameState *game) {
    game -> red_pieces = 0x000000000055AA55ull;
    game -> black_pieces = 0xAA55AA0000000000ull;
    game -> red_kings &= validSpace ;
    game -> black_kings &= validSpace ;
    game->current_turn = 0;
    game->red_kings = 0ull;
    game->black_kings = 0ull;

    printf("BitBoard checkers game\n");
    printBoard(game);
}

int isOccupied(const GameState *game, int position) {
    unsigned long long all = game->red_pieces | game->black_pieces | game->red_kings | game->black_kings;
    return getBitValue(all, position);
}

void movePiece(GameState *game, int from, int to) {
    unsigned long long *pieces =
        (game->current_turn == 0) ? &game->red_pieces : &game->black_pieces;

    if (!((*pieces >> from) & 1ull)) {
        printf("Invalid move\n");
        return;
    }
    if (isOccupied(game, to)) {
        printf("Occupied\n");
    }

    *pieces |= (1ull << to);
    *pieces &= ~(1ull << from);

    printf("moved piece from %d to %d\n", from, to);
}

void switchTurn(GameState *game) {
    game->current_turn = 1 - game->current_turn;
}
int checkWin(const GameState *game) {
    unsigned long long allRed = game -> red_kings | game->red_pieces;
    unsigned long long blackKings = game->black_kings |  game->black_pieces;
    if (allRed == 0ull ) return 1;
    if (blackKings == 0) return 0;
    return -1;
}

int isValidMove(int from, int to) {
    int difference = abs(to - from);
    return difference == 7 || difference == 9;
}

int isValidJumpOver(int from, int to) {
    int difference = abs(to - from);
    return difference == 14 || difference == 18;
}

int tryMoveCapture(GameState *game, int from, int to) {
    unsigned long long fromMask = (1ull << from);
    unsigned long long toMask = (1ull << to);

    if (isOccupied(game, to) || !getBitValue(validSpace, to)) {
        printf("Invalid move\n");
        return 0;
    }
    int isRedTurn = (game->current_turn == 0);
    int isKingPiece = 0;
    unsigned long long *myPieces, *myKings, *oppPieces, *oppKings;

    // 2. Identify current player's pieces
    if (isRedTurn) {
        myPieces = &game->red_pieces;
        myKings = &game->red_kings;
        oppPieces = &game->black_pieces;
        oppKings = &game->black_kings;
        isKingPiece = getBitValue(game->red_kings, from);
    } else {
        myPieces = &game->black_pieces;
        myKings = &game->black_kings;
        oppPieces = &game->red_pieces;
        oppKings = &game->red_kings;
        isKingPiece = getBitValue(game->black_kings, from);
    }
    unsigned long long allOppPieces = *oppPieces | *oppKings;
    unsigned long long allMyPieces = *myKings | *myPieces;

    if (!getBitValue(allMyPieces, from)) {
        printf("Invalid move\n");
        return 0;
    }
    int rowFrom = from / 8;
    int rowTo = to / 8;
    int isJump = isValidJumpOver(from, to);
    int regJump = isValidMove(from, to);

    if (!isKingPiece) {
        if (isRedTurn && rowTo <= rowFrom) {
            printf("Invalid move\n");
            return 0;
        }
        if (!isRedTurn && rowTo >= rowFrom) {
            printf("Invalid move\n");
            return 0;
        }
    }
    if (regJump && !isJump) {
        if (regJump) {
            if (abs(from - to) != 7 && abs(from - to) != 9) {
                printf("Invalid move\n");
                return 0;
            }
            printf("move \n"); //need to come back to change
        }
        else {
            printf("Invalid move\n");
            return 0;
        }
    }
    else if (isJump) {
        int posJump = (from + to) / 2;
        unsigned long long jumpMask = 1ull << posJump;
        if (!(allOppPieces & jumpMask)) {
            printf("Invalid move\n");
            return 0;
        }
        printf("move \n"); //change

        *oppPieces = setBitZero(*oppPieces, posJump);
        *oppKings = setBitZero(*oppKings, posJump);
    }
    else {
        printf("Invalid move\n");
        return 0;
    }
    *myPieces = setBitZero(*myPieces, from);
    *myKings = setBitZero(*myKings, from);
    if (isKingPiece) {
        *myKings = setBitZero(*myKings, to);
    }
    else {
        int promoted = 0;
        if (isRedTurn && (toMask & redKingRow)) {
            *myKings = setBitZero(*myKings, to);
            promoted = 1;
        }
        else if (isRedTurn && (fromMask & blackKingRow)) {
            *myKings = setBitZero(*myKings, from);
            promoted = 1;
        }
        else {
            *myPieces = setBitZero(*myPieces, to);
        }
        if (promoted) {
            printf("PIECE PROMOTED TO KING\n");
        }
    }
    game->current_turn = 1 - game->current_turn;
    return 1;

}

int main(void) {
    GameState game;
    initializeGame(&game);
    int from, to;
    int winner;
    while (1) {
        winner = checkWin(&game);
        if (winner != -1) {
            printf("GAME OVER\n");
            break;
        }
        printf("\n--- %s's Turn ---\n", game.current_turn == 0 ? "RED (r/R)" : "BLACK (b/B)");
        printf("Enter move (from to): ");

        if (scanf("%d", &from) != 1) {
            printf("PLEASE TRY AGAIN\n");
        }
        if (from ==-1) {
            printf("exiting game");
            break;
        }
        if (tryMoveCapture(&game, from, to)) {
            printBoard(&game);
        }
        else {
            printf("TRY AGAIN\n");
        }
    }
    return 0;
}
