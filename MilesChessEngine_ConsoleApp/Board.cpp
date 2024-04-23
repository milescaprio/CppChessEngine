void Board::move(const Move& move) {
	//ASSUMING LEGAL MOVE! Checked by outer shell Game.move() function
	//need to update checks
	//TODO: Optimize by overloading and pass these calculated booleans below
	bool color = pieces_[move.getSource()].getColor();
#ifdef ENPASSANT
	bool isPassantRow = ((move.getSourceRow() + boolnegate(1, color) == move.getDestRow()) && (getSquare(move.getSource()).getPiece() == (p - 8 * color)) && (getSquare(move.getSourceRow(), move.getDestCol()).getPiece() == (P + 8 * color)));
	bool isPassantLeft = isPassantRow && (move.getSourceCol() - 1 == move.getDestCol());
	bool isPassantRight = isPassantRow && (move.getSourceCol() + 1 == move.getDestCol());
#endif
	bool isCastleQueen = ((pieces_[move.getSource()].getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 2));
	bool isCastleKing = ((pieces_[move.getSource()].getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 6));
	bool isPromotion = ((pieces_[move.getSource()].getNeutralPiece() == P) && (move.getDest() == (color ? 0 : 7))); //adding pawn promotion. Somehow this was not previously implemented.
	promotion_.push_back(isPromotion);
	pieces_[move.getDest()] = pieces_[move.getSource()];
	pieces_[move.getSource()] = o;
	if (isPromotion) {
		pieces_[move.getDest()] = color ? Q : q;
	}
	if (isCastleQueen) {
		pieces_[multindex(move.getSourceRow(), 3)] = pieces_[multindex(move.getSourceRow(), 0)];
		pieces_[multindex(move.getSourceRow(), 0)] = o;
	}
	if (isCastleKing) {
		pieces_[multindex(move.getSourceRow(), 5)] = pieces_[multindex(move.getSourceRow(), 7)];
		pieces_[multindex(move.getSourceRow(), 7)] = o;
	}
#ifndef REWINDDEBUG
#ifdef ENPASSANT
	if (isPassantLeft) {
		pieces_[multindex(move.getSourceRow(), move.getSourceCol() - 1)] = o;
	}
	if (isPassantRight) {
		pieces_[multindex(move.getSourceRow(), move.getSourceCol() + 1)] = o;
	}
#endif
#endif
}

void Board::reverseMove(const Move& move, const Piece& captured) {
	//ASSUMING LEGAL MOVE! Checked by outer shell Game.move() function
	//need to update checks
	//TODO finish enpassant
	bool color = pieces_[move.getDest()].getColor();
#ifdef ENPASSANT
	/*bool isPassantLeft = ((move.getSourceRow() - 1 == move.getDestRow()) && (move.getSourceCol() - 1 == move.getDestCol()) && (pieces_[move.getSource()].getPiece() == P)) || \
		((move.getSourceRow() + 1 == move.getDestRow()) && (move.getSourceCol() - 1 == move.getDestCol()) && (pieces_[move.getSource()].getPiece() == p));
	bool isPassantRight = ((move.getSourceRow() - 1 == move.getDestRow()) && (move.getSourceCol() + 1 == move.getDestCol()) && (pieces_[move.getSource()].getPiece() == P)) || \
		((move.getSourceRow() + 1 == move.getDestRow()) && (move.getSourceCol() + 1 == move.getDestCol()) && (pieces_[move.getSource()].getPiece() == p));*/ //TODO finish enpassant
#endif
	bool isCastleQueen = ((pieces_[move.getDest()].getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 2));
	bool isCastleKing = ((pieces_[move.getDest()].getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 6));
#ifdef ENPASSANT
	/*if (isPassantLeft || isPassantRight) {
		if (color == false) {//Opposite color here because can only capture opposite color
			whiteMaterial_ -= pscore;
		}
		if (color == true) {//Opposite color here because can only capture opposite color
			blackMaterial_ -= pscore;
		}
	}*/
#endif
	pieces_[move.getSource()] = pieces_[move.getDest()];
	pieces_[move.getDest()] = captured;
	if (promotion_.back()) {
		pieces_[move.getSource()] = color ? P : p;
	}
	promotion_.pop_back();
	if (isCastleQueen) {
		pieces_[multindex(move.getSourceRow(), 0)] = pieces_[multindex(move.getSourceRow(), 3)]; //source is same on both sides because castling remains in one row
		pieces_[multindex(move.getSourceRow(), 3)] = o;
	}
	if (isCastleKing) {
		pieces_[multindex(move.getSourceRow(), 7)] = pieces_[multindex(move.getSourceRow(), 5)];
		pieces_[multindex(move.getSourceRow(), 5)] = o;
	}
	/*
	if (isPassantLeft) {
		pieces_[multindex(move.getSourceRow(), move.getSourceCol() - 1)] = o;
	}
	if (isPassantRight) {
		pieces_[multindex(move.getSourceRow(), move.getSourceCol() + 1)] = o;
	}*/
}

Board::Board(const std::initializer_list<pieceidtype>& list) {
	for (size_t i = 0; i < 64; i++) {
		pieces_[i] = list.begin()[i];
	}
}

Board::Board() {

}
