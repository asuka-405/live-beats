/*
 * simple.h - struct-like classes, e.g. point, fraction, etc.
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
#ifndef iabc_simple_h
#define iabc_simple_h
#ifdef _DEBUG
#define point_ordinal po_
#endif

#include "iabc/string.h"
#include <math.h>
#include <stdlib.h>

// FILE: simple.h
// DESCRIPTION:
// Define some simple types that are useful, like point
// and line and stuff.
namespace iabc
{
;

// These macros define basic by-value types of operations on
// the simple types, saves a little typing.
#define OPERATOR_EQUALS2(t,a,b) \
    t& operator=(const t& o){ \
        a = o.a; \
        b = o.b; \
        return *this; \
        }

#define COPY_CONSTRUCTOR2(t,a,b) \
        t(const t& o){ \
        a = o.a; \
        b = o.b; \
        }

#define EXPLICIT_CONSTRUCTOR2(t1,t2,a,b) \
        t1(const t2& the_a,const t2& the_b): \
            a(the_a),b(the_b){}

#define OPERATOR_EQUALSEQUALS2(t,a,b) \
        bool operator==(const t& o) const{ \
        return ((a == o.a) && \
        (b == o.b)); \
        }

#define OPERATOR_NOTEQUALS2(t,a,b) \
        bool operator!=(const t& o) const{ \
        return ((a != o.a) || \
        (b != o.b)); \
        }

#define GENERIC_STUFF2(t,t1,a,b) \
    COPY_CONSTRUCTOR2(t,a,b); \
    OPERATOR_EQUALSEQUALS2(t,a,b); \
    OPERATOR_EQUALS2(t,a,b) \
    OPERATOR_NOTEQUALS2(t,a,b) \
    EXPLICIT_CONSTRUCTOR2(t,t1,a,b)

;
// CLASS: point
// DESCRIPTION:
// A 2 dimensional point, and some usefule
// operations
class point
{
public:
    int x;
    int y;
    point(){x=0;y=0; };
    ~point(){};
    point& operator+=(const point& o){x += o.x;y += o.y;return *this;};
    point& operator-=(const point& o){x -= o.x;y -= o.y;return *this;};
    point operator/(int div) const{int ax = x/div;int ay = y/div;return point(ax,ay);};
    point operator*(int mul)const{int ax = x*mul;int ay = y*mul;return point(ax,ay);};
    point operator*(const point& mul)const{int ax = x*mul.x;int ay = y*mul.y;return point(ax,ay);};
    point operator+(const point& o) const{int ax = x + o.x;int ay = y + o.y;return point(ax,ay);};
    point operator- (const point& o) const{int ax = x - o.x;int ay = y - o.y;return point(ax,ay);};
    bool adjacent(const point& o) const{return ((abs(o.x - x) <= 1) &&
                                                (abs(o.y - y) <= 1));};
    GENERIC_STUFF2(point,int,x,y);
};

// CLASS: point_ordinal
// DESCRIPTION:
// In order to create an ordinal type out of 'point',
// assign the y value arbitrary importance.
class point_ordinal
{
public:
    point_ordinal(){};
    point_ordinal(const point& the_point):my_point(the_point){};
    point_ordinal& operator=(const point_ordinal& o){my_point = o.my_point;return *this;};
    point_ordinal(const point_ordinal& o):my_point(o.my_point){};
    bool operator==(const point_ordinal& o) const {return my_point == o.my_point;};
    bool operator!=(const point_ordinal& o) const {return my_point != o.my_point;};
    bool operator>(const point_ordinal& o) const
    {
        if (my_point.y > o.my_point.y)
        {
            return true;
        }
        else if ((my_point.y == o.my_point.y) &&
                 (my_point.x > o.my_point.x))
        {
            return true;
        }
        return false;
    };
    bool operator>=(const point_ordinal& o) const {return ((*this > o) || (*this == o));};
    bool operator<(const point_ordinal& o) const {return ((*this >= o) == false);};
    bool operator<=(const point_ordinal& o) const {return ((*this > o) == false);};
    operator point() const{return my_point;};
private:
    point my_point;
};


// CLASS: size
// DESCRIPTION:
// same as point
typedef point size;

// DESCRIPTION:
// Define a fraction as 2 int's, and some useful
// operations
class fraction
{
public:
    int num;
    int den;
    fraction():num(0),den(0){};
    fraction(int the_whole_number):num(the_whole_number),den(1){};
    ~fraction(){};
    int find_lcd(const fraction& o) const;
    void convert_to_lcd(fraction& the_other);
    fraction operator*(int the_scalar) const{ 
        fraction f(num*the_scalar,den);
        f.reduce();
        return f;};
    fraction operator/(const fraction& the_fraction) const
    {
        fraction f(den,num);
        f = (f * the_fraction);
        f.reduce();
		return f;
    };
    fraction operator*(const fraction& the_fraction) const
        {return fraction(the_fraction.num * this->num,the_fraction.den*this->den);};
    fraction& operator*=(const fraction& the_fraction)
    {
        *this = ((*this) * the_fraction);
        reduce();
		return *this;
    }
    fraction& operator*=(int the_scalar){*this = *this * the_scalar;reduce();return *this;};
    fraction operator+(const fraction& o) const;
    fraction operator-(const fraction& o) const{return *this + (o * -1);};
    fraction get_remainder() const {if (den != 0) return fraction(num % den,den); 
        else return fraction();};
    int as_int() const {if (den != 0) return num/den; else return 0;};
    fraction& operator+=(const fraction& o) {*this = *this + o;return *this;};
    fraction& operator-=(const fraction& o) {*this = *this - o;return *this;};
    bool operator==(const fraction& o) const;
    bool operator>(const fraction& o) const;
    bool operator>=(const fraction& o) const{return ((*this > o) || (*this == o));};
    bool operator<(const fraction& o) const{return ((*this >= o) ? false : true);};
    bool operator<=(const fraction& o) const{return ((*this > o) ? false : true);};
    double as_double() const;
    void reduce();
    COPY_CONSTRUCTOR2(fraction,num,den);
    OPERATOR_EQUALS2(fraction,num,den);
    OPERATOR_NOTEQUALS2(fraction,num,den);
    EXPLICIT_CONSTRUCTOR2(fraction,int,num,den);
};

// CLASS: scale
// DESCRIPTION:
// a floating point version of size
class scale
{
public:
    double x;
    double y;
    scale(){x=0;y=0; };
    ~scale(){};
    scale& operator+=(const scale& o){x += o.x;y += o.y;return *this;};
    scale operator/(double div){double ax = x/div;double ay = y/div;return scale(ax,ay);};
    scale operator+(const scale& o){double ax = x + o.x;double ay = y + o.y;return scale(ax,ay);};
    scale operator*(const scale& o){double ax = x * o.x;double ay = y * o.y;return scale(ax,ay);};
    scale operator*(double d){double ax = d * x;double ay = y * d;return scale(ax,ay);};
    GENERIC_STUFF2(scale,double,x,y);
};

// CLASS: line
// DESCRIPTION:
// A line is just 2 points.
class line
{
public:
    point p1;
    point p2;
    point midpoint() const{return point(((p1.x+ p2.x)/2),((p1.y+ p2.y)/2));};
    int magnitude() const{return (int)sqrt(((double)(p1.x - p2.x)) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));};
    int magnitude2() const{return (int)((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));};
    line(){};
    ~line(){};
    GENERIC_STUFF2(line,point,p1,p2);
};

// CLASS: rect
// DESCRIPTION:
// A rectangle is 2 points also, but it's treated differently
class rect
{
public:
    point ul;
    point lr;
    rect(){ };
    ~rect(){};
    point ur() const {return point(lr.x,ul.y);};
    point ll() const {return point(ul.x,lr.y);};
    int height() const {return abs(bottom() - top());};
    int width() const {return abs(right() - left());};
    int bottom() const {return lr.y;};
    int top() const {return ul.y;};
    int left() const {return ul.x;};
    int right() const {return lr.x;};
    point center() const {return point((ul.x + lr.x) / 2,
                                       (ul.y + lr.y) / 2);
    };
    point center(const rect& o) const;
    bool contains(const point& the_point) const{
        return ((the_point.x >= ul.x) && (the_point.x <= lr.x) &&
                (the_point.y >= ul.y) && (the_point.y <= lr.y));
    };

    bool intersects(const rect& o) const;
    rect intersection(const rect& o) const;
    rect offset(const point& the_offset) const {return rect(ul + the_offset,lr + the_offset);};
    GENERIC_STUFF2(rect,point,ul,lr);
};

// DESCRIPTION:
// These functions are used to map a point from one rectangle to one
// of a different size.
extern point stretch_point(const point& p,const rect& r1,const rect& r2);

extern point scale_point(const point& p,const rect& r1,const rect& r2);

class wd_ellipse
{
public:
    point ul;
    point lr;
    wd_ellipse(){};
    ~wd_ellipse(){};
    GENERIC_STUFF2(wd_ellipse,point,ul,lr);
};

class scaleloc
{
public:
    scaleloc(){};
    ~scaleloc(){};
    scale my_scale;
    point my_loc;
    scaleloc(const scaleloc& o){my_scale = o.my_scale;my_loc = o.my_loc;};
    bool operator==(const scaleloc& o)const {return ((o.my_loc == my_loc) && (o.my_scale == my_scale));};
    scaleloc& operator=(const scaleloc& o){my_scale = o.my_scale;my_loc = o.my_loc;return *this;};
    bool operator!=(const scaleloc& o)const {return ((o.my_loc != my_loc) || (o.my_scale != my_scale));};
    scaleloc(const scale& s,const point& p){my_scale = s;my_loc = p;};
};

class sizeloc
{
public:
    sizeloc(){};
    ~sizeloc(){};
    size my_size;
    point my_loc;
    sizeloc(const sizeloc& o){my_size = o.my_size;my_loc = o.my_loc;};
    bool operator==(const sizeloc& o)const {return ((o.my_loc == my_loc) && (o.my_size == my_size));};
    sizeloc& operator=(const sizeloc& o){my_size = o.my_size;my_loc = o.my_loc;return *this;};
    bool operator!=(const sizeloc& o)const {return ((o.my_loc != my_loc) || (o.my_size != my_size));};
    sizeloc(const size& s,const point& p){my_size = s;my_loc = p;};
    rect as_rect() const{return rect(my_loc,my_loc+point(my_size.x,my_size.y));};
};

class wd_curve
{
public:
    point start;
    point through;
    point end;
    wd_curve& operator=(const wd_curve& o){
        start = o.start;
        end = o.end;
        through = o.through;
        return *this;
        };
    wd_curve(const wd_curve& o){
        start = o.start;
        end = o.end;
        through = o.through;
        };
    bool operator==(const wd_curve& o) const{
        return ((start == o.start) && 
        (end == o.end) &&
        (through == o.through));
        };
};
}
#endif


