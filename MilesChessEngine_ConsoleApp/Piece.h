#pragma once
class Piece {
private:
	pieceidtype pieceid_;
public:
	Piece(pieceidtype pieceid);
	Piece(const Piece& piece) = default;
	Piece();
	Piece& operator=(const Piece& piece) = default;
	Piece& operator=(pieceidtype piece) { pieceid_ = piece; return *this; }
	bool getColor(void) const { return pieceid_ <= 8; } //true or 1 == white, false or 0 == black
	pieceidtype getNeutralPiece() const { return pieceid_ % 8; }
	pieceidtype getPiece() const { return pieceid_; }
};
