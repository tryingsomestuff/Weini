Welcome to CLOP, a generic tool for automatic parameter optimization

Author: Rémi Coulom, Université Lille 3
Home: http://remi.coulom.free.fr/CLOP/

This software is licensed under the terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html


Building CLOP
=============

Linux
-----
Required Ubuntu packages:
sudo apt-get install ccache g++ swig python-dev libgsl0-dev libboost-dev libboost-thread-dev libboost-filesystem-dev libboost-date-time-dev gnuplot libqt4-dev

cd programs/clop/compgcc
make
cd programs/clop/compqt/clop-console
qmake && make
cd programs/clop/compqt/clop-gui
qmake && make

Windows & Mac
-------------
Get the Qt SDK. Project files in programs/clop/compqt/* can be opened in Qt
Creator and compiled.


Optimizing a program with CLOP
==============================

First step: connection script
-----------------------------
The first step to connect a program to CLOP consists in writing a connection
script. A connection script takes parameter values as input, plays one game,
and returns the game outcome as output. For more information, run "Dummy.exe"
in the Windows distribution, or programs/clop/script/real/DummyScript.py in
Linux.

For Go-playing programs using the GTP protocol, a Python script based on gogui
tools is also provided in programs/clop/script/real/GoguiScript.py. You'll have
to customize this GoguiScript.py for your program.

Second step: experiment description in .clop file
------------------------------------------------
Experiment settings such as the list of processors and parameter ranges are
defined in a separate file with a .clop extension. More details can be found in

programs/clop/script/real/DummyExperiment.clop

The experiment can be run by opening such a .clop file with the clop program.

Third step: run the .clop file
-----------------------------
You can either run it with the clop-gui program, or from the command line with
"clop-console c <myscript.clop". Running the experiment will produce a log file
and a data file. You can find error message and other information in the log.
Every game result is written to the data file. If you run the same experiment
again, clop will find the previous data file, and continue the experiment where
it stopped.

Tip: choosing parameter ranges
------------------------------
In general, it is a good idea to use intervals that are very wide. CLOP should
be clever enough to focus on good values by itself.

One problem is that if you have many variables, and very wide intervals, CLOP
may have to explore uniformly at random for a long time before it finds a
victory. In that case, you can prime the experiment by using very narrow
intervals around reasonable values, so that a few wins are collected. Play a
few games in this setting, then continue the experiment with wide intervals.

Another problem is that CLOP only performs local optimization, and may miss a
better optimum. If you know better parameters, you can also use the same
mechanism to start CLOP in a small area, and widen it later.

Choosing parameter type (ie, GammaParameters vs LinearParameter)
----------------------------------------------------------------
The more quadratic the function to be optimized, the better clop works. So you
may wish to perform a non-linear transformation of parameters to make the
function more quadratic. LinearParameter will scale the parameter range
linearly to normalize it between -1 and +1. GammaParameter will take the
logarithm of the parameter before scaling it. GammaParameter must take positive
values only. If a parameter cannot take negative values, and multiplying it by
a factor has a similar effect as dividing it by the same factor, then
GammaParameter should be used instead of LinearParameter.

Interpreting statistics: clop does not estimate strength accurately
-------------------------------------------------------------------
Win rates displayed in clop-gui are biased. The win rate over samples with
w(x)=1 ("Central" column) is often too optimistic. The win rate over all
samples ("All" column) is pessimistic. clop cannot estimate accurately how
strong the program is at the maximum.

Implementing your own optimization algorithm
============================================

If you wish to implement your own optimization algorithm, the base classes you
need to implement are CSamplingPolicy and CMaxEstimator. You will also probably
have to create a CObserver in order to collect the outcome of games. You can
look at CBAST.{h,cpp} for an example. This class implements all 3 at the same
time.

CLOP is designed that way in order to allow asynchronous results.  When testing
on a cluster, next game may start before we know the result of the previous
game.

If you only want to run games strictly in sequence, you can assume that
OnOutcome() is called right after NextSample(), for the same sample.  With this
assumption, you can write simpler code. This assumption works with artificial
problems, but it does not work with real experiments distributed over more than
one processor.

In order to compile with make, you should add your classes to
programs/clop/compgcc/clop.mk. You'll probably want to put your classes in a
different directory, so you'll have to add to -I and vpath %cpp.

Then, if you wish to access your classes with a python script, you must add
them to programs/clop/swig/clop_swig.i. Just add the relevant #include and
%include by analogy with the other ones. If some of your classes keep
references to objects created in python, you should tell python with
%pythonappend, so that python's memory manager does not free them too early.
You don't have to #include/%include classes that won't be used from python.
Those classes should be in clop.mk only.


That's all
==========

So far, I wrote very little documentation. Take a look at the scripts, the
code, and ask questions. I'll write more documentation as necessary.
