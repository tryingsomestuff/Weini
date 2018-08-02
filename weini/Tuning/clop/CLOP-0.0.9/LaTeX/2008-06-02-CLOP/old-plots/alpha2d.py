from plot_parameters import *

exp = Experiment(CPQuadratic2D())
exp.add_qlr()
var = CDFVarianceAlpha(exp.reg)
var.SetMinSamples(2)
exp.reg.GetPF().SetPriorStrength(1e-3)
exp.sp = CSPVOptimal(exp.reg, var)
exp.seed(2)
exp.run(127)

do_tikz(exp)
