#ifndef _NOTE_H
#define _NOTE_H

#include <string>
#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <strstream>
#include <time.h>
#include "Interfaces.h"
#include "Util.h"

class Note;
class NoteBoard;

class NoteField
{
    private:
    	list<string>	values;

    public:
	NoteField() {}
	NoteField(string str) { split(str, " ", back_inserter(values)); }
	NoteField(NoteField &nf)
	{
	    list<string>::iterator it = nf.values.begin();
	    for(; it != nf.values.end(); ++it)
	   	values.push_back(*it);
	}
	~NoteField() {}

	NoteField & operator =(string &val) 
	{
	    split(val, " ", back_inserter(values));
	    return *this;
	}

	NoteField & operator =(const char *val) 
	{
	    split(string(val), " ", back_inserter(values));
	    return *this;
	}

	operator const char *()		{ return toString().c_str(); }

	string toString()
	{
	    strstream ss;
	    list<string>::iterator i = values.begin();
   	    while( i != values.end() )
    	    {       
        	ss << *i << " ";   
        	++i;
    	    }
	    ss << ends;
	    return ss.str();	    
	}

	list<string> getValues() { return values; }
	bool empty() { return values.empty(); }

	void remove(string str)
	{
	    values.remove(str);
	}

 	void add(string str)
 	{
	    values.push_back(str);
	}

	void setValue(string val)
	{
	    values.clear();
	    split(val, " ", back_inserter(values));
	}

 	void setValue(char * val)
	{
	    values.clear();
	    split(string(val), " ", back_inserter(values));
	}

	bool hasValue(string val) 
	{
	    return find_if(values.begin(), values.end(), ci_string_cmp(val)) != values.end();
	}
};

class Note : virtual public IStreamable
{
    public:
	static int 	counter;
	static int	threadCounter;

    private:
	/** name of character who sent the note */
	string      	m_strSender;
 
	/** List of recipients for the note */
	NoteField	m_toList; 

	NoteField	m_ccList;

	NoteField	m_bccList;

	/** Subject of the note */
	string		m_strSubject;   

	/** The actual body of the note */
	string		m_strText;
  
	/** Timestamp of the time the note was posted */
	time_t      	m_lDateStamp;

	int		m_iNumber;

	int		m_iThread;

	NoteBoard 	*m_nbBoard;

    public:
	Note()
	    : m_strSender(), m_toList(), m_ccList(), m_bccList(), m_strSubject(), m_strText(),
	    m_lDateStamp(time(NULL)), m_iNumber(++Note::counter), m_iThread(++Note::threadCounter), m_nbBoard(NULL)
	{}

	Note(StaticInput &is);

	Note(string _sender, string _to, string _cc, string _bcc, string _subj, string _text)
	    :  m_strSender(_sender), m_toList(_to), m_ccList(_cc), m_bccList(_bcc), 
	    m_strSubject(_subj), m_strText(_text), m_lDateStamp(time(NULL)), 
	    m_iNumber(++Note::counter), m_nbBoard(NULL)
	{}

	Note(string _sender, string _to, string _cc, string _bcc, string _subj, string _text, int _thread)
	    :   m_strSender(_sender), m_toList(_to), m_ccList(_cc), m_bccList(_bcc), 
	    m_strSubject(_subj), m_strText(_text), m_lDateStamp(time(NULL)), m_iNumber(++Note::counter), 
	    m_iThread(_thread), m_nbBoard(NULL)
	{}	

	Note(string _sender, string _to, string _cc, string _bcc, NoteBoard *_board, 
			string _subj, string _text, int _thread)
	    : m_strSender(_sender), m_toList(_to), m_ccList(_cc), m_bccList(_bcc), m_strSubject(_subj), 
	    m_strText(_text), m_lDateStamp(time(NULL)), m_iNumber(++Note::counter), m_iThread(_thread), 
	    m_nbBoard(_board) 
	{}
	
	Note(string _sender, NoteField _to, NoteField _cc, NoteField _bcc, NoteBoard *_board, 
			string _subj, string _text, int _thread)
	    : m_strSender(_sender), m_toList(_to), m_ccList(_cc), m_bccList(_bcc), m_strSubject(_subj), 
	    m_strText(_text), m_lDateStamp(time(NULL)), m_iNumber(++Note::counter), m_iThread(_thread), 
	    m_nbBoard(_board) 
	{}

