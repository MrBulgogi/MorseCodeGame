# CSU23021-GRP36, Morse Code Game

This is a ARM Assembly code that provides a user with 4 progressively more difficult levels in order to learn morse code. 
The Game uses a rasberry pi and a button on GPIO input 20 to take inputs from the user. 
The user has the option to choose levels based on his/her skill level and once in game can either move onto the next level by getting 5 correct answers in a row or can lose by getting three incorrect answers in a row. 
There's is a life system in the game which dictates that if a player is incorrect he loses a life and if he's correct he gains a life. A player starts with 3 lives and cannot gain more than 3. 
Once a player completes the hardest level they complete the game and the game restarts. 

# set up
drag uf2 file into rasberry pi.
set up rasberry pi with terminal display on computer.
attach a button to GPIO input 20 and input morse to start game. 
Play. 
