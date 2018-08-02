from plot_parameters import *

exp = Experiment(CPQuadratic1D(-0.3, 0.5, 4.0))
exp.add_qlr()
exp.sp = CSPUniform(1)
exp.seed(1)
exp.run(127)

do_tikz(exp)
