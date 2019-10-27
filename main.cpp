//
//  main.cpp
//  AI Checkers Project
//
//  Created by Andrew Lorber on 10/7/19.
//  Copyright © 2019 Andrew Lorber. All rights reserved.
//

#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <time.h>

using namespace std;

// Board Key:
// 0 = empty space, 1 = player 1, 2 = player 2, 3 = player 1 king, 4 = player 2 king
int currentBoard[8][8] = {0};

// Initializes new game board
void initStartBoard(){
    int player = 2;
    for(int i = 0, j = 0; j < 8; i++){
        if ((i+j+1)%2 == 0){
            currentBoard[i][j] = player;
        }
        
        if (i == 7){
            i = -1;
            if(j == 2){
                j = 5;
                player = 1;
            } else {
                j++;
            }
        }
    }
}

// Initializes user-set game board
void initUserBoard(){
    // Reads in file with game board. See board key above.
    string file;
    cout << "Please enter the name of your board file.\nEach space should be either:\n  0 - empty space\n";
    cout << "  1 - player 1 piece\n  2 - player 2 piece\n  3 - player 1 king\n  4 - player 2 king\n";
    cin >> file;
    ifstream fin;
    fin.open(file);
    
    // Checks for incorrect input
    while(!fin.is_open()){
        cout << "The file you entered cannot be opened.\nPlease enter a new file.\n";
        cin >> file;
        fin.open(file);
    }
    
    int square;
    
    int i = 0; int j = 0;
    while(fin >> square){
        currentBoard[i][j] = square;
        i++;
        if(i == 8){
            i = 0;
            j++;
        }
        if(j == 8){
            break;
        }
    }
}

// Coordinate class to simplify things
class Coordinate {
public:
    int x, y;
    Coordinate(int i, int j) : x{i}, y{j} {}
};

// Vector of Legal Moves
vector<vector<Coordinate>> movesList;
// Vector of moves that include a jump
vector<vector<Coordinate>*> jumpsList;

// Prevents jumping same spot twice
bool legalJump(int jumpedI, int jumpedJ, const vector<Coordinate> & move){
    
    for (int i = 0; i < move.size()-1; i++) {
        if(((abs(move.at(i).x + move.at(i+1).x)) / 2) == jumpedI
           && ((abs(move.at(i).y + move.at(i+1).y)) / 2) == jumpedJ){
            return false;
        }
    }
    return true;
}

