WEINI chess engine
WisEness Is Not Inside

alpha-beta framework (negamax)
   PVS (root and alpha-beta)
   Window
   IID
TT (mainly for sorting)
QTT (idem)
EvalTT
LazyEval
Sort with
   Piece position evaluation (PST)
   MVV-LVA
   Killer
   History
   Counter
   LastCaptureBonus
   SEE (not working ...)
   Checks
QSearch
   SEE (not working)
   Delta prunning (not working)
Futility pruning (extended with adaptative margin)
Razoring (reduction)
Static null move (reversed futility pruning) : drop to QSearch or reduction
Null move prunning (adaptative) : under condition verified null move prunning / reduction
Extension
   Single reply
   Check
   Promotion (and near promotion)
   End game
   Very end game
   PV
LMR (adaptative)
Book
   Small internal
   Big, read from file (too slow !)
Evaluation
   material
   position
   bishop pair
   mobility (global and by piece)   (too expensive)
   pawn push
   pawn shield
   castling
   rook connected
   rook on 7th rank
   center control (too expensive)
   king troppism (very expensive)
   pawn structure
      double
      isolated
      passed
      protected passed
      connected
      king too far (buggy)
   open file
XBOARD (partial)

