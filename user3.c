/*
** Author:Chia-Chen, Wu
** user3.c 
** Send the user name and password to login and send message to Supernode and get the message from Supernode.
** Some code from Beej's Guide book
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h> 

#include <arpa/inet.h>

#define PORT "21563" // TCP login server static port 
#define PHASE3TALKER "3563"//UDP supernode server static port
#define PHASE3LISTENER "3863"//UDP user3 static port

#define MAXDATASIZE 100 //max number of bytes in buffer we can get
#define MAXBUFLEN 100 //max number of bytes in buffer we can get
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//user3 listen the UDP port with supernode and get message from supernode. The UDP connection code is from Beej's Guide Book
int phase3listener()
{   //The following connection setup code comes from Beej's book
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	char *showFirstMessage;//Split the message getting from the supernode
	char *showSecondMessage;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PHASE3LISTENER, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

	addr_len = sizeof their_addr;
	//The following code will get the message from the supernode and display the receved message on the screen
	while(1)
	{   //If user get the finish signal, it will jump out the loop
	    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN , 0,
		    (struct sockaddr *)&their_addr, &addr_len)) == -1) {
		    perror("recvfrom");
		    exit(1);
	    }

	   if(numbytes == 1)
		   break;
	   else
	   {
		  buf[numbytes] = '\0';
          showFirstMessage = strtok(buf,"-");
		  showSecondMessage = strtok(NULL,"-");
	      printf("Phase 3: <User3> received the message %s", showSecondMessage);
	   }
    }
	close(sockfd);
    printf("End of Phase 3 for <User3>\n");
	return 0;
}

int phase3talker()
{   //The following connection setup code comes from Beej's book
  	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	int len;
	char getInfo[100];//Get the message from UserText3.txt
	struct hostent *host;
	struct sockaddr_in sin;

    FILE *fp;
	
	

    if((fp = fopen("UserText3.txt", "r")) == NULL){
		printf("Can not open UserText3.txt\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo("nunki.usc.edu", PHASE3TALKER, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}
	//Get the dynamic port number
	bind(sockfd,(struct sockaddr *)&sin,sizeof(sin));
    len = sizeof(sin);
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	//Print the IP address and port number
	host = gethostbyname("nunki.usc.edu");
	
	printf("End of Phase 1 for <User3>.\n");
	printf("Phase 3: <User3> has static UDP port 3863 IP address %s.\n",inet_ntoa(*((struct in_addr*)host->h_addr_list[0])));
	//The following code will get message and send it to supernode
	//Send signal to notice that it is user3 to send the message to the supernode
    if ((numbytes = sendto(sockfd, "abc", 3, 0, p->ai_addr, p->ai_addrlen)) == -1) {
		        perror("talker: sendto");
		        exit(1);
	        }
    //Send message line by line in the file
	while(fgets(getInfo, 100, fp))
	{
			if ((numbytes = sendto(sockfd, getInfo, 100, 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		        perror("talker: sendto");
		        exit(1);
	        }
			getsockname(sockfd,(struct sockaddr *)&sin, &len);
			printf("Phase 3: <User3> is sending the message %s on UDP dynamic port number %d\n",getInfo,ntohs(sin.sin_port));
	}

    //send signal to notice that user3 finish the process of sending message
    if ((numbytes = sendto(sockfd, "finish", 6, 0, p->ai_addr, p->ai_addrlen)) == -1) {
		        perror("talker: sendto");
		        exit(1);
	        }

    freeaddrinfo(servinfo);
	fclose(fp);
	close(sockfd);
     //Enter the phase 3 to get the message coming from supernode 
	phase3listener();

	return 0;
}

int main(void)
{   //The following connection setup code comes from Beej's book
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
    char userPassword[100];//Open the file and get each pair of password and username
    socklen_t len;
	struct hostent *host;//Use gethostbyname function
	int supernodePortNum = 3563;
	char *prUser;//Store the username
	char *prPass;//Store the password
	
    FILE *fp;
	
	
    //Open UserPass3.txt
    if((fp = fopen("UserPass3.txt", "r")) == NULL){
		printf("Can not open UserPass3.txt\n");
		exit(1);
	}
   
	//Get each line of password and username in UserPass3.txt
	fgets(userPassword, 100, fp);
    //Close UserPass3.txt
    fclose(fp);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo("nunki.usc.edu", PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		phase3talker();//Go to send message function.
		return 2;
	}
	//get the dynamic port number and IP address
     host = gethostbyname("nunki.usc.edu");
	getsockname(sockfd,p->ai_addr, &len);
	freeaddrinfo(servinfo); // all done with this structure
    printf("Phase 1: <User3> has TCP port %d and IP address: %s\n",ntohs(((struct sockaddr_in *)(p->ai_addr))->sin_port),inet_ntoa(*((struct in_addr*)host->h_addr_list[0])));
    //Send the pair of username and password to Login server to wait for response

	if (send(sockfd, userPassword, 100, 0) == -1)
		perror("send");
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	     perror("recv");
	     exit(1);
	}
	//Split the pair of username and password and display each username and password
    prUser = strtok(userPassword," ");
	prPass = strtok(NULL," ");
    printf("Phase 1: Login request. User: %s password: %s",prUser,prPass);
	buf[numbytes] = '\0';
    //Show the Login request and IP address and port number
    printf("Phase 1: Login request reply:%s.\n",buf);
    printf("Phase 1: Supernode has IP Address %s and Port Number %d\n",inet_ntoa(*((struct in_addr*)host->h_addr_list[0])),supernodePortNum);

	close(sockfd);

	return 0;
}

