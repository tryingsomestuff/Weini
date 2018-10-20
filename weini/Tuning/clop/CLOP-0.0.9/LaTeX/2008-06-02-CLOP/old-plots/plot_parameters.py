from qlr import *

def build_tikz(p, scale):
    #########################################################################
    "build tikz object"
    #########################################################################
    if p.GetDimensions() == 1:
        tikz = CTikZOneD()
        tikz.SetScale(5.5)
    elif p.GetDimensions() == 2:
        tikz = CTikZTwoD()
        tikz.SetScale(2.3)
        tikz.SetContourResolution(40)

    if scale != 0:
        tikz.SetScale(scale)

    return tikz

def set_graphical_parameters(tikz):
    #########################################################################
    "set graphical parameters"
    #########################################################################
    my_dash = "dash pattern = on 12pt off 2pt"

    tikz.SetStyle("Confidence", 0.10, 0.10, 0.80, "")
    tikz.SetStyle("Posterior",  0.50, 1.00, 0.50, "")
    tikz.SetStyle("MAP",        0.60, 0.10, 0.00, "thick")
    tikz.SetStyle("Expected",   0.90, 0.00, 0.10, "thick," + my_dash)
    tikz.SetStyle("True",       1.00, 0.00, 0.00, "thick,densely dotted")
    tikz.SetStyle("Contour",    0.80, 0.80, 1.00, "")

#    tikz.SetGrayscale(True)

    #
    # Accuracy parameters
    #
    tikz.SetCircleN(2)
    tikz.SetSplineSamples(5000)
    tikz.SetSplineD(0.0018)

def full_plot(exp, tikz, seed, expected, confidence, plot_true, plot_samples):
    #########################################################################
    "full plot"
    #########################################################################
    rnd = Random(seed)

    tikz.Prolog()
    tikz.BeginClip()

    if exp.p.GetDimensions() == 2:
        tikz.Contour(exp.p, 12)

    if plot_samples:
        tikz.Results(exp.res)

    if exp.p.GetDimensions() == 1:
        reg = exp.reg
        tikz.Posterior(20, reg, rnd)
        if confidence > 0:
            tikz.Confidence(reg, confidence)
            tikz.Confidence(reg, -confidence)
        tikz.MAP(reg)
        if expected > 0:
            tikz.Expected(reg, expected, seed)
        if plot_true:
            tikz.True(exp.p)

    tikz.EndClip()
    if plot_samples:
        tikz.Frame(exp.res.GetSamples())
    else:
        tikz.Frame(0)
    tikz.Key()
    tikz.Epilog()

def do_tikz(exp,
            scale = 0,
            seed = 0,
            expected = 0,
            confidence = 0,
            plot_true = False,
            plot_samples = True):
    #########################################################################
    "plot exp with TikZ"
    #########################################################################
    tikz = build_tikz(exp.p, scale)
    set_graphical_parameters(tikz)
    full_plot(exp, tikz, seed, expected, confidence, plot_true, plot_samples)

def tikz_problem(p, scale = 0):
    #########################################################################
    "plot 1D problem with TikZ"
    #########################################################################
    tikz = build_tikz(p, scale)
    tikz.SetStyle("True", 1.00, 0.00, 0.00, "thick")
    tikz.Prolog()
    tikz.BeginClip()
    tikz.True(p)
    tikz.EndClip()
    tikz.Frame(0)
    tikz.Epilog()
