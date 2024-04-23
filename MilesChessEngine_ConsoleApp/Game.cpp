#ifdef EVALTREEFILEDEBUG 
void fileOutputEval(depthtype, evaltype);
void fileOutputLegalMoveCount(depthtype, depthtype);
#endif

void Game::drawBoard(void) const {
	std::cout << "\n*abcdefgh";
	for (int row = 0; row < 8; row++) {
		std::cout << '\n' << 8 - row;
		for (int col = 0; col < 8; col++) {
			std::cout << displaypieces[board_.getSquare(row, col).getPiece()];
		}
	}
	std::cout << '\n';
}

void Game::drawBoard(const Game& othergame) const {
	std::cout << "\n*abcdefgh";
	for (int row = 0; row < 8; row++) {
		std::cout << '\n' << 8 -  row;
		for (int col = 0; col < 8; col++) {
			std::cout << displaypieces[othergame.board_.getSquare(row, col).getPiece()];
		}
	}
	std::cout << '\n';
}

Move Game::nativeMove(milesString algNot) const {
	pieceidtype search = 0;
	squaretype sourceRow = 8;
	squaretype sourceCol = 8;
	squaretype destRow = 8;
	squaretype destCol = 8;
	bool breakflag;
	switch (algNot[0]) {
	case 'B': if (!search) search = B;
	case 'R': if (!search) search = R;
	case 'Q': if (!search) search = Q;
	case 'K': if (!search) search = K;
	case 'N': if (!search) search = N;
	case 'O': if (!search) {
		if (algNot == "O-O") {
			if (!isLegal(Move(7 * turn_, 4, 7 * turn_, 6))) break;
			sourceRow = 7 * turn_;
			sourceCol = 4;
			destRow = 7 * turn_;
			destCol = 6;
		}
		if (algNot == "O-O-O") {
			if (!isLegal(Move(7 * turn_, 4, 7 * turn_, 2))) break;
			sourceRow = 7 * turn_;
			sourceCol = 4;
			destRow = 7 * turn_;
			destCol = 2;
		}
		break;
	}
	default: if (!search) { search = P; algNot = "P" + algNot; }
		   if (algNot.find('x') != algNot.length()) { algNot.remove(algNot.find('x')); }
		   if (algNot[(algNot.length() - 1)] == '+') { algNot.remove(algNot.length() - 1); }
		   if (algNot[(algNot.length() - 1)] == '#') { algNot.remove(algNot.length() - 1); }
		   search += ((!turn_) * 8);
		   if (algNot.length() == 3) {
			   breakflag = false;
			   destCol = (algNot[1] - 'a');
			   destRow = (7 - (algNot[2] - '1'));
			   for (squaretype row = 0; row < 8; row++) {
				   for (squaretype col = 0; col < 8; col++) {
					   if (board_.getSquare(row, col).getPiece() == search && isLegal(Move(row, col, destRow, destCol))) {
						   sourceRow = row;
						   sourceCol = col;
						   breakflag = true;
						   break;
					   }
					   if (breakflag) break;
				   }
			   }
		   }
		   if (algNot.length() == 4) {
			   breakflag = false;
			   destCol = (algNot[2] - 'a');
			   destRow = (7 - (algNot[3] - '1'));
			   if (algNot[1] <= '9' && algNot[1] >= '0') {
				   squaretype row = 7 - (algNot[1] - '0');
				   for (squaretype col = 0; col < 8; col++) {
					   if (board_.getSquare(row, col).getPiece() == search && isLegal(Move(row, col, destRow, destCol))) {
						   sourceRow = row;
						   sourceCol = col;
						   breakflag = true;
						   break;
					   }
					   if (breakflag) break;
				   }
			   }
			   else if (algNot[1] <= 'h' && algNot[1] >= 'a') {
				   squaretype col = algNot[1] - 'a';
				   for (squaretype row = 0; row < 8; row++) {
					   if (board_.getSquare(row, col).getPiece() == search && isLegal(Move(row, col, destRow, destCol))) {
						   sourceRow = row;
						   sourceCol = col;
						   breakflag = true;
						   break;
					   }
					   if (breakflag) break;
				   }
			   }
			   if (algNot.length() == 5) {
				   destCol = (algNot[3] - 'a');
				   destRow = (7 - (algNot[4] - '1'));
				   sourceCol = algNot[1] - 'a';
				   sourceRow = 7 - (algNot[2] - '0');
			   }
		   }
		   break;
	}
	if (sourceRow == 8) {
		throw 897;
	}
	return Move(sourceRow, sourceCol, destRow, destCol);
}