// Handles jumping
void jump(vector<Coordinate>& move, int i, int j, int player, int otherPlayer, bool king, int board[8][8],
           vector<vector<Coordinate>>& moves = movesList, vector<vector<Coordinate>*>& jumps = jumpsList){
    // Keeps track of multiple possible double jumps (i.e need to add another move to the list)
    bool multiOptions = false;
    vector<Coordinate> moveCopy = move;
    
    // If you end a jump in a space to become a king, your jump ends
    if (((j == 0 && player == 1) || (j == 7 && player == 2)) && !king) {
        return;
    }
    
    // NW Corner
    if(i > 1 && j > 1 && (player == 1 || king)){
        if((board[i-1][j-1] == otherPlayer || board[i-1][j-1] == otherPlayer+2)
           && (board[i-2][j-2] == 0 || (i-2 == move.at(0).x && j-2 == move.at(0).y))
           && legalJump(i-1, j-1, moveCopy)){
            move.push_back(Coordinate(i-2, j-2));
            multiOptions = true;
            jump(move, i-2, j-2, player, otherPlayer, king, board, moves, jumps);
        }
    }
    
    // NE Corner
    if(i < 6 && j > 1 && (player == 1 || king)){
        if((board[i+1][j-1] == otherPlayer || board[i+1][j-1] == otherPlayer+2)
           && (board[i+2][j-2] == 0 || (i+2 == move.at(0).x && j-2 == move.at(0).y))
           && legalJump(i+1, j-1, moveCopy)){
            if (multiOptions) {
                moves.push_back(moveCopy);
                jumps.push_back(&moves.at(moves.size()-1));
                (moves.at(moves.size()-1)).push_back(Coordinate(i+2, j-2));
                jump(moves.at(moves.size()-1), i+2, j-2, player, otherPlayer, king, board, moves, jumps);
            } else {
                move.push_back(Coordinate(i+2, j-2));
                multiOptions = true;
                jump(move, i+2, j-2, player, otherPlayer, king, board, moves, jumps);
            }
        }
    }
    
    // SW Corner
    if(i > 1 && j < 6 && (player == 2 || king)){
        if((board[i-1][j+1] == otherPlayer || board[i-1][j+1] == otherPlayer+2)
           && (board[i-2][j+2] == 0 || (i-2 == move.at(0).x && j+2 == move.at(0).y))
           && legalJump(i-1, j+1, moveCopy)){
            if (multiOptions) {
                moves.push_back(moveCopy);
                jumps.push_back(&moves.at(moves.size()-1));
                (moves.at(moves.size()-1)).push_back(Coordinate(i-2, j+2));
                jump(moves.at(moves.size()-1), i-2, j+2, player, otherPlayer, king, board, moves, jumps);
            } else {
                move.push_back(Coordinate(i-2, j+2));
                multiOptions = true;
                jump(move, i-2, j+2, player, otherPlayer, king, board, moves, jumps);
            }
        }
    }
    
    // SE Corner
    if(i < 6 && j < 6 && (player == 2 || king)){
        if((board[i+1][j+1] == otherPlayer || board[i+1][j+1] == otherPlayer+2)
           && (board[i+2][j+2] == 0 || (i+2 == move.at(0).x && j+2 == move.at(0).y))
           && legalJump(i+1, j+1, moveCopy)){
            if (multiOptions) {
                moves.push_back(moveCopy);
                jumps.push_back(&moves.at(moves.size()-1));
                (moves.at(moves.size()-1)).push_back(Coordinate(i+2, j+2));
                jump(moves.at(moves.size()-1), i+2, j+2, player, otherPlayer, king, board, moves, jumps);
            } else {
                move.push_back(Coordinate(i+2, j+2));
                multiOptions = true;
                jump(move, i+2, j+2, player, otherPlayer, king, board, moves, jumps);
            }
        }
    }
}

