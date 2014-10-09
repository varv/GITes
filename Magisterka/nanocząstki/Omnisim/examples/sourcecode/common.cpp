// ----------------------------------------------------------------------------
// FILE
//    common.cpp
// REVISION
//    $Id: common.cpp,v 1.3 2004/01/23 14:31:07 tfelici Exp $
// COPYRIGHT
//    (c) Photon Design
// ----------------------------------------------------------------------------

static int read (int sock,void* buf, int len)
{
  //if (rtmo != -1 && is_readready (rtmo)==0) return 0;
  int	rval;
  if ((rval = ::recv (sock, (char*) buf, len,0)) == -1)
	  cerr<<"socket error: recv failed"<<endl;
  return (rval==0) ? EOF: rval;
}

static unsigned int write(int sock,const void* buf, int len)
{
  //if (stmo != -1 && is_writeready (stmo)==0) return 0;
  
  int	wlen=0;
  while(len>0) {
    int	wval;
    if ((wval = ::send (sock, (char*) buf, len,0)) == -1) {
      cerr<< "socket error: send failed"<<endl;
      return wval;
    }
    len -= wval;
    wlen += wval;
  }
  return wlen; // == len if every thing is all right
}

#define INTBUFFSIZE 20
enum {MSGOK=0,CONNECTIONBROKEN,MSGERROR};

/*
	message format:
   message=msglen+message;
   msglen is a string INTBUFFSIZE long and contains strlen(message)
   e.g.
   message="17                  this is a command"
*/

static void sendData(int sock,const void* data,unsigned int datalen) {
   if (sock==-1) return;
#if DEBUG
	cout<<"sending message:"<<command<<endl;
#endif
   unsigned int totmsgsize=datalen+INTBUFFSIZE;

   static char* sbuff=NULL;
   static unsigned int sbuffsize=0;
   if (sbuffsize<totmsgsize) {
   	if (sbuff) delete[] sbuff;
      sbuffsize=totmsgsize;
      sbuff=new char[sbuffsize];
   }

   sprintf(sbuff,"%d",datalen);
   memcpy(sbuff+INTBUFFSIZE,data,datalen);
	if ( write(sock,sbuff,totmsgsize) < totmsgsize)
   	clog<<"sendMessage error!"<<endl;
}

//returns message in sbuff.
//sbuff is allocated automatically if no error occurred.
//sbuffsize is set to length of allocated memory pointed by sbuff
//which may be bigger than message size returned in sbuff
//rerturns 0 on success, 1 on connection broken, 2 on message receive error
static int getData(int sock,char*& sbuff,unsigned int& sbuffsize,unsigned int& recvdatalen) {
	recvdatalen=0;
   if (sock==-1) return MSGERROR;
   if (!sbuff) sbuffsize=0;
   //else get message
   static char intbuff[INTBUFFSIZE];
	size_t retflag=::read(sock,intbuff,INTBUFFSIZE);
   if (retflag==EOF || retflag==0) {
      return CONNECTIONBROKEN;
   } else if (retflag!=INTBUFFSIZE) {
   	clog<<"getMessage error! retbytes="<<retflag<<endl;
      return MSGERROR;
   }
   //reallocate sbuff memory if necessary
   unsigned int msgsize=atoi(intbuff);
   if (sbuffsize<msgsize) {
   	if (sbuff) delete[] sbuff;
      sbuffsize=msgsize;
      sbuff=new char[sbuffsize];
   }

   //read msgsize bytes from tcpip buffer.
	unsigned int bytesread=0;
	unsigned int bytesleft=msgsize;
   while (1) {
      retflag=::read(sock,sbuff+bytesread,bytesleft);
      if (retflag==0) {
         return CONNECTIONBROKEN;
      } else if (retflag==EOF) {
         clog<<"getMessage error 2!: bytes read="<<bytesread<<" expected size="<<msgsize<<endl;
         return MSGERROR;
      }
      bytesread+=retflag;
      bytesleft-=retflag;
      if (bytesread>msgsize) {
         clog<<"CODEERROR in getMessage: bytes read="<<bytesread<<" expected size="<<msgsize<<endl;
         return MSGERROR;
      }
      if (bytesread==msgsize) break;
   }
	recvdatalen=msgsize;
   return MSGOK;
}

#undef INTBUFFSIZE
