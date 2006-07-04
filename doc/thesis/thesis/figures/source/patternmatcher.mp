% contents: unknown
%
% Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>

input boxes;
input nw-trees;
input mp-text;
input ../../../include/regex-syntax-trees;

% source:patternmatcher:functions - alternation {{{1
beginfig(1);
  ALT.root(
    leaf(btex $c_l$ etex),
    leaf(btex $c_r$ etex));
  drawtrees(root);
endfig;


% source:patternmatcher:functions - concatenation {{{1
beginfig(2);
  CONS.root(
    leaf(btex $c_l$ etex),
    leaf(btex $c_r$ etex));
  drawtrees(root);
endfig;


% source:patternmatcher:functions - closure {{{1
beginfig(3);
  CLOSURE.root(
    leaf(btex $c$ etex));
  drawtrees(root);
endfig;


% }}}1

end.
