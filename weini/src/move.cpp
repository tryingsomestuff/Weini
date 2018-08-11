#include "move.h"
#include "position.h"
#include "square.h"
#include "moveGenerator.h"

#include "UtilPosition.h"

#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <float.h>
#include "definitions.h"
#include "UtilMove.h"
#include "search.h"
#include "sort.h"

bool Move::Ordering(FastContainer<Move> &moves, Position & p, Searcher & searcher, bool isPV,
                    DepthType depth, const Transposition * ttt, const Line * pv, bool trustedMove){

    // in fact it will ... (this call **won't** clear check mate score against "max" user)
    Sort::ClearScore(moves);

    if (! searcher.IsMainThread() && Definitions::smpConfig.shuffleThreadMoves ) {
        // if not main thread, totally shuffle moves ...
        Sort::Shuffle(moves);

        return false;
    }
    else {
        Sort::SortMoves(moves, p, searcher, Sort::SP_all);

        bool bestMoveFound = false;

        // TT move first
        if (/*searcher.IsMainThread() &&*/ ttt && ttt->move != 0){
          ++Stats::ttsorttry;
          if (Transposition::Sort( *ttt, moves, p)) {
            bestMoveFound = true;
            ++Stats::ttsort;
          }
        }

        // PV node first !
        if (/*searcher.IsMainThread() &&*/ ! bestMoveFound && pv && depth > 0 ){
          ++Stats::pvrootsorttry;
          if ( Sort::PVFirst(*pv,0,moves)){
             bestMoveFound = true;
             ++Stats::pvrootsort;
          }
        }

        if (/*searcher.IsMainThread() &&*/ ! bestMoveFound && isPV ){
          ///@todo isPv OR retBase + IIDMargin >= beta (from stockfish)
          // no best move so far, so let's do IID
          if ( Definitions::sortingConfig.do_internaliterativeDeepening && depth >= Definitions::selectivity.IIDMinDepth ){
              SearcherBase::SearchedMove iid = searcher.SearchRoot(-Definitions::scores.infScore,Definitions::scores.infScore,moves,p,depth/2,
                                                                   NULL,  // never update pv here
                                                                   true,  // forbid nullmove
                                                                   false, // no subject to time control
                                                                   true,  // forbid extension
                                                                   trustedMove);  // given moves are trusted
             ++Stats::iidsort;
             if ( ! Sort::IIDFirst(iid,moves) && SearcherBase::GetMove(iid).IsValid()){
                 LOG(logWARNING) << "Missed IID move " << SearcherBase::GetMove(iid).Show() << " " << p.GetFEN();
                 for ( size_t k = 0 ; k < moves.size() ; ++k ){
                     LOG(logWARNING) << "  " << moves[k].Show();
                 }
                 LOG(logFATAL) << "This is bad !";
             }
          }
        }

        return bestMoveFound;
    }

    return false;
}

bool Move::IsPawnPush(const Position & p)const{
   return p.IsPawn(_from);
}

bool Move::IsAdvancedPawnPush(const Position & p)const{
    return ((p.IsBlackPawn(_from) && RANK(_from) < 5)
         || (p.IsWhitePawn(_from) && RANK(_from) > 4 ));
}


bool Move::IsNearPromotion(const Position & p)const{
    return ((p.IsBlackPawn(_from) && RANK(_to) == 1)
         || (p.IsWhitePawn(_from) && RANK(_to) == 6));
}

bool Move::IsNearPromotion(const Position & p, const Square::LightSquare &s){
     return ((p.IsBlackPawn(s) && RANK(s) == 1)
          || (p.IsWhitePawn(s) && RANK(s) == 6));
}

/*
Move::Move(const Move & m) :
    _from(m._from),
    _to(m._to),
    _moveType(m._moveType),
    _isCapture(m._isCapture),
    _isCheck(m._isCheck),
    _zhash(m._zhash),
    _sortScore(m._sortScore)
{
}

Move::Move(Move && m) :
    _from(m._from),
    _to(m._to),
    _moveType(m._moveType),
    _isCapture(m._isCapture),
    _isCheck(m._isCheck),
    _zhash(m._zhash),
    _sortScore(m._sortScore)
{
}

Move& Move::operator=(const Move& m) {
    _from = m._from;
    _to = m._to;
    _moveType = m._moveType;
    _isCapture = m._isCapture;
    _isCheck = m._isCheck;
    _zhash = m._zhash;
    _sortScore = m._sortScore;
    return *this;
}

Move& Move::operator=(Move&& m) {
    _from = m._from;
    _to = m._to;
    _moveType = m._moveType;
    _isCapture = m._isCapture;
    _isCheck = m._isCheck;
    _zhash = m._zhash;
    _sortScore = m._sortScore;
    return *this;
}
*/

