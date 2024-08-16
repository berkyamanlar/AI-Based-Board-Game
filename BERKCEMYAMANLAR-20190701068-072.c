#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#define BOARD_SIZE 7
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


typedef struct {
    char fromRow;
    int fromCol;
    char toRow;
    int toCol;
} Move;

typedef struct TreeNode {
    char board[BOARD_SIZE][BOARD_SIZE];
    struct TreeNode **children;  // Pointer to an array of TreeNode pointers
    int childrenCount;           // To keep track of the number of children
    Move move;
    int score;
} TreeNode;

// Function to create a new TreeNode
TreeNode* createTreeNode(char board[BOARD_SIZE][BOARD_SIZE], int score, Move move) {
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            node->board[i][j] = board[i][j];
        }
    }
    node->children = NULL;  // Initially, the node has no children
    node->childrenCount = 0;  // No children yet
    node->score = score;
    node->move = move;
    return node;
}

// Function to add a child to a TreeNode
void addChildNode(TreeNode *parent, TreeNode *child) {
    // Resize the children array to accommodate the new child
    parent->children = realloc(parent->children, sizeof(TreeNode*) * (parent->childrenCount + 1));
    parent->children[parent->childrenCount] = child;
    parent->childrenCount++;
}

// Function prototypes
void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE], int* playerPieces, int numPieces, int placementOption);
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]);
bool isValidMove(char board[BOARD_SIZE][BOARD_SIZE], int player, char fromRow, int fromCol, char toRow, int toCol); //Checks if a move is valid
bool makeMove(char board[BOARD_SIZE][BOARD_SIZE], int player, char fromRow, int fromCol, char toRow, int toCol, char piece);
bool hasValidMove(char board[BOARD_SIZE][BOARD_SIZE], int player); //Checks if a player has a valid move
void listUniqueValidMoves(char board[BOARD_SIZE][BOARD_SIZE], int player, char* moves, int* moveCount); //Prints all unique moves for both player
void userMove(char board[BOARD_SIZE][BOARD_SIZE], int userPlayer);

int evaluateBoard(char board[BOARD_SIZE][BOARD_SIZE], int player); //Evaluation Function
int countPlayerMoves(char board[BOARD_SIZE][BOARD_SIZE], int row, int col); //Returns how many moves a player has
void generateMoves(TreeNode *root, char board[BOARD_SIZE][BOARD_SIZE], int currentDepth, int maxDepth, int currentPlayer); //Determines all possible moves that can be made from that particular game state.
Move minimax(TreeNode *root, int currentDepth, int maxDepth, int computerPlayer, int currentPlayer);
void computerMove(char board[BOARD_SIZE][BOARD_SIZE], int computerPlayer, int remainingTurnLimitPerPlayer);
Move findBestMove(TreeNode *root);

