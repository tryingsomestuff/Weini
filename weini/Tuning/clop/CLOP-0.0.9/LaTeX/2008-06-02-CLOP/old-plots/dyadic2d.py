from plot_parameters import *

exp = Experiment(CPQuadratic2D())
exp.sp = CSPDyadic(2)
exp.seed(0)
exp.run(127)

do_tikz(exp)
