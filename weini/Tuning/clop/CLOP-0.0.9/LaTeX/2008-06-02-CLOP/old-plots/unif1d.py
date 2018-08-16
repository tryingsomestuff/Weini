from plot_parameters import *

exp = Experiment(CPQuadratic1D(-0.25, 2.0, 4*(7*7)))
exp.add_qlr()
exp.sp = CSPUniform(exp.p.GetDimensions())
exp.seed(2)
exp.run(127)

do_tikz(exp)
