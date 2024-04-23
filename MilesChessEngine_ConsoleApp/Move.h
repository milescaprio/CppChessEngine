#pragma once
class Move {
private:
	squaretype sourceRow_;
	squaretype sourceCol_;
	squaretype destRow_;
	squaretype destCol_;
public:
	Move(squaretype sourceRow, squaretype sourceCol, squaretype destRow, squaretype destCol);
	Move() : Move(0, 0, 0, 0) {};
	squaretype getSourceRow() const { return sourceRow_; }
	squaretype getSourceCol() const { return sourceCol_; }
	squaretype getSource() const { return multindex(sourceRow_, sourceCol_); }
	squaretype getDestRow() const { return destRow_; }
	squaretype getDestCol() const { return destCol_; }
	squaretype getDest() const { return multindex(destRow_, destCol_); }
	squaretype operator[](size_t index) const {
		switch (index) {
		case 0: return sourceRow_; break;
		case 1: return sourceCol_; break;
		case 2: return destRow_; break;
		case 3: return destCol_; break;
		default: throw("Move element out of range");
		}
	}
};
