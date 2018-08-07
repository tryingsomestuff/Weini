#include "position.h"
#include "move.h"
#include "analyse.h"
#include "moveGenerator.h"
#include "UtilPosition.h"


#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <sstream>
#include <iostream>
#include <iterator>
#include <random>

#include <bitset>

std::string Position::startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const int Position::maxGamePhase = 22;
const int Position::endGameGamePhasePercent = 25;

///@todo this CTOR MUST implement some "ispositionvalid" stuff !!! to be used in SideToMoveFromFEN
Position::Position(const std::string & s, bool withMoveCount):
    _c(NSColor::c_none)
{

    // this is needed in order to init zhash below!
    _ClearBoard();

    // shortcut
    std::string fen = s;
    if (fen == "start") {
        fen = startPosition;
    }

    if (fen == "fine70") {
        fen = "8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - -";
    }

    // remove unused elements
    std::vector<std::string> strList;
    std::stringstream iss(fen);
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              back_inserter(strList));
    std::vector<std::string>(strList.begin(), strList.begin()+(withMoveCount?std::min(6,(int)strList.size()):4)).swap(strList);
    fen = std::accumulate(strList.begin(), strList.end(), std::string(""),
                                                    [](const std::string & a, const std::string & b) {
                                                       return a + ' ' + b;
                                                    });

    // build board, this will NOT set up zhash (invalidate at the end)
    _BuildBoard(fen);
    _info.isInCheck  = UtilMove::KingIsChecked(*this);

    GetZHash(); // this will set up zhash !

    ///@todo : are both king checked ? : or other invalid position (missing king, ...)
}

Position::~Position() {

}

void Position::_AddCastlingRight(Player & player, const Player::eCastlingRight & cr){
   if ( !(player.CastlingRight() && cr) ){
      _XORHash(Util::Zobrist::castlingRights[player.CastlingRight()][player.Color()]);
      player.AddCastlingRight(cr);
      _XORHash(Util::Zobrist::castlingRights[player.CastlingRight()][player.Color()]);
   }
}

void Position::_RemoveCastlingRight(Player & player, const Player::eCastlingRight & cr){
   if ( player.CastlingRight() && cr ){
      _XORHash(Util::Zobrist::castlingRights[player.CastlingRight()][player.Color()]);
      player.RemoveCastlingRight(cr);
      _XORHash(Util::Zobrist::castlingRights[player.CastlingRight()][player.Color()]);
   }
}

