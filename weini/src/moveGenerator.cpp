#include "moveGenerator.h"
#include "position.h"
#include "stats.h"

#include <map>

///@todo special movegenerator for check escaping ??

MoveGenerator::GeneratorStats MoveGenerator::stats;

#define ValidatePieceInTheWayOnSquare(p,to) (!BitScan::IsSet(colorPiece,to))

void MoveGenerator::GeneratorHelperCacheSquareNoStat(Position &            p,
                                                     FastContainer<Move>&  moves,
                                                     Square::LightSquare   k) {

    const Piece::eType t = p.Get(k);

    const BitBoard & colorPiece = p.WhiteToPlay() ? p.BitBoard().GetWhitePieceBitBoard()
                                                  : p.BitBoard().GetBlackPieceBitBoard();

    switch (t) {
    case Piece::t_Bpawn:
    {
        int n = (!_onlyCap && RANK(k)==6)?4:3; // double push only if starting square is 7th rank and not if only capture
        for (int d = 0; d < n; ++d) {
            if (IsOffTheBoard(k, UtilMove::legalBPawn[d].IncrSpe())) continue;
            const Square::LightSquare to = k + UtilMove::legalBPawn[d].Incr();
            // promotion is not for double push
            if (d!=3 && RANK(to) == 0 /*&& UtilMove::legalBPawn[d].RankInc() == -1*/) { // promotion
                static UtilMove::eSpecialMove tovalidateType[4] = { UtilMove::sm_pq,UtilMove::sm_pr,UtilMove::sm_pb,UtilMove::sm_pn };

                // promotions cannot be ep, so we can check for capture early!
                if (_onlyCap && p.IsEmpty(to)) continue;

#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                bool isValidPush = false;
                bool checkStatusOK = false;
                for (int i = 0; i < 4; ++i) {
                    m.Set(tovalidateType[i]);
                    if (isValidPush || UtilMove::ValidatePawn(m, p)){ // only once
                        isValidPush = true;
                        if (!_trusted || checkStatusOK || UtilMove::ValidateCheckStatus(m, p)) { // only once
                           checkStatusOK = true;
                        }
                    }
                    if ( checkStatusOK ){
                        if (_trusted) {
                           UtilMove::ValidateIsCheck(m, p); // this must be done each time
                        }
                        moves.push_back(m);
                    }
                }
            }
            else {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                if (UtilMove::ValidatePawn(m, p)
                    && (!_trusted || UtilMove::ValidateCheckStatus(m, p))) {
                    if (_onlyCap && !m.IsCapture()) continue;
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(m, p);
                    }
                    moves.push_back(std::move(m));
                }
            }
        }
        break;
    }
    case Piece::t_Wpawn:
    {
        int n = (!_onlyCap && RANK(k)==1)?4:3; // double push only if starting square is 2nd rank and not if only capture
        for (int d = 0; d < n; ++d) {
            if (IsOffTheBoard(k, UtilMove::legalWPawn[d].IncrSpe())) continue;
            const Square::LightSquare to = k + UtilMove::legalWPawn[d].Incr();
            // promotion is not for double push
            if (d!=3 && RANK(to) == 7 /*&& UtilMove::legalWPawn[d].RankInc() == 1*/) { // promotion
                static UtilMove::eSpecialMove tovalidateType[4] = { UtilMove::sm_pq,UtilMove::sm_pr,UtilMove::sm_pb,UtilMove::sm_pn };

                // promotions cannot be ep, so we can check for capture early!
                if (_onlyCap && p.IsEmpty(to)) continue;

#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                bool isValidPush = false;
                bool checkStatusOK = false;
                for (int i = 0; i < 4; ++i) {
                    m.Set(tovalidateType[i]);
                    if (isValidPush || UtilMove::ValidatePawn(m, p)){ // only once
                        isValidPush = true;
                        if (!_trusted || checkStatusOK || UtilMove::ValidateCheckStatus(m, p)) { // only once
                           checkStatusOK = true;
                        }
                    }
                    if ( checkStatusOK ){
                        if (_trusted) {
                           UtilMove::ValidateIsCheck(m, p); // this must be done each time
                        }
                        moves.push_back(m);
                    }
                }
            }
            else {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                if (UtilMove::ValidatePawn(m, p)
                    && (!_trusted || UtilMove::ValidateCheckStatus(m, p))) {
                    if (_onlyCap && !m.IsCapture()) continue;
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(m, p);
                    }
                    moves.push_back(std::move(m));
                }
            }
        }
        break;
    }
    case Piece::t_Brook:
    case Piece::t_Wrook:
    {
        for (int d = 0; d < 4; ++d) {
            Square::LightSquare to(k);
            const int kk = UtilMove::legalRook[d].Incr();
            const Square::RankFile IncrSpe = UtilMove::legalRook[d].IncrSpe();
            for (int depth = 1; /*depth <= 7*/; ++depth) {
                if (IsOffTheBoard(to, IncrSpe)) break;
                to += kk;
                bool isCap = p.IsNotEmpty(to);
                if (_onlyCap && !isCap) continue; // early
                if (!ValidatePieceInTheWayOnSquare(p, to)) break;
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                UtilMove::SetIsCapture(m,isCap);
                if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                    moves.push_back(std::move(m));
                }
                if (isCap) {
                    break;
                }
            }
        }
        break;
    }
    case Piece::t_Bknight:
    case Piece::t_Wknight:
    {
        for (int d = 0; d < 8; ++d) {
            const int kk = UtilMove::legalKnight[d].Incr();
            if (IsOffTheBoard(k, UtilMove::legalKnight[d].IncrSpe())) continue;
            Square::LightSquare to(k + kk);
            bool isCap = p.IsNotEmpty(to);
            if (_onlyCap && !isCap) continue; // early
            if (!ValidatePieceInTheWayOnSquare(p, to)) continue;
#ifdef WITH_MEMORY_POOL_BUILDER
            ScopePoolObject<Move, _default_block_size> msp(k, to);
            Move & m = msp.Get();
#else
            Move m(k, to);
#endif
            UtilMove::SetIsCapture(m,isCap);
            if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                moves.push_back(std::move(m));
            }
        }
        break;
    }
    case Piece::t_Bbishop:
    case Piece::t_Wbishop:
    {
        for (int d = 0; d < 4; ++d) {
            Square::LightSquare to(k);
            const int kk = UtilMove::legalBishop[d].Incr();
            const Square::RankFile IncrSpe = UtilMove::legalBishop[d].IncrSpe();
            for (int depth = 1; /*depth <= 7*/; ++depth) {
                if (IsOffTheBoard(to,IncrSpe)) break;
                to += kk;
                bool isCap = p.IsNotEmpty(to);
                if (_onlyCap && !isCap) continue; // early
                if (!ValidatePieceInTheWayOnSquare(p, to)) break;
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                UtilMove::SetIsCapture(m,isCap);
                if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                    moves.push_back(std::move(m));
                }
                if (isCap) {
                    break;
                }
            }
        }
        break;
    }
    case Piece::t_Bqueen:
    case Piece::t_Wqueen:
    {
        for (int d = 0; d < 8; ++d) {
            Square::LightSquare to(k);
            const int kk = UtilMove::legalQueen[d].Incr();
            const Square::RankFile IncrSpe = UtilMove::legalQueen[d].IncrSpe();
            for (int depth = 1; /*depth <= 7*/; ++depth) {
                if (IsOffTheBoard(to,IncrSpe)) break;
                to += kk;
                bool isCap = p.IsNotEmpty(to);
                if (_onlyCap && !isCap) continue; // early
                if (!ValidatePieceInTheWayOnSquare(p, to)) break;
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                UtilMove::SetIsCapture(m,isCap);
                if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                    moves.push_back(std::move(m));
                }
                if (isCap) {
                    break;
                }
            }
        }
        break;
    }
    case Piece::t_Bking:
    case Piece::t_Wking:
    case Piece::t_BkingFalse:
    case Piece::t_WkingFalse:
    {
        for (int d = 0; d < 8; ++d) {
            const int kk = UtilMove::legalKing[d].Incr();
            if (IsOffTheBoard(k, UtilMove::legalKing[d].IncrSpe())) continue;
            Square::LightSquare to(k + kk);
            bool isCap = p.IsNotEmpty(to);
            if (_onlyCap && !isCap) continue; // early
            if (!ValidatePieceInTheWayOnSquare(p, to)) continue;
#ifdef WITH_MEMORY_POOL_BUILDER
            ScopePoolObject<Move, _default_block_size> msp(k, to);
            Move & m = msp.Get();
#else
            Move m(k, to);
#endif
            UtilMove::SetIsCapture(m,isCap);
            if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                moves.push_back(std::move(m));
            }
        }
        if (!_onlyCap && t != Piece::t_BkingFalse && t != Piece::t_WkingFalse) {
            {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(p.WhiteToPlay() ? UtilMove::sm_Ks : UtilMove::sm_ks);
                Move & cks = msp.Get();
#else
                Move cks(p.WhiteToPlay() ? UtilMove::sm_Ks : UtilMove::sm_ks);
#endif
                if (UtilMove::ValidateCastling(cks, p)) {
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(cks, p);
                    }
                    moves.push_back(std::move(cks));
                }
            }
            {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(p.WhiteToPlay() ? UtilMove::sm_Qs : UtilMove::sm_qs);
                Move & cqs = msp.Get();
#else
                Move cqs(p.WhiteToPlay() ? UtilMove::sm_Qs : UtilMove::sm_qs);
#endif
                if (UtilMove::ValidateCastling(cqs, p)) {
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(cqs, p);
                    }
                    moves.push_back(std::move(cqs));
                }
            }
        }
        break;
    }
    }

}