void Game::move(const Move& move, bool assumelegal) {
	//need to ALTER ALL GAME MEMBER DATA WITH Game::move (moveN_, moves_, terminated_, ETC)
	//TODO alter game member data
	if (!assumelegal) if (!isLegal(move)) { throw "Illegal Move"; }
	captured_.push_back(board_.getSquare(move.getDestRow(), move.getDestCol()));
	moves_.push_back(move);

	//here we are updating material and castling and king locations
	//need to update checks
#ifdef ENPASSANT
	bool isPassantRow = ((move.getSourceRow() + boolnegate(1, color) == move.getDestRow()) && (board_.getSquare(move.getSource()).getPiece() == (p - 8 * color)) && (board_.getSquare(move.getSourceRow(), move.getDestCol()).getPiece() == (P + 8 * color)));
	bool isPassantLeft = isPassantRow && (move.getSourceCol() - 1 == move.getDestCol());
	bool isPassantRight = isPassantRow && (move.getSourceCol() + 1 == move.getDestCol());
#endif
	bool isCastleQueen = ((board_.getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 2));
	bool isCastleKing = ((board_.getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 6));

	if (isAttacking(move.getDestRow(), move.getDestCol(), kingRow(!turn_), kingCol(!turn_))) {
		if (!turn_) {//Inverted because whoever is being checked is the opposite of the current turn
			isAttackCheckWhite_ = true;
			attackCheckRowWhite_ = move.getDestRow();
			attackCheckColWhite_ = move.getDestCol();
		}
		else {
			isAttackCheckBlack_ = true;
			attackCheckRowBlack_ = move.getDestRow();
			attackCheckColBlack_ = move.getDestCol();
		}
	}
	else {
		if (!turn_) {
			isAttackCheckWhite_ = false;
		}
		else {
			isAttackCheckBlack_ = false;
		}
	}

	squaretype discoverCheckRow;
	squaretype discoverCheckCol;

	if (getFindNextInSight(move.getSourceRow(), move.getSourceCol(), getLinearRowDirection(kingRow(!turn_), move.getSourceRow()/*Reversed because we're backtracking*/), getLinearColDirection(kingCol(!turn_), move.getSourceCol()), &discoverCheckRow, &discoverCheckCol)) {
		if (isAttacking(discoverCheckRow, discoverCheckCol, kingRow(!turn_), kingCol(!turn_))) {
			if (!turn_) {
				isDiscoverCheckWhite_ = true;
				discoverCheckRowWhite_ = discoverCheckRow;
				discoverCheckColWhite_ = discoverCheckCol;
			}
			else {
				isDiscoverCheckBlack_ = true;
				discoverCheckRowBlack_ = discoverCheckRow;
				discoverCheckColBlack_ = discoverCheckCol;
			}
		}
		else {
			if (!turn_) {
				isDiscoverCheckWhite_ = false;
			}
			else {
				isDiscoverCheckBlack_ = false;
			}
		}
	}

	if (board_.getSquare(move.getSource()).getPiece() == K) {
		whiteKingRow_ = move.getDestRow();
		whiteKingCol_ = move.getDestCol();
		whiteKing_ = move.getDest();
		if (castling_[0]) castlingEndtime_[0] = moveN_; //REVERSED ORDER FOR CASTLING ENDTIME AND CASTLING CHANGES, AND ADDED IF STATEMENTS TO PREVENT OVERWRITING ENDTIMES!! Old way was bugged; also see following reversed and if-statement-ed lines
		if (castling_[1]) castlingEndtime_[1] = moveN_;
		castling_[0] = false;
		castling_[1] = false;
	}
	if (board_.getSquare(move.getSource()).getPiece() == k) {
		blackKingRow_ = move.getDestRow();
		blackKingCol_ = move.getDestCol();
		blackKing_ = move.getDest();
		if (castling_[2]) castlingEndtime_[2] = moveN_;
		if (castling_[3]) castlingEndtime_[3] = moveN_;
		castling_[2] = false;
		castling_[3] = false;

	}
	//TODO OPTIMIZE: Consider removing location check, and remove king remover after check has been implemented
	if (board_.getSquare(move.getSource()).getPiece() == R) {
		if (move.getSource() == 56) { if (castling_[1]) castlingEndtime_[1] = moveN_; castling_[1] = false; }
		if (move.getSource() == 63) { if (castling_[0]) castlingEndtime_[0] = moveN_; castling_[0] = false; }
	}
	if (board_.getSquare(move.getSource()).getPiece() == r) {
		if (move.getSource() == 0) { if (castling_[3]) castlingEndtime_[3] = moveN_; castling_[3] = false; }
		if (move.getSource() == 7) { if (castling_[2]) castlingEndtime_[2] = moveN_; castling_[2] = false; }
	}
	if (board_.getSquare(move.getDest()).getPiece() == R) {
		if (move.getDest() == 56) { if (castling_[1]) castlingEndtime_[1] = moveN_; castling_[1] = false; }
		if (move.getDest() == 63) { if (castling_[0]) castlingEndtime_[0] = moveN_; castling_[0] = false; }
	}
	if (board_.getSquare(move.getDest()).getPiece() == r) {
		if (move.getDest() == 0) { if (castling_[3]) castlingEndtime_[3] = moveN_; castling_[3] = false; }
		if (move.getDest() == 7) { if (castling_[2]) castlingEndtime_[2] = moveN_; castling_[2] = false; }
	}
	if (board_.getSquare(move.getDest()).getPiece() == K) {
		if (move.getDest() == 60) { if (castling_[1]) castlingEndtime_[1] = moveN_; castling_[1] = false; if (castling_[0]) castlingEndtime_[0] = moveN_; castling_[0] = false; }
	}
	if (board_.getSquare(move.getDest()).getPiece() == k) {
		if (move.getDest() == 4) { if (castling_[3]) castlingEndtime_[3] = moveN_; castling_[3] = false; if (castling_[2]) castlingEndtime_[2] = moveN_; castling_[2] = false; }
	}
#ifdef ENPASSANT
	if (isPassantLeft || isPassantRight) {
		if (color == false) {//Opposite color here because can only capture opposite color
			whiteMaterial_ -= pscore;
		}
		if (color == true) {//Opposite color here because can only capture opposite color
			blackMaterial_ -= pscore;
		}
	}
#endif
	advantage_.updateAdvantage(move, *this);
	currentHash_ = HASHKEYS::newHash(*this, move, isCastleQueen, isCastleKing, currentHash_);
	board_.move(move);
	moveN_++;
	turn_ = !turn_;
}

