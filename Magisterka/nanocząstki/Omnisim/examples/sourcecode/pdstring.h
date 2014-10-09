// ----------------------------------------------------------------------------
// FILE
//    pdstring.h
// DESCRIPTION
//    PDString and related classes.
// REVISION
//    $Id: pdstring.h,v 1.7 2004/02/05 13:18:47 tfelici Exp $
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
// There were also some minor extensions made.
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
//       class in definition. therefore the Split/Join members are
//       not available under VMS
// Stefan Hammes (steve), 03.12.94
//
// 25.05.95: all member functions capitalized to fit naming convention
//           of wxWindows. added member functions from the simple string
//           class of wxWindows. changed void return types to return
//           a reference to 'this'.

#define VMSinline /**/ 
//#define VMSinline inline

// This may look like C code, but it is really -*- C++ -*-
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


#ifndef _PDString_h
#ifdef __GNUG__
#pragma interface
#endif
#define _PDString_h 1

#include <iostream>
using std::istream;
using std::ostream;

#include <stdio.h> // for Readline

//TOMSMOD added this
#ifndef Bool
#define Bool int
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
//ENDMOD

struct PDStrRep      // internal PDString representations
{
  unsigned int    len;         // string length 
  unsigned int    sz;          // allocated space
  char              s[1];        // the string starts here 
                                 // (at least 1 char for trailing null)
                                 // allocated & expanded via non-public fcts
};

// primitive ops on StrReps -- nearly all PDString fns go through these.

PDStrRep*     Salloc1(PDStrRep*, const char*, int, int);//TOMSMOD
PDStrRep*     Salloc(PDStrRep*, const char*, int, int);
PDStrRep*     Scopy(PDStrRep*, const PDStrRep*);
PDStrRep*     Sresize(PDStrRep*, int);
PDStrRep*     Scat(PDStrRep*, const char*, int, const char*, int);
PDStrRep*     Scat(PDStrRep*, const char*, int,const char*,int, const char*,int);
PDStrRep*     Sprepend(PDStrRep*, const char*, int);
PDStrRep*     Sreverse(const PDStrRep*, PDStrRep*);
PDStrRep*     Supcase(const PDStrRep*, PDStrRep*);
PDStrRep*     Sdowncase(const PDStrRep*, PDStrRep*);
PDStrRep*     Scapitalize(const PDStrRep*, PDStrRep*);

// These classes need to be defined in the order given

class PDString;
class PDSubString;           

class PDSubString            
{
  friend class      PDString;
protected:

  PDString&           S;        // The PDString I'm a substring of
  unsigned int    pos;      // starting position in S's rep
  unsigned int    len;      // length of substring

  void              assign(const PDStrRep*, const char*, int = -1);
                    PDSubString(PDString& x, int p, int l);
                    PDSubString(const PDSubString& x);

public:

// Note there are no public constructors. SubStrings are always
// created via PDString operations

  inline            ~PDSubString();

  PDSubString&      operator =  (const PDString&     y);
  PDSubString&      operator =  (const PDSubString&  y);
  PDSubString&      operator =  (const char* t);
  PDSubString&      operator =  (char        c);

// return 1 if target appears anywhere in PDSubString; else 0

  int               Contains(char        c) const;
  int               Contains(const PDString&     y) const;
  int               Contains(const PDSubString&  y) const;
  int               Contains(const char* t) const;
  
// IO 

  friend ostream&   operator<<(ostream& s, const PDSubString& x);

// status

  unsigned int      Length() const;
  int               Empty() const;
  const char*       Chars() const;

  int               OK() const; 

};

#ifdef __BORLANDC__
#pragma warn -inl
#endif

class PDString
{
  friend class      PDSubString;
  friend PDString PD_CommonPrefix(const PDString& x, const PDString& y, int startpos);
  friend PDString PD_CommonSuffix(const PDString& x, const PDString& y, int startpos);
protected:
  PDStrRep*         rep;   // Strings are pointers to their representations

// some helper functions

  int               Match(int, int, int, const char*, int = -1) const;
  int               _gsub(const char*, int, const char* ,int);
  PDSubString       _substr(int first, int);

public:
  // Moved from protected to public by JACS because of call in Split function
  int               Search(int, int, const char*, int = -1) const;
  int               Search(int, int, char) const;

// constructors & assignment

