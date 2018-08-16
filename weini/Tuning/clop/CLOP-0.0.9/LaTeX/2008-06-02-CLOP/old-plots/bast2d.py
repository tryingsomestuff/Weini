from plot_parameters import *


exp = Experiment(CPQuadratic2D())
bast = CBAST(exp.res)
exp.sp = bast
exp.seed(1)
exp.run(127)

do_tikz(exp)
