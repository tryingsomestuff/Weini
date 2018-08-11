#pragma once

#include "analyse.h"
#include "bitboard.h"
#include "definitions.h"
#include "line.h"
#include "logger.hpp"
#include "player.h"
#include "search.h"
#include "UtilMove.h"

#include <algorithm>
#include <random>

namespace Sort{

template < typename T, template<typename> class C >
void Shuffle(C<T> & v) {
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(v), std::end(v), rng);
}

struct SortMovesFunctor {
    NSColor::eColor color;
    const Position & _p;
    const Searcher & _s;
    SortMovesFunctor(NSColor::eColor c, const Position & p, const Searcher & searcher) : color(c), _p(p), _s(searcher) {
    }
    bool operator()(const Move & m1, const Move & m2){
        //LOG(logINFO) << m1.Show() << " vs " << m2.Show();

        ScoreType s1 = m1.SortScore();
        ScoreType s2 = m2.SortScore();
        UtilMove::eSpecialMove t1 = m1.Type();
        UtilMove::eSpecialMove t2 = m2.Type();

        float taperedCoeff = (_p.GamePhasePercent() - Position::endGameGamePhasePercent) / (100.f - Position::endGameGamePhasePercent);

        if ( s1 == Definitions::scores.defaultMoveScore){
            if ( t1 == UtilMove::sm_standard || t1 == UtilMove::sm_ep){
                s1 = 0;
                const Square::LightSquare & from1 = m1.From();
                const Square::LightSquare & to1   = m1.To();

                const Piece::eType t = _p.Get(from1);

                // positionnal heuristic
                if ( ! m1.IsCapture() && Definitions::sortingConfig.do_sortpositional ){
                   s1 += (color == NSColor::c_white ? +1 : -1 ) * (Analyse::HelperPosition(_p,to1,t,taperedCoeff) - Analyse::HelperPosition(_p,from1,t,taperedCoeff));
                }

                // en passant bonus
                if ( Definitions::sortingConfig.do_sortenpassantbonus && t1 == UtilMove::sm_ep){
                    s1 += Definitions::scores.epSortScore;
                }

                // check first
                if ( Definitions::sortingConfig.do_sortcheckbonus && m1.IsCheck() ){ // but check information is probably no available yet ...
                    s1 += Definitions::scores.checkSortScore;
                }

                // MVV-LVA
                if ( Definitions::sortingConfig.do_sortmvvlva && m1.IsCapture()){
                    s1 += UtilMove::MvvLvaScores[TypeToInt(_p.Get(to1))][TypeToInt(t)] + Definitions::scores.captureSortScore;
                }
                // SEE
                if (Definitions::sortingConfig.do_sortsee && m1.IsCapture()) {
                    if (!Searcher::SEE(_p, m1, 0)) {
                        //s1 -= 2* Definitions::scores.captureSortScore;
                        s1 *= -1;
                    }
                }

                // Capture last moved piece bonus
                if (Definitions::sortingConfig.do_captureLastMovedBonus && m1.IsCapture() && m1.To() == _p.Info().lastMove.to) {
                    s1 += Definitions::scores.captureLastMovedBonus;
                }

                // Killers & history
                if ( _p.WhiteToPlay()){
                    if(_s.killerMovesWhite[0][_p.CurrentPly()] == m1.ZHash()) {
                        s1 += Definitions::scores.killer1SortScore; // first !
                    }
                    else if(_s.killerMovesWhite[1][_p.CurrentPly()] == m1.ZHash()) {
                        s1 += Definitions::scores.killer2SortScore; // second !
                    }
                    else {
                        // History
                        if ( Definitions::sortingConfig.do_historyHeuristic && !m1.IsCapture() ){
                            s1 += (_s.searchHistory[TypeToInt(t)][(int)to1]);
                        }
                    }
                }
                else if ( !_p.WhiteToPlay()){
                    if(_s.killerMovesBlack[0][_p.CurrentPly()] == m1.ZHash()) {
                        s1 += Definitions::scores.killer1SortScore; // first !
                    }
                    else if(_s.killerMovesBlack[1][_p.CurrentPly()] == m1.ZHash()) {
                        s1 += Definitions::scores.killer2SortScore; // second !
                    }
                    else {
                        // History
                        if ( Definitions::sortingConfig.do_historyHeuristic && !m1.IsCapture() ){
                            s1 += (_s.searchHistory[TypeToInt(t)][(int)to1]);
                        }
                    }
                }

                // counter
                if (Definitions::sortingConfig.do_counterHeuristic && !m1.IsCapture()
                         && m1.Type() == UtilMove::sm_standard) { // ensure from and to are valid ... (so skip castling)
                    if (_s.counterMove[_p.Info().lastMove.from][_p.Info().lastMove.to] == m1.ZHash()) {
                        s1 += Definitions::scores.counterSortScore;
                    }
                }

            }
            else{
                s1 = Definitions::scores.promotionSortScore; // special moves first !

                // promotion
                if ( t1 > UtilMove::sm_castling_max){
                    ///@todo this is a highly dangerous optim to avoid the switch
                    s1 += Piece::Value(Piece::eType(10 - t1));
                    /*
                    switch(t1){
                    case UtilMove::sm_pn:
                        s1 += Piece::Value(Piece::t_Wknight);
                        break;
                    case UtilMove::sm_pb:
                        s1 += Piece::Value(Piece::t_Wbishop);
                        break;
                    case UtilMove::sm_pr:
                        s1 += Piece::Value(Piece::t_Wrook);
                        break;
                    case UtilMove::sm_pq:
                        s1 += Piece::Value(Piece::t_Wqueen);
                        break;
                    }
                    */
                }
            }

            //update move score
            m1.SetSortScore(s1);
        }

        if ( s2 == Definitions::scores.defaultMoveScore){
            if ( t2 == UtilMove::sm_standard || t2 == UtilMove::sm_ep){
                s2 = 0;
                const Square::LightSquare & from2 = m2.From();
                const Square::LightSquare & to2   = m2.To();

                const Piece::eType t = _p.Get(from2);

                // positionnal heuristic
                if ( ! m2.IsCapture() && Definitions::sortingConfig.do_sortpositional ){
                   s2 += (color == NSColor::c_white ? +1 : -1 ) * (Analyse::HelperPosition(_p,to2,t,taperedCoeff) - Analyse::HelperPosition(_p,from2,t,taperedCoeff));
                }

                // en passant bonus
                if ( Definitions::sortingConfig.do_sortenpassantbonus && t2 == UtilMove::sm_ep){
                    s2 += Definitions::scores.epSortScore;
                }

                // check first
                if ( Definitions::sortingConfig.do_sortcheckbonus && m2.IsCheck() ){
                    s2 += Definitions::scores.checkSortScore;
                }

                // MVV-LVA
                if ( Definitions::sortingConfig.do_sortmvvlva && m2.IsCapture()){
                    s2 += UtilMove::MvvLvaScores[TypeToInt(_p.Get(to2))][TypeToInt(t)] + Definitions::scores.captureSortScore;
                }
                // SEE
                if ( Definitions::sortingConfig.do_sortsee && m2.IsCapture()){
                    if (!Searcher::SEE(_p, m2, 0)) {
                        //s2 -= 2 * Definitions::scores.captureSortScore;
                        s2 *= -1;
                    }
                }

                // Capture last moved piece bonus
                if (Definitions::sortingConfig.do_captureLastMovedBonus && m2.IsCapture() && m2.To() == _p.Info().lastMove.to) {
                    s2 += Definitions::scores.captureLastMovedBonus;
                }

                // Killers & history
                if ( _p.WhiteToPlay()){
                    if(_s.killerMovesWhite[0][_p.CurrentPly()] == m2.ZHash()) {
                        s2 += Definitions::scores.killer1SortScore; // first !
                    }
                    else if(_s.killerMovesWhite[1][_p.CurrentPly()] == m2.ZHash()) {
                        s2 += Definitions::scores.killer2SortScore; // second !
                    }
                    else {
                        // History
                        if ( Definitions::sortingConfig.do_historyHeuristic && !m2.IsCapture() ){
                            s2 += (_s.searchHistory[TypeToInt(t)][(int)to2]);
                        }
                    }
                }
                else if ( !_p.WhiteToPlay()){
                    if(_s.killerMovesBlack[0][_p.CurrentPly()] == m2.ZHash()) {
                        s2 += Definitions::scores.killer1SortScore; // first !
                    }
                    else if(_s.killerMovesBlack[1][_p.CurrentPly()] == m2.ZHash()) {
                        s2 += Definitions::scores.killer2SortScore; // second !
                    }
                    else {
                        // History
                        if ( Definitions::sortingConfig.do_historyHeuristic && !m2.IsCapture() ){
                            s2 += (_s.searchHistory[TypeToInt(t)][(int)to2]);
                        }
                    }
                }

                // counter
                if (Definitions::sortingConfig.do_counterHeuristic && !m2.IsCapture()
                         && m2.Type() == UtilMove::sm_standard) { // ensure from and to are valid ... (so skip castling))
                    if (_s.counterMove[_p.Info().lastMove.from][_p.Info().lastMove.to] == m2.ZHash()) {
                        s2 += Definitions::scores.counterSortScore;
                    }
                }

            }
            else{
                s2 = Definitions::scores.promotionSortScore; // special moves first !

                // promotion
                if ( t2 > UtilMove::sm_castling_max){
                    ///@todo this is a highly dangerous optim to avoid the switch
                    s2 += Piece::Value(Piece::eType(10 - t2));
                    /*
                    switch(t2){
                    case UtilMove::sm_pn:
                        s2 += Piece::Value(Piece::t_Wknight);
                        break;
                    case UtilMove::sm_pb:
                        s2 += Piece::Value(Piece::t_Wbishop);
                        break;
                    case UtilMove::sm_pr:
                        s2 += Piece::Value(Piece::t_Wrook);
                        break;
                    case UtilMove::sm_pq:
                        s2 += Piece::Value(Piece::t_Wqueen);
                        break;
                    }
                    */
                }
            }

            //update move score
            m2.SetSortScore(s2);
        }

        return s1 > s2;
    }
};

