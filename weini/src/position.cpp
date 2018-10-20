#include "position.h"
#include "move.h"
#include "analyse.h"
#include "moveGenerator.h"
#include "UtilPosition.h"
#include "game.h"


#include <vector>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <numeric>
#include <locale>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <functional>
#include <sstream>
#include <iostream>
#include <iterator>
#include <random>
#include <locale>
#include <fstream>
#include <iomanip>
#include <bitset>

std::string Position::startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const int Position::maxGamePhase = 22;
const int Position::endGameGamePhasePercent = 25;

namespace {
    // trim from start
    std::string ltrim(const std::string &i) {
        std::string s(i);
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }
}

///@todo this CTOR MUST implement some "ispositionvalid" stuff !!! to be used in SideToMoveFromFEN
Position::Position(const std::string & s, bool withMoveCount):
    _c(NSColor::c_none),
    _zhash(0),
    _zhashpawn(0),
    _gamePhase(0),
    _enPassant(Square::Invalide),
    _halfMovesSinceLastPawnMoveOrCapture(0),
    _currentMovesCount(0),
    _currentply(0),
    _isInCheck(false)
{

    // this is needed in order to init zhash below!
    _ClearBoard();

    // shortcut
    std::string fen = ltrim(s);
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
    _isInCheck  = UtilMove::KingIsChecked(*this);

    GetZHash(); // this will set up zhash !

    ///@todo : are both king checked ? : or other invalid position (missing king, ...)
}

Position::~Position() {

}

void Position::_RemoveCastlingRight(Player & player, const Player::eCastlingRight & cr){
   if ( player.CastlingRight() && cr ){
      _XORHash(Util::Zobrist::castlingRights[player.CastlingRight()][player.Color()]);
      player.RemoveCastlingRight(cr);
      _XORHash(Util::Zobrist::castlingRights[player.CastlingRight()][player.Color()]);
   }
}

