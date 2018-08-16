#include "UtilMove.h"

#include "move.h"
#include "position.h"
#include "moveGenerator.h"
#include "UtilPosition.h"
#include "sort.h"

#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <float.h>

/*
namespace{
struct BlackThings;

struct WhiteThings{
   static const Piece::eType pawn   = Piece::t_Wpawn;
   static const Piece::eType knight = Piece::t_Wknight;
   static const Piece::eType bishop = Piece::t_Wbishop;
   static const Piece::eType rook   = Piece::t_Wrook;
   static const Piece::eType queen  = Piece::t_Wqueen;
   static const Piece::eType king   = Piece::t_Wking;
   typedef BlackThings Opponent;
   static const NSColor::eColor color = NSColor::c_white;

   static inline const UtilMove::Direction * legalPawnStdCap() { return UtilMove::legalWPawnStdCap; }

   static inline const Square::LightSquare KingSq(const Position & p){ return p.WhiteKingSquare();}
};

struct BlackThings{
   static const Piece::eType pawn   = Piece::t_Bpawn;
   static const Piece::eType knight = Piece::t_Bknight;
   static const Piece::eType bishop = Piece::t_Bbishop;
   static const Piece::eType rook   = Piece::t_Brook;
   static const Piece::eType queen  = Piece::t_Bqueen;
   static const Piece::eType king   = Piece::t_Bking;
   typedef WhiteThings Opponent;
   static const NSColor::eColor color = NSColor::c_black;

   static inline const UtilMove::Direction * legalPawnStdCap() { return UtilMove::legalBPawnStdCap; }

   static inline const Square::LightSquare KingSq(const Position & p){ return p.BlackKingSquare();}
};

}*/