void Position::ApplyMove(const Move & m, bool verifyKingCapture) {

    if (! m.IsValid()) {
        LOG(logFATAL) << "Invalid move ! " << GetFEN() << " " << m.Show();
    }

    if ( _info.currentply >= MAX_GAME_PLY-1 ){
        LOG(logFATAL) << "Game is too long ! " << MAX_GAME_PLY << " " << _info.currentply << " " << GetFEN();
    }

    //LOG(logWARNING) << "Hash before apply " << GetZHash();

    // get current player
    Player & player = Turn() == NSColor::c_white ? dynamic_cast<Player&>(_info.white) : dynamic_cast<Player&>(_info.black);
    Player & opponent = Turn() != NSColor::c_white ? dynamic_cast<Player&>(_info.white) : dynamic_cast<Player&>(_info.black);

    // some shortcuts
    UtilMove::eSpecialMove moveType  = m.Type();
    const Square::LightSquare & from = m.From();
    const Square::LightSquare & to   = m.To();

    // special cases first : castling & promotion
    if ( moveType != UtilMove::sm_standard && moveType != UtilMove::sm_ep ){

        if ( m.IsCapture() ){
            _info.lastCapturedPiece = Get(to);
        }

        // castling first
        if ( moveType <= UtilMove::sm_castling_max ){
            //LOG(logDEBUG) << "Castling" ;

            player.SetCastled(true);

            // update castling rights
            _RemoveCastlingRight(player,Player::cr_ks);
            _RemoveCastlingRight(player,Player::cr_qs);

            // apply the special move
            switch(moveType){
            case UtilMove::sm_Ks:
                SetPiece(Square::squaree1,Piece::t_empty);
                SetPiece(Square::squaref1,Piece::t_Wrook);
                // last landing position is the rook one, king should be safe !
                _info.lastMove = { Square::squaree1,Square::squaref1,UtilMove::sm_Ks };
                SetWhiteKing(Square::squareg1);
                SetPiece(Square::squareg1,Piece::t_Wking);
                SetPiece(Square::squareh1,Piece::t_empty);
                break;
            case UtilMove::sm_ks:
                SetPiece(Square::squaree8,Piece::t_empty);
                SetPiece(Square::squaref8,Piece::t_Brook);
                // last landing position is the rook one, king should be safe !
                _info.lastMove = { Square::squaree8,Square::squaref8,UtilMove::sm_ks };
                SetBlackKing(Square::squareg8);
                SetPiece(Square::squareg8,Piece::t_Bking);
                SetPiece(Square::squareh8,Piece::t_empty);
                break;
            case UtilMove::sm_Qs:
                SetPiece(Square::squarea1,Piece::t_empty);
                SetPiece(Square::squareb1,Piece::t_empty);
                SetWhiteKing(Square::squarec1);
                SetPiece(Square::squarec1,Piece::t_Wking);
                SetPiece(Square::squared1,Piece::t_Wrook);
                // last landing position is the rook one, king should be safe !
                _info.lastMove = { Square::squaree1,Square::squared1,UtilMove::sm_Qs };
                SetPiece(Square::squaree1,Piece::t_empty);
                break;
            case UtilMove::sm_qs:
                SetPiece(Square::squarea8,Piece::t_empty);
                SetPiece(Square::squareb8,Piece::t_empty);
                SetBlackKing(Square::squarec8);
                SetPiece(Square::squarec8,Piece::t_Bking);
                SetPiece(Square::squared8,Piece::t_Brook);
                // last landing position is the rook one, king should be safe !
                _info.lastMove = { Square::squaree8,Square::squared8,UtilMove::sm_qs };
                SetPiece(Square::squaree8,Piece::t_empty);
                break;
            case UtilMove::sm_ep:
            case UtilMove::sm_pn:
            case UtilMove::sm_pb:
            case UtilMove::sm_pr:
            case UtilMove::sm_pq:
                // nothing here ...
                break;
            }

            // Update move count
            ++_info.halfMovesSinceLastPawnMoveOrCapture;

        } // end if castling
        else{ // now treat promotion

            const Piece::eType pieceFrom     = Get(from);
            //const Piece::eType pieceTo       = Get(to);

            if ( pieceFrom == Piece::t_Bpawn && RANK(to) == 0 ){
                SetPiece(from,Piece::t_empty);
                switch(moveType){
                case UtilMove::sm_pq:
                    SetPiece(to,Piece::t_Bqueen);
                    break;
                case UtilMove::sm_pr:
                    SetPiece(to,Piece::t_Brook);
                    break;
                case UtilMove::sm_pb:
                    SetPiece(to,Piece::t_Bbishop);
                    break;
                case UtilMove::sm_pn:
                    SetPiece(to,Piece::t_Bknight);
                    break;
                }
                // last landing position
                _info.lastMove = m.GetMini();

            }
            else if ( pieceFrom == Piece::t_Wpawn && RANK(to) == 7 ){
                SetPiece(from,Piece::t_empty);
                switch(moveType){
                case UtilMove::sm_pq:
                    SetPiece(to,Piece::t_Wqueen);
                    break;
                case UtilMove::sm_pr:
                    SetPiece(to,Piece::t_Wrook);
                    break;
                case UtilMove::sm_pb:
                    SetPiece(to,Piece::t_Wbishop);
                    break;
                case UtilMove::sm_pn:
                    SetPiece(to,Piece::t_Wknight);
                    break;
                }
                // last landing position
                _info.lastMove = m.GetMini();
            }
            else{
                LOG(logFATAL) << "ERROR: bad promotion " << Square(from).position() << " " << Square(to).position() << ", " << m.Show() << " " << GetFEN();
            }

            // Update move count
            _info.halfMovesSinceLastPawnMoveOrCapture = 0;

        } // end promotions

        // XOR out enPassant
        if ( Square::IsValid(_info.enPassant)){
           _XORHash(Util::Zobrist::enPassant[FILE(_info.enPassant)][Opponent()]);
           // invalidate en passant
           Square::InvalidateLightSquare(_info.enPassant);
        }
    }
    else{
        const Piece::eType pieceFrom     = Get(from);
        const Piece::eType pieceTo       = Get(to);

        if ( verifyKingCapture && Piece::IsK(pieceTo) ){
           LOG(logFATAL) << "Capturing king ! " << GetFEN() << " " << m.Show();
        }

        //LOG(logDEBUG) << "Move from " << Square(from).position() << " to " << Square(to).position() ;

        // check for capture
        bool capture = false;
        if (pieceTo != Piece::t_empty) {
            if ( !m.IsCapture() ){
                LOG(logFATAL) << "Missed capture during move validation " << GetFEN() << " " << m.Show();
            }
            capture = true;
#ifdef WITH_CAPTURED
            player.captured[player.capturedN++] = Get(to);
#endif
            _info.lastCapturedPiece = Get(to);
        }

        // check for en passant capture
        if ( moveType == UtilMove::sm_ep ){
            if ( !m.IsCapture() ){
                LOG(logFATAL) << "Missed en-passant capture during move validation " << GetFEN() << " " << m.Show();
            }
            //LOG(logDEBUG) << "Apply en-passant capture" ;
            capture = true;
#ifdef WITH_CAPTURED
            player.captured[player.capturedN++] = Get(_info.enPassant);
#endif
            _info.lastCapturedPiece = Get(_info.enPassant);
            SetPiece(_info.enPassant,Piece::t_empty);
        }
        /* // old code for previous if condition
        if ( ( pieceFrom == Piece::t_Wpawn || pieceFrom == Piece::t_Bpawn)){
          Square ep(_info.enPassant);
          if(( ep.IsValid() ) &&
             ( ep.rank() == RANK(from) ) &&
             ( std::abs(ep.file()-FILE(from)) == 1 ) &&
             ( ep.rank() == RANK(to) - NSColor::c_white ? +1 : -1 ) &&
             ( ep.file() == FILE(to) ) ) {
        */

        // update (disable) en-passant
        if ( Square::IsValid(_info.enPassant)){
           //LOG(logINFO) << "XORing out en passant " << Square(_info.enPassant).position();
           // XOR out enPassant (which was set using opponent hash)
           _XORHash(Util::Zobrist::enPassant[FILE(_info.enPassant)][Opponent()]);
           // invalidate en passant
           Square::InvalidateLightSquare(_info.enPassant);
        }

        // update (enable) en-passant
        if (  ( pieceFrom == Piece::t_Wpawn && to-from ==  16 )
              ||( pieceFrom == Piece::t_Bpawn && to-from == -16 ) ){
            //LOG(logDEBUG) << "Set possible en-passant to " << to.position() ;
            _info.enPassant = to;
            // XOR in enPassant
            //LOG(logINFO) << "XORing in en passant " << Square(_info.enPassant).position();
            _XORHash(Util::Zobrist::enPassant[FILE(_info.enPassant)][Turn()]);
        }

        // update (disable) lastCapturedPiece if needed
        if ( ! capture){
            _info.lastCapturedPiece = Piece::t_empty;
        }

        // update castling rights
        if ( Piece::IsK(pieceFrom) ){
            _RemoveCastlingRight(player,Player::cr_ks);
            _RemoveCastlingRight(player,Player::cr_qs);
        }
        else if ( pieceFrom == Piece::t_Wrook ){
            if (from == Square::squarea1){
                _RemoveCastlingRight(player,Player::cr_qs);
            }
            else if (from == Square::squareh1 ){
                _RemoveCastlingRight(player,Player::cr_ks);
            }
        }
        else if ( pieceFrom == Piece::t_Brook ){
            if (from == Square::squarea8){
                _RemoveCastlingRight(player,Player::cr_qs);
            }
            else if (from == Square::squareh8 ){
                _RemoveCastlingRight(player,Player::cr_ks);
            }
        }
        if ( capture && pieceTo == Piece::t_Wrook){
            if (to == Square::squarea1){
                _RemoveCastlingRight(opponent,Player::cr_qs);
            }
            else if (to == Square::squareh1 ){
                _RemoveCastlingRight(opponent,Player::cr_ks);
            }
        }
        else if ( capture && pieceTo == Piece::t_Brook){
            if (to == Square::squarea8){
                _RemoveCastlingRight(opponent,Player::cr_qs);
            }
            else if (to == Square::squareh8 ){
                _RemoveCastlingRight(opponent,Player::cr_ks);
            }
        }

        // Update move count
        ++_info.halfMovesSinceLastPawnMoveOrCapture;
        if (pieceFrom == Piece::t_Bpawn || pieceFrom == Piece::t_Wpawn || capture){
            _info.halfMovesSinceLastPawnMoveOrCapture = 0;
        }

        if (capture) {
            // invalidate gamePhase only if touching not empty Square
            _info.gamePhase = 0;
        }

        // last landing position
        _info.lastMove = m.GetMini();

        // update king position on the fly
        if ( pieceFrom == Piece::t_Wking){
            SetWhiteKing(to);
        }
        if ( pieceFrom == Piece::t_Bking){
            SetBlackKing(to);
        }

        // late promotion (not detected in move validation)
        if ( pieceFrom == Piece::t_Bpawn && RANK(to) == 0 ){
            // should only append during see
            //LOG(logWARNING) << "WARNING: auto promotion to B queen " << GetFEN() ;
            SetPiece(from,Piece::t_empty);
            SetPiece(to,Piece::t_Bqueen);
            //m._moveType = UtilMove::sm_pq;
        }
        else if ( pieceFrom == Piece::t_Wpawn && RANK(to) == 7 ){
            // should only append during see
            //LOG(logWARNING) << "WARNING: auto promotion to W queen " << GetFEN();
            SetPiece(from,Piece::t_empty);
            SetPiece(to,Piece::t_Wqueen);
            //m._moveType = UtilMove::sm_pq;
        }
        else{
            // Apply the standard move (or en-passant)
            //LOG(logINFO) << "Apply standard move " << m.Show();
            SetPiece(to,pieceFrom);
            SetPiece(from,Piece::t_empty);
        }
    }

    // Update player
    SwitchPlayer();

    // Set inCheck flag
    _info.isInCheck = m.IsCheck();
    /*
    if ( _info.isInCheck ){
        LOG(logWARNING) << "Move give check " << GetFEN() << " after " << m.Show();
    }
    */

    // Update move count
    if (_c == NSColor::c_white){
        ++_info.movesCount;
    }

    if ( _info.currentply >= MAX_GAME_PLY-1 ){
        LOG(logFATAL) << "Game is too long ! " << MAX_GAME_PLY << " " << _info.currentply << " " << GetFEN();
    }
    ++_info.currentply;

}

