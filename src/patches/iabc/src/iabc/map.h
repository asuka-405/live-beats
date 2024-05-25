#ifndef iabc_map_h
#define iabc_map_h
#include "iabc/list.h"
#pragma warning ( disable : 4786)

namespace iabc
{
;

//CLASS: kv_pair
// DESCRIPTION:
// A key-value pair template class.  The key is some type of 
// ordinal type, and the value is something associated with the key.
// It's used as an element of a map.
template <class T1,class T2>
class map_iterator;
template <class T1,class T2>
class map_data;
template <class T1,class T2>
class map;

template <class T1,class T2>
class kv_pair
{
public:
    friend class map_data<T1,T2>;
    friend class map_iterator<T1,T2>;
    T1 key;
    T2 value;
    kv_pair(const T1& the_key,const T2& the_value):
        key(the_key),value(the_value),my_next(0),my_last(0){};
    kv_pair():my_next(0),my_last(0){};
    kv_pair(const kv_pair& o):
        key(o.key),value(o.value),my_next(o.my_next),my_last(o.my_last){};
    kv_pair& operator=(const kv_pair& o){
        key = o.key;value = o.value;my_next=o.my_next;my_last=o.my_last;return *this;};
    ~kv_pair(){};
private:
    kv_pair* my_next;
    kv_pair* my_last;
};

// CLASS: list_data
// DESCRIPTION:
// list_data does all the work of the container interface for lists.
template <class T1,class T2>
class map_data
{
  public:
      ~map_data ();
      int get_size() const{return my_size;};
      void add_ref() {++my_ref;};
      void remove_ref() {if (--my_ref <= 0) delete this;};
      bool is_end(kv_pair<T1,T2>& the_node) const {return (my_head == &the_node);};
      // kv_pair* get_head ();
protected:
  private:
      map_data ();
      void insert_after (kv_pair<T1,T2>* the_old, kv_pair<T1,T2>* the_new);
      void insert_before (kv_pair<T1,T2>* the_old, kv_pair<T1,T2>* the_new);
      void remove (kv_pair<T1,T2>* the_node);
      
      map_data(const map_data< T1,T2 > &right);
      map_data& operator=(const map_data< T1,T2 > &right);
      
      kv_pair<T1,T2>* my_head;
      int my_size;
      int my_ref;
      friend class map<T1,T2>;
      friend class map_iterator<T1,T2>;
      static int my_instances;
};

// CLASS: map_match_type
typedef enum map_match_type
{
    exact = 0,
    lt ,
    lteq,
    gt,
    gteq
} map_match_type;

// CLASS: map_equality_pred
// DESCRIPTION:
// Create a predicate for map searches.
template<class ordinal,class T>
class map_equality_pred
{
 public:
typedef kv_pair<ordinal,T> value_pair;
  map_equality_pred(const ordinal& the_key,map_match_type the_match):
    my_key(the_key),my_match(the_match){};
  bool operator()(const value_pair& the_item);
  ordinal my_key;
  map_match_type my_match;
};

// CLASS: map_iterator
// DESCRIPTION:
// Allow a user to iterate through a map from some start point
// to some end point, using the get_item method of map.
template <class ordinal,class T>
class map_iterator
{
public:
    friend class map<ordinal,T>;

    // This saves us some typing later
	typedef kv_pair<ordinal,T> value_pair;
    typedef typename list<value_pair>::iterator iterator;
    typedef map_equality_pred<ordinal,T> map_pred;
    
    map_iterator();
    ~map_iterator();

    // Copy ctor, copy my list to o's list
    map_iterator(const map_iterator& o);

    // Copy ctor, copy my list to o's list
    map_iterator& operator=(const map_iterator& o);
    
    void next();
    void previous();
    map_iterator& most();
    map_iterator& least();

    // METHOD: get_item
    // DESCRIPTION:
    // get the item that maps to the_key, or return a NULL iterator.
    map_iterator
         get_item(const ordinal& the_key,map_match_type the_match = exact);

    void add_before (const ordinal& o,const T& t);
    void add_after (const ordinal& o,const T& t);
    void delete_current();
    void reset_to_front();
    void reset_to_back();
    long get_size() const;
    bool is_end() const;

    // METHOD: operator*
    // DESCRIPTION:
    // Acts like a pointer dereference.  Throws excepction if the list is
    // empty.  Returns a reference.
    value_pair& operator* ();