namespace UtilMove{

void InitMvvLva() {
    for(int attacker = TypeToInt(Piece::t_BkingFalse); attacker <= TypeToInt(Piece::t_WkingFalse); ++attacker) {
        for(int victim = TypeToInt(Piece::t_BkingFalse); victim <= TypeToInt(Piece::t_WkingFalse); ++victim) {
            //MvvLvaScores[victim][attacker] = (ScoreType) (std::abs(Piece::Value(Piece::IntToType(victim))) - std::abs( Piece::Value(Piece::IntToType(attacker))));
            /*
            LOG(logINFO) << "MVVLVA" << " v: " << std::setw(10) << Piece::Name(Piece::IntToType(victim))
                                     << " a: " << std::setw(10) << Piece::Name(Piece::IntToType(attacker))
                                     << " s: " << std::setw(10) << MvvLvaScores[victim][attacker];
            */
        }
    }
}

void DisplayList(const FastContainer<Move> & moves, const std::string & message){
    if ( !message.empty() ) {
       LOG(logINFO) << message;
    }
    std::stringstream str;
    std::copy(moves.begin(), moves.end(),std::ostream_iterator<Move>(str, " "));
    LOG(logINFO) << str.str();
}

std::string GetFrom(const std::string & to, Position & p, const Piece & pp, const std::string & helper = "" ){
    Piece::eType t = pp.Type();
    if ( p.IsTherePieceOnBoard(t) ){
        std::deque<Square::LightSquare> v = Util::FindPiece(p,p.Turn(),t);
        for(auto it = v.begin() ; it != v.end() ; ++it){
           FastContainer<Move> l;
           MoveGenerator().GeneratorSquare(p,l,*it);
           for(auto m = l.begin() ; m != l.end() ; ++m){
               if ( Square((*m).To()).position() == to ){
                   if ( helper.empty()){
                       return Square((*m).From()).position();
                   }
                   else{
                       std::string from = Square((*m).From()).position();
#ifndef __linux__
                       if (isalpha(helper[0])) {
#else
                       if (std::isalpha(helper[0])){
#endif
                          if ( from[0] == helper[0]){
                              return from;
                          }
                       }
                       else{
                           if ( from[1] == helper[0]){
                               return from;
                           }
                       }
                   }
               }
           }
        }
    }

    return "";
}

std::string GetAlgAlt(const std::string & sAlgAbrInit, Position & p){
   Piece pp;
   NSColor::eColor c = p.Turn();
   std::string sAlgAbr = sAlgAbrInit;
   std::string from;
   std::string to;
   std::string prom;
   size_t pos = sAlgAbr.find('=');
   if ( pos != std::string::npos){ // promotion
      prom = sAlgAbr.substr(pos+1);
      if ( prom.size() != 1 ){
          LOG(logFATAL) << "Bad promotion string " << prom << " (" << sAlgAbrInit << ")";
          return "";
      }
      sAlgAbr = sAlgAbr.substr(0,pos);
   }

   switch(sAlgAbr[0]){
   case 'K':
       pp.Set(c == NSColor::c_white ? Piece::t_Wking : Piece::t_Bking);
       sAlgAbr = sAlgAbr.substr(1);
       break;
   case 'Q':
       pp.Set(c == NSColor::c_white ? Piece::t_Wqueen : Piece::t_Bqueen);
       sAlgAbr = sAlgAbr.substr(1);
       break;
   case 'R':
       pp.Set(c == NSColor::c_white ? Piece::t_Wrook : Piece::t_Brook);
       sAlgAbr = sAlgAbr.substr(1);
       break;
   case 'B':
       pp.Set(c == NSColor::c_white ? Piece::t_Wbishop : Piece::t_Bbishop);
       sAlgAbr = sAlgAbr.substr(1);
       break;
   case 'N':
       pp.Set(c == NSColor::c_white ? Piece::t_Wknight : Piece::t_Bknight);
       sAlgAbr = sAlgAbr.substr(1);
       break;
   default:
       pp.Set(c == NSColor::c_white ? Piece::t_Wpawn : Piece::t_Bpawn);
       break;
   }

   sAlgAbr.erase(std::remove(sAlgAbr.begin(), sAlgAbr.end(), 'x'), sAlgAbr.end());

   switch(sAlgAbr.size()){
   case 2:
   {
       to = sAlgAbr;
       from = GetFrom(to,p,pp);
   }
       break;
   case 3:
   {
       to = sAlgAbr.substr(1);
       std::string helper = sAlgAbr.substr(0,1);
       from = GetFrom(to,p,pp,helper);
   }
       break;
   case 4:
   {
       from = sAlgAbr.substr(0,1);
       to   = sAlgAbr.substr(2);
   }
       break;
   default:
       LOG(logFATAL) << "Bad move string " << sAlgAbr << " (" << sAlgAbrInit << ")";
       return "";
   }

   if ( from.empty() ){
       LOG(logFATAL) << "Cannot get from square " << to;
       return "";
   }

   return from + " " + to + (prom.empty()?"":std::string("=")+prom);

}

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " " ){
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first non-delimiter.
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next non-delimiter.
    pos = str.find_first_of(delimiters, lastPos);
  }
}

