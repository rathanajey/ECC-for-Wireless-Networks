#include "security_packet.h"
#include "string.h"

int hdr_security_packet::offset_;
int arr[11][11];
int endseq;
int p=11;
int ey1x[32]={0};	// encrypted x co-ord
int ey1y[32]={0};      // encrypted y co-ord
int ey1;
int ey2[32]={0};       // encrypted data-id
int quo[32]={0};       // quotient
int git=0;             // counter for encryption
int fd=0;              // counter for decrypted text
int fe=0;              // counter for encrypted text
//char datadec[192]={0};       // holds decrypted data
//int dataenc[192]={0};  // holds encrypted data



static class Security_packetHeaderClass : public PacketHeaderClass {
public:
	Security_packetHeaderClass() : PacketHeaderClass("PacketHeader/Security_packet",sizeof(hdr_security_packet)) {
		bind_offset(&hdr_security_packet::offset_);
	}
} class_security_packethdr;


static class Security_packetClass : public TclClass {
public:
	Security_packetClass() : TclClass("Agent/Security_packet") {}
	TclObject* create(int, const char*const*) {
		return (new Security_packetAgent());
	}
} class_security_packet;


Security_packetAgent::Security_packetAgent() : Agent(PT_SECURITY_PACKET), seq(0), oneway(0)
{
	bind("packetSize_", &size_);
}




//Point Compression
int Security_packetAgent::ptcomp(int y)     // module for point compress
{
y=y%2;
return y;
}


//Point decompression
int Security_packetAgent::ptdecomp(int x,int y)
{
int z=((x*x*x)+x+6)%p;
int dy=z*z*z;
dy=dy%p;
if((y-dy)%2==0)
{
dy=dy;
}
else
{
dy=p-dy;
}
printf("Decompressed point is: %d,%d \n",x,dy);
return dy;
}



//ECC Encryption
void Security_packetAgent::encryption(int dataenc[])         // module for encryption
{
int interdataenc[192]={0};

int enci=0;
while(dataenc[enci]!=NULL)
{
int t=dataenc[enci];
int q=4;
int rand=2;
/*randomize();
int rand=1;                        // choosing the random hash value
do
{
rand=random(5);
}while(rand==0);
*/
int kpx;                           // x co-ord for point P
int kpy;                           // y co-ord for point P
int kqx;                           // x co-ord for point Q
int kqy;                           // y co-ord for point Q
int exo;                           // selective co-ord for data enc/dec
int kp=rand%endseq;                // Identifying point kP's sequence num
int ci;
for(ci=0;ci<=10;ci++)
{
for(int j=0;j<=10;j++)
{
if(arr[ci][j]==kp)
{
kpx=ci;
kpy=j;
}
}
}
printf("\n kpx,kpy:%d,%d \n",kpx,kpy);
int kq=(rand*q)%endseq;           // Identifying point kQ's sequence num
int i;
for(i=0;i<=10;i++)
{
for(int j=0;j<=10;j++)
{
if(arr[i][j]==kq)
{
kqx=i;
kqy=j;
}
}
}
printf("kqx,kqy:%d,%d \n",kqx,kqy);
exo=kqx;
int pcy=ptcomp(kpy);                  // calling point-compress
ey1x[git]=kpx;                         // x co-ord of kP
ey1y[git]=pcy;                         // point compressed y co-ord of kP
printf("exo:%d \n",exo);
//ey1=arr[kpx][pcy];
ey2[git]=(exo*t)%p;                    // calc data-id
quo[git]=t/p;
interdataenc[fe]=ey1x[git];
fe++;
interdataenc[fe]=ey1y[git];                 // store encrypted data
fe++;
interdataenc[fe]=quo[git];
fe++;
interdataenc[fe]=ey2[git];
fe++;
//printf("random hash value is:%d ",rand);
printf("The cipher text is: %d,%d  quo: %d  dtid: %d \n",ey1x[git],ey1y[git],quo[git],ey2[git]);
printf("interdataenc value:%d,%d,%d,%d \n",interdataenc[fe-4],interdataenc[fe-3],interdataenc[fe-2],interdataenc[fe-1]);
git++;
enci++;
}

printf("\n At Encryption module interdataenc  data encrypted is : \n");

enci=0;
for(enci=0;enci<fe;enci++)
{
printf("%d",interdataenc[enci]);
dataenc[enci]=interdataenc[enci];
}


printf("\n At Encryption module dataenc data encrypted is : \n"); 
enci=0;
for(enci=0;enci<fe;enci++)
{
printf("%d",dataenc[enci]);
}


}



