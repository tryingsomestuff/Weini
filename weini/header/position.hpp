
inline Position::Position(const Position & p) :
    _c(p._c),
    _bitBoard(p._bitBoard),
    _zhash(p._zhash),
    _zhashpawn(p._zhashpawn),
    _gamePhase(p._gamePhase),
    _enPassant(p._enPassant),
    _halfMovesSinceLastPawnMoveOrCapture(p._halfMovesSinceLastPawnMoveOrCapture),
    _currentMovesCount(p._currentMovesCount),
    _currentply(p._currentply),
    _white(p._white),
    _black(p._black),
    _isInCheck(p._isInCheck)
{
    ++Stats::positionCopy;
    std::memcpy(_board, p._board, BOARD_SIZE*sizeof(Piece::eType));
}

inline Position::Position(const Position && p) :
    _c(p._c),
    _bitBoard(p._bitBoard),
    _zhash(p._zhash),
    _zhashpawn(p._zhashpawn),
    _gamePhase(p._gamePhase),
    _enPassant(p._enPassant),
    _halfMovesSinceLastPawnMoveOrCapture(p._halfMovesSinceLastPawnMoveOrCapture),
    _currentMovesCount(p._currentMovesCount),
    _currentply(p._currentply),
    _white(p._white),
    _black(p._black),
    _isInCheck(p._isInCheck)
{
    ++Stats::positionCopy;
    std::memcpy(_board, p._board, BOARD_SIZE*sizeof(Piece::eType));
}

inline
void Position::operator=(const Position & p) {

    ++Stats::positionCopy;

    _c                                   = p._c;
    std::memcpy(_board, p._board, BOARD_SIZE*sizeof(Piece::eType));
    _bitBoard                            = p._bitBoard;
    _zhash                               = p._zhash;
    _zhashpawn                           = p._zhashpawn;
    _gamePhase                           = p._gamePhase;
    _enPassant                           = p._enPassant;
    _halfMovesSinceLastPawnMoveOrCapture = p._halfMovesSinceLastPawnMoveOrCapture;
    _currentMovesCount                   = p._currentMovesCount;
    _currentply                          = p._currentply;
    _white                               = p._white;
    _black                               = p._black;
    _isInCheck                           = p._isInCheck;
}

inline
const Piece::eType & Position::Get(Square::LightSquare k) const {
#ifdef DEBUG_POSITION_GET
    assert(k >= 0);
    assert(k <= 63);
#endif
    return _board[(int)k];
}

inline
const Piece::eType & Position::Get(Square::RankFile file, Square::RankFile rank) const {
#ifdef DEBUG_POSITION_GET
    assert(file >= 0);
    assert(file <= 7);
    assert(rank >= 0);
    assert(rank <= 7);
#endif
    return _board[file + 8 * rank];
}

inline
const Piece::eType & Position::Get(const Square & sq) const {
    return Get(sq.index());
}

inline
bool Position::IsPawn(Square::LightSquare k)const {
    ::BitBoard b = BitBoard().whitePawn | BitBoard().blackPawn;
    return BitScan::IsSet(b, k);
}

inline
bool Position::IsWhitePawn(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whitePawn, k);
}

inline
bool Position::IsBlackPawn(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackPawn, k);
}

inline
bool Position::IsWhiteKnight(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whiteKnight, k);
}

inline
bool Position::IsBlackKnight(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackKnight, k);
}

inline
bool Position::IsWhiteBishop(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whiteBishop, k);
}

inline
bool Position::IsBlackBishop(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackBishop, k);
}

inline
bool Position::IsWhiteBQ(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whiteBishop | BitBoard().whiteQueen, k);
}

inline
bool Position::IsBlackBQ(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackBishop | BitBoard().blackQueen, k);
}

inline
bool Position::IsWhiteRook(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whiteRook, k);
}

inline
bool Position::IsBlackRook(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackRook, k);
}