struct SortQMovesFunctor {
    NSColor::eColor color;
    const Position & _p;
    SortQMovesFunctor(NSColor::eColor c, const Position & p) : color(c), _p(p) {
    }
    bool operator()(const Move & m1, const Move & m2){
        //LOG(logINFO) << m1.Show() << " vs " << m2.Show();

        ScoreType s1 = m1.SortScore();
        ScoreType s2 = m2.SortScore();
        UtilMove::eSpecialMove t1 = m1.Type();
        UtilMove::eSpecialMove t2 = m2.Type();

        if ( s1 == Definitions::scores.defaultMoveScore){
            s1 = 0;

            const Square::LightSquare & from1 = m1.From();
            const Square::LightSquare & to1   = m1.To();
            const Piece::eType t = _p.Get(from1);

            // en passant bonus
            if ( Definitions::sortingConfig.do_sortenpassantbonus && t1 == UtilMove::sm_ep){
                s1 += Definitions::scores.epSortScore;
            }

            // check first
            if ( Definitions::sortingConfig.do_sortcheckbonus && m1.IsCheck() ){ // but check information is probably no available yet ...
                s1 += Definitions::scores.checkSortScore;
            }

            // MVV-LVA
            if ( Definitions::sortingConfig.do_qsortmvvlva && m1.IsCapture()){
                s1 += UtilMove::MvvLvaScores[TypeToInt(_p.Get(to1))][TypeToInt(t)] + Definitions::scores.captureSortScore;
            }
            // SEE
            if (Definitions::sortingConfig.do_sortsee && m1.IsCapture()) {
                if (!Searcher::SEE(_p, m1, 0)) {
                    //s1 -= 2 * Definitions::scores.captureSortScore;
                    s1 *= -1;
                }
            }

            // Capture last moved piece bonus
            if (Definitions::sortingConfig.do_captureLastMovedBonus && m1.IsCapture() && m1.To() == _p.Info().lastMove.to) {
                s1 += Definitions::scores.captureLastMovedBonus;
            }

            //update move score
            m1.SetSortScore(s1);
        }

        if ( s2 == Definitions::scores.defaultMoveScore){
            s2 = 0;

            const Square::LightSquare & from2 = m2.From();
            const Square::LightSquare & to2   = m2.To();
            const Piece::eType t = _p.Get(from2);

            // en passant bonus
            if ( Definitions::sortingConfig.do_sortenpassantbonus && t2 == UtilMove::sm_ep){
                s2 += Definitions::scores.epSortScore;
            }

            // check first
            if ( Definitions::sortingConfig.do_sortcheckbonus && m2.IsCheck() ){
                s2 += Definitions::scores.checkSortScore;
            }
            // SEE
            if (Definitions::sortingConfig.do_sortsee && m2.IsCapture()) {
                if (!Searcher::SEE(_p, m2, 0)) {
                    //s2 -= 2 * Definitions::scores.captureSortScore;
                    s2 *= -1;
                }
            }

            // MVV-LVA
            if ( Definitions::sortingConfig.do_qsortmvvlva && m2.IsCapture()){
                s2 += UtilMove::MvvLvaScores[TypeToInt(_p.Get(to2))][TypeToInt(t)] + Definitions::scores.captureSortScore;
            }

            // Capture last moved piece bonus
            if (Definitions::sortingConfig.do_captureLastMovedBonus && m2.IsCapture() && m2.To() == _p.Info().lastMove.to) {
                s2 += Definitions::scores.captureLastMovedBonus;
            }

            //update move score
            m2.SetSortScore(s2);
        }

        return s1 > s2;
    }
};