	~Note();

	string getSender()		{ return m_strSender;	}
	string getSubject()		{ return m_strSubject;	}
	string getText()		{ return m_strText;	}
	int getNumber()			{ return m_iNumber;	}
	int getThread()			{ return m_iThread;	}
	NoteBoard *getBoard()		{ return m_nbBoard;	}
	long getDateStamp()		{ return m_lDateStamp;	}

	string toString();
	operator const char *()		{ return toString().c_str(); }

	NoteField & getToList()		{ return m_toList;	}
	NoteField & getCcList()		{ return m_ccList;	}
	NoteField & getBccList()	{ return m_bccList;	}

	void setSender(string str)	{ m_strSender = str;	}
	void setBoard(NoteBoard *nb)	{ m_nbBoard = nb;	}
	void setSubject(string str)	{ m_strSubject = str;	}
	void setToList(string str)	{ m_toList.setValue(str);	}
	void setCcList(string str)	{ m_ccList.setValue(str);	}
	void setBccList(string str)	{ m_bccList.setValue(str);	}
	void setDateStamp(time_t val)	{ m_lDateStamp = val;	}
	void setText(string str)	{ m_strText = str;	}

	static Note *createReply(Note *pNote, string strSender);
	static Note *createForward(Note *pNote, string strSender);
	string redirectFormat(string pre);
	void removeRecipient(string str);

	bool isTo(const string to);
	bool isFrom(const string from);

	bool readFrom(StaticInput &is);
	bool writeTo(Output &os);

	void format();

	void populate(FILE *fp);

	int operator<(const Note &rhs) const
        {
            return m_lDateStamp < rhs.m_lDateStamp;
        }
	int operator>(const Note &rhs) const
        {
            return m_lDateStamp > rhs.m_lDateStamp;
        }
};

class NoteBoard : public IStreamable
{
    private:
	static vector<NoteBoard *> s_nbList;
	static map<string, list<string> > s_grpList;

	/** Name of the note board */
	string      	m_strName;

	/** Display name of the note board */
	string      	m_strDisplay;

	/** Level required to read the board */
	int      	m_iReadLevel;

	/** Level required to write to the board */ 
	int      	m_iWriteLevel;
	
	/** Message to display when write to board fails */
	string		m_strFailedWriteMsg;

	bool		m_bEnabled;

	list<Note *>	m_Notes;

	int 		m_iNumber;

    public:
	NoteBoard() {};
	NoteBoard(int num) : m_iNumber(num) {};
	NoteBoard(string _name, string _display, int _rlevel, int _wlevel, string _fail, bool _enabled);
	~NoteBoard();

	list<Note *> getList()		{ return m_Notes; 		}
	void add(Note *note)		{ note->setBoard(this); m_Notes.push_front(note);	}
	void remove(Note *note)		{ m_Notes.remove(note);		}

	Note *getNote();
	int getReadLevel()		{ return m_iReadLevel; 		}
	int getWriteLevel()		{ return m_iWriteLevel; 	}
	int getNumber()			{ return m_iNumber; 		}

	string getName()		{ return m_strName; 		}
	string getDisplay()		{ return m_strDisplay; 		}
	string getFailedMsg()		{ return m_strFailedWriteMsg;	}

	void setName(string str)	{ m_strName = str; 		}
	void setDisplay(string str)	{ m_strDisplay = str; 		}
	void setFailedMsg(string str)	{ m_strFailedWriteMsg = str;	}
	void clear()			{ m_Notes.clear();		}
	void sort()			{ m_Notes.sort(GreaterByPointedToValue<Note>()); }
	bool isEnabled()		{ return m_bEnabled; }
	
	bool readFrom(StaticInput &is);
	bool writeTo(Output &os);

	list<Note *>::iterator begin()	{ return m_Notes.begin(); 	}
	list<Note *>::iterator end()	{ return m_Notes.end(); 	}	
 	list<Note *> getThread(int iTNum);

	static vector<NoteBoard *> getBoards();
	static NoteBoard *getBoard(const string& name);
	static NoteBoard *getBoard(const int num);
	static void addBoard(NoteBoard *nb);
	static bool isGroup(string grpName);
	static bool isInGroup(string grpName, string name);
	static map<string, list<string> >& getGroups();
	static void addGroup(string grpName, string members);
};

#endif
