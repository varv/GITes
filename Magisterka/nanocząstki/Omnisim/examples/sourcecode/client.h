// ----------------------------------------------------------------------------
// FILE
//    client.h
// REVISION
//    $Id: client.h,v 1.7 2004/02/05 13:18:45 tfelici Exp $
// COPYRIGHT
//    (c) Photon Design
// ----------------------------------------------------------------------------

#ifndef PDCLIENT_H
#define PDCLIENT_H

class Client {
public:
	Client();
   virtual ~Client();
   void CloseConnection();
	bool Connect(int port,const char* serverName="localhost");
	const char* sendMessage(const char* command);
	const char* sendData(unsigned int& recvdatalen,const void* data,unsigned int datalen);
   bool IsConnected() {return (sock ? true:false);}
private:
	int sock;
   //used in getmessage
   char* sbuff;
   unsigned int sbuffsize;
};

/*************  VARIOUS UTILITIES *********************/

class pcomplex {
public:
   pcomplex() {re=im=0;}
   pcomplex(double r,double i) {re=r;im=i;}
   pcomplex(double r) {re=r;im=0;}
   double re,im;
};

class Carray {
private:
   pcomplex *dptr; // defined s.t. &(AA[i][j])==&(dptr[i*ral+j])
   int _dim1,_dim2;
public:
	Carray(void) {Init();}
	Carray(int n1,int n2) {Init();alloc(n1,n2);}
	void alloc(int dim1,int dim2);
	virtual ~Carray() {unalloc();}
	int dim1(void) const {
   	if (!alloced()) return 0;
		else return _dim1;
   }
	int dim2(void) const {
   	if (!alloced()) return 0;
      return _dim2;
   }
   bool alloced() const {return dptr?true:false;}
   pcomplex& operator()(int i,int j);
   const pcomplex& operator()(int i,int j) const;
private:
	const Carray& operator=(const Carray& a) {return *this;}
	void Init(void);
	void unalloc();
};

// returns values contaned in the LAST return value from fmmwave.
// if there is only one value, it is stored in array(1,1).
// if fimmwave returns an array (call it fmmarray) with lower index bounds l1,l2
// then the resulting array will have components:
// array(i,j)=fmmarray[i+l1-1][j+l2+1]
// on success it returns NULL, else an error message
const char* GetVal(Carray& array,const char* s);

#endif //PDCLIENT_H