void Position::Display()const {
    LOG(logINFO) << " ===================== Position display (begin) ==================== ";
    LOG(logINFO) << "Move                " << Moves()           ;
    LOG(logINFO) << "HalfMove (50 rule)  " << HalfMoves50Rule() ;
    LOG(logINFO) << "Ply                 " << CurrentPly()      ;
    LOG(logINFO) << "Possible en passant " << (Square::IsValid(_info.enPassant)?Square(_info.enPassant).position():"none") ;
    LOG(logINFO) << ((_c==NSColor::c_white)? "White":"Black") << " to play" ;
    if ( IsEndGame()){
       LOG(logINFO) << "This is an end-game !";
    }

    std::stringstream ss;
    ss << std::endl;
    for (int j = 7; j >= 0; --j) {
        ss << "+-+-+-+-+-+-+-+-+" << std::endl;
        ss << "|";
        for (int i = 0; i < 8; ++i) {
            ss << Piece::Symbol(Get(i,j)) << '|';
        }
        ss << std::endl; ;
    }
    ss << "+-+-+-+-+-+-+-+-+" ;
    LOG(logINFO) << ss.str();

#ifdef WITH_CAPTURED
    ss.str("");
    ss << "Captured by white : ";
    for (unsigned char k = 0; k < _info.white.capturedN; ++k) {
        ss << Piece::Symbol(_info.white.captured[k]) << " ";
    }
    ss << std::endl;
    LOG(logINFO) << ss.str();
#endif


#ifdef WITH_CAPTURED
    ss.str("");
    ss << "Captured by black : ";
    for (unsigned int k = 0; k < _info.black.capturedN; ++k) {
        ss << Piece::Symbol(_info.black.captured[k]) << " ";
    }
    ss << std::endl;
    LOG(logINFO) << ss.str();
#endif

    FastContainer<Square::LightSquare> checks;
    if ( UtilMove::GetChecks(*this,false,&checks) ){
        LOG(logINFO) << "King is checked" ;
        for ( auto it = checks.begin() ; it != checks.end(); ++it){
            LOG(logINFO) << "check from " << Square(*it).position() ;
        }
    }

    // debug
    //_bitBoard.Display();

    LOG(logINFO) << "FEN : " << GetFEN() ;
    LOG(logINFO) << "HASH  : " << GetZHash();

    LOG(logINFO) << " ===================== Position display (end) ==================== ";
}