int main() {
    char board[BOARD_SIZE][BOARD_SIZE];
    int playerPieces[2];
    int turnLimitPerPlayer; //Turn limit
    int currentPlayer;
    int placementOption;
    int playerTurns[2] = {0, 0}; //Track turns for each player

    // Seed for random number generation
    srand(time(NULL));

    //Get input from the user for number of pieces
    printf("Enter the number of pieces for each player: ");
    scanf("%d", &playerPieces[0]);

    //Validation of number of pieces
    if (playerPieces[0] <= 0) {
        printf("Invalid number of pieces. Exiting the game.\n");
        return 1;
    }

    playerPieces[1] = playerPieces[0]; // Both players have the same number of pieces

    printf("Enter the turn limit per player: ");
    scanf("%d", &turnLimitPerPlayer);
    //Validation of turn limit
    if (turnLimitPerPlayer <= 0) {
        printf("Invalid number of turn limit. Exiting the game.\n");
        return 1;
    }

    //Get placement option from the user
    printf("Do you want to add pieces randomly (1) or by typing coordinates (2)? ");
    scanf("%d", &placementOption);

    //Validation of placement option
    if (placementOption != 1 && placementOption != 2) {
        printf("Invalid choice. Exiting the game.\n");
        return 1;
    }

    //Initialize the board based on the chosen placement option
    initializeBoard(board, playerPieces, playerPieces[0], placementOption);

    printf("Choose your player 1(X) or 2(O): ");
    scanf("%d", &currentPlayer);

    // Validate player choice
    if (currentPlayer != 1 && currentPlayer != 2) {
        printf("Invalid choice. Exiting the game.\n");
        return 1;
    }
    int Player = currentPlayer;

    // Main game loop with updated turn tracking
    while (playerTurns[0] <= turnLimitPerPlayer && playerTurns[1] <= turnLimitPerPlayer) {


        // Print the current state of the board
        printBoard(board);

        // Check for a winner
        if (!hasValidMove(board, currentPlayer)) {
            printf("Player %d has no valid moves. Player %d wins!\n", currentPlayer, 3 - currentPlayer);
            break;
        }

        // Execute player's move based on currentPlayer
        if (currentPlayer == 1) {
            userMove(board, Player);
            playerTurns[0]++;
        } else {
            computerMove(board, 3-Player, turnLimitPerPlayer-playerTurns[0]);
            playerTurns[1]++;
        }
        // Switch players for the next turn
        currentPlayer = 3 - currentPlayer;
    }

    printBoard(board);

    char movesP1[BOARD_SIZE * BOARD_SIZE * 3], movesP2[BOARD_SIZE * BOARD_SIZE * 3];
    int moveCountP1, moveCountP2;

    listUniqueValidMoves(board, 1, movesP1, &moveCountP1);
    listUniqueValidMoves(board, 2, movesP2, &moveCountP2);

    // Print the moveable spaces and counts for both players
    printf("\"X\" can move to \"%s\" and \"O\" can move to \"%s\".\n", movesP1, movesP2);
    printf("P1 moveable space count is %d and P2 moveable space count is %d.\n", moveCountP1, moveCountP2);

    // Determine the winner based on moveable space count
    if (moveCountP1 > moveCountP2) {
        printf("So Player1(X) wins.\n");
    } else if (moveCountP2 > moveCountP1) {
        printf("So Player2(O) wins.\n");
    } else {
        printf("The game is a draw.\n");
    }

    return 0;
}

//Function to initialize the board
void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE], int* playerPieces, int numPieces, int placementOption) {
    //Initialize the board with empty spaces
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = ' ';
        }
    }

    //If the user chooses to add pieces by typing coordinates
    if (placementOption == 2) {
        //Add pieces one by one, asking the user for each piece
        for (int player = 1; player <= 2; ++player) {
            for (int piece = 1; piece <= numPieces; ++piece) {
                char pieceType = (player == 1) ? 'X' : 'O';

                // Print the current state of the board before asking for placement
                printBoard(board);

                printf("Enter the coordinates for Piece %c-%d (e.g., a1): ", pieceType, piece);
                char row;
                int col;
                scanf(" %c%d", &row, &col);

                // Convert row character to index
                int rowIndex = row - 'a';

                // Convert column number to index
                int colIndex = col - 1;

                // Check if the chosen coordinates are valid
                if (rowIndex >= 0 && rowIndex < BOARD_SIZE && colIndex >= 0 && colIndex < BOARD_SIZE &&
                    board[rowIndex][colIndex] == ' ') {
                    // Place the piece on the board
                    board[rowIndex][colIndex] = pieceType;
                } else {
                    printf("Invalid coordinates. Try again.\n");
                    --piece;  // Decrement piece to repeat the current iteration
                }
            }
        }
        printBoard(board);
    } else {
        // If the user chooses to add pieces randomly
        // Place player 1 pieces randomly
        for (int i = 0; i < numPieces; ++i) {
            int row, col;
            do {
                row = rand() % BOARD_SIZE;
                col = rand() % BOARD_SIZE;
            } while (board[row][col] != ' ');

            board[row][col] = 'X';
        }

        // Place player 2 pieces randomly
        for (int i = 0; i < numPieces; ++i) {
            int row, col;
            do {
                row = rand() % BOARD_SIZE;
                col = rand() % BOARD_SIZE;
            } while (board[row][col] != ' ');

            board[row][col] = 'O';
        }
    }
}


