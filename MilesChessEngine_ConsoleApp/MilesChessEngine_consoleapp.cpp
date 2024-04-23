/*
Miles's Chess Engine

TODO:
add checks
->checkmate, and stalemate making use of legalmovecount and inCheck
pawn promotion, en passant, castling finish
game termination
TEZT EVERYTHINZ
better interface etc
make lichess bot
HOW TO CLEAN UP CODE!: ctrl-F Todo, to-do, debug, enpassant, rewinddebug
*/

#include <iostream>
#include <initializer_list>
#include "miles.cpp" //This library is used for one milesString. TODO consider removing later
#pragma	inline_recursion(on)
//#pragma inline_depth(64)
#pragma region defining
#define o 0
#define P 1
#define N 2
#define B 3
#define R 4
#define Q 5
#define K 6
#define p 9
#define n 10
#define b 11
#define r 12
#define q 13
#define k 14

#define pscore 100
#define nscore 300
#define bscore 300
#define rscore 500
#define qscore 900
#define kscore 10000
#define CHESSMATERIAL (pscore * 8 + nscore * 2 + bscore * 2 + rscore * 2 + qscore + kscore)
#define AFTERMATHPLYCAP 6

#define multindex(ROW, COL) ((ROW)*8+(COL))
#define multrow(mult) ((((mult) - multcol(mult)) / 8))
#define multcol(mult) ((mult) % 8)
#define abs(num) ((num) < 0 ? -(num) : (num))
#define boolnegate(num, cond) ((cond) ? -(num) : (num))
#define min(a_,b_) ((a_) < (b_) ? (a_) : (b_))
#define max(a_,b_) ((a_) > (b_) ? (a_) : (b_))
#define inrange(x,a_,b_) ((x) >= (a_) && (x) <= (b_))
#define turncompare(turn_, a_, b_) ((turn_ && a_ > b_) || (!turn_ && a_ < b_))
#define metricdifference(a_, b_) ((a_) > (b_) ? (a_) - (b_) : (b_) - (a_)) //finds the difference of two values ignoring any signs. Works with unsigned values
#define MOSTPOSSIBLELEGAL 218
//#define EVALTREEFILEDEBUG
//#define ENPASSANT

#pragma endregion
typedef __int8 pieceidtype;
typedef __int8 squaretype;
typedef __int16 depthtype;
typedef __int16 evaltype;
typedef unsigned long long hashtype;
//#define COOLBOICHARS //so far glitched, but makes chess symbols instead of letters
#ifdef COOLBOICHARS
const char displaypieces[] = { '.','♙','♘','♗','♖','♕','♔','\a','\a','♟︎','♞','♝','♜','♛','♚' };
#else
const char displaypieces[] = { '.','P','N','B','R','Q','K','\a','\a','p','n','b','r','q','k' };
#endif
const evaltype piecescores[] = { 0,pscore,nscore,bscore,rscore,qscore,kscore,0,0,pscore,nscore,bscore,rscore,qscore,kscore }; // second and third 0 shouldnt be accessed
unsigned long debugPosEvalCount = 0;
unsigned long debugPosEvalTranspositionCount = 0;

#include "MilesChess.h"

#ifdef EVALTREEFILEDEBUG
#include <fstream>
std::fstream evaltreefile;

void fileOutputEval(depthtype, evaltype);
void fileOutputEval(depthtype moveN, evaltype eval) {
	//for adding to the tree of evals. Assumes the previous submission gave an endline.
	for (int i = 0; i < moveN; i++) {
		::evaltreefile << " |";
	}
	::evaltreefile << eval << '\n';
}
void fileOutputLegalMoveCount(depthtype, depthtype);
void fileOutputLegalMoveCount(depthtype moveN, depthtype count) {
	//for adding to the tree of evals. Assumes the previous submission gave an endline.
	for (int i = 0; i < moveN; i++) {
		::evaltreefile << " |";
	}
	::evaltreefile << '/' << count << '\n';
}
/*void fileSubmit(void);
void fileSubmit(void) { evaltreefile.close(); }*/
#endif






int main()
{
	Game newgame;
	TranspositionTable TT;
	newgame.drawBoard();
	while (true) {
		std::cout << "0. Play Move\n1. Rewind Move\n2. Quit\n";
		int input;
		std::cin >> input;
		if (input == 0) {
			char alg[8];
			std::cout << "\nMove ? \n";
			back:
			std::cin >> alg;
			try {
				newgame.move(alg);
			}
			catch (int) {
				std::cout << "Please enter a valid move...\n";
				goto back;
			}
			std::cout << "Move Played\n";
			{
				std::cout << "Thinking...\n";
				depthtype depth;
				Move bestmove;
				evaltype eval;
				debugPosEvalCount = 0;
				eval = newgame.evaluate(5, bestmove, TT, false, true, true, true, false, 0, true, true);
				std::cout << "Position Evaluation: " << eval << '\n';
				std::cout << "Positions Evaluated: " << debugPosEvalCount << '\n';
				{
					std::cout << "Transpositions Saved: " << debugPosEvalTranspositionCount << '\n';
				}
				newgame.move(bestmove, true);
				newgame.drawBoard();
			}
		}
		if (input == 1) {
			newgame.rewind();
			if (newgame.turn() == false) /*black turn*/ {
				newgame.rewind();
			}
			std::cout << "Rewinded Move\n";
			newgame.drawBoard();
		}
		if (input == 2) {
			break; //quit it
		}
	}
}
