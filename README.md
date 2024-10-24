# AI-Based Board Game

## Overview
This project was completed as a term project for the course **CSE 462: Introduction to Artificial Intelligence**. The goal was to implement a two-player board game on a 7x7 board, where players compete to move their pieces strategically across the board. I developed an AI using the minimax algorithm that outperformed all other student-developed AIs in the class, earning the only full score for the project.

## Game Description
The game is played on a 7x7 grid, where each player moves their pieces to one of the four neighboring cells (up, down, left, right) on each turn. The game can be played either against the AI or another human player. At the start, players choose whether they want to play as Player 1 ("X") or Player 2 ("O"). The game ends when one player can no longer make a valid move or when a specified turn limit is reached.

### Game Rules
- The board is represented with rows labeled "a" to "g" and columns numbered "1" to "7" (e.g., "a1" is the top-left corner).
- Players take turns moving their pieces to adjacent empty cells.
- Moves cannot go outside the board or onto cells occupied by the opponent or other pieces of the same player.
- The game ends if a player has no valid moves or the maximum number of turns is reached.

### Winning Conditions
- If a player cannot make a valid move, the other player wins.
- If the turn limit is reached, the player with more valid moveable spaces wins.
- Draws occur if both players have the same number of moveable spaces at the end.

## AI Strategy
The AI uses the **minimax algorithm** to make optimal moves. The algorithm evaluates possible future moves to determine the best action by minimizing the opponent's advantage while maximizing its own.

## Win Scenarios
The project includes visualizations to show different outcomes based on the moveable spaces available to each player. Below are examples of possible win conditions:

### Draw
- "X" can move to `c2, b3, e3, d4, f4, e5`
- "O" can move to `c1, a3, a5, b5, c6, d7`
- Both players have 6 moveable spaces, resulting in a draw.

### Player 2 Wins
- "X" has no valid moves, so Player 2 ("O") wins.

### Player 1 Wins
- "X" can move to `b3, c2, d1, e3, f4, g3`
- "O" can move to `a3, c1, e2, g3`
- Player 1 ("X") has 6 moveable spaces, while Player 2 ("O") has 4. Thus, Player 1 wins.

![Win Scenarios](https://github.com/berkyamanlar/AI-Based-Board-Game/blob/main/assests/scenarios.png)

## Features
- **Flexible Setup:** Players can set the number of pieces and the maximum turn limit at the start.
- **Interactive Interface:** Allows players to specify initial positions of the pieces or randomize them.
- **Move Validation:** The program checks for valid moves and prompts the user to correct any invalid actions.
- **Heuristic Function:** Evaluates board configurations based on moveable spaces, ensuring the AI adapts to various game settings.

## How to Play
1. Choose whether you want to be Player 1 ("X") or Player 2 ("O").
2. Set the number of pieces and maximum turn limit.
3. Take turns moving pieces to adjacent empty cells.
4. The game ends when a player cannot make a valid move or the turn limit is reached.
5. The player with more moveable spaces or the last valid move wins.

