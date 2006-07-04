% contents: Automatons used in the chapter on construction.
%
% Copyright © 2004 Nikolai Weibull <source@pcppopper.org>

input boxes;
input mp-text;
input nw-dgraphs;
input nw-trees;
input ../../../include/regex-syntax-trees;

% construction:thompson atoms-epsilon {{{1
beginfig(1);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex \strut$i$ etex);
  circleit.b(btex \strut$f$ etex);
  drawboxed(a);
  drawdoubleboxed(b);
  startarrow(a);
  label.top(btex $\epsilon$ etex, centerarrow(a,b));
endfig;


% construction:thompson atoms-symbol {{{1
beginfig(2);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex \strut$i$ etex);
  circleit.b(btex \strut$f$ etex);
  drawboxed(a);
  drawdoubleboxed(b);
  startarrow(a);
  label.top(btex $a$ etex, centerarrow(a,b));
endfig;


% construction:thompson alternation {{{1
beginfig(3);
  boxjoin(b.c = a.c + (6cm,0));
  circleit.a(btex \strut$i$ etex);
  circleit.f(btex \strut$f$ etex);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.b(btex $i_{r_1}$ etex);
  circleit.c(btex $f_{r_1}$ etex);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.d(btex $i_{r_2}$ etex);
  circleit.e(btex $f_{r_2}$ etex);
  a.c = origin;
  b.c = (2cm,1.5cm);
  d.c = (2cm,-1.5cm);
  drawboxed(a,b,c,d,e);
  drawdoubleboxed(f);
  startarrow(a);
  encircletext(b,c)(4cm)(btex $A_{r_1}$ etex);
  encircletext(d,e)(4cm)(btex $A_{r_2}$ etex);
  label.ulft(btex $\epsilon$ etex, centerarrow(a,b));
  label.urt(btex $\epsilon$ etex, centerarrow(a,d));
  label.urt(btex $\epsilon$ etex, centerarrow(c,f));
  label.ulft(btex $\epsilon$ etex, centerarrow(e,f));
endfig;


% construction:thompson concatenation {{{1
beginfig(4);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $i_{r_1}$ etex);
  circleit.b(btex etex);
  circleit.c(btex $f_{r_2}$ etex);
  drawboxed(a,b);
  drawdoubleboxed(c);
  encircletext(a,b)(3.2cm)(btex $A_{r_1}$ etex);
  encircletext(b,c)(3.2cm)(btex $A_{r_2}$ etex);
  startarrow(a);
endfig;


% construction:thompson closure {{{1
beginfig(5);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex \strut$i$ etex);
  circleit.b(btex $i_{r_1}$ etex);
  circleit.c(btex $f_{r_1}$ etex);
  circleit.d(btex \strut$f$ etex);
  drawboxed(a,b,c);
  drawdoubleboxed(d);
  startarrow(a);
  encircletext(b,c)(3.5cm)(btex $A_{r_1}$ etex);
  label.top(btex $\epsilon$ etex, centerarrow(a,b));
  label.bot(btex $\epsilon$ etex, cutarrow(a,d) a.c{dir 300}..{dir -300}d.c);
  label.top(btex $\epsilon$ etex, cutarrow(c,b) c.c{dir 120}..(b.c + (c.c - b.c) / 2 + (0,1.5cm))..{dir -120}b.c);
  label.top(btex $\epsilon$ etex, centerarrow(c,d));
endfig;


% construction:thompson example ast {{{1
beginfig(6);
  label(textext(""), origin);
  CLOSURE.root(
    ALT(
      CONS(
        ZERO,
        CONS(
          CLOSURE(
            ONE),
          ZERO)),
      ONE));
  drawtrees(root);
endfig;


% construction:thompson example 0-1 {{{1
beginfig(7);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  drawboxed(a);
  drawdoubleboxed(b);
  startarrow(a);
  label.top(btex $0$ etex, centerarrow(a,b));
  label.lrt(btex . etex, b.e);
endfig;


% construction:thompson example 1-1 {{{1
beginfig(8);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_2$ etex);
  circleit.b(btex $s_3$ etex);
  drawboxed(a);
  drawdoubleboxed(b);
  startarrow(a);
  label.top(btex $1$ etex, centerarrow(a,b));
  label.lrt(btex . etex, b.e);
endfig;


% construction:thompson example 1* {{{1
beginfig(9);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_4$ etex);
  circleit.b(btex $s_2$ etex);
  circleit.c(btex $s_3$ etex);
  circleit.d(btex $s_5$ etex);
  drawboxed(a,b,c);
  drawdoubleboxed(d);
  startarrow(a);
  label.top(btex $\epsilon$ etex, centerarrow(a,b));
  label.bot(btex $\epsilon$ etex, cutarrow(a,d) a.c{dir 300}..{dir -300}d.c);
  label.top(btex $1$ etex, centerarrow(b,c));
  label.top(btex $\epsilon$ etex, cutarrow(c,b) c.c{dir 120}..(b.c + (c.c - b.c) / 2 + (0,1.5cm))..{dir -120}b.c);
  label.top(btex $\epsilon$ etex, centerarrow(c,d));
  label.lrt(btex . etex, d.e);
endfig;


% construction:thompson example 0-2 {{{1
beginfig(10);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_6$ etex);
  circleit.b(btex $s_7$ etex);
  drawboxed(a);
  drawdoubleboxed(b);
  startarrow(a);
  label.top(btex $0$ etex, centerarrow(a,b));
  label.lrt(btex . etex, b.e);
endfig;


% construction:thompson example 1*0 {{{1
beginfig(11);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_4$ etex);
  circleit.b(btex $s_2$ etex);
  circleit.c(btex $s_3$ etex);
  circleit.d(btex $s_5$ etex);
  circleit.e(btex $s_7$ etex);
  drawboxed(a,b,c,d);
  drawdoubleboxed(e);
  startarrow(a);
  label.top(btex $\epsilon$ etex, centerarrow(a,b));
  label.bot(btex $\epsilon$ etex, cutarrow(a,d) a.c{dir 300}..{dir -300}d.c);
  label.top(btex $1$ etex, centerarrow(b,c));
  label.top(btex $\epsilon$ etex, cutarrow(c,b) c.c{dir 120}..(b.c + (c.c - b.c) / 2 + (0,1.5cm))..{dir -120}b.c);
  label.top(btex $\epsilon$ etex, centerarrow(c,d));
  label.top(btex $0$ etex, centerarrow(d,e));
  label.lrt(btex , etex, e.e);
endfig;

  
% construction:thompson example 01*0 {{{1
beginfig(12);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_4$ etex);
  circleit.c(btex $s_2$ etex);
  circleit.d(btex $s_3$ etex);
  circleit.e(btex $s_5$ etex);
  circleit.f(btex $s_7$ etex);
  drawboxed(a,b,c,d,e);
  drawdoubleboxed(f);
  startarrow(a);
  label.top(btex $0$ etex, centerarrow(a,b));
  label.top(btex $\epsilon$ etex, centerarrow(b,c));
  label.bot(btex $\epsilon$ etex, cutarrow(b,e) b.c{dir 300}..{dir -300}e.c);
  label.top(btex $1$ etex, centerarrow(c,d));
  label.top(btex $\epsilon$ etex, cutarrow(d,c) d.c{dir 120}..(c.c + (d.c - c.c) / 2 + (0,1.5cm))..{dir -120}c.c);
  label.top(btex $\epsilon$ etex, centerarrow(d,e));
  label.top(btex $0$ etex, centerarrow(e,f));
  label.lrt(btex . etex, f.e);
endfig;


% construction:thompson example 1-2 {{{1
beginfig(13);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_8$ etex);
  circleit.b(btex $s_9$ etex);
  drawboxed(a);
  drawdoubleboxed(b);
  startarrow(a);
  label.top(btex $1$ etex, centerarrow(a,b));
  label.lrt(btex , etex, b.e);
endfig;


% construction:thompson example 01*0or1 {{{1
beginfig(14);
  boxjoin(b.c = a.c + (7cm,0));
  circleit.a(btex $s_{10}$ etex);
  circleit.j(btex $s_{11}$ etex);
  boxjoin(b.c = a.c + (1cm,0cm));
  circleit.b(btex $s_0$ etex);
  circleit.c(btex $s_4$ etex);
  circleit.d(btex $s_2$ etex);
  circleit.e(btex $s_3$ etex);
  circleit.f(btex $s_5$ etex);
  circleit.g(btex $s_7$ etex);
  boxjoin(b.c = a.c + (3cm,0));
  circleit.h(btex $s_8$ etex);
  circleit.i(btex $s_9$ etex);
  a.c = (0,0);
  b.c = (1cm,1cm);
  h.c = (2cm,-1cm);
  drawboxed(a,b,c,d,e,f,g,h,i);
  drawdoubleboxed(j);
  startarrow(a);
  label.ulft(btex $\epsilon$ etex, centerarrow(a,b));
  label.urt(btex $\epsilon$ etex, centerarrow(a,h));
  label.top(btex $0$ etex, centerarrow(b,c));
  label.top(btex $\epsilon$ etex, centerarrow(c,d));
%  label.top(btex $\epsilon$ etex, cutarrow(j,a) j.c{dir 100}..(6cm,2.5cm)---(1cm,2.5cm)..{dir -100}a.c);
  label.top(btex $1$ etex, centerarrow(d,e));
  label.top(btex $\epsilon$ etex, centerarrow(e,f));
  label.top(btex $\epsilon$ etex, cutarrow(e,d) e.c{dir 120}..(d.c + (e.c - d.c) / 2 + (0,0.75cm))..{dir -120}d.c);
  label.top(btex $0$ etex, centerarrow(f,g));
  label.urt(btex $\epsilon$ etex, centerarrow(g,j));
  label.top(btex $1$ etex, centerarrow(h,i));
  label.ulft(btex $\epsilon$ etex, centerarrow(i,j));
  label.lrt(btex . etex, j.e);
endfig;


% construction:thompson example (01*0or1)* {{{1
beginfig(15);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_{12}$ etex);
  circleit.b(btex $s_{10}$ etex);
  boxjoin(b.c = a.c + (1cm,0cm));
  circleit.c(btex $s_0$ etex);
  circleit.d(btex $s_4$ etex);
  circleit.e(btex $s_2$ etex);
  circleit.f(btex $s_3$ etex);
  circleit.g(btex $s_5$ etex);
  circleit.h(btex $s_7$ etex);
  boxjoin(b.c = a.c + (3cm,0));
  circleit.i(btex $s_8$ etex);
  circleit.j(btex $s_9$ etex);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.k(btex $s_{11}$ etex);
  circleit.l(btex $s_{13}$ etex);
  a.c = (0,0);
  c.c = (3cm,1cm);
  i.c = (4cm,-1cm);
  k.c = (9cm,0);
  drawboxed(a,b,c,d,e,f,g,h,i,j,k);
  drawdoubleboxed(l);
  startarrow(a);
  label.top(btex $\epsilon$ etex, centerarrow(a,b));
  label.ulft(btex $\epsilon$ etex, centerarrow(b,c));
  label.urt(btex $\epsilon$ etex, centerarrow(b,i));
  label.top(btex $0$ etex, centerarrow(c,d));
  label.top(btex $\epsilon$ etex, centerarrow(d,e));
  label.top(btex $\epsilon$ etex, cutarrow(k,b)
    k.c{dir 100}..(8cm,2.5cm)---(3cm,2.5cm)..{dir -100}b.c);
  label.top(btex $1$ etex, centerarrow(e,f));
  label.top(btex $\epsilon$ etex, centerarrow(f,g));
  label.top(btex $\epsilon$ etex, cutarrow(f,e)
    f.c{dir 120}..(e.c + (f.c - e.c) / 2 + (0,0.75cm))..{dir -120}e.c);
  label.top(btex $0$ etex, centerarrow(g,h));
  label.urt(btex $\epsilon$ etex, centerarrow(h,k));
  label.top(btex $1$ etex, centerarrow(i,j));
  label.ulft(btex $\epsilon$ etex, centerarrow(j,k));
  label.top(btex $\epsilon$ etex, centerarrow(k,l));
  label.bot(btex $\epsilon$ etex, cutarrow(a,l)
    a.c{dir 300}..(2cm,-2cm)---(9cm,-2cm)..{dir -300}l.c);
  label.lrt(btex . etex, l.e);
endfig;


% construction:thompson example {{{1
beginfig(16);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  boxjoin(b.c = a.c + (1cm,0cm));
  circleit.c(btex $s_2$ etex);
  circleit.d(btex $s_3$ etex);
  circleit.e(btex $s_4$ etex);
  circleit.f(btex $s_5$ etex);
  circleit.g(btex $s_6$ etex);
  circleit.h(btex $s_7$ etex);
  boxjoin(b.c = a.c + (3cm,0));
  circleit.i(btex $s_8$ etex);
  circleit.j(btex $s_9$ etex);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.k(btex $s_{10}$ etex);
  circleit.l(btex $s_{11}$ etex);
  a.c = (0,0);
  c.c = (3cm,1cm);
  i.c = (4cm,-1cm);
  k.c = (9cm,0);
  drawboxed(a,b,c,d,e,f,g,h,i,j,k);
  drawdoubleboxed(l);
  startarrow(a);
  label.top(btex $\epsilon$ etex, centerarrow(a,b));
  label.ulft(btex $\epsilon$ etex, centerarrow(b,c));
  label.urt(btex $\epsilon$ etex, centerarrow(b,i));
  label.top(btex $0$ etex, centerarrow(c,d));
  label.top(btex $\epsilon$ etex, centerarrow(d,e));
  label.top(btex $\epsilon$ etex, cutarrow(k,b) k.c{dir 100}..(8cm,2.5cm)---(3cm,2.5cm)..{dir -100}b.c);
  label.top(btex $1$ etex, centerarrow(e,f));
  label.top(btex $\epsilon$ etex, centerarrow(f,g));
  label.top(btex $\epsilon$ etex, cutarrow(f,e) f.c{dir 120}..(e.c + (f.c - e.c) / 2 + (0,0.75cm))..{dir -120}e.c);
  label.top(btex $0$ etex, centerarrow(g,h));
  label.urt(btex $\epsilon$ etex, centerarrow(h,k));
  label.top(btex $1$ etex, centerarrow(i,j));
  label.ulft(btex $\epsilon$ etex, centerarrow(j,k));
  label.top(btex $\epsilon$ etex, centerarrow(k,l));
  label.bot(btex $\epsilon$ etex, cutarrow(a,l) a.c{dir 300}..(2cm,-2cm)---(9cm,-2cm)..{dir -300}l.c);
endfig;


% construction:tnfa tag {{{1
beginfig(17);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex \strut$i$ etex);
  circleit.b(btex \strut$f$ etex);
  drawboxed(a);
  drawdoubleboxed(b);
  startarrow(a);
  label.top(btex $\epsilon/t_i$ etex, centerarrow(a,b));
endfig;


% construction:curly tnfa {{{1
vardef fo(expr x) =
  if x mod 4 = 1: -.09cm elseif x mod 4 = 2: 0cm elseif x mod 4 = 3: .09cm else: 0cm fi
enddef;

def curlypath(suffix f)(expr a, b, parts) =
  ( a{curl 0}
    for x = 1 upto parts:
      .. (xpart(a) + x * ((xpart(b) - xpart(a)) / (parts + 1)),f(x))
    endfor .. {curl 0}b )
enddef;

beginfig(18);
  boxjoin(b.w = a.e + (2.35cm,0));
  circleit.a(btex $\phantom{s_i}$ etex);
  circleit.b(btex $\phantom{s_j}$ etex);
  circleit.c(btex $\phantom{s_k}$ etex);
  a.c = origin;
  drawboxed(a,b);
  drawdoubleboxed(c);
  drawarrow curlypath(fo,a.w - (2.35cm,0),a.w,12) dashed evenly;
  label.top(btex $\epsilon/t_x$ etex, cutarrow(a,b) a.c..b.c);
  label.top(btex $P_1$ etex, cutarrow(b,c) curlypath(fo,b.e,c.w,12));
  label.top(btex $P_2$ etex,
            cutarrow(b,a) b.c..(b.c + (.25cm,.95cm))..
            (curlypath(fo,b.w,a.e,13) shifted (0,1cm))..(a.c + (-.25cm,.95cm))..a.c);
endfig;


% construction:worst case tnfa for tagged epsilon-closure {{{1
beginfig(19);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  circleit.c(btex $s_2$ etex);
  circleit.d(btex $s_n$ etex);
  drawboxed(a,b,c,d);
  startarrow(a);

  label.top(btex $\epsilon$ etex, cutarrow(a,b) a.c{dir 45}..{dir -45}b.c);
  label.bot(btex $\epsilon/t_0$ etex, cutarrow(a,b) a.c{dir -45}..{dir 45}b.c);
  label.top(btex $\epsilon$ etex, cutarrow(b,c) b.c{dir 45}..{dir -45}c.c);
  label.bot(btex $\epsilon/t_1$ etex, cutarrow(b,c) b.c{dir -45}..{dir 45}c.c);

  path p,q;
  p = c.c{dir 45}..{dir -45}d.c cutbefore bpath.c cutafter bpath.d;
  q = c.c{dir -45}..{dir 45}d.c cutbefore bpath.c cutafter bpath.d;

  draw p cutafter point .2 along p;
  draw p cutbefore point .2 along p cutafter point .8 along p dashed evenly;
  drawarrow p cutbefore point .8 along p;

  draw q cutafter point .2 along q;
  draw q cutbefore point .2 along q cutafter point .8 along q dashed evenly;
  drawarrow q cutbefore point .8 along q;

  label(btex \strut\cdots etex, point .5 along (c.c..d.c));
  label.bot(btex $\epsilon$ etex,
            cutarrow(d,a) d.c{dir -120}..(.5[c.c,d.c] - (0,1.25cm))---(.5[a.c,b.c] - (0,1.25cm))..{dir 120}a.c);
endfig;


% }}}1

end.