void Position::ApplyMove(const Move & m, Game * game, bool verifyKingCapture) {

    if (! m.IsValid()) {
        LOG(logFATAL) << "Invalid move ! " << GetFEN() << " " << m.Show();
    }

    if ( CurrentPly() >= MAX_GAME_PLY-1 ){
        LOG(logFATAL) << "Game is too long ! " << MAX_GAME_PLY << " " << CurrentPly() << " " << GetFEN();
    }

    //LOG(logWARNING) << "Hash before apply " << GetZHash();

    // get current player
    Player & player   = Turn() == NSColor::c_white ? dynamic_cast<Player&>(WhitePlayer()) : dynamic_cast<Player&>(BlackPlayer());
    Player & opponent = Turn() != NSColor::c_white ? dynamic_cast<Player&>(WhitePlayer()) : dynamic_cast<Player&>(BlackPlayer());

    // some shortcuts
    UtilMove::eSpecialMove moveType  = m.Type();
    const Square::LightSquare & from = m.From();
    const Square::LightSquare & to   = m.To();

    // special cases first : castling & promotion
    if ( moveType != UtilMove::sm_standard && moveType != UtilMove::sm_ep ){

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
                if ( game ) game->InfoRef(CurrentPly()+1).lastMove = { Square::squaree1,Square::squaref1,UtilMove::sm_Ks };
                SetWhiteKing(Square::squareg1); // update king square
                SetPiece(Square::squareg1,Piece::t_Wking);
                SetPiece(Square::squareh1,Piece::t_empty);
                break;
            case UtilMove::sm_ks:
                SetPiece(Square::squaree8,Piece::t_empty);
                SetPiece(Square::squaref8,Piece::t_Brook);
                // last landing position is the rook one, king should be safe !
                if ( game ) game->InfoRef(CurrentPly()+1).lastMove = { Square::squaree8,Square::squaref8,UtilMove::sm_ks };
                SetBlackKing(Square::squareg8); // update king square
                SetPiece(Square::squareg8,Piece::t_Bking);
                SetPiece(Square::squareh8,Piece::t_empty);
                break;
            case UtilMove::sm_Qs:
                SetPiece(Square::squarea1,Piece::t_empty);
                SetPiece(Square::squareb1,Piece::t_empty);
                SetWhiteKing(Square::squarec1); // update king square
                SetPiece(Square::squarec1,Piece::t_Wking);
                SetPiece(Square::squared1,Piece::t_Wrook);
                // last landing position is the rook one, king should be safe !
                if ( game ) game->InfoRef(CurrentPly()+1).lastMove = { Square::squaree1,Square::squared1,UtilMove::sm_Qs };
                SetPiece(Square::squaree1,Piece::t_empty);
                break;
            case UtilMove::sm_qs:
                SetPiece(Square::squarea8,Piece::t_empty);
                SetPiece(Square::squareb8,Piece::t_empty);
                SetBlackKing(Square::squarec8); // update king square
                SetPiece(Square::squarec8,Piece::t_Bking);
                SetPiece(Square::squared8,Piece::t_Brook);
                // last landing position is the rook one, king should be safe !
                if ( game ) game->InfoRef(CurrentPly()+1).lastMove = { Square::squaree8,Square::squared8,UtilMove::sm_qs };
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
            ++_halfMovesSinceLastPawnMoveOrCapture;

        } // end if castling
        else{ // now treat promotion

            const Piece::eType pieceFrom     = Get(from);
            const Piece::eType pieceTo       = Get(to);

            if ( verifyKingCapture && Piece::IsK(pieceTo) ){
               LOG(logFATAL) << "Capturing king ! " << GetFEN() << " " << m.Show();
            }

            // verify capture
            bool capture = false;
            if (pieceTo != Piece::t_empty) {
                if ( !m.IsCapture() ){
                    LOG(logFATAL) << "Missed capture during move validation " << GetFEN() << " " << m.Show();
                }
                capture = true;
    #ifdef WITH_CAPTURED
                player.captured[player.capturedN++] = pieceTo;
    #endif
                if ( game ) game->InfoRef(CurrentPly()+1).lastCapturedPiece = pieceTo;
            }

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
                if ( game ) game->InfoRef(CurrentPly()+1).lastMove = m.GetMini();

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
                if ( game ) game->InfoRef(CurrentPly()+1).lastMove = m.GetMini();
            }
            else{
                LOG(logFATAL) << "ERROR: bad promotion " << Square(from).position() << " " << Square(to).position() << ", " << m.Show() << " " << GetFEN();
            }

            // update opponent castling rights
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
            _halfMovesSinceLastPawnMoveOrCapture = 0;

        } // end promotions

        // XOR out enPassant for promotion and castling
        if ( Square::IsValid(EnPassant())){
           _XORHash(Util::Zobrist::enPassant[FILE(EnPassant())][Opponent()]);
           // invalidate en passant
           Square::InvalidateLightSquare(EnPassant());
        }
    }
    else{
        // standard moves and ep

        const Piece::eType pieceFrom     = Get(from);
        const Piece::eType pieceTo       = Get(to);

        if ( verifyKingCapture && Piece::IsK(pieceTo) ){
           LOG(logFATAL) << "Capturing king ! " << GetFEN() << " " << m.Show();
        }

        //LOG(logDEBUG) << "Move from " << Square(from).position() << " to " << Square(to).position() ;

        // verify capture
        bool capture = false;
        if (pieceTo != Piece::t_empty) {
            if ( !m.IsCapture() ){
                LOG(logFATAL) << "Missed capture during move validation " << GetFEN() << " " << m.Show();
            }
            capture = true;
#ifdef WITH_CAPTURED
            player.captured[player.capturedN++] = pieceTo;
#endif
            if ( game ) game->InfoRef(CurrentPly()+1).lastCapturedPiece = pieceTo;
        }

        // check for en passant capture
        if ( moveType == UtilMove::sm_ep ){
            if ( !m.IsCapture() ){
                LOG(logFATAL) << "Missed en-passant capture during move validation " << GetFEN() << " " << m.Show();
            }
            //LOG(logDEBUG) << "Apply en-passant capture" ;
            capture = true;
#ifdef WITH_CAPTURED
            player.captured[player.capturedN++] = Get(EnPassant());
#endif
            if ( game ) game->InfoRef(CurrentPly()+1).lastCapturedPiece = Get(EnPassant());
            SetPiece(EnPassant(),Piece::t_empty);
        }

        // update (disable) en-passant
        if ( Square::IsValid(EnPassant())){
           //LOG(logINFO) << "XORing out en passant " << Square(EnPassant()).position();
           // XOR out enPassant (which was set using opponent hash)
           _XORHash(Util::Zobrist::enPassant[FILE(EnPassant())][Opponent()]);
           // invalidate en passant
           Square::InvalidateLightSquare(EnPassant());
        }

        // update (enable) en-passant
        if (  ( pieceFrom == Piece::t_Wpawn && to-from ==  16 )
            ||( pieceFrom == Piece::t_Bpawn && to-from == -16 ) ){
            //LOG(logDEBUG) << "Set possible en-passant to " << to.position() ;
            EnPassant() = to;
            // XOR in enPassant
            //LOG(logINFO) << "XORing in en passant " << Square(EnPassant()).position();
            _XORHash(Util::Zobrist::enPassant[FILE(EnPassant())][Turn()]);
        }

        // update (invalidate) lastCapturedPiece if needed
        if ( ! capture){
            if ( game ) game->InfoRef(CurrentPly()+1).lastCapturedPiece = Piece::t_empty;
        }

        // update player castling rights
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

        // update opponent castling rights
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
        if (pieceFrom == Piece::t_Bpawn || pieceFrom == Piece::t_Wpawn || capture){
            _halfMovesSinceLastPawnMoveOrCapture = 0;
        }
        else{
            ++_halfMovesSinceLastPawnMoveOrCapture;
        }

        if (capture) {
            // invalidate gamePhase only if touching a not-empty Square
            _gamePhase = 0;
        }

        // last landing position
        if ( game ) game->InfoRef(CurrentPly()+1).lastMove = m.GetMini();

        // update kings position on the fly
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
    _isInCheck = m.IsCheck();
    /*
    if ( IsInCheck() ){
        LOG(logWARNING) << "Move give check " << GetFEN() << " after " << m.Show();
    }
    */

    // Update move count
    if (_c == NSColor::c_white){
        ++_currentMovesCount;
    }

    if ( CurrentPly() >= MAX_GAME_PLY-1 ){
        LOG(logFATAL) << "Game is too long ! " << MAX_GAME_PLY << " " << CurrentPly() << " " << GetFEN();
    }
    ++_currentply;

}

