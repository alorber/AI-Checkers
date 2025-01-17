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
// Global variables for heuristic
int currentTotalPcs = 0;
int currentP1Pcs = 0;
int currentP2Pcs = 0;
int currentP1Kings = 0;
int currentP2Kings = 0;
// Keeps track of turns without jumps. After 80 (40 each player), a draw is called.
int turnCount = 0;

// Initializes new game board
void initStartBoard(){
    int player = 2;
    for(int i = 0, j = 0; j < 8; i++){
        if ((i+j+1)%2 == 0){
            currentBoard[j][i] = player;
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
    currentTotalPcs = 24;
    currentP1Pcs = 12;
    currentP2Pcs = 12;
}

// Initializes user-set game board
void initUserBoard(){
    // Reads in file with game board. See board key above.
    string file;
    cout << "Please enter the name of your board file.\nEach space should be either:\n  0 - empty space\n";
    cout << "  1 - player 1 piece\n  2 - player 2 piece\n  3 - player 1 king\n  4 - player 2 king\n";
    cout << "The squares of the board that are not used in checkers should be replaced with spaces.\n";
    cout << "Note: Player 1 starts on the bottom of the board and player 2 starts on the top of the board.\n";
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
    
    int i = 1; int j = 0;
    while(fin >> square){
        if ((i+j+1)%2 == 0){
            // If you input a board with a player in the opposite player's home row, it will become a king
            if(j == 0 && square == 1){
                square = 3;
            } else if(j == 7 && square == 2){
                square = 4;
            }
            
            currentBoard[j][i] = square;
            // Keeps track of # pcs
            if(square == 1 || square == 3){
                currentP1Pcs++;
                currentTotalPcs++;
                if(square == 3){
                    currentP1Kings++;
                }
            } else if(square == 2 || square == 4){
                currentP2Pcs++;
                currentTotalPcs++;
                if(square == 4){
                    currentP2Kings++;
                }
            }
        }
        do{
            i++;
            if(i == 8){
                i = 0;
                j++;
            }
            if(j == 8){
                break;
            }
        } while ((i+j+1)%2 != 0);
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
        if((board[j-1][i-1] == otherPlayer || board[j-1][i-1] == otherPlayer+2)
           && (board[j-2][i-2] == 0 || (i-2 == move.at(0).x && j-2 == move.at(0).y))
           && legalJump(i-1, j-1, moveCopy)){
            move.push_back(Coordinate(i-2, j-2));
            multiOptions = true;
            jump(move, i-2, j-2, player, otherPlayer, king, board, moves, jumps);
        }
    }
    
    // NE Corner
    if(i < 6 && j > 1 && (player == 1 || king)){
        if((board[j-1][i+1] == otherPlayer || board[j-1][i+1] == otherPlayer+2)
           && (board[j-2][i+2] == 0 || (i+2 == move.at(0).x && j-2 == move.at(0).y))
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
        if((board[j+1][i-1] == otherPlayer || board[j+1][i-1] == otherPlayer+2)
           && (board[j+2][i-2] == 0 || (i-2 == move.at(0).x && j+2 == move.at(0).y))
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
        if((board[j+1][i+1] == otherPlayer || board[j+1][i+1] == otherPlayer+2)
           && (board[j+2][i+2] == 0 || (i+2 == move.at(0).x && j+2 == move.at(0).y))
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
        if(board[j][i] == player || board[j][i] == player +2){
            
            king = board[j][i] > 2;
            
            // Checks North corners
            if((player == 1 || king) && j != 0){ // If on top row, then no North corners
    
                // check NW
                if(i != 0){
                    if(!jumped && board[j-1][i-1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i-1, j-1)});
                    } else if((board[j-1][i-1] == otherPlayer || board[j-1][i-1] == otherPlayer + 2)
                              && i != 1 && j != 1){
                        if(board[j-2][i-2] == 0){
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
                    if(!jumped && board[j-1][i+1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i+1, j-1)});
                    } else if((board[j-1][i+1] == otherPlayer ||board[j-1][i+1] == otherPlayer + 2)
                              && i != 6 && j != 1){
                        if(board[j-2][i+2] == 0){
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
                    if(!jumped && board[j+1][i-1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i-1, j+1)});
                    } else if((board[j+1][i-1] == otherPlayer ||board[j+1][i-1] == otherPlayer + 2)
                              && i != 1 && j != 6){
                        if(board[j+2][i-2] == 0){
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
                    if(!jumped && board[j+1][i+1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i+1, j+1)});
                    } else if((board[j+1][i+1] == otherPlayer ||board[j+1][i+1] == otherPlayer + 2)
                              && i != 6 && j != 6){
                        if(board[j+2][i+2] == 0){
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

// Displays all legal moves (If jumps are possible, will only display jumps)
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
    cout << " \nKEY:\n  P1 Pawn: " << "\033[1;31m" << "\u25EF" << "\033[0m\n";
    cout << "  P1 King: " << "\033[1;31m" << "\u25C6" << "\033[0m\n";
    cout << "  P2 Pawn: " << "\033[1;34m" << "\u25EF" << "\033[0m\n";
    cout << "  P2 King: " << "\033[1;34m" << "\u25C6" << "\033[0m\n";
    cout << "              X - axis" << '\n';
    cout << "         |-0-|-1-|-2-|-3-|-4-|-5-|-6-|-7-|" << '\n';
    cout << "  Y";
    cout << '\n' << "|---|    |---|---|---|---|---|---|---|---|" << '\n';
    for(int i = 0, j = 0; j < 8; i++){
        if(i == 0){
            cout << "|-" << j << "-|   ";
        }
        if(((i+j+1)%2 == 0)){
            if(currentBoard[j][i] == 1){ // Player 1 pawn = red circle
                cout << " | " << "\033[1;31m" << "\u25EF" << "\033[0m";
            } else if(currentBoard[j][i] == 3){ // Player 1 king = red diamond
                cout << " | " << "\033[1;31m" << "\u25C6" << "\033[0m";
            } else if(currentBoard[j][i] == 2){ // Player 2 pawn = blue circle
                cout << " | " << "\033[1;34m" << "\u25EF" << "\033[0m";
            } else if(currentBoard[j][i] == 4){ // Player 2 king = blue diamond
                cout << " | " << "\033[1;34m" << "\u25C6" << "\033[0m";
            } else {
                cout << " | " << currentBoard[j][i];
            }
        } else {
            cout << " |  ";
        }
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
        piece = board[moves.at(moveNum).at(0).y][moves.at(moveNum).at(0).x];
        // Makes piece king when needed
        if ((moves.at(moveNum).at(1).y == 0 && piece == 1) || (moves.at(moveNum).at(1).y == 7 && piece == 2)) {
            board[moves.at(moveNum).at(1).y][moves.at(moveNum).at(1).x] = piece + 2;
            // Keeps track of # pcs
            if(piece == 1){
                currentP1Kings++;
            } else{
                currentP2Kings++;
            }
        } else {
            board[moves.at(moveNum).at(1).y][moves.at(moveNum).at(1).x] = piece;
        }
        board[moves.at(moveNum).at(0).y][moves.at(moveNum).at(0).x] = 0;
    } else {
        piece = board[jumps.at(moveNum)->at(0).y][jumps.at(moveNum)->at(0).x];
        int jumpedX;
        int jumpedY;
        // Clears starting square
        board[jumps.at(moveNum)->at(0).y][jumps.at(moveNum)->at(0).x] = 0;
        for (int i = 0; i < jumps.at(moveNum)->size(); i++) {
            if(i == jumps.at(moveNum)->size() - 1){
                // Makes piece king when needed
                if ((jumps.at(moveNum)->at(i).y == 0 && piece == 1) || (jumps.at(moveNum)->at(i).y == 7 && piece == 2)){
                    board[jumps.at(moveNum)->at(i).y][jumps.at(moveNum)->at(i).x] = piece + 2;
                } else {
                    board[jumps.at(moveNum)->at(i).y][jumps.at(moveNum)->at(i).x] = piece;
                }
            } else {
                // Gets coordinate of jumped square and clears it
                jumpedX = (abs(jumps.at(moveNum)->at(i).x + jumps.at(moveNum)->at(i+1).x)) / 2;
                jumpedY = (abs(jumps.at(moveNum)->at(i).y + jumps.at(moveNum)->at(i+1).y)) / 2;
                // Keeps track of # pcs
                int jumpedPc = board[jumpedY][jumpedX];
                if(board == currentBoard){
                    currentTotalPcs--;
                    if(jumpedPc == 1 || jumpedPc == 3){
                        currentP1Pcs--;
                        if(jumpedPc == 3){
                            currentP1Kings--;
                        }
                    } else {
                        currentP2Pcs--;
                        if(jumpedPc == 4){
                            currentP2Kings--;
                        }
                    }
                }
                board[jumpedY][jumpedX] = 0;
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
int gameOver(int board[8][8] = currentBoard){
    bool player1Wins = true;
    bool player2Wins = true;
    
    for(int i = 0, j = 0; j < 8; i++){
        if(player2Wins && (board[j][i] == 1 || board[j][i] == 3)){
            player2Wins = false;
        } else if(player1Wins && (board[j][i] == 2 || board[j][i] == 4)){
            player1Wins = false;
        }
        
        if (!player1Wins && !player2Wins) {
            return 0;
        }
        
        if(i == 7){
            i = -1;
            j++;
        }
    }
    
    if (player1Wins) {
        return 1;
    } else {
        return 2;
    }
}

void copyBoard(int from[8][8], int to[8][8]){
    for(int i = 0, j = 0; j < 8; i++){
        
        to[j][i] = from[j][i];
        
        if(i == 7){
            i = -1;
            j++;
        }
    }
}

// Heuristic function
int evalFunc(int board[8][8], int player, int depth, bool noMoves, int numMoves){
    int val = 0;
    int p1Pawns = 0;
    int p2Pawns = 0;
    int p1Kings = 0;
    int p2Kings = 0;
    int p1Bottom = 0;
    int p2Top = 0;
    int p1MidBox = 0;
    int p2MidBox = 0;
    int p1MidRows = 0;
    int p2MidRows = 0;
    int p1Pcs = 0;
    int p2Pcs = 0;
    int totalpcs = 0;
    
    for(int i = 0, j = 0; j < 8; i++){
        // Home Rows (+- 50)
        if(j == 0 && (board[j][i] == 2 || board[j][i] == 4)){
            p2Top++;
        } else if(j == 7 && (board[j][i] == 1 || board[j][i] == 3)){
            p1Bottom++;
        }
        
        // Pawns (+- 100)
        if(board[j][i] == 1){
            val-=100;
            p1Pawns++;
            p1Pcs++;
            totalpcs++;
        } else if(board[j][i] == 2){
            val+=100;
            p2Pawns++;
            p2Pcs++;
            totalpcs++;
        } else if(board[j][i] == 3){ // Kings (+- 155)
            val-=155;
            p1Kings++;
            p1Pcs++;
            totalpcs++;
        } else if(board[j][i] == 4){
            val += 155;
            p2Kings++;
            p2Pcs++;
            totalpcs++;
        }
        
        // Middle box (+- 50)
        if((i >= 2 && i <= 5) && (j >= 2 && j <= 5)){
            if (board[j][i] == 2 || board[j][i] == 4) {
                p2MidBox++;
            } else if(board[j][i] == 2 || board[j][i] == 4){
                p1MidBox++;
            }
        }
        
        // Mid-board, but not in middle box (+- 10)
        if((i < 2 || i > 5) && (j >= 2 && j <= 5)){
            if (board[j][i] == 2 || board[j][i] == 4) {
                p2MidRows++;
            } else if(board[j][i] == 2 || board[j][i] == 4){
                p1MidRows++;
            }
        }
        
        if(i == 7){
            i = -1;
            j++;
        }
    }
    
    // Uses different heuristic for end-game. Players care more about kings and total pcs left.
    // End-game is "triggered" when one player has less than 2/3 of the other player's pcs, when there are
    //      10 or less pcs on the board, or when there are less than 15 pcs and one player has more kings.
    if(currentTotalPcs <= 10 || (currentP2Pcs*(2/3) >= currentP1Pcs) || (currentP1Pcs*(2/3) >= currentP2Pcs)
       || (currentTotalPcs <= 14 && abs(currentP1Kings - currentP2Kings) > 0)){
        // Different depending on who has more pcs. Winner will want to trade more.
        if(currentP2Pcs > currentP1Pcs && p2Pcs > p1Pcs){
            val += ((currentTotalPcs - totalpcs)*30);
        } else if(currentP1Pcs > currentP2Pcs && p1Pcs > p2Pcs) {
            val -= ((currentTotalPcs - totalpcs)*30);
        }
        // Kings are more valuable (+- 20)
        val += p2Kings*20;
        val -= p1Kings*20;
        
        // If early or mid game, positioning and pcs count matters more
    } else {
        val += ((p2Top*50) + (p2MidBox*50) + (p2MidRows*10));
        val -= ((p1Bottom*50) + (p1MidBox*50) + (p1MidRows*10));
    }
    
    // Values home row more if other player still has pawns to king (+- 40)
    if(p1Pawns > 0){
        val+=40*p2Top;
    } else if(p2Pawns > 0){
        val-=40*p1Bottom;
    }
    
    // Makes AI take the closer win and push off loss
    // Will also make AI try to block their opponent if it means winning fastest
    if(player == 2 && (noMoves || gameOver(board) == 1)){
        val -= (500 + (700 - (depth*10)));
    } else if (player == 1 && (noMoves || gameOver(board) == 2)){
        val += (500 + (700 - (depth*10)));
    }
    
    // Random number added to make it randomly choose between multiple "equal" moves
    val += rand() % 10;
    return val;
}

// Global variable to keep track of time limit
bool timeLimitPassed = false;

// Alpha Beta Search
int alphaBeta(int board[8][8], int depthLeft, int alpha, int beta, bool maxPlayer, time_t endTime,
              int currentDepth, bool root = false){
    
    // Checks if time limit has run out
    if(timeLimitPassed || time(nullptr) >= endTime){
        timeLimitPassed = true;
        return 0;
    }
    
    int boardCopy[8][8] = {0};
    vector<vector<Coordinate>> nodeMoves;
    vector<vector<Coordinate>*> nodeJumps;
    nodeMoves.reserve(200);
    int value;
    int tmpValue;
    int bestMove = 1;
    int player = maxPlayer ? 2 : 1;
    
    getLegalMoves(player, board, nodeMoves, nodeJumps);
    int moveAmt = nodeJumps.size() > 0 ? nodeJumps.size() : nodeMoves.size();
    
    // Runs evaluation function at max depth or end-game board, but will continue down a branch if there is a forced jump
    if((depthLeft <= 0 && (nodeJumps.size() == 0)) || moveAmt == 0 || gameOver(board) > 0){
        return evalFunc(board, player, currentDepth, (moveAmt == 0), moveAmt);
    }
    
    if(maxPlayer){
        value = -90000;
        for(int i = 1; i <= moveAmt; i++){
            copyBoard(board, boardCopy);
            ImplementMove(i, boardCopy, nodeMoves, nodeJumps);
            
            tmpValue = alphaBeta(boardCopy, depthLeft-1, alpha, beta, false, endTime, currentDepth+1);
            if(tmpValue > value){
                value = tmpValue;
                
                if(root){
                    bestMove = i;
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
        value = 90000;
        for(int i = 1; i <= moveAmt; i++){
            copyBoard(board, boardCopy);
            ImplementMove(i, boardCopy, nodeMoves, nodeJumps);
            
            tmpValue = alphaBeta(boardCopy, depthLeft-1, alpha, beta, true, endTime, currentDepth+1);
            if(tmpValue < value){
                value = tmpValue;
                
                if(root){
                    bestMove = i;
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
        cout << "Depth: 0\nTime Searching: " << time(nullptr) - startTime << " Seconds\n";
        return 1;
    }
    
    // If there is less than half the time left, then it won't finish the next iteration, so stop
    while((time(nullptr) + (seconds/2)) <= endTime){
        move = alphaBeta(currentBoard, depth, -90000, 90000, player2, endTime, 0, true);
        if(!timeLimitPassed){
            bestMove = move;
            depth++;
        }
    }
    cout << "Depth: " << depth << "\nTime Searching: " << time(nullptr) - startTime << " Seconds\n";
    return bestMove;
}

// Keeps track of turns without jumps. After 80 turns (40 each player) calls a draw.
bool callDraw(){
    if(jumpsList.size() == 0){
        turnCount++;
    } else {
        turnCount = 0;
    }
    if(turnCount >= 80){
        cout << "It has been 80 turns since a piece has been jumped. The game is a draw.\n";
        return true;
    } else {
        return false;
    }
}

// Plays AI vs AI
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
        
        // Checks for draw
        if(callDraw()){
            return;
        }
        
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
        
        // Checks for draw
        if(callDraw()){
            return;
        }
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
    if(gameOver() > 0){
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
        
        // Checks for draw
        if(callDraw()){
            return;
        }
        
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
        
        // Checks for draw
        if(callDraw()){
            return;
        }
    }
    
}

int main(int argc, const char * argv[]) {
    srand(time(nullptr));
    playGame();
    return 0;
}
