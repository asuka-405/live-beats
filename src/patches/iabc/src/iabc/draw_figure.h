#ifndef iabc_draw_figure_h
#define iabc_draw_figure_h
#include "iabc/figure.h"
#define STEM_LENGTH 2000

namespace iabc
{
        
// These classes all specialize figures that are always the same,
// like clefs and flats.  Most of the drawing is derived from pslib.c
// from abc_to_ps series of programs.
class staff_figure:public scaled_figure
{
public:
    staff_figure(const size& the_ppi):scaled_figure(the_ppi){};
    virtual ~staff_figure();
protected:
    virtual void create_poly(polygon& poly);
};

class ledger_line:public scaled_figure
{
public:
    ledger_line(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);
};

class draw_command<class T,class G>;

class gclef:public figure
{
public:
    gclef(const size& the_ppi):figure(the_ppi){};
    virtual ~gclef();

protected:
    virtual void create_poly(polygon& poly);
};

class fclef:public figure
{
public:
    fclef(const size& the_ppi):figure(the_ppi){};
    virtual ~fclef(){};
protected:
    virtual void create_poly(polygon& poly);
};

class open_head:public figure
{
public:
    open_head(const size& the_ppi):figure(the_ppi){};
    virtual ~open_head(){};

protected:
    virtual void create_poly(polygon &poly);
};

class closed_head:public figure
{
public:
    closed_head(const size& the_ppi):figure(the_ppi){my_debug_string="closed_head";};
    virtual ~closed_head(){};

protected:
    virtual void create_poly(polygon &poly);
};

class dot:public figure
{
public:
    // derivation draws the thing here.
    dot(const size& the_ppi):figure(the_ppi){};
protected:
    virtual void create_poly(polygon &poly);
};
class up_stem:public scaled_figure
{
public:
    up_stem(const size& the_ppi):scaled_figure(the_ppi){};
    virtual ~up_stem(){};
protected:
    virtual void create_poly(polygon &poly);
};

class down_stem:public scaled_figure
{
public:
    down_stem(const size& the_ppi):scaled_figure(the_ppi){};
    virtual ~down_stem(){};
protected:
    virtual void create_poly(polygon &poly);
};

class sharp_sign:public scaled_figure
{
public:
    sharp_sign(const size& the_ppi):scaled_figure(the_ppi){my_debug_string = "sharp_sign";};
    virtual ~sharp_sign(){};
protected:
    virtual void create_poly(polygon &poly);
};

class flat_sign:public scaled_figure
{
public:
    flat_sign(const size& the_ppi):scaled_figure(the_ppi){};
    virtual ~flat_sign(){};
protected:
    virtual void create_poly(polygon &poly);
};

class natural_sign:public figure
{
public:
    natural_sign(const size& the_ppi):figure(the_ppi){};
    virtual ~natural_sign(){};
protected:
    virtual void create_poly(polygon &poly);
};

class single_bar:public scaled_figure
{
public:
    // derivation draws the thing here.
    single_bar(const size& the_ppi):scaled_figure(the_ppi){};
protected:
    virtual void create_poly(polygon &poly);
};

class quarter_rest:public figure
{
public:
    quarter_rest(const size& the_ppi):figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};

class half_rest:public scaled_figure
{
public:
    // derivation draws the thing here.
    half_rest(const size& the_ppi):scaled_figure(the_ppi){};
protected:
    virtual void create_poly(polygon &poly);
};
class eighth_rest:public figure
{
public:
    eighth_rest(const size& the_ppi):figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};
class sixteenth_rest:public figure
{
public:
    sixteenth_rest(const size& the_ppi):figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};

class whole_note:public figure
{
public:
    whole_note(const size& the_ppi):figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};

class whole_rest:public scaled_figure
{
public:
    whole_rest(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};

class nth_ending:public scaled_figure
{
public:
    nth_ending(const size& the_ppi,const scale& the_scale = scale(1.0,1.0));
    nth_ending(const size& the_ppi,const scale& the_scale,const size& the_size);
    // derivation draws the thing here.
protected:
    void set_xform(polygon& poly);
    virtual void create_poly(polygon &poly);
    size my_size_in_pixels;
};

class thin_thin_bar:public scaled_figure
{
public:
    thin_thin_bar(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};
class thin_thick_bar:public scaled_figure
{
public:
    thin_thick_bar(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};
class thick_thin_bar:public scaled_figure
{
public:
    thick_thin_bar(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};
class begin_repeat:public scaled_figure
{
public:
    begin_repeat(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};
class end_repeat:public scaled_figure
{
public:
    end_repeat(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};
class begin_end_repeat:public scaled_figure
{
public:
    begin_end_repeat(const size& the_ppi):scaled_figure(the_ppi){};
    // derivation draws the thing here.
protected:
    virtual void create_poly(polygon &poly);

};

class common_time_figure:public scaled_figure
{
public:
    common_time_figure(const size& the_ppi):scaled_figure(the_ppi){};
protected:
    virtual void create_poly(polygon &poly);
};

class cut_time_figure:public scaled_figure
{
public:
    cut_time_figure(const size& the_ppi):scaled_figure(the_ppi){};
protected:
    virtual void create_poly(polygon &poly);
};

class gracing_figure:public scaled_figure
{
public:
    gracing_figure(const size& the_ppi):scaled_figure(the_ppi){};
protected:
    virtual void create_poly(polygon &poly);
};

class roll_sign:public scaled_figure
{
public:
    roll_sign(const size& the_ppi):scaled_figure(the_ppi){};

protected:
    virtual void create_poly(polygon &poly);
};

class slide_figure:public scaled_figure
{
public:
    slide_figure(const size& the_ppi):scaled_figure(the_ppi){};

protected:
    virtual void create_poly(polygon &poly);
};

class accent_figure:public scaled_figure
{
public:
    accent_figure(const size& the_ppi):scaled_figure(the_ppi){};

protected:
    virtual void create_poly(polygon &poly);
};

class fermata_figure:public figure
{
public:
    fermata_figure(const size& the_ppi):figure(the_ppi){};

protected:
    virtual void create_poly(polygon &poly);
};

class down_bow_figure:public scaled_figure
{
public:
    down_bow_figure(const size& the_ppi):scaled_figure(the_ppi){};

protected:
    virtual void create_poly(polygon &poly);
};

class up_bow_figure:public scaled_figure
{
public:
    up_bow_figure(const size& the_ppi):scaled_figure(the_ppi){};

protected:
    virtual void create_poly(polygon &poly);
};

class mordant_figure:public scaled_figure
{
public:
    mordant_figure(const size& the_ppi):scaled_figure(the_ppi){};

protected:
    virtual void create_poly(polygon &poly);
};

class trill_figure:public scaled_figure
{
public:
    trill_figure(const size& the_ppi):scaled_figure(the_ppi){};
protected:
    virtual void create_poly(polygon& poly);
};

class crescendo_figure:public scaled_figure
{
    // METHOD: create_poly
    // DESCRIPTION:
    // Derivation draws the thing here.
public:
    crescendo_figure(const size& the_size,int the_width);
    ~crescendo_figure(){};
    virtual void create_poly(polygon &poly);
    virtual void set_xform(polygon& poly);
private:
    int my_width;
    int my_depth;
};

class decrescendo_figure:public scaled_figure
{
    // METHOD: create_poly
    // DESCRIPTION:
    // Derivation draws the thing here.
public:
    decrescendo_figure(const size& the_size,int the_width);
    ~decrescendo_figure(){};
    virtual void create_poly(polygon &poly);
    virtual void set_xform(polygon& poly);
private:
    int my_width;
    int my_depth;
};

class box_figure:public scaled_figure
{
public:
    box_figure(const size& the_ppi,const size& the_size);
    ~box_figure();
    // METHOD: set_xform
    // DESCRIPTION:
    // Set the scale of the drawing surface.  Many of the drawing routines
    // we have use twips in a postscript-like coordinate system, but some
    // are in absolute pixels.  The default behavior is the postscript like
    // scaling.
protected:
    virtual void set_xform(polygon &poly);
    // METHOD: create_poly
    // DESCRIPTION:
    // Derivation draws the thing here.
    virtual void create_poly(polygon &poly);
    size my_size;
};

class long_rest_figure:public scaled_figure
{
public:
    long_rest_figure(const size& the_ppi,const size& the_size);
    ~long_rest_figure();
    // METHOD: set_xform
    // DESCRIPTION:
    // Set the scale of the drawing surface.  Many of the drawing routines
    // we have use twips in a postscript-like coordinate system, but some
    // are in absolute pixels.  The default behavior is the postscript like
    // scaling.
protected:
    virtual void set_xform(polygon &poly);
    // METHOD: create_poly
    // DESCRIPTION:
    // Derivation draws the thing here.
    virtual void create_poly(polygon &poly);
    size my_size;
};

}
#endif



