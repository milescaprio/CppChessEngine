#pragma once
class Game;
class Advantage {
private:
	evaltype whiteMaterial_ = CHESSMATERIAL;
	evaltype blackMaterial_ = CHESSMATERIAL;
	evaltype whiteFenceBonus_ = 0;
	evaltype blackFenceBonus_ = 0;
	evaltype whiteDoublePawnBonus_ = 0; //technically a penalty
	evaltype blackDoublePawnBonus_ = 0;
	evaltype white1PlacementBonus_ = 0; //for opening
	evaltype black1PlacementBonus_ = 0;
	evaltype white2PlacementBonus_ = 0; //for middlegame
	evaltype black2PlacementBonus_ = 0;
	evaltype white3PlacementBonus_ = 0; //for endgame
	evaltype black3PlacementBonus_ = 0;
	evaltype white1KingSafetyBonus_ = 0;
	evaltype black1KingSafetyBonus_ = 0;
	evaltype white2KingSafetyBonus_ = 0;
	evaltype black2KingSafetyBonus_ = 0;
	evaltype white3KingSafetyBonus_ = 0;
	evaltype black3KingSafetyBonus_ = 0;
	squaretype whitePawns_[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
	squaretype blackPawns_[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
public:
	evaltype getAdvantage() const;
	void listAdvantages() const {
		std::cout << "whiteMaterial:" << whiteMaterial_ << '\n';
		std::cout << "blackMaterial:" << blackMaterial_ << '\n';
		std::cout << "whiteFenceBonus:" << whiteFenceBonus_ << '\n';
		std::cout << "blackFenceBonus:" << blackFenceBonus_ << '\n';
		std::cout << "whiteDoublePawnBonus:" << whiteDoublePawnBonus_ << '\n';
		std::cout << "blackDoublePawnBonus:" << blackDoublePawnBonus_ << '\n';
		std::cout << "white1KingSafetyBonus:" << white1KingSafetyBonus_ << '\n';
		std::cout << "black1KingSafetyBonus:" << black1KingSafetyBonus_ << '\n';
		std::cout << "white2KingSafetyBonus:" << white2KingSafetyBonus_ << '\n';
		std::cout << "black2KingSafetyBonus:" << black2KingSafetyBonus_ << '\n';
		std::cout << "white3KingSafetyBonus:" << white3KingSafetyBonus_ << '\n';
		std::cout << "black3KingSafetyBonus:" << black3KingSafetyBonus_ << '\n';
		std::cout << "white1PlacementBonus:" << white1PlacementBonus_ << '\n';
		std::cout << "black1PlacementBonus:" << black1PlacementBonus_ << '\n';
		std::cout << "white2PlacementBonus:" << white2PlacementBonus_ << '\n';
		std::cout << "black2PlacementBonus:" << black2PlacementBonus_ << '\n';
		std::cout << "white3PlacementBonus:" << white3PlacementBonus_ << '\n';
		std::cout << "black3PlacementBonus:" << black3PlacementBonus_ << '\n';
		std::cout << "Overall Advantage:" << getAdvantage() << '\n';
	}
	evaltype _quickdebugWhite() const {
		return getAdvantage();
	}
	evaltype _quickdebugBlack() const {
		return getAdvantage();
	}
	void updateAdvantage(const Move& move, const Game& game); // TODO change back to const Game, there was a bug
	void downdateAdvantage(const Move& move, const Game& game);
	evaltype whiteMaterial(void) const { return whiteMaterial_; }
	evaltype blackMaterial(void) const { return blackMaterial_; }
};
