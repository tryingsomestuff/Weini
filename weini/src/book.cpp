#include "book.h"
#include "position.h"

#include <fstream>
#include <random>
#include <iterator>

std::map<Util::Zobrist::HashType, std::set<Book::BookMove> > Book::_book;

namespace {
    //struct to hold the value:
    template<typename T> struct bits_t { T t; }; //no constructor necessary
                                                 //functions to infer type, construct bits_t with a member initialization list
                                                 //use a reference to avoid copying. The non-const version lets us extract too
    template<typename T> bits_t<T&> bits(T &t) { return bits_t<T&>{t}; }
    template<typename T> bits_t<const T&> bits(const T& t) { return bits_t<const T&>{t}; }
    //insertion operator to call ::write() on whatever type of stream
    template<typename S, typename T>
    S& operator<<(S &s, bits_t<T> b) {
        s.write((char*)&b.t, sizeof(T));
        return s;
    }
    //extraction operator to call ::read(), require a non-const reference here
    template<typename S, typename T>
    S& operator>>(S& s, bits_t<T&> b) {
        s.read((char*)&b.t, sizeof(T));
        return s;
    }

    bool HasEnding(const std::string & fullString, const std::string & ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else {
            return false;
        }
    }

    bool FileExists(const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

}

void Book::AddLine(const std::string & line, bool algAbr, bool trusted, std::ofstream * binFile){
   LOG(logDEBUG) << "Adding line " << line;
   std::stringstream str(Util::Trim(line));
   std::string moveStr;
   Position p(Position::startPosition);
   int k = 0;
   while( str >> moveStr ){
       ++k;
       ///@todo read directly from PGN file ???
       /*
       size_t pos = moveStr.find('.');
       if ( pos != std::string::npos ){
           moveStr = moveStr.substr(pos);
       }
       */
       if ( ! Add(moveStr, p, algAbr, trusted, binFile)) {
           LOG(logERROR) << "Bad move string " << moveStr << ", skipping rest of the line " << line << " " << k;
           break;
       }
   }
}

bool Book::ContainsMove(const Move & m, const Position & p){
   if ( _book.find(p.GetZHash()) == _book.end() ) return false;
   const std::set<BookMove> & s = _book[p.GetZHash()];
   for( auto it = s.begin() ; it != s.end() ; ++it){
       if ( (*it).h == m.ZHash()) return true;
   }
   return false;
}

bool Book::Add(const std::string & moveAlg, Position & p, bool algAbr, bool trusted, std::ofstream * binFile){
   LOG(logDEBUG) << "Adding move " << moveAlg;
   std::string moveStr;
   // beurk !!!!
   if ( moveAlg == "0-0" || moveAlg == "0-0-0" || moveAlg == "O-O" || moveAlg == "O-O-O"){
       algAbr = false;
   }
   if ( moveAlg == "0-0" || moveAlg == "0-0-0" || moveAlg == "O-O" || moveAlg == "O-O-O" || algAbr){
      moveStr = moveAlg;
   }
   else{
      moveStr.push_back(moveAlg[0]);
      moveStr.push_back(moveAlg[1]);
      moveStr.push_back(' ');
      moveStr.push_back(moveAlg[2]);
      moveStr.push_back(moveAlg[3]);
   }

   Move *mPtr = NULL;
   if ( algAbr ){
       mPtr = new Move(moveStr,p);
   }
   else{
       mPtr = new Move(moveStr,p.Turn());
   }
   Move & m = *mPtr;
   if ( !m.IsValid()){
       LOG(logERROR) << "Invalid try to add book move " << moveStr;
       delete mPtr;
       return false;
   }

   //LOG(logINFO) << "Read move " << m.ShowAlgAbr(p) << " " << m.ZHash();

   bool ok = true;
   const bool c = ContainsMove(m,p);
   const Util::Zobrist::HashType h = p.GetZHash();
   if (trusted) {
       p.ApplyMove(m, true);
   }
   else {
       p = m.Apply(p, &ok);
   }
   if (!ok) {
       LOG(logERROR) << "Cannot apply move " << p.GetFEN() << " " << m.ShowAlgAbr(p);
   }
   else {
       if (binFile) {
           //LOG(logINFO) << "hash " << m.ZHash();
           (*binFile) << bits(m.ZHash());
       }
       if (!c) {
           BookMove bm;
           bm.m = m;
           bm.h = m.ZHash();
           _book[h].insert(bm);
       }
   }

   //LOG(logINFO) << "Read move " << m.Show() << " " << m.ZHash();

   delete mPtr;
   return ok;
}

void Book::ReadBinaryBook(std::ifstream & stream, bool trusted) {
    Position p(Position::startPosition);
    Move::HashType hash = 0;
    int count = 0;
    int mcount = 0;
    while (!stream.eof()) {
        hash = 0;
        stream >> bits(hash);
        //LOG(logINFO) << "hash " << hash;
        if (hash == 0) {
            ++count;
            mcount = 0;
            //LOG(logINFO) << "Reset position";
            p = Position(Position::startPosition);
            stream >> bits(hash);
            if (stream.eof()) {
                break;
            }
        }
        //LOG(logINFO) << "hash " << hash;
        Move m(Move::HashToMini(hash));
        //LOG(logINFO) << "Read move " << m.ShowAlgAbr(p) << " " << hash;
        //LOG(logINFO) << "=> hash " << hash << " " << Square(Move::Hash2From(hash)).position()
        //    << " " << Square(Move::Hash2To(hash)).position()
        //    << " " << Move::Hash2Type(hash);

        const bool c = ContainsMove(m, p);
        const Util::Zobrist::HashType h = p.GetZHash();
        bool ok = true;
        if (trusted) {
            p.ApplyMove(m, true);
        }
        else {
            p = m.Apply(p, &ok);
        }
        if (!ok) {
            LOG(logERROR) << "Book cannot apply move " << p.GetFEN() << " " << m.ShowAlgAbr(p) << " line " << count << " move " << mcount;
            LOG(logFATAL) << "=> hash " << hash << " " << Square(Move::Hash2From(hash)).position()
                                                << " " << Square(Move::Hash2To(hash)).position()
                                                << " " << Move::Hash2Type(hash);
        }
        ++mcount;
        if (ok && !c) {
            const BookMove bm = { m, m.ZHash() };
            _book[h].insert(bm);
        }
    }
}

bool Book::ReadBook(bool trusted){

    if (Definitions::bookConfig.with_bigBook) {
        if ( Definitions::bookConfig.bookFileName.empty() ){
           LOG(logINFO) << "No book name given";
           return false;
        }
        LOG(logINFO) << "Loading big book ! " << Definitions::bookConfig.bookFileName;
        const std::string bookFileName(Definitions::bookConfig.bookFileName);
        bool isBinary = HasEnding(bookFileName, ".bin");

        if (!isBinary) {
            LOG(logINFO) << "Skip is set to " << Definitions::bookConfig.bookSkip;
            std::ifstream bookFile(bookFileName);
            const size_t lines = Util::CountLine(bookFile);
            size_t l = 0;
            size_t count = 0;
            if (bookFile) {
                LOG(logINFO) << "Book is not binary. Will be converted ...";
                const std::string bookFileNameBin = bookFileName.substr(0, bookFileName.find_last_of('.')) + ".bin";
                if (FileExists(bookFileNameBin)) {
                    std::remove(bookFileNameBin.c_str());
                }
                std::ofstream bookFileBinary(bookFileNameBin, std::ios::out | std::ios::binary);
                std::string line;
                size_t randStart = size_t(Definitions::bookConfig.bookSkip * (rand()*1. / RAND_MAX));
                while (std::getline(bookFile, line)) {
                    ++l;
                    if (l < randStart) continue;
                    if (l % 10000 == 0) {
                        int p = int(100.*l / lines);
                        LOG(logINFO) << p << "%" << " (" << count << "/" << lines / Definitions::bookConfig.bookSkip << ")";
                    }
                    if (l%Definitions::bookConfig.bookSkip == 0) {
                        ++count;
                        //LOG(logINFO) << line;
                        AddLine(line, true, false, &bookFileBinary);
                        bookFileBinary << bits(Move::HashType(0));
                    }
                }
                bookFileBinary.close();
            }
            else {
                LOG(logERROR) << "Cannot open book file " << bookFileName;
            }
            bookFile.close();
        }
        else {
            std::ifstream bookFileBin(bookFileName, std::ios::in | std::ios::binary);
            ReadBinaryBook(bookFileBin);
            bookFileBin.close();
        }
    }

    if (Definitions::bookConfig.with_smallBook) {
        LOG(logINFO) << "Loading small book !";

        // Four knights

        AddLine("e2e4 e7e5 b1c3 g8f6 g1f3 b8c6 f1b5 f8b4 0-0 0-0 d2d3 d7d6 c1g5 b4c3 b2c3 d8e7 f1e1 c6d8 d3d4 d8e6");

        // Italian

        AddLine("e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 c2c3 g8f6 d2d4 e5d4 c3d4 c5b4 c1d2 b4d2 b1d2 d7d5");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 c2c3 g8f6 d2d3 d7d6 b2b4 c5b6 a2a4 a7a5 b4b5 c6e7");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 d2d3 g8f6 c2c3 d7d6 c4b3 a7a6 b1d2 c5a7 h2h3 c6e7");

        // Two knights

        AddLine("e2e4 e7e5 g1f3 b8c6 f1c4 g8f6 f3g5 d7d5 e4d5 c6a5 c4b5 c7c6 d5c6 b7c6 b5e2 h7h6 g5f3 e5e4 f3e5 f8d6 f2f4 e4f3 e5f3 0-0 d2d4 c6c5");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1c4 g8f6 d2d4 e5d4 0-0 f6e4 f1e1 d7d5 c4d5 d8d5 b1c3 d5h5 c3e4 c8e6");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1c4 g8f6 d2d3 f8e7 0-0 0-0 c2c3 d7d6 c4b3");