// Function to print the current state of the board
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    // Print column headers
    printf("  ");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%d ", i + 1);
    }
    printf("\n");

    // Print the board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%c ", 'a' + i);
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Function to check if a move is valid
bool isValidMove(char board[BOARD_SIZE][BOARD_SIZE], int player, char fromRow, int fromCol, char toRow, int toCol) {
    // Check if coordinates are within bounds
    if (fromRow < 'a' || fromRow > 'g' || toRow < 'a' || toRow > 'g' || fromCol < 1 || fromCol > 7 || toCol < 1 || toCol > 7) {
        return false;
    }

    // Convert row character to index
    int fromRowIndex = fromRow - 'a';
    int toRowIndex = toRow - 'a';

    // Convert column number to index
    int fromColIndex = fromCol - 1;
    int toColIndex = toCol - 1;

    // Check if the move is within one square horizontally or vertically
    if ((abs(fromRowIndex - toRowIndex) == 1 && fromColIndex == toColIndex) ||
        (abs(fromColIndex - toColIndex) == 1 && fromRowIndex == toRowIndex)) {
        // Check if the destination is an empty space
        if (board[toRowIndex][toColIndex] == ' ') {
            // Check if the piece belongs to the current player
            if (board[fromRowIndex][fromColIndex] == (player == 1 ? 'X' : 'O')) {
                return true;
            }
        }
    }

    return false;
}

// Function to make a move on the board
bool makeMove(char board[BOARD_SIZE][BOARD_SIZE], int player, char fromRow, int fromCol, char toRow, int toCol, char piece) {
    if (isValidMove(board, player, fromRow, fromCol, toRow, toCol)) {
        // Convert row character to index
        int fromRowIndex = fromRow - 'a';
        int toRowIndex = toRow - 'a';

        // Convert column number to index
        int fromColIndex = fromCol - 1;
        int toColIndex = toCol - 1;

        // Move the piece
        board[toRowIndex][toColIndex] = board[fromRowIndex][fromColIndex];
        board[fromRowIndex][fromColIndex] = ' ';

        return true;
    }

    return false;
}

// Function to check if a player has a valid move
bool hasValidMove(char board[BOARD_SIZE][BOARD_SIZE], int player) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == (player == 1 ? 'X' : 'O')) {
                // Check for valid moves in all four directions
                if ((i > 0 && board[i - 1][j] == ' ') ||
                    (i < BOARD_SIZE - 1 && board[i + 1][j] == ' ') ||
                    (j > 0 && board[i][j - 1] == ' ') ||
                    (j < BOARD_SIZE - 1 && board[i][j + 1] == ' ')) {
                    return true;
                }
            }
        }
    }

    return false;
}

// Function to list unique valid moveable spaces for a player
void listUniqueValidMoves(char board[BOARD_SIZE][BOARD_SIZE], int player, char* moves, int* moveCount) {
    // Initialize all cells as not visited
    bool visited[BOARD_SIZE][BOARD_SIZE] = {{false}};

    *moveCount = 0;
    int index = 0; // Index for the moves string

    // Check each cell on the board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // If the cell contains the player's piece, check adjacent cells
            if (board[i][j] == (player == 1 ? 'X' : 'O')) {
                // Check the cell above
                if (i > 0 && board[i - 1][j] == ' ' && !visited[i - 1][j]) {
                    moves[index++] = 'a' + (i - 1);
                    moves[index++] = '1' + j;
                    moves[index++] = ',';
                    visited[i - 1][j] = true;
                    (*moveCount)++;
                }
                // Check the cell below
                if (i < BOARD_SIZE - 1 && board[i + 1][j] == ' ' && !visited[i + 1][j]) {
                    moves[index++] = 'a' + (i + 1);
                    moves[index++] = '1' + j;
                    moves[index++] = ',';
                    visited[i + 1][j] = true;
                    (*moveCount)++;
                }
                // Check the cell to the left
                if (j > 0 && board[i][j - 1] == ' ' && !visited[i][j - 1]) {
                    moves[index++] = 'a' + i;
                    moves[index++] = '1' + (j - 1);
                    moves[index++] = ',';
                    visited[i][j - 1] = true;
                    (*moveCount)++;
                }
                // Check the cell to the right
                if (j < BOARD_SIZE - 1 && board[i][j + 1] == ' ' && !visited[i][j + 1]) {
                    moves[index++] = 'a' + i;
                    moves[index++] = '1' + (j + 1);
                    moves[index++] = ',';
                    visited[i][j + 1] = true;
                    (*moveCount)++;
                }
            }
        }
    }

    // Terminate the moves string properly
    if (index > 0) {
        moves[index - 1] = '\0'; // Replace the last comma with a null terminator
    } else {
        moves[0] = '\0'; // If no moves, end the string immediately
    }
}

