 /*
* Boggle is a board game with a 4x4 square grid of letter cubes where players try to make words by connecting letters on
* adjacent cubes. This program searches a Boggle board for words using backtracking.
*
* The human player plays first, entering words one by one. Your code verifies that the word is is at least 4 letters long,
* then uses backtracking to see if the word can be made using letters on the board, using any cube at most once.
*
* Once the player has found as many words as they can, the computer player takes a turn. The computer searches through the
* board using recursive backtracking to find all the possible words that can be formed. The computer's algorithm is similar
* to the human's, but rather than verifying the existence of a single word in the board, you are exhaustively finding the
* set of all possible words.
*
*/

#include <string>
#include "grid.h"
#include "bogglegui.h"
#include "lexicon.h"
#include "vector.h"
#include "set.h"
using namespace std;

const static int N_BOGGLE_DICE = 16;
const static int N_DIRECTIONS = 8;

int scoreWord(int wordSize);
Vector<int> findNextStartDie(int i);
Vector<int> findNextDirectionDie(int i, Vector<int> currentDie);

// if current die is in bounds and hasn't been chosen in this explore path, recursively call for each of the 8 directions to check for entered word
bool humanWordSearchHelper(Grid<char>& board, Lexicon& dictionary, Vector<string> chosenDice, Vector<int> currentDie, string chosenWord, string enteredWord) {
   int row = currentDie[0];
   int col = currentDie[1];
   if(board.inBounds(row, col) && !chosenDice.contains(currentDie.toString()) && dictionary.containsPrefix(chosenWord)) {
       // choose - add letter to chosen word & current die to chosen dice
       string str = chosenWord;
       chosenWord.push_back(board[row][col]);
       chosenDice.add(currentDie.toString());

       // if word is found, does not continue further
       if(chosenWord.size() >= BoggleGUI::MIN_WORD_LENGTH && chosenWord == enteredWord && dictionary.contains(chosenWord)) {
           BoggleGUI::scorePointsHuman(scoreWord(chosenWord.size()));
           return true;
       }

       // explore - recursively call for each of the 8 directions
       for(int i = 0; i < N_DIRECTIONS; i++)
           if(humanWordSearchHelper(board, dictionary, chosenDice, findNextDirectionDie(i, currentDie), chosenWord, enteredWord)) return true;

       // unchoose - revert chosen word & dice
       chosenWord = str;
       chosenDice.removeBack();
   }
   return false;
}

// for each of the boggle dice letters, checks for the entered word starting with that letter
bool humanWordSearch(Grid<char>& board, Lexicon& dictionary, string word) {
   Vector<string> chosenDice;
   for(int i = 0; i < N_BOGGLE_DICE; i++)
       if(humanWordSearchHelper(board, dictionary, chosenDice, findNextStartDie(i), "", word))
           return true;
   return false;
}

// if current die is in bounds and hasn't been chosen in this explore path, recursively call for each of the 8 directions
Set<string> computerWordSearchHelper(Grid<char>& board, Lexicon& dictionary, Set<string>& foundWords, Set<string>& humanWords,
                                    Vector<string>& chosenDice, Vector<int> currentDie, string chosenWord) {
   int row = currentDie[0];
   int col = currentDie[1];
   if(board.inBounds(row, col) && !chosenDice.contains(currentDie.toString()) && dictionary.containsPrefix(chosenWord)) {
       // choose - add letter to chosen word & current die to chosen dice
       string str = chosenWord;
       chosenWord.push_back(board[row][col]);
       chosenDice.add(currentDie.toString());

       // if a valid word is found and not chosen by human, word is added to list of found words
       if(chosenWord.size() >= BoggleGUI::MIN_WORD_LENGTH && dictionary.contains(chosenWord) && !humanWords.contains(chosenWord) && !foundWords.contains(chosenWord)) {
           foundWords.add(chosenWord);
           BoggleGUI::scorePointsComputer(scoreWord(chosenWord.size()));
       }

       // explore - recursively call for each of the 8 directions
       Set<Set<string> > computerSetsFound;
       for(int i = 0; i < N_DIRECTIONS; i++) {
           Set<string> set = computerWordSearchHelper(board, dictionary, foundWords, humanWords,
                                                      chosenDice, findNextDirectionDie(i, currentDie), chosenWord);
           computerSetsFound.add(set);
       }

       // combine words found in all sets into one set
       for(Set<string> foundSet : computerSetsFound)
           for(string s : foundSet)
               foundWords.add(s);

       // unchoose - revert chosen word & dice
       chosenWord = str;
       chosenDice.removeBack();
   }
   return foundWords;
}

// explores all possible words on boggle board starting with letters on each die
Set<string> computerWordSearch(Grid<char>& board, Lexicon& dictionary, Set<string>& humanWords) {
   Vector<string> chosenDice;
   Set<Set<string> > computerSetsFound;
   Set<string> computerWordsFound, foundWords;
   for(int i = 0; i < N_BOGGLE_DICE; i++) {    // for each die on the boggle board,
                                               // explore the set of words starting
                                               // with the letter at the iTH die.
       Set<string> set = computerWordSearchHelper(board, dictionary, foundWords, humanWords, chosenDice, findNextStartDie(i), "");
       computerSetsFound.add(set);             // add each set to a set of sets
   }
   for(Set<string> foundSet : computerSetsFound)
       for(string s : foundSet)
           computerWordsFound.add(s);          // combine the words from all sets into
   return computerWordsFound;                  // one set of strings to return
}

// returns the appropriate score based on word size
int scoreWord(int wordSize) {
   if(wordSize <= 4)       return 1;
   else if(wordSize == 5)  return 2;
   else if(wordSize == 6)  return 3;
   else if(wordSize == 7)  return 5;
   else return 11;
}

// returns the coordinate of the next desired of 8 directions
Vector<int> findNextDirectionDie(int i, Vector<int> currentDie) {
   int row = currentDie[0];
   int col = currentDie[1];
   Vector<int> nextDirectionDie;
   if(i == 0) nextDirectionDie = {row - 1, col - 1};       // top left
   else if(i == 1) nextDirectionDie = {row - 1, col};      // top
   else if(i == 2) nextDirectionDie = {row - 1, col + 1};  // top right
   else if(i == 3) nextDirectionDie = {row, col - 1};      // left
   else if(i == 4) nextDirectionDie = {row, col + 1};      // right
   else if(i == 5) nextDirectionDie = {row + 1, col - 1};  // bottom left
   else if(i == 6) nextDirectionDie = {row + 1, col};      // bottom
   else if(i == 7) nextDirectionDie = {row + 1, col + 1};  // bottom right
   return nextDirectionDie;
}

// returns the coordinate of the next desired of 16 boggle dice
Vector<int> findNextStartDie(int i) {
   Vector<int> startDie;
   if(i == 0) startDie = {0, 0};
   else if(i == 1) startDie = {0, 1};
   else if(i == 2) startDie = {0, 2};
   else if(i == 3) startDie = {0, 3};
   else if(i == 4) startDie = {1, 0};
   else if(i == 5) startDie = {1, 1};
   else if(i == 6) startDie = {1, 2};
   else if(i == 7) startDie = {1, 3};
   else if(i == 8) startDie = {2, 0};
   else if(i == 9) startDie = {2, 1};
   else if(i == 10) startDie = {2, 2};
   else if(i == 11) startDie = {2, 3};
   else if(i == 12) startDie = {3, 0};
   else if(i == 13) startDie = {3, 1};
   else if(i == 14) startDie = {3, 2};
   else if(i == 15) startDie = {3, 3};
   return startDie;
}