Move::~Move() {}

void Move::Reset(){
    _from = Square::Invalide;
    _to = Square::Invalide;
    _moveType = UtilMove::sm_invalide;
    _isCapture = false;
    _isCheck = false;
    _zhash = 0;
    _sortScore = (int)Definitions::scores.defaultMoveScore;
}

Move::Move(UtilMove::eSpecialMove moveType)
    :
      _from(Square::Invalide),
      _to(Square::Invalide),
      _moveType(moveType),
      _isCapture(false),
      _isCheck(false),
      _zhash(0),
      _sortScore((int)Definitions::scores.defaultMoveScore) {

    if ( moveType > UtilMove::sm_standard && moveType <= UtilMove::sm_castling_max){
        //ok
    }
    else{
        _moveType = UtilMove::sm_invalide;
        //LOG(logERROR) << "Trying to instanciate bad move by type";
    }
}

Move::Move(const std::string & s,const NSColor::eColor & c, bool alt)
    :
      _from(Square::Invalide),
      _to(Square::Invalide),
      _moveType(UtilMove::sm_standard),
      _isCapture(false),
      _isCheck(false),
      _zhash(0),
      _sortScore((int)Definitions::scores.defaultMoveScore) {

    std::string ss(s);

    if ( s.size() < 2 ){
        _moveType = UtilMove::sm_invalide;
        LOG(logFATAL) << "Not a move : " << s;
        return;
    }

    if ( !alt ){
       // add space to go to own alg notation
       if ( ss != "0-0" && ss != "0-0-0" && ss != "O-O" && ss != "O-O-O" ){
           ss.insert(2," ");
       }
    }
    if (! UtilMove::Read(c,ss,_from,_to,_moveType)){
        // to be sure ...
        _moveType = UtilMove::sm_invalide;
    }
}

Move::Move(const std::string & sAlgAbr, Position & p)
    :
      _from(Square::Invalide),
      _to(Square::Invalide),
      _moveType(UtilMove::sm_standard),
      _isCapture(false),
      _isCheck(false),
      _zhash(0),
      _sortScore((int)Definitions::scores.defaultMoveScore) {

      std::string s = UtilMove::GetAlgAlt(sAlgAbr,p);
      if ( ! Read(p.Turn(),s,_from,_to,_moveType)){
          _moveType = UtilMove::sm_invalide;
      }
}

Move::Move(const Square::LightSquare & from, const Square::LightSquare & to , UtilMove::eSpecialMove moveType)
    : _from(from),
      _to(to),
      _moveType(moveType),
      _isCapture(false),
      _isCheck(false),
      _zhash(0),
      _sortScore((int)Definitions::scores.defaultMoveScore) {

}

Move::Move(const MiniMove & mini)
    : _from(mini.from),
    _to(mini.to),
    _moveType(mini.moveType),
    _isCapture(false),
    _isCheck(false),
    _zhash(0),
    _sortScore((int)Definitions::scores.defaultMoveScore) {

}

void Move::Set(const Square::LightSquare & from, const Square::LightSquare & to, UtilMove::eSpecialMove moveType){
    _from = from;
    _to = to;
    _moveType = moveType;
    //_isCapture = false;
    //_isCheck = false;
    _zhash = 0;
    _sortScore = (int)Definitions::scores.defaultMoveScore;
}

void Move::Set(UtilMove::eSpecialMove moveType) {
    _moveType = moveType;
    //_isCapture = false;
    //_isCheck = false;
    _zhash = 0;
    _sortScore = (int)Definitions::scores.defaultMoveScore;
}

MiniMove Move::GetMini()const {
    return {_from, _to, _moveType};
}

Move::HashType Move::ZHash()const{
    if ( _zhash != 0 ) return _zhash;

    _zhash = ToHash(_from,_to,_moveType);

    return _zhash;
}

// inner move string
std::string Move::Show() const {

    if (_moveType == UtilMove::sm_invalide) {
        std::stringstream str;
        str << "xx (" << (int)_from << ", " << (int)_to << ")";
        return str.str();
    }

    if ( _moveType == UtilMove::sm_Ks || _moveType == UtilMove::sm_ks){
        return "0-0";
    }

    if ( _moveType == UtilMove::sm_Qs || _moveType == UtilMove::sm_qs){
        return "0-0-0";
    }

    std::string s = Square(_from).position() + " " + Square(_to).position();

    if ( _moveType == UtilMove::sm_ep){
        s += " ep";
    }
    else if ( _moveType > UtilMove::sm_castling_max){
        switch(_moveType){
        case UtilMove::sm_pq:
            s += "=";
            s += "Q";
            break;
        case UtilMove::sm_pr:
            s += "=";
            s += "R";
            break;
        case UtilMove::sm_pb:
            s += "=";
            s += "B";
            break;
        case UtilMove::sm_pn:
            s += "=";
            s += "N";
            break;
        }
    }
    return s;
}