// Function for the user's move
void userMove(char board[BOARD_SIZE][BOARD_SIZE], int userPlayer) {
    char fromRow, toRow;
    int fromCol, toCol;
    char userPiece = (userPlayer == 1) ? 'X' : 'O'; // Correctly determine the user's piece

    // Get the move from the user
    printf("Choose piece to move: ");
    scanf(" %c%d", &fromRow, &fromCol);
    printf("Choose the new position for %c%d: ", fromRow, fromCol);
    scanf(" %c%d", &toRow, &toCol);

    if (makeMove(board, userPlayer, fromRow, fromCol, toRow, toCol, userPiece)) {
        printf("Player moves the piece at %c%d to %c%d\n", fromRow, fromCol, toRow, toCol);
    } else {
        printf("Invalid move. Try again.\n");
        userMove(board, userPlayer);
    }
}

int evaluateBoard(char board[BOARD_SIZE][BOARD_SIZE], int computerPlayer) {
    int score = 0;
    int opponent = (computerPlayer == 1) ? 'O' : 'X'; // Assuming 'X' and 'O' are players
    int playerMoveCount, opponentMoveCount;

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == computerPlayer) {
                playerMoveCount = countPlayerMoves(board, row, col);
                score += playerMoveCount * 0.6; // Weight for maximizing computer's moves
            } else if (board[row][col] == opponent) {
                opponentMoveCount = countPlayerMoves(board, row, col);
                score -= opponentMoveCount * 0.4; // Weight for minimizing opponent's moves
            }
        }
    }
    return score;
}

int countPlayerMoves(char board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
    int moveCount = 0;

    // Check the cell above
    if (row > 0 && board[row - 1][col] == ' ') {
        moveCount++;
    }
    // Check the cell below
    if (row < BOARD_SIZE - 1 && board[row + 1][col] == ' ') {
        moveCount++;
    }
    // Check the cell to the left
    if (col > 0 && board[row][col - 1] == ' ') {
        moveCount++;
    }
    // Check the cell to the right
    if (col < BOARD_SIZE - 1 && board[row][col + 1] == ' ') {
        moveCount++;
    }

    return moveCount;
}

// Recursive function to generate moves and build the game tree
void generateMoves(TreeNode *root, char board[BOARD_SIZE][BOARD_SIZE], int currentDepth, int maxDepth, int currentPlayer) {
    if (currentDepth == maxDepth || !hasValidMove(board, currentPlayer)) {
        return; // Stop recursion at max depth or if no valid moves
    }

    char char_currentPlayer = (currentPlayer == 1 ? 'X' : 'O');
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == char_currentPlayer) {
                char fromRowChar = 'a' + row;
                char toRowChar;

                // Try moving in each direction
                if (row > 0 && board[row - 1][col] == ' ') {
                    toRowChar = 'a' + (row - 1);
                    char tempBoard[BOARD_SIZE][BOARD_SIZE];
                    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);
                    if (makeMove(tempBoard, currentPlayer, fromRowChar, col + 1, toRowChar, col + 1, char_currentPlayer)) {
                        Move new_move = {fromRowChar, col + 1, toRowChar, col + 1};
                        TreeNode *childNode = createTreeNode(tempBoard, evaluateBoard(tempBoard, currentPlayer), new_move);
                        addChildNode(root, childNode);
                        generateMoves(childNode, tempBoard, currentDepth + 1, maxDepth, 3 - currentPlayer);
                    }
                }
                // Repeat for moving down, left, and right
                if (row < BOARD_SIZE - 1 && board[row + 1][col] == ' ') {
                    toRowChar = 'a' + (row + 1);
                    char tempBoard[BOARD_SIZE][BOARD_SIZE];
                    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);
                    if (makeMove(tempBoard, currentPlayer, fromRowChar, col + 1, toRowChar, col + 1, char_currentPlayer)) {
                        Move new_move = {fromRowChar, col + 1, toRowChar, col + 1};
                        TreeNode *childNode = createTreeNode(tempBoard, evaluateBoard(tempBoard, currentPlayer), new_move);
                        addChildNode(root, childNode);
                        generateMoves(childNode, tempBoard, currentDepth + 1, maxDepth, 3 - currentPlayer);
                    }
                }
                if (col > 0 && board[row][col - 1] == ' ') {
                    toRowChar = fromRowChar;
                    char tempBoard[BOARD_SIZE][BOARD_SIZE];
                    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);
                    if (makeMove(tempBoard, currentPlayer, fromRowChar, col, toRowChar, col, char_currentPlayer)) {
                        Move new_move = {fromRowChar, col, toRowChar, col};
                        TreeNode *childNode = createTreeNode(tempBoard, evaluateBoard(tempBoard, currentPlayer), new_move);
                        addChildNode(root, childNode);
                        generateMoves(childNode, tempBoard, currentDepth + 1, maxDepth, 3 - currentPlayer);
                    }
                }
                if (col < BOARD_SIZE - 1 && board[row][col + 1] == ' ') {
                    toRowChar = fromRowChar;
                    char tempBoard[BOARD_SIZE][BOARD_SIZE];
                    memcpy(tempBoard, board, sizeof(char) * BOARD_SIZE * BOARD_SIZE);
                    if (makeMove(tempBoard, currentPlayer, fromRowChar, col + 2, toRowChar, col + 2, char_currentPlayer)) {
                        Move new_move = {fromRowChar, col + 2, toRowChar, col + 2};
                        TreeNode *childNode = createTreeNode(tempBoard, evaluateBoard(tempBoard, currentPlayer), new_move);
                        addChildNode(root, childNode);
                        generateMoves(childNode, tempBoard, currentDepth + 1, maxDepth, 3 - currentPlayer);
                    }
                }
            }
        }
    }
}


