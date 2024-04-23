#pragma once
//class Advantage;
class Game {
private:
	unsigned short moveN_ = 0;
	std::vector<Move> moves_;
	std::vector<Piece> captured_;
	depthtype castlingEndtime_[4] = { 0,0,0,0 }; //when each castling was deactivated, for backtracking
	bool castling_[4] = { true,true,true,true }; //KINGSIDE, QUEENSIDE, kingside, queenside
	bool turn_ = true;
	bool terminated_ = false;
	squaretype whiteKingRow_ = 7;
	squaretype whiteKingCol_ = 4;
	squaretype blackKingRow_ = 0;
	squaretype blackKingCol_ = 4;
	squaretype whiteKing_ = 60;
	squaretype blackKing_ = 4;

	//removed all whiteking/blackking mult variables, am now just using row/col
	//IMPORTANT: each color represents which KING is being checked.
	bool isAttackCheckBlack_ = false;
	bool isDiscoverCheckBlack_ = false;
	squaretype attackCheckRowBlack_ = 0;
	squaretype attackCheckColBlack_ = 0;
	squaretype discoverCheckRowBlack_ = 0;
	squaretype discoverCheckColBlack_ = 0;
	bool isAttackCheckWhite_ = false;
	bool isDiscoverCheckWhite_ = false;
	squaretype attackCheckRowWhite_ = 0;
	squaretype attackCheckColWhite_ = 0;
	squaretype discoverCheckRowWhite_ = 0;
	squaretype discoverCheckColWhite_ = 0;

	Advantage advantage_;
	hashtype currentHash_ = 0;
	Board board_ = { \
			r,n,b,q,k,b,n,r,\
			p,p,p,p,p,p,p,p,\
			o,o,o,o,o,o,o,o,\
			o,o,o,o,o,o,o,o,\
			o,o,o,o,o,o,o,o,\
			o,o,o,o,o,o,o,o,\
			P,P,P,P,P,P,P,P,\
			R,N,B,Q,K,B,N,R }; //row\/,col->
public:
	Game() {};
	void drawBoard(void) const;
	void drawBoard(const Game& othergame) const;
	void debugInfo(void) const;
	void move(const Move& move, bool assumelegal = false);
	void move(const milesString& algNot) { move(nativeMove(algNot), true); }
	void rewind(void);
	bool canStillCastleKingside(bool color) const { return castling_[0 + (!color) * 2]; }
	bool canStillCastleQueenside(bool color) const { return castling_[1 + (!color) * 2]; }
	bool readyCastleQueenside(bool color) const { return (!board_.getSquare(0 + color * 7, 1).getPiece() && !board_.getSquare(0 + color * 7, 2).getPiece() && !board_.getSquare(0 + color * 7, 3).getPiece() && canStillCastleQueenside(color)); }
	bool readyCastleKingside(bool color) const { return (!board_.getSquare(0 + color * 7, 5).getPiece() && !board_.getSquare(0 + color * 7, 6).getPiece() && canStillCastleKingside(color)); }
	squaretype kingRow(bool color) const { return color ? whiteKingRow_ : blackKingRow_; }
	squaretype kingCol(bool color) const { return color ? whiteKingCol_ : blackKingCol_; }
	squaretype kingLocation(bool color) const { return color ? whiteKing_ : blackKing_; }
	Piece getSquare(squaretype row, squaretype col) const { return board_.getSquare(row, col); }
	Piece getSquare(squaretype mult) const { return board_.getSquare(mult); }
	depthtype castlingEndtime(squaretype index) const { return castlingEndtime_[index]; }
	bool castlingStill(squaretype index) const { return castling_[index]; }
	depthtype moveN(void) const { return moveN_; }
	Move nativeMove(milesString algNot) const;
	std::vector<Move> listLegalMoves(bool onlyCaptures = false, bool moveOrdering = false) const;
	bool isLegal(const Move& move) const;
	/*--------- Functions for helping with Checks ---------*/
	bool isAttacking(squaretype attacker, squaretype attackee) const { return isAttacking(multrow(attacker), multcol(attacker), multrow(attackee), multcol(attackee)); }
	bool isAttacking(squaretype attackerRow, squaretype attackerCol, squaretype attackeeRow, squaretype attackeeCol) const;
	bool hasLineOfSight(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol, bool colorAllow, bool safeMode = true) const;
	bool isCardinalDirection(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol) const;
	squaretype cardinalDirection(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol) const;
	void getLinearDirection(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol, int* rowDirectionOutput, int* colDirectionOutput) const;
	squaretype getLinearRowDirection(squaretype sourceRow, squaretype destRow) const;
	squaretype getLinearColDirection(squaretype sourceCol, squaretype destCol) const;
	squaretype findNextInSight(squaretype sourceRow, squaretype sourceCol, int rowDirection, int colDirection) const;
	bool getFindNextInSight(squaretype sourceRow, squaretype sourceCol, int rowDirection, int colDirection, squaretype* destRowOutput, squaretype* destColOutput) const;
	squaretype cardinalDirection(squaretype source, squaretype dest) const { return cardinalDirection(multrow(source), multcol(source), multrow(dest), multcol(dest)); }
	bool isCheck(void) const { return turn_ ? (isAttackCheckWhite_ || isDiscoverCheckWhite_) : (isAttackCheckBlack_ || isDiscoverCheckBlack_);/*If both of these are true at once its a bug*/ }
	/*--------        ---------*/
	evaltype evaluate(depthtype depth, TranspositionTable& TT, bool onlyCaptures = false, bool doAfterMath = true, bool anyPrune = true, bool moveOrdering = true, bool doPrune = false, evaltype prunelimit = 0, bool doTranspositionCheck = true, bool doStoreTranspositions = true);
	evaltype evaluate(depthtype depth, Move& bestmove, TranspositionTable& TT, bool onlyCaptures = false, bool doAfterMath = true, bool anyPrune = true, bool moveOrdering = true, bool doPrune = false, evaltype prunelimit = 0, bool doTranspositionCheck = true, bool doStoreTranpositions = true); //puts best move into bestmove alias
	evaltype heuristic(void);
	bool turn(void) { return turn_; }
};
