#pragma once

#include <string>
#include <list>
#include <map>
#include <set>

#include "move.h"

class Book{
public:

	static bool ReadBook(bool trusted = false);

	static const Move * Get(Util::Zobrist::HashType h);

private:

	static void AddLine(const std::string & pgn, bool algAbr = false, bool trusted = false, std::ofstream * binFile = 0);
	static bool Add(const std::string & moveAlg, Position &p, bool algAbr = false, bool trusted = false, std::ofstream * binFile = 0);
	static bool ContainsMove(const Move & m, const Position & p);
    static void ReadBinaryBook(std::ifstream & stream, bool trusted = false);
	struct BookMove{
		Move m;
		Util::Zobrist::HashType h;
		inline bool operator<(const BookMove& m)const{
			return h < m.h;
		}
	};

	static std::map<Util::Zobrist::HashType, std::set<BookMove> > _book;
};

