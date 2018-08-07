# x64/Release/chessEngine.exe -analysis "k7/2q5/8/8/8/6Q1/8/1K6 b - -" 2   ### easy must take queen
# x64/Release/chessEngine.exe -analysis "k7/8/8/8/3n4/6Q1/8/2K5 b - -" 3  ### easy fork with knight
# x64/Release/chessEngine.exe -analysis "k3r3/8/8/8/8/2Q5/8/2K5 w - -" 3  ### easy fork with check
# x64/Release/chessEngine.exe -analysis "k7/8/8/7r/4N3/6K1/6B1/8 w - -" 3  ### easy discover check + fork
# x64/Release/chessEngine.exe -analysis "4r2k/R5b1/4N3/8/8/8/1B6/K7 w - -" 4  ### rook should take  black will lose rook after taking knight
# x64/Release/chessEngine.exe -analysis "k6b/5P2/8/8/2K5/8/8/8 w - -" 2   ### should promote to queen
# x64/Release/chessEngine.exe -analysis "bn5K/k1P5/7R/8/8/8/8/7B w - - 0 1" 3 ### should pomote to knight
# x64/Release/chessEngine.exe -analysis "8/2k5/NR6/8/8/8/5p1B/K7 b - - 0 1" 6  ### should take rook and then promote to queen  check twice and take bishop. Final score: 900-320 = 580
# x64/Release/chessEngine.exe -analysis "k5r1/1r6/8/8/8/8/8/7K b - -" 2  ### check mate with rook
# x64/Release/chessEngine.exe -analysis "k7/8/8/8/8/2q5/8/6RK b - -" 2  ### check mate with queen
# x64/Release/chessEngine.exe -analysis "8/8/3k4/8/3b1n1r/6p1/7p/7K b - -" 2  ### check mate with pawn
# x64/Release/chessEngine.exe -analysis "7k/8/6P1/3B4/8/8/5B2/1q4RK w - -" 3  ### check mate with bishop
# x64/Release/chessEngine.exe -analysis "8/8/8/8/8/6R1/5R2/4K2k b - -" 2  ### stalemate
# x64/Release/chessEngine.exe -analysis "8/8/q7/8/7R/4N1n1/4N3/4K1k1 b - -" 2  ### white have check but black can take knight and win
# x64/Release/chessEngine.exe -analysis "8/8/8/8/7q/2k5/8/1K6 b - -" 3  ### check mate with queen in 2 moves
# x64/Release/chessEngine.exe -analysis "1R6/8/8/6k1/8/8/1K6/5R2 w - - 0 1" 4  ### mate with two rooks : iterative deepening and move order needed to ensure best check mate sequence is found
# x64/Release/chessEngine.exe -analysis "7R/1rk5/1pp5/b7/2n1N3/4PP2/5K1p/6R1 w - - 0 1" 3  ### check mate in one  should stop deepening
# x64/Release/chessEngine.exe -analysis "b7/k1P5/5R2/8/8/8/3K4/1R6 w - - 0 1" 3  ### check mate with promotion to knight
# x64/Release/chessEngine.exe -analysis "8/8/8/5q2/8/1k6/7p/KQ6 b - - 0 1" 3  ### black are check take queen and promote to queen or rook
# x64/Release/chessEngine.exe -analysis "5q1k/4q3/8/8/8/3K4/8/8 b - - 0 1" 7  ### best checkmate sequence in 5 ply
# x64/Release/chessEngine.exe -analysis "K3k2r/NB6/3n4/8/3B4/8/r7/8 b k - 0 1" 5  ### castling wins in 2
# x64/Release/chessEngine.exe -analysis "8/8/8/6b1/8/4qn1k/PPP5/NK1R4 b - -" 4  ### queen sacrifice to win
# x64/Release/chessEngine.exe -analysis "1Bb3BN/R2Pk2r/1Q5B/4q2R/2bN4/4Q1BK/1p6/1bq1R1rb w - - 0 1" 3  ### crazy checkmate in 1 : Qa3#
# x64/Release/chessEngine.exe -analysis "8/8/pp6/kp6/p7/R1K5/1P6/8 w - - 0 1" 3  ### forbidden en passant
# x64/Release/chessEngine.exe -analysis "2q5/K1k1p3/8/1pPQ4/8/8/8/8 w - b5 0 2" 3  ### mate with en passant   ###/@todo ERROR IN FEN en-passant definition : should be b6 here    FIX ME FIX ME FIX ME
# x64/Release/chessEngine.exe -analysis "rr4k1/5nb1/8/8/7R/1Bq5/8/1K4RQ w - - 0 1" 2  ### everyone is pinned !
# x64/Release/chessEngine.exe -analysis "8/b3nbp1/5k2/3Pp3/3P4/4KN2/7B/1B6 w - - 0 1" 2  ### mate with bishop
# x64/Release/chessEngine.exe -analysis "k7/2K5/1P6/8/8/8/8/8 w - - 0 1" 6  ### check  get queen  check and mate
# x64/Release/chessEngine.exe -analysis "8/8/8/6P1/p6p/P7/K1k5/8 w - -" 9  ### promotion race to near null score
# x64/Release/chessEngine.exe -analysis "8/8/8/6P1/p1k4p/P7/K7/8 w - - 0 1" 12  ### white wins
# x64/Release/chessEngine.exe -analysis "8/8/8/8/8/1kp5/2P5/1K6 b - - 0 1" 20 ### draw
# x64/Release/chessEngine.exe -analysis "1q6/2b2r2/8/8/8/B4k2/8/KQ6 w - - 0 1" 5  ### tempting to take queen but then lose own queen
# x64/Release/chessEngine.exe -analysis "kr5r/ppp3pp/2n1bp2/8/8/1P2N3/PBP2PPP/KR5R w - - 0 1" 2  ### open file
# x64/Release/chessEngine.exe -analysis "p2k2p1/Pp3pPp/pPp1pPpP/PpPpPpPp/pPpPpPpP/PpPpPpP1/1PpPpP2/2P1P1K1 w - -" 4  ### stalemate
# x64/Release/chessEngine.exe -analysis "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1" 2
# x64/Release/chessEngine.exe -analysis "5r1k/4Qpq1/4p3/1p1p2P1/2p2P2/1p2P3/3P4/BK6 b - -" 12  ### very hard draw with horizon effect !!!
# x64/Release/chessEngine.exe -analysis "5k2/ppp2p2/8/7P/b6r/P4K2/1P6/1Q6 b - - 0 1" 5  ### rook sacrifice to capture queen
# x64/Release/chessEngine.exe -analysis "8/8/1q2k3/8/8/2N5/6PP/5R1K w - - 0 1" 4  ### rook sacrifice + fork gets queen
# x64/Release/chessEngine.exe -analysis "8/8/7p/2p5/PpPp4/1P1Pp1p1/3pPppp/3K1nbk w - - 0 1" 10  ### hard stalemate
# x64/Release/chessEngine.exe -analysis "8/8/8/8/8/4p3/3nP3/3K4 w - -" 3  ### debug
# x64/Release/chessEngine.exe -analysis "6r1/2rp1kpp/2qQp3/p3Pp1P/1pP2P2/1P2KP2/P5R1/6R1 w - - 0 1" 14  ### Shirov
# x64/Release/chessEngine.exe -analysis "1p1p1k2/pPpPpBp1/PpPpPpPp/pPpPpPpP/PpPpPpPp/pPpPpPpP/P1PpPpPp/1K1P1P1P w - - 0 1" 3  ### debug
# x64/Release/chessEngine.exe -analysis "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 3  ### initial position
# x64/Release/chessEngine.exe -analysis "1r3rk1/1b4Np/5p1R/8/2q5/5P2/2BQ2PP/6RK w - - 1 1" 15  ### forced mate in 7
# x64/Release/chessEngine.exe -analysis "8/8/p4B2/Pp4pp/1P5k/5P2/4q1PK/8 w - - 0 3" 15  ### white wins with queen takes rook
# x64/Release/chessEngine.exe -analysis "2k5/8/1pP1K3/1P6/8/8/8/8 w – – 0 1" 31 ### should play c7 !
