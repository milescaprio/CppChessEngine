#pragma once
#define ENTRIES 16777216

struct TranspositionEntry {
	hashtype hash;
	evaltype eval;
	depthtype depth;
	bool inuse = false;
	bool pad; //pad would be used anyway
	TranspositionEntry(int hash_, int eval_, int depth_) {
		hash = hash_;
		eval = eval_;
		depth = depth_;
		inuse = true;
	}
	TranspositionEntry() {
		inuse = false;
	}
};

class TranspositionTable {
private:
	TranspositionEntry* data_;
public:
	TranspositionTable() {
		data_ = new TranspositionEntry[ENTRIES];
	}
	~TranspositionTable() {
		delete[] data_;
	}
	void submit(hashtype hash, depthtype depth, evaltype eval) {
		size_t index = hash % ENTRIES;
		if (data_[index].inuse == false) {
			//submitting transposition to empty slot
			data_[index].inuse = true;
			data_[index].hash = hash;
			data_[index].depth = depth;
			data_[index].eval = eval;
			return;
		}
		if (hash != data_[index].hash) {
			//inuse must be true because would have already returned if not, replacing old data because of ALWAYS REPLACE method
			data_[index].hash = hash;
			data_[index].depth = depth;
			data_[index].eval = eval;
			return;
		}
		else { //position already found (or collision)
			if (depth > data_[index].depth) { //updating with higher depth search
				data_[index].depth = depth;
				data_[index].eval = eval;
				return;
			}
			else { //data already exists in transposition table so no need to update
				return;
			}
		}
	}
	bool shouldEvaluate(hashtype hash, depthtype depth) {
		size_t index = hash % ENTRIES;
		if (data_[index].inuse == false) {
			return 1;
		}
		if (hash != data_[index].hash) { //needs to replace hash
			return 1;
		}
		else { //position already found (or collision)
			if (depth > data_[index].depth) { //needs higher search
				return 1;
			}
			else { //data already exists in transposition table so no need to evaluate
				return 0;
			}
		}
	}
	evaltype getEval(hashtype hash) {
		size_t index = hash % ENTRIES;
		TranspositionEntry* DELETEMEAFTERDEBUG = &data_[index]; //todo delete
		if (!data_[index].inuse) {
			throw "Error, tried to recieve eval of nonexistent transposition";
		}
		return data_[index].eval;
	}

};