// only read internal "a3 b8 ep" own notation
bool Read(const NSColor::eColor & c, const std::string & s, Square::LightSquare & from, Square::LightSquare & to, eSpecialMove & moveType ) {

    if ( s.empty()){
        LOG(logERROR) << "Trying to read empty move ! ";
        moveType = sm_invalide;
        return false;
    }

    //LOG(logINFO) << "Reading move " << s ;
    std::vector<std::string> strList;
    std::stringstream iss(s);
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              back_inserter(strList));

    if ( strList.empty()){
        LOG(logERROR) << "Trying to read bad move, seems empty " << s;
        moveType = sm_invalide;
        return false;
    }

    // detect special move
    if (strList[0] == "0-0" || strList[0] == "O-O"){
        if ( c == NSColor::c_white ){
            moveType = sm_Ks;
        }
        else{
            moveType = sm_ks;
        }
    }
    else if (strList[0] == "0-0-0" || strList[0] == "O-O-O"){
        if ( c == NSColor::c_white ){
            moveType = sm_Qs;
        }
        else{
            moveType = sm_qs;
        }
    }
    else{
        if ( strList.size() == 1 ){
            LOG(logERROR) << "Trying to read bad move, malformed (=1) " << s;
            moveType = sm_invalide;
            return false;
        }
        if ( strList.size() > 2 && strList[2] != "ep"){
            LOG(logERROR) << "Trying to read bad move, malformed (>2)" << s;
            moveType = sm_invalide;
            return false;
        }

        if (strList[0].size() == 2 && (strList[0].at(0) >= 'a') && (strList[0].at(0) <= 'h') &&
                ((strList[0].at(1) >= 1) && (strList[0].at(1) <= '8'))) {
            from = Square(strList[0]).index();
        }
        else {
            LOG(logERROR) << "Trying to read bad move, invalid from square " << s;
            moveType = sm_invalide;
            return false;
        }

        moveType = sm_standard;

        // be carefull, promotion possible !
        if (strList[1].size() >= 2 && (strList[1].at(0) >= 'a') && (strList[1].at(0) <= 'h') &&
                ((strList[1].at(1) >= '1') && (strList[1].at(1) <= '8'))) {
            if ( strList[1].size() > 2 ){ // promotion
                std::string prom;
                if ( strList[1].size() == 3 ){ // probably e7 e8q notation
                   prom = strList[1][2];
                   to = Square(strList[1].substr(0,2)).index();
                }
                else{ // probably e7 e8=q notation
                   std::vector<std::string> strListTo;
                   Tokenize(strList[1],strListTo,"=");
                   to = Square(strListTo[0]).index();
                   prom = strListTo[1];
                }

                if ( prom == "Q" || prom == "q"){
                    moveType = sm_pq;
                }
                else if ( prom == "R" || prom == "r"){
                    moveType = sm_pr;
                }
                else if ( prom == "B" || prom == "b"){
                    moveType = sm_pb;
                }
                else if ( prom == "N" || prom == "n"){
                    moveType = sm_pn;
                }
                else{
                    LOG(logERROR) << "Trying to read bad move, invalid to square " << s;
                    moveType = sm_invalide;
                    return false;
                }
            }
            else{
               to = Square(strList[1]).index();
            }
        }
        else {
            LOG(logERROR) << "Trying to read bad move, invalid to square " << s;
            moveType = sm_invalide;
            return false;
        }

    }

    return true;
}

bool ValidatePieceInTheWay(const Position & p, const Square::LightSquare & from, int depth, int dir, int startDepth, NSColor::eColor c){
    // checking for piece in the way
    Square::LightSquare k = from + dir*startDepth;
    for (int i = startDepth; i < depth; ++i) {
        // there is a piece in the way
        // the only possible valid case is an opposite color piece at depth max
        if (p.IsNotEmpty(k)) {
            //LOG(logINFO) << "Bad move, there is a piece in the way at " << Square(from + dir*i).position() ;
            return false;
        }
        k += dir;
    }
    // there is a piece in the way
    // the only possible valid case is an opposite color piece at depth max
    if (p.Color(k) == c) {
        //LOG(logINFO) << "Bad move, there is a piece in the way at " << Square(from + dir*depth).position() ;
        return false;
    }

    return true;
}

bool ValidateLegal(const Position & p,const Direction * legal, int nDir, const Square::LightSquare & from, const Square::LightSquare & to, int maxdepth){
    const Square::LightSquare diff = to - from;
    int depth = 8; // > 7
    Square::LightSquare dir = 0;
    Square::RankFile incrFile = 0;
    bool found = false;

    const Square::RankFile ffile = FILE(from);

    // depth first look-up (needed for early _exit)
    for (int k = 1; k <= maxdepth && !found; ++k) {
        for (int d = 0; d < nDir && !found; ++d ) {
            // checking for legal direction
            Square::RankFile file = legal[d].FileInc();
            //Square::RankFile rank = legal[d].RankInc();
            Square::LightSquare kk = legal[d].Incr();
            if (kk*k == diff) {
                incrFile = file*k;
                if ( UtilMove::ValidateBoardSides(ffile+incrFile) ){
                    depth = k;
                    dir = kk;
                    found = true;
                }
            }
        }
    }

    if (!found){
        //LOG(logINFO) << "Invalid move for " << Piece::Symbol(p.Get(from)) << ": " << from.position() << " " << to.position() ;
        return false;
    }

    return ValidatePieceInTheWay(p,from,depth,dir,1,p.Turn());
}

