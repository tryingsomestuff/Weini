#include "search.h"
#include "analyse.h"
#include "moveGenerator.h"
#include "ttable.h"
#include "book.h"
#include "sort.h"
#include "UtilMove.h"

#include <iterator>
#include <iomanip>

// Static Exchange Evaluation (from stockfish)
bool Searcher::SEE(const Position & p, const Move & m, ScoreType threshold){

    //LOG(logINFO) << "See from " << p.GetFEN() << " move " << m.Show();

    // =====================
    // who's playing ?
    const bool whiteToMove = p.WhiteToPlay();

#ifdef SEE_WITHOUT_CHECK_VALIDATION
    // =====================
    // No more king !
    // =====================
    if ((whiteToMove && p.NoWhiteKingAnymore()) || (!whiteToMove && p.NoBlackKingAnymore()) ){
        return false;
    }
#endif

    // Only deal with normal moves
    if (m.Type() != UtilMove::sm_standard) {
        return true;
    }

    Square::LightSquare from = m.From();
    Square::LightSquare to = m.To();
    Piece::eType nextVictim = p.Get(from);
    NSColor::eColor us = Piece::Color(nextVictim);
    NSColor::eColor stm = Position::SwitchColor(us); // First consider opponent's move

    //LOG(logINFO) << "us " << (us==NSColor::c_white ? "white" : "black");
    //LOG(logINFO) << "stm " << (stm == NSColor::c_white ? "white" : "black");
    //LOG(logINFO) << "nextVictim " << Piece::Name(nextVictim);

    // The opponent may be able to recapture so this is the best result
    // we can hope for.
    ScoreType balance = Piece::ValueAbs(p.Get(to)) - threshold;

    //LOG(logINFO) << "balance init " << balance;

    if (balance < 0) {
        //LOG(logINFO) << "balance < 0 " << balance;
        return false;
    }

    // Now assume the worst possible result: that the opponent can
    // capture our piece for free.
    balance -= Piece::ValueAbs(nextVictim);

    // If it is enough (like in PxQ) then return immediately.
    if (balance >= 0) {
        //LOG(logINFO) << "balance >= 0 " << balance;
        return true;
    }

#ifdef WITH_MEMORY_POOL_BUILDER
    ScopeClone<Position, _default_block_size> psc(p);
    Position & p2 = psc.Get();
#else
    Position p2(p);
#endif

    Move m2(m);

    UtilMove::DetectStandardCapture(m2, p2);
    if ( UtilMove::PossibleEnPassant(m2,p2) && !UtilMove::ValidateEnPassant(m2,p2)){
        LOG(logFATAL) << "move seems to be not a legal en-passant in this position " << p2.GetFEN() << ", move was " << m2.Show();
        return false;
    }

#ifndef SEE_WITHOUT_CHECK_VALIDATION
    if ( !Definitions::do_trustedGenerator && !UtilMove::ValidateCheckStatus(m2,p2) ){
        //LOG(logINFO) << "move cannot be applied " << p2.GetFEN() << " move was " << m2.Show();
        return false;
    }
    if ( !Definitions::do_trustedGenerator) UtilMove::ValidateIsCheck(m2,p2);
#endif

    FastContainer<Square::LightSquare> stmAttackers;
    p2.ApplyMove(m2,false); // apply the starting move

    //LOG(logINFO) << "move applied " << p2.GetFEN() << " move was " << m.Show();

    bool endOfSEE = false;

    while (!endOfSEE){
        stmAttackers.clear();
        // get threats
        p2.SwitchPlayer();
        bool threatsFound = UtilMove::GetThreads(p2, to, false, &stmAttackers);
        if (p2.Turn() == NSColor::c_white) {
            stmAttackers.sort(UtilMove::SortThreatsFunctor(p2));
        }
        else {
            stmAttackers.sort(UtilMove::ReverseSortThreatsFunctor(p2));
        }
        p2.SwitchPlayer();

        // If no more attackers then give up: current stm loses
        if (!threatsFound) {
            //LOG(logINFO) << "no more threat on " << Square(to).position() << " fen " << p2.GetFEN();
            endOfSEE = true;
            break;
        }

        bool validThreatFound = false;
        int threatId = 0;
        while (!validThreatFound && threatId < stmAttackers.size()) {

            Move mm(stmAttackers[threatId], to);
            nextVictim = p2.Get(stmAttackers[threatId]);

            ++threatId;

            UtilMove::DetectStandardCapture(mm, p2);

#ifndef SEE_WITHOUT_CHECK_VALIDATION
            if ( ! Definitions::do_trustedGenerator && ! UtilMove::ValidateCheckStatus(mm,p2) ){
              //LOG(logINFO) << "move cannot be applied " << p2.GetFEN() << " move was " << mm.Show();
              continue
            }
            if ( ! Definitions::do_trustedGenerator) UtilMove::ValidateIsCheck(mm,p2);
#endif

            p2.ApplyMove(mm,false);
            //LOG(logINFO) << "move applied " << p2.GetFEN() << " move was " << mm.Show();

            validThreatFound = true;

            // Negamax the balance with alpha = balance, beta = balance+1 and
            // add nextVictim's value.
            //      (balance, balance+1) -> (-balance-1, -balance)
            balance = -balance - 1 - Piece::ValueAbs(nextVictim);

            //LOG(logINFO) << "balance is " << balance;

            // If balance is still non-negative after giving away nextVictim then we
            // win. The only thing to be careful about it is that we should revert
            // stm if we captured with the king when the opponent still has attackers.
            if (balance >= 0) {
                if (Piece::IsK(nextVictim) /*&& (attackers & pieces(stm))*/) {
                    p2.SwitchPlayer();
                }
                endOfSEE = true;
            }
        }

        if (!validThreatFound) {
            //LOG(logINFO) << "no valid threat found " << Square(to).position() << " fen " << p2.GetFEN();
            endOfSEE = true;
            break;
        }
    }
    return us != p2.Turn(); // We break the above loop when stm loses

}