void Game::rewind(void) {
	Move move = moves_.back();
	Piece captured = captured_.back();
	bool color = board_.getSquare(move.getDest()).getColor();
	if (board_.getSquare(move.getDest()).getPiece() == K) {
		whiteKing_ = move.getSource();
	}
	if (board_.getSquare(move.getDest()).getPiece() == k) {
		blackKing_ = move.getSource();
	}
	board_.reverseMove(move, captured);
	moves_.pop_back();
	captured_.pop_back();
	moveN_--;
	turn_ = !turn_;
	bool isQueensideCastle = ((getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 2));
	bool isKingsideCastle = ((getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 6));
	currentHash_ = HASHKEYS::newHash(*this, move, isQueensideCastle, isKingsideCastle, currentHash_);
	for (size_t i = 0; i < 4; i++) {
		if (!castling_[i]) {
			if (moveN_ == castlingEndtime_[i]) { //changed to just castlingEndtime_[i] (not + 1) because moveN_-- was moved before it
				castling_[i] = true;
			}
		}
	}
	advantage_.downdateAdvantage(move, *this);
}

bool Game::isLegal(const Move& move) const {
	//this is an isLegal function, the legalMoveGeneration function will not rely on this, but use its own method, as they have reverse algorithms and would make huge inefficiency if connected
	//still gotta add pins and checks
	Piece sourcepiece = board_.getSquare(move.getSourceRow(), move.getSourceCol());
	Piece destpiece = board_.getSquare(move.getDestRow(), move.getDestCol());
	bool color = sourcepiece.getColor();
	if (destpiece.getColor() == color && destpiece.getPiece()) { return 0; } //friendly fire, also covers no-moves
	if (color != turn_) {
		return 0; //wrong turn
	}
	switch (sourcepiece.getNeutralPiece()) {
	case P: {//finish and add enpassant //TODO: Add en passant
		bool isTaking = destpiece.getPiece();
		if (isTaking) {
			if (abs(move.getDestCol() - move.getSourceCol()) != 1 || (move.getDestRow() - move.getSourceRow() != boolnegate(1, color))) { return 0; }
		}
		else {
			if (move.getDestCol() != move.getSourceCol()) { return 0; }
			if (move.getDestRow() - move.getSourceRow() != boolnegate(1, color)) {
				if (move.getDestRow() - move.getSourceRow() != boolnegate(2, color) && move.getSourceRow() == (1 + 5 * color)) {
					return 0;
				}
			}
		}
		break; }
	case N: {
		squaretype drow = abs(move.getDestRow() - move.getSourceRow());
		squaretype dcol = abs(move.getDestCol() - move.getSourceCol());
		if (!((drow == 2 && dcol == 1) || (drow == 1 && dcol == 2))) {
			return 0;
		}
		break; }
	case K: {
		bool isCastleQueen = ((move.getSourceCol() == 4) && (move.getDestCol() == 2));
		bool isCastleKing = ((move.getSourceCol() == 4) && (move.getDestCol() == 6));
		if (isCastleKing && !canStillCastleKingside(color)) { return 0; }
		if (isCastleQueen && !canStillCastleQueenside(color)) { return 0; }
		if ((abs(move.getDestCol() - move.getSourceCol()) > 1 || abs(move.getDestRow() - move.getSourceRow()) > 1) && !(isCastleKing || isCastleQueen)) { return 0; } //all done!! not castling but too long a stride
		break;
	}
	case R: {
		if ((move.getSourceCol() == move.getDestCol()) == (move.getSourceRow() == move.getDestRow())) { return 0; }
		if (move.getSourceCol() == move.getDestCol()) {
			for (squaretype i = min(move.getSourceRow(), move.getDestRow()) + 1; i < max(move.getSourceRow(), move.getDestRow()); i++) {
				if (board_.getSquare(i, move.getSourceCol()).getPiece()) {
					return 0; //obstacle piece in col move
				}
			}
		}
		else {
			for (squaretype i = min(move.getSourceCol(), move.getDestCol()) + 1; i < max(move.getSourceCol(), move.getDestCol()); i++) {
				if (board_.getSquare(move.getSourceRow(), i).getPiece()) {
					return 0; //obstaclpiece in row move
				}
			}
		}
		break; }
	case B: {
		if ((abs(move.getDestCol() - move.getSourceCol()) != abs(move.getDestRow() - move.getSourceRow())) || move.getDestRow() == move.getSourceRow()) { return 0; }
		if ((move.getDestCol() - move.getSourceCol()) / (move.getDestRow() - move.getSourceRow()) == 1) {
			squaretype col = min(move.getSourceCol(), move.getDestCol()) + 1;
			for (squaretype row = min(move.getSourceRow(), move.getDestRow()) + 1; row < max(move.getSourceRow(), move.getDestRow()); row++) {
				if (board_.getSquare(row, col).getPiece()) {
					return 0; //obstacle piece in lowslope move (technically row and col are both going up but rows are inverted)
				}
				col++;
			}
		}
		else {
			squaretype col = min(move.getSourceCol(), move.getDestCol()) + 1;
			for (squaretype row = max(move.getSourceRow(), move.getDestRow()) - 1; row > min(move.getSourceRow(), move.getDestRow()); row--) {
				if (board_.getSquare(row, col).getPiece()) {
					return 0; //obstacle piece in highslope move 
				}
				col++;
			}
		}
		break; }
	case Q: {//a queen move is either a bishop or rook move
		bool isBishopMove = !((abs(move.getDestCol() - move.getSourceCol()) != abs(move.getDestRow() - move.getSourceRow())) || move.getDestRow() == move.getSourceRow()); //check if bishop move
		bool isRookMove = !((move.getSourceCol() == move.getDestCol()) == (move.getSourceRow() == move.getDestRow())); //check if rook move
		if (isRookMove == isBishopMove) { return 0; } //either both a rook and bishop move or neither
		if (isRookMove) {//below, checking same for rook and bishop but with conditional based on move type
			if (move.getSourceCol() == move.getDestCol()) {
				for (squaretype i = min(move.getSourceRow(), move.getDestRow()) + 1; i < max(move.getSourceRow(), move.getDestRow()); i++) {
					if (board_.getSquare(i, move.getSourceCol()).getPiece()) {
						return 0;
					}
				}
			}
			else {
				for (squaretype i = min(move.getSourceCol(), move.getDestCol()) + 1; i < max(move.getSourceCol(), move.getDestCol()); i++) {
					if (board_.getSquare(move.getSourceRow(), i).getPiece()) {
						return 0;
					}
				}
			}
		}
		if (isBishopMove) {
			if ((move.getDestCol() - move.getSourceCol()) / (move.getDestRow() - move.getSourceRow()) == 1) {
				squaretype col = min(move.getSourceCol(), move.getDestCol()) + 1;
				for (squaretype row = min(move.getSourceRow(), move.getDestRow()) + 1; row < max(move.getSourceRow(), move.getDestRow()); row++) {
					if (board_.getSquare(row, col).getPiece()) {
						return 0;
					}
					col++;//dad put here to fix diagonal bug (was above line 272)
				}
			}
			else {
				squaretype col = min(move.getSourceCol(), move.getDestCol()) + 1;
				for (squaretype row = max(move.getSourceRow(), move.getDestRow()) - 1; row > min(move.getSourceRow(), move.getDestRow()); row--) {
					if (board_.getSquare(row, col).getPiece()) {
						return 0;
					}
					col++;//dad put here to fix diagonal bug (was above line 281)
				}
			}
		}
		break; }
	}
	return 1;
}

