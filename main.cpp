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


using namespace std;

// Board Key:
// 0 = empty space, 1 = player 1, 2 = player 2, 3 = player 1 king, 4 = player 2 king
int board[8][8] = {0};

//Initializes new game board
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

//Coordinate class to simplify things
class Coordinate {
public:
    int x, y;
    Coordinate(int i, int j) : x{i}, y{j} {}
};

void jump(){
    
}

// Legal Moves
vector<vector<Coordinate>> moves;
vector<vector<Coordinate>*> jumps;

void legalMoves(int player){

    int otherPlayer;
    bool king;
    
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
                    if(board[i-1][j-1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i-1, j-1)});
                    } else if((board[i-1][j-1] == otherPlayer || board[i-1][j-1] == otherPlayer + 2)
                              && i != 1 && j != 1){
                        if(board[i-2][j-2] == 0){
                           // ***Jump
                        }
                    }
                }
                
                // check NE
                if(i != 7){
                    if(board[i+1][j-1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i+1, j-1)});
                    } else if((board[i+1][j-1] == otherPlayer ||board[i+1][j-1] == otherPlayer + 2)
                              && i != 6 && j != 1){
                        if(board[i+2][j-2]){
                           //***Jump
                        }
                    }
                }
            }
            
            // Checks South Corners
            if((player == 2 || king) && j != 7){
                // check SW
                if(i != 0){
                    if(board[i-1][j+1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i-1, j+1)});
                    } else if((board[i-1][j+1] == otherPlayer ||board[i-1][j+1] == otherPlayer + 2)
                              && i != 1 && j != 6){
                        if(board[i-2][j+2] == 0){
                            //***Jump
                        }
                    }
                }
                
                // check SE
                if(i != 7){
                    if(board[i+1][j+1] == 0){
                        // Adds move
                        moves.push_back({Coordinate(i, j), Coordinate(i+1, j+1)});
                    } else if((board[i+1][j+1] == otherPlayer ||board[i+1][j+1] == otherPlayer + 2)
                              && i != 6 && j != 6){
                        if(board[i+2][j+2] == 0){
                            //***Jump
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

void printBoard(){
    for(int i = 0, j = 0; j < 8; i++){
        cout << " | " << board[i][j];
        if(i == 7){
            cout << " | " << '\n';
            i = -1;
            j++;
        }
    }
}

int main(int argc, const char * argv[]) {
    initStartBoard();
    legalMoves(1);
    
    return 0;
}
