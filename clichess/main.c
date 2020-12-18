#include <stdio.h>
#include <stdint.h>

typedef enum {
    EMPTY,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    NUM_PIECES,
} piece;

char piece_repr[NUM_PIECES] = {
    ' ', 'p', 'r', 'k', 'b', 'Q', 'K',
};

#define WHITE 128
#define BLACK 0

#define WHITE_SQUARE "43m"
#define BLACK_SQUARE "40m"

#define WHITE_PIECE "1;37m"
#define BLACK_PIECE "1;30m"

void put_piece(uint8_t piece, uint8_t square) {
    //printf("%c", piece_repr[piece & 0b01111111]);
    printf("\033[%s\033[%s %c \033[m",
        piece & WHITE ? WHITE_PIECE  : BLACK_PIECE,
        square        ? WHITE_SQUARE : BLACK_SQUARE,
        piece_repr[piece & 0b01111111]);
}

void put_board(uint8_t board[64]) {
    printf("\n");
    for (int i = 0; i < 64; i++) {
        int rank = 8 - (i / 8);
        if (i % 8 == 0) printf("%d  ", rank);
        put_piece(board[i], (i % 2 == 0) != (i / 8 % 2 == 0));
        if (i % 8 == 7 && i != 0) putchar('\n');
    }
    printf("\n   ");
    for (char c = 'a'; c <= 'h'; c++) {
        printf(" %c ", c);
    }
    printf("\n\n");
}

int main(int argc, char** argv) {
    uint8_t board[64] = {
        BLACK | ROOK, BLACK | KNIGHT, BLACK | BISHOP, BLACK | QUEEN, BLACK | KING, BLACK | BISHOP, BLACK | KNIGHT, BLACK | ROOK,
        BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN,
        WHITE | ROOK, WHITE | KNIGHT, WHITE | BISHOP, WHITE | QUEEN, WHITE | KING, WHITE | BISHOP, WHITE | KNIGHT, WHITE | ROOK,
    };
    put_board(board);
}