bool Game::isAttacking(squaretype attackerRow, squaretype attackerCol, squaretype attackeeRow, squaretype attackeeCol) const {
	//IMPORTANT: THIS DOES NOT FACTOR PINS OR CURRENT CHECKS, JUST SEES IF PIECE CAN REACH THE OTHER PIECE. This is intended for use with isCheck().
	//WILL ONLY RETURN TRUE IF THE PIECE IS ATTACKING AN OPPOSITE COLORED PIECE
	pieceidtype attackerPiece = board_.getSquare(attackerRow, attackerCol).getNeutralPiece();
	bool attackerColor = board_.getSquare(attackerRow, attackerCol).getColor();
	switch (attackerPiece) {
	case P:
		return (metricdifference(attackerCol, attackeeCol) == 1 && attackerRow - attackeeRow == boolnegate(-1, attackerColor));
		//         /*If the capture is diagonal*/            /*Negates to see if pawn is attacking, reverse because white negates but should do opposite*/
	case B:
		return (metricdifference(attackerRow, attackeeRow) == metricdifference(attackerCol, attackeeCol) && hasLineOfSight(attackerRow, attackerCol, attackeeRow, attackeeCol, !attackerColor, false));
		//          /*Check if bishop move*/                                                          /*Check line of sight*/
		break;
	case N: {
		squaretype drow = metricdifference(attackerRow, attackeeRow);
		squaretype dcol = metricdifference(attackerCol, attackeeCol);
		return (drow == 2 && dcol == 1 || drow == 1 && dcol == 2); }
	case R:
		return ((attackerRow - attackeeRow == 0 || attackerCol - attackeeCol == 0) && hasLineOfSight(attackerRow, attackerCol, attackeeRow, attackeeCol, !attackerColor, false)); //TODO nest in if loop and research && optimization, test
		//          /*Check if rook move*/                                                            /*Check line of sight*/ 
	case Q:
		return hasLineOfSight(attackerRow, attackerCol, attackeeRow, attackeeCol, !attackerColor, true);
		//                                         /*Check line of sight*/ 
	case K:
		return (metricdifference(attackerRow, attackeeRow) <= 1 && metricdifference(attackerCol, attackeeCol) <= 1);
	}
}

bool Game::hasLineOfSight(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol, bool colorAllow, bool safeMode) const {
	//WILL ONLY RETURN TRUE IF THE PIECE AT THE DESTINATION IS THE SAME AS THE COLOR ALLOW
	//will first check if a cardinaldirection exists
	pieceidtype debugAttacker = board_.getSquare(sourceRow, sourceCol).getPiece();
	if (safeMode && !isCardinalDirection(sourceRow, sourceCol, destRow, destCol)) {
		return false;
	}
	//now checking if line of sight exists
	int rowDirection = (sourceRow > destRow) ? -1 : (sourceRow < destRow); //Outputs: >:-1 =:0 <:1
	int colDirection = (sourceCol > destCol) ? -1 : (sourceCol < destCol);
	sourceRow += rowDirection; //
	sourceCol += colDirection; //get off the initial square, which contains the moving piece. instead of putting increment at start of while loop, bc we want to have different behavior for last square (colorAllow).
	while (sourceRow != destRow || sourceCol != destCol) {
		if (board_.getSquare(sourceRow, sourceCol).getPiece()) { return false; }
		sourceRow += rowDirection;
		sourceCol += colDirection;
	}
	if (board_.getSquare(sourceRow, sourceCol).getPiece() && board_.getSquare(sourceRow, sourceCol).getColor() != colorAllow) { return false; }
	return true;
}

