/*
 * figure_factory.cpp - Factory for some commonly-used figures.
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
#include "iabc/figure_factory.h"
#include "iabc/draw_figure.h"
#include "iabc/list.cpp"
#include "iabc/map.cpp"

namespace iabc
{

globalton<figure_factory>
figure_factory::force_init;

figure_factory&
figure_factory::get_instance()
{
    static figure_factory* the_factory = 0;
    if (the_factory == 0)
    {
        the_factory = new figure_factory;
    }
    return *the_factory;
}

void
figure_factory::gc()
{
    delete (&(get_instance()));
}

    
figure_factory::~figure_factory()
{
    map<figure_ordinal,figure*>::iterator tmp_it = figure_map.least();
    while (tmp_it)
    {
        figure* tmp_figure = (*tmp_it).value;
        tmp_figure->remove_ref();
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
    }
}

figure* 
figure_factory::get_figure(const size& the_screen_ppi,
                       figure_factory::FIGURE_ENUM the_figure_id,
					   const scale& the_scale)
{
    lock tmp_lock(figure_list_mutex);
    scale tmp_real_scale = scale(the_scale.x * (double)the_screen_ppi.x,
                                 the_scale.y * (double)the_screen_ppi.y);

    figure *tmp_figure = get_figure(the_figure_id,tmp_real_scale);
    if (tmp_figure == 0)
    {
        if (the_figure_id == figure_factory::STAFF)
            {
            tmp_figure = new staff_figure(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::GCLEF)
            {
            tmp_figure = new gclef(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::FCLEF)
            {
            tmp_figure = new fclef(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::OPEN_HEAD)
            {
            tmp_figure = new open_head(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::CLOSED_HEAD)
            {
            tmp_figure = new closed_head(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::DOT)
            {
            tmp_figure = new dot(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::UP_STEM)
            {
            tmp_figure = new up_stem(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::DOWN_STEM)
            {
            tmp_figure = new down_stem(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::SHARP_SIGN)
            {
            tmp_figure = new sharp_sign(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::NATURAL_SIGN)
            {
            tmp_figure = new natural_sign(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::FLAT_SIGN)
            {
            tmp_figure = new flat_sign(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::SINGLE_BAR)
            {
            tmp_figure = new single_bar(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::LEDGER_LINE)
            {
            tmp_figure = new ledger_line(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::QUARTER_REST)
            {
            tmp_figure = new quarter_rest(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::HALF_REST)
            {
            tmp_figure = new half_rest(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::EIGHTH_REST)
            {
            tmp_figure = new eighth_rest(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::SIXTEENTH_REST)
            {
            tmp_figure = new sixteenth_rest(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::WHOLE_NOTE)
            {
            tmp_figure = new whole_note(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::WHOLE_REST)
            {
            tmp_figure = new whole_rest(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::THIN_THIN_BAR)
            {
            tmp_figure = new thin_thin_bar(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::THIN_THICK_BAR)
            {
            tmp_figure = new thin_thick_bar(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::THICK_THIN_BAR)
            {
            tmp_figure = new thick_thin_bar(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::NTH_ENDING)
            {
            tmp_figure = new nth_ending(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::BEGIN_REPEAT)
            {
            tmp_figure = new begin_repeat(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::END_REPEAT)
            {
            tmp_figure = new end_repeat(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::BEGIN_END_REPEAT)
            {
            tmp_figure = new begin_end_repeat(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::CUT_TIME)
            {
            tmp_figure = new cut_time_figure(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::COMMON_TIME)
            {
            tmp_figure = new common_time_figure(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::GRACING_FIGURE)
            {
            tmp_figure = new gracing_figure(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::ROLL_SIGN)
            {
            tmp_figure = new roll_sign(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::SLIDE_FIGURE)
            {
            tmp_figure = new slide_figure(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::ACCENT_FIGURE)
            {
            tmp_figure = new accent_figure(the_screen_ppi);
            }
        else if (the_figure_id == figure_factory::FERMATA_FIGURE)
            {
            tmp_figure = new fermata_figure(the_screen_ppi);
            }        
        else if (the_figure_id == figure_factory::DOWN_BOW_FIGURE)
            {
            tmp_figure = new down_bow_figure(the_screen_ppi);
            }        
        else if (the_figure_id == figure_factory::UP_BOW_FIGURE)
            {
            tmp_figure = new up_bow_figure(the_screen_ppi);
            }        
        else if (the_figure_id == figure_factory::MORDANT)
            {
            tmp_figure = new mordant_figure(the_screen_ppi);
            }        
        else if (the_figure_id == figure_factory::TRILL)
            {
            tmp_figure = new trill_figure(the_screen_ppi);
            }        
        else
            {
            tmp_figure = 0;
            }
        if (tmp_figure)
        {
			tmp_figure->add_ref();
			tmp_figure->rescale(the_scale);
            figure_map.add_pair(figure_ordinal(the_figure_id,tmp_real_scale),tmp_figure);
        }
    }

    return tmp_figure;
}

figure* 
figure_factory::get_figure(FIGURE_ENUM the_enum,const scale& the_scale)
{
    lock tmp_lock(figure_factory::figure_list_mutex);
    map<figure_ordinal,figure*>::iterator tmp_it = 
        figure_map.get_item(figure_factory::figure_ordinal(the_enum,the_scale),
                         exact);

    if (tmp_it)
    {
        return (*tmp_it).value;
    }
    return 0;
}

}

