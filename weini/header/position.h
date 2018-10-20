#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <functional>
#include <random>
#include <algorithm>

#include "player.h"
#include "Piece.h"
#include "square.h"
#include "move.h"
#include "util.h"
#include "definitions.h"
#include "stats.h"
#include "UtilMove.h"

#include <cstring>

#include "logger.hpp"

#include "bitboard.h"

class Game;

#pragma pack(push, 2)
class Position {

#ifdef WITH_MEMORY_POOL
    friend class MemoryPool<Position, _default_block_size>;
#endif

public:

    static std::string startPosition;

    Position(const std::string & fen, bool withMoveCount = true);
    ~Position();

    Position(const Position & p);
    Position(const Position && p);

    void operator=(const Position & p);

    void Display()const;

    void ApplyMove(const Move & m, Game * game = 0, bool verifyKingCapture = true);

    //#define DEBUG_POSITION_GET

    const Piece::eType & Get(Square::LightSquare k) const;
    const Piece::eType & Get(Square::RankFile file, Square::RankFile rank) const;
    const Piece::eType & Get(const Square & sq) const;

    // Player
    inline Player &       WhitePlayer()      { return _white; }
    inline Player &       BlackPlayer()      { return _black; }
    inline const Player & WhitePlayer()const { return _white; }
    inline const Player & BlackPlayer()const { return _black; }

    // Piece stuff
    bool IsPawn        (Square::LightSquare k)const;
    bool IsWhitePawn   (Square::LightSquare k)const;
    bool IsBlackPawn   (Square::LightSquare k)const;
    bool IsWhiteKnight (Square::LightSquare k)const;
    bool IsBlackKnight (Square::LightSquare k)const;
    bool IsWhiteBishop (Square::LightSquare k)const;
    bool IsBlackBishop (Square::LightSquare k)const;
    bool IsWhiteBQ     (Square::LightSquare k)const;
    bool IsBlackBQ     (Square::LightSquare k)const;
    bool IsWhiteRook   (Square::LightSquare k)const;
    bool IsBlackRook   (Square::LightSquare k)const;
    bool IsWhiteRQ     (Square::LightSquare k)const;
    bool IsBlackRQ     (Square::LightSquare k)const;
    bool IsWhiteQueen  (Square::LightSquare k)const;
    bool IsBlackQueen  (Square::LightSquare k)const;
    bool IsWhiteKing   (Square::LightSquare k)const;
    bool IsBlackKing   (Square::LightSquare k)const;
    bool IsEmpty       (Square::LightSquare k)const;
    bool IsNotEmpty    (Square::LightSquare k)const;
    bool IsOfType      (Piece::eType t, Square::LightSquare k)const;

    bool PawnOn7thRank(NSColor::eColor c)const;

    // this one MUST be inlined
    int NumberOf(Piece::eType t)const;

    // this one MUST be inlined
    bool IsTherePieceOnBoard(Piece::eType t)const;

    void SetPiece(Square::LightSquare k, Piece::eType t);
    void SetPiece(Square::RankFile file, Square::RankFile rank, Piece::eType t);
    void SetPiece(const Square & sq, Piece::eType t);

    // Rule of chess stuff
    inline int                        HalfMoves50Rule()   const { return _halfMovesSinceLastPawnMoveOrCapture; }
    inline int                        CurrentMoveCount()  const { return _currentMovesCount; }
    inline int                        CurrentPly()        const { return _currentply; }
    inline Square::LightSquare&       EnPassant()               { return _enPassant; }
    inline const Square::LightSquare& EnPassant()         const { return _enPassant; }
    inline bool                       IsInCheck()         const { return _isInCheck; }

    // King square stuff
    const Square::LightSquare   WhiteKingSquare    ()const;
    void                        SetWhiteKing       (Square::LightSquare k);
    bool                        NoWhiteKingAnymore ()const;
    const Square::LightSquare   BlackKingSquare    ()const;
    void                        SetBlackKing       (Square::LightSquare k);
    bool                        NoBlackKingAnymore ()const;
    Square::LightSquare         InvalidateWhiteKing();
    Square::LightSquare         InvalidateBlackKing();

    // Color stuff
    static NSColor::eColor   SwitchColor  (const NSColor::eColor    & color);
    void                     SwitchPlayer ();
    void                     SwitchPlayerNullMove ();
    const NSColor::eColor    Turn         () const;
    const NSColor::eColor    Opponent     () const;
    inline const bool        WhiteToPlay  ()const { return Turn() == NSColor::c_white; }
    const NSColor::eColor    Color        (Square::LightSquare k)const;

    // BitBoards stuff
    BitBoards &         BitBoard();
    const BitBoards &   BitBoard()const;

    // FEN stuff
    std::string GetFEN      ()const;
    std::string GetFENShort ()const;
    std::string GetFENShort2()const;

    // Hash stuff
    Util::Zobrist::HashType GetZHash    ()const;
    Util::Zobrist::HashType GetZHashPawn()const;
    void InvalidateHash();
    void Invalidate    ();

    // Game phase stuff
    static bool IsEndGame       (const Position & p);
    bool        IsEndGame       ()const;
    static bool IsVeryEndGame   (const Position & p);
    bool        IsVeryEndGame   ()const;
    int         GamePhase       ()const;
    int         GamePhasePercent()const;
    static const int maxGamePhase;
    static const int endGameGamePhasePercent;

private:

    Position();

    void _ClearBoard();
    void _BuildBoard(const std::string & s);

    void _RemoveCastlingRight(Player & player, const Player::eCastlingRight & cr);

    void _XORHash(Util::Zobrist::HashType h);
    void _XORHashPawn(Util::Zobrist::HashType h);

    // should use get if not using the reference for modification !!
    Piece::eType &      _RGet(Square::LightSquare k, bool invalidateAlsoHash = true);
    Piece::eType &      _RGet(Square::RankFile file, Square::RankFile rank, bool invalidateAlsoHash = true);
    Piece::eType &      _RGet(const Square & sq);

    // this is slow to ask biboard for that !
    const Piece::eType  _GetB(Square::LightSquare k) const;
    const Piece::eType  _GetB(Square::RankFile file, Square::RankFile rank) const;
    const Piece::eType  _GetB(const Square & sq) const;

    // called by SetPiece to update bitboards
    void                _SetPieceB(Square::LightSquare k, Piece::eType t, Piece::eType t_helper);
    void                _SetPieceB(Square::RankFile file, Square::RankFile rank, Piece::eType t, Piece::eType t_helper);
    void                _SetPieceB(const Square & sq, Piece::eType t, Piece::eType t_helper);

    //----------
    // vars of a Position
    //----------

    NSColor::eColor                 _c;

    Piece::eType                    _board[BOARD_SIZE];
    BitBoards                       _bitBoard;

    mutable Util::Zobrist::HashType _zhash;
    mutable Util::Zobrist::HashType _zhashpawn;

    mutable unsigned char           _gamePhase;

    Square::LightSquare             _enPassant;
    unsigned char                   _halfMovesSinceLastPawnMoveOrCapture;
    unsigned char                   _currentMovesCount;
    PlyType                         _currentply;

    White                           _white;
    Black                           _black;

    bool                            _isInCheck;

};
#pragma pack(pop)

#include "position.hpp"
