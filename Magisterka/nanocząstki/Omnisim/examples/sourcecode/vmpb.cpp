/**************************************************************************
  class VMProfile - vector mode profile object
**************************************************************************/

/*
This file contains the basic C++ code which will show you how to load a FIMMWAVE
.BMF mode profile file from disk. An example of its usage is given in the main()
function at the end of this file.

The function VMProfile::readBMF() will allocate the required memory. By default,
FIMMWAVE saves all 6 field components in the raw file - Ex,Ey,Ez,Hx,Hy,Hz.
However you can test whether a particular component is available using the call:
     test=VMProfile::available(AMP_X);

Once the profile is loaded, you can access the profile using set_line(int ix) to
set the x coordinate; the vertical profile at x is then given by the public
pointers rEx,rEy,rEz,rHx,rHy,rHz for the real components, and if the mode is complex,
by the pointers iEx,iEy... for the imaginary components.

UNITS
------------------------
The fields are scaled such that E.H is in units of W/um2

Coordinate System
-----------------------
The BMF data is stored in FIMMWAVE's left hand coordinate system. E.g. if Ex and Hy are positive
then power is travelling in the -z direction.

ARRAY CLASSES
------------------------
VMProfile uses an array class Array2d. You may have your own array classes, e.g. you
may use the Standard Template Library arrays. If so, you will probably want to delete
the Array2d class and its usage and rewrite this file.

INTEGER SIZE
------------------------
FIMMWAVE is compiled with a 32bit (4byte) integer - if your compiler is using a
2byte int (i.e. sizeof(int)==2) then you will have to modify this code accordingly.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>


class Array2d {
public:
		Array2d(void);                              // constructor
		~Array2d();                                 // destructor
		void alloc(int nrl,int nrh,int ncl,int nch);     // allocate array size; row=(nrl..nrh), col=(ncl..nch)
		void unalloc(void);                              //unallocate storage
		bool alloced(void);                         // returns true if array is allocated
      operator float**() {return refptr;}  // this returns a pointer to a vector of pointers, such that
                  // refptr[i] is a pointer to the i-th row and refptr[i][j] would give the (i,j)'th
                  // element

      float **refptr,*buff;
      int _nrl,_nrh,_ncl,_nch;
};

Array2d::Array2d() {
	refptr=NULL;
   buff=NULL;
}

Array2d::~Array2d() {
	unalloc();
}

void Array2d::alloc(int nrl,int nrh,int ncl,int nch)
{
	int i,colsize;
	float *block;
   unalloc();
   _nrl=nrl;_nrh=nrh;_ncl=ncl;_nch=nch;

	refptr=new float*[nrh-nrl+1];
	refptr-=nrl;
	colsize=(nch-ncl+1);
	buff=block=new float[colsize*(nrh-nrl+1)];
	for(i=nrl;i<=nrh;i++) {
		refptr[i]=block-ncl;
		block+=colsize;
	}
}

void Array2d::unalloc(void) {
	if (refptr) delete[] refptr;refptr=NULL;
	if (buff)   delete[] buff;  buff=NULL;
}

bool Array2d::alloced(void) {
	if (buff!=NULL) return true;
   else return false;
}

struct Error {
   int ier;
   char *msg;
   char msg80[200];
   operator int() {return ier;}
};

Error error;

enum {AMP_INIT,AMP_POINT,AMP_EX=4,AMP_HX=8,AMP_EY=16,
        AMP_HY=32,AMP_EZ=64,AMP_HZ=128};

#define AMP_XY (AMP_EX|AMP_EY|AMP_HX|AMP_HY)
#define AMP_XYZ (AMP_XY|AMP_EZ|AMP_HZ)
#define AMP_PZ (AMP_XY|256)

#define lprintf printf

FILE *efopen(const char *filename, const char *mode) {
	FILE *fp=fopen(filename,mode);
   if (fp==NULL) {
   	lprintf("Failed to open file");
      exit(1);
   }
}

/************************************************************************
 Defines the region dimensions, grid spacing etc for a VMProfile
************************************************************************/
// IMPORTANT: must NOT use virtual functions - not compatible with memset(0)
struct VMPdim {
   VMPdim() {nxseg=nyseg=0;}
   int operator==(const VMPdim&d2);
   int nxseg,nyseg;
   double xmin,xmax,ymin,ymax;
};

