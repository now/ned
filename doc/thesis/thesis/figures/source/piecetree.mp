% contents: PieceTree related graphics.
%
% Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>

input boxes;
input nw-trees;

def drawcenteredge(suffix a,b) =
  draw a.c..b.c cutbefore bpath.a cutafter bpath.b;
enddef;

setuptreemethods("circleit", "drawboxed", "drawcenteredge");

% source:piecetree:rotate left (before)
beginfig(1);
  tree.root(btex $N_2$ etex)
    (leaf(btex $N_1$ etex),
     tree(btex $N_4$ etex)
       (leaf(btex $N_3$ etex),
        leaf(btex $N_5$ etex)));
  drawtrees(root);
endfig;


% source:piecetree:rotate left (after)
beginfig(2);
  tree.root(btex $N_4$ etex)
    (tree(btex $N_2$ etex)
       (leaf(btex $N_1$ etex),
        leaf(btex $N_3$ etex)),
       (leaf(btex $N_5$ etex)));
  drawtrees(root);
endfig;


% source:piecetree:rotate right (before)
beginfig(3);
  tree.root(btex $N_4$ etex)
    (tree(btex $N_2$ etex)
       (leaf(btex $N_1$ etex),
        leaf(btex $N_3$ etex)),
       (leaf(btex $N_5$ etex)));
  drawtrees(root);
endfig;


% source:piecetree:rotate right (after)
beginfig(4);
  tree.root(btex $N_2$ etex)
    (leaf(btex $N_1$ etex),
     tree(btex $N_4$ etex)
       (leaf(btex $N_3$ etex),
        leaf(btex $N_5$ etex)));
  drawtrees(root);
endfig;


end.
