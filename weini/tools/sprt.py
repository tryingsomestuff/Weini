from __future__ import division
import sys
import math

def LL(x):
    return 1/(1+10**(-x/400))

def LLR(W,D,L,elo0,elo1):
    """
This function computes the log likelihood ratio of H0:elo_diff=elo0 versus
H1:elo_diff=elo1 under the logistic elo model

expected_score=1/(1+10**(-elo_diff/400)).

W/D/L are respectively the Win/Draw/Loss count. It is assumed that the outcomes of
the games follow a trinomial distribution with probabilities (w,d,l). Technically
this is not quite an SPRT but a so-called GSPRT as the full set of parameters (w,d,l)
cannot be derived from elo_diff, only w+(1/2)d. For a description and properties of
the GSPRT (which are very similar to those of the SPRT) see

http://stat.columbia.edu/~jcliu/paper/GSPRT_SQA3.pdf

This function uses the convenient approximation for log likelihood
ratios derived here:

http://hardy.uhasselt.be/Toga/GSPRT_approximation.pdf

The previous link also discusses how to adapt the code to the 5-nomial model
discussed above.
"""
# avoid division by zero
    if W==0 or D==0 or  L==0:
        return 0.0
    N=W+D+L
    w,d,l=W/N,D/N,L/N
    s=w+d/2
    m2=w+d/4
    var=m2-s**2
    var_s=var/N
    s0=LL(elo0)
    s1=LL(elo1)
    return (s1-s0)*(2*s-s0-s1)/var_s/2.0

def SPRT(W,D,L,elo0,elo1,alpha,beta):
    """
This function sequentially tests the hypothesis H0:elo_diff=elo0 versus
the hypothesis H1:elo_diff=elo1 for elo0<elo1. It should be called after
each game until it returns either 'H0' or 'H1' in which case the test stops
and the returned hypothesis is accepted.

alpha is the probability that H1 is accepted while H0 is true
(a false positive) and beta is the probability that H0 is accepted
while H1 is true (a false negative). W/D/L are the current win/draw/loss
counts, as before.
"""
    LLR_=LLR(W,D,L,elo0,elo1)
    LA=math.log(beta/(1-alpha))
    LB=math.log((1-beta)/alpha)
    if LLR_>LB:
        return 'H1'
    elif LLR_<LA:
        return 'H0'
    else:
        return 'not sure'


if __name__ == "__main__":
    print SPRT(int(sys.argv[1]),int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4]),int(sys.argv[5]),float(sys.argv[6]),float(sys.argv[7]))

