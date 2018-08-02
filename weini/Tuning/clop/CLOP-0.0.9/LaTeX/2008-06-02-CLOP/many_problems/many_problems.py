#!/usr/bin/env python
#############################################################################
"""
 many_problems.py

 Remi Coulom

 August, 2011
"""
#############################################################################
import sys
sys.path.append('../../../programs/clop/swig')
from clop import *

fast_mode = False

if fast_mode:
    repeats = 100
    samples = 10000
    replications = 100
    gibbs = 10
else:
    repeats = 1000
    samples = 10000000
    replications = 1000
    gibbs = 1000

class CLOP_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "CLOP"
        #####################################################################
        p = self.problem_class()

        exp = Experiment(p)
        pf = CPFQuadratic(p.GetDimensions())
        pf.SetPriorStrength(1e-2)
        exp.reg = CRegression(exp.res, pf)
        exp.reg.SetRefreshRate(0.1)
        exp.reg.SetLocalizationHeight(3.0)
        exp.reg.SetLocalizationPower(0.0)
        exp.me = CMESampleMean(exp.reg)
        exp.sp = CSPWeight(exp.reg, replications, gibbs)
    
        return exp

class UCT_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "UCT"
        #####################################################################
        p = self.problem_class()
        exp = Experiment(p)
        uct = CBAST(exp.res)
        exp.me = uct
        exp.sp = uct
    
        return exp

class CEM_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "CEM"
        #####################################################################
        p = self.problem_class()
        exp = Experiment(p)
        cem = CCrossEntropy(exp.res, 0.9)
        exp.me = cem
        exp.sp = cem
    
        return exp

def CPLog5D(): return CPMultiple(5, CPLog1D())
def CPLog2D(): return CPMultiple(2, CPLog1D())
def CPAngle(): return CPNonQuadraticND(1)
def CPIllCorrelated2(): return CPMultiple(2, CPIllCorrelated())
def CPRosenbrock2(): return CPMultiple(2, CPRosenbrock())
def CPRosenbrock5(): return CPMultiple(5, CPRosenbrock())

#############################################################################
"Main program"
#############################################################################
for problem_class in [CPLog1D, CPFlat, CPPower1D, CPAngle, CPComplicated1D, CPRosenbrock, CPLog5D, CPRosenbrock2, CPLog2D, CPIllCorrelated, CPIllCorrelated2, CPRosenbrock5]:
    for eb_class in [UCT_EB, CLOP_EB, CEM_EB]:
        eb = eb_class()
        eb.problem_class = problem_class
        s = problem_class.__name__ + '-' + eb_class.__name__
        eb.threads(repeats, samples, filename = s)