enum SortPhase{
    SP_all   = 0,
    SP_fisrt = 1, ///@todo bug, trying to use this leads to issue with TT ...
    SP_last  = 2
};

static inline
void SortMoves(FastContainer<Move> & moves,const Position & p, const Searcher & searcher, SortPhase sortPhase){
    //debug
    int k = 0;
    for(auto it = moves.begin() ; it != moves.end() ; ++it){
        if ( !(*it).IsValid() ) {
           LOG(logFATAL) << "Trying to sort a list with invalid move ! " << k;
        }
        ++k;
    }
    switch(sortPhase){
    case SP_all:
        moves.sort(SortMovesFunctor(p.Turn(), p,searcher));
        break;
    case SP_fisrt:
        moves.partial_sort(SortMovesFunctor(p.Turn(), p,searcher));
        break;
    case SP_last:
        moves.partial_sort_end(SortMovesFunctor(p.Turn(), p,searcher));
        break;
    }
}

static inline
void SortQMoves(FastContainer<Move> & moves,const Position & p){
    int k = 0;
    for(auto it = moves.begin() ; it != moves.end() ; ++it){
        if ( (*it).ZHash() == 0 ) {
           LOG(logFATAL) << "Trying to sort a q list with zero hash ! " << k;
        }
        if ( !(*it).IsValid() ) {
           LOG(logFATAL) << "Trying to sort a q list with invalid move ! " << k;
        }
        ++k;
    }
    moves.sort(SortQMovesFunctor(p.Turn(), p));
    ///@todo use sortPhase ?
}

