// ----------------------------------------------------------------------------
// FILE
//    pdstring.cpp
// DESCRIPTION
//    PDString and related classes.
// REVISION
//    $Id: pdstring.cpp,v 1.5 2004/02/05 13:18:47 tfelici Exp $
// NOTES
//    MODIFIED BY THOMAS FELICI to detach from wxwindows:
//	   added PD prefix everywhere
//	   changed #ifdefs wx... to Photon Design notation
//	   removed wx dependency everywhere
// COPYRIGHT
//    (c) Photon Design
// ----------------------------------------------------------------------------

// This file was adapted to usage in non-GNU environments
// (especially under wxWindows) by Stefan Hammes (steve),
// (stefan.hammes@urz.uni-heidelberg.de)
//
// There were also some minor extensions made. They can
// be found in the beginning of the code.
//
// To use this code indepently from libg++ all classes
// were renamed by adding a 'wx' in front of their names!
//
// 01.12.94
//
// NOTE: the DEC C++ compiler doesn't allow friend members to be
//       declared NOT inline in the class and inline afterwards.
//       therefore add the inline qualifier to the declarations
//       in the class.
// NOTE: the DEC C++ compiler doesn't allow array parameters of the
//       class in definition. therefore the split/join members are
//       not available under VMS
// Stefan Hammes (steve), 03.12.94
//
// 25.05.95: all member functions capitalized to fit naming convention
//           of wxWindows. added member functions from the simple string
//           class of wxWindows. changed void return types to return
//           a reference to 'this'.

/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* 
  PDString class implementation
 */

#ifdef __GNUG__
#pragma implementation
#endif

#ifdef WIN32
// disables warnings about conversion between different integral types and
// '==' : signed/unsigned mismatch
#pragma warning( disable : 4135 4018 )
#endif

#include "pdstring.h"

//#include <std.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <new.h>
#include <string.h>//TOMSMOD added this
//#include <builtin.h>


//********************************************************************************
//********************************************************************************
//********************************************************************************
// additions by steve, 1.12.94
#include <stdarg.h>

#define   PDToUpper(C)      (((C) >= 'a' && (C) <= 'z')? (C) - 'a' + 'A': (C))
#define   PDToLower(C)      (((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))
                   
static const int sprintfBufferSize = 1024;

inline static int PD_slen(const char* t); // inline  strlen (prototype)

#if 0 // do not use anymore: clash with GNU constructor!
// generate a string (sprintf-like)
// used also for normal construction 'PDString a("hello");'
// fmt == NULL is allowed!
PDString::PDString(const char *fmt, ...) // formatted construction
{
    va_list args;

    va_start(args,fmt);
    if(fmt==NULL){
      rep = Salloc(0, fmt, -1, -1);
    }else{
      char *tempString,buf[sprintfBufferSize];
      int allocated=0;
      int len=PD_slen(fmt);
      if(len>sprintfBufferSize/2){
        tempString = new char[len*2]; // for long strings make a big buffer
        allocated = 1;
      }else{
        tempString = buf;
      }
      vsprintf(tempString,fmt,args);
      rep = Salloc(0, tempString, -1, -1);
      if (allocated) delete[] tempString;
    }
    va_end(args);
}
#endif


PDString PDString::SubString(int from, int to) {
	return(At(from,to-from+1));
}


// operator versions

PDString& PDString::operator +=(const PDString& y)
{
  Cat(*this, y, *this);
  return *this;
}

PDString& PDString::operator +=(const PDSubString& y)
{
  Cat(*this, y, *this);
  return *this;
}

PDString& PDString::operator += (const char* y)
{
  Cat(*this, y, *this);
  return *this;
}

PDString& PDString:: operator +=(char y)
{
  Cat(*this, y, *this);
  return *this;
}


// Zillions of cats...

void Cat(const PDString& x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDString& x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDString& x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), y, -1);
}

void Cat(const PDString& x, char y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), &y, 1);
}

void Cat(const PDSubString& x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDSubString& x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDSubString& x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), y, -1);
}

void Cat(const PDSubString& x, char y, PDString& r)
{
  r.rep = Scat(r.rep, x.Chars(), x.Length(), &y, 1);
}

void Cat(const char* x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, x, -1, y.Chars(), y.Length());
}

void Cat(const char* x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, x, -1, y.Chars(), y.Length());
}

void Cat(const char* x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, x, -1, y, -1);
}

void Cat(const char* x, char y, PDString& r)
{
  r.rep = Scat(r.rep, x, -1, &y, 1);
}

void Cat(const PDString& a, const PDString& x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDString& a, const PDString& x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDString& a, const PDString& x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), y, -1);
}

void Cat(const PDString& a, const PDString& x, char y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), &y, 1);
}

void Cat(const PDString& a, const PDSubString& x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDString& a, const PDSubString& x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const PDString& a, const PDSubString& x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), y, -1);
}

void Cat(const PDString& a, const PDSubString& x, char y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x.Chars(), x.Length(), &y, 1);
}

void Cat(const PDString& a, const char* x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x, -1, y.Chars(), y.Length());
}

void Cat(const PDString& a, const char* x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x, -1, y.Chars(), y.Length());
}

void Cat(const PDString& a, const char* x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x, -1, y, -1);
}

void Cat(const PDString& a, const char* x, char y, PDString& r)
{
  r.rep = Scat(r.rep, a.Chars(), a.Length(), x, -1, &y, 1);
}


void Cat(const char* a, const PDString& x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const char* a, const PDString& x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const char* a, const PDString& x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), y, -1);
}

void Cat(const char* a, const PDString& x, char y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), &y, 1);
}

void Cat(const char* a, const PDSubString& x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const char* a, const PDSubString& x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), y.Chars(), y.Length());
}

void Cat(const char* a, const PDSubString& x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), y, -1);
}

void Cat(const char* a, const PDSubString& x, char y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x.Chars(), x.Length(), &y, 1);
}

void Cat(const char* a, const char* x, const PDString& y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x, -1, y.Chars(), y.Length());
}

void Cat(const char* a, const char* x, const PDSubString& y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x, -1, y.Chars(), y.Length());
}

void Cat(const char* a, const char* x, const char* y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x, -1, y, -1);
}

void Cat(const char* a, const char* x, char y, PDString& r)
{
  r.rep = Scat(r.rep, a, -1, x, -1, &y, 1);
}

// searching

int PDString::Index(char c, int startpos) const
{
  return Search(startpos, Length(), c);
}

int PDString::Index(const char* t, int startpos) const
{
  return Search(startpos, Length(), t);
}

int PDString::Index(const PDString& y, int startpos) const
{
  return Search(startpos, Length(), y.Chars(), y.Length());
}

int PDString::Index(const PDSubString& y, int startpos) const
{
  return Search(startpos, Length(), y.Chars(), y.Length());
}

int PDString::Contains(char c) const
{
  return Search(0, Length(), c) >= 0;
}

int PDString::Contains(const char* t) const
{   
  return Search(0, Length(), t) >= 0;
}

int PDString::Contains(const PDString& y) const
{   
  return Search(0, Length(), y.Chars(), y.Length()) >= 0;
}

