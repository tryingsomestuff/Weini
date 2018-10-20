#include "game.h"
#include "logger.hpp"
#include "position.h"

Game::Game(){
   memset(_gameHashes,0,MAX_GAME_PLY*sizeof(Util::Zobrist::HashType));
}

void Game::SetHash(const Position & p, int ply, bool moveWasCapture ){
    if ( ply == -1 ){
        ply = p.CurrentPly();
    }
    _gameHashes[ply] = p.GetZHash();
    _isCapture[ply]  = moveWasCapture;
#ifdef DEBUG_GAME_HASH
    _gameFEN[ply]    = p.GetFENShort2();
#endif
}

void Game::SetHash(const Util::Zobrist::HashType & h, int ply, bool moveWasCapture ){
    _gameHashes[ply] = h;
    _isCapture[ply]  = moveWasCapture;
#ifdef DEBUG_GAME_HASH
    _gameFEN[ply]    = p.GetFENShort2();
#endif
}

void Game::Display(Util::Zobrist::HashType h, DepthType currentPly)const{
    if ( h!=0 ){
        LOG(logINFO) << "Current hash " << h;
    }
    for( int k = 0 ; k < MAX_GAME_PLY ; ++k){
        if ( _gameHashes[k] == 0 ){
            break;
        }
        LOG(logINFO) << k
                     << " " << _gameHashes[k] << ((h!=0 && _gameHashes[k]==h) ? "*" : " ")  << " " << (_isCapture[k]?"c":" ") << ((currentPly==k)?"<<==":"")
#ifdef DEBUG_GAME_HASH
                     << "\t" << _gameFEN[k]
#endif
                       ;
    }
    LOG(logINFO) << "**********************************" ;
    LOG(logINFO) << "";
}

bool Game::Is3RepDraw(const Position &p, DepthType nbRep, bool displayDebug)const {
    const Util::Zobrist::HashType* game = GameHashes();
    const bool* caps = GameCaptures();
    const Util::Zobrist::HashType h = game[p.CurrentPly()];
    if ( h == 0 ){
        Display();
        LOG(logFATAL) << "Null h at current position ... " << p.CurrentPly();
    }
    int c = 0;
    if ( p.CurrentPly() < 2 ) return false;
    for (int k = p.CurrentPly()-2 ; k >= 0; k-=2) {
        if (game[k] == h){
          ++c;
#ifdef DEBUG_GAME_HASH
          if ( _gameFEN[k] != p.GetFENShort2()){
              Display(h,k);
              LOG(logFATAL) << "3rep draw detected with different FEN !";
          }
#endif
        }
        if (c >= nbRep) {
            if (displayDebug) {
                LOG(logINFO) << "Draw, 3 repetitions rule !";
                Display(h,k);
            }
            return true;
        }
        if ( caps[k] ) break; // don't look for 3 reps rules "before" a capture
    }

    return false;
}
