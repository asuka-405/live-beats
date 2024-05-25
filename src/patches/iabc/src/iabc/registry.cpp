
#include "iabc/registry.h"

#include "iabc/scan.h"
#include "iabc/parse.h"
#include "iabc/event_log.h"

#include <stdio.h>
#ifdef _MSC_VER
#include <direct.h>
#else
#define _getcwd getcwd
#include <unistd.h>
#endif

#include "iabc/map.cpp"

namespace iabc
{
;
registry_key::registry_key(const string& the_category,const string& the_key)
{
    category = the_category;
    subkey = get_subkey_from_string(the_key);
    key = get_key_from_string(the_key);
}

registry_key::registry_key(const string& the_category,const string& the_key,const string& the_subkey)
{
    category = the_category;
    subkey = the_subkey;
    key = the_key;
}

    
registry_key& 
registry_key::operator=(const registry_key& o)
{
    key = o.key;
    subkey = o.subkey;
    category = o.category;
    return *this;
}

registry_key::registry_key(const registry_key& o)
{
    key = o.key;
    subkey = o.subkey;
    category = o.category;
}

registry_key::registry_key()
{
}

registry_key::~registry_key()
{
}

bool 
registry_key::operator > (const registry_key& o) const
{
    bool tmp_rv = false;
    if (category > o.category)
    {
        tmp_rv = true;
    }
    else if ((category == o.category) &&
             (key > o.key))
    {
        tmp_rv = true;
    }
    else if ((category == o.category) &&
             (key == o.key) &&
             (subkey > o.subkey))
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

bool 
registry_key::operator < (const registry_key& o) const
{
    return ((*this != o) && ((*this > o) == false));
}

bool
registry_key::operator >= (const registry_key& o) const
{
    return ((*this < o) == false);
}

bool 
registry_key::operator <= (const registry_key& o) const
{
    return ((*this > o) == false);
}
    
bool 
registry_key::operator== (const registry_key& o) const
{
    return ((category == o.category) &&
            (subkey == o.subkey) && 
            (key == o.key));
}

bool 
registry_key::operator!= (const registry_key& o) const
{
    return ((*this == o) == false);
}

string 
registry_key::get_subkey_from_string(const string& the_string)
{
    string tmp_rv = "";
    regexp tmp_exp(".+\\.(.+)");
    if (tmp_exp.match(the_string.access_char_array(),the_string.length()) == true)
    {
        tmp_rv = tmp_exp[1];
    }
    return tmp_rv;
}

string 
registry_key::get_key_from_string(const string& the_string)
{
    string tmp_rv = the_string;
    regexp tmp_exp("(.+)\\..+");
    if (tmp_exp.match(the_string.access_char_array(),the_string.length()) == true)
    {
        tmp_rv = tmp_exp[1];
    }
    return tmp_rv;
}

registry_entry&
registry_entry::operator=(const registry_entry& o)
{
    key = o.key;
    value = o.value;
    persistent = o.persistent;
	return *this;
}

registry_entry::registry_entry(const string& the_category,
               const string& the_key,
               const string& the_subkey,
               const string& the_value,
               bool the_is_persistent)
{
    key.category = the_category;
    key.key = the_key;
    if (the_subkey.length())
        key.key += (string)"." + the_subkey;
    value = the_value;
    persistent = the_is_persistent;
}

registry_entry::registry_entry(const registry_entry& o)
{
    key = o.key;
    value = o.value;
    persistent = o.persistent;
}

registry_entry::registry_entry(const registry_key& the_key,
               const string& the_value,
               bool the_is_persistent)
{
    key = the_key;
    value = the_value;
    persistent = the_is_persistent;
}

registry_entry::registry_entry()
:persistent(false)
{
}
    
registry&
registry::get_instance()
{return GetRegistryInstance();
}

void registry::gc(){ delete &(GetRegistryInstance());};

registry::registry(const char* the_entry):
    my_filename(the_entry),
    my_category("\\[(\\w+)\\]"),
    my_pair("(.+)=(.+)")
{
    read_file();
}

void 
registry::flush()
{
    lock tmp_lock(my_mutex);
    registry_map::iterator tmp_it = my_map.get_item(registry_key(),gteq);
    string tmp_last_category;
    FILE* tmp_file = fopen(my_filename.access_char_array(),"w");
    while (tmp_file && tmp_it)
    {
        registry_entry tmp_entry = (*tmp_it).value;
        if (tmp_entry.is_persistent() == true)
        {
            if (tmp_entry.get_key().category != tmp_last_category)
            {
                tmp_last_category = tmp_entry.get_key().category;
                fprintf(tmp_file,"[%s]\n",tmp_last_category.access_char_array());
            }
            string tmp_key = tmp_entry.get_key().key;
            string tmp_subkey = tmp_entry.get_key().subkey;
            string tmp_value = tmp_entry.get_value();
            fprintf(tmp_file,"%s",tmp_key.access_char_array());
            if (tmp_subkey.length() > 0)
            {
                fprintf(tmp_file,".%s",tmp_subkey.access_char_array());
            }
            fprintf(tmp_file,"=%s\n",tmp_value.access_char_array());
        }

        tmp_it = my_map.get_item((*tmp_it).key,gt);
    }

    if (tmp_file)
    {
        fclose(tmp_file);
    }
}

registry_entry 
registry::get_entry(const registry_key& the_key)
{
    lock tmp_lock(my_mutex);
    registry_map::iterator tmp_it = my_map.get_item(the_key);
    if (tmp_it)
    {
        return (*tmp_it).value;
    }
    return registry_entry();
}

bool 
registry::set_value(const registry_entry& the_entry)
{
    lock tmp_lock(my_mutex);
    registry_map::iterator tmp_it = 
        my_map.get_item(the_entry.get_key());
    bool tmp_rv = false;
    if (tmp_it)
    {
        (*tmp_it).value = the_entry;
        tmp_rv = true;
    }
	else
	{
		my_map.add_pair(the_entry.get_key(),the_entry);
	}

    return tmp_rv;
}

void
registry::read_file()
{
    lock tmp_lock(my_mutex);
    global_settings_event_log.log_event(my_filename.access_char_array(),
                                         my_filename.length(),
                                         settings_event_data::set_ini_file);
    FILE* tmp_file = fopen(my_filename.access_char_array(),"r");
    int tmp_next;
    int tmp_zero = 0; // For call the non_terminal::match
    string tmp_category;
    string tmp_line;
    while ((tmp_file) && ((tmp_next = fgetc(tmp_file)) >= 0))
    {
        if (tmp_next == '#')
        {
            while ((tmp_next = fgetc(tmp_file) != '\n') &&
                   (tmp_next > (char)0));
        }
        if ((tmp_next == '\n') || (tmp_next == (char)0))
        {
            if ((tmp_line.length() > 2) &&
				(my_category.match(tmp_line.access_char_array(),tmp_line.length()) == true))
            {
                tmp_category = my_category[1];
                my_category.reset();
                tmp_line.clear();
            }
            else if ((tmp_line.length() > 2) &&
				(my_pair.match(tmp_line.access_char_array(),tmp_line.length()) == true))
            {
                string tmp_key = registry_key::get_key_from_string(my_pair[1]);
                string tmp_subkey = registry_key::get_subkey_from_string(my_pair[1]);
                registry_entry tmp_entry(tmp_category,
                                         tmp_key,
                                         tmp_subkey,
                                         my_pair[2],true);
                my_map.add_pair(tmp_entry.get_key(),tmp_entry);
                my_pair.reset();
                tmp_line.clear();
            }
            else
            {
                tmp_line.clear();
            }
        }
        else
        {
            tmp_line += (string)(char)(tmp_next & 0xff);
        }
    }

    if (tmp_file)
    {
        fclose(tmp_file);
    }
}

}
