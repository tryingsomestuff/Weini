#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <functional>
#include <random>
#include <algorithm>

#include "game.h"
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

    struct AdditionalInfo {

        White white;
        Black black;

        Square::LightSquare enPassant;

        MiniMove lastMove;
        Piece::eType lastCapturedPiece;
        bool isInCheck;
        unsigned char halfMovesSinceLastPawnMoveOrCapture;
        unsigned char movesCount;
        unsigned short int currentply;
        unsigned char gamePhase;

        Util::Zobrist::HashType zhash;
        Util::Zobrist::HashType zhashpawn;

        AdditionalInfo();
        AdditionalInfo(const AdditionalInfo & i);
        AdditionalInfo(const AdditionalInfo && i);

        void operator=(const AdditionalInfo & i);

        inline bool lastMoveWasCapture()const{return lastCapturedPiece != Piece::t_empty;}
    };

    const AdditionalInfo & Info()const;

    AdditionalInfo & InfoRef();

    void operator=(const Position & p);

    void Display()const;

    void ApplyMove(const Move & m, bool verifyKingCapture = true);

    //#define DEBUG_POSITION_GET

    const Piece::eType & Get(Square::LightSquare k) const;
    const Piece::eType & Get(Square::RankFile file, Square::RankFile rank) const;
    const Piece::eType & Get(const Square & sq) const;

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

private:

    // should use get if not using the reference for modification !!
    Piece::eType & _RGet(Square::LightSquare k, bool invalidateAlsoHash = true);
    // should use get if not using the reference for modification !!
    Piece::eType & _RGet(Square::RankFile file, Square::RankFile rank, bool invalidateAlsoHash = true);
    // should use get if not using the reference for modification !!
    Piece::eType & _RGet(const Square & sq);

public:

    void SetPiece(Square::LightSquare k, Piece::eType t);
    void SetPiece(Square::RankFile file, Square::RankFile rank, Piece::eType t);
    void SetPiece(const Square & sq, Piece::eType t);

private:

    const Piece::eType  _GetB(Square::LightSquare k) const;
    const Piece::eType  _GetB(Square::RankFile file, Square::RankFile rank) const;
    const Piece::eType  _GetB(const Square & sq) const;
    void                _SetPieceB(Square::LightSquare k, Piece::eType t, Piece::eType t_helper);
    void                _SetPieceB(Square::RankFile file, Square::RankFile rank, Piece::eType t, Piece::eType t_helper);
    void                _SetPieceB(const Square & sq, Piece::eType t, Piece::eType t_helper);

public:

    const Square::LightSquare   WhiteKingSquare()const;
    void                        SetWhiteKing(Square::LightSquare k);
    bool                        NoWhiteKingAnymore()const;
    const Square::LightSquare   BlackKingSquare()const;
    void                        SetBlackKing(Square::LightSquare k);
    bool                        NoBlackKingAnymore()const;

    Square::LightSquare InvalidateWhiteKing();
    Square::LightSquare InvalidateBlackKing();

    void InvalidateFen ();
    void InvalidateHash();
    void Invalidate    ();

    static NSColor::eColor    SwitchColor   (const NSColor::eColor    & color);
    void SwitchPlayer();

    const NSColor::eColor    Turn    () const;
    const NSColor::eColor    Opponent() const;

    inline const bool WhiteToPlay()const { return Turn() == NSColor::c_white; }

    const NSColor::eColor    Color(Square::LightSquare k)const;

    BitBoards &         BitBoard();
    const BitBoards &   BitBoard()const;

    bool IsInCheck()const;

    int HalfMoves50Rule()const;
    int Moves()const;
    int CurrentPly()const;

    std::string   GetFEN      ()const;
    std::string   GetFENShort ()const;
    std::string   GetFENShort2()const;

    Util::Zobrist::HashType GetZHash    ()const;
    Util::Zobrist::HashType GetZHashPawn()const;

    static bool IsEndGame(const Position & p);
    bool IsEndGame()const;

    int GamePhase()const;
    int GamePhasePercent()const;

    static const int maxGamePhase;
    static const int endGameGamePhasePercent;

    static bool IsVeryEndGame(const Position & p);
    bool IsVeryEndGame()const;

    // this one MUST be inlined
    int NumberOf(Piece::eType t)const;

    // this one MUST be inlined
    bool IsTherePieceOnBoard(Piece::eType t)const;

private:

    Position();

    void _ClearBoard();
    void _BuildBoard(const std::string & s);

    void _AddCastlingRight(Player & player, const Player::eCastlingRight & cr);
    void _RemoveCastlingRight(Player & player, const Player::eCastlingRight & cr);

    void _XORHash(Util::Zobrist::HashType h);

    void _XORHashPawn(Util::Zobrist::HashType h);

    //----------
    // vars of a Position
    //----------

    NSColor::eColor        _c;
    Piece::eType           _board[BOARD_SIZE];
    BitBoards              _bitBoard;
    mutable AdditionalInfo _info;

};
#pragma pack(pop)

#include "position.hpp"
