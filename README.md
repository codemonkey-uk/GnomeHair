# GnomeHair
An extremely task specific tool for generating a json look up table of hair colours for a population of gnomes found in npc.csv

# Compile
g++ -std=c++17 gh.cpp 

# Run
Loads npc.csv (you need to supply this) and colours.txt (provided) from the current directory. 

Produces summary info followed by json to the console.

# Assumption
* Gnomes are found by checking race column == 1