void MoveGenerator::GeneratorHelperCacheSquare(Position &             p,
                                               FastContainer<Move>&   moves,
                                               Square::LightSquare    k,
                                               Stats::NodeCount *     nodes) {

    stats.Reset();


    const Piece::eType t = p.Get(k);

    const BitBoard & colorPiece = p.WhiteToPlay() ? p.BitBoard().GetWhitePieceBitBoard()
                                                  : p.BitBoard().GetBlackPieceBitBoard();

    switch (t) {
    case Piece::t_Bpawn:
    {
        int n = (!_onlyCap && RANK(k)==6)?4:3; // double push only if starting square is 7th rank and not if only capture
        for (int d = 0; d < n; ++d) {
            if (IsOffTheBoard(k, UtilMove::legalBPawn[d].IncrSpe())) continue;
            const Square::LightSquare to = k + UtilMove::legalBPawn[d].Incr();
            // promotion is not for double push
            if (d!=3 && RANK(to) == 0 /*&& UtilMove::legalBPawn[d].RankInc() == -1*/) { // promotion
                static UtilMove::eSpecialMove tovalidateType[4] = { UtilMove::sm_pq,UtilMove::sm_pr,UtilMove::sm_pb,UtilMove::sm_pn };

                // promotions cannot be ep, so we can check for capture early!
                if (_onlyCap && p.IsEmpty(to)) continue;

#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                bool isValidPush = false;
                bool checkStatusOK = false;
                for (int i = 0; i < 4; ++i) {
                    ++stats.createdNodes;
                    m.Set(tovalidateType[i]);
                    if (isValidPush || UtilMove::ValidatePawn(m, p)){ // only once
                        ++stats.pseudoNodes;
                        isValidPush = true;
                        if (!_trusted || checkStatusOK || UtilMove::ValidateCheckStatus(m, p)) { // only once
                           checkStatusOK = true;
                        }
                    }
                    if ( checkStatusOK ){
                        if (_trusted) {
                           UtilMove::ValidateIsCheck(m, p); // this must be done each time
                        }
                        ++stats.validNodes;
                        if ( m.IsCapture() ) ++stats.captureNodes;
                        ++stats.promotionNodes;
                        if ( m.IsCheck() ) ++stats.checkNodes;
                        moves.push_back(m);
                    }
                }
            }
            else {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                ++stats.createdNodes;
                if (UtilMove::ValidatePawn(m, p)
                    && (!_trusted || UtilMove::ValidateCheckStatus(m, p))) {
                    ++stats.pseudoNodes;
                    if (_onlyCap && !m.IsCapture()) continue;
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(m, p);
                    }
                    if ( m.Type() == UtilMove::sm_ep ) ++stats.epNodes;
                    ++stats.validNodes;
                    if ( m.IsCapture() ) ++stats.captureNodes;
                    if ( m.IsCheck() ) ++stats.checkNodes;
                    moves.push_back(std::move(m));
                }
            }
        }
        break;
    }
    case Piece::t_Wpawn:
    {
        int n = (!_onlyCap && RANK(k)==1)?4:3; // double push only if starting square is 2nd rank and not if only capture
        for (int d = 0; d < n; ++d) {
            if (IsOffTheBoard(k, UtilMove::legalWPawn[d].IncrSpe())) continue;
            const Square::LightSquare to = k + UtilMove::legalWPawn[d].Incr();
            // promotion is not for double push
            if (d!=3 && RANK(to) == 7 /*&& UtilMove::legalWPawn[d].RankInc() == 1*/) { // promotion
                static UtilMove::eSpecialMove tovalidateType[4] = { UtilMove::sm_pq,UtilMove::sm_pr,UtilMove::sm_pb,UtilMove::sm_pn };

                // promotions cannot be ep, so we can check for capture early!
                if (_onlyCap && p.IsEmpty(to)) continue;

#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                bool isValidPush = false;
                bool checkStatusOK = false;
                for (int i = 0; i < 4; ++i) {
                    ++stats.createdNodes;
                    m.Set(tovalidateType[i]);
                    if (isValidPush || UtilMove::ValidatePawn(m, p)){ // only once
                        ++stats.pseudoNodes;
                        isValidPush = true;
                        if (!_trusted || checkStatusOK || UtilMove::ValidateCheckStatus(m, p)) { // only once
                           checkStatusOK = true;
                        }
                    }
                    if ( checkStatusOK ){
                        if (_trusted) {
                           UtilMove::ValidateIsCheck(m, p); // this must be done each time
                        }
                        ++stats.validNodes;
                        if ( m.IsCapture() ) ++stats.captureNodes;
                        ++stats.promotionNodes;
                        if ( m.IsCheck() ) ++stats.checkNodes;
                        moves.push_back(m);
                    }
                }
            }
            else {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                ++stats.createdNodes;
                if (UtilMove::ValidatePawn(m, p)
                    && (!_trusted || UtilMove::ValidateCheckStatus(m, p))) {
                    ++stats.pseudoNodes;
                    if (_onlyCap && !m.IsCapture()) continue;
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(m, p);
                    }
                    if ( m.Type() == UtilMove::sm_ep ) ++stats.epNodes;
                    ++stats.validNodes;
                    if ( m.IsCapture() ) ++stats.captureNodes;
                    if ( m.IsCheck() ) ++stats.checkNodes;
                    moves.push_back(std::move(m));
                }
            }
        }
        break;
    }
    case Piece::t_Brook:
    case Piece::t_Wrook:
    {
        for (int d = 0; d < 4; ++d) {
            Square::LightSquare to(k);
            const int kk = UtilMove::legalRook[d].Incr();
            const Square::RankFile IncrSpe = UtilMove::legalRook[d].IncrSpe();
            for (int depth = 1; /*depth <= 7*/; ++depth) {
                ++stats.createdNodes;
                if (IsOffTheBoard(to, IncrSpe)) break;
                to += kk;
                bool isCap = p.IsNotEmpty(to);
                if (_onlyCap && !isCap) continue; // early
                if (!ValidatePieceInTheWayOnSquare(p, to)) break;
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                UtilMove::SetIsCapture(m,isCap);
                ++stats.pseudoNodes;
                if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                    ++stats.validNodes;
                    if ( m.IsCapture() ) ++stats.captureNodes;
                    if ( m.IsCheck() ) ++stats.checkNodes;
                    moves.push_back(std::move(m));
                }
                if (isCap) {
                    break;
                }
            }
        }
        break;
    }
    case Piece::t_Bknight:
    case Piece::t_Wknight:
    {
        for (int d = 0; d < 8; ++d) {
            ++stats.createdNodes;
            const int kk = UtilMove::legalKnight[d].Incr();
            if (IsOffTheBoard(k, UtilMove::legalKnight[d].IncrSpe())) continue;
            Square::LightSquare to(k + kk);
            bool isCap = p.IsNotEmpty(to);
            if (_onlyCap && !isCap) continue; // early
            if (!ValidatePieceInTheWayOnSquare(p, to)) continue;
#ifdef WITH_MEMORY_POOL_BUILDER
            ScopePoolObject<Move, _default_block_size> msp(k, to);
            Move & m = msp.Get();
#else
            Move m(k, to);
#endif
            UtilMove::SetIsCapture(m,isCap);
            ++stats.pseudoNodes;
            if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                ++stats.validNodes;
                if ( m.IsCapture() ) ++stats.captureNodes;
                if ( m.IsCheck() ) ++stats.checkNodes;
                moves.push_back(std::move(m));
            }
        }
        break;
    }
    case Piece::t_Bbishop:
    case Piece::t_Wbishop:
    {
        for (int d = 0; d < 4; ++d) {
            Square::LightSquare to(k);
            const int kk = UtilMove::legalBishop[d].Incr();
            const Square::RankFile IncrSpe = UtilMove::legalBishop[d].IncrSpe();
            for (int depth = 1; /*depth <= 7*/; ++depth) {
                ++stats.createdNodes;
                if (IsOffTheBoard(to,IncrSpe)) break;
                to += kk;
                bool isCap = p.IsNotEmpty(to);
                if (_onlyCap && !isCap) continue; // early
                if (!ValidatePieceInTheWayOnSquare(p, to)) break;
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                UtilMove::SetIsCapture(m,isCap);
                ++stats.pseudoNodes;
                if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                    ++stats.validNodes;
                    if ( m.IsCapture() ) ++stats.captureNodes;
                    if ( m.IsCheck() ) ++stats.checkNodes;
                    moves.push_back(std::move(m));
                }
                if (isCap) {
                    break;
                }
            }
        }
        break;
    }
    case Piece::t_Bqueen:
    case Piece::t_Wqueen:
    {
        for (int d = 0; d < 8; ++d) {
            Square::LightSquare to(k);
            const int kk = UtilMove::legalQueen[d].Incr();
            const Square::RankFile IncrSpe = UtilMove::legalQueen[d].IncrSpe();
            for (int depth = 1; /*depth <= 7*/; ++depth) {
                ++stats.createdNodes;
                if (IsOffTheBoard(to,IncrSpe)) break;
                to += kk;
                bool isCap = p.IsNotEmpty(to);
                if (_onlyCap && !isCap) continue; // early
                if (!ValidatePieceInTheWayOnSquare(p, to)) break;
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(k, to);
                Move & m = msp.Get();
#else
                Move m(k, to);
#endif
                UtilMove::SetIsCapture(m,isCap);
                ++stats.pseudoNodes;
                if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                    ++stats.validNodes;
                    if ( m.IsCapture() ) ++stats.captureNodes;
                    if ( m.IsCheck() ) ++stats.checkNodes;
                    moves.push_back(std::move(m));
                }
                if (isCap) {
                    break;
                }
            }
        }
        break;
    }
    case Piece::t_Bking:
    case Piece::t_Wking:
    case Piece::t_BkingFalse:
    case Piece::t_WkingFalse:
    {
        for (int d = 0; d < 8; ++d) {
            ++stats.createdNodes;
            const int kk = UtilMove::legalKing[d].Incr();
            if (IsOffTheBoard(k, UtilMove::legalKing[d].IncrSpe())) continue;
            Square::LightSquare to(k + kk);
            bool isCap = p.IsNotEmpty(to);
            if (_onlyCap && !isCap) continue; // early
            if (!ValidatePieceInTheWayOnSquare(p, to)) continue;
#ifdef WITH_MEMORY_POOL_BUILDER
            ScopePoolObject<Move, _default_block_size> msp(k, to);
            Move & m = msp.Get();
#else
            Move m(k, to);
#endif
            UtilMove::SetIsCapture(m,isCap);
            ++stats.pseudoNodes;
            if (!_trusted || (UtilMove::ValidateCheckStatus(m, p) && UtilMove::ValidateIsCheck(m, p))) {
                ++stats.validNodes;
                if ( m.IsCapture() ) ++stats.captureNodes;
                if ( m.IsCheck() ) ++stats.checkNodes;
                moves.push_back(std::move(m));
            }
        }
        if (!_onlyCap && t != Piece::t_BkingFalse && t != Piece::t_WkingFalse) {
            {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(p.WhiteToPlay() ? UtilMove::sm_Ks : UtilMove::sm_ks);
                Move & cks = msp.Get();
#else
                Move cks(p.WhiteToPlay() ? UtilMove::sm_Ks : UtilMove::sm_ks);
#endif
                ++stats.createdNodes;
                if (UtilMove::ValidateCastling(cks, p)) {
                    ++stats.pseudoNodes;
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(cks, p);
                    }
                    ++stats.validNodes;
                    if ( (cks).IsCheck() ) ++stats.checkNodes;
                    moves.push_back(std::move(cks));
                }
            }
            {
#ifdef WITH_MEMORY_POOL_BUILDER
                ScopePoolObject<Move, _default_block_size> msp(p.WhiteToPlay() ? UtilMove::sm_Qs : UtilMove::sm_qs);
                Move & cqs = msp.Get();
#else
                Move cqs(p.WhiteToPlay() ? UtilMove::sm_Qs : UtilMove::sm_qs);
#endif
                ++stats.createdNodes;
                if (UtilMove::ValidateCastling(cqs, p)) {
                    ++stats.pseudoNodes;
                    if (_trusted) {
                        UtilMove::ValidateIsCheck(cqs, p);
                    }
                    ++stats.validNodes;
                    if ( cqs.IsCheck() ) ++stats.checkNodes;
                    moves.push_back(std::move(cqs));
                }
            }
        }
        break;
    }
    }

    if ( nodes ){
      nodes->createdNodes   += stats.createdNodes;
      nodes->pseudoNodes    += stats.pseudoNodes;
      nodes->validNodes     += stats.validNodes;
      nodes->epNodes        += stats.epNodes;
      nodes->captureNodes   += stats.captureNodes;
      nodes->promotionNodes += stats.promotionNodes;
      nodes->checkNodes     += stats.checkNodes;
      nodes->checkMateNodes += stats.checkMateNodes;
    }

    Stats::currentEveryNodesCount  += stats.createdNodes;
    Stats::currentPseudoNodesCount += stats.pseudoNodes;

    stats.createdNodesAllTime += stats.createdNodes;
    stats.validNodesAllTime   += stats.validNodes;

}