                    PDString(void);
                    PDString(const PDString& x);
 inline             PDString(const PDSubString&  x);
                    PDString(const char* t);
                    PDString(const char* t, int len);
                    PDString(char c);

 inline             ~PDString();

  PDString&         operator =  (const PDString&     y);
  PDString&         operator =  (const char* y);
  PDString&         operator =  (char        c);
  PDString&         operator =  (const PDSubString&  y);
                    
  PDString          Copy() const;


// replication      

  friend PDString   PD_Replicate(char        c, int n);
  friend PDString   PD_Replicate(const PDString&     y, int n);

// concatenation

  PDString&  operator += (const PDString&     y); 
  PDString&         operator += (const PDSubString&  y);
  PDString&  operator += (const char* t);
  PDString&         operator += (char        c);

  PDString&         Append(const char* cs){ return(*this += cs); }
  PDString&         Append(const PDString& s){ return(*this += s); }
  PDString&         Append(char c, int rep=1){ return(*this += PD_Replicate(c,rep)); }   // Append c rep times

  inline PDString&  Prepend(const PDString&     y); 
  PDString&         Prepend(const PDSubString&  y);
  PDString&         Prepend(const char* t);
  PDString&         Prepend(char        c);
  PDString&         Prepend(char c, int rep=1){ Prepend(PD_Replicate(c,rep)); return(*this); }  // Prepend c rep times
                                          
// procedural versions:
// concatenate first 2 args, store result in last arg

  VMSinline friend void     Cat(const PDString&, const PDString&, PDString&);
  VMSinline friend void     Cat(const PDString&, const PDSubString&, PDString&);
  VMSinline friend void     Cat(const PDString&, const char*, PDString&);
  VMSinline friend void     Cat(const PDString&, char, PDString&);

  VMSinline friend void     Cat(const PDSubString&, const PDString&, PDString&);
  VMSinline friend void     Cat(const PDSubString&, const PDSubString&, PDString&);
  VMSinline friend void     Cat(const PDSubString&, const char*, PDString&);
  VMSinline friend void     Cat(const PDSubString&, char, PDString&);

  VMSinline friend void     Cat(const char*, const PDString&, PDString&);
  VMSinline friend void     Cat(const char*, const PDSubString&, PDString&);
  VMSinline friend void     Cat(const char*, const char*, PDString&);
  VMSinline friend void     Cat(const char*, char, PDString&);

// double concatenation, by request. (yes, there are too many versions, 
// but if one is supported, then the others should be too...)
// Concatenate first 3 args, store in last arg

  VMSinline friend void     Cat(const PDString&,const PDString&, const PDString&,PDString&);
  VMSinline friend void     Cat(const PDString&,const PDString&,const PDSubString&,PDString&);
  VMSinline friend void     Cat(const PDString&,const PDString&, const char*, PDString&);
  VMSinline friend void     Cat(const PDString&,const PDString&, char, PDString&);
  VMSinline friend void     Cat(const PDString&,const PDSubString&,const PDString&,PDString&);
  VMSinline friend void     Cat(const PDString&,const PDSubString&,const PDSubString&,PDString&);
  VMSinline friend void     Cat(const PDString&,const PDSubString&, const char*, PDString&);
  VMSinline friend void     Cat(const PDString&,const PDSubString&, char, PDString&);
  VMSinline friend void     Cat(const PDString&,const char*, const PDString&,    PDString&);
  VMSinline friend void     Cat(const PDString&,const char*, const PDSubString&, PDString&);
  VMSinline friend void     Cat(const PDString&,const char*, const char*, PDString&);
  VMSinline friend void     Cat(const PDString&,const char*, char, PDString&);

