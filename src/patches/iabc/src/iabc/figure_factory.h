#ifndef iabc_figure_factory_h
#define iabc_figure_factory_h
#include "iabc/figure.h"
#include "iabc/factory.h"
#include "iabc/map.h"
#include "iabc/mutex.h"
#ifdef _DEBUG
#define figure_ordinal fo_
#endif

namespace iabc
{
;

// CLASS: figure_factory
// DESCRIPTION:
// Create and manage figures that are fixed.  that is,
// they have the same set of points regardless of where the
// center of the figure is.

class figure_factory
{
public:
    static void gc();
    typedef enum FIGURE_ENUM
    {
        GCLEF,
        FCLEF,
        STAFF,
        WHOLE_NOTE,
        OPEN_HEAD,
        CLOSED_HEAD,
        DOT,
        UP_STEM,
        DOWN_STEM ,
        SHARP_SIGN,
        FLAT_SIGN,
        NATURAL_SIGN,
        SINGLE_BAR,
        LEDGER_LINE,
        QUARTER_REST,
        WHOLE_REST,
        HALF_REST,
        EIGHTH_REST,
        SIXTEENTH_REST,
        THIN_THIN_BAR,
        THIN_THICK_BAR,
        THICK_THIN_BAR,
        NTH_ENDING,
        BEGIN_REPEAT,
        END_REPEAT,
        BEGIN_END_REPEAT,
        COMMON_TIME,
        CUT_TIME,
        GRACING_FIGURE,
        ROLL_SIGN,
        SLIDE_FIGURE,
        ACCENT_FIGURE,
        FERMATA_FIGURE,
        DOWN_BOW_FIGURE,
        UP_BOW_FIGURE,
        MORDANT,
        TRILL
    } FIGURE_ENUM;
    
    class figure_ordinal
    {
    public:
        FIGURE_ENUM my_figure_id;
        scale my_scale;
        figure_ordinal():my_figure_id(GCLEF){};
        figure_ordinal(FIGURE_ENUM the_figure,
                       const scale& the_scale):
            my_scale(the_scale),my_figure_id(the_figure){};
        figure_ordinal(const figure_ordinal& o)
        {
            my_scale = o.my_scale;
            my_figure_id = o.my_figure_id;
        };
        figure_ordinal& operator=(const figure_ordinal& o)
        {
            my_scale = o.my_scale;
            my_figure_id = o.my_figure_id;
            return *this;
        };
        bool operator==(const figure_ordinal& o) const
        {
            return ((my_figure_id == o.my_figure_id) &&
                    (my_scale == o.my_scale));
        };
        bool operator>(const figure_ordinal& o) const
        {
            return ((my_figure_id > o.my_figure_id) ||
                    ((my_figure_id == o.my_figure_id) &&
                     (my_scale.x > o.my_scale.x)) ||
                    ((my_figure_id == o.my_figure_id) &&
                     (my_scale.x == o.my_scale.x) &&
                     (my_scale.y > o.my_scale.y)));
        };

        bool operator>=(const figure_ordinal& o) const
        {
            return ((*this == o) || (*this > o));
        };
        bool operator<(const figure_ordinal& o) const
        {
            return ((*this >= o) == false);
        };
        bool operator<=(const figure_ordinal& o) const
        {
            return ((*this < o) || (*this == o));
        };

    };

    figure* get_figure(const size& the_screen_ppi,
                       figure_factory::FIGURE_ENUM,
                       const scale& the_scale = scale(1.0,1.0));
    ~figure_factory();
    static figure_factory& get_instance();
protected:
    figure* get_figure(FIGURE_ENUM the_enum,const scale& the_scale);
    static globalton<figure_factory> force_init;
    typedef list<kv_pair <figure_ordinal,figure*> > figure_list;
    map<figure_ordinal,figure*> figure_map;
    figure_factory(){};
    mutex figure_list_mutex;
};
}
#endif



