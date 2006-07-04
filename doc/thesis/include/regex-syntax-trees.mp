% contents: Regular expression abstract syntax tree definitions.
%
% Copyright (C) 2004 Nikolai Weibull <source@pcppopper.org>

def_nonterminal(CONS, textext("$\mathstrut\circ$"));
def_nonterminal(CLOSURE, textext("$\mathstrut{}^\ast$"));
def_nonterminal(ALT, textext("$\mathstrut|$"));
def_nonterminal(LEFT, textext("$\mathstrut($"));
def_nonterminal(RIGHT, textext("$\mathstrut)$"));
def_terminal(ZERO, textext("$\mathstrut0$"));
def_terminal(ONE, textext("$\mathstrut1$"));
