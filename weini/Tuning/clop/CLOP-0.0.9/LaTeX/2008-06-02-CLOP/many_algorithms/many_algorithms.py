#!/usr/bin/env python
#############################################################################
"""
 many_algorithms.py

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

class Quadratic_CLOP_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "Quadratic CLOP"
        #####################################################################
        p = CPLog1D()
        exp = Experiment(p)
        pf = CPFQuadratic(p.GetDimensions())
        pf.SetPriorStrength(1e-2)
        exp.reg = CRegression(exp.res, pf)
        exp.reg.SetRefreshRate(0.1)
        exp.reg.SetLocalizationHeight(self.LocalizationHeight)
        exp.reg.SetLocalizationPower(self.LocalizationPower)
        exp.me = CMESampleMean(exp.reg)
        exp.sp = CSPWeight(exp.reg, replications, gibbs)
    
        return exp

class Cubic_CLOP_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "Cubic CLOP"
        #####################################################################
        p = CPLog1D()
        exp = Experiment(p)
        pf = CPFCubic(p.GetDimensions())
        pf.SetPriorStrength(1e-2)
        exp.reg = CRegression(exp.res, pf)
        exp.reg.SetRefreshRate(0.1)
        exp.reg.SetLocalizationHeight(0.8)
        exp.reg.SetLocalizationPower(1.0 / 4.0)
        exp.me = CMERegressionMAPMax(exp.reg)
        exp.sp = CSPWeight(exp.reg, replications, gibbs)
    
        return exp

class UCT_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "UCT"
        #####################################################################
        p = CPLog1D()
        exp = Experiment(p)
        uct = CBAST(exp.res)
        exp.me = uct
        exp.sp = uct
    
        return exp

class Basic_CEM_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "Basic CEM"
        #####################################################################
        p = CPLog1D()
        exp = Experiment(p)
        cem = CCrossEntropy(exp.res, 0.9, 0.0, False, 100, 10, 10, 1.15, True)
        exp.me = cem
        exp.sp = cem
    
        return exp

class CEM_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "CEM"
        #####################################################################
        p = CPLog1D()
        exp = Experiment(p)
        cem = CCrossEntropy(exp.res, 0.9, 0.1, True, 100, 10, 10, 1.15, True)
        exp.me = cem
        exp.sp = cem
    
        return exp

class RSPSA_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "CEM"
        #####################################################################
        p = CPLog1D()
        exp = Experiment(p)
        rspsa = CRSPSA(exp.res, 1000,  # Batch size
                                1.0,   # Batch growth
                                1.00,  # Eta+
                                0.90,  # Eta-
                                0.0,   # DeltaMin
                                0.02,  # DeltaMax
                                0.019, # Delta0
                                25.0   # Rho
                      )
        exp.me = rspsa
        exp.sp = rspsa
    
        return exp

class SPSA_star_EB(ExperimentBuilder):
    def build(self):
        #####################################################################
        "SPSA*"
        #####################################################################
        p = CPLog1D()
        exp = Experiment(p)
        spsa = CSPSA(exp.res, 3.00, 0.0, 0.1, 1.0, 1.0 / 6.0)
        exp.me = spsa
        exp.sp = spsa
    
        return exp


#############################################################################
"Main program"
#############################################################################
q_clop_eb = Quadratic_CLOP_EB()

q_clop_eb.LocalizationHeight = 3
q_clop_eb.LocalizationPower = 0
q_clop_eb.threads(repeats, samples, "QuadraticClop-H=3")

q_clop_eb.LocalizationHeight = 0.8
q_clop_eb.LocalizationPower = 1.0 / 6.0
q_clop_eb.threads(repeats, samples, "QuadraticClop-H=0.8-Power=0.17")

c_clop_eb = Cubic_CLOP_EB()
c_clop_eb.threads(repeats, samples, "CubicClop")

uct_eb = UCT_EB()
uct_eb.threads(repeats, samples, "UCT")

cem_eb = CEM_EB()
cem_eb.threads(repeats, samples, "CEM")

basic_cem_eb = Basic_CEM_EB()
basic_cem_eb.threads(repeats, samples, "Basic_CEM")

rspsa_eb = RSPSA_EB()
rspsa_eb.threads(repeats, samples, "RSPSA")

spsa_star_eb = SPSA_star_EB()
spsa_star_eb.threads(repeats, samples, "SPSA_Star")
