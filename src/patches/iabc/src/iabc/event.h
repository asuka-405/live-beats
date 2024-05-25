#ifndef iabc_event
#define iabc_event
#include "iabc/list.h"
#include "iabc/mutex.h"
#include "iabc/semaphore.h"
#include "iabc/event_log.h"

// FILE: event.h
// DESCRIPTION:
// Classes to handle events safely and efficiently
namespace iabc
{
class event_source;
// CLASS: event_handler_if
// DESCRIPTION:
// The interface to an event handler, that an event
// source knows about.  This breaks up the circular
// dependency between source and handler using the
// obserer pattern.
class event_handler_if
{
    friend class event_source;
public:
    class HandleException{};
    event_handler_if(){++class_instance_count;};
    
    // METHOD: Add/Remove Ref
    // DESCRIPTION:
    // Event Handlers and event sources need to keep reference
    // counts to each other very carefully.
    void event_handler_add_ref(){++my_ref;};
    void event_handler_remove_ref(){if (!--my_ref) delete this;};

    // METHOD: handle_event
    // DESCRIPTION:
    // A source object call this instance of a handler that
    // it knows about.
    virtual void handle_event() = 0;
protected:
    virtual ~event_handler_if(){--class_instance_count;};
    static int class_instance_count;
    atomic_counter my_ref;
};

// CLASS: EventSource
// DESCRIPTION:
// A class that represents an event that could occur.  It is capable
// of notifying  event handlers that the event has occured.
class event_source
{
public:
    event_source(){++class_instance_count;};
    void event_source_add_ref(){++my_count;};
    void event_source_remove_ref(){if (!--my_count) delete this;};
    
    // METHOD: add_event_handler
    // DESCRIPTION:
    // An event handler wants to be informed if this event fires;
    // Add it to the list of events we notify in the event of an event.
    void add_event_handler(event_handler_if& the_event);
    
    // METHOD: RemoveEventHandler
    // DESCRIPTION:
    // The event handler no longer cares about this event.  Remove it from
    // the list.
    void remove_event_handler(event_handler_if& the_event);
    
    // METHOD: NotifyHandlers
    // DESCRIPTION:
    // The event has occured!  Notify all event handlers that the event has
    // occured.
    void notify_handlers();
protected:
    // METHOD: pre/post add_event_handler
    // DESCRIPTION:
    // Before and after the event handler adds the event, there may be special things
    // that we need to do, like set a timer in the case of a timer for example.
    virtual void pre_add_event_handler(){};
    virtual void post_add_event_handler(){};
	
    // METHOD: AddEventHandlerToList
    // DESCRIPTION:
    // Protected method that actually adds the handler to the linked list.
    void add_event_handler_to_list(event_handler_if& the_event);
    virtual ~event_source(){--class_instance_count;};
    mutex my_mutex;
private:
    // METHOD: PopulateHandlerList
    // DESCRIPTION:
    // We need threadsafe access to our internal list, but we can't block while
    // we're notifying events.  So we just deep copy our internal list and we don't
    // have to worry about the list changing underneath us.
    list<event_handler_if*>::iterator populate_handler_list();
    
    list<event_handler_if*> my_list;
    atomic_counter my_count;
    static int class_instance_count;
  };

// CLASS: EventHandler
// DESCRIPTION:
// An event handler can block on events until they occur.  Many event
// handlers can be or'd together so that we can wait on the first of many
// things at once.
class event_handler:public event_handler_if
{
public:
    event_handler();

    ~event_handler(){};
    
    // METHOD: operator|
    // DESCRIPTION:
    // Add the_other to the list of events in this event
    // chain.
    event_handler& operator|(event_handler& the_other);
    
    // METHOD: wait
    // DESCRIPTION:
    // Set the event source for all the events in the chain, and
    // then pend on the semaphore that indicates that the event has
    // occured.
    static void wait(event_handler& the_event);
    bool has_fired(){return my_fired;};

protected:
    // METHOD: get_event_source
    // DESCRIPTION:
    // This is the part that must be derived.  Each instance of an event
    // handler should know how to get an instance of the source that it
    // can wait on.
    virtual event_source* get_event_source() = 0;

    // METHOD: handle_event
    // DESCRIPTION:
    // This is called when the event source fires.  The event chain will
    // then be torn down and the thread that was waiting on the event will
    // be allowed to continue.
    void handle_event();
    
    // METHOD: Rearm
    // DESCRIPTION:
    // Allow an event to be reused.
    void rearm(){my_fired = false;my_armed = true;};
    
    // ATTRIBUTE: my_source
    // DESCRIPTION:
    // This is the actual source that I am waiting on.
    event_source* my_source;
private:
    // METHOD: set_event_source
    // DESCRIPTION:
    // The generic logic that calls GetEventSource.  An excpetion
    // is thrown here if the event source already exists.
    void set_event_source();
    
    // METHOD: add_handler_to_chain
    // DESCRIPTION:
    // the_next is an event handler in the chain.  This is used\
    // when setting up the 'or' chain.
    void add_handler_to_chain(event_handler& the_next);
    
    // METHOD: pass_event_to_first
    // DESCRIPTION:
    // Only one event in a chain is the 'first', which is the one
    // that ends on blocking.  Make sure we always release the block
    // from that point to assure deterministic behavior.
    void pass_event_to_first();
    
    // METHOD:
    // tear_down_chain
    // DESCRIPTION:
    // The pending has ended.  The event has fired.  Tear down the event
    // chain.
    void tear_down_chain();

    // METHOD: zero/add/remove Refs
    // DESCRIPTION:
    // When handling events in chains, the actual values can be zeroed
    // out underneath us when the chain is released.  Prevent the events
    // and sources from being released prematurely.
    void zero_all_refs();
    void add_all_refs(event_handler*& tmp_last, 
                      event_handler*& tmp_next, 
                      event_source*& tmp_source);
    void remove_all_refs(event_handler* tmp_last,
                         event_handler* tmp_next,
                         event_source* tmp_source);
    
    // ATTRIBUTE: my_sem
    // DESCRIPTION:
    // The semaphore that we are pending on.
    semaphore my_sem;
    
    // ATTRIBUTE: my_next/last
    // DESCRIPTION:
    // The other events in the chain.
    event_handler* my_next;
    event_handler* my_last;
	mutex my_mutex;
    bool my_fired;
    bool my_armed;
};

class sticky_event_source:public event_source
{
public:
    static sticky_event_source* create(){return new sticky_event_source();};
    void set(){my_set = true;notify_handlers();};
    void clear(){my_set = false;};
protected:
    sticky_event_source():my_set(false){};
    virtual ~sticky_event_source(){};
    virtual void post_add_event_handler(){if (my_set) notify_handlers();};
private:
    bool my_set;
};

class sticky_event_handler:public event_handler
{
public:
    static sticky_event_handler* 
      create(sticky_event_source& the_source){return new sticky_event_handler(the_source);};
    virtual event_source* get_event_source(){return &my_sticky_source;};
protected:
    ~sticky_event_handler(){my_sticky_source.event_source_remove_ref();};
    sticky_event_handler(sticky_event_source& the_source):my_sticky_source(the_source)
        {my_sticky_source.event_source_add_ref();};
    void clear(){my_sticky_source.clear();rearm();};

private:
    sticky_event_source& my_sticky_source;

};
}
#endif