void Position::Display()const {
    LOG(logINFO) << " ===================== Position display (begin) ==================== ";
    LOG(logINFO) << "Move                " << CurrentMoveCount();
    LOG(logINFO) << "HalfMove (50 rule)  " << HalfMoves50Rule() ;
    LOG(logINFO) << "Ply                 " << CurrentPly()      ;
    LOG(logINFO) << "Possible en passant " << (Square::IsValid(EnPassant())?Square(EnPassant()).position():"none") ;
    LOG(logINFO) << ((_c==NSColor::c_white)? "White":"Black") << " to play" ;
    if ( IsEndGame()){
       LOG(logINFO) << "This is an end-game !";
    }
    if ( IsVeryEndGame()){
       LOG(logINFO) << "This is a very end-game !";
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
    for (unsigned char k = 0; k < WhitePlayer().capturedN; ++k) {
        ss << Piece::Symbol(WhitePlayer().captured[k]) << " ";
    }
    ss << std::endl;
    LOG(logINFO) << ss.str();
#endif


#ifdef WITH_CAPTURED
    ss.str("");
    ss << "Captured by black : ";
    for (unsigned int k = 0; k < BlackPlayer().capturedN; ++k) {
        ss << Piece::Symbol(BlackPlayer().captured[k]) << " ";
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
            WhitePlayer().AddCastlingRight(Player::cr_ks);
            found = true;
        }
        if (strList[2].find('Q') != std::string::npos){
            //LOG(logDEBUG) << "Adding Qs" ;
            WhitePlayer().AddCastlingRight(Player::cr_qs);
            found = true;
        }
        if (strList[2].find('k') != std::string::npos){
            //LOG(logDEBUG) << "Adding ks" ;
            BlackPlayer().AddCastlingRight(Player::cr_ks);
            found = true;
        }
        if (strList[2].find('q') != std::string::npos){
            //LOG(logDEBUG) << "Adding qs" ;
            BlackPlayer().AddCastlingRight(Player::cr_qs);
            found = true;
        }
        if ( ! found ){
            //LOG(logWARNING) << "No castling right given" ;
        }
    }
    else{
        //LOG(logWARNING) << "No castling right given" ;
    }

    // read en passant and save it
    if ((strList.size() >= 4) && strList[3] != "-" ){
        if (strList[3].length() >= 2){
            ///@todo we have an inner wrong representation of en-passant, incompatible with fen !
            if ( strList[3].at(1) == '3' ) strList[3].at(1) = '4';
            if ( strList[3].at(1) == '6' ) strList[3].at(1) = '5';
            if ((strList[3].at(0) >= 'a') && (strList[3].at(0) <= 'h') &&
                    ((strList[3].at(1) == '4') || (strList[3].at(1) == '5'))){
                EnPassant() = Square(strList[3]).index();
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
        _halfMovesSinceLastPawnMoveOrCapture = tmp;
    }
    else{
        _halfMovesSinceLastPawnMoveOrCapture = 0;
    }

    // read number of move
    if (strList.size() >= 6){
        std::stringstream ss(strList[5]);
        int tmp;
        ss >> tmp;
        _currentMovesCount = tmp;
    }
    else{
        _currentMovesCount = 1;
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
    if ( _zhash != 0 ) return _zhash;
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
        if ( Square::IsValid(EnPassant()) ){
            h ^= Util::Zobrist::enPassant[FILE(EnPassant())][Opponent()];
        }
        h ^= Util::Zobrist::turn[Turn()];
        h ^= Util::Zobrist::castlingRights[WhitePlayer().CastlingRight()][0];
        h ^= Util::Zobrist::castlingRights[BlackPlayer().CastlingRight()][1];
    }

#ifdef DEBUG_HASH_RECOMPUTE
    if (_zhash != 0 && _zhash != h) {
        LOG(logFATAL) << "Hash error " << h << " " << _zhash << " " << GetFEN();
    }
#endif

#ifndef DEBUG_HASH_RECOMPUTE
    _zhash = h;
#else
    if (_zhash == 0) _zhash = h;
#endif

    return _zhash;
}

Util::Zobrist::HashType Position::GetZHashPawn  ()const{

#ifndef DEBUG_HASH_RECOMPUTE
    if ( _zhashpawn != 0 ) return _zhashpawn;
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
    if (_zhashpawn != 0 && _zhashpawn != h) {
        LOG(logFATAL) << "Hash error " << h << " " << _zhashpawn << " " << GetFEN();
    }
#endif

#ifndef DEBUG_HASH_RECOMPUTE
    _zhashpawn = h;
#else
    if (_hashpawn == 0) _zhashpawn = h;
#endif

    return _zhashpawn;
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

    switch (WhitePlayer().CastlingRight()) {
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

    switch (BlackPlayer().CastlingRight()) {
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

    if (Square::IsValid(EnPassant())) {
        ss << " " << Square(EnPassant()).position();
    }
    else {
        ss << " -";
    }

    return ss.str();
}


std::string Position::GetFEN()const{

    // "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d5 0 2"

    std::stringstream ss;

    ss << GetFENShort2();

    ss << " " << (int)HalfMoves50Rule() << " " << (int)CurrentMoveCount();

    return ss.str();
}

NSColor::eColor Position::SwitchColor(const NSColor::eColor & color){
   return NSColor::eColor( color ^ NSColor::c_black); // c_black is 1
}

void Position::SwitchPlayer(){
    // update hash
    _XORHash(Util::Zobrist::turn[Turn()]);
    _c = SwitchColor(_c);
    // update hash
    _XORHash(Util::Zobrist::turn[Turn()]);

    /*
    // XOR out enPassant
    if ( Square::IsValid(EnPassant())){
       XORHash(Util::Zobrist::enPassant[FAST_MOD_8(EnPassant())][Turn()]);
       // invalidate en passant
       Square::InvalidateLightSquare(EnPassant());
    }
    ///@todo null move CARE CARE
    */

}

void Position::SwitchPlayerNullMove(){
    // update hash
    _XORHash(Util::Zobrist::turn[Turn()]);
    _c = SwitchColor(_c);
    // update hash
    _XORHash(Util::Zobrist::turn[Turn()]);

    /*
    // XOR out enPassant
    if ( Square::IsValid(EnPassant())){
       XORHash(Util::Zobrist::enPassant[FAST_MOD_8(EnPassant())][Turn()]);
       // invalidate en passant
       Square::InvalidateLightSquare(EnPassant());
    }
    ///@todo null move CARE CARE
    */

    _currentply++;
}

int Position::GamePhase()const {

    if (_gamePhase != 0) return _gamePhase;

    // between 0 and Position::maxGamePhase (may be more if strange promotion)
    int p =   NumberOf(Piece::t_Wknight)
        +     NumberOf(Piece::t_Wbishop)
        + 2 * NumberOf(Piece::t_Wrook)
        + 3 * NumberOf(Piece::t_Wqueen)
        +     NumberOf(Piece::t_Bknight)
        +     NumberOf(Piece::t_Bbishop)
        + 2 * NumberOf(Piece::t_Brook)
        + 3 * NumberOf(Piece::t_Bqueen);
    _gamePhase = std::min(p,maxGamePhase); // info is mutable !
    return _gamePhase;
}

int Position::GamePhasePercent()const{
    return int(float(GamePhase()) / Position::maxGamePhase * 100);
}

bool Position::IsEndGame(const Position & p){
    return p.GamePhasePercent() < endGameGamePhasePercent;
}

bool Position::IsVeryEndGame(const Position & p){
    return p.GamePhasePercent() < endGameGamePhasePercent/2;
}

bool Position::IsEndGame()const{
   return IsEndGame(*this);
}

bool Position::IsVeryEndGame()const{
   return IsVeryEndGame(*this);
}