  VMSinline friend void     Cat(const char*, const PDString&, const PDString&,PDString&);
  VMSinline friend void     Cat(const char*,const PDString&,const PDSubString&,PDString&);
  VMSinline friend void     Cat(const char*,const PDString&, const char*, PDString&);
  VMSinline friend void     Cat(const char*,const PDString&, char, PDString&);
  VMSinline friend void     Cat(const char*,const PDSubString&,const PDString&,PDString&);
  VMSinline friend void     Cat(const char*,const PDSubString&,const PDSubString&,PDString&);
  VMSinline friend void     Cat(const char*,const PDSubString&, const char*, PDString&);
  VMSinline friend void     Cat(const char*,const PDSubString&, char, PDString&);
  VMSinline friend void     Cat(const char*,const char*, const PDString&,    PDString&);
  VMSinline friend void     Cat(const char*,const char*, const PDSubString&, PDString&);
  VMSinline friend void     Cat(const char*,const char*, const char*, PDString&);
  VMSinline friend void     Cat(const char*,const char*, char, PDString&);


// searching & matching

// additions for compatibility with the simple wxWindows string class.
// the member functions of the simple PDString class are modelled by
// using the GNU string functions. therefore they will be realized
// much more efficient. functions of the simple PDString class which
// are identical to functions of the GNU string class are left out
// here.
// some functions are left out, because they are too dangerous:

#define NO_POS ((int)(-1)) // undefined position
  enum CaseCompare {exact, ignoreCase};
  
// comparison (more: see below)
  int               CompareTo(const char* cs,     CaseCompare cmp = exact) const;
  int               CompareTo(const PDString& st, CaseCompare cmp = exact) const;

// return position of target in string or -1 for failure

  int               Index(char        c, int startpos = 0) const;      
  int               Index(const PDString&     y, int startpos = 0) const;
  int               Index(const PDString&     y, int startpos, CaseCompare cmp) const;      
  int               Index(const PDSubString&  y, int startpos = 0) const;      
  int               Index(const char* t, int startpos = 0) const;  
  int               Index(const char* t, int startpos, CaseCompare cmp) const;  
  
// return 1 if target appears anyhere in PDString; else 0

  Bool              Contains(char        c) const;
  Bool              Contains(const PDString&     y) const;
  Bool              Contains(const PDSubString&  y) const;
  Bool              Contains(const char* t) const;
  
// case dependent/independent variation  
  Bool              Contains(const char* pat,     CaseCompare cmp) const;
  Bool              Contains(const PDString& pat, CaseCompare cmp) const;
  

// return 1 if target appears anywhere after position pos 
// (or before, if pos is negative) in PDString; else 0

  Bool              Contains(char        c, int pos) const;
  Bool              Contains(const PDString&     y, int pos) const;
  Bool              Contains(const PDSubString&  y, int pos) const;
  Bool              Contains(const char* t, int pos) const;
  
// return 1 if target appears at position pos in PDString; else 0

  Bool              Matches(char        c, int pos = 0) const;
  Bool              Matches(const PDString&     y, int pos = 0) const;
  Bool              Matches(const PDSubString&  y, int pos = 0) const;
  Bool              Matches(const char* t, int pos = 0) const;
  
//  return number of occurences of target in PDString

  int               Freq(char        c) const; 
  int               Freq(const PDString&     y) const;
  int               Freq(const PDSubString&  y) const;
  int               Freq(const char* t) const;

// first or last occurrence of item
  int               First(char c) const;
  int               First(const char* cs) const;
  int               First(const PDString& cs) const;
  
  int               Last(char c) const;
  int               Last(const char* cs) const;
  int               Last(const PDString& cs) const;

// PDSubString extraction

// Note that you can't take a substring of a const PDString, since
// this leaves open the possiblility of indirectly modifying the
// PDString through the PDSubString


  PDSubString       At(int         pos, int len);
  PDSubString       operator () (int         pos, int len); // synonym for at

  PDSubString       At(const PDString&     x, int startpos = 0); 
  PDSubString       At(const PDSubString&  x, int startpos = 0); 
  PDSubString       At(const char* t, int startpos = 0);
  PDSubString       At(char        c, int startpos = 0);
  
  PDSubString       Before(int          pos);
  PDSubString       Before(const PDString&      x, int startpos = 0);
  PDSubString       Before(const PDSubString&   x, int startpos = 0);
  PDSubString       Before(const char*  t, int startpos = 0);
  PDSubString       Before(char         c, int startpos = 0);
  
  PDSubString       Through(int          pos);
  PDSubString       Through(const PDString&      x, int startpos = 0);
  PDSubString       Through(const PDSubString&   x, int startpos = 0);
  PDSubString       Through(const char*  t, int startpos = 0);
  PDSubString       Through(char         c, int startpos = 0);
  
  PDSubString       From(int          pos);
  PDSubString       From(const PDString&      x, int startpos = 0);
  PDSubString       From(const PDSubString&   x, int startpos = 0);
  PDSubString       From(const char*  t, int startpos = 0);
  PDSubString       From(char         c, int startpos = 0);
  