void Position::_ClearBoard(){
    Invalidate();
    std::fill(_board,_board + BOARD_SIZE, Piece::t_empty);
    _bitBoard.Reset();
}

void Position::_BuildBoard(const std::string &s) {
    std::vector<std::string> strList;
    std::stringstream iss(s);
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              back_inserter(strList));

    _ClearBoard();

    // read the board - translate each loop idx into a square
    Square::LightSquare j = 1;
    Square::LightSquare i = 0;
    while ((j <= BOARD_SIZE) && (i <= strList[0].length()))	{
        char letter = strList[0].at(i);
        ++i;
        int aFile = ((j - 1) % 8);
        int aRank = 7 - ((j - 1) / 8);
        switch (letter)
        {
        case 'p': SetPiece(aFile,aRank,Piece::t_Bpawn);   break;
        case 'r': SetPiece(aFile,aRank,Piece::t_Brook);   break;
        case 'n': SetPiece(aFile,aRank,Piece::t_Bknight); break;
        case 'b': SetPiece(aFile,aRank,Piece::t_Bbishop); break;
        case 'q': SetPiece(aFile,aRank,Piece::t_Bqueen);  break;
        case 'k': SetPiece(aFile,aRank,Piece::t_Bking);   break;
        case 'P': SetPiece(aFile,aRank,Piece::t_Wpawn);   break;
        case 'R': SetPiece(aFile,aRank,Piece::t_Wrook);   break;
        case 'N': SetPiece(aFile,aRank,Piece::t_Wknight); break;
        case 'B': SetPiece(aFile,aRank,Piece::t_Wbishop); break;
        case 'Q': SetPiece(aFile,aRank,Piece::t_Wqueen);  break;
        case 'K': SetPiece(aFile,aRank,Piece::t_Wking);   break;
        case '/': j--; break;
        case '1': break;
        case '2': j++; break;
        case '3': j += 2; break;
        case '4': j += 3; break;
        case '5': j += 4; break;
        case '6': j += 5; break;
        case '7': j += 6; break;
        case '8': j += 7; break;
        default: LOG(logERROR) << "FEN ERROR 0 : " << letter ;
        }
        j++;
    }

    // set the turn; default = White
    _c = NSColor::c_white;
    if (strList.size() >= 2){
        if (strList[1] == "w") {
            _c = NSColor::c_white;
        }
        else if (strList[1] == "b") {
            _c = NSColor::c_black;
        }
        else {
            LOG(logERROR) << "FEN ERROR 1" ;
            return;
        }
    }

    // Initialize all castle possibilities
    if (strList.size() >= 3){
        bool found = false;
        if (strList[2].find('K') != std::string::npos){
            //LOG(logDEBUG) << "Adding Ks" ;
            _info.white.AddCastlingRight(Player::cr_ks);
            found = true;
        }
        if (strList[2].find('Q') != std::string::npos){
            //LOG(logDEBUG) << "Adding Qs" ;
            _info.white.AddCastlingRight(Player::cr_qs);
            found = true;
        }
        if (strList[2].find('k') != std::string::npos){
            //LOG(logDEBUG) << "Adding ks" ;
            _info.black.AddCastlingRight(Player::cr_ks);
            found = true;
        }
        if (strList[2].find('q') != std::string::npos){
            //LOG(logDEBUG) << "Adding qs" ;
            _info.black.AddCastlingRight(Player::cr_qs);
            found = true;
        }
        if ( ! found ){
            LOG(logWARNING) << "No castling right given" ;
        }
    }
    else{
        LOG(logWARNING) << "No castling right given" ;
    }

    // read en passant and save it
    if ((strList.size() >= 4) && strList[3] != "-" ){
        if (strList[3].length() >= 2){
            ///@todo we have an inner wrong representation of en-passant, incompatible with fen !
            if ( strList[3].at(1) == '3' ) strList[3].at(1) = '4';
            if ( strList[3].at(1) == '6' ) strList[3].at(1) = '5';
            if ((strList[3].at(0) >= 'a') && (strList[3].at(0) <= 'h') &&
                    ((strList[3].at(1) == '4') || (strList[3].at(1) == '5'))){
                _info.enPassant = Square(strList[3]).index();
            }
            else {
                LOG(logERROR) << "FEN ERROR 3-1 : bad en passant square : " << strList[3] ;
                return;
            }
        }
        else{
            LOG(logERROR) << "FEN ERROR 3-2 : bad en passant square : " << strList[3] ;
            return;
        }
    }
    else{
        LOG(logDEBUG) << "No en passant square given" ;
    }

    // read 50 moves rules
    if (strList.size() >= 5){
        std::stringstream ss(strList[4]);
        int tmp;
        ss >> tmp;
       _info.halfMovesSinceLastPawnMoveOrCapture = tmp;
    }
    else{
        _info.halfMovesSinceLastPawnMoveOrCapture = 0;
    }

    // read number of move
    if (strList.size() >= 6){
        std::stringstream ss(strList[5]);
        int tmp;
        ss >> tmp;
        _info.movesCount = tmp;
    }
    else{
        _info.movesCount = 1;
    }

    // find kings
    SetWhiteKing(Util::FindKing(*this,NSColor::c_white));
    SetBlackKing(Util::FindKing(*this,NSColor::c_black));
    if ( !Square::IsValid(WhiteKingSquare()) || !Square::IsValid(BlackKingSquare()) ){
        LOG(logFATAL) << "A king is missing !!!";
    }
    else{
        LOG(logDEBUG) << "White king is at " << Square(WhiteKingSquare()).position();
        LOG(logDEBUG) << "Black king is at " << Square(BlackKingSquare()).position();
    }

    // debug
    //Display();

    Invalidate();
}