void MoveGenerator::GeneratorHelperCacheNoStat( Position &             p,
                                                FastContainer<Move> &  moves){

    if ( p.WhiteToPlay() ){
        for(Square::LightSquare k = 0 ; k < BOARD_SIZE ; ++k){
            if ( BitScan::IsSet(p.BitBoard().GetWhitePieceBitBoard(),k) ){
                GeneratorSquareNoStat(p,moves,k);
            }
        }
    }
    else{
        for(Square::LightSquare k = 0 ; k < BOARD_SIZE ; ++k){
            if ( BitScan::IsSet(p.BitBoard().GetBlackPieceBitBoard(),k) ) {
                GeneratorSquareNoStat(p,moves,k);
            }
        }
    }
}

void MoveGenerator::GeneratorHelperCache( Position &             p,
                                          FastContainer<Move> &  moves,
                                          Stats::NodeCount *     nodes){

    if ( p.WhiteToPlay() ){
        for(Square::LightSquare k = 0 ; k < BOARD_SIZE ; ++k){
            if ( BitScan::IsSet(p.BitBoard().GetWhitePieceBitBoard(),k) ){
                GeneratorSquare(p,moves,k,nodes);
            }
        }
    }
    else{
        for(Square::LightSquare k = 0 ; k < BOARD_SIZE ; ++k){
            if ( BitScan::IsSet(p.BitBoard().GetBlackPieceBitBoard(),k) ) {
                GeneratorSquare(p,moves,k,nodes);
            }
        }
    }
}

