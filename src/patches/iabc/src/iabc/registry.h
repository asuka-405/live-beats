#ifndef iabc_registry_h
#define iabc_registry_h

#include "iabc/factory.h"
#include "iabc/string.h"
#include "iabc/map.h"
#include "iabc/scan.h"
#include "iabc/parse.h"
#include "iabc/mutex.h"

namespace iabc
{
;
// FILE: registry.h
// DESCRIPTION:
// Classes that implement a simple platform-independent
// persistent registry for 
// a program, that manage a list of name-value pairs
// from a .ini file.  This can be used to store preferences,
// program state, etc.
// The form of the registry is:
// [category]
// key1=value1
// key2=value2
// key3.subkey = value3
//
// The subkey is optional, and there can be only one level
// of subkey per key, unlike e.g. .XDefaults files.

// CLASS: registry_key
// DESCRIPTION:
// A registry key is a set of strings that can be associated
// with a value.  The strings are arranged in a heirarchy so that
// we can sort them and peruse them.
class registry_key
{
public:
    // METHOD: ctor, no subkey
    registry_key(const string& the_category,const string& the_key);
    // METHOD: ctor, no key and subkey
    registry_key(const string& the_category,const string& the_key,const string& the_subkey);
    // METHOD: op=, deep copy
    registry_key& operator=(const registry_key& o);
    // METHOD: copy ctor, deep copy
    registry_key(const registry_key& o);
    // METHOD: default ctor, emptry string values.
    registry_key();
    ~registry_key();
    bool operator > (const registry_key& o) const;
    bool operator < (const registry_key& o) const;
    bool operator >= (const registry_key& o) const;
    bool operator <= (const registry_key& o) const;
    bool operator== (const registry_key& o) const;
    bool operator!= (const registry_key& o) const;
    // METHOD: get_subkey_from_string
    // DESCRIPTION:
    // Parse the string to split on the '.', which allows entries like: LASTFILE.1=foo.abc
    static string get_subkey_from_string(const string& the_string);
    static string get_key_from_string(const string& the_string);
    string category;
    string key;
    string subkey;
};

// CLASS: registry_entry
// DESCRIPTION:
// registry_entry is a registry_key + a string value.
// An entry can be put into the registry and looked up later,
// based on the key.
class registry_entry
{
public:
    // METHOD: registry_entry
    // DESCRIPTION:
    // construct a registry entry based on the strings.
    registry_entry(const string& the_category,
                   const string& the_key,
                   const string& the_subkey,
                   const string& the_value,
                   bool the_is_persistent = false);
    // METHOD: registry_entry
    // DESCRIPTION:
    // construct a registry entry based on the key an a value.
    registry_entry(const registry_key& the_key,
                   const string& the_value,
                   bool the_is_persistent = false);
    // METHOD: registry_entry
    // DESCRIPTION:
    // construct an empty registry entry.
    registry_entry();
    // METHOD: copy ctor
    registry_entry(const registry_entry& o);
    // METHOD: op= deep copy
    registry_entry& operator=(const registry_entry& o);
    // METHOD: op=, value only
    // DESCRIPTION:
    // change the value of the existing key.
    registry_entry& operator=(const string& the_value)
    {
        value = the_value;
        return *this;
    };
    registry_key get_key() const {return key;};
    string get_value() const {return value;};
    // METHOD: is_persistent
    // DESCRIPTION:
    // Indicate whether or not this registry entry should be written to disk 
    // when a flush() is performed on the registry.
    bool is_persistent() const {return persistent;};
    void make_persistent() {persistent = true;};
private:
    registry_key key;
    string value;
    bool persistent;
};

// This is a pretty inefficient way to store this registry as you
// end up storing the key twice.  But it is easy for now.
typedef map<registry_key,registry_entry> registry_map;

// CLASS: registry
// DESCRIPTION:
// Allow the user toe store key/value pairs persistently between releases.
// It is similary to a windows .ini file, hence the name, but it works cross-
// platforms.
class registry
{
public:
    // METHOD: registry
    // DESCRIPTION:
    // Create a new registry instance, and initialize it with values from
    // the_filename.  Future flushes of this buffer will also be written to
    // this filename.
    registry(const char* the_filename);

    // METHOD: get_entry
    // DESCRIPTION:
    // look up the registry and return the value associated with the given key,
    // or an empty string if no such value is found.
    registry_entry get_entry(const registry_key& the_key);

    // METHOD: get_entry
    // DESCRIPTION:
    // look up the registry and add or replace the given entry.
    bool set_value(const registry_entry& the_entry);

    // METHOD: flush
    // DESCRIPTION:
    // write all values marked as persistent to the file
    void flush();
    static registry& get_instance();
    static void gc();
private:
    registry();
    registry(const registry& r);
    registry& operator=(const registry& r);
    void read_file();
    mutex my_mutex;
    string my_filename;
    regexp my_category;
    regexp my_pair;
    registry_map my_map;
};

extern registry& GetRegistryInstance();
#define globalRegistry GetRegistryInstance()
}

#endif