//Finds all legal moves for player
void getLegalMoves(int player, int board[8][8] = currentBoard, vector<vector<Coordinate>>& moves = movesList,
                   vector<vector<Coordinate>*>& jumps = jumpsList){

    moves.clear();
    jumps.clear();
    
    int otherPlayer;
    bool king;
    bool jumped = false; // If there is a jump that could be made,
                        // then no need to check for non-jump moves anymore
    
    if(player == 1){
        otherPlayer = 2;
    } else {
        otherPlayer = 1;
    }
    
    for(int i = 0, j = 0; j < 8; i++){
        if(board[i][j] == player || board[i][j] == player +2){
            
            king = board[i][j] > 2;
            
            // Checks North corners
            if((player == 1 || king) && j != 0){ // If on top row, then no North corners
    
                // check NW
                if(i != 0){
                    if(!jumped && board[i-1][j-1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i-1, j-1)});
                    } else if((board[i-1][j-1] == otherPlayer || board[i-1][j-1] == otherPlayer + 2)
                              && i != 1 && j != 1){
                        if(board[i-2][j-2] == 0){
                            // Handle jump
                            jumped = true;
                            moves.push_back({Coordinate(i, j), Coordinate(i-2, j-2)});
                            jumps.push_back(&moves.at(moves.size()-1));
                            jump(moves.at(moves.size()-1), i-2, j-2, player, otherPlayer, king, board, moves, jumps);
                        }
                    }
                }
                
                // check NE
                if(i != 7){
                    if(!jumped && board[i+1][j-1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i+1, j-1)});
                    } else if((board[i+1][j-1] == otherPlayer ||board[i+1][j-1] == otherPlayer + 2)
                              && i != 6 && j != 1){
                        if(board[i+2][j-2] == 0){
                            // Handle jump
                            jumped = true;
                            moves.push_back({Coordinate(i, j), Coordinate(i+2, j-2)});
                            jumps.push_back(&(moves.at(moves.size()-1)));
                            jump(moves.at(moves.size()-1), i+2, j-2, player, otherPlayer, king, board, moves, jumps);
                        }
                    }
                }
            }
            
            // Checks South Corners
            if((player == 2 || king) && j != 7){
                // check SW
                if(i != 0){
                    if(!jumped && board[i-1][j+1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i-1, j+1)});
                    } else if((board[i-1][j+1] == otherPlayer ||board[i-1][j+1] == otherPlayer + 2)
                              && i != 1 && j != 6){
                        if(board[i-2][j+2] == 0){
                            // Handle jump
                            jumped = true;
                            moves.push_back({Coordinate(i, j), Coordinate(i-2, j+2)});
                            jumps.push_back(&moves.at(moves.size()-1));
                            jump(moves.at(moves.size()-1), i-2, j+2, player, otherPlayer, king, board, moves, jumps);
                        }
                    }
                }
                
                // check SE
                if(i != 7){
                    if(!jumped && board[i+1][j+1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i+1, j+1)});
                    } else if((board[i+1][j+1] == otherPlayer ||board[i+1][j+1] == otherPlayer + 2)
                              && i != 6 && j != 6){
                        if(board[i+2][j+2] == 0){
                            // Handle jump
                            jumped = true;
                            moves.push_back({Coordinate(i, j), Coordinate(i+2, j+2)});
                            jumps.push_back(&moves.at(moves.size()-1));
                            jump(moves.at(moves.size()-1), i+2, j+2, player, otherPlayer, king, board, moves, jumps);
                        }
                    }
                }
            }
        }
        
        if(i == 7){
            i = -1;
            j++;
        }
    }
}

// Displays all legal moves (If jump are possible, will only display jumps)
void printMoves(){
    if(jumpsList.size() > 0){
        for(int i = 0; i < jumpsList.size(); i++){
            cout << i+1 << ".";
            
            for(int j = 0; j < jumpsList.at(i)->size(); j++){
                cout << " (" << jumpsList.at(i)->at(j).x << ", " << jumpsList.at(i)->at(j).y << ")";
            }
            cout << '\n';
        }
    } else {
        for(int i = 0; i < movesList.size(); i++){
            cout << i+1 << ".";
            
            for(int j = 0; j < movesList.at(i).size(); j++){
                cout << " (" << movesList.at(i).at(j).x << ", " << movesList.at(i).at(j).y << ")";
            }
            cout << '\n';
        }
    }
}

void printBoard(){
    cout << "              X - axis" << '\n';
    cout << "         |-0-|-1-|-2-|-3-|-4-|-5-|-6-|-7-|" << '\n';
    cout << "  Y";
    cout << '\n' << "|---|    |---|---|---|---|---|---|---|---|" << '\n';
    for(int i = 0, j = 0; j < 8; i++){
        if(i == 0){
            cout << "|-" << j << "-|   ";
        }
        cout << " | " << currentBoard[i][j];
        if(i == 7){
            cout << " | " << '\n' << "|---|    |---|---|---|---|---|---|---|---|" << '\n';
            i = -1;
            j++;
        }
    }
    cout << '\n';
}

