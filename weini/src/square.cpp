#include "square.h"

const Square::LightSquare Square::Invalide = -1;

const Square::LightSquare Square::squarea1 = Square("a1").index();
const Square::LightSquare Square::squareb1 = Square("b1").index();
const Square::LightSquare Square::squarec1 = Square("c1").index();
const Square::LightSquare Square::squared1 = Square("d1").index();
const Square::LightSquare Square::squaree1 = Square("e1").index();
const Square::LightSquare Square::squaref1 = Square("f1").index();
const Square::LightSquare Square::squareg1 = Square("g1").index();
const Square::LightSquare Square::squareh1 = Square("h1").index();

const Square::LightSquare Square::squarea2 = Square("a2").index();
const Square::LightSquare Square::squareb2 = Square("b2").index();
const Square::LightSquare Square::squarec2 = Square("c2").index();
const Square::LightSquare Square::squared2 = Square("d2").index();
const Square::LightSquare Square::squaree2 = Square("e2").index();
const Square::LightSquare Square::squaref2 = Square("f2").index();
const Square::LightSquare Square::squareg2 = Square("g2").index();
const Square::LightSquare Square::squareh2 = Square("h2").index();

const Square::LightSquare Square::squarea3 = Square("a3").index();
const Square::LightSquare Square::squareb3 = Square("b3").index();
const Square::LightSquare Square::squarec3 = Square("c3").index();
const Square::LightSquare Square::squared3 = Square("d3").index();
const Square::LightSquare Square::squaree3 = Square("e3").index();
const Square::LightSquare Square::squaref3 = Square("f3").index();
const Square::LightSquare Square::squareg3 = Square("g3").index();
const Square::LightSquare Square::squareh3 = Square("h3").index();

const Square::LightSquare Square::squarea4 = Square("a4").index();
const Square::LightSquare Square::squareb4 = Square("b4").index();
const Square::LightSquare Square::squarec4 = Square("c4").index();
const Square::LightSquare Square::squared4 = Square("d4").index();
const Square::LightSquare Square::squaree4 = Square("e4").index();
const Square::LightSquare Square::squaref4 = Square("f4").index();
const Square::LightSquare Square::squareg4 = Square("g4").index();
const Square::LightSquare Square::squareh4 = Square("h4").index();

const Square::LightSquare Square::squarea5 = Square("a5").index();
const Square::LightSquare Square::squareb5 = Square("b5").index();
const Square::LightSquare Square::squarec5 = Square("c5").index();
const Square::LightSquare Square::squared5 = Square("d5").index();
const Square::LightSquare Square::squaree5 = Square("e5").index();
const Square::LightSquare Square::squaref5 = Square("f5").index();
const Square::LightSquare Square::squareg5 = Square("g5").index();
const Square::LightSquare Square::squareh5 = Square("h5").index();

const Square::LightSquare Square::squarea6 = Square("a6").index();
const Square::LightSquare Square::squareb6 = Square("b6").index();
const Square::LightSquare Square::squarec6 = Square("c6").index();
const Square::LightSquare Square::squared6 = Square("d6").index();
const Square::LightSquare Square::squaree6 = Square("e6").index();
const Square::LightSquare Square::squaref6 = Square("f6").index();
const Square::LightSquare Square::squareg6 = Square("g6").index();
const Square::LightSquare Square::squareh6 = Square("h6").index();

const Square::LightSquare Square::squarea7 = Square("a7").index();
const Square::LightSquare Square::squareb7 = Square("b7").index();
const Square::LightSquare Square::squarec7 = Square("c7").index();
const Square::LightSquare Square::squared7 = Square("d7").index();
const Square::LightSquare Square::squaree7 = Square("e7").index();
const Square::LightSquare Square::squaref7 = Square("f7").index();
const Square::LightSquare Square::squareg7 = Square("g7").index();
const Square::LightSquare Square::squareh7 = Square("h7").index();

const Square::LightSquare Square::squarea8 = Square("a8").index();
const Square::LightSquare Square::squareb8 = Square("b8").index();
const Square::LightSquare Square::squarec8 = Square("c8").index();
const Square::LightSquare Square::squared8 = Square("d8").index();
const Square::LightSquare Square::squaree8 = Square("e8").index();
const Square::LightSquare Square::squaref8 = Square("f8").index();
const Square::LightSquare Square::squareg8 = Square("g8").index();
const Square::LightSquare Square::squareh8 = Square("h8").index();

const char Square::mailbox[120] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
	-1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
	-1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
	-1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
	-1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
	-1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
	-1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
	-1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

const char Square::mailbox64[64] = {
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98
};

bool Square::offBoard[64][43];

void Square::InitOffBoard(){
   LOG(logINFO) << "Init OffBoard table";
   for(char s = 0 ; s < 64 ; ++s){
       for (char inc = -21 ; inc <= 21 ; ++inc){
           offBoard[s][inc+21] = mailbox[mailbox64[s]+inc] < 0;
       }
   }
}