        // Scotch

        AddLine("e2e4 e7e5 g1f3 b8c6 d2d4 e5d4 f3d4 g8f6 d4c6 b7c6 e4e5 d8e7 d1e2 f6d5 c2c4 c8a6 g2g3 g7g6 b2b3 f8g7 c1b2 0-0 f1g2 a8e8 0-0 ");
        AddLine("e2e4 e7e5 g1f3 b8c6 d2d4 e5d4 f3d4 f8c5 c1e3 d8f6 c2c3 g8e7 g2g3 0-0 f1g2");

        // Ruy Lopez

        AddLine("e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 0-0 f8e7 f1e1 b7b5 a4b3 d7d6 c2c3 0-0 h2h3 c6a5 b3c2 c7c5 d2d4 d8c7");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 0-0 f8e7 f1e1 b7b5 a4b3 d7d6 c2c3 0-0 h2h3 c8b7 d2d4 f8e8 b1d2 e7f8 a2a3 h7h6 b3c2 c6b8 b2b4 b8d7 c1b2 g7g6");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 0-0 f6e4 d2d4 b7b5 a4b3 d7d5 d4e5 c8e6 c2c3 e4c5 b3c2 e6g4");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1b5 f8c5 c2c3 g8f6 0-0 0-0 d2d4 c5b6 f1e1 d7d6 h2h3 c6e7 b1d2");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 0-0 f8c5 f3e5 c6e5 d2d4 c7c6 d4e5 f6e4 b5d3 d7d5 e5d6 e4f6 f1e1");
        AddLine("e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 0-0 f6e4 d2d4 f8e7 d1e2 e4d6 b5c6 b7c6 d4e5 d6b7 b1c3 0-0 f3d4");

        // Petroff

        AddLine("e2e4 e7e5 g1f3 g8f6 f3e5 d7d6 e5f3 f6e4 d2d4 d6d5 f1d3 b8c6 0-0 c8g4 c2c4 e4f6");
        AddLine("e2e4 e7e5 g1f3 g8f6 f3e5 d7d6 e5f3 f6e4 d2d4 d6d5 f1d3 b8c6 0-0 f8e7 c2c4 c6b4");
        AddLine("e2e4 e7e5 g1f3 g8f6 f3e5 d7d6 e5f3 f6e4 b1c3 e4c3 d2c3 b8c6 c1e3 f8e7 d1d2 c8g4");
        AddLine("e2e4 e7e5 g1f3 g8f6 d2d4 f6e4 f1d3 d7d5 f3e5 b8d7 e5d7 c8d7 0-0 f8d6 c2c4 c7c6 b1c3 e4c3 b2c3");

        // Sicilian

        AddLine("e2e4 c7c5 c2c3 d7d5 e4d5 d8d5 d2d4 e7e6");
        AddLine("e2e4 c7c5 c2c3 g8f6 e4e5 f6d5 d2d4 c5d4 g1f3 e7e6 c3d4 b7b6 b1c3 d5c3 b2c3 d8c7");
        AddLine("e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6 b1c3 a7a6 f1e2 e7e5 d4b3 f8e7 0-0 0-0 a2a4 b7b6");
        AddLine("e2e4 c7c5 b1c3 b8c6 g2g3 g7g6 f1g2 f8g7 d2d3 e7e6 c1e3 d7d6 g1e2 c6d4 d1d2");
        AddLine("e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6 b1c3 a7a6 c1g5 e7e6 d1d2 f8e7");
        AddLine("e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6 b1c3 a7a6 g2g3 e7e5 d4e2 b7b5 f1g2 c8b7 0-0 b8d7");
        AddLine("e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 g7g6 b1c3 f8g7 c1e3 g8f6 f1c4 0-0");
        AddLine("e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 g8f6 b1c3 d7d6 f1e2 e7e5 d4b3 f8e7 0-0 0-0 c1e3 c8e6");
        AddLine("e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 g8f6 b1c3 d7d6 f1e2 g7g6 c1e3 f8g7 0-0 0-0 d4b3 c8e6");

        // French

        AddLine("e2e4 e7e6 d2d4 d7d5 e4e5 c7c5 c2c3 b8c6 g1f3 d8b6 a2a3 c5c4");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1c3 g8f6 c1g5 f8e7 e4e5 f6d7 g5e7 d8e7 f2f4 0-0 d1d2 c7c5 g1f3 b8c6 0-0-0 c5c4");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1c3 d5e4 c3e4 b8d7 g1f3 g8f6 e4f6 d7f6 f1d3 b7b6 d1e2 c8b7? c1g5 f8e7");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1c3 f8b4 e4e5 g8e7 a2a3 b4c3 b2c3 c7c5 g1f3 b8c6 a3a4 d8a5 d1d2 c8d7");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1c3 f8b4 e4e5 g8e7 a2a3 b4c3 b2c3 c7c5 a3a4 b8c6 g1f3 d8a5 c1d2 c8d7");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1c3 f8b4 e4e5 c7c5 a2a3 b4c3 b2c3 g8e7 d1g4 d8c7 g4g7 h8g8 g7h7 c5d4 g1e2 b8c6 f2f4 c8d7");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1d2 c7c5 e4d5 e6d5 g1f3 b8c6 f1b5 f8d6 d4c5 d6c5 0-0 g8e7");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1d2 c7c5 g1f3 g8f6 e4d5 e6d5 f1b5 c8d7 b5d7 b8d7 0-0 f8e7");
        AddLine("e2e4 e7e6 d2d4 d7d5 b1d2 g8f6 e4e5 f6d7 f1d3 c7c5 c2c3 b8c6 g1e2 c5d4 c3d4 f7f6 e5f6 d7f6 0-0 f8d6");

        // Caro-Kann

        AddLine("e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 b8d7 g1f3 g8f6 e4f6 d7f6 f3e5");
        AddLine("e2e4 c7c6 b1c3 d7d5 d2d4 d5e4 c3e4 b8d7 f1c4 g8f6 e4g5 e7e6 d1e2 d7b6");
        AddLine("e2e4 c7c6 b1c3 d7d5 d2d4 d5e4 c3e4 b8d7 e4g5 g8f6 f1d3 e7e6");
        AddLine("e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 c8f5 e4g3 f5g6 h2h4 h7h6 g1f3 g8f6 f3e5 g6h7 f1d3 b8d7 d3h7");
        AddLine("e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 c8f5 e4g3 f5g6 h2h4 h7h6 g1f3 b8d7 h4h5 g6h7 f1d3 h7d3 d1d3 g8f6 c1d2 e7e6 0-0-0");
        AddLine("e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 c8f5 e4g3 f5g6 h2h4 h7h6 g1f3 b8d7 h4h5 g6h7 f1d3 h7d3 d1d3 e7e6 c1d2 g8f6 0-0-0");
        AddLine("e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 c8f5 e4g3 f5g6 g1f3 b8d7 h2h4 h7h6 f1d3 g6d3 d1d3 e7e6 c1d2 g8f6 0-0-0");
        AddLine("e2e4 c7c6 d2d4 d7d5 b1d2 d5e4 d2e4");
        AddLine("e2e4 c7c6 d2d4 d7d5 e4d5 c6d5 c2c4 g8f6 b1c3 e7e6 g1f3");
        AddLine("e2e4 c7c6 d2d4 d7d5 e4d5 c6d5 c2c4 g8f6 b1c3 b8c6 c1g5 e7e6 c4c5 f8e7 f1b5 0-0 g1f3 f6e4");
        AddLine("e2e4 c7c6 d2d4 d7d5 e4e5 c8f5 f1d3 f5d3 d1d3 e7e6 b1c3 d8b6");
        AddLine("e2e4 c7c6 b1c3 d7d5 g1f3 c8g4 h2h3 g4f3 d1f3 e7e6 d2d4 g8f6 f1d3 d5e4 c3e4 d8d4 c2c3 d4d8");

        // Pirc and modern

        AddLine("d2d4 d7d6 e2e4 g8f6 b1c3 g7g6 f1c4 c7c6 d1e2 f8g7 g1f3 0-0 c1g5 b7b5 c4d3 d8c7");
        AddLine("e2e4 d7d6 d2d4 g8f6 b1c3 g7g6 c1g5 f8g7 d1d2 b8d7 0-0-0 e7e5 d4e5 d6e5 g1f3 h7h6 g5h4 g6g5 h4g3 d8e7");

        // QGA

        AddLine("d2d4 d7d5 c2c4 d5c4 g1f3 g8f6 e2e3 e7e6 f1c4 c7c5 0-0 a7a6 d1e2 b7b5 c4d3 c5d4 e3d4 b8c6");

        // QGD

        AddLine("d2d4 d7d5 c2c4 e7e6 b1c3 f8e7 g1f3 g8f6 c4d5 e6d5");
        AddLine("c2c4 e7e6 d2d4 d7d5 b1c3 c7c5 c4d5 e6d5 g1f3 b8c6 g2g3 g8f6 f1g2 f8e7 0-0 0-0");
        AddLine("c2c4 e7e6 b1c3 d7d5 d2d4 g8f6 c1g5 f8e7 e2e3 0-0 g1f3 b8d7 a1c1 c7c6");
        AddLine("d2d4 d7d5 c2c4 e7e6 b1c3 g8f6 c1g5 b8d7 c4d5 e6d5 e2e3 c7c6 f1d3 f8e7 d1c2 0-0 g1e2 f8e8");
        AddLine("d2d4 d7d5 c2c4 e7e6 b1c3 g8f6 c1g5 b8d7 e2e3 c7c6 g1f3 d8a5 f3d2 f8b4 d1c2 0-0 g5h4 c6c5");
        AddLine("d2d4 d7d5 c2c4 e7e6 b1c3 g8f6 c4d5 e6d5 c1g5 c7c6 e2e3 f8e7 f1d3 0-0 d1c2 b8d7");
        AddLine("d2d4 d7d5 c2c4 e7e6 b1c3 g8f6 g1f3 f8e7 c4d5 e6d5 c1g5 0-0");

        // Slav

        AddLine("d2d4 d7d5 c2c4 c7c6 b1c3 g8f6 g1f3 d5c4 a2a4 c8f5 f3e5 e7e6 f2f3 f8b4 c1g5 h7h6 g5f6 d8f6 e2e4 f5h7");
        AddLine("d2d4 d7d5 c2c4 c7c6 g1f3 g8f6 b1c3 e7e6 e2e3 b8d7 f1d3 f8d6");

        // Catalan

        AddLine("d2d4 e7e6 c2c4 d7d5 g2g3 g8f6 g1f3 f8e7 f1g2 0-0 0-0 f6d7 d1c2 c7c6 b1d2 b7b6 e2e4 c8b7");

        // Nimzo-Indian

        AddLine("d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 d1c2 c7c5 d4c5 0-0 a2a3 b4c5 g1f3 b7b6");
        AddLine("d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 d1c2 0-0 a2a3 b4c3 c2c3 b7b6 c1g5 c8b7");
        AddLine("d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 g1f3 b7b6 g2g3 c8b7 f1g2");
        AddLine("d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 a2a3 b4c3 b2c3 0-0 f2f3 d7d5 c4d5 e6d5 e2e3 c8f5 g1e2 b8d7 e2g3 f5g6");
        AddLine("d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 c1d2 0-0 e2e3 d7d5 g1f3 c7c5 a2a3 b4c3 d2c3 f6e4 a1c1 e4c3 c1c3 c5d4");
        AddLine("d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 e2e3 0-0 f1d3 d7d5 g1f3 c7c5 0-0 b8c6 a2a3 b4c3 b2c3 d5c4 d3c4 d8c7");
        AddLine("d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 d1c2 d7d5 a2a3 b4c3 c2c3 b8c6 g1f3 f6e4 c3b3 c6a5 b3a4 c7c6");

        // Queen's Indian

        AddLine("d2d4 g8f6 c2c4 e7e6 g1f3 b7b6 g2g3 c8b7 f1g2 f8e7 0-0 0-0 b1c3 f6e4 d1c2 e4c3 c2c3");
        AddLine("d2d4 g8f6 c2c4 e7e6 g1f3 b7b6 e2e3 c8b7 f1d3 f8e7 b1c3 d7d5 0-0 0-0 d1e2 b8d7");

        // King's Indian

        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6 f2f3 0-0 c1e3 e7e5 d4d5 f6h5 d1d2 f7f5 0-0-0 b8d7");
        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6 g1f3 0-0 f1e2 e7e5 d4d5 a7a5");
        AddLine("d2d4 g8f6 c2c4 g7g6 g2g3 f8g7 f1g2 0-0 b1c3 d7d6 g1f3 b8d7 0-0 e7e5 e2e4 c7c6 h2h3 d8b6");
        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6 f2f4 c7c5 g1f3 0-0 d4d5 e7e6 f1d3 e6d5 c4d5 d8b6");
        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6 g1f3 0-0 f1e2 e7e5 0-0 b8c6 d4d5 c6e7 f3e1 f6e8 f2f3 f7f5");
        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 g1f3 0-0 c1f4 d7d6 h2h3 b8d7 e2e3 c7c6");

        // Grunfeld

        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 d7d5 c1f4 f8g7");
        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 d7d5 c4d5 f6d5 e2e4 d5c3 b2c3 c7c5 f1c4 f8g7 g1e2 0-0 0-0 c5d4 c3d4 b8c6");
        AddLine("d2d4 g8f6 c2c4 g7g6 b1c3 d7d5 g1f3 f8g7 d1b3 d5c4 b3c4 0-0 e2e4 c8g4 c1e3 f6d7 0-0-0 b8c6");

        // Benoni

        AddLine("d2d4 g8f6 c2c4 c7c5 d4d5 e7e6 b1c3 e6d5 c4d5 d7d6 e2e4 g7g6 f1d3 f8g7 g1e2 0-0 0-0 a7a6 a2a4 d8c7");

        // Dutch

        AddLine("d2d4 f7f5 g2g3 e7e6 f1g2 g8f6 g1f3 f8e7 0-0 0-0 c2c4 d7d6 b1c3 d8e8 d1c2 e8h5 b2b3 b8c6 c1a3 a7a5");

        // Queen's Pawn

        AddLine("d2d4 d7d5 g1f3 g8f6 c1f4 c7c5 e2e3 b8c6 c2c3 d8b6 d1c1 c8f5 d4c5 b6c5 b1d2 a8c8 f3d4 c6d4 e3d4 c5b6");

        // English

        AddLine("c2c4 e7e5 b1c3 g8f6 g1f3 b8c6 e2e4 f8b4 d2d3 d7d6 f1e2 0-0 0-0 b4c3 b2c3 d8e7");
        AddLine("c2c4 e7e5 b1c3 g8f6 g1f3 b8c6 g2g3 d7d5 c4d5 f6d5 f1g2 d5b6 0-0 f8e7 d2d3 0-0 c1e3 f7f5");
        //AddLine("c2c4 g8f6 b1c3 d7d5 c4d5 f6d5 e2e4 d5f4 f1c4 c8e6 c4e6 f7e6"); // what the fuck ?????
        AddLine("c2c4 g8f6 b1c3 e7e5 g1f3 b8c6 g2g3 f8c5 f1g2 d7d6 0-0 0-0 d2d3 h7h6");
        AddLine("c2c4 g8f6 b1c3 e7e5 g1f3 b8c6 g2g3 f8b4 f1g2 0-0 0-0 e5e4 f3e1 b4c3 d2c3 h7h6 e1c2 b7b6");
        AddLine("c2c4 c7c5 g1f3 b8c6 b1c3 g8f6 g2g3 g7g6 f1g2 f8g7 0-0 0-0 d2d4 c5d4 f3d4 c6d4 d1d4 d7d6 d4d3");

        // Reti
        AddLine("g1f3 d7d5 g2g3 g8f6 f1g2 g7g6 0-0 f8g7 d2d3 0-0 b1d2 b8c6 e2e4 e7e5 c2c3 a7a5 f1e1 d5e4 d3e4");
        AddLine("g1f3 d7d5 c2c4 e7e6 g2g3 g8f6 f1g2 f8e7 0-0 0-0 b2b3 c7c5 c4d5 f6d5 c1b2 b8c6 d2d4 b7b6 b1c3 d5c3");
        AddLine("g1f3 d7d5 c2c4 d5c4 e2e3 c7c5 f1c4 e7e6 0-0 g8f6 b2b3 b8c6 c1b2 a7a6 a2a4 f8e7");
    }

    return true;
}

namespace {
    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, (int)std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }

    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }
}

const Move *Book::Get(Util::Zobrist::HashType h){
   std::map<Util::Zobrist::HashType,std::set<BookMove> >::iterator it = _book.find(h);

   // position not found
   if ( it == _book.end() ){
       return NULL;
   }

   // randomize if multiple choices
   const std::set<BookMove> & l = it->second;
   return &(*select_randomly(l.begin(),l.end())).m;
}


