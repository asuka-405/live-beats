/*
 * draw_figure.cpp - Derived figure classes for drawing notes and stuff.
 * Copyright (C) 2002 Aaron Nemwan
 * e-mail: aaron@meet-the-newmans.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

;
/*
   Some of the drawing routines in this file are based on postscript code
   taken from various programs written by several authors.  Among those of
   whom I am aware are  Michael Methfessel and James Allwright.
*/

#include "iabc/draw_figure.h"

namespace iabc
{
;
#define staff_height 500
#define staff_width ((int)(72.0*100.0*7.2))
// Note that not all of these are used; Some of these guys
// must be scaled in very specific ways, so we derive them
// into their own classes.  This is true of stems, for example.
void
staff_figure::create_poly(polygon& poly)
{
    global_figure_event_log.log_event(figure_event_data::staff_create,(unsigned long)this);
    poly.moveto_relative(point(0,0));
    poly.lineto_relative(point(staff_width,0));
    poly.outline_poly();
    poly.moveto_relative(point(-staff_width,-staff_height));
    poly.lineto_relative(point(staff_width,0));
    poly.outline_poly();
    poly.moveto_relative(point(-staff_width,-staff_height));
    poly.lineto_relative(point(staff_width,0));
    poly.outline_poly();
    poly.moveto_relative(point(-staff_width,-staff_height));
    poly.lineto_relative(point(staff_width,0));
    poly.outline_poly();
    poly.moveto_relative(point(-staff_width,-staff_height));
    poly.lineto_relative(point(staff_width,0));
    poly.outline_poly();
    poly.moveto_relative(point(-staff_width,-staff_height));
    poly.outline_poly();
}

staff_figure::~staff_figure()
{
}

void
ledger_line::create_poly(polygon& poly)
{
    poly.moveto_relative(point(-500,0));
    poly.lineto_relative(point(1000,0));
    poly.outline_poly();
}

void
gclef::create_poly(polygon& poly)
{
    global_figure_event_log.log_event(figure_event_data::gclef_create,(unsigned long)this);
    poly.moveto_relative(point(0,0));
    // fprintf(f,"  -2.22 5.91 rmoveto\n");
    // fprintf(f,"  -0.74 -1.11 rlineto\n");
    poly.lineto_relative(point(-74, -111));
    // fprintf(f,"  -2.22 2.22 -0.74 8.12 3.69 8.12 rcurveto\n");
    poly.curveto_relative(point(-222, 222),
                               point(-74,812),
                               point( 369, 812));
    // fprintf(f,"  2.22 0.74 7.02 -1.85 7.02 -6.65 rcurveto\n");
    poly.curveto_relative(point(222, 74 ),
                               point(702 ,-185),
                               point( 702, -665));
    // fprintf(f,"  0.00 -4.43 -4.06 -6.65 -7.75 -6.65 rcurveto\n");
    poly.curveto_relative(point(000, -443),
                               point(-406, -665),
                               point( -775, -665));

    //fprintf(f,"  -4.43 0.00 -8.49 2.22 -8.49 8.49 rcurveto\n");
    poly.curveto_relative(point(-443, 000),
                               point(-849 ,222),
                               point( -849, 849));
    // fprintf(f,"  0.00 2.58 0.37 5.54 5.91 9.97 rcurveto\n");
    poly.curveto_relative(point(000 ,258),
                               point(37, 554),
                               point( 591, 997));
    // fprintf(f,"  6.28 4.43 6.28 7.02 6.28 8.86 rcurveto\n");
    poly.curveto_relative(point(628, 443),
                               point(628, 702),
                               point(628, 886));
    // fprintf(f,"  0.00 1.85 -0.37 3.32 -1.11 3.32 rcurveto\n");
    poly.curveto_relative(point(000 ,185),
                               point(-37, 332),
                               point(-111, 332));
    // fprintf(f,"  -1.85 -1.48 -3.32 -5.17 -3.32 -7.38 rcurveto\n");
    poly.curveto_relative(point(-185 ,-148),
                               point(-332, -517),
                               point(-332, -738));
    // fprintf(f,"  0.00 -13.66 4.43 -16.25 4.80 -25.85 rcurveto\n");
    poly.curveto_relative(point(0 ,-1366),
                               point(443, -1625),
                               point(480, -2585));
    // fprintf(f,"  0.00 -3.69 -2.22 -5.54 -5.54 -5.54 rcurveto\n");
    poly.curveto_relative(point(0 ,-369),
                               point(-222, -554),
                               point(-554, -554));
    // fprintf(f,"  -2.22 0.00 -4.06 1.85 -4.06 3.69 rcurveto\n");
    poly.curveto_relative(point(-222 ,0),
                               point(-406, 185),
                               point(-406, 369));
    // fprintf(f,"  0.00 1.85 1.11 3.32 2.95 3.32 rcurveto\n");
    poly.curveto_relative(point(0 ,185),
                               point(111, 332),
                               point(295, 332));
    // fprintf(f,"  3.69 0.00 3.69 -5.91 0.37 -4.80 rcurveto\n");
    poly.curveto_relative(point(369 ,0),
                               point(369, -591),
                               point(37, -480));
    // fprintf(f,"  0.37 -2.22 5.54 -1.11 5.54 2.95 rcurveto\n");
    poly.curveto_relative(point(37 ,-222),
                               point(554, -111),
                               point(554, 295));
    // fprintf(f,"  -0.74 12.18 -5.17 14.40 -5.17 28.06 rcurveto\n");
    poly.curveto_relative(point(-74 ,1218),
                               point(-517, 1440),
                               point(-517, 2806));
    // fprintf(f,"  0.00 4.06 1.11 7.38 4.43 8.86 rcurveto\n");
    poly.curveto_relative(point(0 ,406),
                               point(111, 738),
                               point(443, 886));
    // fprintf(f,"  2.22 -1.48 4.06 -4.06 3.69 -6.65 rcurveto\n");
    poly.curveto_relative(point(222 ,-148),
                               point(406, -406),
                               point(369, -665));
    // fprintf(f,"  0.00 -4.06 -2.58 -7.02 -5.91 -10.34 rcurveto\n");
    poly.curveto_relative(point(0 ,-406),
                               point(-258, -702),
                               point(-591, -1034));
    // fprintf(f,"  -2.22 -2.58 -5.91 -4.43 -5.91 -9.60 rcurveto\n");
    poly.curveto_relative(point(-222 ,-258),
                               point(-591, -443),
                               point(-591, -960));
    // fprintf(f,"  0.00 -4.43 2.58 -6.65 5.54 -6.65 rcurveto\n");
    poly.curveto_relative(point(0 ,-443),
                               point(258, -665),
                               point(554, -665));
    // fprintf(f,"  2.95 0.00 5.54 1.85 5.54 4.80 rcurveto\n");
    poly.curveto_relative(point(295 ,0),
                               point(554, 185),
                               point(554, 480));
    // fprintf(f,"  0.00 3.32 -2.95 4.43 -4.80 4.43 rcurveto\n");
    poly.curveto_relative(point(0 ,332),
                               point(-295, 443),
                               point(-480, 443));
    // fprintf(f,"  -2.58 0.00 -4.06 -1.85 -2.95 -3.69 rcurveto\n");
    poly.curveto_relative(point(-258 ,0),
                               point(-406, -185),
                               point(-295, -369));
    poly.fill_poly();
}

gclef::~gclef()
{
}

void
fclef::create_poly(polygon& poly)
{
    global_figure_event_log.log_event(figure_event_data::fclef_create,(unsigned long)this);
    poly.moveto_relative(point(0,0));
//  fprintf(f,"  8.30 4.00 12.80 9.00 12.80 13.00 rcurveto\n");
    poly.curveto_relative(point(830 ,400),
                               point(1280, 900),
                               point(1280, 1300));
    // fprintf(f,"  0.00 4.50 -2.00 7.50 -4.30 7.30 rcurveto\n");
    poly.curveto_relative(point(0 ,450),
                               point(-200, 750),
                               point(-430, 730));
    // fprintf(f,"  -1.00 0.20 -4.20 0.20 -5.70 -3.80 rcurveto\n");
    poly.curveto_relative(point(-100 ,20),
                               point(-420, 20),
                               point(-570, -380));
    // fprintf(f,"  1.50 0.50 3.50 0.00 3.50 -1.50 rcurveto\n");
    poly.curveto_relative(point(150 ,50),
                               point(350, 0),
                               point(350, -150));
    // fprintf(f,"  0.00 -1.00 -0.50 -2.00 -2.00 -2.00 rcurveto\n");
    poly.curveto_relative(point(0 ,-100),
                               point(-50, -200),
                               point(-200, -200));
    // fprintf(f,"  -1.00 0.00 -2.00 0.90 -2.00 2.50 rcurveto\n");
    poly.curveto_relative(point(-100 ,0),
                               point(-200, 90),
                               point(-200, 250));
    // fprintf(f,"  0.00 2.50 2.10 5.50 6.00 5.50 rcurveto\n");
    poly.curveto_relative(point(0 ,250),
                               point(210, 550),
                               point(600, 550));
    // fprintf(f,"  4.00 0.00 7.50 -2.50 7.50 -7.50 rcurveto\n");
    poly.curveto_relative(point(400 ,0),
                               point(750, -250),
                               point(750, -750));
    // fprintf(f,"  0.00 -5.50 -6.50 -11.00 -15.50 -13.70 rcurveto\n");
    poly.curveto_relative(point(0 ,-550),
                               point(-650, -1100),
                               point(-1550, -1370));
    // fprintf(f,"  8.30 4.00 12.80 9.00 12.80 13.00 rcurveto\n");
    poly.curveto_relative(point(830 ,400),
                               point(1280, 900),
                               point(1280, 1300));
    // fprintf(f,"  0.00 4.50 -2.00 7.50 -4.30 7.30 rcurveto\n");
    poly.curveto_relative(point(0 ,450),
                               point(-200, 750),
                               point(-430, 730));
    // fprintf(f,"  -1.00 0.20 -4.20 0.20 -5.70 -3.80 rcurveto\n");
    poly.curveto_relative(point(-100 ,20),
                               point(-420, 20),
                               point(-570, -380));
    // fprintf(f,"  1.50 0.50 3.50 0.00 3.50 -1.50 rcurveto\n");
    poly.curveto_relative(point(150,50),
                               point(350,0),
                               point(350,-150));
    // fprintf(f,"  0.00 -1.00 -0.50 -2.00 -2.00 -2.00 rcurveto\n");
    poly.curveto_relative(point(0,-100),
                               point(-50, -200),
                               point(-200, -200));
    // fprintf(f,"  -1.00 0.00 -2.00 0.90 -2.00 2.50 rcurveto\n");
    poly.curveto_relative(point(-100,0),
                               point(-200, 90),
                               point(-200, 250));
    // fprintf(f,"  0.00 2.50 2.10 5.50 6.00 5.50 rcurveto\n");
    poly.curveto_relative(point(0 ,250),
                               point(210, 550),
                               point(600, 550));
    // fprintf(f,"  4.00 0.00 7.50 -2.50 7.50 -7.50 rcurveto\n");
    poly.curveto_relative(point(400,0),
                               point(750,-250),
                               point(750,-750));
    // fprintf(f,"  0.00 -5.50 -6.50 -11.00 -15.50 -13.70 rcurveto\n");
    poly.curveto_relative(point(0,-550),
                               point(-650, -1100),
                               point(-1550, -1370));
    // fprintf(f,"  16.90 18.20 rmoveto\n");
    poly.fill_poly();

    poly.moveto_relative(point(1690 ,1820));
    // fprintf(f,"  0.00 1.50 2.00 1.50 2.00 0.00 rcurveto\n");
    poly.curveto_relative(point(0 ,150),
                               point(200, 150),
                               point(200, 0));
    // fprintf(f,"  0.00 -1.50 -2.00 -1.50 -2.00 0.00 rcurveto\n");
    poly.curveto_relative(point(0,-150),
                               point(-200,-150),
                               point(-200, 0));
    // fprintf(f,"  0.00 -5.50 rmoveto\n");
    poly.moveto_relative(point(0 ,-550));
    // fprintf(f,"  0.00 1.50 2.00 1.50 2.00 0.00 rcurveto\n");
    poly.curveto_relative(point(0 ,150),
                               point(200, 150),
                               point(200, 0));
    // fprintf(f,"  0.00 -1.50 -2.00 -1.50 -2.00 0.00 rcurveto\n");
    poly.curveto_relative(point(0 ,-150),
                               point(-200, -150),
                               point(-200, 0));
    poly.fill_poly();
}

void
open_head::create_poly(polygon& poly)
{
    global_figure_event_log.log_event(figure_event_data::ohead_create,(unsigned long)this);
    poly.moveto_relative(point(351,192));
    ////fprintf(f,"  3.51 1.92 rmoveto\n");
    // fprintf(f,"  -2.04 3.73 -9.06 -0.10 -7.02 -3.83 rcurveto\n");
    poly.curveto_relative(point(-204,473),
                          point(-906,273),
                          point(-702,-383));
    // fprintf(f,"  2.04 -3.73 9.06 0.10 7.02 3.83 rcurveto\n");
    poly.curveto_relative(point(204,-473),
                          point(906,10),
                          point(702,383));
    // fprintf(f,"  -0.44 -0.24 rmoveto\n");
    poly.moveto_relative(point(-44,-24));
    // fprintf(f,"  0.96 -1.76 -5.19 -5.11 -6.15 -3.35 rcurveto\n");
    poly.curveto_relative(point(96,-276),
                          point(-519,-711),
                          point(-615,-335));
    // fprintf(f,"  -0.96 1.76 5.19 5.11 6.15 3.35 rcurveto\n");
    poly.curveto_relative(point(-96,276),
                          point(519,511),
                          point(615,335));

    poly.fill_poly();

}

void
closed_head::create_poly(polygon& poly)
{
    global_figure_event_log.log_event(figure_event_data::chead_create,(unsigned long)this);
    // poly.set_spline_mode(polygon::tight);

    // abc2ps one
    poly.moveto_relative(point(330,226));
    //fprintf(f,"  3.30 2.26 rmoveto\n");
    //fprintf(f,"  -2.26 3.30 -8.86 -1.22 -6.60 -4.52 rcurveto\n");
    poly.curveto_relative(point(-226,330),
                          point(-886,-122),
                          point(-660,-452));
    // fprintf(f,"  2.26 -3.30 8.86 1.22 6.60 4.52 rcurveto\n");
    poly.curveto_relative(point(226,-330),
                          point(886,122),
                          point(660,452));

#if 0
    // abcm2ps one
    // "    3.5 2.0 rmoveto\n"
    poly.moveto_relative(point(350,200));
    // "    -2.0 3.5 -9.0 -0.5 -7.0 -4.0 rcurveto\n"
    poly.curveto_relative(point(-200,350),
                          point(-900,-50),
                          point(-700,-400));
    //" 2.0 -3.5 9.0 0.5 7.0 4.0 rcurveto fill\n"
    poly.curveto_relative(point(200,-350),
                          point(900,50),
                          point(700,400));
#endif
    poly.fill_poly();
}

void
dot::create_poly(polygon& poly)
{
    poly.moveto_relative(point(0,0));
    poly.arc(0,360,120);
    poly.fill_poly();
}

void
up_stem::create_poly(polygon& the_poly)
{
    global_figure_event_log.log_event(figure_event_data::ustem_create,(unsigned long)this);
    // 1000 is 1/2 the size of the stem, plus an offset for the note head size
    the_poly.moveto_relative(point(330,(STEM_LENGTH / 2) + 548));
    the_poly.lineto_relative(point(0,STEM_LENGTH));
    the_poly.outline_poly();
}

void
down_stem::create_poly(polygon& the_poly)
{
    global_figure_event_log.log_event(figure_event_data::dstem_create,(unsigned long)this);
    // 1000 is 1/2 the size of the stem, plus an offset for the note head size
    the_poly.moveto_relative(point(-330,(STEM_LENGTH / 2) + 322));
    the_poly.lineto_relative(point(0,-STEM_LENGTH));
    the_poly.outline_poly();
}

void
sharp_sign::create_poly(polygon& the_poly)
{
    global_figure_event_log.log_event(figure_event_data::sharp_create,(unsigned long)this);
    the_poly.moveto_relative(point(260,289));
    // fprintf(f,"/sh0 {  %% usage:  x y sh0  - sharp sign\n");
    // fprintf(f,"  moveto\n");
    // fprintf(f,"  2.60 2.89 rmoveto\n");
    // fprintf(f,"  0.00 2.17 rlineto\n");
    the_poly.lineto_relative(point(0,217));
    // fprintf(f,"  -5.20 -1.44 rlineto\n");
    the_poly.lineto_relative(point(-520,-144));
    // fprintf(f,"  0.00 -2.17 rlineto\n");
    the_poly.lineto_relative(point(0,-217));
    // fprintf(f,"  5.20 1.44 rlineto\n");
    the_poly.lineto_relative(point(520,144));
    // fprintf(f,"  0.00 -6.50 rmoveto\n");
    the_poly.fill_poly();

    the_poly.moveto_relative(point(0,-650));
    // fprintf(f,"  0.00 2.17 rlineto\n");
    the_poly.lineto_relative(point(0,217));
    // fprintf(f,"  -5.20 -1.44 rlineto\n");
    the_poly.lineto_relative(point(-520,-144));
    //fprintf(f,"  0.00 -2.17 rlineto\n");
    the_poly.lineto_relative(point(0,-217));
    //fprintf(f,"  5.20 1.44 rlineto\n");
    the_poly.lineto_relative(point(520,144));
    // fprintf(f,"  currentpoint fill moveto\n");
    the_poly.fill_poly();
    // fprintf(f,"  -1.30 -3.61 rmoveto\n");
    the_poly.moveto_relative(point(-130,-361));
    // fprintf(f,"  0.00 15.53 rlineto\n");
    the_poly.lineto_relative(point(0,1553));
    the_poly.outline_poly();
    // fprintf(f,"  currentpoint stroke moveto\n");
    // fprintf(f,"  -2.60 -16.61 rmoveto\n");
    the_poly.moveto_relative(point(-260,-1661));
    // fprintf(f,"  -260 -1661 rlineto\n");
    the_poly.lineto_relative(point(0,1553));
    the_poly.outline_poly();
    // fprintf(f,"  stroke\n");
    // fprintf(f," } bind def\n");

}

void
flat_sign::create_poly(polygon &the_poly)
{
    // fprintf(f,"  -1.60 2.67 rmoveto\n");
    the_poly.moveto_relative(point(-160,267));
    // fprintf(f,"  6.40 3.11 6.40 -3.56 0.00 -6.67 rcurveto\n");
    the_poly.curveto_relative(point(640,311),point(640,-356),point(0,-557));
    // fprintf(f,"  4.80 4.00 4.80 7.56 0.00 5.78 rcurveto\n");
    the_poly.curveto_relative(point(480,400),point(480,756),point(0,578));
    // fprintf(f,"  currentpoint fill moveto\n");
    the_poly.fill_poly();
    // fprintf(f,"  0.00 7.11 rmoveto\n");
    the_poly.moveto_relative(point(0,711));
    // fprintf(f,"  0.00 -12.44 rlineto\n");
    the_poly.lineto_relative(point(0,-1244));
    // fprintf(f,"  stroke\n");
    the_poly.outline_poly();
}

void
natural_sign::create_poly(polygon &the_poly)
{
    // fprintf(f,"  -1.62 -4.33 rmoveto\n");
    the_poly.moveto_relative(point(-162,-433));
    // fprintf(f,"  3.25 0.72 rlineto\n");
    the_poly.lineto_relative(point(325,72));
    // fprintf(f,"  0.00 2.17 rlineto\n");
    the_poly.lineto_relative(point(0,217));
    // fprintf(f,"  -3.25 -0.72 rlineto\n");
    the_poly.lineto_relative(point(-325,-72));
    // fprintf(f,"  0.00 6.50 rlineto\n");
    the_poly.lineto_relative(point(0,650));
    // fprintf(f,"  0.00 -2.89 rmoveto\n");
    the_poly.moveto_relative(point(0,-289));
    // fprintf(f,"  3.25 0.72 rlineto\n");
    the_poly.lineto_relative(point(325,72));
    // fprintf(f,"  0.00 2.17 rlineto\n");
    the_poly.lineto_relative(point(0,217));
    // fprintf(f,"  -3.25 -0.72 rlineto\n");
    the_poly.lineto_relative(point(-325,-72));
    // fprintf(f,"  0.00 -2.17 rlineto\n");
    the_poly.lineto_relative(point(0,-217));
    the_poly.fill_poly();
    // fprintf(f,"  currentpoint fill moveto\n");
    // fprintf(f,"  0.00 6.50 rmoveto\n");
    the_poly.moveto_relative(point(0,650));
    // fprintf(f,"  0.00 -11.92 rlineto\n");
    // fprintf(f,"  currentpoint stroke moveto\n");
    the_poly.lineto_relative(point(0,-1192));
    the_poly.outline_poly();

    // fprintf(f,"  3.25 7.94 rmoveto\n");
    the_poly.moveto_relative(point(325,794));
    // fprintf(f,"  0.00 -11.92 rlineto\n");
    // fprintf(f,"  stroke\n");
    the_poly.lineto_relative(point(0,-1192));
    the_poly.outline_poly();
}

void
single_bar::create_poly(polygon &poly)
{
    //  fprintf(f,"  0 moveto  0 24 rlineto stroke\n");
    poly.moveto_relative(point(0,staff_height));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.outline_poly();
}

void
quarter_rest::create_poly(polygon& poly)
{
    // abc2ps one
#if 0
    //fprintf(f,"  -0.52 8.87 rmoveto\n");
    poly.moveto_relative(point(-52,887));
    //fprintf(f,"  8.35 -6.78 -2.61 -4.70 3.91 -11.48 rcurveto\n");
    poly.curveto_relative(point(835,-678),point(-261,-470),point(391,-1148));
    //fprintf(f,"  -4.43 1.57 -6.00 -3.13 -2.87 -5.22 rcurveto\n");
    poly.curveto_relative(point(-443,157),point(-600,-313),point(-287,-922));
    //fprintf(f,"  -5.22 2.09 -3.65 7.83 0.00 7.30 rcurveto\n");
    poly.curveto_relative(point(-522,209),point(-365,783),point(0,930));
    //fprintf(f,"  -5.22 4.17 3.13 3.13 -1.04 9.39 rcurveto\n");
    poly.curveto_relative(point(-522,417),point(313,313),point(-104,1139));
    poly.fill_poly();

#endif
#if 1
static double p[14][2]={
    {-100,1700}, {1700,400}, {-600,800}, {650,-500}, {-200,-200}, {-500,-1100}, {100,-1500},
    {-1100,-1300}, {-600,0}, {100,-100}, {-900,900}, {700,500}, {-100,1700} };
    
    double f1,f2;
    f1=6.0/11.5;
    f2=6.0/11.5;

    poly.moveto_relative(point((int)(f1 * p[0][0]), (int)(f2 * p[0][1])));

    int i1,i0;
    i1 = i0 = 1;
    int ncv = 4;
    for (int m = 0; m < ncv; m++)
    {
        poly.curveto_relative(
            point((int)(f1 * (p[i1 + 0][0] - p[i1 - 1][0])),
            (int)(f2 * (p[i1 + 0][1] - p[i1 - 1][1]))),
            point((int)(f1 * (p[i1 + 1][0] - p[i1 - 1][0])),
            (int)(f2 * (p[i1 + 1][1] - p[i1 - 1][1]))),
            point((int)(f1 * (p[i1 + 2][0] - p[i1 - 1][0])),
            (int)(f2 * (p[i1 + 2][1] - p[i1 - 1][1]))));

        i1 += 3;
    }
    poly.fill_poly();
#endif
}

void
half_rest::create_poly(polygon& poly)
{
    // fprintf(f,"  dup /y exch def exch dup /x exch def exch moveto\n");
    poly.moveto_relative(point(-600,0));
    // fprintf(f,"  -3 0 rmoveto 0 3 rlineto 6 0 rlineto 0 -3 rlineto fill\n");
    poly.lineto_relative(point(300,0));
    poly.lineto_relative(point(0,300));
    poly.lineto_relative(point(300,0));
    poly.lineto_relative(point(0,-300));
    poly.lineto_relative(point(300,0));
    poly.fill_poly();
}

void
eighth_rest::create_poly(polygon& poly)
{
    //fprintf(f,"  3.11 5.44 rmoveto\n");
    poly.moveto_relative(point(311,544));
    //fprintf(f,"  -1.17 -1.94 -1.94 -3.50 -3.69 -3.89 rcurveto\n");
    poly.curveto_relative(point(-117, -194),point( -194, -350)
                          ,point( -369, -389));
    //fprintf(f,"  2.14 2.72 -2.92 3.89 -2.92 1.17 rcurveto\n");
    poly.curveto_relative(point(214, 272),
                          point( -292, 389),
                          point( -292, 117));
    //fprintf(f,"  0.00 -1.17 1.17 -1.94 2.33 -1.94 rcurveto\n");
    poly.curveto_relative(point(0, -117),
                          point( 117, -194),
                          point( 233, -194));
    //fprintf(f,"  2.72 0.00 3.11 1.94 3.89 3.50 rcurveto\n");
    poly.curveto_relative(point(272, 000),point( 311, 194),
                          point( 389, 350));
    //fprintf(f,"  -3.42 -12.06 rlineto\n");
    poly.lineto_relative(point(-342, -1206));
    //fprintf(f,"  0.51 0.00 rlineto\n");
    poly.lineto_relative(point(51, 0));
    //fprintf(f,"  3.50 13.22 rlineto\n");
    poly.lineto_relative(point(350, 1322));
    poly.fill_poly();
}

void
sixteenth_rest::create_poly(polygon& poly)
{
    //fprintf(f,"  3.11 5.44 rmoveto\n");
    poly.moveto_relative(point(311, 544));
    poly.curveto_relative(point(-117 ,-194),
                          point( -194, -350), point(-369 ,-389));
    //fprintf(f,"  2.14 2.72 -2.92 3.89 -2.92 1.17 rcurveto\n");
    poly.curveto_relative(point(214 ,272),point( -292 ,389 ),
                          point(-292 ,117));
    //fprintf(f,"  0.00 -1.17 1.17 -1.94 2.33 -1.94 rcurveto\n");
    poly.curveto_relative(point(0, -117),point( 117 ,-194),
                          point( 233,194));
    //fprintf(f,"  2.72 0.00 3.11 1.94 3.89 3.50 rcurveto\n");
    poly.curveto_relative(point(272,0),point( 311, 194),
                          point( 389 ,350));
    //fprintf(f,"  -1.24 -4.28 rlineto\n");
    poly.lineto_relative(point( -124, -428));
    //fprintf(f,"  -1.17 -1.94 -1.94 -3.50 -3.69 -3.89 rcurveto\n");
    poly.curveto_relative(point(-117, -194),point( -194, -350),
                          point( -369, -389));
    //fprintf(f,"  2.14 2.72 -2.92 3.89 -2.92 1.17 rcurveto\n");
    poly.curveto_relative(point(214, 272 ),
                          point(-292, 389),
                          point( -292 ,117));
    //fprintf(f,"  0.00 -1.17 1.17 -1.94 2.33 -1.94 rcurveto\n");
    poly.curveto_relative(point(0, -117),point( 117 ,-194),
                          point( 233 ,-194));
    //fprintf(f,"  2.72 0.00 3.11 1.94 4.01 3.50 rcurveto\n");
    poly.curveto_relative(point(272, 0),
                          point(311, 194),point( 401, 350));
    poly.lineto_relative(point(-191, -700));
    //fprintf(f,"  0.51 0.00 rlineto\n");
    poly.lineto_relative(point(-51, 0));
    //fprintf(f,"  3.50 13.61 rlineto\n");
    poly.lineto_relative(point(350, 1361));
    poly.fill_poly();
}

void
whole_note::create_poly(polygon& poly)
{
    // fprintf(f,"  5.96 0.00 rmoveto\n");
    poly.moveto_relative(point(596,0));
    // fprintf(f,"  0.00 1.08 -2.71 3.52 -5.96 3.52 rcurveto\n");
    poly.curveto_relative(point(0, 108),
                           point(-271, 352),point( -596, 352));
    // fprintf(f,"  -3.25 0.00 -5.96 -2.44 -5.96 -3.52 rcurveto\n");
    poly.curveto_relative(point(-325, 0),
                           point(-596, -244),point( -596, -352));
    // fprintf(f,"  0.00 -1.08 2.71 -3.52 5.96 -3.52 rcurveto\n");
    poly.curveto_relative(point(0, -108),
                           point(271, -352),point( 596, -352));
    // fprintf(f,"  3.25 0.00 5.96 2.44 5.96 3.52 rcurveto\n");
    poly.curveto_relative(point(325,0),
                           point(596, 244),point( 596, 352));
    // fprintf(f,"  -8.13 1.62 rmoveto\n");
    poly.moveto_relative(point(-813,162));

    // fprintf(f,"  1.62 2.17 5.96 -1.07 4.34 -3.24 rcurveto\n");
    poly.curveto_relative(point(162, 217),
                           point(596, -107),point( 434, -324));

    // fprintf(f,"  -1.62 -2.17 -5.96 1.07 -4.34 3.24 rcurveto\n");
    poly.curveto_relative(point(-162, -217),
                           point(-596, 107),point( -434, 324));
    // fprintf(f,"   fill\n");
    poly.fill_poly();
}

void
whole_rest::create_poly(polygon& poly)
{
    poly.moveto_relative(point(-300, 0));
    poly.lineto_relative(point(0,-300));
    poly.lineto_relative(point(600,0));
    poly.lineto_relative(point(0,300));
    poly.fill_poly();
}

void
thin_thin_bar::create_poly(polygon& poly)
{
    poly.moveto_relative(point(0,0));
    poly.lineto_relative(point(0,4 * -staff_height));
    poly.outline_poly();
    poly.moveto_relative(point(400,0));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.outline_poly();
}

// The default size is 1 inch wide by 1/2 the staff height high.
nth_ending::nth_ending(const size& the_ppi,const scale& the_scale):
scaled_figure(the_ppi),
my_size_in_pixels(the_ppi.x,(int)((double)the_ppi.y * ((double)staff_height/1440.0)))
{
    my_scale = the_scale;
}

void
nth_ending::set_xform(polygon& poly)
{
    my_poly.set_x_zoom(1.0);
    my_poly.set_x_zoom(1.0);
    my_poly.set_y_offset(0);
    my_poly.set_x_offset(0);
}


nth_ending::nth_ending(const size& the_ppi,const scale& the_scale,const size& the_size):
scaled_figure(the_ppi),my_size_in_pixels(the_size)
{
    my_scale = the_scale;
}

void
nth_ending::create_poly(polygon& poly)
{
    poly.moveto_relative(point(0,0));
    poly.lineto_relative(point(0,-my_size_in_pixels.y));
    poly.lineto_relative(point(my_size_in_pixels.x,0));
    poly.outline_poly();
}

void
thin_thick_bar::create_poly(polygon& poly)
{
    poly.moveto_relative(point(0,staff_height));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.outline_poly();
    poly.moveto_relative(point(200,0));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.lineto_relative(point(200,0));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.lineto_relative(point(-200,0));
    poly.fill_poly();
}

void
thick_thin_bar::create_poly(polygon& poly)
{
    poly.moveto_relative(point(0,staff_height));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.lineto_relative(point(200,0));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.lineto_relative(point(-200,0));
    poly.fill_poly();
    poly.moveto_relative(point(400,0));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.outline_poly();
}

void
begin_repeat::create_poly(polygon& poly)
{
    // poly.moveto_relative(point(-200,0));
    poly.moveto_relative(point(0,staff_height));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.lineto_relative(point(200,0));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.lineto_relative(point(-200,0));
    poly.fill_poly();
    poly.moveto_relative(point(400,0));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.lineto_relative(point(-40,0));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.lineto_relative(point(40,0));
    poly.fill_poly();
    poly.moveto_relative(point(400,-800));
    poly.arc(0,360,120);
    poly.fill_poly();
    poly.moveto_relative(point(-120,-1000));
    poly.arc(0,360,120);
    poly.fill_poly();
}

void
end_repeat::create_poly(polygon& poly)
{
    // poly.moveto_relative(point(-200,0));
    poly.moveto_relative(point(0,staff_height));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.lineto_relative(point(200,0));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.lineto_relative(point(-200,0));
    poly.fill_poly();
    poly.moveto_relative(point(-200,0));
    poly.lineto_relative(point(0,-4 * staff_height));
    poly.lineto_relative(point(40,0));
    poly.lineto_relative(point(0,4 * staff_height));
    poly.lineto_relative(point(-40,0));
    poly.fill_poly();
    poly.moveto_relative(point(-400,-(3 * staff_height)/2));
    poly.arc(0,360,120);
    poly.fill_poly();
    poly.moveto_relative(point(-120,-1000));
    poly.arc(0,360,120);
    poly.fill_poly();
}

void
begin_end_repeat::create_poly(polygon& poly)
{
    // End part
    // poly.moveto_relative(point(-200,0));
    poly.moveto_relative(point(0,staff_height));
    poly.lineto_relative(point(0,-2400));
    poly.lineto_relative(point(200,0));
    poly.lineto_relative(point(0,2400));
    poly.lineto_relative(point(-200,0));
    poly.fill_poly();
    poly.moveto_relative(point(-200,0));
    poly.lineto_relative(point(0,-2400));
    poly.lineto_relative(point(40,0));
    poly.lineto_relative(point(0,2400));
    poly.lineto_relative(point(-40,0));
    poly.fill_poly();
    poly.moveto_relative(point(-400,-800));
    poly.arc(0,360,120);
    poly.fill_poly();
    poly.moveto_relative(point(-120,-1000));
    poly.arc(0,360,120);
    poly.fill_poly();

    // Begin part
    poly.moveto_relative(point(500,1800));
    poly.lineto_relative(point(0,-2400));
    poly.lineto_relative(point(-40,0));
    poly.lineto_relative(point(0,2400));
    poly.lineto_relative(point(40,0));
    poly.fill_poly();
    poly.moveto_relative(point(400,-800));
    poly.arc(0,360,120);
    poly.fill_poly();
    poly.moveto_relative(point(-120,-1000));
    poly.arc(0,360,120);
    poly.fill_poly();

}

void
common_time_figure::create_poly(polygon& the_poly)
{
    //fprintf(f,"  0 moveto\n");
    the_poly.moveto_relative(point(0,0));
    //fprintf(f,"  1.00 17.25 rmoveto\n");
    //fprintf(f,"  1.00 0.00 2.75 -0.75 2.75 -3.00 rcurveto\n");
    the_poly.curveto_relative(point(100, 0),
                           point(275, -75),point( 275, -300));
    //fprintf(f,"  0.00 1.50 -1.50 1.25 -1.50 0.00 rcurveto\n");
    the_poly.curveto_relative(point(0, 150),
                           point(-150, 125),point( -150, 0));
    //fprintf(f,"  0.00 -1.25 1.75 -1.25 1.75 0.25 rcurveto\n");
    the_poly.curveto_relative(point(0, -125),
                           point(175, -125),point( 175, 25));
    //fprintf(f,"  0.00 2.50 -1.50 3.25 -3.00 3.25 rcurveto\n");
    the_poly.curveto_relative(point(0, 250),
                           point(-150, 325),point( -300, 325));
    //fprintf(f,"  -3.75 0.00 -6.25 -2.75 -6.25 -6.50 rcurveto\n");
    the_poly.curveto_relative(point(-375, 0),
                           point(-625, -275),point( -625, -650));
    //fprintf(f,"  0.00 -3.00 3.75 -7.50 9.00 -2.50 rcurveto\n");
    the_poly.curveto_relative(point(0, -300),
                           point(375, -750),point( 900, -250));
    //fprintf(f,"  -4.25 -3.00 -7.25 -0.75 -7.25 2.50 rcurveto\n");
    the_poly.curveto_relative(point(-425, -300),
                           point(-725, -75),point( -725, 250));
    //fprintf(f,"  0.00 3.00 1.00 6.00 4.50 6.00 rcurveto\n");
    the_poly.curveto_relative(point(0,300),
                           point(100, 600),point( 450, 600));
    //fprintf(f,"   fill\n");
    the_poly.fill_poly();
}

void
cut_time_figure::create_poly(polygon& the_poly)
{
    //fprintf(f,"  0 moveto\n");
    the_poly.moveto_relative(point(0,0));
    //fprintf(f,"  1.00 17.25 rmoveto\n");
    //fprintf(f,"  1.00 0.00 2.75 -0.75 2.75 -3.00 rcurveto\n");
    the_poly.curveto_relative(point(100, 0),
                           point(275, -75),point( 275, -300));
    //fprintf(f,"  0.00 1.50 -1.50 1.25 -1.50 0.00 rcurveto\n");
    the_poly.curveto_relative(point(0, 150),
                           point(-150, 125),point( -150, 0));
    //fprintf(f,"  0.00 -1.25 1.75 -1.25 1.75 0.25 rcurveto\n");
    the_poly.curveto_relative(point(0, -125),
                           point(175, -125),point( 175, 25));
    //fprintf(f,"  0.00 2.50 -1.50 3.25 -3.00 3.25 rcurveto\n");
    the_poly.curveto_relative(point(0, 250),
                           point(-150, 325),point( -300, 325));
    //fprintf(f,"  -3.75 0.00 -6.25 -2.75 -6.25 -6.50 rcurveto\n");
    the_poly.curveto_relative(point(-375, 0),
                           point(-625, -275),point( -625, -650));
    //fprintf(f,"  0.00 -3.00 3.75 -7.50 9.00 -2.50 rcurveto\n");
    the_poly.curveto_relative(point(0, -300),
                           point(375, -750),point( 900, -250));
    //fprintf(f,"  -4.25 -3.00 -7.25 -0.75 -7.25 2.50 rcurveto\n");
    the_poly.curveto_relative(point(-425, -300),
                           point(-725, -75),point( -725, 250));
    //fprintf(f,"  0.00 3.00 1.00 6.00 4.50 6.00 rcurveto\n");
    the_poly.curveto_relative(point(0,300),
                           point(100, 600),point( 450, 600));
    //fprintf(f,"   fill\n");
    the_poly.fill_poly();
    
    the_poly.moveto_relative(point(-100,300));
    the_poly.lineto_relative(point(0,-1400));
    the_poly.lineto_relative(point(-40,0));
    the_poly.lineto_relative(point(0,1400));
    the_poly.lineto_relative(point(40,0));
    the_poly.fill_poly();
}

void
gracing_figure::create_poly(polygon &poly)
{
    // fprintf(f,"  -5.00 -1.00 rmoveto\n");
    poly.moveto_relative(point(-500,-100));
    // fprintf(f,"  5.00 8.50 5.50 -4.50 10.00 2.00 rcurveto\n");
    poly.curveto_relative(point(500,850),
                           point(550,-450),point(1000,200));
    // fprintf(f,"  -5.00 -8.50 -5.50 4.50 -10.00 -2.00 rcurveto\n");
    poly.curveto_relative(point(-500,-850),
                           point(-550, 450),point(-1000,-200));
    poly.fill_poly();
}

void
roll_sign::create_poly(polygon &poly)
{
    //fprintf(f,"  -5.85 0.00 rmoveto\n");
    poly.moveto_relative(point(-585,0));
    //fprintf(f,"  0.45 7.29 11.25 7.29 11.70 0.00 rcurveto\n");
    poly.curveto_relative(point(45,729),
                           point(1125,729),point(1170,0));
    //fprintf(f,"  -1.35 5.99 -10.35 5.99 -11.70 0.00 rcurveto\n");
    poly.curveto_relative(point(-135,599),
                           point(-1035,599),point(-1170,0));
    poly.fill_poly();
}

void
slide_figure::create_poly(polygon &poly)
{
    // fprintf(f,"  -7.20 -4.80 rmoveto\n");
    poly.moveto_relative(point(-720,-480));
    // fprintf(f,"  1.80 -0.70 4.50 0.20 7.20 4.80 rcurveto\n");
    poly.curveto_relative(point(180,-70),
                           point(450,20),point(720,480));
    // fprintf(f,"  -2.07 -5.00 -5.40 -6.80 -7.65 -6.00 rcurveto\n");
    poly.curveto_relative(point(-207,-500),
                           point(-540,-680),point(-765,-600));
    // fprintf(f,"   fill\n");
    poly.fill_poly();
}

void
accent_figure::create_poly(polygon &poly)
{
    // fprintf(f," -2.5 0 rmoveto 5 0 rlineto stroke grestore\n");
    poly.moveto_relative(point(-250,0));
    poly.lineto_relative(point(500,-250));
    poly.lineto_relative(point(-500,-250));
    poly.outline_poly();
}

void
fermata_figure::create_poly(polygon &poly)
{
    poly.moveto_relative(point(-750,0));
    // fprintf(f,"  0.00 11.50 15.00 11.50 15.00 0.00 rcurveto\n");
    poly.curveto_relative(point(0,1150),
                           point(1500,1150),point(1500,0));
    // fprintf(f,"  -0.25 0.00 rlineto\n");
    poly.lineto_relative(point(-25,0));
    // fprintf(f,"  -1.25 9.00 -13.25 9.00 -14.50 0.00 rcurveto\n");
    poly.curveto_relative(point(-125,900),
                           point(-1325,900),point(-1450,0));
    // fprintf(f,"   fill\n");
    poly.fill_poly();
    // fprintf(f,"  x exch 2 copy 1.5 add 1.3 0 360 arc moveto\n");
    // fprintf(f,"  -7.50 0.00 rmoveto\n");
    poly.moveto_relative(point(720,0));
    poly.arc(0,360,130);
    poly.fill_poly();
}

void
down_bow_figure::create_poly(polygon &poly)
{
    // fprintf(f,"  -3.20 0.00 rmoveto\n");
    poly.moveto_relative(point(-320,0));
    // fprintf(f,"  0.00 7.20 rlineto\n");
    poly.lineto_relative(point(0,720));
    // fprintf(f,"  6.40 0.00 rlineto\n");
    poly.lineto_relative(point(640,0));
    // fprintf(f,"  0.00 -7.20 rlineto\n");
    poly.lineto_relative(point(0,-720));
    // fprintf(f,"   currentpoint stroke moveto\n");
    poly.outline_poly();
    // fprintf(f,"  -6.40 4.80 rmoveto\n");
    poly.moveto_relative(point(-640,480));
    // fprintf(f,"  0.00 2.40 rlineto\n");
    poly.lineto_relative(point(0,240));
    // fprintf(f,"  6.40 0.00 rlineto\n");
    poly.lineto_relative(point(640,0));
    // fprintf(f,"  0.00 -2.40 rlineto\n");
    poly.lineto_relative(point(0,-240));
    // fprintf(f,"   fill\n");
    poly.fill_poly();
}

void
up_bow_figure::create_poly(polygon &poly)
{
    // fprintf(f,"  -2.56 8.80 rmoveto\n");
    poly.moveto_relative(point(-640,480));
    // fprintf(f,"  2.56 -8.80 rlineto\n");
    poly.lineto_relative(point(256,-880));
    // fprintf(f,"  2.56 8.80 rlineto\n");
    poly.lineto_relative(point(256,880));
    poly.outline_poly();
}

void
mordant_figure::create_poly(polygon &poly)
{
    //" 2.2 2.2 rlineto 2.18 -3.0 rlineto 0.62 0.8 rlineto\n"
    poly.moveto_relative(point(0,0));
    poly.lineto_relative(point(220,220));
    poly.lineto_relative(point(218,-300));
    poly.lineto_relative(point(62,80));
    //" -2.2 -2.2 rlineto -2.18 3.0 rlineto -0.62 -0.8 rlineto\n"
    poly.lineto_relative(point(-220,-220));
    poly.lineto_relative(point(-218,300));
    poly.lineto_relative(point(-62,-80));
    //" -2.2 -2.2 rlineto -2.18 3.0 rlineto -0.62 -0.8 rlineto\n"
    poly.lineto_relative(point(-220,-220));
    poly.lineto_relative(point(-218,300));
    poly.lineto_relative(point(-62,-80));
    //" 2.2 2.2 rlineto 2.18 -3.0 rlineto 0.62 0.8 rlineto fill\n"
    poly.lineto_relative(point(220,220));
    poly.lineto_relative(point(218,-300));
    poly.lineto_relative(point(62,80));
    poly.fill_poly();
}

void
trill_figure::create_poly(polygon& poly)
{
    poly.moveto_relative(point(0,0));
    poly.lineto_relative(point(220,220));
    poly.lineto_relative(point(218,-300));
    poly.lineto_relative(point(62,80));
    //" -2.2 -2.2 rlineto -2.18 3.0 rlineto -0.62 -0.8 rlineto\n"
    poly.lineto_relative(point(-220,-220));
    poly.lineto_relative(point(-218,300));
    poly.lineto_relative(point(-62,-80));
    //" -2.2 -2.2 rlineto -2.18 3.0 rlineto -0.62 -0.8 rlineto\n"
    poly.lineto_relative(point(-220,-220));
    poly.lineto_relative(point(-218,300));
    poly.lineto_relative(point(-62,-80));
    //" 2.2 2.2 rlineto 2.18 -3.0 rlineto 0.62 0.8 rlineto fill\n"
    poly.lineto_relative(point(220,220));
    poly.lineto_relative(point(218,-300));
    poly.lineto_relative(point(62,80));
    poly.fill_poly();
}

crescendo_figure::crescendo_figure(const size& the_ppi,int the_width):
scaled_figure(the_ppi),my_width(the_width),my_depth(7)
{
}

void
crescendo_figure::create_poly(polygon& poly)
{
    poly.moveto_relative(point(0,my_depth));
    poly.lineto_relative(point(my_width,-my_depth));
    poly.moveto_relative(point(-my_width,my_depth));
    poly.lineto_relative(point(my_width,my_depth));
    poly.outline_poly();
}

void
crescendo_figure::set_xform(polygon& poly)
{
    double x_xform = 1.0 * (double)(my_ppi.y) / 96.0;
    double y_xform = 1.0 * (double)(my_ppi.y) / 96.0;;
    poly.set_x_zoom(x_xform);
    poly.set_y_zoom(y_xform);
}

void
decrescendo_figure::set_xform(polygon& poly)
{
    double x_xform = 1.0 * (double)(my_ppi.y) / 96.0;
    double y_xform = 1.0 * (double)(my_ppi.y) / 96.0;;
    poly.set_x_zoom(x_xform);
    poly.set_y_zoom(y_xform);
}

decrescendo_figure::decrescendo_figure(const size& the_ppi,int the_width):
scaled_figure(the_ppi),my_width(the_width),my_depth(7)
{
}

void
decrescendo_figure::create_poly(polygon& poly)
{
    poly.lineto_relative(point(my_width,my_depth));
    poly.lineto_relative(point(-my_width,my_depth));
    poly.outline_poly();
}

box_figure::box_figure(const size& the_ppi,const size& box_size)
:scaled_figure(the_ppi),my_size(box_size)
{
}
box_figure::~box_figure()
{
}
void
box_figure::create_poly(polygon& poly)
{
    poly.lineto_relative(point(my_size.x,0));
    poly.lineto_relative(point(0,-my_size.y));
    poly.lineto_relative(point(-my_size.x,0));
    poly.lineto_relative(point(0,my_size.y));
    poly.outline_poly();
}
void
box_figure::set_xform(polygon &poly)
{
    double x_xform = 1.0 * (double)(my_ppi.y) / 96.0;
    double y_xform = 1.0 * (double)(my_ppi.y) / 96.0;;
    poly.set_x_zoom(x_xform);
    poly.set_y_zoom(y_xform);
}

long_rest_figure::long_rest_figure(const size& the_ppi,const size& box_size)
:scaled_figure(the_ppi),my_size(box_size)
{
}
long_rest_figure::~long_rest_figure()
{
}
void
long_rest_figure::create_poly(polygon& poly)
{
    int tmp_bar_width = my_size.y / 4;
    int tmp_bar_distance = (my_size.y / 2) - tmp_bar_width;
    if (tmp_bar_width < 2)
    {
        tmp_bar_width = 2;
    }
    poly.moveto_relative(point(0,-tmp_bar_distance));
    poly.lineto_relative(point(0,my_size.y));
    poly.lineto_relative(point(tmp_bar_width,0));
    poly.lineto_relative(point(0,-tmp_bar_distance));
    poly.lineto_relative(point(my_size.x - tmp_bar_width,0));
    poly.lineto_relative(point(0,tmp_bar_distance));
    poly.lineto_relative(point(tmp_bar_width,0));
    poly.lineto_relative(point(0,-my_size.y));
    poly.lineto_relative(point(-tmp_bar_width,0));
    poly.lineto_relative(point(0,tmp_bar_distance));
    poly.lineto_relative(point(-(my_size.x - tmp_bar_width),0));
    poly.lineto_relative(point(0,-tmp_bar_distance));
    poly.lineto_relative(point(-tmp_bar_width,0));
    poly.fill_poly();
}

void
long_rest_figure::set_xform(polygon &poly)
{
    double x_xform = 1.0 * (double)(my_ppi.y) / 96.0;
    double y_xform = 1.0 * (double)(my_ppi.y) / 96.0;;
    poly.set_x_zoom(x_xform);
    poly.set_y_zoom(y_xform);
}

}