//ECC Decryption
void Security_packetAgent::decryption(int datadec[])      // module for decryption
{
int  interdatadec[192]={0};

int deci=0;

while(datadec[deci]!=NULL)
{
int x=datadec[deci];
int y=datadec[deci+1];
int var=datadec[deci+3];
int quo=datadec[deci+2];

int key=4;
int mx;                         // x co-ord for intermediate point
int my;                         // y co-ord for intermediate point
int calx;                       // retrieved exo
int i;
int j;
int qu;                         // quotient
int pdy=ptdecomp(x,y);          // calling point-decompress
int medpt=(key*arr[x][pdy])%13; // calc sequence num of intermediate point
 qu=quo;
for(i=0;i<=10;i++)
{
for(j=0;j<=10;j++)
{
if(arr[i][j]==medpt)
{
mx=i;
my=j;
}
}
}
printf("Final decrypted point is:%d,%d \n",mx,my);
for(i=0;i<=25;i++)
{
if(((1-(mx*i))%p)==0)
{
calx=i;                           // identifying exo
}
}
int value=(var*calx)%p;          // calc encrypted data value
printf("Data quotient,value extracted= %d,%d \n ",qu,value);
printf("The decrypted text is: %c \n \n",char((p*qu)+value));
interdatadec[fd]=char((p*qu)+value);   // store into decrypted data array
fd++;
deci=deci+4;
}

deci=0;
for(deci=0;deci<fd;deci++)
{
datadec[deci]=interdatadec[deci];
}

}






