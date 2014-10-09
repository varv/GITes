// ----------------------------------------------------------------------------
// FILE
//    client.cpp
// REVISION
//    $Id: client.cpp,v 1.6 2004/02/05 13:18:45 tfelici Exp $
// COPYRIGHT
//    (c) Photon Design
// ----------------------------------------------------------------------------

#include "client.h"

#include <stdio.h>
#include <iostream>
using std::istream;
using std::ostream;
using std::endl;
using std::ends;
using std::cout;
using std::cerr;
using std::clog;
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/uio.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#define DEBUG 0

#include "common.cpp"

Client::Client() {
	sock=-1;
	sbuff=NULL;
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD( 1, 1 ), &wsaData))
		cerr<<"socket error: WSAStartup failed"<<endl;
#endif
}

void Client::CloseConnection() {
	if (sock!=-1) {
		::closesocket(sock);
		sock=-1;
	}
}

Client::~Client() {
	CloseConnection();
   if (sbuff) {delete [] sbuff;sbuff=NULL;}
	sbuffsize=0;
#ifdef WIN32
	WSACleanup();
#endif
}

bool Client::Connect(int port,const char* serverName) {
	CloseConnection();
	if (!serverName) {
   	cerr<<"CODEERROR in Client::Init: serverName=NULL";
      return false;
   }
	if(sock==-1) {
		sock = ::socket (AF_INET, SOCK_STREAM, 0);
	}
   
	sockaddr_in sa;
	sa.sin_port	= htons(port);

	//check that sd.GetServerName() returns a valid IP address.
   //if not, try treating it as a IP name
	if ( (sa.sin_addr.s_addr = inet_addr(serverName)) != -1) {
#if DEBUG
		cout << "connecting to server with address "<<serverName<<"..."<< endl;
#endif
		sa.sin_family = AF_INET;
	} else {
#if DEBUG
		cout << "connecting to server with name "<<serverName<<"..."<< endl;
#endif
		hostent* hp = gethostbyname(serverName);
		if (hp == 0) {
			cerr<< "socket error: could not find host " << serverName <<endl;
			CloseConnection();
			return false;
		}
		memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
		sa.sin_family = hp->h_addrtype;
	}
	if (::connect(sock, (const sockaddr*)&sa, sizeof(sa)) == -1) {
		cerr<<"Client error: could not find server on "<<serverName<<" at port "<<port<<endl;
		CloseConnection();
		return false;
	}

	return true;
}

const char* Client::sendMessage(const char* command) {
	::sendData(sock,command,strlen(command)+1);
	unsigned int recvdatalen;
	int flag=::getData(sock,sbuff,sbuffsize,recvdatalen);
   return (flag==MSGOK ? sbuff:NULL);
}

const char* Client::sendData(unsigned int& recvdatalen,const void* data,unsigned int datalen) {
	::sendData(sock,data,datalen);
	int ok=::getData(sock,sbuff,sbuffsize,recvdatalen);
   return (ok==0 ? sbuff:NULL);
}

/*************  VARIOUS UTILITIES *********************/

//-----------------------------------------------
// class Carray
//-----------------------------------------------

void Carray::Init(void) {
   dptr=NULL;
   _dim1=_dim2=0;
}

pcomplex& Carray::operator()(int i,int j) {
	static pcomplex dummy;
	if (i<1 || i>_dim1 || j<1 || j>_dim2) {
   	cerr<<"("<<i<<","<<j<<") out of range!"<<endl;
      return dummy;
   }
   return dptr[i*_dim2 + j];
}
const pcomplex& Carray::operator()(int i,int j) const {
	static pcomplex dummy;
	if (i<1 || i>_dim1 || j<1 || j>_dim2) {
   	cerr<<"("<<i<<","<<j<<") out of range!"<<endl;
      return dummy;
   }
   return dptr[i*_dim2 + j];
}

void Carray::alloc(int dim1,int dim2) {
   if (dim1<=0 || dim2<=0) {
   	unalloc();
      return;
   }
   int size=dim1*dim2;
   if (_dim1!=dim1 || _dim2!=dim2) {
      unalloc();
      _dim1=dim1;_dim2=dim2;
      dptr=new pcomplex[size];
      dptr-=1+dim2; // s.t. dptr[i*dim2+j] is Carray[i][j]
   }
   //zero array
   for (int i=1;i<=_dim1;i++) for (int j=1;j<=_dim2;j++) {
      pcomplex& c=operator()(i,j);
      c.re=c.im=0;
   }
}

