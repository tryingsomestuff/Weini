# Weini chess engine  
## WisEness Is Not Inside  

Project started : december 2016  
Language : C++11  
Compiles on both Linux (CMake, no extra dependency needed) and Windows (Visual Studio 17, CYGWIN)  
(a cross-compiled mingw version has also been tested successfully)

*Weini* is the chess engine I develop just for fun and learn about chess programming.
A lots of ideas are taken from _chessprogramming wiki_ and _talkchess_ forum. 
Many thanks to all the chess programming community for being so kind and interesseting.

Here's an incomplete list of inspiring open-source engines :
* Arasan by Jon Dart
* CPW by Pawel Koziol and Edmund Moshammer
* Deepov by Romain Goussault 
* Dorpsgek by Dan Ravensloft
* Galjoen by Werner Taelemans
* Rodent III by Pawel Koziol
* Stockfish by Tord Romstad, Marco Costalba, Joona Kiiski and Gary Linscott
* TSCP by Tom Kerrigan
* Vajolet by Marco Belli
* Vice by BlueFeverSoft
* Xiphos by Milos Tatarevic

*Weini* is still under development and testing and not all the implemented features are validated.

*Weini* comes with a highly tunable configuration file (in json format), and with good logging features. 
The *config.json* file must be in the same directory as the executable.

*Weini* is quite slow, move validation and attack detection are still more bullet proof than optimized ... *Weini* is using a mailbox structure as well as some very simple bitboards and uses the copy-make paradigm.

Only releases (often just before or after HGM monthly tournament) are available here : https://github.com/tryingsomestuff/Weini/releases

*Weini* makes use of the beautiful (very slightly modified) TCLAP header only library to parse the command line argument (inside the "dep" directory).

*Weini* is using mostly all the classic stuff :

* Multi-threads (lazySMP, up to 20 threads)
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
    * SEE  
    * Checks priority  
    * En-passant priority  
* QSearch   
    * SEE  
    * Delta prunning  
    * Futility pruning  
* Selectivity
    * Futility pruning (extended with adaptative margin)  
    * Razoring (drop to QSearch directly)  
    * Static null move
    * Null move prunning (adaptative) : under condition verified null move prunning / reduction  
    * Move Count Pruning  
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
    * UCI (very partial and buggy)  

*Weini* is sometimes playing on lichess, the available lichess BOT is : https://lichess.org/@/weini

## How to compile

Simply use CMake to compile. Or build the command line by yourself, something like `g++ src/*.cpp -Iheader/ -Idep/ --std=c++11 -msse4.2 -O3 -mpopcnt -flto -o weini` or any optimization you think shall be profitable ...

As been built on linux (g++ >= 4.8, clang) and Windows (visual studio >=17, or last version of cygwin).

## How to run

`weini --xboard` or `weini -x` in your favorite GUI (Xboard/Winboard for example).
**Be carrefull** the command line has changed since release 0.0.22, now a double "-" is needed for the "long" version of the argument. 

## Other options

Just ask !

```
./weini -h

USAGE: 

   ./bin/weini  [-t <string>] [-o <string>] ...  [-u] [-x] [-l] [--]
                [--version] [-h]


Where: 

   -t <string>,  --test <string>
     Run a specific test (help for getting some hints)

   -o <string>,  --option <string>  (accepted multiple times)
     Override an option given in the config.json file

   -u,  --uci
     Switch to uci mode directly (default is false)

   -x,  --xboard
     Switch to xboard mode directly (default is false)

   -l,  --logInFile
     Output log to file instead of stdout (default is false)

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.
```

Every options from the *config.json* file can be override by hand from the command line

`./weini -x -o "ttsize 36"`

can be use to set a 36Mb TT by hand. This is usefull for running bench without changing the configuration file.

Every options from the *config.json* file can also be override using xboard command "setoption" ; the same example

`setoption ttsize 36`

This is usefull when using clop for example !

## Books

*Weini* does not support classic opening book formats, nor classic endgame table formats.
But *Weini* comes with 2 books : a "simple" one, very small and hard coded, and a bigger one named "book3.pgn" (although this is not really a pgn file ... sorry). All of this is easily configurable using the *config.json* file.

Ascii books are long to read, so *Weini* will convert the book to a binary one as soon as it reads it for the first time and then will use the binary one forever.

You can of course give your own book as soon as it is using the good format, which is a "line" each line and moves separated by a simple space written in (english) algebraic notation.

## Tests

*Weini* has some little tests facilities inside : `./weini -t help` will be helpfull. Somes classic analysis tests (BT2630, Arasan, CCROH, BK, KM, STS, ...) and some "unitary" tests (but the code coverage is bad ...)

A fun command to launch an analysis by hand

`./weini -t "analysis :rn2kb1r/pp2pppp/1qP2n2/8/6b1/1Q6/PP1PPPBP/RNB1K1NR b KQkq - 1 6: 40"`

See how the fen string is given between ":".


