# Weini chess engine  
## WisEness Is Not Inside  

Project started : december 2016  
Language : C++11  
Compiles on both Linux (CMake, no dependency) and Windows (Visual Studio 17)  

*Weini* is the chess engine I develop just for fun and learn about chess programming.
A lots of ideas are taken from _chessprogramming wiki_ and _talkchess_ forum. 
Many thanks to all the chess programming community for being so kind and interesseting.

*Weini* is still under development and testing and not all the implemented features are validated.

*Weini* comes with a tunable configuration file (in json), and with good logging features. 

*Weini* is quite slow, move validation and attack detection are still more bullet proof than optimized ...

Only releases (just before or after HGM monthly tournament) are available here : https://github.com/tryingsomestuff/Weini/releases

* Multi-threads (lazySMP)
* alpha-beta framework (negamax)  
    * PVS (root and alpha-beta)  
    * Aspiration window  
    * IID  
* Transposition
    * TT (mainly for sorting, always replace and best depth buckets)  
    * QTT (same)  
    * EvalTT (always replace)
    * PawnTT (always replace)
* Kind of LazyEval with own TT (always replace)  
* Sort with  
    * Piece position evaluation (PSQT)  
    * MVV-LVA  
    * Killer  
    * History  
    * Counter  
    * LastCaptureBonus  
    * SEE (too slow)  
    * Checks priority  
    * En-passant priority  
* QSearch   
    * SEE  
    * Delta prunning  
    * Futility pruning  
* Selectivity
    * Futility pruning (extended with adaptative margin)  
    * Razoring (reduction or drop to QSearch directly)  
    * Null move prunning (adaptative) : under condition verified null move prunning / reduction  
    * Move Count Pruning ; too risky for now ...  
    * LMR (adaptative)  
* Extension  
    * Single reply  
    * Check  
    * Promotion (and near promotion)  
    * End game  
    * Very end game  
    * PV  
    * Singular  
    * Recapture  
* Books  
    * Small internal  
    * Big, read from file (too slow !)  
* Evaluation  
    * material  
    * PSQT 
    * piece pair  
    * blocked pieces
    * mobility (global and by piece) (too expensive)  
    * pawn push  
    * pawn shield  
    * castling  
    * center control (too expensive)  
    * king troppism (too expensive)  
    * open file  
    * pawn structure  
        * double  
        * isolated  
        * passed  
        * candidate  
        * protected passed  
        * connected  
        * king  too far (buggy)  
* Protocols
    * XBOARD (partial)  
    * UCI (partial)

Available lichess BOT : https://lichess.org/@/weini

## How to compile

Use CMake to compile. Or build the command line yourself, something like `g++ src/*.cpp -Iheader/ --std=c++11 -msse4.2 -O3 -mpopcnt -flto -o weini` or any optimization you think shall be profitable ...

As been built on linux (g++ >= 4.8, clang) and Windows (visual studio >=17, or last version of cygwin).

## How to run

`weini -xboard` in your favorite GUI (Xboard/Winboard for example).
