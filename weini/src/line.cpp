#include "line.h"
#include "position.h"

Line & Line::operator=(const Line & l){
   n = l.n;
   for(DepthType k = 0 ; k < n ; ++k){
       moves[k] = l.moves[k];
   }
   return *this;
}

void Line::ClearPV(Line * pv){
    if (pv) {
        pv->n = 0;
    }
}

void Line::ClearPV(Line & pv){
    pv.n = 0;
}

void Line::SetPV(Line * pv,const MiniMove & m){
    // init the pv (this will be the last move in it)
    if (pv) {
        pv->moves[0] = m; // copy
        pv->n = 1;
    }
}

void Line::UpdatePV(Line * pv, const Line & pv_loc, const MiniMove & m, const Position & p){
    // update pv
    if (pv) {
        pv->moves[0] = m; // copy
        for (DepthType k = 0; k < pv_loc.n; ++k) {
            pv->moves[k + 1] = pv_loc.moves[k];
        }
        pv->n = pv_loc.n + 1;
        if ( pv->n >= MAX_SEARCH_DEPTH ){
            LOG(logFATAL) << "PV is too long ... " << pv->n << " " << p.GetFEN() << " : " << pv->GetPV(p,true);
        }
    }
}

std::string Line::ShowPV(){
    std::string s;
    for(DepthType k = 0 ; k < n ; ++k){
        s += Move(moves[k]).Show() + " " ;
    }
    return s;
}

std::string Line::GetPV(const Position & pp, bool abr) {
    std::string s;
    Position p(pp);
    for(DepthType k = 0 ; k < n ; ++k){
        Move m(moves[k]);
        if (p.IsPawn(m.From())) UtilMove::ValidateEnPassant(m, p);
        UtilMove::CheckForCapture(m, p);
        UtilMove::ValidateIsCheck(m, p);
        s+= (abr?m.ShowAlgAbr(p):m.ShowAlg(p,false)) + " ";
        p.ApplyMove(m);
    }
    return s;
}