  PDSubString       After(int         pos);
  PDSubString       After(const PDString&     x, int startpos = 0);
  PDSubString       After(const PDSubString&  x, int startpos = 0);
  PDSubString       After(const char* t, int startpos = 0);
  PDSubString       After(char        c, int startpos = 0);
  
  // Edward Z.'s additions
  PDString          SubString(int from, int to);

// deletion

// delete len chars starting at pos
  PDString&         Del(int         pos, int len);

// delete the first occurrence of target after startpos

  PDString&         Del(const PDString&     y, int startpos = 0);
  PDString&         Del(const PDSubString&  y, int startpos = 0);
  PDString&         Del(const char* t, int startpos = 0);
  PDString&         Del(char        c, int startpos = 0);
  PDString&         RemoveLast(void) { Del((int)(Length()-1),(int)1); return(*this); } // the funny casting is necessary to distinguish between the 'Del' members  
  PDString&         Remove(int pos){ Del(pos,(int)(Length()-pos)); return(*this); }         // Remove pos to end of string
  PDString&         Remove(int pos, int len){ Del(pos,len); return(*this); }         // Remove pos to end of string


// insertion
  PDString&         Insert(int pos, const char*);
  PDString&         Insert(int pos, const PDString&);
  
// global substitution: substitute all occurrences of pat with repl (return number of matches)

  int               GSub(const PDString&     pat, const PDString&     repl);
  int               GSub(const PDSubString&  pat, const PDString&     repl);
  int               GSub(const char* pat, const PDString&     repl);
  int               GSub(const char* pat, const char* repl);
  
  PDString&         Replace(int pos, int n, const char*);
  PDString&         Replace(int pos, int n, const PDString&);


// friends & utilities

// Split string into array res at separators; return number of elements
#ifndef VMS
  // VMS doesn't allow the arrays here, (steve) 03.12.94
  friend int        Split(const PDString& x, PDString res[], int maxn, 
                          const PDString& sep);
  friend PDString   Join(PDString src[], int n, const PDString& sep);
#endif

  friend PDString   CommonPrefix(const PDString& x, const PDString& y, 
                                  int startpos = 0);
  friend PDString   CommonSuffix(const PDString& x, const PDString& y, 
                                  int startpos = -1);

// strip chars at the front and/or end
// stripType is defined for bitwise ORing!
  enum              StripType {leading = 0x1, trailing = 0x2, both = 0x3};
  PDSubString       Strip(StripType s=trailing, char c=' ');

// simple builtin transformations

  VMSinline friend PDString     Reverse(const PDString& x);
  VMSinline friend PDString     Upcase(const PDString& x);
  VMSinline friend PDString     Downcase(const PDString& x);
  VMSinline friend PDString     Capitalize(const PDString& x);

// in-place versions of above

  void              Reverse();
  inline void       Upcase();
  void              UpperCase(){ Upcase(); } //steve
  inline void       Downcase() { rep = Sdowncase(rep, rep); }
  void              LowerCase(){ Downcase(); } //steve
  void              Capitalize();

// element extraction

  inline char&      operator [] (int i);
  inline const char&      operator [] (int i) const;
  char&             operator()(int pos){ return((*this)[(int)pos]); }
  char              Elem(int i) const;
  char              Firstchar() const;
  char              Lastchar() const;

// conversion

                    operator const char*() const;
  const char*       Chars() const;
  char*             GetData(void) const;  // wxWindows compatibility

// IO

// formatted assignment
// we do not use the 'sprintf' constructor anymore, because with that
// constructor, every initialisation with a string would go through
// sprintf and this is not desireable, because sprintf interprets
// some characters. with the above function we can write i.e.:
//
// PDString msg; msg.sprintf("Processing item %d\n",count);
  void              sprintf(const char *fmt, ...);

  VMSinline friend ostream&   operator<<(ostream& s, const PDString& x);
  friend ostream&   operator<<(ostream& s, const PDSubString& x);
  friend istream&   operator>>(istream& s, PDString& x);

  friend int        Readline(istream& s, PDString& x, 
                             char terminator = '\n',
                             int discard_terminator = 1);
  friend int        Readline(FILE *f, PDString& x, 
                             char terminator = '\n',
                             int discard_terminator = 1);

// status

  inline unsigned int Length(void) const;
  int               Empty(void) const;

// classification (should be capital, because of ctype.h macros!)
  int               IsAscii(void) const;
  int               IsWord(void) const;
  int               IsNumber(void) const;
  int               IsNull(void) const;
  int               IsDefined(void) const { return(! IsNull()); }

// preallocate some space for PDString
  void              Alloc(int newsize);

// report current allocation (not length!)
  int               Allocation(void) const;