// Takes the choice of move from legal move list and implements move
void ImplementMove(int moveNum, int board[8][8] = currentBoard, vector<vector<Coordinate>>& moves = movesList,
                   vector<vector<Coordinate>*>& jumps = jumpsList){
    moveNum--;
    bool jump = jumps.size() > 0;
    int piece;
    if(!jump){
        piece = board[moves.at(moveNum).at(0).x][moves.at(moveNum).at(0).y];
        // Makes piece king when needed
        if ((moves.at(moveNum).at(1).y == 0 && piece == 1) || (moves.at(moveNum).at(1).y == 7 && piece == 2)) {
            board[moves.at(moveNum).at(1).x][moves.at(moveNum).at(1).y] = piece + 2;
        } else {
            board[moves.at(moveNum).at(1).x][moves.at(moveNum).at(1).y] = piece;
        }
        board[moves.at(moveNum).at(0).x][moves.at(moveNum).at(0).y] = 0;
    } else {
        piece = board[jumps.at(moveNum)->at(0).x][jumps.at(moveNum)->at(0).y];
        int jumpedX;
        int jumpedY;
        // Clears starting square
        board[jumps.at(moveNum)->at(0).x][jumps.at(moveNum)->at(0).y] = 0;
        for (int i = 0; i < jumps.at(moveNum)->size(); i++) {
            if(i == jumps.at(moveNum)->size() - 1){
                // Makes piece king when needed
                if ((jumps.at(moveNum)->at(i).y == 0 && piece == 1) || (jumps.at(moveNum)->at(i).y == 7 && piece == 2)){
                    board[jumps.at(moveNum)->at(i).x][jumps.at(moveNum)->at(i).y] = piece + 2;
                } else {
                    board[jumps.at(moveNum)->at(i).x][jumps.at(moveNum)->at(i).y] = piece;
                }
            } else {
                // Gets coordinate of jumped square and clears it
                jumpedX = (abs(jumps.at(moveNum)->at(i).x + jumps.at(moveNum)->at(i+1).x)) / 2;
                jumpedY = (abs(jumps.at(moveNum)->at(i).y + jumps.at(moveNum)->at(i+1).y)) / 2;
                board[jumpedX][jumpedY] = 0;
            }
        }
    }
}

// Asks user for starting player
bool player1Starts(bool userPlaying){
    string resp;
    if(userPlaying){
        cout << "Would you like to go first? (Y/N) \n";
    } else {
        cout << "Would you like player 1 to go first? (Y/N) \n";
    }
    
    cin >> resp;
    while(resp.compare("Y") != 0 && resp.compare("N") != 0){
        cout << "That was an invalid response. Please respond with 'Y' or 'N'." << '\n';
        cin >> resp;
    }
    if(resp.compare("Y") == 0){
        return true;
    } else {
        return false;
    }
}

// Tests if a player has won
bool gameOver(int board[8][8] = currentBoard){
    bool player1Wins = true;
    bool player2Wins = true;
    
    for(int i = 0, j = 0; j < 8; i++){
        if(player2Wins && (board[i][j] == 1 || board[i][j] == 3)){
            player2Wins = false;
        } else if(player1Wins && (board[i][j] == 2 || board[i][j] == 4)){
            player1Wins = false;
        }
        
        if (!player1Wins && !player2Wins) {
            return false;
        }
        
        if(i == 7){
            i = -1;
            j++;
        }
    }
    
    if (player1Wins) {
        return true;
    } else {
        return true;
    }
}

void copyBoard(int from[8][8], int to[8][8]){
    for(int i = 0, j = 0; j < 8; i++){
        
        to[i][j] = from[i][j];
        
        if(i == 7){
            i = -1;
            j++;
        }
    }
}

// Simple evaluation functions based on the pieces left
int evalFunc(int board[8][8]){
    int val = 0;
    for(int i = 0, j = 0; j < 8; i++){
        // Top or Bottom Rows
        if(j == 0 && board[i][j] == 2){
            val+=30;
        } else if(j == 7 && board[i][j] == 1){
            val-=30;
        }
        // Pawns
        if(board[i][j] == 1){
            val-=60;
        } else if(board[i][j] == 2){
            val+=60;
        } else if(board[i][j] == 3){ // Kings
            val-=100;
        } else if(board[i][j] == 4){
            val += 100;
        }
        
        if(i == 7){
            i = -1;
            j++;
        }
    }
    // Random number added to make it randomly choose between multiple "equal" moves
    val += rand() % 10;
    return val;
}

