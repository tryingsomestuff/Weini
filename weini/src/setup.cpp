#include "analyse.h"
#include "book.h"
#include "stats.h"
#include "search.h"
#include "timeman.h"
#include "ttable.h"
#include "thread.h"
#include "util.h"

bool Setup(){
    srand( (int) time( NULL ) );

    Util::Zobrist::Init();

    Definitions::InitKeys();

    Stats::Init();

    Square::InitOffBoard();

    if (!Definitions::ReadConfig("config.json")) {
        LOG(logWARNING) << "Cannot load local configuration, using the default one";
    }

    Analyse::InitConstants();

    LOG(logINFO) << "Move::HashType    " << sizeof(Move::HashType);
    LOG(logINFO) << "Zobrist::HashType " << sizeof(Util::Zobrist::HashType);
    LOG(logINFO) << "Type              " << sizeof(Piece::eType);
    LOG(logINFO) << "BitBoard          " << sizeof(BitBoard);
    LOG(logINFO) << "BitBoards         " << sizeof(BitBoards);
    LOG(logINFO) << "AdditionalInfo    " << sizeof(Position::AdditionalInfo);
    LOG(logINFO) << "Piece             " << sizeof(Piece);
    LOG(logINFO) << "Player            " << sizeof(Player);
    LOG(logINFO) << "Move              " << sizeof(Move);
    LOG(logINFO) << "Position          " << sizeof(Position) << " (includes BitBoards and AdditionalInfo)";
    LOG(logINFO) << "Square            " << sizeof(Square);
    LOG(logINFO) << "Transposition     " << sizeof(Transposition);
    LOG(logINFO) << "Bucket            " << sizeof(Bucket);

    UtilMove::InitMvvLva();

    Book::ReadBook(true);

    TimeMan::Instance().Init();

    if (Definitions::ttConfig.do_transpositionTableAlphaBeta
        || Definitions::ttConfig.do_transpositionTableSearch
        || Definitions::ttConfig.do_transpositionTableSortSearch
        || Definitions::ttConfig.do_transpositionTableSortAlphaBeta) {
        Transposition::InitTT();
    }
    if (Definitions::ttConfig.do_transpositionTableQuiesce
        || Definitions::ttConfig.do_transpositionTableQSort) {
        Transposition::InitTTQ();
    }

    if (Definitions::ttConfig.do_transpositionTableEval) {
        Analyse::InitTTE();
    }

    if (Definitions::ttConfig.do_transpositionTableEval) { ///@todo do not depend on do_lazy
        Analyse::InitTTL();
    }

    if (Definitions::ttConfig.do_transpositionTableEvalPawn) {
        Analyse::InitTTP();
    }

    ThreadPool<Searcher>::Instance().Setup((int)Definitions::smpConfig.threads);

    return true;
}