  void              Error(const char* msg) const;
  int               OK() const;
  
};

// TOMSMOD commented this out typedef PDString StrTmp; // for backward compatibility

// other externs

int        Compare(const PDString&    x, const PDString&     y);
int        Compare(const PDString&    x, const PDSubString&  y);
int        Compare(const PDString&    x, const char* y);
int        Compare(const PDSubString& x, const PDString&     y);
int        Compare(const PDSubString& x, const PDSubString&  y);
int        Compare(const PDSubString& x, const char* y);
int        FCompare(const PDString&   x, const PDString&     y); // ignore case

extern PDStrRep PD_nilStrRep;
extern PDString PD_nilString;

// other inlines

VMSinline  PDString operator + (const PDString& x, const PDString& y);
VMSinline  PDString operator + (const PDString& x, const PDSubString& y);
VMSinline  PDString operator + (const PDString& x, const char* y);
VMSinline  PDString operator + (const PDString& x, char y);
VMSinline  PDString operator + (const PDSubString& x, const PDString& y);
VMSinline  PDString operator + (const PDSubString& x, const PDSubString& y);
VMSinline  PDString operator + (const PDSubString& x, const char* y);
VMSinline  PDString operator + (const PDSubString& x, char y);
VMSinline  PDString operator + (const char* x, const PDString& y);
VMSinline  PDString operator + (const char* x, const PDSubString& y);

VMSinline  int operator==(const PDString& x, const PDString& y); 
VMSinline  int operator!=(const PDString& x, const PDString& y);
VMSinline  int operator> (const PDString& x, const PDString& y);
VMSinline  int operator>=(const PDString& x, const PDString& y);
VMSinline  int operator< (const PDString& x, const PDString& y);
VMSinline  int operator<=(const PDString& x, const PDString& y);
VMSinline  int operator==(const PDString& x, const PDSubString&  y);
VMSinline  int operator!=(const PDString& x, const PDSubString&  y);
VMSinline  int operator> (const PDString& x, const PDSubString&  y);
VMSinline  int operator>=(const PDString& x, const PDSubString&  y);
VMSinline  int operator< (const PDString& x, const PDSubString&  y);
VMSinline  int operator<=(const PDString& x, const PDSubString&  y);
VMSinline  int operator==(const PDString& x, const char* t);
VMSinline  int operator!=(const PDString& x, const char* t);
VMSinline  int operator> (const PDString& x, const char* t);
VMSinline  int operator>=(const PDString& x, const char* t);
VMSinline  int operator< (const PDString& x, const char* t);
VMSinline  int operator<=(const PDString& x, const char* t);
VMSinline  int operator==(const PDSubString& x, const PDString& y);
VMSinline  int operator!=(const PDSubString& x, const PDString& y);
VMSinline  int operator> (const PDSubString& x, const PDString& y);
VMSinline  int operator>=(const PDSubString& x, const PDString& y);
VMSinline  int operator< (const PDSubString& x, const PDString& y);
VMSinline  int operator<=(const PDSubString& x, const PDString& y);
VMSinline  int operator==(const PDSubString& x, const PDSubString&  y);
VMSinline  int operator!=(const PDSubString& x, const PDSubString&  y);
VMSinline  int operator> (const PDSubString& x, const PDSubString&  y);
VMSinline  int operator>=(const PDSubString& x, const PDSubString&  y);
VMSinline  int operator< (const PDSubString& x, const PDSubString&  y);
VMSinline  int operator<=(const PDSubString& x, const PDSubString&  y);
VMSinline  int operator==(const PDSubString& x, const char* t);
VMSinline  int operator!=(const PDSubString& x, const char* t);
VMSinline  int operator> (const PDSubString& x, const char* t);
VMSinline  int operator>=(const PDSubString& x, const char* t);
VMSinline  int operator< (const PDSubString& x, const char* t);
VMSinline  int operator<=(const PDSubString& x, const char* t);


// status reports, needed before defining other things

inline unsigned int PDString::Length() const {  return rep->len; }
inline int         PDString::Empty() const { return rep->len == 0; }
inline const char* PDString::Chars() const { return &(rep->s[0]); }
// Compatibility with wxWindows
inline char* PDString::GetData() const { return (char *)&(rep->s[0]); }
inline int         PDString::Allocation() const { return rep->sz; }
inline void        PDString::Alloc(int newsize) { rep = Sresize(rep, newsize); }