/************************************************************************
 Holds a 2d (vectorial) (real) mode profile of a WGuide mode
************************************************************************/
class VMProfile {
public:
   VMProfile();
   ~VMProfile();
   void writeBMF(const char *filename,int cpt);
   void readBMF(const char *filename);
   bool available(VMPdim &dim,int cpt);
   void set_line(int ix);
   void unalloc();
   void alloc(VMPdim &dim,bool cxalloc=false);
   VMPdim dim;
   void *unused1;
   double x_offset,y_offset;
   double Amin,Amax;
   complex beta;
   double lambda;
   float *rEx,*rEy,*rEz,*rHx,*rHy,*rHz,*rNe;
   float *iEx,*iEy,*iEz,*iHx,*iHy,*iHz,*iNe;
   bool alloced;
   bool iscomplex;
   bool normd;
   int profileOK;
protected:
   Array2d rAEx,rAHx,rAEy,rAHy,rAEz,rAHz,rANe;
   Array2d iAEx,iAHx,iAEy,iAHy,iAEz,iAHz,iANe;
};


int VMPdim::operator==(const VMPdim&d2) {
	return !memcmp(this,&d2,sizeof(VMPdim));
}

//================================================================================

//================================================================================


VMProfile::VMProfile() {
	alloced=normd=false;
	x_offset=y_offset=0.0;
	profileOK=0;
}

VMProfile::~VMProfile() {
	if (alloced) unalloc();
}


void VMProfile::alloc(VMPdim &_dim,bool cxalloc) {
	int nxseg,nyseg;
// if we can avoid reallocation then we will do so
// this is useful if VMProfile holds a partial profile (eg only Ex)
// and we want to add other parts to it.
   if (alloced && _dim==dim) return;
	if (alloced) unalloc();
	dim=_dim;
   nxseg=dim.nxseg;
	nyseg=dim.nyseg;
   rAEx.alloc(0,nxseg,0,nyseg);
   if (!rAEx.alloced()) goto fail;
   rAHx.alloc(0,nxseg,0,nyseg);
   if (!rAHx.alloced()) goto fail;
   rAEy.alloc(0,nxseg,0,nyseg);
   if (!rAEy.alloced()) goto fail;
   rAHy.alloc(0,nxseg,0,nyseg);
   if (!rAHy.alloced()) goto fail;
   rAEz.alloc(0,nxseg,0,nyseg);
   if (!rAEz.alloced()) goto fail;
   rAHz.alloc(0,nxseg,0,nyseg);
   if (!rAHz.alloced()) goto fail;
   rANe.alloc(0,nxseg,0,nyseg);
   if (!rANe.alloced()) goto fail;
   if (cxalloc) {
      iAEx.alloc(0,nxseg,0,nyseg);
      if (!iAEx.alloced()) goto fail;
      iAHx.alloc(0,nxseg,0,nyseg);
      if (!iAHx.alloced()) goto fail;
      iAEy.alloc(0,nxseg,0,nyseg);
      if (!iAEy.alloced()) goto fail;
      iAHy.alloc(0,nxseg,0,nyseg);
      if (!iAHy.alloced()) goto fail;
      iAEz.alloc(0,nxseg,0,nyseg);
      if (!iAEz.alloced()) goto fail;
      iAHz.alloc(0,nxseg,0,nyseg);
      if (!iAHz.alloced()) goto fail;
      iANe.alloc(0,nxseg,0,nyseg);
      if (!iANe.alloced()) goto fail;
      iscomplex=true;
	} else {
   	iscomplex=false;
   }
	profileOK=0;
	alloced=true;
   return;
fail:
   unalloc();
   error.ier=1;
   error.msg="Not enough memory to build mode profile";
}

