#ifndef _UTIL_H
#define _UTIL_H

#include <algorithm>
#include <functional>
#include <string>

extern "C" void    logfi(char * fmt, ...);

string str_rem_color(const string target);

template<typename T>
struct LessByPointedToValue //: std::binary_function<T const *, T const *, bool>
{
    bool operator()(T const * x, T const * y) const
    {
//      return less<T>()(*x, *y);
        return *x < *y;
    }
}; 

template<typename T>
struct GreaterByPointedToValue //: std::binary_function<T const *, T const *, bool>
{
    bool operator()(T const * x, T const * y) const
    {
//      return less<T>()(*x, *y);
        return *x > *y;
    }
};   

template<class OutIt>
void split(const std::string& s, const string& sep, OutIt dest)
{
    string::size_type left = s.find_first_not_of( sep );
    string::size_type right = s.find_first_of( sep, left );
    while( left < right )
    {
        *dest = s.substr( left, right-left );
        ++dest;
        left = s.find_first_not_of( sep, right );
        right = s.find_first_of( sep, left );  
    }
}

/*
 * This template class is used to facilitate the lookups into struct containers.
 * By specifying the structure member and what to compare it against, finding the  
 * proper entry is easier
 */ 
template<class Struct, class T>
class field_equal_t
{
    public:
        field_equal_t(T Struct::*field_, const T& cmp_)
            : field(field_), cmp(cmp_)
        {}

        bool operator()(const Struct& s) const
        {
            return s.*field == cmp;
        }
    private:
        T Struct::*field;
        const T cmp;
};

template <class Struct, class T>
field_equal_t<Struct, T> field_equal (T Struct::*field_, const T& cmp_)
{
    return field_equal_t<Struct, T>(field_, cmp_);
}
  
/*
 * This template class is used to facilitate the lookups into the class containers
 * By specifying the structure member function and what to compare it against, finding the  
 * proper entry is easier
 */ 
template<class Struct, class T>
class func_equal_t
{
    public:
        func_equal_t(T (Struct::*func_)(), const T& cmp_)
            : func(func_), cmp(cmp_)
        {}

        bool operator()(Struct *s) const
        {
            return (s->*func)() == cmp;
        }

        bool operator()(Struct& s) const
        {
            return (s.*func)() == cmp;
        }
    private:
        T (Struct::*func)();
        const T cmp;
};

template <class Struct, class T>
func_equal_t<Struct, T> func_equal (T (Struct::*func_)(), const T& cmp_)
{
    return func_equal_t<Struct, T>(func_, cmp_);
}

struct CharCaseEq
{
    bool operator()(char c1, char c2)
    {
        return std::toupper(c1) == std::toupper(c2);
    }
};

/*
inline string str_rem_color( string &target )
{
    int x = 0;
                
    for (size_t loc = target.find( '`' );
        loc != std::string::npos;
        loc = target.find( '`', loc + 2))
    {
        target.replace( loc, 2, "");
        x++;
    }

    return target;
}
*/

inline bool string_cmp(const std::string& s1, const std::string& s2)
{
    return (s1.size() == s2.size()) &&
           (true == std::equal(s1.begin(), s1.end(), s2.begin(), CharCaseEq() ));
}

inline bool string_pre(const std::string& s1, const std::string& s2)
{
    return (true == std::equal(s1.begin(), s1.end(), s2.begin(), CharCaseEq() ));
}

class ci_string_cmp : public unary_function<string, bool>
{
    private:
    	string x;

    public:
	explicit ci_string_cmp(const string &str) : x(str) {}
	bool operator()(const string &y) const
	{
	    return string_cmp(x, y);
	}
};

template <class Struct>
class func_ci_str_cmp //: public unary_function<string, bool>
{
    private:
	string (Struct::*func)();
	string str;

    public:
	func_ci_str_cmp(string (Struct::*func_)(), const string& _str) 
	    : func(func_), str(_str) {}
        
	bool operator()(Struct *s) const
        {
            return string_cmp( (s->*func)(), str );
        }

        bool operator()(Struct& s) const
        {
            return string_cmp( (s.*func)(), str );
        }
};
  
template <class Struct>
func_ci_str_cmp<Struct> func_equal_ci (string (Struct::*func_)(), const string& cmp_)
{
    return func_ci_str_cmp<Struct>(func_, cmp_);
}

template <class Struct>
class func_ci_str_pre //: public unary_function<string, bool>
{
    private:
        string (Struct::*func)();
        string str;

    public:
        func_ci_str_pre(string (Struct::*func_)(), const string& _str)
            : func(func_), str(_str) {}
     
        bool operator()(Struct *s) const
        {
            return string_pre( str, (s->*func)() );
        }

        bool operator()(Struct& s) const
        {
            return string_pre( str, (s.*func)() );
        }
};      

template <class Struct>
func_ci_str_pre<Struct> func_pre_ci (string (Struct::*func_)(), const string& cmp_)                       
{
    return func_ci_str_pre<Struct>(func_, cmp_);
}

/**
 * Utility function for finding an Object based on
 * one of the Objects fields.  Just pass the
 * address of the class member function that exposes the
 * private attribute you wish to search on.
 *
 * Eg. getByFunc<NoteBoard, string>(&NoteBoard::getName, "OOC",
 *              List.begin(), List.end())
 * Will search for the NoteBoard with a name equal to "OOC"
 */
template<class Struct, class Type, class InputIterator>
Struct *getPtrByFunc(Type (Struct::*func_)(), const Type& cmp_, InputIterator begin, InputIterator end)
{
    InputIterator it = find_if(begin, end, func_equal<Struct, Type>(func_, cmp_));
    return (it != end) ? (*it) : NULL;
}

template<class Struct, class InputIterator>
Struct *getPtrByFuncCI(string (Struct::*func_)(), const string& cmp_, InputIterator begin, InputIterator end)
{
    InputIterator it = find_if(begin, end, func_equal_ci<Struct>(func_, cmp_));
    return (it != end) ? (*it) : NULL;
}

template<class Struct, class InputIterator>
Struct *getPtrByFuncCIPre(string (Struct::*func_)(), const string& cmp_, InputIterator begin, InputIterator end)
{
    InputIterator it = find_if(begin, end, func_pre_ci<Struct>(func_, cmp_));
    return (it != end) ? (*it) : NULL;
}

string strUpper(string& str);
string strLower(string& str);

#endif
