#ifdef WIN32
#include <windows.h>
#endif

//#include <stdlib.h>

#include <iostream>
#include "client.h"
#include <stdio.h>


int RunCommand(char* cmd,char*  args) {
   char *strTemp = new char[strlen(cmd)+strlen(args)+20];
#ifdef _Windows
	int retVal;
	STARTUPINFO suinfo;
   PROCESS_INFORMATION processInfo;
   memset(&suinfo,0,sizeof(suinfo));
   suinfo.cb=sizeof(suinfo);
   strcpy(strTemp,cmd);
   strcat(strTemp,args);
   // 1st arg in args should be argv[0], i.e. the command ???
	retVal=CreateProcess(cmd,strTemp,NULL,NULL,
   	FALSE,CREATE_NEW_CONSOLE|NORMAL_PRIORITY_CLASS,NULL,NULL,
      &suinfo,&processInfo);
   delete[] strTemp;
   return (retVal==0?1:0);
#else
   strcpy(strTemp,cmd);
   strcat(strTemp," ");
   strcat(strTemp,args);
   strcat(strTemp," & ");
   int blob = system(strTemp);
   delete[] strTemp;
	return blob;
#endif
}

#define PORT_NO 101

int main (int ac, char** av)
{
   if (ac < 3) {
	  std::cout << "USAGE: " << av[0] << " <portNo> <hostname> "<<std::endl;
      std::cout << "<portNo> = port number: port number on which fimmwave is (or will be) talking"<<std::endl;
      std::cout << "<hostname> = machine IP address on which fimmwave is running"<<std::endl;
      std::cout << std::endl;
      std::cout << "e.g."<<std::endl;
      std::cout << std::endl;
      std::cout << "To connect to fimmwave locally on port 101 type:"<<std::endl;
      std::cout << "   "<<av[0] << " 101 localhost"<<std::endl;
      std::cout << "To talk to an existing fimmwave running on 214.1.1.5 via port 101, type:"<<std::endl;
		std::cout << "   "<<av[0] << " 101 214.1.1.5"<<std::endl;
      std::cout << "To connect to an existing fimmwave running on the machine called Bob via port 101, type:"<<std::endl;
      std::cout << "   "<<av[0] << " 101 Bob"<<std::endl;
      return 1;
   }
   //get port number;
   int err=0;
   int portNo=atoi(av[1]);

	Client client; //instantiate a client

	while (!client.Connect(portNo,av[2])){}
   char s[1000];
/***************************************************************************
	if we got this far, then we're ready to talk to fimmwave!
***************************************************************************/

	while (!feof(stdin)) {
   	char command[1000];
      s[0] = 0;
      command[0]=0;

   	//input a command
      std::cout<<"\nCOMMAND? (type \"exit\" to finish) "<<std::endl;
		fgets(s,1000,stdin);
      std::cout<<s;
      if (strlen(s)==0) return 0;
      strcpy(command,s);
      char buff[200];sscanf(command,"%20s",buff);
      if (memicmp(buff,"exit",4)==0) break;

   	//send command to fimmwave,
   	//wait till we get an answer, and print out result
      const char* received=client.sendMessage(command);
   /*
      syntax of received: FIMWAVE will return the output once execution of the
      command (or command batch) is completed.
      The return string consists of a series of lines of the following format:
      RETVAL: output string
      or
      ERROR: error message
      The former will be appended for every command sent that makes FIMMWAVE
      return a nonempty string. The return string is exactly the same as the
      one written to the output pane in the FIMMWAVE command window.
      The latter will be appended whenever an error occurs.
   */
      if (!received) {
      	std::cout<<"connection broken?"<<std::endl;
         exit(1);
      }
      std::cout<<std::endl;
      std::cout<<"MESSAGE RECEIVED:"<<std::endl;
      //std::cout<<std::endl;
      std::cout<<received<<std::endl;
   /*
      use GetVal to extract numerical values, if there were any,
      from the last command (i.e. the last RETVAL: statement) - see notes above.
   */
      Carray array;
      const char* errmsg=GetVal(array,received);
      if (!errmsg) {
	      std::cout<<"NUMERICAL OUTPUT:"<<std::endl;
         for(int i=1;i<=array.dim1();i++) for(int j=1;j<=array.dim2();j++) {
            std::cout<<"array("<<i<<","<<j<<") = ("<<array(i,j).re<<","<<array(i,j).im<<")"<<std::endl;
         }
      }
   }

   //if I started fimmwave, then send message to shut down fimmwave
   //if (IstartedFimmwave) client.sendMessage("fimmwave.exit");
	return 0;
}