bool PossibleEnPassant     (const Move & m, const Position & p){
    if ( !p.IsPawn(m.From())) return false;
    const Square::LightSquare & from  = m.From();
    const Square::LightSquare & to    = m.To();
    const Square::LightSquare absdiff = std::abs(to - from);
    return (absdiff == 9 || absdiff == 7) && p.IsEmpty(to);
}

bool ValidateEnPassant(const Move & m, const Position & p) {

    const Square::LightSquare ep = p.Info().enPassant;
    if (!Square::IsValid(ep)) {
        //LOG(logDEBUG) << "No valid en passant available" ;
        return false;
    }

    const Square::LightSquare & from = m.From();
    const Square::LightSquare & to = m.To();

    if (RANK(ep) == RANK(from) &&
        std::abs(FILE(ep) - FILE(from)) == 1 &&
        FILE(ep) == FILE(to) &&
        RANK(ep) == RANK(to) - (p.IsWhitePawn(from) ? +1 : -1) )
    {
        //LOG(logDEBUG) << "Valid en-passant capture" ;
        m._moveType = sm_ep;
        m._isCapture = true;
        return true;
    }
    else {
        //LOG(logDEBUG) << "Error: invalid en-passant capture" ;
        return false;
    }
}

bool ValidatePawn(const Move & m, const Position & p) {

    const Square::LightSquare & from = m.From();
    const Square::LightSquare & to = m.To();
    const Square::LightSquare absdiff = std::abs(to - from);

    switch ( absdiff ){
    case 8:
        //case landing empty
        return p.IsEmpty(to);
        /*
        if ( p.IsEmpty(to) ){
            return true;
        }
        else{
            //case landing not empty
            //LOG(logERROR) << "Invalid pawn move, piece at landing square : " << pp;
            return false;
        }
        */
        break;
    case 7:
    case 9:
        if (p.IsNotEmpty(to)){
            //case landing not empty (capture) )
            if ( p.Color(to) == p.Color(from)) { // ok because pp is not empty, thus it has a color
                //LOG(logDEBUG) << "Bad move, there is a piece in the way at " << to.position() ;
                return false;
            }
            else{
                //LOG(logDEBUG) << "Valid capture with pawn" ;
                m._isCapture = true; //not necessary here
                return true;
            }
        }
        else{
            return ValidateEnPassant(m, p);
        }
        break;
    case 16:
        //case landing empty
        if (p.IsEmpty(to)){
            if (p.IsWhitePawn(from)){
                // checking starting rank
                if (RANK(from) != 1) {
                    //LOG(logDEBUG) << "Bad move, invalid double pawn push for white from " << from.position() ;
                    return false;
                }
                // checking no piece in the way
                if (p.IsNotEmpty(from + 8)) {
                    //LOG(logDEBUG) << "Bad move, invalid double pawn push from " << from.position() << ", there a piece in the way " << Square(from + 8).position() ;
                    return false;
                }
            }
            else{ // black
                // checking starting rank
                if (RANK(from) != 6) {
                    //LOG(logDEBUG) << "Bad move, invalid double pawn push for black from " << from.position() ;
                    return false;
                }
                // checking no piece in the way
                if (p.IsNotEmpty(from - 8)) {
                    //LOG(logDEBUG) << "Bad move, invalid double pawn push from " << from.position() << ", there a piece in the way " << Square(from - 8).position() ;
                    return false;
                }
            }
            return true;
        }
        //case landing not empty
        else{
            //LOG(logDEBUG) << "Invalid pawn move, piece at landing square" ;
            return false;
        }
        break;
    default:
        LOG(logFATAL) << "Wrong pawn push ! " << (int)absdiff << " " << Square(from).position() << " " << Square(to).position() << " " << p.GetFEN();
    }

    //LOG(logDEBUG) << "Invalid pawn move" ;
    return false;
}