// move string for PV
std::string Move::ShowAlg(const Position & p, bool withSep, bool specialRoque, bool withCheck) const {

    if (_moveType == UtilMove::sm_invalide) {
        return "xx";
    }

    if ( _moveType == UtilMove::sm_Ks || _moveType == UtilMove::sm_ks){
        return specialRoque?"O-O":"0-0";
    }

    if ( _moveType == UtilMove::sm_Qs || _moveType == UtilMove::sm_qs){
        return specialRoque?"O-O-O":"0-0-0";
    }

    std::string s;
    s+= Square(From()).position();
    if ( false && IsCapture()){
        s+="x";
    }
    else{
        if (withSep) s+="-";
    }
    s+= Square(To()).position();

    if (IsPromotion()){
        switch(_moveType){
        case UtilMove::sm_pq:
            s += "=";
            s += "Q";
            break;
        case UtilMove::sm_pr:
            s += "=";
            s += "R";
            break;
        case UtilMove::sm_pb:
            s += "=";
            s += "B";
            break;
        case UtilMove::sm_pn:
            s += "=";
            s += "N";
            break;
        }
    }

    if ( withCheck && IsCheck() ){
        s += "+";
    }

    return s;
}

// move string for pgn
std::string Move::ShowAlgAbr(Position & p) const {

    if (_moveType == UtilMove::sm_invalide) {
        return "xx";
    }

    if ( _moveType == UtilMove::sm_Ks || _moveType == UtilMove::sm_ks){
        return "0-0";
    }

    if ( _moveType == UtilMove::sm_Qs || _moveType == UtilMove::sm_qs){
        return "0-0-0";
    }

    std::string s;
    Piece::eType t = p.Get(_from);

    // add piece type if not pawn
    s+= Piece::SymbolAlg(t);
    if ( t==Piece::t_Wpawn || t==Piece::t_Bpawn ){
        s.clear(); // no piece symbol for pawn
    }

    // ensure move is not ambiguous
    bool isAmbiguousFile = false;
    bool isAmbiguousRank = false;
    if ( p.NumberOf(t)>1 ){
        std::deque<Square::LightSquare> v = Util::FindPiece(p,p.Turn(),t);
        for(auto it = v.begin() ; it != v.end() ; ++it){
           if ( *it == _from ) continue; // to not compare to myself ...
           FastContainer<Move> l;
           MoveGenerator().GeneratorSquare(p,l,*it);
           for(auto m = l.begin() ; m != l.end() ; ++m){
               if ( *m == *this ) continue; // to not compare to myself ... should no happend thanks to previous verification
               if ( (*m).To() == To() ){
                   isAmbiguousRank = true; ///@todo !!!!
                   if ( ! isAmbiguousFile && FILE((*m).From()) == FILE(From())){
                       isAmbiguousFile = true;
                   }
                   /*
                   if ( !isAmbiguousRank && RANK((*m).From()) == RANK(From())){
                       isAmbiguousRank = true;
                   }
                   */
               }
           }
        }
    }

    if ( isAmbiguousRank || ((t==Piece::t_Wpawn || t==Piece::t_Bpawn) && IsCapture()) ){
       s+= Square::fileStr(FILE(From()));
    }
    if ( isAmbiguousFile ){
       s+= Square::rankStr(RANK(From()));
    }

    // add 'x' if capture
    if ( IsCapture()){
        s+="x";
    }

    // add landing position
    s+= Square(To()).position();

    // and promotion to
    if (IsPromotion()){
        switch(_moveType){
        case UtilMove::sm_pq:
            s += "=";
            s += "Q";
            break;
        case UtilMove::sm_pr:
            s += "=";
            s += "R";
            break;
        case UtilMove::sm_pb:
            s += "=";
            s += "B";
            break;
        case UtilMove::sm_pn:
            s += "=";
            s += "N";
            break;
        }
    }

    if ( IsCheck() ){
        s += "+";
    }

    return s;
}

std::string Move::Show(const Position & p) const {
    std::string s = Piece::Symbol(p.Get(_from));
    s += Show();
    return s;
}

