//heuristic function to-do's: TODO
//May need to optimize by making pawns "remember" their defenders or who they're defending (putting pieces on defended squares adds bonuses for the pawns defending it) 
//Make sure captured pawns are not still recieving double pawn bonuses DONE CANT REMEMBER?
//May need to optimize by not adding stages to crowding king
//back-rank mate stopper
//castling CHECK! and enpassant for fence, doubled pawns, king safety, and positional bonuses
evaltype Advantage::getAdvantage() const {
	squaretype stage = BONUSCALC::stage(whiteMaterial_, blackMaterial_);
	evaltype ret = whiteMaterial_ - blackMaterial_ + whiteDoublePawnBonus_ + blackDoublePawnBonus_;
	if (stage == 1) {
		ret += white1PlacementBonus_ - black1PlacementBonus_;
		ret += white1KingSafetyBonus_ - black1KingSafetyBonus_;
	}
	else if (stage == 2) {
		ret += white2PlacementBonus_ - black2PlacementBonus_;
		ret += white2KingSafetyBonus_ - black2KingSafetyBonus_;
		ret += whiteFenceBonus_ - blackFenceBonus_;
	}
	else if (stage == 3) {
		ret += white3PlacementBonus_ - black3PlacementBonus_;
		ret += white3KingSafetyBonus_ - black3KingSafetyBonus_;
		ret += whiteFenceBonus_ - blackFenceBonus_;
	}
	return ret;
}
void Advantage::updateAdvantage(const Move& move, const Game& game) {
	bool color = game.getSquare(move.getSource()).getColor();
	//material update
	if (game.getSquare(move.getDest()).getPiece()) {
		if (color == false) {//Opposite color here because can only capture opposite color
			whiteMaterial_ -= piecescores[game.getSquare(move.getDest()).getPiece()];
		}
		if (color == true) {//Opposite color here because can only capture opposite color
			blackMaterial_ -= piecescores[game.getSquare(move.getDest()).getPiece()];
		}
	}
	//pawn fence update
	int direction = (color ? -1 : 1);
	if (game.getSquare(move.getSource()).getNeutralPiece() == P) {
		//old fence eval
		if (inrange(move.getSourceRow() + direction, 0, 7)) {
			if (move.getSourceCol() > 0) {
				Piece defending = game.getSquare(move.getSourceRow() + direction, move.getSourceCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
			if (move.getSourceCol() < 7) {
				Piece defending = game.getSquare(move.getSourceRow() + direction, move.getSourceCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
		}

		//new fence eval
		if (inrange(move.getDestRow() + direction, 0, 7)) {
			if (move.getDestCol() > 0) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
		}
	}
	//Pieces going onto defended sqaures
	if (game.getSquare(move.getSource()).getNeutralPiece()) {
		//old fence eval
		if (inrange(move.getSourceRow() - direction, 0, 7)) {
			Piece me = game.getSquare(move.getSourceRow(), move.getSourceCol());
			if (move.getSourceCol() > 0) {
				Piece defending = game.getSquare(move.getSourceRow() - direction, move.getSourceCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) {
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
			if (move.getSourceCol() < 7) {
				Piece defending = game.getSquare(move.getSourceRow() - direction, move.getSourceCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) {
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
		}

		//new fence eval
		if (inrange(move.getDestRow() - direction, 0, 7)) {
			Piece me = game.getSquare(move.getDestRow(), move.getDestCol());
			if (move.getDestCol() > 0 && !(move.getDestCol() - 1 == move.getSourceCol() && move.getDestRow() - direction == move.getSourceRow())) { //added conditional to prevent engine thinking that moving a pawn onto a square that it was previously defending was defended, same with other similar conditionals
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7 && !(move.getDestCol() + 1 == move.getSourceCol() && move.getDestRow() - direction == move.getSourceRow())) {
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
		}
	}
	direction *= -1;
	//when a piece is captured, the defending has to be reevaluated.
	if (game.getSquare(move.getDest()).getNeutralPiece() == P) {
		if (inrange(move.getDestRow() + direction, 0, 7)) {
			if (move.getDestCol() > 0) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() - 1);
				if (defending.getColor() != color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug //all color checks are now inverted because this old eval is for the case of a piece being taken
					if (color != true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() + 1);
				if (defending.getColor() != color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color != true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
		}
	}
	//Pieces going onto defending squares
	if (game.getSquare(move.getDest()).getNeutralPiece()) {
		//old fence eval
		if (inrange(move.getDestRow() - direction, 0, 7)) {
			Piece me = game.getSquare(move.getDestRow(), move.getDestCol());
			if (move.getDestCol() > 0) {
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() - 1);
				if (defending.getColor() != color && defending.getNeutralPiece() == P) {
					if (color != true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7) {
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() + 1);
				if (defending.getColor() != color && defending.getNeutralPiece() == P) {
					if (color != true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
		}
	}
	direction *= -1; //revert, even if not later used
	//doubled pawn fence update
	if (game.getSquare(move.getSource()).getNeutralPiece() == P) {
		if (move.getSourceCol() != move.getDestCol()) {
			if (color == true) {
				whiteDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getSourceCol()]]; //removing score from previous penalty (except opposite, this is from source) //GLITCH!! WE SHOULDNT HAVE OPPOSITE OPERATORS FOR THE SOURCE AND DEST!! Changing.
				whiteDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //removing score from previous penalty
				whitePawns_[move.getSourceCol()]--;
				whitePawns_[move.getDestCol()]++;
				whiteDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getSourceCol()]]; //adding score from new penalty (except opposite, this is from source)
				whiteDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //adding score from new penalty

			}
			else {
				blackDoublePawnBonus_ -= PAWNDOUBLEDSCORE[blackPawns_[move.getSourceCol()]]; //SAME THING BUT BLACK
				blackDoublePawnBonus_ -= PAWNDOUBLEDSCORE[blackPawns_[move.getDestCol()]];
				blackPawns_[move.getSourceCol()]--;
				blackPawns_[move.getDestCol()]++;
				blackDoublePawnBonus_ += PAWNDOUBLEDSCORE[blackPawns_[move.getSourceCol()]];
				blackDoublePawnBonus_ += PAWNDOUBLEDSCORE[blackPawns_[move.getDestCol()]];
			}
		}
	}
	//Captured pawns here
	if (game.getSquare(move.getDest()).getNeutralPiece() == P) {
		if (color != true) { //inverting color because opposite color is doing the capturING
			whiteDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //removing score from previous penalty
			whitePawns_[move.getDestCol()]--;
			whiteDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //adding score from new penalty

		}
		else {
			blackDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //SAME THING BUT BLACK
			blackPawns_[move.getDestCol()]--;
			blackDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]];
		}
	}
	//piece positional update
	bool isCastleQueen = ((game.getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 2));
	bool isCastleKing = ((game.getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 6));
	if (color == true) {
		white1PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		white2PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);
		white3PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1PlacementBonus_ += PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		white2PlacementBonus_ += PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		white3PlacementBonus_ += PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		black2PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		black3PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			white1PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			white3PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			white1PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			white3PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			white1PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			white3PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			white1PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			white3PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
	if (color == false) {
		black1PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		black2PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1PlacementBonus_ += PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		black2PlacementBonus_ += PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3PlacementBonus_ += PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		white2PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		white3PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			black1PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			black3PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			black1PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			black3PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			black1PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			black3PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			black1PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			black3PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
	//king safety update
	//THERE IS A BUG: when the king moves, previous bonuses will not be reevaluated. However, this is okay; the engine will notice that moving these pieces is no longer bad and that they will naturally form back around the king. Even if high bonuses get occasionally racked up by the king repeatedly moving then pieces moving too, this shouldn't affect the way the engine plays but may result in a false advantage for someone.
	if (color == true) {
		white1KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		white2KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);/* = white1KingSafetyBonus_;*/ //middlegame same as opening optimizable
		white3KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		white2KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		white3KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		black2KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		black3KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			white1KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			white3KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			white1KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			white3KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			white1KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			white3KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			white1KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			white3KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
	if (color == false) {
		black1KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		black2KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		black2KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		white2KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		white3KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			black1KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			black3KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			black1KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			black3KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			black1KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			black3KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			black1KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			black3KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
}
void Advantage::downdateAdvantage(const Move& move, const Game& game) {
	bool color = game.getSquare(move.getSource()).getColor();
	//material downdate
	if (game.getSquare(move.getDest()).getPiece()) {
		if (color == false) {//Opposite color here because can only capture opposite color
			whiteMaterial_ += piecescores[game.getSquare(move.getDest()).getPiece()];
		}
		if (color == true) {//Opposite color here because can only capture opposite color
			blackMaterial_ += piecescores[game.getSquare(move.getDest()).getPiece()];
		}
	}
	//pawn fence downdate
	int direction = (color ? -1 : 1);
	if (game.getSquare(move.getSource()).getNeutralPiece() == P) {
		//old fence eval (technically opposite because rewinding)
		if (inrange(move.getSourceRow() + direction, 0, 7)) {
			if (move.getSourceCol() > 0) {
				Piece defending = game.getSquare(move.getSourceRow() + direction, move.getSourceCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
			if (move.getSourceCol() < 7) {
				Piece defending = game.getSquare(move.getSourceRow() + direction, move.getSourceCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
		}

		//new fence eval (technically opposite because rewinding)
		if (inrange(move.getDestRow() + direction, 0, 7)) {
			if (move.getDestCol() > 0) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
		}
	}
	//Pieces going onto defended sqaures
	if (game.getSquare(move.getSource()).getNeutralPiece()) {
		//old fence eval (technically opposite because rewinding)
		if (inrange(move.getSourceRow() - direction, 0, 7)) {
			Piece me = game.getSquare(move.getSourceRow(), move.getSourceCol());
			if (move.getSourceCol() > 0/* && !(move.getDestCol() - 1 == move.getSourceCol() && move.getDestRow() - direction == move.getSourceRow())*/) { //Commented extra clause out; was inconsistent with updateAdvantage() and was also causing bugs because the same bug with pieces defending themselves doesn't happen with old evals 
				Piece defending = game.getSquare(move.getSourceRow() - direction, move.getSourceCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) {
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
			if (move.getSourceCol() < 7/* && !(move.getDestCol() + 1 == move.getSourceCol() && move.getDestRow() - direction == move.getSourceRow())*/) {
				Piece defending = game.getSquare(move.getSourceRow() - direction, move.getSourceCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) {
					if (color == true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
		}

		//new fence eval (technically opposite because rewinding)
		if (inrange(move.getDestRow() - direction, 0, 7)) {
			Piece me = game.getSquare(move.getDestRow(), move.getDestCol());
			if (move.getDestCol() > 0 && !(move.getDestCol() - 1 == move.getSourceCol() && move.getDestRow() - direction == move.getSourceRow())) { //ADDED CONDITIONAL CLAUSE, CHECK COMMENT IN UPDATEADVANTAGE()
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() - 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7 && !(move.getDestCol() + 1 == move.getSourceCol() && move.getDestRow() - direction == move.getSourceRow())) {
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() + 1);
				if (defending.getColor() == color && defending.getNeutralPiece() == P) { //last part of conditional to prevent empty-square color bug
					if (color == true) {
						whiteFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color == false) {
						blackFenceBonus_ -= PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
		}
	}
	direction *= -1;
	//when a piece is captured, the defending has to be reevaluated.
	if (game.getSquare(move.getDest()).getNeutralPiece() == P) {
		if (inrange(move.getDestRow() + direction, 0, 7)) {
			if (move.getDestCol() > 0) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() - 1);
				if (defending.getColor() != color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug //all color checks are now inverted because this old eval is for the case of a piece being taken
					if (color != true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7) {
				Piece defending = game.getSquare(move.getDestRow() + direction, move.getDestCol() + 1);
				if (defending.getColor() != color && defending.getNeutralPiece()) { //last part of conditional to prevent empty-square color bug
					if (color != true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[defending.getNeutralPiece()];
					}
				}
			}
		}
	}
	//Pieces going onto defending squares
	if (game.getSquare(move.getDest()).getNeutralPiece()) {
		//old fence eval (technically opposite because rewinding)
		if (inrange(move.getDestRow() - direction, 0, 7)) {
			Piece me = game.getSquare(move.getDestRow(), move.getDestCol());
			if (move.getDestCol() > 0) {
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() - 1);
				if (defending.getColor() != color && defending.getNeutralPiece() == P) {
					if (color != true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
			if (move.getDestCol() < 7) {
				Piece defending = game.getSquare(move.getDestRow() - direction, move.getDestCol() + 1);
				if (defending.getColor() != color && defending.getNeutralPiece() == P) {
					if (color != true) {
						whiteFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
					if (color != false) {
						blackFenceBonus_ += PAWNDEFENSESCORE[me.getNeutralPiece()];
					}
				}
			}
		}
	}
	direction *= -1; //revert, even if not later used
	//doubled pawn fence downdate
	if (game.getSquare(move.getSource()).getNeutralPiece() == P) {
		if (move.getSourceCol() != move.getDestCol()) {
			if (color == true) {
				whiteDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getSourceCol()]]; //removing score from previous penalty (except opposite, this is from source) //GLITCH!! WE SHOULDNT HAVE OPPOSITE OPERATORS FOR THE SOURCE AND DEST!! Changing. //Glitch! We shouldn't invert the operators for rewind, just the pawn increments
				whiteDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //removing score from previous penalty
				whitePawns_[move.getSourceCol()]++; //AAAAH! THE BUG!! Swapped around and fixed.
				whitePawns_[move.getDestCol()]--;
				whiteDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getSourceCol()]]; //adding score from new penalty (except opposite, this is from source)
				whiteDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //adding score from new penalty

			}
			else {
				blackDoublePawnBonus_ -= PAWNDOUBLEDSCORE[blackPawns_[move.getSourceCol()]]; //SAME THING BUT BLACK
				blackDoublePawnBonus_ -= PAWNDOUBLEDSCORE[blackPawns_[move.getDestCol()]];
				blackPawns_[move.getSourceCol()]++;
				blackPawns_[move.getDestCol()]--;
				blackDoublePawnBonus_ += PAWNDOUBLEDSCORE[blackPawns_[move.getSourceCol()]];
				blackDoublePawnBonus_ += PAWNDOUBLEDSCORE[blackPawns_[move.getDestCol()]];
			}
		}
	}
	//Captured pawns here
	if (game.getSquare(move.getDest()).getNeutralPiece() == P) {
		if (color != true) { //inverting color because opposite color is doing the capturING
			whiteDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //removing score from previous penalty
			whitePawns_[move.getDestCol()]++;
			whiteDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //adding score from new penalty

		}
		else {
			blackDoublePawnBonus_ -= PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]]; //SAME THING BUT BLACK
			blackPawns_[move.getDestCol()]++;
			blackDoublePawnBonus_ += PAWNDOUBLEDSCORE[whitePawns_[move.getDestCol()]];
		}
	}
	//piece positional downdate
	bool isCastleQueen = ((game.getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 2));
	bool isCastleKing = ((game.getSquare(move.getSource()).getNeutralPiece() == K) && (move.getSourceCol() == 4) && (move.getDestCol() == 6));
	if (color == true) {
		white1PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		white2PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);
		white3PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		white2PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		white3PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1PlacementBonus_ += PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		black2PlacementBonus_ += PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		black3PlacementBonus_ += PLACEMENT::squarebonus(true, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			white1PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			white3PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			white1PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			white3PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			white1PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			white3PlacementBonus_ += PLACEMENT::squarebonus(true, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			white1PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			white3PlacementBonus_ -= PLACEMENT::squarebonus(true, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
	if (color == false) {
		black1PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		black2PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		black2PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1PlacementBonus_ += PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		white2PlacementBonus_ += PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		white3PlacementBonus_ += PLACEMENT::squarebonus(false, move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			black1PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			black3PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			black1PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			black3PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			black1PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			black3PlacementBonus_ += PLACEMENT::squarebonus(false, move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			black1PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			black3PlacementBonus_ -= PLACEMENT::squarebonus(false, move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
	//king safety downdate
	if (color == true) {
		white1KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		white2KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);/* = white1KingSafetyBonus_;*/ //middlegame same as opening optimizable
		white3KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		white2KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		white3KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		black2KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		black3KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			white1KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			white3KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			white1KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			white3KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			white1KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			white3KingSafetyBonus_ += KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			white1KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			white2KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			white3KingSafetyBonus_ -= KINGSAFETY::squarebonus(true, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
	if (color == false) {
		black1KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 1); //old eval
		black2KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), move.getSourceCol(), game.getSquare(move.getSource()).getPiece(), 3);
		black1KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 1); //new eval
		black2KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 2);
		black3KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getSource()).getPiece(), 3);
		white1KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 1); //captured piece
		white2KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 2);
		white3KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getDestRow(), move.getDestCol(), game.getSquare(move.getDest()).getPiece(), 3);
		if (isCastleQueen) { //castling moves rook
			black1KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 2);
			black3KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 0, game.getSquare(multindex(move.getSourceRow(), 0)).getPiece(), 3);
			black1KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 2);
			black3KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 3, game.getSquare(multindex(move.getSourceRow(), 3)).getPiece(), 3);
		}
		if (isCastleKing) {
			black1KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 2);
			black3KingSafetyBonus_ += KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 7, game.getSquare(multindex(move.getSourceRow(), 7)).getPiece(), 3);
			black1KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 1); //0 is where the rook is before castling queenside //piece could be changed to just a rook because thats how castling should always work
			black2KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 2);
			black3KingSafetyBonus_ -= KINGSAFETY::squarebonus(false, game.kingLocation(color), move.getSourceRow(), 5, game.getSquare(multindex(move.getSourceRow(), 5)).getPiece(), 3);
		}
	}
}