inline unsigned int PDSubString::Length() const { return len; }
inline int         PDSubString::Empty() const { return len == 0; }
inline const char* PDSubString::Chars() const { return &(S.rep->s[pos]); }

// constructors

inline PDString::PDString() 
  : rep(&PD_nilStrRep) {}
inline PDString::PDString(const PDString& x) 
  : rep(Scopy(0, x.rep)) {}
inline PDString::PDString(const char* t) 
  : rep(Salloc1(0, t, -1, -1)) {}//TOMSMOD
inline PDString::PDString(const char* t, int tlen)
  : rep(Salloc(0, t, tlen, tlen)) {}
inline PDString::PDString(const PDSubString& y)
  : rep(Salloc(0, y.Chars(), y.Length(), y.Length())) {}
inline PDString::PDString(char c) 
  : rep(Salloc(0, &c, 1, 1)) {}

inline PDString::~PDString() { if (rep != &PD_nilStrRep) delete[] (char*) rep; } // dfgg: must match Snew

inline PDSubString::PDSubString(const PDSubString& x)
  :S(x.S), pos(x.pos), len(x.len) {}
inline PDSubString::PDSubString(PDString& x, int first, int l)
  :S(x), pos((unsigned int) first), len((unsigned int) l) {}

inline PDSubString::~PDSubString() {}

// assignment

inline PDString& PDString::operator =  (const PDString& y)
{ 
  rep = Scopy(rep, y.rep);
  return *this;
}

inline PDString& PDString::operator=(const char* t)
{
  rep = Salloc1(rep, t, -1, -1);//TOMSMOD
  return *this;
}

inline PDString& PDString::operator=(const PDSubString&  y)
{
  rep = Salloc(rep, y.Chars(), y.Length(), y.Length());
  return *this;
}

inline PDString& PDString::operator=(char c)
{
  rep = Salloc(rep, &c, 1, 1);
  return *this;
}


inline PDSubString& PDSubString::operator = (const char* ys)
{
  assign(0, ys);
  return *this;
}

inline PDSubString& PDSubString::operator = (char ch)
{
  assign(0, &ch, 1);
  return *this;
}

inline PDSubString& PDSubString::operator = (const PDString& y)
{
  assign(y.rep, y.Chars(), y.Length());
  return *this;
}

inline PDSubString& PDSubString::operator = (const PDSubString& y)
{
  assign(y.S.rep, y.Chars(), y.Length());
  return *this;
}

// Zillions of cats...

void Cat(const PDString& x, const PDString& y, PDString& r);
void Cat(const PDString& x, const PDSubString& y, PDString& r);
void Cat(const PDString& x, const char* y, PDString& r);
void Cat(const PDString& x, char y, PDString& r);
void Cat(const PDSubString& x, const PDString& y, PDString& r);
void Cat(const PDSubString& x, const PDSubString& y, PDString& r);
void Cat(const PDSubString& x, const char* y, PDString& r);
void Cat(const PDSubString& x, char y, PDString& r);
void Cat(const char* x, const PDString& y, PDString& r);
void Cat(const char* x, const PDSubString& y, PDString& r);
void Cat(const char* x, const char* y, PDString& r);
void Cat(const char* x, char y, PDString& r);
void Cat(const PDString& a, const PDString& x, const PDString& y, PDString& r);
void Cat(const PDString& a, const PDString& x, const PDSubString& y, PDString& r);
void Cat(const PDString& a, const PDString& x, const char* y, PDString& r);
void Cat(const PDString& a, const PDString& x, char y, PDString& r);
void Cat(const PDString& a, const PDSubString& x, const PDString& y, PDString& r);
void Cat(const PDString& a, const PDSubString& x, const PDSubString& y, PDString& r);
void Cat(const PDString& a, const PDSubString& x, const char* y, PDString& r);
void Cat(const PDString& a, const PDSubString& x, char y, PDString& r);
void Cat(const PDString& a, const char* x, const PDString& y, PDString& r);
void Cat(const PDString& a, const char* x, const PDSubString& y, PDString& r);
void Cat(const PDString& a, const char* x, const char* y, PDString& r);
void Cat(const PDString& a, const char* x, char y, PDString& r);
void Cat(const char* a, const PDString& x, const PDString& y, PDString& r);
void Cat(const char* a, const PDString& x, const PDSubString& y, PDString& r);
void Cat(const char* a, const PDString& x, const char* y, PDString& r);
void Cat(const char* a, const PDString& x, char y, PDString& r);
void Cat(const char* a, const PDSubString& x, const PDString& y, PDString& r);
void Cat(const char* a, const PDSubString& x, const PDSubString& y, PDString& r);
void Cat(const char* a, const PDSubString& x, const char* y, PDString& r);
void Cat(const char* a, const PDSubString& x, char y, PDString& r);
void Cat(const char* a, const char* x, const PDString& y, PDString& r);
void Cat(const char* a, const char* x, const PDSubString& y, PDString& r);
void Cat(const char* a, const char* x, const char* y, PDString& r);
void Cat(const char* a, const char* x, char y, PDString& r);