static inline
void ClearScore(FastContainer<Move> & moves){
    for(auto it = moves.begin() ; it != moves.end() ; ++it){
        // we don't reset score when move is a checkmate against "max" player
        // because possible reorder (PV, TT) and LMR can lose this information !
        //if ( (*it).SortScore() > -( Definitions::checkMateScore - MAX_GAME_PLY) ){
          (*it).SetSortScore(Definitions::scores.defaultMoveScore);
        //}
    }
}

static inline
bool PVFirst(const Line & pv, int depth, FastContainer<Move> & moves){
   if (depth >= MAX_SEARCH_DEPTH ){
      LOG(logFATAL) <<"Looking for a PV with depth >= MAX_SEARCH_DEPTH";
   }
   if ( pv.n > depth && depth >= 0 ){
      Move move(pv.moves[depth]);
      auto it = std::find(moves.begin(), moves.end(), move);
      if (it != moves.end()) {
          moves.moveToFront(it - moves.begin());
          moves.front().SetSortScore(moves.front().SortScore() + Definitions::scores.pvSortScore);
          return true;
      }
   }

   /*
   if ( pv.n > depth ){
      LOG(logINFO) << "PV move  was " << Move(pv.moves[depth]).Show() << " (" << depth << "," << pv.n << ")";
      LOG(logINFO) << "PV moves was " << pv.ShowPV();
      UtilMove::DisplayList(moves,"Current moves");
   }
   */

   return false;
}

static inline
bool IIDFirst(const SearcherBase::SearchedMove & iid,FastContainer<Move> & moves){
    if ( SearcherBase::GetMove(iid).IsValid()){
      auto it = std::find(moves.begin(), moves.end(), SearcherBase::GetMove(iid));
      if (it != moves.end()) {
          moves.moveToFront(it - moves.begin());
          moves.front().SetSortScore(moves.front().SortScore() + Definitions::scores.iidSortScore);
          return true;
      }
    }
    return false;
}


}
