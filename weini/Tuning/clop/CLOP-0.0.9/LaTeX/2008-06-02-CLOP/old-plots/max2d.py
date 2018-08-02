from plot_parameters import *

exp = Experiment(CPQuadratic2D())
exp.add_qlr()
exp.sp = CSPQLRMax(exp.reg)
exp.seed(2)
exp.run(127)

do_tikz(exp)