// constructive concatenation
PDString operator + (const PDString& x, const PDString& y);
PDString operator + (const PDString& x, const PDSubString& y);
PDString operator + (const PDString& x, const char* y);
PDString operator + (const PDString& x, char y);
PDString operator + (const PDSubString& x, const PDString& y);
PDString operator + (const PDSubString& x, const PDSubString& y);
PDString operator + (const PDSubString& x, const char* y);
PDString operator + (const PDSubString& x, char y);
PDString operator + (const char* x, const PDString& y);
PDString operator + (const char* x, const PDSubString& y);
PDString Reverse(const PDString& x);
PDString Upcase(const PDString& x);
PDString Downcase(const PDString& x);
PDString Capitalize(const PDString& x);

// element extraction

inline char&  PDString::operator [] (int i)
{
  if (((unsigned)i) >= Length()) Error("invalid index");
  return rep->s[i];
}

inline const char&  PDString::operator [] (int i) const
{
  if (((unsigned)i) >= Length()) Error("invalid index");
  return rep->s[i];
}

inline char  PDString::Elem (int i) const
{
  if (((unsigned)i) >= Length()) Error("invalid index");
  return rep->s[i];
}

inline char  PDString::Firstchar() const
{
  return Elem(0);
}

inline char  PDString::Lastchar() const
{
  return Elem(Length() - 1);
}


inline  ostream& operator<<(ostream& s, const PDString& x)
{
   s << x.Chars(); return s;
}

// a zillion comparison operators
int operator==(const PDString& x, const PDString& y);
int operator!=(const PDString& x, const PDString& y);
int operator>(const PDString& x, const PDString& y);
int operator>=(const PDString& x, const PDString& y);
int operator<(const PDString& x, const PDString& y);
int operator<=(const PDString& x, const PDString& y);
int operator==(const PDString& x, const PDSubString&  y);
int operator!=(const PDString& x, const PDSubString&  y);
int operator>(const PDString& x, const PDSubString&  y);
int operator>=(const PDString& x, const PDSubString&  y);
int operator<(const PDString& x, const PDSubString&  y);
int operator<=(const PDString& x, const PDSubString&  y);
int operator==(const PDString& x, const char* t);
int operator!=(const PDString& x, const char* t);
int operator>(const PDString& x, const char* t);
int operator>=(const PDString& x, const char* t);
int operator<(const PDString& x, const char* t);
int operator<=(const PDString& x, const char* t);
int operator==(const PDSubString& x, const PDString& y);
int operator!=(const PDSubString& x, const PDString& y);
int operator>(const PDSubString& x, const PDString& y);
int operator>=(const PDSubString& x, const PDString& y);
int operator<(const PDSubString& x, const PDString& y);
int operator<=(const PDSubString& x, const PDString& y);
int operator==(const PDSubString& x, const PDSubString&  y);
int operator!=(const PDSubString& x, const PDSubString&  y);
int operator>(const PDSubString& x, const PDSubString&  y);
int operator>=(const PDSubString& x, const PDSubString&  y);
int operator<(const PDSubString& x, const PDSubString&  y);
int operator<=(const PDSubString& x, const PDSubString&  y);
int operator==(const PDSubString& x, const char* t);
int operator!=(const PDSubString& x, const char* t);
int operator>(const PDSubString& x, const char* t);
int operator>=(const PDSubString& x, const char* t);
int operator<(const PDSubString& x, const char* t);
int operator<=(const PDSubString& x, const char* t);

#ifdef __BORLANDC__
#pragma warn -inl.
#endif

#endif
