#ifndef _MUDIO_H
#define _MUDIO_H

#include <ctype.h> 
#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <cstring>
#ifdef WIN32
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#define TERM_CHAR 	'~'
#define MAX_FILENAME 	128
#define BUFFERSIZE 	4096

#if defined(WIN32) || defined(__CYGWIN32__)
#define endl "\n\r"
#else
#define endl '\n'
#endif

int isitblank(int c);

using namespace std;

class StaticInput
{
protected:
	const char *m_szData;
	const char *m_pcStart;
	const char *m_pcEnd;
	int m_iLine;
	
	// protected constructor to avoid direct instances
	StaticInput() :
	m_szData(0), m_pcStart(0), m_pcEnd(0), m_iLine(1)
	{}
	
	virtual ~StaticInput() {}
	
public:
	bool isEof() const 	{ return m_pcStart >= m_pcEnd; }
	int size() const 	{ return (m_pcEnd - m_pcStart); }
	operator bool() const 	{ return m_szData != 0; }
		
	char getch()
	{
		if ( isEof() )
			return '\0';
		if ( *m_pcStart =='\n' )
			m_iLine++;
		return *m_pcStart++;
	}
	
	int getInt();
	float getFloat();
	unsigned long getLong();
	char * getWord(char *);
	char * getString(char *); 	// ~ delimited
	char * getLine(char *);
	void getBitfield( unsigned long *);
	void * read( void *, int ); // for reading structs directly
	
	void skipWhite() 
	{ 
		while( !isEof() && isspace(*m_pcStart) ) 
		{
			if ( *m_pcStart++ == '\n' )
				m_iLine++;
			//m_pcStart++; 
		}
	}
	
	void skipLine();
 	void skipBlank() 	{ while( !isEof() && isitblank((int)*m_pcStart) ) m_pcStart++; }
	void putBack()		{ m_pcStart--; if(*m_pcStart == '\n') m_iLine--; }
	
	int getLineNo() { return m_iLine; }
	virtual const string getName() { return "Unknown"; }
};

inline void StaticInput::skipLine()
{
	char ch;
	while( !isEof() )
	{
		ch = getch();
		if( ch == '\n' || ch == '\r' )
		{
			ch = getch();
			if( ch != '\n' && ch != '\r' )
				putBack();
			m_iLine++;
			return;
		}		
	}
}

class InputFile : public StaticInput
{
protected:
	char *m_szTCache;
	string m_strFilename;	
	int m_iFlags;
	struct stat stats;
	
	public:
		InputFile( string & strName ) 
			: m_strFilename(strName), m_iFlags(0)
		{
			open();
		}
		
		InputFile( const char * strName ) 
			: m_strFilename(strName), m_iFlags(0)
		{
			open();
		}
		
		~InputFile()	{ close(); }
		
		void open();	
		void close();		
		const string getName() { return m_strFilename; }
		
};

class Output
{
protected:
	char m_szBuf[BUFFERSIZE];
	char *m_pcStart;
	char *m_pcEnd;
	bool m_bActive;
	
	Output() 
		: m_pcStart(m_szBuf), m_pcEnd(&m_szBuf[BUFFERSIZE])
	{}
	
	virtual ~Output() {}
	
public:	
	virtual void flush() = 0;
	virtual void largewrite( const void *, size_t ) =0;
	
	void write( const void *, size_t );
	void send( const char * src) { write(src, strlen(src)); }
	void sendCstring( const char * src ) { write( src, strlen(src)+1 ); } 
	void vsendf(const char * fmt, va_list args)
	{
		char buf[BUFFERSIZE * 2];
		write( buf, vsnprintf( buf, BUFFERSIZE * 2, fmt, args) );
	}
	void sendf( const char * fmt, ... )
	{
		va_list args;
		va_start(args, fmt );
		this->vsendf(fmt, args);
		va_end(args);
	}
	
	operator bool() { return m_bActive; }
	
	// all this << operator stuff for int, String etc
	// ...
	// using write(void *, int) which puts it into buffer, and if it
	// is filled flushes it
	
	Output & operator << ( const char * );
	Output & operator << ( char * str )
		{	return (*this) << (const char *)str;	}
	Output & operator << ( const string & str )
		{	return *(this) << str.c_str(); }
	Output & operator << ( int );
	Output & operator << ( long );
	Output & operator << ( unsigned long );
	Output & operator << ( char );
	Output & operator << ( short int );
	Output & operator << ( unsigned short int );
	void putBitfield( const unsigned long *, int );
};

enum IOMode
{
	ioWrite, ioAppend
};

const int DEFAULT_FILE_MODE = 0644;

class OutputFile: public Output
{
protected:
	string 	m_strFilename;
	int 	m_iFD;
	bool 	m_bSystemFD;
	
	void open(IOMode);
	
public:
	OutputFile( int iFD )
		: m_strFilename("System Descriptor"), m_iFD(iFD),
		m_bSystemFD(true)
	{
		m_bActive = true;
	}
	
	OutputFile( const char * name, IOMode mode = ioWrite )
		: m_strFilename(name), m_bSystemFD(false)
	{
		open(mode);
	}
	
	OutputFile( const string &name, IOMode mode = ioWrite )
		: m_strFilename(name), m_bSystemFD(false)
	{
		open(mode);
	}
	
	~OutputFile()
	{
		// Flush called in close
		close();
	}
	
	void flush();
	void close();
	void largewrite( const void *, size_t );
	static void main()
	{
		OutputFile of("testfile.txt");
		int x = 1;
		of << "Testing" << x << endl;
		of << x << endl;
	}
};

#endif