int Security_packetAgent::command(int argc, const char*const* argv)
{

if (argc ==3) {

    if (strcmp(argv[1], "send") == 0) {
      // Create a new packet
      Packet* pkt = allocpkt();
      // Access the security packet header for the new packet:
      hdr_security_packet* hdr = hdr_security_packet::access(pkt);
      // Set the 'ret' field to 0, so the receiving node
      // knows that it has to generate an acknowledge packet
      hdr->ret = 0;
      hdr->seq = seq++;
      // Store the current time in the 'send_time' field
      hdr->send_time = Scheduler::instance().clock();
      // copy date to be sent to header

char hedch[192];      
strcpy(hedch, argv[2]);
	
printf("data arg is \n");
int hdrc=0;
while(hdrc<strlen(hedch))
{
printf("%c",hedch[hdrc]);
hdrc++;
}
printf("\n");

int i;
int z;                         // param z of EC eqn
int x;
int y;
int j;
int count=0;                   // counter for residue checks
int m;                         // modulus over z
int pcount=1;                  // counter for num of points
int lambda;                    // param Lambda for calc of co-ords x3,y3

for(i=0;i<=10;i++)              // declaring all array cells to zero
{
for(j=0;j<=10;j++)
{
arr[i][j]=0;
}
}

for(i=0;i<=10;i++)                // substituting for x
{
z=(i*i*i)+i+6;                    //x^3 + ax + b mod p
m=z%11;                           // modulus of elliptic curve for x

for(j=0;j<=10;j++)
{
count=0;

if((0-m)%p!=0)                  // quadratic residue check 1
{
count++;
}

if(((m-(j*j))%p)==0)           // quadratic residue check 2
{
count++;
}

if(count==2)                    // marking value for (x,y) co-ordinate
{
arr[i][j]=pcount++;
}
}

printf("z: %d mod 11=  %d \n",z,(z%p));
}

printf(" \n Possible X,Y co-ordinates: \n");
printf("X   Y \n");

for(i=0;i<=10;i++)
{
for(j=0;j<=10;j++)
{
if(arr[i][j]>=1)
{
printf("%d  %d \n ",i,j);
}
}
}
printf("\n  Number of points is :%d \n ",--pcount);
// Generating the sequence

int gi;                  // x co-ord of second multiple
int gj;                  // y co-ord of second multiple
int gx;
int gy;
int gl;                  // holds inverse function value
int gx3;                 // x co-ord of next multiple
int gy3;                 // y co-ord of next multiple
int seqcount=1;          // counter for sequence num

for(i=0;i<=10;i++)
{
for(j=0;j<=10;j++)
{
if(arr[i][j]==2)          // identifying second multiple
{
gi=i;
gj=j;
}
}
}
arr[gi][gj]=seqcount;     // setting sequence number

for(int seqec=1;seqec<=p;seqec++)
{
 if(seqec==1)                        // calc point 2-alpha
 {
  for(i=0;i<=25;i++)
  {
  if(((1-(i*2*gj))%p)==0)          // identify inverse value
  {
   gl=i;
  }
  }
  lambda=(((3*gi*gi)+1)*gl)%p;     // calc lambda for point 2
  gx3=((lambda*lambda) - gi - gi)%p;
  if(gx3<0)
  {
  gx3=p+gx3;                       // calc X3
  }
  gy3=((lambda*(gi-gx3)) - gj)%p;
  if(gy3<0)
  {
  gy3=p+gy3;                       // calc Y3
  }
  arr[gx3][gy3]=++seqcount;       // setting sequence number for X3,Y3
 }
 else
 {                                // calc 3-alpha and beyond
 for(i=0;i<=25;i++)
  {
  if(((1-(i*(gx3-gi)))%p)==0)
  {                               // calc inverse function
   gl=i;
  }
  }
  lambda=((gy3-gj)*gl)%p;
  gx3=((lambda*lambda) - gi - gx3)%p;
  if(gx3<0)
  {
  gx3=p+gx3;                      // calc X3
  }
  gy3=((lambda*(gi-gx3)) - gj)%p;
  if(gy3<0)
  {
  gy3=p+gy3;                      // calc Y3
  }
  arr[gx3][gy3]=++seqcount;       // set sequence num for X3,Y3
  }
endseq=seqcount;
++endseq;
}

printf("\n Sequence ends at:%d  ",endseq);
seqcount=1;
printf("The sequence of points is:\n");   // showing the final point sequence
for(seqcount=1;seqcount<=endseq;seqcount++)
{
for(i=0;i<=10;i++)
{
for(j=0;j<=10;j++)
{
if(arr[i][j]==seqcount)
{
printf("point %d  X:%d  Y:%d \n",endseq,i,j);
}
}
}
}


int ascii[192]={0};
char interasc[192];
strcpy(interasc,hedch);
int asc=0;


hdrc=0;
while(hdrc<strlen(hedch))
{
interasc[hdrc]=hedch[hdrc];
hdrc++;
}


printf("Data obtained for encryption in interasc is \n");
while(interasc[asc]!='\0')
{
printf("%c",interasc[asc]);
asc++;
}
printf("\n");

asc=0;
while(interasc[asc]!='\0')
{
ascii[asc]=int(interasc[asc]);
asc++;
}

encryption(ascii);

printf("Encrypted data is:");

asc=0;
for(asc=0;asc<fe;asc++)
{
printf("%d",ascii[asc]);
}

asc=0;
for(asc=0;asc<fe;asc++)
{
hdr->data[asc]=ascii[asc];
}



printf("\n header is:");
asc=0;
for(asc=0;asc<fe;asc++)
{
printf("%d",hdr->data[asc]);
}


printf("\n");


     
      // Send the packet
      send(pkt, 0);
      // return TCL_OK,check if func was called
    
      return (TCL_OK);    
    }    
    else if (strcmp(argv[1], "start-WL-brdcast") == 0) {
      Packet* pkt = allocpkt();
      
      hdr_ip* iph = HDR_IP(pkt);
      hdr_security_packet* ph = hdr_security_packet::access(pkt);
      strcpy(ph->hedd, "test");
      
      iph->daddr() = IP_BROADCAST;
      iph->dport() = iph->sport();
      ph->ret = 0;
      send(pkt, (Handler*) 0);
      return (TCL_OK);
    }

    else if (strcmp(argv[1], "oneway") == 0) {
      oneway=1;
      return (TCL_OK);
    }
  }
  
  
  // call the command() function for the base class
  return (Agent::command(argc, argv));
}