bool ValidateCastling(const Move & m, const Position & p){

    //LOG(logINFO) << "Move validation for castling" << p.GetFEN();

    // debug, castling are invalid in this case
    //return false;

    Square::LightSquare mustBeFree[3]        = {Square::Invalide,Square::Invalide,Square::Invalide};
    Square::LightSquare mustNotBeThreaten[3] = {Square::Invalide,Square::Invalide,Square::Invalide};

    switch(m.Type()){
    case(sm_Ks):
        if ( (p.Info().white.CastlingRight() & Player::cr_ks) == 0){
            //LOG(logDEBUG) << "White king side castling not allowed anymore" ;
            return false;
        }
        mustBeFree[0] = Square::squaref1;
        mustBeFree[1] = Square::squareg1;
        mustNotBeThreaten[0] = Square::squaree1;
        mustNotBeThreaten[1] = Square::squaref1;
        mustNotBeThreaten[2] = Square::squareg1;
        break;
    case(sm_Qs):
        if ( (p.Info().white.CastlingRight() & Player::cr_qs) == 0){
            //LOG(logDEBUG) << "White queen side castling not allowed anymore" ;
            return false;
        }
        mustBeFree[0] = Square::squareb1;
        mustBeFree[1] = Square::squarec1;
        mustBeFree[2] = Square::squared1;
        mustNotBeThreaten[0] = Square::squarec1;
        mustNotBeThreaten[1] = Square::squared1;
        mustNotBeThreaten[2] = Square::squaree1;
        break;
    case(sm_ks):
        if ( (p.Info().black.CastlingRight() & Player::cr_ks) == 0){
            //LOG(logDEBUG) << "Black king side castling not allowed anymore" ;
            return false;
        }
        mustBeFree[0] = Square::squaref8;
        mustBeFree[1] = Square::squareg8;
        mustNotBeThreaten[0] = Square::squaree8;
        mustNotBeThreaten[1] = Square::squaref8;
        mustNotBeThreaten[2] = Square::squareg8;
        break;
    case(sm_qs):
        if ( (p.Info().black.CastlingRight() & Player::cr_qs) == 0){
            //LOG(logDEBUG) << "Black queen side castling not allowed anymore" ;
            return false;
        }
        mustBeFree[0] = Square::squareb8;
        mustBeFree[1] = Square::squarec8;
        mustBeFree[2] = Square::squared8;
        mustNotBeThreaten[0] = Square::squarec8;
        mustNotBeThreaten[1] = Square::squared8;
        mustNotBeThreaten[2] = Square::squaree8;
        break;
    }

    // check square that must be free
    for ( unsigned short int k = 0 ; k < 3 ; ++k){
        if ( Square::IsValid(mustBeFree[k]) && p.IsNotEmpty(mustBeFree[k]) ){
            //LOG(logDEBUG) << "Cannot castle, square " << Square(mustBeFree[k]).position() << " isn't free" ;
            return false;
        }
    }

    // check square that must not be threaten
    for ( unsigned short int k = 0 ; k < 3 ; ++k){
        if ( Square::IsValid(mustNotBeThreaten[k]) && IsThreatened(p,mustNotBeThreaten[k])){
            //LOG(logDEBUG) << "Cannot castle, square " << Square(mustNotBeThreaten[k]).position() << " is threatened" ;
            return false;
        }
    }

    return true;
}

bool IsTherePieceOnBoard(const Position & p, const Piece::eType & t){
    return p.IsTherePieceOnBoard(t);
}

bool GetThreadsHelperFast(const Position & p, Square::LightSquare index, bool earlyExit, FastContainer<Square::LightSquare> *threats){

    if ( ! Square::IsValid(index) ){
        LOG(logFATAL) << "Fatal : invalid square..." ;
    }

    return p.WhiteToPlay() ? GetThreadsHelperFastWhite(p,index,earlyExit,threats)
                           : GetThreadsHelperFastBlack(p,index,earlyExit,threats);
}