bool Game::isCardinalDirection(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol) const {
	if (sourceRow == destRow && sourceCol == destCol) return false; //this line can be disabled, but would return true if it would have been called 
	/*
	if (metricdifference(sourceRow, destRow) == metricdifference(sourceCol, destCol)) {
		//now we know that we are on the same diagonal
		return true;
	}
	if (sourceCol == destCol) { //cols first for very minor optimization as these checks are more common
		//now we know we are on the same horizontal
		return true;
	}
	if (sourceRow == destRow) {
		//now we know we are on the same vertical
		return true;
	}
	//else
	return false;
	*/
	//COMPRESSES TO
	return metricdifference(sourceRow, destRow) == metricdifference(sourceCol, destCol) || sourceCol == destCol || sourceRow == destRow;
}

squaretype Game::cardinalDirection(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol) const {
	/*for seeing if two squares have a diagonal, vertical, or horizontal path. Returns: (Directions are relative to white's side on the chess board, which has rows starting at 0 at the top)
	* N/A: 0
	* N  : 1
	* NE : 2
	* E  : 3
	* SE : 4
	* S  : 5
	* SW : 6
	* W  : 7
	* NW : 8
	* NS : 9 //Staying Put
	* Cardinals are even, diagonals are odd
	*/
	if (sourceRow == destRow && sourceCol == destCol) return 9; //this line can be disabled, but would get unexpected behavior if it would have been called 
	if (metricdifference(sourceRow, destRow) == metricdifference(sourceCol, destCol)) {
		//now we know that we are on the same diagonal
		if (sourceRow > destRow && sourceCol > destCol) return 8;
		if (sourceRow > destRow && sourceCol < destCol) return 2;
		if (sourceRow < destRow && sourceCol < destCol) return 4;
		if (sourceRow < destRow && sourceCol > destCol) return 6;
	}
	if (sourceCol == destCol) { //cols first for very minor optimization as these checks are more common 
		//now we know we are on the same horizontal
		if (sourceRow > destRow) {
			return 1;
		}
		else {
			return 5;
		}
	}
	if (sourceRow == destRow) {
		//now we know we are on the same vertical
		if (sourceCol > destCol) {
			return 7;
		}
		else {
			return 3;
		}
	}
	//else
	return 0;
}

void Game::getLinearDirection(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol, int* rowDirectionOutput, int* colDirectionOutput) const {
	*rowDirectionOutput = (sourceRow > destRow) ? -1 : (sourceRow < destRow); //Outputs: >:-1 =:0 <:1
	*colDirectionOutput = (sourceCol > destCol) ? -1 : (sourceCol < destCol);
}

squaretype Game::getLinearRowDirection(squaretype sourceRow, squaretype destRow) const {
	//CAREFUL NOT TO INPUT TWO SQUARE MULTS!!!
	return (sourceRow > destRow) ? -1 : (sourceRow < destRow);
}

squaretype Game::getLinearColDirection(squaretype sourceCol, squaretype destCol) const {
	//CAREFUL NOT TO INPUT TWO SQUARE MULTS!!!
	return (sourceCol > destCol) ? -1 : (sourceCol < destCol);
}

squaretype Game::findNextInSight(squaretype sourceRow, squaretype sourceCol, int rowDirection, int colDirection) const {
	while (inrange(sourceRow, 0, 7) && inrange(sourceCol, 0, 7)) {
		sourceRow += rowDirection;
		sourceCol += colDirection;
		if (board_.getSquare(sourceRow, sourceCol).getPiece()) { return multindex(sourceRow, sourceCol); }
	}
	return 64; //fallback
}

bool Game::getFindNextInSight(squaretype sourceRow, squaretype sourceCol, int rowDirection, int colDirection, squaretype *destRowOutput, squaretype *destColOutput) const {
	while (inrange(sourceRow, 0, 7) && inrange(sourceCol, 0, 7)) {
		sourceRow += rowDirection;
		sourceCol += colDirection;
		if (board_.getSquare(sourceRow, sourceCol).getPiece()) { *destRowOutput = sourceRow; *destColOutput = sourceCol; return true; }
	}
	return false;
}

void Game::debugInfo(void) const {
	std::cout << "WhiteKing:" << (int)whiteKing_ << '\n';
	std::cout << "BlackKing:" << (int)blackKing_ << '\n';
	std::cout << "KingsideCastleWhite:" << castling_[0] << '\n';
	std::cout << "QueensideCastleWhite:" << castling_[1] << '\n';
	std::cout << "KingsideCastleBlack:" << castling_[2] << '\n';
	std::cout << "QueensideCastleBlack:" << castling_[3] << '\n';
	std::cout << "Evaluation:\n";
	advantage_.listAdvantages();
	std::cout << "Position Hash:" << currentHash_ << '\n';
	return;
}

evaltype Game::heuristic(void) { return advantage_.getAdvantage(); }