// Global variable to keep track of time limit
bool timeLimitPassed = false;

// Alpha Beta Search
int alphaBeta(int board[8][8], int depth, int alpha, int beta, bool maxPlayer, time_t endTime, bool root = false){
    
    // Checks if time limit has run out
    if(timeLimitPassed || time(nullptr) >= endTime){
        timeLimitPassed = true;
        return 0;
    }
    
    // Runs evaluation function at max depth or end-game board
    if(depth == 0 || gameOver(board)){
        return evalFunc(board);
    }
    
    int boardCopy[8][8] = {0};
    vector<vector<Coordinate>> nodeMoves;
    vector<vector<Coordinate>*> nodeJumps;
    nodeMoves.reserve(200);
    int value;
    int tmpValue;
    int player = maxPlayer ? 2 : 1;
    int bestMove = 1;
    
    getLegalMoves(player, board, nodeMoves, nodeJumps);
    int moveAmt = nodeJumps.size() > 0 ? nodeJumps.size() : nodeMoves.size();
    
    if(maxPlayer){
        value = -9000; /*Make -Infinity*/
        for(int i = 1; i <= moveAmt; i++){
            copyBoard(board, boardCopy);
            ImplementMove(i, boardCopy, nodeMoves, nodeJumps);
            
            tmpValue = alphaBeta(boardCopy, depth-1, alpha, beta, false, endTime);
            if(tmpValue > value){
                value = tmpValue;
                
                if(root){
                    bestMove = i;
                    //cout << "The best move is move #" << bestMove << '\n';
                }
            }
            
            alpha = max(alpha, value);
            if(alpha >= beta){
                break;
            }
        }
        if(root){
            return bestMove;
        } else {
           return value;
        }
    } else {
        value = 9000; /*Make Infinity*/
        for(int i = 1; i <= moveAmt; i++){
            copyBoard(board, boardCopy);
            ImplementMove(i, boardCopy, nodeMoves, nodeJumps);
            
            tmpValue = alphaBeta(boardCopy, depth-1, alpha, beta, true, endTime);
            if(tmpValue < value){
                value = tmpValue;
                
                if(root){
                    bestMove = i;
                    //cout << "The best move is move #" << bestMove << '\n';
                }
            }
            
            beta = min(beta, value);
            if(alpha >= beta){
                break;
            }
        }
        if(root){
            return bestMove;
        } else {
            return value;
        }
    }
}

// Implements Iterative Deepening for the Alpha Beta Search
int iterativeDeepening(int seconds, bool player2 = true){
    timeLimitPassed = false;
    int bestMove = 1;
    int move;
    time_t startTime = time(nullptr);
    time_t endTime = startTime + seconds;
    int depth = 1;
    
    // If there is only one move, do it
    if(movesList.size() == 1 || jumpsList.size() == 1){
        cout << "Depth: 0\nTime Searching: " << time(nullptr) - startTime << '\n';
        return 1;
    }
    
    // If there is less than half the time left, then it won't finish the next iteration, so stop
    while((time(nullptr) + (seconds/2)) <= endTime){
        move = alphaBeta(currentBoard, depth, -9000, 9000, player2, endTime, true);
        if(!timeLimitPassed){
            bestMove = move;
            depth++;
        }
    }
    cout << "Depth: " << depth << "\nTime Searching: " << time(nullptr) - startTime << '\n';
    return bestMove;
}

