WEINI chess engine  
WisEness Is Not Inside  

alpha-beta framework (negamax)  
   PVS (root and alpha-beta)  
   Aspiration window  
   IID  
TT (mainly for sorting)  
QTT (idem)  
EvalTT  
PawnTT
Sort of LazyEval  
Sort with  
   Piece position evaluation (PSQT)  
   MVV-LVA  
   Killer  
   History  
   Counter  
   LastCaptureBonus  
   SEE (too slow)  
   Checks priority  
   En-passant priority  
QSearch   
   SEE  
   Delta prunning  
   Futility pruning  
Futility pruning (extended with adaptative margin)  
Razoring (reduction or drop to QSearch directly)  
Null move prunning (adaptative) : under condition verified null move prunning / reduction  
Move Count Pruning ; too risky for now ...  
Extension  
   Single reply  
   Check  
   Promotion (and near promotion)  
   End game  
   Very end game  
   PV  
   Singular  
   Recapture  
LMR (adaptative)  
Book  
   Small internal  
   Big, read from file (too slow !)  
Evaluation  
   material  
   PSQT 
   piece pair  
   blocked pieces
   mobility (global and by piece) (too expensive)  
   pawn push  
   pawn shield  
   castling  
   center control (too expensive)  
   king troppism (too expensive)  
   pawn structure  
      double  
      isolated  
      passed  
      candidate  
      protected passed  
      connected  
      king too far (buggy)  
   open file  
XBOARD (partial)  
UCI (partial)

Available lichess BOT

