/*
 * simple.cpp - struct-like classes, e.g. point, fraction, etc.
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
#include "iabc/simple.h"

namespace iabc
{

;
int
fraction::find_lcd(const fraction& o) const
{
    if ((o.den == 0) || (den == 0))
        {
        return 0;
        }

    return den * o.den;
}

void
fraction::reduce()
{
    int tmp_gcd = 1;
    int tmp_low;
    if (num > den)
    {
        tmp_low = den;
    }
    else
    {
        tmp_low = num;
    }

    int tmp_test = 2;
    while (tmp_test <= tmp_low)
    {
        if ((num % tmp_test == 0) &&
            (den % tmp_test == 0))
        {
            tmp_gcd = tmp_test;
            num /= tmp_test;
            den /= tmp_test;
            if (num > den)
            {
                tmp_low = den;
            }
            else
            {
                tmp_low = num;
            }
        }
        else
        {
            ++tmp_test;
        }
    }

    // Represent exactly 0 in exactly 1 way.
    if (num == 0) den = 0;
}

void 
fraction::convert_to_lcd(fraction& the_other)
{
    ;
    if (den == 0)
      num = 0;
    if (the_other.den == 0)
      the_other.num = 0;
    if ((den != 0) && (the_other.den != 0))
    {
        int tmp_lcd = find_lcd(the_other);
        num = num * (tmp_lcd / den);
        den = tmp_lcd;
        the_other.num = the_other.num * (tmp_lcd / the_other.den);
        the_other.den = tmp_lcd;
    }
}

fraction 
fraction::operator+(const fraction& o) const
{
    if (den == 0)
    {
        return o;
    }
    else if (o.den == 0)
    {
        return *this;
    }
    fraction tmp_copy = o;
    fraction tmp_this_copy = *this;
    tmp_this_copy.convert_to_lcd(tmp_copy);
    fraction f(tmp_this_copy.num + tmp_copy.num,tmp_this_copy.den);
    f.reduce();
    return f;
}

bool
fraction::operator==(const fraction& o) const
{
    fraction tmp1 = *this;
    fraction tmp2 = o;
    tmp1.reduce();
    tmp2.reduce();
    return ((tmp1.num == tmp2.num) && (tmp1.den == tmp2.den));
}

double 
fraction::as_double() const
{
    if ((num!=0) && (den != 0)) return (double)num/(double)den;
    else return 0.0;
}

bool 
fraction::operator>(const fraction& o) const
{
    ;
    fraction tmp_lcd = o;
    fraction tmp_this = *this;
    tmp_this.convert_to_lcd(tmp_lcd);
    return tmp_this.num > tmp_lcd.num;
}

point
rect::center(const rect& o) const
{
    point tmp_rv((ul.x + o.ul.x) / 2,(ul.y + o.ul.y) / 2);
    return tmp_rv;
}

rect
rect::intersection(const rect& o) const
{
    int x[4];
    x[0] = ul.x;
    x[1] = o.ul.x;
    x[2] = lr.x;
    x[3] = o.lr.x;
    int y[4];
    y[0] = ul.y;
    y[1] = o.ul.y;
    y[2] = lr.y;
    y[3] = o.lr.y;
    int i,j,swap;
    for (i = 0;i < 4;++i)
    {
        for (j = i;j < 4;++j)
        {
            if (x[j] < x[i])
            {
                swap = x[j];
                x[j] = x[i];
                x[i] = swap;
            }
            if (y[j] < y[i])
            {
                swap = y[j];
                y[j] = y[i];
                y[i] = swap;
            }
        }
    }
    return rect(point(x[1],y[1]),point(x[2],y[2]));
}

bool
rect::intersects(const rect& o) const
{
    int x[4];
    x[0] = ul.x;
    x[1] = o.ul.x;
    x[2] = lr.x;
    x[3] = o.lr.x;
    int y[4];
    y[0] = ul.y;
    y[1] = o.ul.y;
    y[2] = lr.y;
    y[3] = o.lr.y;
    int i,j,swap;
    for (i = 0;i < 4;++i)
    {
        for (j = i;j < 4;++j)
        {
            if (x[j] < x[i])
            {
                swap = x[j];
                x[j] = x[i];
                x[i] = swap;
            }
            if (y[j] < y[i])
            {
                swap = y[j];
                y[j] = y[i];
                y[i] = swap;
            }
        }
    }
    point tmp_mid((x[1] + x[2]) / 2,(y[1] + y[2]) / 2);

    return (contains(tmp_mid)) && (o.contains(tmp_mid));
}

point stretch_point(const point& p,const rect& source,const rect& target)
{
    point tmp_rv = p - (source.ul - target.ul);
    tmp_rv.x =  
		(int)((double)tmp_rv.x * ((double)target.width() / (double)source.width()));
    tmp_rv.y =  
		(int)((double)tmp_rv.y * ((double)target.height() / (double)source.height()));
    return tmp_rv;
}

point scale_point(const point& p,const rect& source,const rect& target)
{
    point tmp_rv = p;
    tmp_rv.x = (int)((double)tmp_rv.x * (double)target.width() / (double)source.width());
    tmp_rv.y = (int)((double)tmp_rv.y * (double)target.height() / (double)source.height());
    return tmp_rv;
}

}