bool Move::Validate(Position & p, bool validateCheckState, bool checkValidity)/*const*/{

    bool isPromotion = false;

    // convert king Xboard notation to internal castling style if needed
    if ( _moveType == UtilMove::sm_standard &&
         _from == (p.WhiteToPlay()?Square::squaree1:Square::squaree8) &&
         p.Get(_from) == (p.WhiteToPlay()?Piece::t_Wking:Piece::t_Bking) ){
        if ( _to == (p.WhiteToPlay()?Square::squarec1:Square::squarec8)){
           Set(Square::Invalide,Square::Invalide,p.WhiteToPlay()?UtilMove::sm_Qs:UtilMove::sm_qs);
        }
        else if ( _to == (p.WhiteToPlay()?Square::squareg1:Square::squareg8)){
           Set(Square::Invalide,Square::Invalide,p.WhiteToPlay()?UtilMove::sm_Ks:UtilMove::sm_ks);
        }
    }

    // special cases first
    if ( _moveType > UtilMove::sm_standard ){
        if ( _moveType <= UtilMove::sm_castling_max ){ // castling
            return UtilMove::ValidateCastling(*this,p);
        }
        else{ // promotion (can be capture at the same time !)
            isPromotion = true;
        }
    }

    UtilMove::CheckForCapture(*this,p);

    if ( checkValidity ){
        // just in case
        if ( ! Square::IsValid(_from) ){
            LOG(logERROR) << "Invalid starting square " << _from ;
            return false;
        }
        if ( ! Square::IsValid(_to) ){
            LOG(logERROR) << "Invalid landing square " << _to ;
            return false;
        }
    }

    //LOG(logDEBUG) << "Move validation for " << Square(_from).position() << " " << Square(_to).position() << " (" << Poece::Symbol(p.Get(_from)) << ")" ;

    if ( checkValidity ){
        // the good color is moved
        if ((p.Color(_from)) != p.Turn()) {
            // from is actually a piece (not blank square)
            if (p.IsEmpty(_from)) {
                LOG(logERROR) << "Move from no piece " << Square(_from).position() ;
                return false;
            }
            LOG(logERROR) << "Not your turn " << ShowAlgAbr(p) << " " << p.GetFEN() ;
            return false;
        }
    }

    // is the move legal for the corresponding piece
    bool isOK = true;
    switch(p.Get(_from)) {
    case Piece::t_Brook:
    case Piece::t_Wrook:
        //LOG(logDEBUG) << "   rook" ;
        isOK = UtilMove::ValidateLegal(p,UtilMove::legalRook,4, _from, _to);
        break;
    case Piece::t_Bbishop:
    case Piece::t_Wbishop:
        //LOG(logDEBUG) << "   bishop" ;
        isOK = UtilMove::ValidateLegal(p,UtilMove::legalBishop,4, _from, _to);
        break;
    case Piece::t_Bknight:
    case Piece::t_Wknight:
        //LOG(logDEBUG) << "   knight" ;
        isOK = UtilMove::ValidateLegal(p,UtilMove::legalKnight,8, _from, _to, 1);
        break;
    case Piece::t_Bqueen:
    case Piece::t_Wqueen:
        //LOG(logDEBUG) << "   queen" ;
        isOK = UtilMove::ValidateLegal(p,UtilMove::legalQueen,8, _from, _to);
        break;
    case Piece::t_Bking:
    case Piece::t_Wking:
        //LOG(logDEBUG) << "   king" ;
        isOK = UtilMove::ValidateLegal(p,UtilMove::legalKing,8, _from, _to, 1);
        break;
    case Piece::t_Bpawn:
        //LOG(logDEBUG) << "   black pawn" ;
        isOK = UtilMove::ValidatePawn(*this,p);
        break;
    case Piece::t_Wpawn:
        //LOG(logDEBUG) << "   white pawn" ;
        isOK = UtilMove::ValidatePawn(*this,p);
        break;
    default:
        //LOG(logDEBUG) << "Error: bad piece" << Square(_from).position() ;
        break;
    }
    if ( ! isOK ){
        //LOG(logERROR) << "Move is not valid " << Show();
        return false;
    }

    if ( validateCheckState ){
        UtilMove::ValidateIsCheck(*this,p);
    }

    return UtilMove::ValidateCheckStatus(*this,p);
}

Position Move::Apply(const Position &p, bool * success) {
#ifdef WITH_MEMORY_POOL_BUILDER
   ScopeClone<Position, _default_block_size> psc(p);
   Position & p2 = psc.Get();
#else
   Position p2(p);
#endif

    if( Validate(p2) ) { // THIS IS SLOWWWWWWW
        p2.ApplyMove(*this);
        if ( success ) *success = true;
    }
    else {
        if ( success ) *success = false;
    }
    return p2; // copy
}

Position & Move::ApplyInPlace(Position & p, bool *success) {
    bool b = false;
    Position p2 = Apply(p, &b);
    if (success) *success = b;
    p = p2;
    return p;
}

