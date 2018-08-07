#!/usr/bin/env python
#############################################################################
"""
 htest.py

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

class MyExperimentBuilder(ExperimentBuilder):
    def build(self):
        #####################################################################
        "Build experiment"
        #####################################################################
        p = self.problem_class()

        exp = Experiment(p)
        pf = CPFQuadratic(p.GetDimensions())
        #pf = CLinearCubic(p.GetDimensions())
        pf.SetPriorStrength(1e-2)
        exp.reg = CRegression(exp.res, pf)
        exp.reg.SetRefreshRate(0.1)
        exp.reg.SetLocalizationHeight(self.LocalizationHeight)
        exp.reg.SetLocalizationPower(self.LocalizationPower)
        exp.me = CMESampleMean(exp.reg)
        #exp.me = CMERegressionMAPMax(exp.reg)
        exp.sp = CSPWeight(exp.reg, replications, gibbs)
    
        return exp

def test_class(problem_class, h_values, power):
    #########################################################################
    "test a class"
    #########################################################################
    eb = MyExperimentBuilder()
    eb.problem_class = problem_class
    
    for H in h_values:
        eb.LocalizationHeight = H
        eb.LocalizationPower = power
        s = problem_class.__name__ + "-H=%f-Power=%f" % (H, power)
        eb.threads(repeats, samples, filename = s)

def CPLog5D():
    #########################################################################
    "5 * CPLog1D"
    #########################################################################
    return CPMultiple(5, CPLog1D())

#############################################################################
"Main program"
#############################################################################
test_class(CPLog1D, [1, 2, 3, 4, 6, 8, 10, 12], 0)
test_class(CPLog1D, [0.8], 1.0 / 6.0)

test_class(CPLog5D, [1, 2, 3, 4, 6, 8, 10, 12], 0)
#test_class(CPLog5D, [0.55], 1.0 / 6.0)

test_class(CPPower1D, [1, 2, 3, 4, 6, 8, 10, 12], 0)