bool GetThreadsHelperFastWhite(const Position & p, Square::LightSquare index, bool earlyExit, FastContainer<Square::LightSquare> *threats){
    bool threadFound = false;

    if ( IsTherePieceOnBoard(p,Piece::t_Bpawn) ){
        const Piece::eType targetTypeP = Piece::t_Bpawn;
        for (int d = 0; d < 2; ++d ) {
            if (IsOffTheBoard(index, legalWPawnStdCap[d].IncrSpe())) continue;
            const Square::LightSquare to = index+legalWPawnStdCap[d].Incr();
            if( p.IsOfType(targetTypeP, to) ){
                if ( threats ) threats->push_back(to);
                if ( earlyExit ) return true;
                threadFound = true;
                //break; // thread can only come from one direction !
            }
        }
    }

    if ( IsTherePieceOnBoard(p, Piece::t_Brook)
       ||IsTherePieceOnBoard(p, Piece::t_Bqueen) ){
        for (int d = 0; d < 4; ++d ) {
            Square::LightSquare to = index;
            const Square::RankFile IncrSpe = legalRook[d].IncrSpe();
            for(int depth = 1 ; /*depth <= 7*/ ; ++depth){
                if (IsOffTheBoard(to,IncrSpe)) break; // next dir !
                to += legalRook[d].Incr();
                if ( p.IsBlackRQ(to) ){
                    if ( threats ) threats->push_back(to);
                    if ( earlyExit ) return true;
                    threadFound = true;
                    // DO NOT go to next direction, because of aligned threats
                }
                else{
                    if ( p.IsNotEmpty(to)){
                        break;
                    }
                }
            }
        }
    }

    if ( IsTherePieceOnBoard(p, Piece::t_Bbishop)
       ||IsTherePieceOnBoard(p, Piece::t_Bqueen) ) {
        for (int d = 0; d < 4; ++d ) {
            Square::LightSquare to = index;
            const Square::RankFile IncrSpe = legalBishop[d].IncrSpe();
            for(int depth = 1 ; /*depth <= 7*/ ; ++depth){
                if (IsOffTheBoard(to,IncrSpe)) break; // next dir
                to += legalBishop[d].Incr();
                if ( p.IsBlackBQ(to) ){
                    if ( threats ) threats->push_back(to);
                    if ( earlyExit ) return true;
                    threadFound = true;
                    // DO NOT go to next direction, because of aligned threats
                }
                else{
                    if ( p.IsNotEmpty(to)){
                        break;
                    }
                }
            }
        }
    }

    if (IsTherePieceOnBoard(p, Piece::t_Bknight)){
        for (int d = 0; d < 8; ++d ) {
            if (IsOffTheBoard(index, legalKnight[d].IncrSpe())) continue;
            const Square::LightSquare to = index+legalKnight[d].Incr();
            if(p.IsBlackKnight(to)){
                if ( threats ) threats->push_back(to);
                if ( earlyExit ) return true;
                threadFound = true;
                //break; // threat from two knight can come from multiple directions
            }
        }
    }

    if ( IsTherePieceOnBoard(p, Piece::t_Bking)
         && std::max(
             std::abs(FILE(p.BlackKingSquare())-FILE(index)) , // distance to king (file)
             std::abs(RANK(p.BlackKingSquare())-RANK(index))   // distance to king (rank)
         ) < 2 ){
        for (int d = 0; d < 8; ++d ) {
            if (IsOffTheBoard(index, legalKing[d].IncrSpe())) continue;
            const Square::LightSquare to = index+legalKing[d].Incr();
            if(p.IsBlackKing(to)){
                if ( threats ) threats->push_back(to);
                if ( earlyExit ) return true;
                threadFound = true;
                break; // only one king
            }
        }
    }

    // This is expensive ! a lot !!!
    //if ( threats ) threats->sort(SortThreatsFunctor(p));

    return threadFound;
}