//#define DEBUG_HASH_RECOMPUTE

Util::Zobrist::HashType Position::GetZHash  ()const{

#ifndef DEBUG_HASH_RECOMPUTE
    if ( _info.zhash != 0 ) return _info.zhash;
#endif

    //LOG(logERROR) << "Zhash late init, this should not append !";

    Util::Zobrist::HashType h = 0;
    for (int i = 0 ; i < 64 ; ++i){
       const Piece::eType t = Get(i);
       //if ( t != Piece::t_empty ){
          h ^= Util::Zobrist::zTable[i][TypeToInt(t)];
       //}
    }

    if ( Turn() != NSColor::c_none ){
        if ( Square::IsValid(_info.enPassant) ){
            h ^= Util::Zobrist::enPassant[FILE(_info.enPassant)][Opponent()];
        }
        h ^= Util::Zobrist::turn[Turn()];
        h ^= Util::Zobrist::castlingRights[_info.white.CastlingRight()][0];
        h ^= Util::Zobrist::castlingRights[_info.black.CastlingRight()][1];
    }

#ifdef DEBUG_HASH_RECOMPUTE
    if (_info.zhash != 0 && _info.zhash != h) {
        LOG(logFATAL) << "Hash error " << h << " " << _info.zhash << " " << GetFEN();
    }
#endif

#ifndef DEBUG_HASH_RECOMPUTE
    _info.zhash = h;
#else
    if (_info.zhash == 0) _info.zhash = h;
#endif

    return _info.zhash;
}

