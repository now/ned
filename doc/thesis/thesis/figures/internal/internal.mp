% contents: Buffering strategies explained.
%
% Copyright © 2004 Nikolai Weibull <source@pcppopper.org>

input boxes;
input nw-dgraphs;

w = 1.4in;
dh = 1.25in;
sep = .25in;
color seqcolor; seqcolor = .8white;

% internal:array buffering strategy {{{1
beginfig(1);
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.span(btex \strut etex);
  boxit.gap(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.seq(btex \strut etex);

  span.dx = seq.dx = w;
  gap.dx = .2w;

  span.sw = (0, dh);
  seq.sw = origin;

  drawcolorboxed(seqcolor, "drawboxed", span, seq);
  drawboxed(gap);

  drawarrow seq.nw--span.sw dashed evenly;
  drawarrow seq.ne--span.se dashed evenly;

  label.top(btex Span etex, span.n);
  label.rt(btex $w_{\text{mem}}$ etex, gap.e shifted (2sep, 0));
  label.rt(btex $w$ etex, (xpart gap.e + 2sep, ypart seq.e));
endfig;


% internal:gap buffering strategy {{{1
beginfig(2);
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.span1(btex \strut etex);
  boxit.gap(btex \strut etex);
  boxit.span2(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.seq(btex \strut etex);

  span1.dx + span2.dx = seq.dx = w;
  span1.dx = .6seq.dx;
  gap.dx = .5in;

  span1.sw = (0, dh);
  seq.sw = (gap.dx, 0);

  drawcolorboxed(seqcolor, "drawboxed", span1, span2, seq);
  drawboxed(gap);

  z1 = .6[seq.nw,seq.ne];

  drawarrow seq.nw--span1.sw dashed evenly;
  drawarrow z1--span1.se dashed evenly;
  drawarrow z1--span2.sw dashed evenly;
  drawarrow seq.ne--span2.se dashed evenly;

  label.top(btex Span etex, span1.n);
  label.top(btex Gap etex, gap.n);
  label.top(btex Span etex, span2.n);
  label.rt(btex $w_{\text{mem}}$ etex, span2.e shifted (2sep, 0));
  label.rt(btex $w$ etex, (xpart span2.e + 2sep, ypart seq.e));
endfig;


% internal:line span buffering strategy {{{1
beginfig(3);
  boxit.span1(btex \strut etex);
  boxit.span2(btex \strut etex);
  boxit.span3(btex \strut etex);
  boxit.span4(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.seq(btex \strut etex);

  span1.dx + span2.dx + span3.dx + span4.dx = seq.dx = w;
  span1.dx = .2seq.dx;
  span2.dx = .3seq.dx;
  span3.dx = .4seq.dx;

  span1.sw = (0, dh);
  span2.sw = span1.se shifted (sep, 0);
  span3.sw = span2.se shifted (sep, 0);
  span4.sw = span3.se shifted (sep, 0);
  seq.sw = (3sep / 2, 0);

  drawcolorboxed(seqcolor, "drawboxed", span1, span2, span3, span4, seq);

  z1 = .2[seq.nw,seq.ne];
  z2 = .5[seq.nw,seq.ne];
  z3 = .9[seq.nw,seq.ne];

  drawarrow seq.nw--span1.sw dashed evenly;
  drawarrow z1--span1.se dashed evenly;
  drawarrow z1--span2.sw dashed evenly;
  drawarrow z2--span2.se dashed evenly;
  drawarrow z2--span3.sw dashed evenly;
  drawarrow z3--span3.se dashed evenly;
  drawarrow z3--span4.sw dashed evenly;
  drawarrow seq.ne--span4.se dashed evenly;

  drawarrow span1.e--span2.w;
  drawarrow span2.e--span3.w;
  drawarrow span3.e--span4.w;

  label.top(btex Line 1 etex, span1.n);
  label.top(btex Line 2 etex, span2.n);
  label.top(btex Line 3 etex, span3.n);
  label.top(btex Line 4 etex, span4.n);
  label.rt(btex $w_{\text{mem}}$ etex, span4.e shifted (2sep, 0));
  label.rt(btex $w$ etex, (xpart span4.e + 2sep, ypart seq.e));
endfig;  


% internal:fixed size buffering strategy {{{1
beginfig(4);
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.span1(btex \strut etex);
  boxit.gap1(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.span2(btex \strut etex);
  boxit.gap2(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.span3(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.span4(btex \strut etex);
  boxit.gap4(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.seq(btex \strut etex);

  span1.dx + span2.dx + span3.dx + span4.dx = seq.dx = w;
  span1.dx = .275seq.dx;
  span2.dx = .175seq.dx;
  span3.dx = .325seq.dx;
  span1.dx + gap1.dx = span2.dx + gap2.dx = span3.dx = span4.dx + gap4.dx;

  span1.sw = (0, dh);
  span2.sw = gap1.se shifted (sep, 0);
  span3.sw = gap2.se shifted (sep, 0);
  span4.sw = span3.se shifted (sep, 0);
  seq.sw = (gap1.dx + gap2.dx + gap4.dx + 3sep / 2, 0);

  drawcolorboxed(seqcolor, "drawboxed", span1, span2, span3, span4, seq);
  drawboxed(gap1, gap2, gap4);

  z1 = .275[seq.nw,seq.ne];
  z2 = .450[seq.nw,seq.ne];
  z3 = .775[seq.nw,seq.ne];

  drawarrow seq.nw--span1.sw dashed evenly;
  drawarrow z1--span1.se dashed evenly;
  drawarrow z1--span2.sw dashed evenly;
  drawarrow z2--span2.se dashed evenly;
  drawarrow z2--span3.sw dashed evenly;
  drawarrow z3--span3.se dashed evenly;
  drawarrow z3--span4.sw dashed evenly;
  drawarrow seq.ne--span4.se dashed evenly;

  label.top(btex Spans and Gaps etex, .5[gap2.ne,span3.nw]);
  label.rt(btex $w_{\text{mem}}$ etex, gap4.e shifted (2sep, 0));
  label.rt(btex $w$ etex, (xpart gap4.e + 2sep, ypart seq.e));
endfig;


% internal:piece table buffering strategy {{{1
beginfig(5);
  boxit.orig(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.span1(btex \strut etex);
  boxit.gap1(btex \strut etex);
  boxit.span2(btex \strut etex);
  boxit.gap2(btex \strut etex);
  boxit.span3(btex \strut etex);
  boxit.gap3(btex \strut etex);

  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.seq(btex \strut etex);

  seq.dx = w;
  orig.dx = .7seq.dx;
  span1.dx = .175seq.dx;
  span2.dx = .075seq.dx;
  span3.dx = .2seq.dx;
  gap1.dx = .01seq.dx;
  gap2.dx = .05seq.dx;
  gap3.dx = .015seq.dx;

  orig.sw = (0, dh);
  span1.sw = orig.se shifted (2sep, 0);
  seq.sw = ((orig.dx + span1.dx + span2.dx + span3.dx - seq.dx) + gap1.dx + gap2.dx + gap3.dx + sep, 0);

  drawcolorboxed(seqcolor, "drawboxed", orig, span1, span2, span3, seq);
  drawboxed(gap1, gap2, gap3);

  z1 = .4[seq.nw,seq.ne];
  z2 = .575[seq.nw,seq.ne];
  z3 = .725[seq.nw,seq.ne];
  z4 = .925[seq.nw,seq.ne];

  drawarrow seq.nw--orig.sw dashed evenly;
  drawarrow z1--(orig.sw + (z1 - seq.nw)) dashed evenly;
  drawarrow z1--span1.sw dashed evenly;
  drawarrow z2--span1.se dashed evenly;
  drawarrow z2--(orig.se - (z3 - z2)) dashed evenly;
  drawarrow z3--orig.se dashed evenly;
  drawarrow z3--span3.sw dashed evenly;
  drawarrow z4--span3.se dashed evenly;
  drawarrow z4--span2.sw dashed evenly;
  drawarrow seq.ne--span2.se dashed evenly;

  label.top(btex $w_{\text{original}}$ etex, orig.n);
  label.top(btex $w_{\text{added}}$ etex, .5[span1.nw, gap3.ne]);
  label.rt(btex $w_{\text{mem}}$ etex, gap3.e shifted (2sep, 0));
  label.rt(btex $w$ etex, (xpart gap3.e + 2sep, ypart seq.e));
endfig;


% internal:piece table example-original {{{1
beginfig(6);
  % TODO: fix this value
  interim defaultdx := 7bp;
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.s1(btex \strut\type{I} etex);
  boxit.s2(btex \strut\type{ } etex);
  boxit.s3(btex \strut\type{w} etex);
  boxit.s4(btex \strut\type{a} etex);
  boxit.s5(btex \strut\type{n} etex);
  boxit.s6(btex \strut\type{t} etex);
  boxit.s7(btex \strut\type{ } etex);
  boxit.s8(btex \strut\type{t} etex);
  boxit.s9(btex \strut\type{o} etex);
  boxit.s10(btex \strut\type{ } etex);
  boxit.s11(btex \strut\type{b} etex);
  boxit.s12(btex \strut\type{e} etex);
  boxit.s13(btex \strut\type{l} etex);
  boxit.s14(btex \strut\type{i} etex);
  boxit.s15(btex \strut\type{e} etex);
  boxit.s16(btex \strut\type{v} etex);
  boxit.s17(btex \strut\type{e} etex);

  drawboxed(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17);

  label.top(btex \ix 0 etex, s1.n);
  label.top(btex \ix 1 etex, s2.n);
  label.top(btex \ix 2 etex, s3.n);
  label.top(btex \ix 3 etex, s4.n);
  label.top(btex \ix 4 etex, s5.n);
  label.top(btex \ix 5 etex, s6.n);
  label.top(btex \ix 6 etex, s7.n);
  label.top(btex \ix 7 etex, s8.n);
  label.top(btex \ix 8 etex, s9.n);
  label.top(btex \ix 9 etex, s10.n);
  label.top(btex \ix 10 etex, s11.n);
  label.top(btex \ix 11 etex, s12.n);
  label.top(btex \ix 12 etex, s13.n);
  label.top(btex \ix 13 etex, s14.n);
  label.top(btex \ix 14 etex, s15.n);
  label.top(btex \ix 15 etex, s16.n);
  label.top(btex \ix 16 etex, s17.n);
endfig;


% internal:piece table example-added in miracles {{{1
beginfig(7);
  interim defaultdx := 7bp;
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.s1(btex \strut\type{ } etex);
  boxit.s2(btex \strut\type{i} etex);
  boxit.s3(btex \strut\type{n} etex);
  boxit.s4(btex \strut\type{ } etex);
  boxit.s5(btex \strut\type{m} etex);
  boxit.s6(btex \strut\type{i} etex);
  boxit.s7(btex \strut\type{r} etex);
  boxit.s8(btex \strut\type{a} etex);
  boxit.s9(btex \strut\type{c} etex);
  boxit.s10(btex \strut\type{l} etex);
  boxit.s11(btex \strut\type{e} etex);
  boxit.s12(btex \strut\type{s} etex);

  drawboxed(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12);

  label.top(btex \ix 0 etex, s1.n);
  label.top(btex \ix 1 etex, s2.n);
  label.top(btex \ix 2 etex, s3.n);
  label.top(btex \ix 3 etex, s4.n);
  label.top(btex \ix 4 etex, s5.n);
  label.top(btex \ix 5 etex, s6.n);
  label.top(btex \ix 6 etex, s7.n);
  label.top(btex \ix 7 etex, s8.n);
  label.top(btex \ix 8 etex, s9.n);
  label.top(btex \ix 9 etex, s10.n);
  label.top(btex \ix 10 etex, s11.n);
  label.top(btex \ix 11 etex, s12.n);
endfig;


% internal:piece table example-added don't {{{1
beginfig(8);
  interim defaultdx := 7bp;
  boxjoin(a.se = b.sw; a.ne = b.nw);
  boxit.s1(btex \strut\type{ } etex);
  boxit.s2(btex \strut\type{i} etex);
  boxit.s3(btex \strut\type{n} etex);
  boxit.s4(btex \strut\type{ } etex);
  boxit.s5(btex \strut\type{m} etex);
  boxit.s6(btex \strut\type{i} etex);
  boxit.s7(btex \strut\type{r} etex);
  boxit.s8(btex \strut\type{a} etex);
  boxit.s9(btex \strut\type{c} etex);
  boxit.s10(btex \strut\type{l} etex);
  boxit.s11(btex \strut\type{e} etex);
  boxit.s12(btex \strut\type{s} etex);
  boxit.s13(btex \strut\type{d} etex);
  boxit.s14(btex \strut\type{o} etex);
  boxit.s15(btex \strut\type{n} etex);
  boxit.s16(btex \strut\type{'} etex);
  boxit.s17(btex \strut\type{t} etex);
  boxit.s18(btex \strut\type{ } etex);

  drawboxed(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18);

  label.top(btex \ix 0 etex, s1.n);
  label.top(btex \ix 1 etex, s2.n);
  label.top(btex \ix 2 etex, s3.n);
  label.top(btex \ix 3 etex, s4.n);
  label.top(btex \ix 4 etex, s5.n);
  label.top(btex \ix 5 etex, s6.n);
  label.top(btex \ix 6 etex, s7.n);
  label.top(btex \ix 7 etex, s8.n);
  label.top(btex \ix 8 etex, s9.n);
  label.top(btex \ix 9 etex, s10.n);
  label.top(btex \ix 10 etex, s11.n);
  label.top(btex \ix 11 etex, s12.n);
  label.top(btex \ix 12 etex, s13.n);
  label.top(btex \ix 13 etex, s14.n);
  label.top(btex \ix 14 etex, s15.n);
  label.top(btex \ix 15 etex, s16.n);
  label.top(btex \ix 16 etex, s17.n);
  label.top(btex \ix 17 etex, s18.n);
endfig;


% }}}1

end.