bool GetThreadsHelperFastBlack(const Position & p, Square::LightSquare index, bool earlyExit, FastContainer<Square::LightSquare> *threats){

    bool threadFound = false;

    if ( IsTherePieceOnBoard(p,Piece::t_Wpawn) ){
        const Piece::eType targetTypeP = Piece::t_Wpawn;
        for (int d = 0; d < 2; ++d ) {
            if (IsOffTheBoard(index, legalBPawnStdCap[d].IncrSpe())) continue;
            const Square::LightSquare to = index+legalBPawnStdCap[d].Incr();
            if( p.IsOfType(targetTypeP,to) ){
                if ( threats ) threats->push_back(to);
                if ( earlyExit ) return true;
                threadFound = true;
                //break; // thread can only come from one direction !
            }
        }
    }

    if ( IsTherePieceOnBoard(p, Piece::t_Wrook)
       || IsTherePieceOnBoard(p, Piece::t_Wqueen) ){
        for (int d = 0; d < 4; ++d ) {
            Square::LightSquare to = index;
            const Square::RankFile IncrSpe = legalRook[d].IncrSpe();
            for(int depth = 1 ; /*depth <= 7*/ ; ++depth){
                if (IsOffTheBoard(to,IncrSpe)) break; // next dir !
                to += legalRook[d].Incr();
                if ( p.IsWhiteRQ(to) ){
                    if ( threats ) threats->push_back(to);
                    if ( earlyExit ) return true;
                    threadFound = true;
                    // DO NOT go to next direction, because of aligned threats
                }
                else{
                    if ( p.IsNotEmpty(to)){
                        break;
                    }
                }
            }
        }
    }

    if ( IsTherePieceOnBoard(p, Piece::t_Wbishop)
       ||IsTherePieceOnBoard(p, Piece::t_Wqueen) ) {
        for (int d = 0; d < 4; ++d ) {
            Square::LightSquare to = index;
            const Square::RankFile IncrSpe = legalBishop[d].IncrSpe();
            for(int depth = 1 ; /*depth <= 7*/ ; ++depth){
                if (IsOffTheBoard(to,IncrSpe)) break; // next dir
                to += legalBishop[d].Incr();
                if ( p.IsWhiteBQ(to) ){
                    if ( threats ) threats->push_back(to);
                    if ( earlyExit ) return true;
                    threadFound = true;
                    // DO NOT go to next direction, because of aligned threats
                }
                else{
                    if ( p.IsNotEmpty(to)){
                        break;
                    }
                }
            }
        }
    }

    if (IsTherePieceOnBoard(p, Piece::t_Wknight)){
        for (int d = 0; d < 8; ++d ) {
            if (IsOffTheBoard(index, legalKnight[d].IncrSpe())) continue;
            const Square::LightSquare to = index+legalKnight[d].Incr();
            if(p.IsWhiteKnight(to)){
                if ( threats ) threats->push_back(to);
                if ( earlyExit ) return true;
                threadFound = true;
                //break; // threat from two knight can come from multiple directions
            }
        }
    }

    if ( IsTherePieceOnBoard(p, Piece::t_Wking)
         && std::max(
             std::abs(FILE(p.WhiteKingSquare())-FILE(index)) , // distance to king (file)
             std::abs(RANK(p.WhiteKingSquare())-RANK(index))   // distance to king (rank)
         ) < 2 ){
        for (int d = 0; d < 8; ++d ) {
            if (IsOffTheBoard(index, legalKing[d].IncrSpe())) continue;
            const Square::LightSquare to = index+legalKing[d].Incr();
            if(p.IsWhiteKing(to)){
                if ( threats ) threats->push_back(to);
                if ( earlyExit ) return true;
                threadFound = true;
                break; // only one king
            }
        }
    }

    // This is expensive ! a lot !!!
    //if ( threats ) threats->sort(SortThreatsFunctor(p));

    return threadFound;
}


// be carefull, GetThreads may be illegal moves for king, (king become check)
bool GetThreads(const Position & p, Square::LightSquare index, bool earlyExit, FastContainer<Square::LightSquare> * threats){
    return GetThreadsHelperFast(p,index,earlyExit, earlyExit?NULL:threats);
}

bool IsThreatened(const Position & p, Square::LightSquare index){
    if (!Square::IsValid(index)) {
        LOG(logFATAL) << "Fatal : invalid square...";
    }

    return GetThreads(p,index);
}

