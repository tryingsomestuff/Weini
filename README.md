# WEINI chess engine  
## WisEness Is Not Inside  

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

Available lichess BOT