int PDString::Contains(const PDSubString& y) const
{
  return Search(0, Length(), y.Chars(), y.Length()) >= 0;
}

int PDString::Contains(char c, int p) const
{
  return Match(p, Length(), 0, &c, 1) >= 0;
}

int PDString::Contains(const char* t, int p) const
{
  return Match(p, Length(), 0, t) >= 0;
}

int PDString::Contains(const PDString& y, int p) const
{
  return Match(p, Length(), 0, y.Chars(), y.Length()) >= 0;
}

int PDString::Contains(const PDSubString& y, int p) const
{
  return Match(p, Length(), 0, y.Chars(), y.Length()) >= 0;
}

int PDString::Matches(const PDSubString& y, int p) const
{
  return Match(p, Length(), 1, y.Chars(), y.Length()) >= 0;
}

int PDString::Matches(const PDString& y, int p) const
{
  return Match(p, Length(), 1, y.Chars(), y.Length()) >= 0;
}

int PDString::Matches(const char* t, int p) const
{
  return Match(p, Length(), 1, t) >= 0;
}

int PDString::Matches(char c, int p) const
{
  return Match(p, Length(), 1, &c, 1) >= 0;
}

int PDSubString::Contains(const char* t) const
{   
  return S.Search(pos, pos+len, t) >= 0;
}

int PDSubString::Contains(const PDString& y) const
{   
  return S.Search(pos, pos+len, y.Chars(), y.Length()) >= 0;
}

int PDSubString::Contains(const PDSubString&  y) const
{   
  return S.Search(pos, pos+len, y.Chars(), y.Length()) >= 0;
}

int PDSubString::Contains(char c) const
{
  return S.Search(pos, pos+len, c) >= 0;
}

int PDString::GSub(const PDString& pat, const PDString& r)
{
  return _gsub(pat.Chars(), pat.Length(), r.Chars(), r.Length());
}

int PDString::GSub(const PDSubString&  pat, const PDString& r)
{
  return _gsub(pat.Chars(), pat.Length(), r.Chars(), r.Length());
}

int PDString::GSub(const char* pat, const PDString& r)
{
  return _gsub(pat, -1, r.Chars(), r.Length());
}

int PDString::GSub(const char* pat, const char* r)
{
  return _gsub(pat, -1, r, -1);
}

// Prepend

PDString& PDString::Prepend(const PDString& y)
{
  rep = Sprepend(rep, y.Chars(), y.Length());
  return(*this);
}

PDString& PDString::Prepend(const char* y)
{
  rep = Sprepend(rep, y, -1);
  return(*this);
}

PDString& PDString::Prepend(char y)
{
  rep = Sprepend(rep, &y, 1);
  return(*this);
}

PDString& PDString::Prepend(const PDSubString& y)
{
  rep = Sprepend(rep, y.Chars(), y.Length());
  return(*this);
}

// misc transformations

void PDString::Reverse()
{
  rep = Sreverse(rep, rep);
}

void PDString::Upcase()
{
  rep = Supcase(rep, rep);
}

void PDString::Capitalize()
{
  rep = Scapitalize(rep, rep);
}

#if defined(__GNUG__) && !defined(_G_NO_NRV)

PDString operator + (const PDString& x, const PDString& y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const PDString& x, const PDSubString& y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const PDString& x, const char* y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const PDString& x, char y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const PDSubString& x, const PDString& y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const PDSubString& x, const PDSubString& y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const PDSubString& x, const char* y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const PDSubString& x, char y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const char* x, const PDString& y) return r;
{
  Cat(x, y, r);
}

PDString operator + (const char* x, const PDSubString& y) return r;
{
  Cat(x, y, r);
}

PDString Reverse(const PDString& x) return r;
{
  r.rep = Sreverse(x.rep, r.rep);
}

PDString Upcase(const PDString& x) return r;
{
  r.rep = Supcase(x.rep, r.rep);
}

PDString Downcase(const PDString& x) return r;
{
  r.rep = Sdowncase(x.rep, r.rep);
}

PDString Capitalize(const PDString& x) return r;
{
  r.rep = Scapitalize(x.rep, r.rep);
}

#else /* !_G_NO_NRV */


PDString operator + (const PDString& x, const PDString& y)
{
  PDString r;  Cat(x, y, r);  return r;
}