bool GetChecks(const Position & p, bool earlyExit,FastContainer<Square::LightSquare> * threats){
    Square::LightSquare ks = p.WhiteToPlay() ? p.WhiteKingSquare() : p.BlackKingSquare();

    if (!Square::IsValid(ks)) {
        LOG(logFATAL) << "Fatal : invalid square...";
    }

    return GetThreads(p,ks,earlyExit, earlyExit?NULL:threats);
}

bool KingIsChecked(const Position &p){

    // =====================
    // No more king !
    // =====================
    if ((p.WhiteToPlay() && p.NoWhiteKingAnymore()) || (!p.WhiteToPlay() && p.NoBlackKingAnymore()) ){
        //LOG(logDEBUG) << "no more king inside KingIsChecked, this should not happend ... " << p.GetFEN();
        return false;
    }

    int ks = p.WhiteToPlay() ? p.WhiteKingSquare() : p.BlackKingSquare();

    if (!Square::IsValid(ks)) {
        LOG(logFATAL) << "Fatal : invalid square...";
    }

    return GetThreads(p,ks);
}

bool ValidateCheckStatus(const Move & m, Position & p){

    // is the king was in check ?
    bool kingWasChecked = p.IsInCheck();

    // applying move from current position (forced style)
#ifdef WITH_MEMORY_POOL_BUILDER
   ScopeClone<Position, _default_block_size> psc(p);
   Position & p2 = psc.Get();
#else
   Position p2(p);
#endif

    p2.ApplyMove(m);

    // Now verify that current player escape a check or do not go in check itself
    p2.SwitchPlayer();

    bool ret = true;

    // check for king pin
    if ( ! kingWasChecked ){
        //LOG(logINFO) << "King wasn't in check and it must remain so" ;
        if ( KingIsChecked(p2) ){
            /*
            if ( p.Get(_from) == (p.WhiteToPlay() ? Piece::t_Wking : Piece::t_Bking)){
              LOG(logINFO) << "Impossible move for king ! => Move " << _from.position() << " " << m.To().position();
            }
            else{
              LOG(logINFO) << "This piece is pinned ! => Move " << _from.position() << " " << m.To().position();
            }
            */
            ret = false;
        }
    }
    // check for king to escape current check
    else{
        //LOG(logINFO) << "King was in check and this must change" ;
        if ( KingIsChecked(p2) ){
            //LOG(logINFO) << "King is still checked !" ;
            ret = false;
        }
    }

    return ret;
}

bool ValidateIsCheck(const Move &m, Position & p){
    // applying move from current position (forced style)

#ifdef WITH_MEMORY_POOL_BUILDER
   ScopeClone<Position, _default_block_size> psc(p);
   Position & p2 = psc.Get();
#else
   Position p2(p);
#endif

    p2.ApplyMove(m);

    Square::LightSquare wk = Square::Invalide;
    Square::LightSquare bk = Square::Invalide;

    // Is the current move give a check ?
    if ( p2.Turn() == NSColor::c_white){
        bk = p2.InvalidateBlackKing();
    }
    else{
        wk = p2.InvalidateWhiteKing();
    }

    m._isCheck = KingIsChecked(p2); // mutable

    return true; // ALWAYS RETURN TRUE, returns bool only to be used with && in movegenerator
}

bool DetectStandardCapture(const Move & m,const Position & p){
    if ( p.IsNotEmpty(m.To()) ){
        m._isCapture = true;
    }
    return m._isCapture;
}

void SetIsCapture          (const Move & m, bool b){
    m._isCapture = b;
}

SortThreatsFunctor::SortThreatsFunctor(const Position & p) : _p(p){
}

bool SortThreatsFunctor::operator()(const Square::LightSquare & s1,const Square::LightSquare & s2) {
    // least value attacker first !
    return Piece::Value(_p.Get(s1)) < Piece::Value(_p.Get(s2));
}

ReverseSortThreatsFunctor::ReverseSortThreatsFunctor(const Position & p) : _p(p) {
}

bool ReverseSortThreatsFunctor::operator()(const Square::LightSquare & s1, const Square::LightSquare & s2) {
    // greatest value attacker first !
    return Piece::Value(_p.Get(s1)) > Piece::Value(_p.Get(s2));
}

}
