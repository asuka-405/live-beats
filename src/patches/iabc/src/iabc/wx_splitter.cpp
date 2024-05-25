#include "iabc/wx_splitter.h"
#include "iabc/event_log.h"
namespace iabc
{
;
wx_splitter::wx_splitter(wx_splitter_notifier& the_notifier,
                         wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
:wxSplitterWindow(parent,id,pos,size,style,name),
    my_notifier(the_notifier)
{};

bool 
wx_splitter::OnSashPositionChange(int newSashPosition)
{

    double tmp_sash = 0.0;
    if (newSashPosition > 0) 
    {
        int tmp_width = GetSize().GetWidth();
        tmp_sash = (double)newSashPosition / (double)tmp_width;
        global_windows_event_log.log_event(windows_event_data::move_sash,
                                               (unsigned long)this,(unsigned long)(tmp_sash * 100.0),
                                               (unsigned long)tmp_width,(unsigned long)newSashPosition);
    }

    my_notifier.notify_sash_change(tmp_sash);
    return true;
}

}