evaltype Game::evaluate(depthtype depth, TranspositionTable& TT, bool onlyCaptures, bool doAfterMath, bool anyPrune, bool moveOrdering, bool doPrune, evaltype prunelimit, bool doTranspositionCheck, bool doStoreTranspositions) {
	evaltype ret;
	if (depth == 0) {
		if (doAfterMath && !onlyCaptures) {
			ret = evaluate(AFTERMATHPLYCAP, TT, true, true, anyPrune, moveOrdering, 0, 0, doTranspositionCheck, doStoreTranspositions);
		}
		else {
			ret = heuristic();
			debugPosEvalCount++; //for position evaluation counting
		}
#ifdef EVALTREEFILEDEBUG 
		fileOutputEval(moveN_, ret);
#endif
		return ret;
		}
	std::vector<Move> legalMoves = listLegalMoves(onlyCaptures, moveOrdering);
#ifdef EVALTREEFILEDEBUG 
	fileOutputLegalMoveCount(moveN_, legalMoves.size());
#endif
	if (legalMoves.size() == 0 && !onlyCaptures) { //either checkmate or stalemate, once check is implemented
		if (false) { //TODO:CHANGE TO isCheck!!!!
#ifdef EVALTREEFILEDEBUG 
			fileOutputEval(moveN_, boolnegate(20000, !turn_));
#endif
			return boolnegate(20000, !turn_);
	}
		else {
#ifdef EVALTREEFILEDEBUG 
			fileOutputEval(moveN_, 0);
#endif
			return 0;
	}
}
	ret = boolnegate(20000, turn_); //AAH! HUGE BUG!! THIS CAUSED LIMIT WHEN I CHANGED EVALTYPE!!
	evaltype currenteval;
	for (size_t i = 0; i < legalMoves.size(); i++) {
		if (board_.getSquare(legalMoves[i].getDest()).getNeutralPiece() == K) {
#ifdef EVALTREEFILEDEBUG 
			fileOutputEval(moveN_, boolnegate(20000, turn_));
#endif
			return boolnegate(20000, turn_);
	}
		move(legalMoves[i], true);
		bool shouldStore = false; //some awkward logic here. shouldStore is a flag that tells the program whether to retrieve transposition data or submit it. However, with doStoreTP or doTPCheck, the logic gets circumvented
		if (!doTranspositionCheck || TT.shouldEvaluate(currentHash_, depth)) {
			shouldStore = true;
		}
		else {
			currenteval = TT.getEval(currentHash_);
			debugPosEvalTranspositionCount++;
		}
		if (shouldStore) {
			if (anyPrune && i > 0/*To avoid passing the initial ret value*/) {
				currenteval = evaluate(depth - 1, TT, onlyCaptures, doAfterMath, true, moveOrdering, true, ret, doTranspositionCheck, doStoreTranspositions); //BUG FOUND!! Missing bool doPrune argument caused odd behavior
			}
			else {
				currenteval = evaluate(depth - 1, TT, onlyCaptures, doAfterMath, anyPrune, moveOrdering, false, 0, doTranspositionCheck, doStoreTranspositions);
			}
			if (doStoreTranspositions) {
				TT.submit(currentHash_, depth, currenteval);
			}
		}
		if (turncompare(turn_, ret, currenteval)) { ret = currenteval; } //comparison is inverted because we moved
		/*if (advantage_.isDebugProblem()) {
			std::cout << "bug move!\n";
		}*/
		rewind();
		if (anyPrune && doPrune) if ((turn_ && currenteval > prunelimit) || (!turn_ && currenteval < prunelimit)) return currenteval;//turns are not inverted because we rewound //we can just return the current evaluation because we know it will be beaten by another prune, thats the whole point of that algorithm
	}
	if (onlyCaptures) {
		evaltype passeval = heuristic(); //for checking if not capturing is better--considers just not playing a move assuming there are other moves, and this may fail in situations of zugzwang but of course this is not a full search anyway just a capture one
		if (turncompare(turn_, passeval, ret)) {
			ret = passeval;
		}
	}
	if (legalMoves.size() == 0) {
		evaltype ret = heuristic();
		debugPosEvalCount++; //for position evaluation counting
#ifdef EVALTREEFILEDEBUG 
		fileOutputEval(moveN_, ret);
#endif
		return ret;
	}
#ifdef EVALTREEFILEDEBUG 
	fileOutputEval(moveN_, ret);
#endif
	return ret;
	}

evaltype Game::evaluate(depthtype depth, Move& bestmove, TranspositionTable& TT, bool onlyCaptures, bool doAfterMath, bool anyPrune, bool moveOrdering, bool doPrune, evaltype prunelimit, bool doTranspositionCheck, bool doStoreTranspositions) { //shouldnt be used for pruning and onlyCaptures, but will still implement
	evaltype ret;
	if (depth == 0) {
		if (doAfterMath && !onlyCaptures) {
			ret = evaluate(AFTERMATHPLYCAP, TT, true, true, anyPrune, moveOrdering, 0, 0, doTranspositionCheck, doStoreTranspositions);//doAfterMath is true but there wont be any more aftermath because this already is the aftermath
		}
		else {
			ret = heuristic();
			debugPosEvalCount++; //for position evaluation counting
		}
#ifdef EVALTREEFILEDEBUG 
		fileOutputEval(moveN_, ret);
#endif
		return ret; //wont return bestmove because it didnt search
		}
	std::vector<Move> legalMoves = listLegalMoves(onlyCaptures, moveOrdering);
#ifdef EVALTREEFILEDEBUG 
	fileOutputLegalMoveCount(moveN_, legalMoves.size());
#endif
	if (legalMoves.size() == 0 && !onlyCaptures) { //either checkmate or stalemate, once check is implemented
		if (false) { //TODO:CHANGE TO isCheck!!!!
#ifdef EVALTREEFILEDEBUG 
			fileOutputEval(moveN_, boolnegate(20000, !turn_));
#endif
			return boolnegate(20000, !turn_);
	}
		else {
#ifdef EVALTREEFILEDEBUG 
			fileOutputEval(moveN_, 0);
#endif
			return 0;
	}
}
	ret = boolnegate(20000, turn_);
	size_t besti = 0;
	evaltype currenteval;
	for (size_t i = 0; i < legalMoves.size(); i++) {
		if (board_.getSquare(legalMoves[i].getDest()).getNeutralPiece() == K) {
#ifdef EVALTREEFILEDEBUG 
			fileOutputEval(moveN_, boolnegate(20000, turn_));
#endif
			return boolnegate(20000, turn_);
	}
		move(legalMoves[i], true);
		bool shouldStore = false; //some awkward logic here. shouldStore is a flag that tells the program whether to retrieve transposition data or submit it. However, with doStoreTP or doTPCheck, the logic gets circumvented
		if (!doTranspositionCheck || TT.shouldEvaluate(currentHash_, depth)) {
			shouldStore = true;
		}
		else {
			currenteval = TT.getEval(currentHash_);
			debugPosEvalTranspositionCount++;
		}
		if (shouldStore) {
			if (anyPrune && i > 0) {
				currenteval = evaluate(depth - 1, TT, onlyCaptures, doAfterMath, true, moveOrdering, true, ret, doTranspositionCheck, doStoreTranspositions);
			}
			else {
				currenteval = evaluate(depth - 1, TT, onlyCaptures, doAfterMath, anyPrune, moveOrdering, false, 0, doTranspositionCheck, doStoreTranspositions);
			}
			if (doStoreTranspositions) {
				TT.submit(currentHash_, depth, currenteval);
			}
		}
		if (turncompare(turn_, ret, currenteval)) { ret = currenteval; besti = i; } //comparison is inverted because we moved
		rewind();
		if (anyPrune && doPrune) if ((turn_ && currenteval > prunelimit) || (!turn_ && currenteval < prunelimit)) return currenteval; //turns are not inverted because we rewound
	}
	if (onlyCaptures) {
		evaltype passeval = heuristic(); //for checking if not capturing is better--considers just not playing a move assuming there are other moves, and this may fail in situations of zugzwang but of course this is not a full search anyway just a capture one
		if (turncompare(turn_, passeval, ret)) {
			ret = passeval;
		}
	}
	if (legalMoves.size() == 0) {
		evaltype ret = heuristic();
		debugPosEvalCount++; //for position evaluation counting
#ifdef EVALTREEFILEDEBUG 
		fileOutputEval(moveN_, ret);
#endif
		return ret;
	}
	bestmove = legalMoves[besti];
#ifdef EVALTREEFILEDEBUG 
	fileOutputEval(moveN_, ret);
#endif
	return ret;
	}