void Carray::unalloc() {
	if (dptr) {
      dptr+=1+_dim2;
      delete[] dptr;
   }
   Init();
}

#include "pdstring.h"

static int atoI(const char* s,int& err) {
	err=0;
	int v;
	if (sscanf(s,"%d",&v)!=1) err=1;
	return v;
}

static double atoD(const char* s,int& err) {
	err=0;
	double v;
	if (sscanf(s,"%lf",&v)!=1) err=1;
	return v;
}

static const char* GetVal(pcomplex& pc,int& i1,int& i2,const char* s) {
	PDString sval=s,temp;
   int err=0;
   if (sval.Contains("[") && sval.Contains("][") && sval.Contains("]")) {
   	sval=sval.After("[");
      i1=atoI(sval.Before("][").Chars(),err);
      if (err!=0) return "parse error";
   	sval=sval.After("][");
      i2=atoI(sval.Before("]").Chars(),err);
      if (err!=0) return "parse error";
   	sval=sval.After("]");
   } else if (sval.Contains("[") && sval.Contains("]")) {
   	i2=1;
   	sval=sval.After("[");
      i1=atoI(sval.Before("]").Chars(),err);
      if (err!=0) return "parse error";
   	sval=sval.After("]");
   } else {
   	i1=i2=1;
   }
   if (sval.Contains("(") && sval.Contains(")")) {
   	sval=sval.After("(");
   	sval=sval.Before(")");
      pc.re=atoD(sval.Before(",").Chars(),err);
      pc.im=atoD(sval.After(",").Chars(),err);
   } else {
      pc.re=atoD(sval,err);
      pc.im=0;
   }
   if (err!=0) return "parse error";
   return NULL;
}

static void minimisestring(PDString& s) {
   while (s.Length()>0 && (
   	s[0]==' '	||
      s[0]=='\t'	||
      s[0]=='\r'	||
      s[0]=='\n'	)
   	) s.Del(0,1);
   while (s.Length()>0 && (
          s[(int)s.Length()-1]==' '	||
          s[(int)s.Length()-1]=='\t' 	||
          s[(int)s.Length()-1]=='\r' 	||
          s[(int)s.Length()-1]=='\n' 	)
   	) s.Del((int)s.Length()-1,1);
}

const char* GetVal(Carray& array,const char* s) {
	PDString sval=s;
   //get last command
   while (sval.Contains("RETVAL:")) sval=sval.After("RETVAL:");
   if (sval.Contains("ERROR:")) {
   	return "fimmwave returned an ERROR";
   }
   pcomplex val;
   //get dimensions
   minimisestring(sval);
   if (sval=="") return "string is empty";
   PDString item,temp=sval;
   int l1=0,l2=0,u1=0,u2=0;
   bool firstTime=true;
   while (temp!="") {
   	if (temp.Contains("\n")) {
      	item=temp.Before("\n");
	      temp=temp.After("\n");
      } else {
      	item=temp;
         temp="";
      }
	   minimisestring(item);
      if (item=="") continue;
      int i1,i2;
      const char* errmsg=GetVal(val,i1,i2,item);
      if (errmsg) return errmsg;
      if (firstTime) {
         firstTime=false;
         l1=u1=i1;u2=l2=i2;
      } else {
         l1=(l1<i1?l1:i1);
         l2=(l2<i2?l2:i2);
         u1=(u1>i1?u1:i1);
         u2=(u2>i2?u2:i2);
      }
   }
   //calculate dimensions and fill array
   array.alloc(u1-l1+1,u2-l2+1);
   temp=sval;
   while (temp!="") {
   	if (temp.Contains("\n")) {
      	item=temp.Before("\n");
	      temp=temp.After("\n");
      } else {
      	item=temp;
         temp="";
      }
	   minimisestring(item);
      if (item=="") continue;
      int i1,i2;
      const char* errmsg=GetVal(val,i1,i2,item);
      if (errmsg) return errmsg;
      array(i1-l1+1,i2-l2+1)=val;
   }
   return NULL;
}
