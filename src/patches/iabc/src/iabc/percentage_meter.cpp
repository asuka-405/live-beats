/*
 * percentage_meter.cpp - interface for some type of progress meter.
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
#include "iabc/percentage_meter.h"

namespace iabc
{
;
percentage_meter::percentage_meter():
my_is_cancelled(false)
{
}

percentage_meter::~percentage_meter()
{
}

void 
percentage_meter::set_message(const string& the_string)
{
    my_message = the_string;
    update(percentage_meter::update_gui);
}

void
percentage_meter::set_percentage(double the_number)
{
    my_value = the_number;
    update(percentage_meter::update_gui);
}

}
