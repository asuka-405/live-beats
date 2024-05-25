#include "iabc/media.h"
#include <stdio.h>
namespace iabc
{
;

class message_box
{
public:
    static void display(const string& the_string);
};

class null_media:public media
{
public:
    null_media(){};
	bool setup(){return true;};
	void complete(){};
protected:
    // METHOD: render_music_feature
    // DESCRIPTION:
    // The base class overrides this method to actually present the end of the measure
    // to the user
    virtual void render_measure_feature(const measure_feature& the_feature, const score_point &the_point)
    {
        switch (the_feature.end_type())
        {
        case measure_feature::double_bar:
            printf(" double bar ");
            break;
        case measure_feature::single_bar:
            printf(" line break\n");
            break;
        default:
            printf(" repeat ");
            break;

        }
    }
    // METHOD: render_music_feature
    // DESCRIPTION:
    // The base class overrides this method to actually present the music to the user
    virtual void render_music_feature(const chord_info& the_feature, const score_point &the_point) 
    {
        printf(" %s ",(the_feature).as_string().access_char_array());
    }
};
}


