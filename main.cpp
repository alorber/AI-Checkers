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
#include <stdlib.h>

using namespace std;

// Board Key:
// 0 = empty space, 1 = player 1, 2 = player 2, 3 = player 1 king, 4 = player 2 king
int board[8][8] = {0};

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
    printBoard();
    return 0;
}