inline
bool Position::IsWhiteRQ(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whiteRook | BitBoard().whiteQueen, k);
}

inline
bool Position::IsBlackRQ(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackRook | BitBoard().blackQueen, k);
}

inline
bool Position::IsWhiteQueen(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whiteQueen, k);
}

inline
bool Position::IsBlackQueen(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackQueen, k);
}

inline
bool Position::IsWhiteKing(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whiteKing, k);
}

inline
bool Position::IsBlackKing(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().blackKing, k);
}

inline
bool Position::IsEmpty(Square::LightSquare k)const {
    return !BitScan::IsSet(BitBoard().whitePiece | BitBoard().blackPiece, k);
}

inline
bool Position::IsNotEmpty(Square::LightSquare k)const {
    return BitScan::IsSet(BitBoard().whitePiece | BitBoard().blackPiece, k);
}

inline
bool Position::IsOfType(Piece::eType t, Square::LightSquare k)const {
    return BitBoard().IsOfType(t, k);
}

inline
bool Position::PawnOn7thRank(NSColor::eColor c)const {
    return WhiteToPlay() ? ((BitBoard().whitePawn & BitBoards::white7thRank) != 0)
                         : ((BitBoard().blackPawn & BitBoards::black7thRank) != 0);
}

// should use get if not using the reference for modification !!
inline
Piece::eType & Position::_RGet(Square::LightSquare k, bool invalidateAlsoHash) {
    if ( invalidateAlsoHash) Invalidate(); // you should use get if not using the reference for modification !!
#ifdef DEBUG_POSITION_GET
    assert(k >= 0);
    assert(k <= 63);
#endif
    return _board[(int)k];
}

// should use get if not using the reference for modification !!
inline
Piece::eType & Position::_RGet(Square::RankFile file, Square::RankFile rank, bool invalidateAlsoHash) {
    if ( invalidateAlsoHash) Invalidate(); // you should use get if not using the reference for modification !!
#ifdef DEBUG_POSITION_GET
    assert(file >= 0);
    assert(file <= 7);
    assert(rank >= 0);
    assert(rank <= 7);
#endif
    return _board[file + 8 * rank];
}

// should use get if not using the reference for modification !!
inline
Piece::eType & Position::_RGet(const Square & sq) {
    // you should use get if not using the reference for modification !!
    return _RGet(sq.index());
}

inline
void Position::SetPiece(Square::LightSquare k, Piece::eType t) {
    // update available
    const Piece::eType t1 = Get(k);  // former piece

    _SetPieceB(k, t, t1);

    const Util::Zobrist::HashType zt1 = Util::Zobrist::zTable[(int)k][TypeToInt(t1)];
    const Util::Zobrist::HashType zt  = Util::Zobrist::zTable[(int)k][TypeToInt(t)];

    // update hashes
    _XORHash(zt1); // remove former piece hash
    _XORHash(zt);  // add new piece hash
    if (Piece::IsPawn(t1)) {
        _XORHashPawn(zt1);
    }
    if (Piece::IsPawn(t)) {
        _XORHashPawn(zt);
    }

    _RGet(k, false) = t; // assume not to invalidate zhash because it has just been updated above
}

inline
void Position::SetPiece(Square::RankFile file, Square::RankFile rank, Piece::eType t) {
    SetPiece(file + 8 * rank, t);
}

inline
void Position::SetPiece(const Square & sq, Piece::eType t) {
    SetPiece(sq.index(), t);
}

inline
const Piece::eType  Position::_GetB(Square::LightSquare k) const {
    return _bitBoard.Get(k);
}

inline
const Piece::eType  Position::_GetB(Square::RankFile file, Square::RankFile rank) const {
    return _bitBoard.Get(file + 8 * rank);
}

inline
const Piece::eType  Position::_GetB(const Square & sq) const {
    return _bitBoard.Get(sq.index());
}