void MoveGenerator::GeneratorHelperCachePieceNoStat( Position &                                     p,
                                                     std::map<Piece::eType, FastContainer<Move> > & moves){

    const NSColor::eColor c = p.Turn();
    for(Square::LightSquare k = 0 ; k < BOARD_SIZE ; ++k){
        const Piece::eType t = p.Get(k);
        if ( Piece::Color(t) == c){
           FastContainer<Move> & l = moves[t];
           GeneratorSquareNoStat(p,l,k);
        }
    }
}

void MoveGenerator::GeneratorHelperCachePiece( Position &                                     p,
                                               std::map<Piece::eType, FastContainer<Move> > & moves,
                                               Stats::NodeCount *                             nodes ){

    const NSColor::eColor c = p.Turn();
    for(Square::LightSquare k = 0 ; k < BOARD_SIZE ; ++k){
        const Piece::eType t = p.Get(k);
        if ( Piece::Color(t) == c){
           FastContainer<Move> & l = moves[t];
           GeneratorSquare(p,l,k,nodes);
        }
    }
}

void MoveGenerator::GeneratorSquareNoStat(Position &             p,
                                          FastContainer<Move> &  moves,
                                          Square::LightSquare    k){
   // no clear of course !!!!
   GeneratorHelperCacheSquareNoStat(p,moves,k);
}

void MoveGenerator::GeneratorSquare(Position &             p,
                                    FastContainer<Move> &  moves,
                                    Square::LightSquare    k,
                                    Stats::NodeCount *     nodes){
   // no clear of course !!!!
   GeneratorHelperCacheSquare(p, moves, k,nodes);
}

void MoveGenerator::Generator(Position &            p,
                              FastContainer<Move> & moves,
                              Stats::NodeCount *    nodes){
    moves.clear();
    _withStats ? GeneratorHelperCache(p,moves,nodes)
               : GeneratorHelperCacheNoStat(p,moves);
}

void MoveGenerator::GeneratorPiece(Position &                                      p,
                                   std::map<Piece::eType, FastContainer<Move> > &  moves,
                                   Stats::NodeCount *                              nodes){
    moves.clear();
    _withStats ? GeneratorHelperCachePiece(p,moves,nodes)
               : GeneratorHelperCachePieceNoStat(p,moves);
}

