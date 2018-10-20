from plot_parameters import *

exp = Experiment(CPQuadratic2D())
cem = CCrossEntropy(exp.res, 1.0, 0.0, 10, 5, 3)
exp.sp = cem
exp.seed(0)
exp.run(127)

do_tikz(exp)