inline
void Position::_SetPieceB(Square::LightSquare k, Piece::eType t, Piece::eType t_helper) {

    if (BitScan::IsSet(_bitBoard.whitePiece, k)) {
        BitScan::UnsetBit(_bitBoard.whitePiece, k);
    }
    else if (BitScan::IsSet(_bitBoard.blackPiece, k)) {
        BitScan::UnsetBit(_bitBoard.blackPiece, k);
    }

    if (Piece::IsWhite(t)) {
        BitScan::SetBit(_bitBoard.whitePiece, k);
    }
    else if (Piece::IsBlack(t)) {
        BitScan::SetBit(_bitBoard.blackPiece, k);
    }

    _bitBoard.UnSetPiece(k, t_helper); // clear t_helper bitboard
    _bitBoard.SetPiece(k, t); // set t bitboard
}

inline
void Position::_SetPieceB(Square::RankFile file, Square::RankFile rank, Piece::eType t, Piece::eType t_helper) {
    _SetPieceB(file + 8 * rank, t, t_helper);
}

inline
void Position::_SetPieceB(const Square & sq, Piece::eType t, Piece::eType t_helper) {
    _SetPieceB(sq.index(), t, t_helper);
}

inline
const Square::LightSquare Position::WhiteKingSquare()const { return _bitBoard.WhiteKingSquare(); }

inline
void Position::SetWhiteKing(Square::LightSquare s) { BitScan::SetBit(_bitBoard.whiteKing, s); }

inline
bool Position::NoWhiteKingAnymore()const { return _bitBoard.whiteKing == 0; }

inline
const Square::LightSquare Position::BlackKingSquare()const { return _bitBoard.BlackKingSquare(); }

inline
void Position::SetBlackKing(Square::LightSquare s) { BitScan::SetBit(_bitBoard.blackKing, s); }

inline
bool Position::NoBlackKingAnymore()const { return _bitBoard.blackKing == 0; }

inline
Square::LightSquare Position::InvalidateWhiteKing() { Square::LightSquare s = WhiteKingSquare(); _bitBoard.InvalidateWhiteKing(); return s; }

inline
Square::LightSquare Position::InvalidateBlackKing() { Square::LightSquare s = BlackKingSquare(); _bitBoard.InvalidateBlackKing(); return s; }

inline void Position::InvalidateHash() {
    _zhash = 0;
}

inline void Position::Invalidate() {
    InvalidateHash();
}

inline const NSColor::eColor    Position::Turn() const { return _c; }
inline const NSColor::eColor    Position::Opponent()const { return SwitchColor(_c); }

inline const NSColor::eColor    Position::Color(Square::LightSquare k)const {
    if (BitScan::IsSet(BitBoard().whitePiece,k)) return NSColor::c_white;
    if (BitScan::IsSet(BitBoard().blackPiece,k)) return NSColor::c_black;
    return NSColor::c_none;
}

inline BitBoards &         Position::BitBoard()      { return _bitBoard; }
inline const BitBoards &   Position::BitBoard()const { return _bitBoard; }

// this one MUST be inlined
inline
int Position::NumberOf(Piece::eType t)const {
    return _bitBoard.NumberOf(t);
}

// this one MUST be inlined
inline
bool Position::IsTherePieceOnBoard(Piece::eType t)const {
    return _bitBoard.IsTherePieceOnBoard(t);
}

inline Position::Position() :
    _c(NSColor::c_none)
{
    // this is needed in order to init zhash below!
    _ClearBoard();
}

inline
void Position::_XORHash(Util::Zobrist::HashType h) {
    if (_zhash == 0) {
        //LOG(logWARNING) << "XORing uninitialized hash";
        GetZHash();
    }
    _zhash ^= h;
}

inline
void Position::_XORHashPawn(Util::Zobrist::HashType h) {
    if (_zhashpawn == 0) {
        //LOG(logWARNING) << "XORing uninitialized pawnhash";
        GetZHashPawn();
    }
    _zhashpawn ^= h;
}