void VMProfile::unalloc() {
   rAEx.unalloc();
   rAEy.unalloc();
   rAEz.unalloc();
   rAHx.unalloc();
   rAHy.unalloc();
   rAHz.unalloc();
   rANe.unalloc();
   iAEx.unalloc();
   iAEy.unalloc();
   iAEz.unalloc();
   iAHx.unalloc();
   iAHy.unalloc();
   iAHz.unalloc();
   iANe.unalloc();
	memset(&dim,0,sizeof(VMPdim));
	profileOK=0;
   alloced=false;
}

/***********************************************************************
 returns true if the object has the requested field component available
 cpt = AMP_EX, AMP_EY, etc
***********************************************************************/
bool VMProfile::available(VMPdim &_dim,int cpt) {
	cpt&=AMP_XYZ;
	if (!alloced) return false;
	if ((profileOK&cpt)!=cpt) return false;
	if (!(dim==_dim)) return false;
	return true;
}

/***********************************************************
 retrieves the ix'th line of 2D mode profile from XMS store
************************************************************/
void VMProfile::set_line(int ix) {
   rEx=rAEx[ix];
   rEy=rAEy[ix];
   rEz=rAEz[ix];
   rHx=rAHx[ix];
   rHy=rAHy[ix];
   rHz=rAHz[ix];
   rNe=rANe[ix];
   if (iscomplex) {
      iEx=iAEx[ix];
      iEy=iAEy[ix];
      iEz=iAEz[ix];
      iHx=iAHx[ix];
      iHy=iAHy[ix];
      iHz=iAHz[ix];
      iNe=iANe[ix];
   }
}

void RunSignal(int);


struct BMFHeader {
static char *Sig;
   char sig[32];
   VMPdim dim;
   complex beta;
   double lambda;
   int components;
   short iscomplex,isEmode; // older versions were int iscomplex
};

char *BMFHeader::Sig="<fmm_rawprofile(1.2)>";