void playAIvsAI(int startingPlayer, int seconds){
    if(startingPlayer == 1){
        getLegalMoves(1);
        // If no moves then game ends
        if (movesList.size() == 0) {
            cout << "Player 1 has no possible moves. Player 2 wins.\n";
            return;
        }
        
        cout << "Player 1 is thinking...\n";
        ImplementMove(iterativeDeepening(floor(seconds), false));
        printBoard();
    }
    while(true){
        // Player 2 turn
        getLegalMoves(2);
        // If no moves then game ends
        if(movesList.size() == 0){
            cout << "Player 2 has no possible moves. Player 1 wins.\n";
            return;
        }
        
        cout << "Player 2 is thinking...\n";
        ImplementMove(iterativeDeepening(floor(seconds)));
        printBoard();
        
        // Player 1 turn
        getLegalMoves(1);
        // If no moves then game ends
        if(movesList.size() == 0) {
            cout << "Player 1 has no possible moves. Player 2 wins.\n";
            return;
        }
        cout << "Player 1 is thinking...\n";
        ImplementMove(iterativeDeepening(floor(seconds), false));
        printBoard();
    }
}


// Gets user's move choice
int getMoveChoice(){
    double moveChoice;
    int numChoices = jumpsList.size() > 0 ? jumpsList.size() : movesList.size();
    cout << "Which move would you like to do? \n";
    cin >> moveChoice;
    moveChoice = round(moveChoice);
    while(moveChoice < 1 || moveChoice > numChoices){
        cout << "That was an invalid choice.\nPlease give a number choice from the list of possible moves.\n";
        cin >> moveChoice;
        moveChoice = round(moveChoice);
    }
    return moveChoice;
}

void playGame(){
    // Reserves vector space so pointers don't get messed up when vector resizes
    movesList.reserve(200);
    
    // User decides game board
    double input;
    cout << "Welcome to checkers!\nPlease respond with the number of your choice:\n";
    cout << "1. Begin a new game\n2. Load a custom board\n";
    cin >> input;
    while(input != 1 && input != 2){
        cout << "That was an invalid choice. Please respond with 1 or 2.\n";
        cin >> input;
    }
    if(input == 1){
        initStartBoard();
    } else {
        initUserBoard();
    }
    
    // User Vs AI or AI Vs AI
    bool AIvsAI = false;
    cout << "Please choose a game mode:\n";
    cout << "1. User Vs AI\n2. AI Vs AI\n";
    cin >> input;
    while(input != 1 && input != 2){
        cout << "That was an invalid choice. Please respond with 1 or 2.\n";
        cin >> input;
    }
    AIvsAI = (input == 2);
    
    // User inputs time limit
    cout << "Please enter the number of seconds (integer value) that the AI has to move\n";
    cin >> input;
    while(floor(input) <= 1){
        cout << "Please enter an integer value greater than 1\n";
        cin >> input;
    }
    
    // Decides starting player
    int startingPlayer;
    if(player1Starts(!AIvsAI)){
        startingPlayer = 1;
    } else {
        startingPlayer = 2;
    }
    cout << "The starting player is player " << startingPlayer << ".\n\n";
    
    printBoard();
    // Checks if the board can be played
    if(gameOver()){
        return;
    }
    
    // Differs here if AI vs AI
    if(AIvsAI){
        playAIvsAI(startingPlayer, input);
        return;
    }
    
    if(startingPlayer == 1){
        getLegalMoves(1);
        // If no moves then game ends
        if (movesList.size() == 0) {
            cout << "You have no possible moves. The AI wins.\n";
            return;
        }
        printMoves();
        ImplementMove(getMoveChoice());
        printBoard();
    }
    while(true){
        // Player 2 turn
        getLegalMoves(2);
        // If no moves then game ends
        if(movesList.size() == 0){
            cout << "The AI is unable to move. You win!\n";
            return;
        }
        
        cout << "I am thinking...\n";
        ImplementMove(iterativeDeepening(floor(input)));
        printBoard();
        
        // Player 1 turn
        getLegalMoves(1);
        // If no moves then game ends
        if(movesList.size() == 0) {
            cout << "You have no possible moves. The AI wins.\n";
            return;
        }
        printMoves();
        ImplementMove(getMoveChoice());
        printBoard();
    }
    
}

int main(int argc, const char * argv[]) {
    srand(time(nullptr));
    playGame();
    return 0;
}