Util::Zobrist::HashType Position::GetZHashPawn  ()const{

#ifndef DEBUG_HASH_RECOMPUTE
    if ( _info.zhashpawn != 0 ) return _info.zhashpawn;
#endif

    //LOG(logERROR) << "ZhashPawn late init, this should not append !";

    Util::Zobrist::HashType h = 0;
    for (int i = 0 ; i < 64 ; ++i){
       const Piece::eType t = Get(i);
       if ( Piece::IsPawn(t) ){
          h ^= Util::Zobrist::zTable[i][TypeToInt(t)];
       }
    }

#ifdef DEBUG_HASH_RECOMPUTE
    if (_info.zhashpawn != 0 && _info.zhashpawn != h) {
        LOG(logFATAL) << "Hash error " << h << " " << _info.zhashpawn << " " << GetFEN();
    }
#endif

#ifndef DEBUG_HASH_RECOMPUTE
    _info.zhashpawn = h;
#else
    if (_info.zhashpawn == 0) _info.zhashpawn = h;
#endif

    return _info.zhashpawn;
}

std::string Position::GetFENShort ()const{

    // "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR"

    std::stringstream ss;
    int count = 0;
    for (int i = 7 ; i >= 0 ; --i){
        for (int j = 0 ; j < 8 ; j++){
            int k =8*i+j;
            if ( Get(k)==Piece::t_empty){
                ++count;
            }
            else{
                if ( count != 0){
                    ss << count;
                    count = 0;
                }
                ss << Piece::Symbol(Get(k));
            }
            if ( j == 7 ){
                if ( count != 0){
                    ss << count;
                    count = 0;
                }
                if ( i!=0 ){
                    ss << "/";
                }
            }
        }
    }

    return ss.str();
}

