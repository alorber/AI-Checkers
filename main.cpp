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


using namespace std;

// Board Key:
// 0 = empty space, 1 = player 1, 2 = player 2, 3 = player 1 king, 4 = player 2 king
int board[8][8] = {0};

// Initializes new game board
void initStartBoard(){
    int player = 2;
    for(int i = 0, j = 0; j < 8; i++){
        if ((i+j+1)%2 == 0){
            board[i][j] = player;
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
    cout << "Enter name of file." << '\n';
    cin >> file;
    ifstream fin;
    fin.open(file);
    int square;
    
    int i = 0; int j = 0;
    while(fin >> square){
        board[i][j] = square;
        i++;
        if(i == 7){
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
vector<vector<Coordinate>> moves;
// Vector of moves that include a jump
vector<vector<Coordinate>*> jumps;

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
void jump(vector<Coordinate>& move, int i, int j, int player, int otherPlayer, bool king){
    // Keeps track of multiple possible double jumps (i.e need to add another move to the list)
    bool multiOptions = false;
    vector<Coordinate> moveCopy = move;
    
    // NW Corner
    if(i > 1 && j > 1 && (player == 1 || king)){
        if((board[i-1][j-1] == otherPlayer || board[i-1][j-1] == otherPlayer+2)
           && board[i-2][j-2] == 0 && legalJump(i-1, j-1, moveCopy)){
            move.push_back(Coordinate(i-2, j-2));
            multiOptions = true;
            jump(move, i-2, j-2, player, otherPlayer, king);
        }
    }
    
    // NE Corner
    if(i < 6 && j > 1 && (player == 1 || king)){
        if((board[i+1][j-1] == otherPlayer || board[i+1][j-1] == otherPlayer+2)
           && board[i+2][j-2] == 0 && legalJump(i+1, j-1, moveCopy)){
            if (multiOptions) {
                moves.push_back(moveCopy);
                jumps.push_back(&moves.at(moves.size()-1));
                (moves.at(moves.size()-1)).push_back(Coordinate(i+2, j-2));
                jump(moves.at(moves.size()-1), i+2, j-2, player, otherPlayer, king);
            } else {
                move.push_back(Coordinate(i+2, j-2));
                multiOptions = true;
                jump(move, i+2, j-2, player, otherPlayer, king);
            }
        }
    }
    
    // SW Corner
    if(i > 1 && j < 6 && (player == 2 || king)){
        if((board[i-1][j+1] == otherPlayer || board[i-1][j+1] == otherPlayer+2)
           && board[i-2][j+2] == 0 && legalJump(i-1, j+1, moveCopy)){
            if (multiOptions) {
                moves.push_back(moveCopy);
                jumps.push_back(&moves.at(moves.size()-1));
                (moves.at(moves.size()-1)).push_back(Coordinate(i-2, j+2));
                jump(moves.at(moves.size()-1), i-2, j+2, player, otherPlayer, king);
            } else {
                move.push_back(Coordinate(i-2, j+2));
                multiOptions = true;
                jump(move, i-2, j+2, player, otherPlayer, king);
            }
        }
    }
    
    // SE Corner
    if(i < 6 && j < 6 && (player == 2 || king)){
        if((board[i+1][j+1] == otherPlayer || board[i+1][j+1] == otherPlayer+2)
           && board[i+2][j+2] == 0 && legalJump(i+1, j+1, moveCopy)){
            if (multiOptions) {
                moves.push_back(moveCopy);
                jumps.push_back(&moves.at(moves.size()-1));
                (moves.at(moves.size()-1)).push_back(Coordinate(i+2, j+2));
                jump(moves.at(moves.size()-1), i+2, j+2, player, otherPlayer, king);
            } else {
                move.push_back(Coordinate(i+2, j+2));
                multiOptions = true;
                jump(move, i+2, j+2, player, otherPlayer, king);
            }
        }
    }
}

//Finds all legal moves for player
void getLegalMoves(int player){

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
                            jump(moves.at(moves.size()-1), i-2, j-2, player, otherPlayer, king);
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
                            jump(moves.at(moves.size()-1), i+2, j-2, player, otherPlayer, king);
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
                            jump(moves.at(moves.size()-1), i-2, j+2, player, otherPlayer, king);
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
                            jump(moves.at(moves.size()-1), i+2, j+2, player, otherPlayer, king);
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
    if(jumps.size() > 0){
        for(int i = 0; i < jumps.size(); i++){
            cout << i+1 << ".";
            
            for(int j = 0; j < jumps.at(i)->size(); j++){
                cout << " (" << jumps.at(i)->at(j).x << ", " << jumps.at(i)->at(j).y << ")";
            }
            cout << '\n';
        }
    } else {
        for(int i = 0; i < moves.size(); i++){
            cout << i+1 << ".";
            
            for(int j = 0; j < moves.at(i).size(); j++){
                cout << " (" << moves.at(i).at(j).x << ", " << moves.at(i).at(j).y << ")";
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
        cout << " | " << board[i][j];
        if(i == 7){
            cout << " | " << '\n' << "|---|    |---|---|---|---|---|---|---|---|" << '\n';
            i = -1;
            j++;
        }
    }
    cout << '\n';
}

// Takes the choice of move from legal move list and implements move
void ImplementMove(int moveNum){
    moveNum--;
    bool jump = jumps.size() > 0;
    if(!jump){
        board[moves.at(moveNum).at(1).x][moves.at(moveNum).at(1).y] = board[moves.at(moveNum).at(0).x][moves.at(moveNum).at(0).y];
        board[moves.at(moveNum).at(0).x][moves.at(moveNum).at(0).y] = 0;
    } else {
        int piece = board[jumps.at(moveNum)->at(0).x][jumps.at(moveNum)->at(0).y];
        int jumpedX;
        int jumpedY;
        // Clears starting square
        board[jumps.at(moveNum)->at(0).x][jumps.at(moveNum)->at(0).y] = 0;
        for (int i = 0; i < jumps.at(moveNum)->size(); i++) {
            if(i == jumps.at(moveNum)->size() - 1){
                board[jumps.at(moveNum)->at(i).x][jumps.at(moveNum)->at(i).y] = piece;
            } else {
                // Gets coordinate of jumped square
                jumpedX = (abs(jumps.at(moveNum)->at(i).x + jumps.at(moveNum)->at(i+1).x)) / 2;
                jumpedY = (abs(jumps.at(moveNum)->at(i).y + jumps.at(moveNum)->at(i+1).y)) / 2;
                board[jumpedX][jumpedY] = 0;
            }
        }
    }
}

// Asks user for starting player
bool userStarts(){
    string resp;
    cout << "Would you like to go first? (Y/N)" << '\n';
    cin >> resp;
    while(resp.compare("Y") != 0 && resp.compare("N") != 0){
        cout << "That was an invalid response. Please respond with 'Y' or 'N'." << '\n';
        cout << "Would you like to go first? (Y/N)" << '\n';
        cin >> resp;
    }
    if(resp.compare("Y") == 0){
        return true;
    } else {
        return false;
    }
}

void playGame(){
    // Decides starting player
    int startingPlayer;
    if(userStarts()){
        startingPlayer = 1;
    } else {
        startingPlayer = 2;
    }
    cout << "The starting player is " << startingPlayer << ".\n";
    
}

void testMoves(){
    string re = "Y";
    int choice;
    printBoard();
    while(re.compare("Y") == 0){
        getLegalMoves(1);
        printMoves();
        cout << "Which move?\n";
        cin >> choice;
        ImplementMove(choice);
        printBoard();
        cout << "Would you like to move again?\n";
        cin >> re;
    }
}

int main(int argc, const char * argv[]) {
    moves.reserve(200);
    initUserBoard();
    testMoves();
   // playGame();
//    printBoard();
//    getLegalMoves(1);
//    printMoves();
    return 0;
}