//-------------------------------
void Security_packetAgent::recv(Packet* pkt, Handler*)
{
  // Access the IP header for the received packet:
  hdr_ip* hdrip = hdr_ip::access(pkt);
  
  // Access the security packet header for the received packet:
  hdr_security_packet* hdr = hdr_security_packet::access(pkt);
  

   if ((u_int32_t)hdrip->daddr() == IP_BROADCAST) 
  {
    if (hdr->ret == 0)
    {
      
      printf("Recv BRDCAST Security_packet REQ : at %d.%d from %d.%d\n", here_.addr_, here_.port_, hdrip->saddr(), hdrip->sport());
      Packet::free(pkt);
      
      // create reply
      Packet* pktret = allocpkt();

      hdr_security_packet* hdrret = hdr_security_packet::access(pktret);
      hdr_cmn* ch = HDR_CMN(pktret);
      hdr_ip* ipret = hdr_ip::access(pktret);
      
      hdrret->ret = 1;
      
      // add brdcast address
      ipret->daddr() = IP_BROADCAST;
      ipret->dport() = ipret->sport();
      send(pktret, 0);    
    }
    else 
    {
      printf("Recv BRDCAST security_packet REPLY : at %d.%d from %d.%d\n", here_.addr_, here_.port_, hdrip->saddr(), hdrip->sport());
      Packet::free(pkt);
    }
    return;
  }
// end of broadcast mode
  
  if (hdr->ret == 0) 
  {
    // Send an 'echo'. First save the old packet's send_time
    double stime = hdr->send_time;
   
    char original_data[192];
    int encrypted_data[192];
   // strcpy(encrypted_data,hdr->data);
   // strcpy(original_data,hdr->data);
    int rcv_seq = hdr->seq;
     	
    char out[105];
    unsigned int newhash;
    char authenticate_result[50];
    int deec[192]={0};

    int dcn=0;

printf("Encrypted data at dest in deec \n");
for(dcn=0;dcn<fe;dcn++)
    {
     deec[dcn]=hdr->data[dcn];
     printf("%d",deec[dcn]);
    }
printf("\n");

     dcn--;

dcn=0;
for(dcn=0;dcn<fd;dcn++)
{
encrypted_data[dcn]=deec[dcn];
}


    // Performing decryption
     decryption(deec);
     
dcn=0;
for(dcn=0;dcn<fd;dcn++)
{
hdr->data[dcn]=deec[dcn];
}
     
printf("Decrypted data in Header:");
dcn=0;
for(dcn=0;dcn<fd;dcn++)
{
printf("%d",hdr->data[dcn]);
}

printf("\n");

dcn=0;
for(dcn=0;dcn<fd;dcn++)
{
printf("%c",char(hdr->data[dcn]));
}

printf("\n");


//  sprintf(out, "%s recv %d %3.1f ", name(), hdrip->src_.addr_ >> Address::instance().NodeShift_[1],
//			(Scheduler::instance().clock()-hdr->send_time) * 1000); 
    Tcl& tcl = Tcl::instance();
    tcl.eval(out);

    // Discard the packet
    Packet::free(pkt);
    // Create a new packet
    Packet* pktret = allocpkt();
    // Access the header for the new packet:
    hdr_security_packet* hdrret = hdr_security_packet::access(pktret);
    // Set the 'ret' field to 1, so the receiver won't send
    // another echo
    hdrret->ret = 1;
    // Set the send_time field to the correct value
    hdrret->send_time = stime;
    
    hdrret->rcv_time = Scheduler::instance().clock();
    hdrret->seq = rcv_seq;
    strcpy(hdrret->hedd, authenticate_result);//save data to new packet
    // Send the packet back to the originator
    send(pktret, 0);
  }
  else
  {
    char out[105];
     // showing at originator node when packet comes back	
    
//    sprintf(out, "%s recv %d %3.1f", name(), hdrip->src_.addr_ >> Address::instance().NodeShift_[1],
//			(Scheduler::instance().clock()-hdr->send_time) * 1000); 
    Tcl& tcl = Tcl::instance();
    tcl.eval(out);
    // Discard the packet
    Packet::free(pkt);
  }
}