std::string Position::GetFENShort2()const {

    // "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d5"

    std::stringstream ss;

    ss << GetFENShort();

    ss << " " << (Turn() == NSColor::c_white ? "w" : "b") << " ";

    switch (_info.white.CastlingRight()) {
    case Player::cr_none:
        break;
    case Player::cr_ks:
        ss << "K";
        break;
    case Player::cr_qs:
        ss << "Q";
        break;
    case Player::cr_qs + Player::cr_ks:
        ss << "KQ";
        break;
    }

    switch (_info.black.CastlingRight()) {
    case Player::cr_none:
        ss << "-";
        break;
    case Player::cr_ks:
        ss << "k";
        break;
    case Player::cr_qs:
        ss << "q";
        break;
    case Player::cr_qs + Player::cr_ks:
        ss << "kq";
        break;
    }

    if (Square::IsValid(_info.enPassant)) {
        ss << " " << Square(_info.enPassant).position();
    }
    else {
        ss << " -";
    }

    return ss.str();
}


std::string Position::GetFEN()const{

    // "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d5 0 2"

    //if ( _info.fen[0] != '\0' ) return _info.fen;

    std::stringstream ss;

    ss << GetFENShort2();

    ss << " " << (int)_info.halfMovesSinceLastPawnMoveOrCapture << " " << (int)_info.movesCount;

    return ss.str();
}

NSColor::eColor Position::SwitchColor(const NSColor::eColor & color){
   return NSColor::eColor( (color+1) % 2);
}

void Position::SwitchPlayer(){
    // update hash
    _XORHash(Util::Zobrist::turn[Turn()]);
    _c = SwitchColor(_c);
    // update hash
    _XORHash(Util::Zobrist::turn[Turn()]);

    /*
    // XOR out enPassant
    if ( Square::IsValid(_info.enPassant)){
       XORHash(Util::Zobrist::enPassant[FAST_MOD_8(_info.enPassant)][Turn()]);
       // invalidate en passant
       Square::InvalidateLightSquare(_info.enPassant);
    }
    ///@todo null move CARE CARE
    */

    InvalidateFen();
}

int Position::GamePhase()const {

    if (_info.gamePhase != 0) return _info.gamePhase;

    // between 0 and Position::maxGamePhase (may be more if strange promotion)
    int p =   NumberOf(Piece::t_Wknight)
        +     NumberOf(Piece::t_Wbishop)
        + 2 * NumberOf(Piece::t_Wrook)
        + 3 * NumberOf(Piece::t_Wqueen)
        +     NumberOf(Piece::t_Bknight)
        +     NumberOf(Piece::t_Bbishop)
        + 2 * NumberOf(Piece::t_Brook)
        + 3 * NumberOf(Piece::t_Bqueen);
    _info.gamePhase = std::min(p,maxGamePhase);
    return _info.gamePhase;
}

int Position::GamePhasePercent()const{
    return int(float(GamePhase()) / Position::maxGamePhase * 100);
}

bool Position::IsEndGame(const Position & p){
    return p.GamePhasePercent() < endGameGamePhasePercent;
}

bool Position::IsVeryEndGame(const Position & p){
   return Util::NumberOfWhiteMajorPiece(p) == 0
       || Util::NumberOfBlackMajorPiece(p) == 0;
}

bool Position::IsEndGame()const{
   return IsEndGame(*this);
}

bool Position::IsVeryEndGame()const{
   return IsVeryEndGame(*this);
}
