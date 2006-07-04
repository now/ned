% contents: Figures for the definitions chapter.
%
% Copyright © 2004 Nikolai Weibull <source@pcppopper.org>

input boxes;
input nw-dgraphs;
input ../../../include/colors;

% definitions:relationship {{{1
beginfig(1);
  circleit.regex(btex \bf regular expressions etex);
  circleit.reglang(btex \bf regular languages etex);
  circleit.fa(btex \bf finite automatons etex);

  path p; p = unitcircle rotated 90 scaled 6cm;

  reglang.c = point 0 along p;
  regex.c = point 1/3 along p;
  fa.c = point 2/3 along p;

  reglang.dx = reglang.dy;
  regex.dx = regex.dy;
  fa.dx = fa.dy;

  drawunboxed(regex, reglang, fa);

  z1 = direction 0 along p;
  z2 = direction 1/3 along p;
  z3 = direction 2/3 along p;

  label.llft(btex described by etex, cutarrowp(reglang,regex,.75) reglang.c{z1}..{z2}regex.c);
  label.lrt(btex compile to etex, cutarrowp(regex,fa,.75) regex.c{z2}..{z3}fa.c);
  label.urt(btex recognize etex, cutarrowp(fa,reglang,.6) fa.c{z3}..{z1}reglang.c);
  label.llft(btex accepted by etex, cutarrowp(reglang,fa,.75) reglang.c{-z1}..{-z3}fa.c);
  label.urt(btex decompile to etex, cutarrowp(fa,regex,.75) fa.c{-z3}..{-z2}regex.c);
  label.lrt(btex describe etex, cutarrowp(regex,reglang,.6) regex.c{-z2}..{-z1}reglang.c);
endfig;


% definitions:finite control {{{1
beginfig(2);
  interim defaultdx := 7bp;

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.i1(btex \strut$b$ etex); boxit.i2(btex \strut$a$ etex); boxit.i3(btex \strut$n$ etex);
  boxit.i4(btex \strut$a$ etex); boxit.i5(btex \strut$n$ etex); boxit.i6(btex \strut$a$ etex);


  drawboxed(i1, i2, i3, i4, i5, i6);
  drawcolorboxed((.388235,.541176,.686274), "drawboxed", i3);

  draw thelabel(btex \strut$n$ etex, i3.c) withcolor white;
  label.top(btex input etex, .5[i3.n,i4.n]);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.control(btex $\displaystyle{\matrix{\text{finite}\cr\text{control}\cr}}$ etex);
  control.c = i3.c + (0,-2cm);

  drawboxed(control);

  label.urt(btex cursor etex, cutarrow(control,i3) control.c..i3.c);
endfig;


% definitions:even zeros dfa {{{1
beginfig(3);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  drawboxed(b);
  drawdoubleboxed(a);
  startarrow(a);
  label.ulft(btex $1$ etex, self.a(-.5cm,.5cm));
  label.top(btex $0$ etex, cutarrow(a,b) a.c{dir 45}..{dir -45}b.c);
  label.urt(btex $1$ etex, self.b(.5cm,.5cm));
  label.bot(btex $0$ etex, cutarrow(b,a) b.c{dir -135}..{dir 135}a.c);
endfig;


% definitions:even zeros dfa ex-1 {{{1
beginfig(4);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  drawboxed(b);
  drawcolorboxed(activestatecolor, "drawdoubleboxed", a);
  startarrow(a);
  label.ulft(btex $1$ etex, self.a(-.5cm,.5cm));
  label.top(btex $0$ etex, cutarrow(a,b) a.c{dir 45}..{dir -45}b.c);
  label.urt(btex $1$ etex, self.b(.5cm,.5cm));
  label.bot(btex $0$ etex, cutarrow(b,a) b.c{dir -135}..{dir 135}a.c);
  label.lrt(btex . etex, b.e);
endfig;


% definitions:even zeros dfa ex-2 {{{1
beginfig(5);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  drawdoubleboxed(a);
  drawcolorboxed(activestatecolor, "drawboxed", b);
  startarrow(a);
  label.ulft(btex $1$ etex, self.a(-.5cm,.5cm));
  draw thelabel.top(btex $0$ etex, cutcolorarrow(activetranscolor,a,b) a.c{dir 45}..{dir -45}b.c) withcolor activetranscolor;
  label.urt(btex $1$ etex, self.b(.5cm,.5cm));
  label.bot(btex $0$ etex, cutarrow(b,a) b.c{dir -135}..{dir 135}a.c);
  label.lrt(btex . etex, b.e);
endfig;


% definitions:even zeros dfa ex-3 {{{1
beginfig(6);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  drawdoubleboxed(a);
  drawcolorboxed(activestatecolor, "drawboxed", b);
  startarrow(a);
  label.ulft(btex $1$ etex, self.a(-.5cm,.5cm));
  label.top(btex $0$ etex, cutarrow(a,b) a.c{dir 45}..{dir -45}b.c);
  draw thelabel.urt(btex $1$ etex, colorself.b(activetranscolor)(.5cm,.5cm)) withcolor activetranscolor;
  label.bot(btex $0$ etex, cutarrow(b,a) b.c{dir -135}..{dir 135}a.c);
  label.lrt(btex . etex, b.e);
endfig;


% definitions:even zeros dfa ex-4 {{{1
beginfig(7);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  drawboxed(b);
  drawcolorboxed(activestatecolor, "drawdoubleboxed", a);
  startarrow(a);
  label.ulft(btex $1$ etex, self.a(-.5cm,.5cm));
  label.top(btex $0$ etex, cutarrow(a,b) a.c{dir 45}..{dir -45}b.c);
  label.urt(btex $1$ etex, self.b(.5cm,.5cm));
  draw thelabel.bot(btex $0$ etex, cutcolorarrow(activetranscolor,b,a) b.c{dir -135}..{dir 135}a.c) withcolor activetranscolor;
  label.lrt(btex . etex, b.e);
endfig;


% definitions:even zeros dfa ex-5 {{{1
beginfig(8);
  boxjoin(b.c = a.c + (2cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  drawboxed(b);
  drawcolorboxed(activestatecolor, "drawdoubleboxed", a);
  startarrow(a);
  draw thelabel.ulft(btex $1$ etex, colorself.a(activetranscolor)(-.5cm,.5cm)) withcolor activetranscolor;
  label.top(btex $0$ etex, cutarrow(a,b) a.c{dir 45}..{dir -45}b.c);
  label.urt(btex $1$ etex, self.b(.5cm,.5cm));
  label.bot(btex $0$ etex, cutarrow(b,a) b.c{dir -135}..{dir 135}a.c);
  label.lrt(btex . etex, b.e);
endfig;


% definitions:epsilon decimal numbers nfa {{{1
beginfig(9);
  boxjoin(b.c = a.c + (2.5cm,0));
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  circleit.c(btex $s_2$ etex);
  circleit.e(btex $s_4$ etex);
  circleit.h(btex $s_7$ etex);
  boxjoin(b.c = a.c + (2.5cm,0));
  circleit.d(btex $s_3$ etex);
  circleit.f(btex $s_5$ etex);
  circleit.g(btex $s_6$ etex);
  a.c = origin;
  d.c = (5cm,-2.5cm);
  drawboxed(a,b,c,d,e,f,g);
  drawdoubleboxed(h);
  startarrow(a);
  label.top(btex $\epsilon, +, -$ etex, centerarrow(a,b));
  label.top(btex $0, 1, \dots, 9$ etex, self.b(0,.7cm));
  label.top(btex . etex, centerarrow(b,c));
  label.llft(btex $0, 1, \dots, 9$ etex, centerarrow(b,d));
  label.top(btex $0, 1, \dots, 9$ etex, centerarrow(c,e));
  label.lrt(btex . etex, centerarrow(d,e));
  label.top(btex $0, 1, \dots, 9$ etex, self.e(0,.7cm));
  label.top(btex $\epsilon$ etex, centerarrow(e,h));
  label.lft(btex e,E etex, centerarrow(e,f));
  label.bot(btex $\epsilon, +, -$ etex, centerarrow(f,g));
  label.bot(btex $0, 1, \dots, 9$ etex, self.g(0,-.7cm));
  label.rt(btex $0, 1, \dots, 9$ etex, centerarrow(g,h));
endfig;


% definitions:simple tnfa {{{1
beginfig(10);
  circleit.a(btex $s_0$ etex);
  circleit.b(btex $s_1$ etex);
  circleit.c(btex $s_2$ etex);
  circleit.d(btex $s_3$ etex);
  a.c = origin;
  b.c = (2cm,1cm);
  c.c = (2cm,-1cm);
  d.c = (4cm,0);
  drawboxed(a,b,c);
  drawdoubleboxed(d);
  startarrow(a);
  label.ulft(btex $a/t_0$ etex, cutarrow(a,b) a.c{dir 60}..{dir 0}b.c);
  label.urt(btex $b$ etex, cutarrow(b,d) b.c{dir 0}..{dir -60}d.c);
  label.llft(btex $a/t_1$ etex, cutarrow(a,c) a.c{dir -60}..{dir 0}c.c);
  label.lrt(btex $b$ etex, cutarrow(c,d) c.c{dir 0}..{dir 60}d.c);
endfig;


% definitions:ascii character set lookup {{{1
beginfig(11);
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.i0(btex $0$ etex); boxit.i1(btex $0$ etex);
  boxit.i2(btex $1$ etex); boxit.i3(btex $0$ etex);
  boxit.i4(btex $0$ etex); boxit.i5(btex $0$ etex);
  boxit.i6(btex $0$ etex); boxit.i7(btex $1$ etex);
  drawboxed(i0, i1, i2, i3, i4, i5, i6, i7);
  label.lft(btex input ($65$): etex, i0.w);
  label.top(btex byte etex, i3.ne);
  interim defaultdy := 7bp;
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.a0(btex $\dots$ etex); boxit.a1(btex \type{ ? } etex);
  boxit.a2(btex \type{ @ } etex); boxit.a3(btex \type{ A } etex);
  boxit.a4(btex \type{ B } etex); boxit.a5(btex \type{ C } etex);
  boxit.a6(btex $\dots$ etex);
  a0.n = i0.s - (0bp, 40bp);
  drawunboxed(a0);
  drawboxed(a1, a2, a3, a4, a5);
  drawunboxed(a6);
  label.lft(btex \cap{ASCII} table: etex, a0.w);
  label.bot(btex $63$ etex, a1.s);
  label.bot(btex $64$ etex, a2.s);
  label.bot(btex $65$ etex, a3.s);
  label.bot(btex $66$ etex, a4.s);
  label.bot(btex $67$ etex, a5.s);
  drawarrow i7.e...(i7.e + (25bp, -20bp))...(a3.n + (0bp, 20bp))...a3.n;
endfig;


% }}}1

end.