Move minimax(TreeNode *root, int currentDepth, int maxDepth, int computerPlayer, int currentPlayer) {

    generateMoves(root, root->board, currentDepth, maxDepth, currentPlayer);
    return findBestMove(root);

}

void computerMove(char board[BOARD_SIZE][BOARD_SIZE], int computerPlayer, int remainingTurnLimitPerPlayer) {
    int MAX_DEPTH = remainingTurnLimitPerPlayer*2; // or remainingTurnLimitPerPlayer*2 for maximum play game
    int currentDepth = 0;
    Move emptyMove = {-1, -1, -1, -1};
    TreeNode *root = createTreeNode(board, evaluateBoard(board, computerPlayer), emptyMove); // Generate tree data structure
    int opponentPlayer = 3 - computerPlayer;

    // Start minimax for the computer player
    Move bestMove = minimax(root, currentDepth, MAX_DEPTH, computerPlayer, computerPlayer);

    // Make the best move
    if (makeMove(board, computerPlayer, bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol, (computerPlayer == 1 ? 'X' : 'O'))) {
        printf("Computer moves the piece at %c%d to %c%d\n", bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol);
    }

}

// Recursive function to find the maximum score at the deepest level of each branch
int findMaxScoreAtDepth(TreeNode *node, int *maxDepth, int currentDepth) {
    if (node == NULL) return INT_MIN;

    // If it's a leaf node, return its score if it's at the maximum depth
    if (node->childrenCount == 0) {
        if (currentDepth > *maxDepth) {
            *maxDepth = currentDepth;
            return node->score;
        } else if (currentDepth == *maxDepth) {
            return node->score;
        }
        return INT_MIN;
    }

    int maxScore = INT_MIN;
    // Recursively find the maximum score in the children
    for (int i = 0; i < node->childrenCount; i++) {
        int score = findMaxScoreAtDepth(node->children[i], maxDepth, currentDepth + 1);
        maxScore = max(maxScore, score);
    }
    return maxScore;
}

// Function to find the best move
Move findBestMove(TreeNode *root) {
    int maxDepth = 0;
    int bestScore = findMaxScoreAtDepth(root, &maxDepth, 0);
    Move bestMove = {' ', 0, ' ', 0};

    for (int i = 0; i < root->childrenCount; i++) {
        int childDepth = maxDepth - 1;
        int score = findMaxScoreAtDepth(root->children[i], &childDepth, 1);
        if (score == bestScore) {
            bestMove = root->children[i]->move;
            break;
        }
    }

    return bestMove;
}