PDString operator + (const PDString& x, const PDSubString& y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const PDString& x, const char* y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const PDString& x, char y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const PDSubString& x, const PDString& y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const PDSubString& x, const PDSubString& y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const PDSubString& x, const char* y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const PDSubString& x, char y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const char* x, const PDString& y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString operator + (const char* x, const PDSubString& y)
{
  PDString r; Cat(x, y, r); return r;
}

PDString Reverse(const PDString& x)
{
  PDString r; r.rep = Sreverse(x.rep, r.rep); return r;
}

PDString Upcase(const PDString& x)
{
  PDString r; r.rep = Supcase(x.rep, r.rep); return r;
}

PDString Downcase(const PDString& x)
{
  PDString r; r.rep = Sdowncase(x.rep, r.rep); return r;
}

PDString Capitalize(const PDString& x)
{
  PDString r; r.rep = Scapitalize(x.rep, r.rep); return r;
}

#endif /* !_G_NO_NRV */


// a zillion comparison operators

int operator==(const PDString& x, const PDString& y)
{
  return Compare(x, y) == 0;
}

int operator!=(const PDString& x, const PDString& y)
{
  return Compare(x, y) != 0;
}

int operator>(const PDString& x, const PDString& y)
{
  return Compare(x, y) > 0;
}

int operator>=(const PDString& x, const PDString& y)
{
  return Compare(x, y) >= 0;
}

int operator<(const PDString& x, const PDString& y)
{
  return Compare(x, y) < 0;
}

int operator<=(const PDString& x, const PDString& y)
{
  return Compare(x, y) <= 0; 
}

int operator==(const PDString& x, const PDSubString&  y) 
{
  return Compare(x, y) == 0; 
}

int operator!=(const PDString& x, const PDSubString&  y)
{
  return Compare(x, y) != 0; 
}

int operator>(const PDString& x, const PDSubString&  y)      
{
  return Compare(x, y) > 0; 
}

int operator>=(const PDString& x, const PDSubString&  y)
{
  return Compare(x, y) >= 0; 
}

int operator<(const PDString& x, const PDSubString&  y) 
{
  return Compare(x, y) < 0; 
}

int operator<=(const PDString& x, const PDSubString&  y)
{
  return Compare(x, y) <= 0; 
}

int operator==(const PDString& x, const char* t) 
{
  return Compare(x, t) == 0; 
}

int operator!=(const PDString& x, const char* t) 
{
  return Compare(x, t) != 0; 
}

int operator>(const PDString& x, const char* t)  
{
  return Compare(x, t) > 0; 
}

int operator>=(const PDString& x, const char* t) 
{
  return Compare(x, t) >= 0; 
}

int operator<(const PDString& x, const char* t)  
{
  return Compare(x, t) < 0; 
}

int operator<=(const PDString& x, const char* t) 
{
  return Compare(x, t) <= 0;
}

int operator==(const PDSubString& x, const PDString& y) 
{
  return Compare(y, x) == 0; 
}

int operator!=(const PDSubString& x, const PDString& y)
{
  return Compare(y, x) != 0;
}

int operator>(const PDSubString& x, const PDString& y)      
{
  return Compare(y, x) < 0;
}

int operator>=(const PDSubString& x, const PDString& y)     
{
  return Compare(y, x) <= 0;
}

int operator<(const PDSubString& x, const PDString& y)      
{
  return Compare(y, x) > 0;
}

int operator<=(const PDSubString& x, const PDString& y)     
{
  return Compare(y, x) >= 0;
}

int operator==(const PDSubString& x, const PDSubString&  y) 
{
  return Compare(x, y) == 0; 
}

int operator!=(const PDSubString& x, const PDSubString&  y)
{
  return Compare(x, y) != 0;
}

int operator>(const PDSubString& x, const PDSubString&  y)      
{
  return Compare(x, y) > 0;
}

int operator>=(const PDSubString& x, const PDSubString&  y)
{
  return Compare(x, y) >= 0;
}

int operator<(const PDSubString& x, const PDSubString&  y) 
{
  return Compare(x, y) < 0;
}

int operator<=(const PDSubString& x, const PDSubString&  y)
{
  return Compare(x, y) <= 0;
}

int operator==(const PDSubString& x, const char* t) 
{
  return Compare(x, t) == 0; 
}

int operator!=(const PDSubString& x, const char* t) 
{
  return Compare(x, t) != 0;
}

int operator>(const PDSubString& x, const char* t)  
{
  return Compare(x, t) > 0; 
}

int operator>=(const PDSubString& x, const char* t) 
{
  return Compare(x, t) >= 0; 
}

int operator<(const PDSubString& x, const char* t)  
{
  return Compare(x, t) < 0; 
}

int operator<=(const PDSubString& x, const char* t) 
{
  return Compare(x, t) <= 0; 
}

//---


void PDString::sprintf(const char *fmt, ...)
{
    va_list args;

    va_start(args,fmt);
    char *tempString,buf[sprintfBufferSize];
    int allocated=0;
    int len=PD_slen(fmt);
    if(len>sprintfBufferSize/2){
      tempString = new char[len*2];
      allocated = 1;
    }else{
      tempString = buf;
    }
    vsprintf(tempString,fmt,args);
    rep = Salloc(rep, tempString, -1, -1); //tfelici - fixed meme leak: 0 -> rep
    if (allocated) delete[] tempString;
    va_end(args);
}

Bool PDString::IsAscii(void) const
{
  const char *s = Chars();
  while(*s){
    if(!isascii(*s)) return(FALSE);
    s++;
  }
  return(TRUE);
}
  
Bool PDString::IsWord(void) const
{
  const char *s = Chars();
  while(*s){
    if(!isalpha(*s)) return(FALSE);
    s++;
  }
  return(TRUE);
}
  
Bool PDString::IsNumber(void) const
{
  const char *s = Chars();
  while(*s){
    if(!isdigit(*s)) return(FALSE);
    s++;
  }
  return(TRUE);
}

Bool PDString::IsNull(void) const
{
  // we have 2 ways how an null representation is defined!
  // a PDString is only null when it has nothing assigned or
  // has assigned a NULL pointer.
  return(rep==NULL || rep==&PD_nilStrRep);
}

// Return a substring of self stripped at beginning and/or end
PDSubString PDString::Strip(StripType stype, char c)
{
  int start = 0;     // Index of first character
  int end = Length();    // One beyond last character
  const char* direct = Chars();  

  if (stype & leading)
    while (start < end && direct[start] == c)
      ++start;
  if (stype & trailing)
    while (start < end && direct[end-1] == c)
      --end;
      
  return(_substr(start, end-start));
}

// problem: how to check for EOF?
int Readline(FILE *f, PDString& x, char terminator, int discard)
{
  if (ferror(f)!=0)
    return 0;
  int ch;
  int i = 0;
  x.rep = Sresize(x.rep, 80);
  while ((ch = fgetc(f)) != EOF)
  {
    if (ch != terminator || !discard)
    {
      if (i >= x.rep->sz - 1)
        x.rep = Sresize(x.rep, i+1);
      x.rep->s[i++] = ch;
    }
    if (ch == terminator)
      break;
  }
  x.rep->s[i] = 0;
  x.rep->len = i;
  return i;
}

// addition by steve for non-GNU environments
static void PD_testErrorHandler(const char *className, const char *msg)
{
	std::cerr << className << ": " << msg << "\n";
}
static void (*lib_error_handler)(const char *, const char *) = PD_testErrorHandler;

// additions for compatibility with simple wxWindows string class

// String comparisons
int PDString::CompareTo(const char* s, CaseCompare cmp) const
{
  if(cmp==ignoreCase){
    return(FCompare(*this,s));
  }else{
    return(Compare(*this,s));
  }
}

int PDString::CompareTo(const PDString& st, CaseCompare cmp) const
{
  if(cmp==ignoreCase){
    return(FCompare(*this,st));
  }else{
    return(Compare(*this,st));
  }
}

// compare memory case-insensitive
static Bool PD_mem_insensitive_equal(const char* p, const char* q, int n)
{
  while (n--){
    if (PDToUpper((unsigned char)*p) != PDToUpper((unsigned char)*q)) return FALSE;
    p++; q++;
  }
  return TRUE;
}

// Pattern Matching:
int PDString::Index(const char* pattern, int startIndex, CaseCompare cmp) const
{
  assert(pattern!=NULL && "int PDString::Index(const char* pattern, int startIndex, CaseCompare cmp) const: NULL argument");
  int PD_slen = Length();
  int plen = strlen(pattern);
  if (PD_slen < startIndex + plen) return NO_POS;
  if (plen == 0) return startIndex;
  PD_slen -= startIndex + plen;
  const char* sp = Chars() + startIndex;
  if (cmp == exact) {
    int i;
    char first = *pattern;
    for (i = 0; i <= PD_slen; ++i)
      if (sp[i] == first && memcmp(sp+i+1, pattern+1, plen-1) == 0)
        return i + startIndex;
  } else {
    int first = PDToUpper((unsigned char) *pattern);
    int i;
    for (i = 0; i <= PD_slen; ++i)
      if (PDToUpper((unsigned char) sp[i]) == first &&
        PD_mem_insensitive_equal(sp+i+1, pattern+1, plen-1)) return i + startIndex;
  }
  return NO_POS;
}
  
int PDString::Index(const PDString& cs, int i, CaseCompare cmp) const
{
  return(Index(cs.Chars(),i,cmp));
}

Bool PDString::Contains(const char* pat, CaseCompare cmp) const
{
  assert(pat!=NULL&&"Bool PDString::Contains(const char* pat, CaseCompare cmp) const: NULL argument");
  return(Index(pat,0,cmp)!=NO_POS);
}

Bool PDString::Contains(const PDString& pat, CaseCompare cmp) const
{
  return(Index(pat,0,cmp)!=NO_POS);
}

PDString PDString::Copy() const
{
  PDString string(*this);
  return(string);
}

int PDString::First(char c) const
{
  const char *s = Chars();
  if(s == NULL){
    return(NO_POS);
  }else{
    const char *pos = strchr(s,c);
    if(pos==NULL){
      return(NO_POS);
    }else{
      return(pos-s);
    }
  }
}

int PDString::First(const char* cs) const
{
  assert(cs!=NULL&&"int PDString::First(const char* cs) const: NULL argument");
  const char *s = Chars();
  if(s==NULL || strlen(cs)==0){
    return(NO_POS);
  }else{
// PHE
#if defined(__BORLANDC__) && defined(WIN32)
    const char *pos = strstr((char *)s,(char *)cs);
#else
    const char *pos = strstr(s,cs);
#endif
    if(pos==NULL){
      return(NO_POS);
    }else{
      return(pos-s);
    }
  }
}

int PDString::First(const PDString& cs) const
{
  return(First(cs.Chars()));
}

int PDString::Last(const char* cs) const
{
  assert(cs!=NULL&&"int PDString::Last(const char* cs) const: NULL argument");
  const char *s = Chars();
  if(s==NULL || strlen((char *)cs)==0){
    return(NO_POS);
  }else{
    const char *pos = strstr((char *)s,cs);
    if(pos==NULL){ // it's absolutely not there!
      return(NO_POS);
    }else{
      // try to search the 'cs' again
      while(TRUE){
        const char *lastPos = pos; // remember pos
        pos = strstr(pos+1,cs); // search again (after the pattern)
        if(pos==NULL) return(lastPos-s); // this WILL be reached!
      }
    }
  }
}

int PDString::Last(const PDString& cs) const
{
  return(Last(cs.Chars()));
}

int PDString::Last(char c) const
{
  const char *s = Chars();
  if(s == NULL){
    return(NO_POS);
  }else{
    const char *pos = strrchr(s,c);
    if(pos==NULL){
      return(NO_POS);
    }else{
      return(pos-s);
    }
  }
}

PDString& PDString::Insert(int pos, const char* cs)
{
  assert(cs!=NULL&&"PDString& PDString::Insert(int pos, const char* cs): NULL argument");
  const char *s = Chars();
  assert(s != NULL&&"PDString& PDString::Insert(int pos, const char* cs): self is NULL");
  int len = strlen(s);
  assert(pos < len&&"PDString& PDString::Insert(int pos, const char* cs): insert at undefined position");
  char *buf = new char[len+strlen(cs)+1];
  if (pos==0) { // Insert at beginning
    strcpy(buf,cs);
    strcat(buf,s);
  } else {
    strncpy(buf,s,pos); // copy first part
    strcpy(buf+pos,cs); // copy new part
    strcat(buf,s+pos); // copy rest
  }
  *this = buf;
  delete[] buf; // dfgg: my addition - we need to delete here yes?
  return(*this);
}

PDString& PDString::Insert(int pos, const PDString& cs)
{
  return(Insert(pos,cs.Chars()));
}

PDString& PDString::Replace(int pos, int n, const char *cs)
{
  char *s = GetData(); // use GetData to have write access
  assert(s != NULL && "Bool PDString::Replace: self is NULL");
  int len=Length();
  if (pos<len) {
    char *p = s+pos;
    while(*p && n>0) { // replace as much as possible
      *p++ = *cs++;
      n--;
    }
  }
  return(*this);
}

PDString& PDString::Replace(int pos, int n, const PDString& cs)
{
  Replace(pos,n,cs.Chars());
  return(*this);
}

//********************************************************************************
//********************************************************************************
//********************************************************************************
// standard libg++ code following

void PDString::Error(const char* msg) const
{
  (*lib_error_handler)("PDString", msg);
}

PDString::operator const char*() const
{ 
  return (const char*)Chars();
}

//  globals

PDStrRep PD_nilStrRep = { 0, 1, { 0 } }; // nil strings point here
PDString PD_nilString;               // nil SubStrings point here




/*
 the following inline fcts are specially designed to work
 in support of PDString classes, and are not meant as generic replacements
 for libc "str" functions.

 inline copy fcts -  I like left-to-right from->to arguments.
 all versions assume that `to' argument is non-null

 These are worth doing inline, rather than through calls because,
 via procedural integration, adjacent copy calls can be smushed
 together by the optimizer.
*/

// copy n bytes
static void PD_ncopy(const char* from, char* to, int n)
{
  if (from != to) while (--n >= 0) *to++ = *from++;
}

// copy n bytes, null-terminate
static void PD_ncopy0(const char* from, char* to, int n)
{
  if (from != to)
  {
    while (--n >= 0) *to++ = *from++;
    *to = 0;
  }
  else
    to[n] = 0;
}

// copy until null
static void PD_scopy(const char* from, char* to)
{
  if (from != 0) while((*to++ = *from++) != 0);
}

// copy right-to-left
static void PD_revcopy(const char* from, char* to, int n)
{
  if (from != 0) while (--n >= 0) *to-- = *from--;
}

static int PD_slen(const char* t) // inline  strlen
{
  if (t == 0)
    return 0;
  else
  {
    const char* a = t;
    while (*a++ != 0);
    return a - 1 - t;
  }
}

// minimum & maximum representable rep size

#define MAXStrRep_SIZE   ((1 << (sizeof(int) * CHAR_BIT - 2)) - 1)
#define MINStrRep_SIZE   16

#ifndef MALLOC_MIN_OVERHEAD
#define MALLOC_MIN_OVERHEAD  4
#endif

// The basic allocation primitive:
// Always round request to something close to a power of two.
// This ensures a bit of padding, which often means that
// concatenations don't have to realloc. Plus it tends to
// be faster when lots of Strings are created and discarded,
// since just about any version of malloc (op new()) will
// be faster when it can reuse identically-sized chunks

static PDStrRep* PD_Snew(int newsiz)
{
  unsigned int siz = sizeof(PDStrRep) + newsiz + MALLOC_MIN_OVERHEAD;
  unsigned int allocsiz = MINStrRep_SIZE;
  while (allocsiz < siz) allocsiz <<= 1;
  allocsiz -= MALLOC_MIN_OVERHEAD;
  if (allocsiz >= MAXStrRep_SIZE)
    (*lib_error_handler)("PDString", "Requested length out of range");
    
  PDStrRep* rep = (PDStrRep *) new char[allocsiz];
  rep->sz = allocsiz - sizeof(PDStrRep);
  return rep;
}

// Do-something-while-allocating routines.

// We live with two ways to signify empty Sreps: either the
// null pointer (0) or a pointer to the nilStrRep.

// We always signify unknown source lengths (usually when fed a char*)
// via len == -1, in which case it is computed.

// allocate, copying src if nonull

//TOMSMOD
PDStrRep* Salloc1(PDStrRep* old, const char* src, int srclen, int newlen)
{ if(!src)return Salloc(old, "", srclen, newlen);
  else return Salloc(old, src, srclen, newlen);
}
//ENDMOD

PDStrRep* Salloc(PDStrRep* old, const char* src, int srclen, int newlen)
{
// ++++ added by steve, 06.12.94
  // do not allocate something, when passed a char-NULL pointer (without length)
    if (src == 0 && srclen == -1 && newlen == -1) {
    if (old == 0 || old == &PD_nilStrRep) return(old);
    // if there was something allocated, free it
    delete[] (char*) old;  // dfgg: 
    return(0); // and return null string
  }
// ---- added by steve, 06.12.94
  
  if (old == &PD_nilStrRep) old = 0;
  if (srclen < 0) srclen = PD_slen(src);
  if (newlen < srclen) newlen = srclen;
  PDStrRep* rep;
  if (old == 0 || newlen > old->sz)
    rep = PD_Snew(newlen);
  else
    rep = old;

  rep->len = newlen;
  PD_ncopy0(src, rep->s, srclen);

  if (old != rep && old != 0) delete[] (char*) old;  // dfgg:

  return rep;
}

// reallocate: Given the initial allocation scheme, it will
// generally be faster in the long run to get new space & copy
// than to call realloc

PDStrRep* Sresize(PDStrRep* old, int newlen)
{
  if (old == &PD_nilStrRep) old = 0;
  PDStrRep* rep;
  if (old == 0)
    rep = PD_Snew(newlen);
  else if (newlen > old->sz)
  {
    rep = PD_Snew(newlen);
    PD_ncopy0(old->s, rep->s, old->len);
    delete[] (char*) old;  // dfgg: 
  }
  else
    rep = old;

  rep->len = newlen;

  return rep;
}

// like allocate, but we know that src is a PDStrRep

PDStrRep* Scopy(PDStrRep* old, const PDStrRep* s)
{
  if (old == &PD_nilStrRep) old = 0;
  if (s == &PD_nilStrRep) s = 0;
  if (old == s) 
    return (old == 0)? &PD_nilStrRep : old;
  else if (s == 0)
  {
    old->s[0] = 0;
    old->len = 0;
    return old;
  }
  else 
  {
    PDStrRep* rep;
    int newlen = s->len;
    if (old == 0 || newlen > old->sz)
    {
      if (old != 0) delete[] (char*) old;  // dfgg: 
      rep = PD_Snew(newlen);
    }
    else
      rep = old;
    rep->len = newlen;
    PD_ncopy0(s->s, rep->s, newlen);
    return rep;
  }
}

// allocate & concatenate

PDStrRep* Scat(PDStrRep* old, const char* s, int srclen, const char* t, int tlen)
{
  if (old == &PD_nilStrRep) old = 0;
  if (srclen < 0) srclen = PD_slen(s);
  if (tlen < 0) tlen = PD_slen(t);
  int newlen = srclen + tlen;
  PDStrRep* rep;

  if (old == 0 || newlen > old->sz || 
      (t >= old->s && t < &(old->s[old->len]))) // beware of aliasing
    rep = PD_Snew(newlen);
  else
    rep = old;

  rep->len = newlen;

  PD_ncopy(s, rep->s, srclen);
  PD_ncopy0(t, &(rep->s[srclen]), tlen);

  if (old != rep && old != 0) delete[] (char*) old;  // dfgg:

  return rep;
}

// double-concatenate

PDStrRep* Scat(PDStrRep* old, const char* s, int srclen, const char* t, int tlen,
             const char* u, int ulen)
{
  if (old == &PD_nilStrRep) old = 0;
  if (srclen < 0) srclen = PD_slen(s);
  if (tlen < 0) tlen = PD_slen(t);
  if (ulen < 0) ulen = PD_slen(u);
  int newlen = srclen + tlen + ulen;
  PDStrRep* rep;
  if (old == 0 || newlen > old->sz || 
      (t >= old->s && t < &(old->s[old->len])) ||
      (u >= old->s && u < &(old->s[old->len])))
    rep = PD_Snew(newlen);
  else
    rep = old;

  rep->len = newlen;

  PD_ncopy(s, rep->s, srclen);
  PD_ncopy(t, &(rep->s[srclen]), tlen);
  PD_ncopy0(u, &(rep->s[srclen+tlen]), ulen);

  if (old != rep && old != 0) delete[] (char*) old;  // dfgg:

  return rep;
}

// like Cat, but we know that new stuff goes in the front of existing rep

PDStrRep* Sprepend(PDStrRep* old, const char* t, int tlen)
{
  char* s;
  int srclen;
  if (old == &PD_nilStrRep || old == 0)
  {
    s = 0; old = 0; srclen = 0;
  }
  else
  {
    s = old->s; srclen = old->len;
  }
  if (tlen < 0) tlen = PD_slen(t);
  int newlen = srclen + tlen;
  PDStrRep* rep;
  if (old == 0 || newlen > old->sz || 
      (t >= old->s && t < &(old->s[old->len])))
    rep = PD_Snew(newlen);
  else
    rep = old;

  rep->len = newlen;

  PD_revcopy(&(s[srclen]), &(rep->s[newlen]), srclen+1);
  PD_ncopy(t, rep->s, tlen);

  if (old != rep && old != 0) delete[] (char*) old;  // dfgg: 

  return rep;
}


// string compare: first argument is known to be non-null

static int PD_scmp(const char* a, const char* b)
{
  if (b == 0)
    return *a != 0;
  else
  {
    signed char diff;
    while ((diff = *a - *b++) == 0 && *a++ != 0);
    return diff;
  }
}


static int PD_ncmp(const char* a, int al, const char* b, int bl)
{
  int n = (al <= bl)? al : bl;
  signed char diff;
  while (n-- > 0) if ((diff = *a++ - *b++) != 0) return diff;
  return al - bl;
}

int FCompare(const PDString& x, const PDString& y)
{
  const char* a = x.Chars();
  const char* b = y.Chars();
  int al = x.Length();
  int bl = y.Length();
  int n = (al <= bl)? al : bl;
  signed char diff;
  while (n-- > 0)
  {
    char ac = *a++;
    char bc = *b++;
    if ((diff = ac - bc) != 0)
    {
      if (ac >= 'a' && ac <= 'z')
        ac = ac - 'a' + 'A';
      if (bc >= 'a' && bc <= 'z')
        bc = bc - 'a' + 'A';
      if ((diff = ac - bc) != 0)
        return diff;
    }
  }
  return al - bl;
}

// these are not inline, but pull in the above inlines, so are 
// pretty fast

int Compare(const PDString& x, const char* b)
{
  return PD_scmp(x.Chars(), b);
}

int Compare(const PDString& x, const PDString& y)
{
  return PD_scmp(x.Chars(), y.Chars());
}

int Compare(const PDString& x, const PDSubString& y)
{
  return PD_ncmp(x.Chars(), x.Length(), y.Chars(), y.Length());
}

int Compare(const PDSubString& x, const PDString& y)
{
  return PD_ncmp(x.Chars(), x.Length(), y.Chars(), y.Length());
}

int Compare(const PDSubString& x, const PDSubString& y)
{
  return PD_ncmp(x.Chars(), x.Length(), y.Chars(), y.Length());
}

int Compare(const PDSubString& x, const char* b)
{
  if (b == 0)
    return x.Length();
  else
  {
    const char* a = x.Chars();
    int n = x.Length();
    signed char diff;
    while (n-- > 0) if ((diff = *a++ - *b++) != 0) return diff;
    return (*b == 0) ? 0 : -1;
  }
}

/*
 index fcts
*/

int PDString::Search(int start, int sl, char c) const
{
  const char* s = Chars();
  if (sl > 0)
  {
    if (start >= 0)
    {
      const char* a = &(s[start]);
      const char* lasta = &(s[sl]);
      while (a < lasta) if (*a++ == c) return --a - s;
    }
    else
    {
      const char* a = &(s[sl + start + 1]);
      while (--a >= s) if (*a == c) return a - s;
    }
  }
  return -1;
}

int PDString::Search(int start, int sl, const char* t, int tl) const
{
  const char* s = Chars();
  if (tl < 0) tl = PD_slen(t);
  if (sl > 0 && tl > 0)
  {
    if (start >= 0)
    {
      const char* lasts = &(s[sl - tl]);
      const char* lastt = &(t[tl]);
      const char* p = &(s[start]);

      while (p <= lasts)
      {
        const char* x = p++;
        const char* y = t;
        while (*x++ == *y++) if (y >= lastt) return --p - s;
      }
    }
    else
    {
      const char* firsts = &(s[tl - 1]);
      const char* lastt =  &(t[tl - 1]);
      const char* p = &(s[sl + start + 1]); 

      while (--p >= firsts)
      {
        const char* x = p;
        const char* y = lastt;
        while (*x-- == *y--) if (y < t) return ++x - s;
      }
    }
  }
  return -1;
}

int PDString::Match(int start, int sl, int exact, const char* t, int tl) const
{
  if (tl < 0) tl = PD_slen(t);

  if (start < 0)
  {
    start = sl + start - tl + 1;
    if (start < 0 || (exact && start != 0))
      return -1;
  }
  else if (exact && sl - start != tl)
    return -1;

  if (sl == 0 || tl == 0 || sl - start < tl || start >= sl)
    return -1;

  int n = tl;
  const char* s = &(rep->s[start]);
  while (--n >= 0) if (*s++ != *t++) return -1;
  return tl;
}

void PDSubString::assign(const PDStrRep* ysrc, const char* ys, int ylen)
{
  if (&S == &PD_nilString) return;

  if (ylen < 0) ylen = PD_slen(ys);
  PDStrRep* targ = S.rep;
  int sl = targ->len - len + ylen;

  if (ysrc == targ || sl >= targ->sz)
  {
    PDStrRep* oldtarg = targ;
    targ = Sresize(0, sl);
    PD_ncopy(oldtarg->s, targ->s, pos);
    PD_ncopy(ys, &(targ->s[pos]), ylen);
    PD_scopy(&(oldtarg->s[pos + len]), &(targ->s[pos + ylen]));
    delete[] (char*) oldtarg;  // dfgg:
  }
  else if (len == ylen)
    PD_ncopy(ys, &(targ->s[pos]), len);
  else if (ylen < len)
  {
    PD_ncopy(ys, &(targ->s[pos]), ylen);
    PD_scopy(&(targ->s[pos + len]), &(targ->s[pos + ylen]));
  }
  else
  {
    PD_revcopy(&(targ->s[targ->len]), &(targ->s[sl]), targ->len-pos-len +1);
    PD_ncopy(ys, &(targ->s[pos]), ylen);
  }
  targ->len = sl;
  S.rep = targ;
}



/*
 * substitution
 */


int PDString::_gsub(const char* pat, int pl, const char* r, int rl)
{
  int nmatches = 0;
  if (pl < 0) pl = PD_slen(pat);
  if (rl < 0) rl = PD_slen(r);
  int sl = Length();
  if (sl <= 0 || pl <= 0 || sl < pl)
    return nmatches;
  
  const char* s = Chars();

  // prepare to make new rep
  PDStrRep* nrep = 0;
  int nsz = 0;
  char* x = 0;

  int si = 0;
  int xi = 0;
  int remaining = sl;

  while (remaining >= pl)
  {
    int pos = Search(si, sl, pat, pl);
    if (pos < 0)
      break;
    else
    {
      ++nmatches;
      int mustfit = xi + remaining + rl - pl;
      if (mustfit >= nsz)
      {
        if (nrep != 0) nrep->len = xi;
        nrep = Sresize(nrep, mustfit);
        nsz = nrep->sz;
        x = nrep->s;
      }
      pos -= si;
      PD_ncopy(&(s[si]), &(x[xi]), pos);
      PD_ncopy(r, &(x[xi + pos]), rl);
      si += pos + pl;
      remaining -= pos + pl;
      xi += pos + rl;
    }
  }

  if (nrep == 0)
  {
    if (nmatches == 0)
      return nmatches;
    else
      nrep = Sresize(nrep, xi+remaining);
  }

  PD_ncopy0(&(s[si]), &(x[xi]), remaining);
  nrep->len = xi + remaining;

  if (nrep->len <= rep->sz)   // fit back in if possible
  {
    rep->len = nrep->len;
    PD_ncopy0(nrep->s, rep->s, rep->len);
    delete[] (char*) nrep;  // dfgg:
  }
  else
  {
    delete[] (char*) rep;  // dfgg:
    rep = nrep;
  }
  return nmatches;
}


/*
 * deletion
 */

PDString& PDString::Del(int pos, int len)
{
  if (pos < 0 || len <= 0 || (unsigned)(pos + len) > Length()) return(*this);
  int nlen = Length() - len;
  int first = pos + len;
  PD_ncopy0(&(rep->s[first]), &(rep->s[pos]), Length() - first);
  rep->len = nlen;
  return(*this);
}

PDString& PDString::Del(const char* t, int startpos)
{
  int tlen = PD_slen(t);
  int p = Search(startpos, Length(), t, tlen);
  Del(p, tlen);
  return(*this);
}

PDString& PDString::Del(const PDString& y, int startpos)
{
  Del(Search(startpos, Length(), y.Chars(), y.Length()), y.Length());
  return(*this);
}

PDString& PDString::Del(const PDSubString& y, int startpos)
{
  Del(Search(startpos, Length(), y.Chars(), y.Length()), y.Length());
  return(*this);
}

PDString& PDString::Del(char c, int startpos)
{
  Del(Search(startpos, Length(), c), 1);
  return(*this);
}

/*
 * substring extraction
 */


PDSubString PDString::At(int first, int len)
{
  return _substr(first, len);
}

PDSubString PDString::operator() (int first, int len)
{
  return _substr(first, len);
}

PDSubString PDString::Before(int pos)
{
  return _substr(0, pos);
}

PDSubString PDString::Through(int pos)
{
  return _substr(0, pos+1);
}

PDSubString PDString::After(int pos)
{
  return _substr(pos + 1, Length() - (pos + 1));
}

PDSubString PDString::From(int pos)
{
  return _substr(pos, Length() - pos);
}

PDSubString PDString::At(const PDString& y, int startpos)
{
  int first = Search(startpos, Length(), y.Chars(), y.Length());
  return _substr(first,  y.Length());
}

PDSubString PDString::At(const PDSubString& y, int startpos)
{
  int first = Search(startpos, Length(), y.Chars(), y.Length());
  return _substr(first, y.Length());
}

PDSubString PDString::At(const char* t, int startpos)
{
  int tlen = PD_slen(t);
  int first = Search(startpos, Length(), t, tlen);
  return _substr(first, tlen);
}

PDSubString PDString::At(char c, int startpos)
{
  int first = Search(startpos, Length(), c);
  return _substr(first, 1);
}

PDSubString PDString::Before(const PDString& y, int startpos)
{
  int last = Search(startpos, Length(), y.Chars(), y.Length());
  return _substr(0, last);
}

PDSubString PDString::Before(const PDSubString& y, int startpos)
{
  int last = Search(startpos, Length(), y.Chars(), y.Length());
  return _substr(0, last);
}

PDSubString PDString::Before(char c, int startpos)
{
  int last = Search(startpos, Length(), c);
  return _substr(0, last);
}

PDSubString PDString::Before(const char* t, int startpos)
{
  int tlen = PD_slen(t);
  int last = Search(startpos, Length(), t, tlen);
  return _substr(0, last);
}

PDSubString PDString::Through(const PDString& y, int startpos)
{
  int last = Search(startpos, Length(), y.Chars(), y.Length());
  if (last >= 0) last += y.Length();
  return _substr(0, last);
}

PDSubString PDString::Through(const PDSubString& y, int startpos)
{
  int last = Search(startpos, Length(), y.Chars(), y.Length());
  if (last >= 0) last += y.Length();
  return _substr(0, last);
}

PDSubString PDString::Through(char c, int startpos)
{
  int last = Search(startpos, Length(), c);
  if (last >= 0) last += 1;
  return _substr(0, last);
}

PDSubString PDString::Through(const char* t, int startpos)
{
  int tlen = PD_slen(t);
  int last = Search(startpos, Length(), t, tlen);
  if (last >= 0) last += tlen;
  return _substr(0, last);
}

PDSubString PDString::After(const PDString& y, int startpos)
{
  int first = Search(startpos, Length(), y.Chars(), y.Length());
  if (first >= 0) first += y.Length();
  return _substr(first, Length() - first);
}

PDSubString PDString::After(const PDSubString& y, int startpos)
{
  int first = Search(startpos, Length(), y.Chars(), y.Length());
  if (first >= 0) first += y.Length();
  return _substr(first, Length() - first);
}

PDSubString PDString::After(char c, int startpos)
{
  int first = Search(startpos, Length(), c);
  if (first >= 0) first += 1;
  return _substr(first, Length() - first);
}

PDSubString PDString::After(const char* t, int startpos)
{
  int tlen = PD_slen(t);
  int first = Search(startpos, Length(), t, tlen);
  if (first >= 0) first += tlen;
  return _substr(first, Length() - first);
}

PDSubString PDString::From(const PDString& y, int startpos)
{
  int first = Search(startpos, Length(), y.Chars(), y.Length());
  return _substr(first, Length() - first);
}

PDSubString PDString::From(const PDSubString& y, int startpos)
{
  int first = Search(startpos, Length(), y.Chars(), y.Length());
  return _substr(first, Length() - first);
}

PDSubString PDString::From(char c, int startpos)
{
  int first = Search(startpos, Length(), c);
  return _substr(first, Length() - first);
}

PDSubString PDString::From(const char* t, int startpos)
{
  int tlen = PD_slen(t);
  int first = Search(startpos, Length(), t, tlen);
  return _substr(first, Length() - first);
}



/*
 * split/join
 */

#ifndef VMS
int Split(const PDString& src, PDString results[], int n, const PDString& sep)
{
  PDString x = src;
  const char* s = x.Chars();
  int sl = x.Length();
  int i = 0;
  int pos = 0;
  while (i < n && pos < sl)
  {
    int p = x.Search(pos, sl, sep.Chars(), sep.Length());
    if (p < 0)
      p = sl;
    results[i].rep = Salloc(results[i].rep, &(s[pos]), p - pos, p - pos);
    i++;
    pos = p + sep.Length();
  }
  return i;
}

#if defined(__GNUG__) && !defined(_G_NO_NRV)

PDString Join(PDString src[], int n, const PDString& separator) return x;
{
  PDString sep = separator;
  int xlen = 0;
  int i;
  for (i = 0; i < n; ++i)
    xlen += src[i].Length();
  xlen += (n - 1) * sep.Length();

  x.Alloc(xlen);

  int j = 0;
  
  for (i = 0; i < n - 1; ++i)
  {
    PD_ncopy(src[i].Chars(), &(x.rep->s[j]), src[i].Length());
    j += src[i].Length();
    PD_ncopy(sep.Chars(), &(x.rep->s[j]), sep.Length());
    j += sep.Length();
  }
  PD_ncopy0(src[i].Chars(), &(x.rep->s[j]), src[i].Length());
}

#else

PDString Join(PDString src[], int n, const PDString& separator)
{
  PDString x;
  PDString sep = separator;
  int xlen = 0;
  int i;
  for (i = 0; i < n; ++i)
    xlen += src[i].Length();
  xlen += (n - 1) * sep.Length();

  x.Alloc(xlen);

  int j = 0;
  
  for (i = 0; i < n - 1; ++i)
  {
    PD_ncopy(src[i].Chars(), &(x.rep->s[j]), src[i].Length());
    j += src[i].Length();
    PD_ncopy(sep.Chars(), &(x.rep->s[j]), sep.Length());
    j += sep.Length();
  }
  PD_ncopy0(src[i].Chars(), &(x.rep->s[j]), src[i].Length());
  return x;
}

#endif

#endif // ifndef VMS
  
/*
 misc
*/

    
PDStrRep* Sreverse(const PDStrRep* src, PDStrRep* dest)
{
  int n = src->len;
  if (src != dest)
    dest = Salloc(dest, src->s, n, n);
  if (n > 0)
  {
    char* a = dest->s;
    char* b = &(a[n - 1]);
    while (a < b)
    {
      char t = *a;
      *a++ = *b;
      *b-- = t;
    }
  }
  return dest;
}


PDStrRep* Supcase(const PDStrRep* src, PDStrRep* dest)
{
  int n = src->len;
  if (src != dest) dest = Salloc(dest, src->s, n, n);
  char* p = dest->s;
  char* e = &(p[n]);
  for (; p < e; ++p) if (islower(*p)) *p = toupper(*p);
  return dest;
}

PDStrRep* Sdowncase(const PDStrRep* src, PDStrRep* dest)
{
  int n = src->len;
  if (src != dest) dest = Salloc(dest, src->s, n, n);
  char* p = dest->s;
  char* e = &(p[n]);
  for (; p < e; ++p) if (isupper(*p)) *p = tolower(*p);
  return dest;
}

PDStrRep* Scapitalize(const PDStrRep* src, PDStrRep* dest)
{
  int n = src->len;
  if (src != dest) dest = Salloc(dest, src->s, n, n);

  char* p = dest->s;
  char* e = &(p[n]);
  for (; p < e; ++p)
  {
    int at_word=islower(*p);
    if (at_word)
      *p = toupper(*p);
    else 
      at_word = isupper(*p) || isdigit(*p);

    if (at_word)
    {
      while (++p < e)
      {
        if (isupper(*p))
          *p = tolower(*p);
	/* A '\'' does not break a word, so that "Nathan's" stays
	   "Nathan's" rather than turning into "Nathan'S". */
        else if (!islower(*p) && !isdigit(*p) && (*p != '\''))
          break;
      }
    }
  }
  return dest;
}

#if defined(__GNUG__) && !defined(_G_NO_NRV)

PDString PD_Replicate(char c, int n) return w;
{
  w.rep = Sresize(w.rep, n);
  char* p = w.rep->s;
  while (n-- > 0) *p++ = c;
  *p = 0;
}

PDString PD_Replicate(const PDString& y, int n) return w
{
  int len = y.Length();
  w.rep = Sresize(w.rep, n * len);
  char* p = w.rep->s;
  while (n-- > 0)
  {
    PD_ncopy(y.Chars(), p, len);
    p += len;
  }
  *p = 0;
}

PDString PD_CommonPrefix(const PDString& x, const PDString& y, int startpos) return r;
{
  const char* xchars = x.Chars();
  const char* ychars = y.Chars();
  const char* xs = &(xchars[startpos]);
  const char* ss = xs;
  const char* topx = &(xchars[x.Length()]);
  const char* ys = &(ychars[startpos]);
  const char* topy = &(ychars[y.Length()]);
  int l;
  for (l = 0; xs < topx && ys < topy && *xs++ == *ys++; ++l);
  r.rep = Salloc(r.rep, ss, l, l);
}

PDString PD_CommonSuffix(const PDString& x, const PDString& y, int startpos) return r;
{
  const char* xchars = x.Chars();
  const char* ychars = y.Chars();
  const char* xs = &(xchars[x.Length() + startpos]);
  const char* botx = xchars;
  const char* ys = &(ychars[y.Length() + startpos]);
  const char* boty = ychars;
  int l;
  for (l = 0; xs >= botx && ys >= boty && *xs == *ys ; --xs, --ys, ++l);
  r.rep = Salloc(r.rep, ++xs, l, l);
}

#else

PDString PD_Replicate(char c, int n)
{
  PDString w;
  w.rep = Sresize(w.rep, n);
  char* p = w.rep->s;
  while (n-- > 0) *p++ = c;
  *p = 0;
  return w;
}

PDString PD_Replicate(const PDString& y, int n)
{
  PDString w;
  int len = y.Length();
  w.rep = Sresize(w.rep, n * len);
  char* p = w.rep->s;
  while (n-- > 0)
  {
    PD_ncopy(y.Chars(), p, len);
    p += len;
  }
  *p = 0;
  return w;
}

PDString CommonPrefix(const PDString& x, const PDString& y, int startpos)
{
  PDString r;
  const char* xchars = x.Chars();
  const char* ychars = y.Chars();
  const char* xs = &(xchars[startpos]);
  const char* ss = xs;
  const char* topx = &(xchars[x.Length()]);
  const char* ys = &(ychars[startpos]);
  const char* topy = &(ychars[y.Length()]);
  int l;
  for (l = 0; xs < topx && ys < topy && *xs++ == *ys++; ++l);
  r.rep = Salloc(r.rep, ss, l, l);
  return r;
}

PDString CommonSuffix(const PDString& x, const PDString& y, int startpos) 
{
  PDString r;
  const char* xchars = x.Chars();
  const char* ychars = y.Chars();
  const char* xs = &(xchars[x.Length() + startpos]);
  const char* botx = xchars;
  const char* ys = &(ychars[y.Length() + startpos]);
  const char* boty = ychars;
  int l;
  for (l = 0; xs >= botx && ys >= boty && *xs == *ys ; --xs, --ys, ++l);
  r.rep = Salloc(r.rep, ++xs, l, l);
  return r;
}

#endif

// IO

istream& operator>>(istream& s, PDString& x)
{
	if (!s.ipfx(0) || (!(s.flags() & std::ios::skipws) && !std::ws(s)))
  {
	  s.clear(std::ios::failbit|s.rdstate()); // Redundant if using GNU iostreams.
    return s;
  }
  int ch;
  int i = 0;
  x.rep = Sresize(x.rep, 20);
  register std::streambuf *sb = s.rdbuf();
  while ((ch = sb->sbumpc()) != EOF)
  {
    if (isspace(ch))
      break;
    if (i >= x.rep->sz - 1)
      x.rep = Sresize(x.rep, i+1);
    x.rep->s[i++] = ch;
  }
  x.rep->s[i] = 0;
  x.rep->len = i;
  int new_state = s.rdstate();
  if (i == 0) new_state |= std::ios::failbit;
  if (ch == EOF) new_state |= std::ios::eofbit;
  s.clear(new_state);
  return s;
}

int Readline(istream& s, PDString& x, char terminator, int discard)
{
  if (!s.ipfx(0))
    return 0;
  int ch;
  int i = 0;
  x.rep = Sresize(x.rep, 80);
  register std::streambuf *sb = s.rdbuf();
  while ((ch = sb->sbumpc()) != EOF)
  {
    if (ch != terminator || !discard)
    {
      if (i >= x.rep->sz - 1)
        x.rep = Sresize(x.rep, i+1);
      x.rep->s[i++] = ch;
    }
    if (ch == terminator)
      break;
  }
  x.rep->s[i] = 0;
  x.rep->len = i;
  if (ch == EOF) s.clear(std::ios::eofbit|s.rdstate());
  return i;
}


ostream& operator<<(ostream& s, const PDSubString& x)
{ 
  const char* a = x.Chars();
  const char* lasta = &(a[x.Length()]);
  while (a < lasta)
    s.put(*a++);
  return(s);
}

// from John.Willis@FAS.RI.CMU.EDU

int PDString::Freq(const PDSubString& y) const
{
  int found = 0;
  for (unsigned int i = 0; i < Length(); i++) 
    if (Match(i,Length(),0,y.Chars(), y.Length())>= 0) found++;
  return(found);
}

int PDString::Freq(const PDString& y) const
{
  int found = 0;
  for (unsigned int i = 0; i < Length(); i++) 
    if (Match(i,Length(),0,y.Chars(),y.Length()) >= 0) found++;
  return(found);
}

int PDString::Freq(const char* t) const
{
  int found = 0;
  for (unsigned int i = 0; i < Length(); i++) 
    if (Match(i,Length(),0,t) >= 0) found++;
  return(found);
}

int PDString::Freq(char c) const
{
  int found = 0;
  for (unsigned int i = 0; i < Length(); i++) 
    if (Match(i,Length(),0,&c,1) >= 0) found++;
  return(found);
}


int PDString::OK() const
{
  if (rep == 0             // don't have a rep
    || rep->len > rep->sz     // string oustide bounds
    || rep->s[rep->len] != 0)   // not null-terminated
      Error("invariant failure");
  return 1;
}

int PDSubString::OK() const
{
  int v = S != (const char*)0; // have a PDString;
  v &= S.OK();                 // that is legal
  v &= pos + len >= S.rep->len;// pos and len within bounds
  if (!v) S.Error("PDSubString invariant failure");
  return v;
}

// a helper needed by at, before, etc.

PDSubString PDString::_substr(int first, int l)
{
  if (first < 0 || (unsigned)(first + l) > Length() )
    return PDSubString(PD_nilString, 0, 0) ;
  else
    return PDSubString(*this, first, l);
}


