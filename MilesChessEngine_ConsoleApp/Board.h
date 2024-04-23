#pragma once
class Board {
private:
	Piece pieces_[64];
	std::vector<bool> promotion_; /*adding REVERSE pawn promotion. Somehow this was not previously implemented.*/
public:
	Board();
	Board(const std::initializer_list<pieceidtype>& list);
	void move(const Move& move);
	void reverseMove(const Move& move, const Piece& captured);
	Piece getSquare(squaretype row, squaretype col) const {
#ifdef REWINDDEBUG
		if (!(inrange(row, 0, 7) && inrange(col, 0, 7))) throw ("Square Out Of Range"); //DEBUG
#endif
		return pieces_[multindex(row, col)];
	}
	Piece getSquare(squaretype mult) const {
		if (!(inrange(mult, 0, 63))) throw ("Square Out Of Range");
		return pieces_[mult];
	}
	bool operator==(const Board& otherboard) {
		for (int i = 0; i < 64; i++) {
			if (pieces_[i].getPiece() != otherboard.pieces_[i].getPiece()) {
				return false;
			}
		}
		return true;
	}
};