void VMProfile::writeBMF(const char *filename,int cpt) {
   int ix,nyseg1;
   BMFHeader rfh;
   FILE *fp=efopen(filename,"wb");
   if (error) return;
   if (!available(dim,cpt)) {
      error.ier=1;
      error.msg="Code error in VMProfile::writeBMF";
      return;
   }
   strcpy(rfh.sig,BMFHeader::Sig);
   rfh.dim=dim;
   rfh.components=cpt;
   rfh.beta=beta;
   rfh.lambda=lambda;
   rfh.iscomplex=iscomplex;
   rfh.isEmode=isEmode;
   fwrite((char*)&rfh,sizeof(rfh),1,fp);
   nyseg1=dim.nyseg+1;
   for (ix=0;ix<=dim.nxseg;ix++) {
      set_line(ix);
      fwrite((char*)rNe,sizeof(float),nyseg1,fp);
      if (iscomplex) fwrite((char*)iNe,sizeof(float),nyseg1,fp);
      if (cpt&AMP_EX) {
      	fwrite((char*)rEx,sizeof(float),nyseg1,fp);
         if (iscomplex) fwrite((char*)iEx,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_EY) {
      	fwrite((char*)rEy,sizeof(float),nyseg1,fp);
         if (iscomplex) fwrite((char*)iEy,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_EZ) {
      	fwrite((char*)rEz,sizeof(float),nyseg1,fp);
         if (iscomplex) fwrite((char*)iEz,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_HX) {
      	fwrite((char*)rHx,sizeof(float),nyseg1,fp);
         if (iscomplex) fwrite((char*)iHx,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_HY) {
      	fwrite((char*)rHy,sizeof(float),nyseg1,fp);
         if (iscomplex) fwrite((char*)iHy,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_HZ) {
      	fwrite((char*)rHz,sizeof(float),nyseg1,fp);
         if (iscomplex) fwrite((char*)iHz,sizeof(float),nyseg1,fp);
      }
   }
   fclose(fp);
   lprintf("%s raw file written\n",filename);
}

void VMProfile::readBMF(const char *filename) {
   int ix,nyseg1,cpt;
   BMFHeader rfh;
   VMPdim dim;
   FILE *fp=efopen(filename,"rb");
   if (error) return;
   fread((char*)&rfh,sizeof(rfh),1,fp);
   if (strcmp(rfh.sig,BMFHeader::Sig)!=0) {
      error.ier=1;
      sprintf(error.msg80,"Expected %s signature in BMF file\n"
         "But was %s",BMFHeader::Sig,rfh.sig);
   }
   dim=rfh.dim;
   cpt=rfh.components;
   beta=rfh.beta;
   lambda=rfh.lambda;
   iscomplex=(rfh.iscomplex!=0);
   isEmode=(rfh.isEmode!=0);
   nyseg1=dim.nyseg+1;
   alloc(dim,iscomplex);
   profileOK=rfh.components;
   for (ix=0;ix<=dim.nxseg;ix++) {
      set_line(ix);
      fread((char*)rNe,sizeof(float),nyseg1,fp);
      if (iscomplex) fread((char*)iNe,sizeof(float),nyseg1,fp);
      if (cpt&AMP_EX) {
      	fread((char*)rEx,sizeof(float),nyseg1,fp);
         if (iscomplex) fread((char*)iEx,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_EY) {
      	fread((char*)rEy,sizeof(float),nyseg1,fp);
         if (iscomplex) fread((char*)iEy,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_EZ) {
      	fread((char*)rEz,sizeof(float),nyseg1,fp);
         if (iscomplex) fread((char*)iEz,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_HX) {
      	fread((char*)rHx,sizeof(float),nyseg1,fp);
         if (iscomplex) fread((char*)iHx,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_HY) {
      	fread((char*)rHy,sizeof(float),nyseg1,fp);
         if (iscomplex) fread((char*)iHy,sizeof(float),nyseg1,fp);
      }
      if (cpt&AMP_HZ) {
      	fread((char*)rHz,sizeof(float),nyseg1,fp);
         if (iscomplex) fread((char*)iHz,sizeof(float),nyseg1,fp);
      }
   }
   fclose(fp);
   lprintf("%s raw file loaded\n",filename);
}


/**************************************************************************
  main() function to test your compilation
**************************************************************************/

void main(void) {
	int ix,iy,nx,ny;
	VMProfile vmp;
   vmp.readBMF("mode.bmf");
   lprintf("normd: %s iscomplex: %s\n",(vmp.normd?"YES":"NO"),(vmp.iscomplex?"YES":"NO"));
   lprintf("nxseg,nyseg=%d %d\n",vmp.dim.nxseg,vmp.dim.nyseg);
   lprintf("xmin,xmax=%.5g %.5g\n",vmp.dim.xmin,vmp.dim.xmax);
   lprintf("ymin,ymax=%.5g %.5g\n",vmp.dim.ymin,vmp.dim.ymax);
   lprintf("beta={%.5g %.5g} lambda=%.5g\n",vmp.beta,vmp.lambda);
   nx=vmp.dim.nxseg;ny=vmp.dim.nyseg;
   for (ix=0;ix<=nx;ix+=nx/10) {
   	vmp.set_line(ix);
	   for (iy=0;iy<=ny;iy+=ny/8) {
      	float p=(vmp.rEx[iy]*vmp.rHy[iy] - vmp.rEy[iy]*vmp.rHx[iy]);
         lprintf("%8.1e",p);
      }
      lprintf("\n");
   }
   lprintf("press RETURN");
   getchar();
}