    // METHOD: operator* const
    // DESCRIPTION:
    // Acts like a pointer dereference.  Throws excepction if the list is
    // empty.  Returns a copy.
    value_pair operator*  () const;

    // METHOD: T*
    // DESCRIPTION:
    // Allow the list to be checked for validity in the pointer way, 
    // e.g.:
    // if (my_iterator) {       // check for end of list.
    // tmp_data = *my_iterator; // now this is safe
    operator value_pair* ();
    operator const value_pair*() const;
private:
    map_data<ordinal,T>* my_data;
    kv_pair<ordinal,T>* my_current;
};

// CLASS: map
// DESCRIPTION:
// If there's a place you need to go, it'll get you there I know.
// Map a set of keys to a set of values.
// 
// There's a couple of things that make this class special.  First off,
// the map follows a 'worst possible' matching algorithm, which means
// for example, that the lt (less-than) match type will always return the greatest
// ordinal for which the lt constraint holds.
// Second, the map contains an iterator, which always points to the last
// thing selected.  This means that copying one map to another will be an
// O(n) operation.  Frequently the map iterator will be used instead of creating
// a seperate iterator class.
template <class ordinal,class T>
class map
{
public:
    friend class map_iterator<ordinal,T>;
    // METHOD: get_iterator
    // DESCRIPTION:
    // Allow the client to iterate through the list.  I'm not sure that I like
    // this since it allows the user to delete an item and not my_last_cursor.
    // But it allows the user to easily dereference all the pointers from a map.
    // The assumption is that this is only used before the list is destroyed.
    // list<kv_pair <ordinal,T> >::iterator
    //     get_iterator(){return (my_list.first());}

    // Creates a new map
    map();

    // Deletes the map
    ~map();

    // Copy ctor, copy my list to o's list
    map(const map& o);

    // Copy ctor, copy my list to o's list
    map& operator=(const map& o);

    // This saves us some typing later
    typedef map_iterator<ordinal,T> iterator;
    typedef map_equality_pred<ordinal,T> map_pred;
    
    // METHOD: add_pair
    // DESCRIPTION:
    // Add a new pair to the map, with the_key mapping to the_item
    void add_pair(const ordinal& the_key,const T& the_item);

    // METHOD: get_item
    // DESCRIPTION:
    // get the item that maps to the_key, or return a NULL iterator.
    map_iterator<ordinal,T>
         get_item(const ordinal& the_key,map_match_type the_match = exact);

    // METHOD: least
    // DESCRIPTION:
    // get the item that maps to lowest ordinal in the map.
    map_iterator<ordinal,T>
         least();
    
    // METHOD: most
    // DESCRIPTION:
    // Get the item that maps to the greatest ordinal in the map.
    map_iterator<ordinal,T>
         most();

    // METHOD: remove
    // DESCRIPTION:
    // Safely remove an item.
    void remove(const ordinal& the_key,map_match_type the_match = exact);

    // METHOD: clear
    // DESCRIPTION:
    // Clear out all the element in the map.
    void clear();

    // METHOD: get_size()
    // DESCRIPTION:
    // Get the size of the list of keys/values
    int get_size() const { return my_last_cursor.get_size();};

private:
    static map_iterator<ordinal,T> 
    findit(const ordinal& the_key,map_match_type the_match,
                           map_iterator<ordinal,T>& the_cursor);
    map_iterator<ordinal,T> my_last_cursor;

    // METHOD: init_my_current
    // DESCRIPTION:
    // It is possible for this class to be populated entirely by a copy of this
    // map class, in which case my iterator's my_current pointer will be null.
    // This is never a valid case since even an empty list has the seed element.
    // So perform this initializtion one time if required.
    void init_my_current();
};

// METHOD: get_inverse_map
// DESCRIPTION:
// Return map<key,value> give map<value,key>.  Note that
// not all maps will have an inverse.
template <class T1,class T2>
extern map<T1,T2> get_inverse_map(map<T2,T1>&);

template <class ordinal,class T,class predicate>
extern map_iterator<ordinal,T>
findit_forwards(map_iterator<ordinal,T>& the_list,predicate& the_predicate);

template <class ordinal,class T,class predicate>
extern map_iterator<ordinal,T>
findit_backwards(map_iterator<ordinal,T>& the_list,predicate& the_predicate);

template <typename ordinal,typename map_type,typename T>
extern T& get_map_value(const ordinal& o,map_type& the_map);

template <typename ordinal,typename map_type,typename T>
extern void do_to_all(map<ordinal,T>& the_map);
}
#endif