std::vector<Move> Game::listLegalMoves(bool onlyCaptures, bool moveOrdering) const
{
	//TODO: Finish
	std::vector<Move> movelist;
	std::vector<Piece> capturelist;
	movelist.reserve(MOSTPOSSIBLELEGAL);
	capturelist.reserve(MOSTPOSSIBLELEGAL);
	for (squaretype row = 0; row < 8; row++) {
		for (squaretype col = 0; col < 8; col++) {
			//TODO add enpassant
			Piece currentpiece = board_.getSquare(row, col);
			if (!currentpiece.getPiece() || currentpiece.getColor() != turn_) continue;
			switch (currentpiece.getNeutralPiece()) {
			case P: {
				if (row > 0 && turn_ || row < 7 && !turn_) {
					if (!board_.getSquare(row + boolnegate(1, turn_), col).getPiece()) {
						movelist.push_back(Move(row, col, row + boolnegate(1, turn_), col));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						if (row == (1 + 5 * turn_) && !board_.getSquare(row + boolnegate(2, turn_), col).getPiece()) {
							movelist.push_back(Move(row, col, row + boolnegate(2, turn_), col));
							capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						}
					}
				}
				if (col < 7 && (row > 0 && turn_ || row < 7 && !turn_)) {
					if (board_.getSquare(row + boolnegate(1, turn_), col + 1).getColor() != turn_ && board_.getSquare(row + boolnegate(1, turn_), col + 1).getPiece()) {
						movelist.push_back(Move(row, col, row + boolnegate(1, turn_), col + 1));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
					}
				}
				if (col > 0 && (row > 0 && turn_ || row < 7 && !turn_)) {
					if (board_.getSquare(row + boolnegate(1, turn_), col - 1).getColor() != turn_ && board_.getSquare(row + boolnegate(1, turn_), col - 1).getPiece()) {
						movelist.push_back(Move(row, col, row + boolnegate(1, turn_), col - 1));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
					}
				}
				break;
			}
			case N: {
				squaretype spaces[8][2] = { {1,2},{2,1},{1,-2},{-2,1},{-1,2},{2,-1},{-1,-2},{-2,-1} };
				for (size_t i = 0; i < 8; i++) {
					if (inrange(row + spaces[i][0], 0, 7) && inrange(col + spaces[i][1], 0, 7)) {//if the knight can actually jump to that square
						if (!(board_.getSquare(row + spaces[i][0], col + spaces[i][1]).getColor() == turn_ && board_.getSquare(row + spaces[i][0], col + spaces[i][1]).getPiece())) { // if there is no friendly piece occupying that square
							movelist.push_back(Move(row, col, row + spaces[i][0], col + spaces[i][1]));
							capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						}
					}
				}
				break;
			}
			case R: {
				for (squaretype destrow = row + 1; destrow < 8; destrow++) {
					if ((board_.getSquare(destrow, col).getColor() == turn_ && board_.getSquare(destrow, col).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, col).getColor() != turn_ && board_.getSquare(destrow, col).getPiece())) {
						movelist.push_back(Move(row, col, destrow, col));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, col));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row - 1; destrow >= 0; destrow--) {
					if ((board_.getSquare(destrow, col).getColor() == turn_ && board_.getSquare(destrow, col).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, col).getColor() != turn_ && board_.getSquare(destrow, col).getPiece())) {
						movelist.push_back(Move(row, col, destrow, col));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, col));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destcol = col + 1; destcol < 8; destcol++) {
					if ((board_.getSquare(row, destcol).getColor() == turn_ && board_.getSquare(row, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(row, destcol).getColor() != turn_ && board_.getSquare(row, destcol).getPiece())) {
						movelist.push_back(Move(row, col, row, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, row, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destcol = col - 1; destcol >= 0; destcol--) {
					if ((board_.getSquare(row, destcol).getColor() == turn_ && board_.getSquare(row, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(row, destcol).getColor() != turn_ && board_.getSquare(row, destcol).getPiece())) {
						movelist.push_back(Move(row, col, row, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, row, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				break;
			}
			case B: {
				squaretype destcol;
				for (squaretype destrow = row + 1 + 0 * (destcol = col + 1); destrow < 8 && destcol < 8; destrow += 1 + 0 * (destcol++)) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row - 1 + 0 * (destcol = col + 1); destrow >= 0 && destcol < 8; destrow -= (1 + 0 * (destcol++))) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row + 1 + 0 * (destcol = col - 1); destrow < 8 && destcol >= 0; destrow += 1 + 0 * (destcol--)) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row - 1 + 0 * (destcol = col - 1); destrow >= 0 && destcol >= 0; destrow -= 1 + 0 * (destcol--)) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				break;
			}
			case Q: {
				for (squaretype destrow = row + 1; destrow < 8; destrow++) {
					if ((board_.getSquare(destrow, col).getColor() == turn_ && board_.getSquare(destrow, col).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, col).getColor() != turn_ && board_.getSquare(destrow, col).getPiece())) {
						movelist.push_back(Move(row, col, destrow, col));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, col));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row - 1; destrow >= 0; destrow--) {
					if ((board_.getSquare(destrow, col).getColor() == turn_ && board_.getSquare(destrow, col).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, col).getColor() != turn_ && board_.getSquare(destrow, col).getPiece())) {
						movelist.push_back(Move(row, col, destrow, col));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, col));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destcol = col + 1; destcol < 8; destcol++) {
					if ((board_.getSquare(row, destcol).getColor() == turn_ && board_.getSquare(row, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(row, destcol).getColor() != turn_ && board_.getSquare(row, destcol).getPiece())) {
						movelist.push_back(Move(row, col, row, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, row, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destcol = col - 1; destcol >= 0; destcol--) {
					if ((board_.getSquare(row, destcol).getColor() == turn_ && board_.getSquare(row, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(row, destcol).getColor() != turn_ && board_.getSquare(row, destcol).getPiece())) {
						movelist.push_back(Move(row, col, row, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, row, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				squaretype destcol;
				for (squaretype destrow = row + 1 + 0 * (destcol = col + 1); destrow < 8 && destcol < 8; destrow += 1 + 0 * (destcol++)) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row - 1 + 0 * (destcol = col + 1); destrow >= 0 && destcol < 8; destrow -= 1 + 0 * (destcol++)) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row + 1 + 0 * (destcol = col - 1); destrow < 8 && destcol >= 0; destrow += 1 + 0 * (destcol--)) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				for (squaretype destrow = row - 1 + 0 * (destcol = col - 1); destrow >= 0 && destcol >= 0; destrow -= 1 + 0 * (destcol--)) {
					if ((board_.getSquare(destrow, destcol).getColor() == turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						break;
					}
					if ((board_.getSquare(destrow, destcol).getColor() != turn_ && board_.getSquare(destrow, destcol).getPiece())) {
						movelist.push_back(Move(row, col, destrow, destcol));
						capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						break;
					}
					movelist.push_back(Move(row, col, destrow, destcol));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				break;
			}
			case K: {
				squaretype spaces[8][2] = { {1,1},{1,0},{1,-1},{0,1},{0,-1},{-1,1},{-1,0},{-1,-1} };
				for (size_t i = 0; i < 8; i++) {
					if (inrange(row + spaces[i][0], 0, 7) && inrange(col + spaces[i][1], 0, 7)) {//if the king can actually get to that square
						if (!(board_.getSquare(row + spaces[i][0], col + spaces[i][1]).getColor() == turn_ && board_.getSquare(row + spaces[i][0], col + spaces[i][1]).getPiece())) { // if there is no friendly piece occupying that square
							movelist.push_back(Move(row, col, row + spaces[i][0], col + spaces[i][1]));
							capturelist.push_back(board_.getSquare(movelist.back().getDest()));
						}
					}
				}
				if (readyCastleKingside(turn_)) {
					movelist.push_back(Move(row, col, row, col + 2));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				if (readyCastleQueenside(turn_)) {
					movelist.push_back(Move(row, col, row, col - 2));
					capturelist.push_back(board_.getSquare(movelist.back().getDest()));
				}
				break;
			}
			}
		}
	}
	if (moveOrdering) { //moved this outside of for row-col loop -- that was a bug
		for (size_t i = 1; i < capturelist.size(); i++) {
			size_t j = i;
			while (j > 0 && piecescores[capturelist.begin()[j - 1].getPiece()] < piecescores[capturelist.begin()[j].getPiece()]) { //flipped sign because move ordering going wrong way
				std::swap(movelist[j], movelist[j - 1]);
				std::swap(capturelist[j], capturelist[j - 1]);
				j--;
			}
		}
	} //TODO: Optimize moveordering strategy

	if (onlyCaptures) {
		std::vector<Move> capturesList;
		for (size_t i = 0; i < movelist.size(); i++) {
			if (board_.getSquare(movelist[i].getDest()).getPiece()) {
				capturesList.push_back(movelist[i]);
			}
		}
		return capturesList;
	}//TODO: optimize onlyCaptures
	return movelist;
}

/*void Game::listLegalMoves(std::vector<Move>& dest, bool onlyCaptures) const {*/ // seems that pass by pointer... slows down?? maybe /*//TODO*/ should try to use pointer not reference later
