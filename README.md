# AI-Checkers
###### This is a checkers playing AI for project 1 of my artificial intelligence course.

On startup the game prompts the user with a series of options for the game:
- Load a new game board or import a custom board.
- Play against the AI or have the AI play itself.
- How many seconds the AI has to “think” of a move.
- The starting player.

The AI is implemented by a mini-max search with alpha-beta pruning and iterative deepening.

Whenever the AI moves, iterative deepening is run. Iterative deepening will successively run the alpha-beta search to a greater depth. If there is only one possible move then that move will be chosen without searching. If at any point while searching, the time limit is passed, the current search will be terminated and the move chosen by the previous search will be done. If half the time limit has passed after concluding the search to a certain depth, then the search will conclude, as it is unlikely that a search to the next depth will finish before the time limit. 

The alpha-beta search begins by checking the time limit, terminating if it has passed. If it is at the maximum depth to be searched or the current player is unable to move, it will return the output of the evaluation function. Since trades are an important part of checkers, it is possible for the evaluation function to be run after a piece is given up, but before the trade is completed. Therefore, the alpha-beta search will continue to a further depth until there are no more jumps to be made. For each possible move, the alpha-beta search will make a copy of the board, implement the move on the board copy, and will run the alpha-beta search on the new board. 

The evaluation function was created by continuously testing different heuristics against each other. The heuristic splits the game into two stages, early-game and end-game. The end-game is triggered when one player has less than two-thirds of the other player’s pieces, there are less than 15 pieces on the board and one player has more kings, or when there are less than 10 pieces on the board. Since, during early-game, position and piece count matter most, the early-game heuristic takes into account the following (in order of least valuable to most valuable): 
- Pieces each player has in the middle rows, but not in the middle box.
- Pieces each player has in the middle box.
- Pieces in a player’s own home row. This number is increased if the other player has pawns left to king.
- The number of pawns each player has.
- The number of kings each player has. 

Since, during end-game, the number of kings and total pieces left on the board matter most, the end-game heuristic takes into account the following (in order of least valuable to most valuable): 
- The number of pieces on the board
- The number of pawns each player has.
- The number of kings each player has. The value of a king is boosted. 
- The end-game heuristic makes a winning player more likely to trade in order to lower the total pieces on the board. 

If the current board results in one player winning, the value of both heuristics is shifted substantially and is proportional to the depth. Therefore, the AI will aim for the quickest win and the furthest lost. Finally a small random value is added to allow the AI to randomly pick between “equal” moves.
