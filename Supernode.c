/*
** Author:Chia-Chen, Wu
** Supernode.c 
** Store username and IP address coming from Login server.
** Store the message coming from users and send the messages to the message.
** Some code from Beej's Guide book
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXDATASIZE 50//max size the buffer can store
#define PORT "22563"  //static TCP port number to the Login server
#define PHASE3LISTENER "3563" //static UDP port number to the users
#define MAXBUFLEN 100// max size the buffer can store

#define PHASE3TALKER1 "3663"//static UDP port number to the user1
#define PHASE3TALKER2 "3763"//static UDP port number to the user2
#define PHASE3TALKER3 "3863"//static UDP port number to the user3

#define BACKLOG 10	 // how many pending connections queue will hold

char textInfo[100];//Store the message 

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//The following code is used for sending the finish signal to tell the users to finish the phase 3
int finishSignal()
{   //The following connection setup code comes from Beej's book
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
    
	
	//The part of the code is to tell user1 to finish the phase 3
   
	   memset(&hints, 0, sizeof hints);
	   hints.ai_family = AF_UNSPEC;
	   hints.ai_socktype = SOCK_DGRAM;

	   if ((rv = getaddrinfo("nunki.usc.edu", PHASE3TALKER1, &hints, &servinfo)) != 0) {
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

	   if (p == NULL) {
		   fprintf(stderr, "talker: failed to bind socket\n");
		   return 2;
	   }
       //send finish signal
	   if ((numbytes = sendto(sockfd, "F", 1, 0,
		    	 p->ai_addr, p->ai_addrlen)) == -1) {
		   perror("talker: sendto");
		   exit(1);
	   }

	   freeaddrinfo(servinfo);

	   close(sockfd);
   // The part of code is to tell the user2 to finish the phase 3
       memset(&hints, 0, sizeof hints);
	   hints.ai_family = AF_UNSPEC;
	   hints.ai_socktype = SOCK_DGRAM;

	   if ((rv = getaddrinfo("nunki.usc.edu", PHASE3TALKER2, &hints, &servinfo)) != 0) {
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

	   if (p == NULL) {
		   fprintf(stderr, "talker: failed to bind socket\n");
		   return 2;
	   }
       //send finish signal
	   if ((numbytes = sendto(sockfd, "F", 1, 0,
		    	 p->ai_addr, p->ai_addrlen)) == -1) {
		   perror("talker: sendto");
		   exit(1);
	   }

	   freeaddrinfo(servinfo);

	   close(sockfd);
  // The part of code is to tell the user2 to finish the phase 3
   	   memset(&hints, 0, sizeof hints);
	   hints.ai_family = AF_UNSPEC;
	   hints.ai_socktype = SOCK_DGRAM;

	   if ((rv = getaddrinfo("nunki.usc.edu", PHASE3TALKER3, &hints, &servinfo)) != 0) {
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

	   if (p == NULL) {
		   fprintf(stderr, "talker: failed to bind socket\n");
		   return 2;
	   }
       //send finish signal
	   if ((numbytes = sendto(sockfd, "F", 1, 0,
		    	 p->ai_addr, p->ai_addrlen)) == -1) {
		   perror("talker: sendto");
		   exit(1);
	   }

	   freeaddrinfo(servinfo);

	   close(sockfd);
   
	return 0;
}
//Send the message to the user1 through UDP
int phase3tranTo1()
{  //The following connection setup code comes from Beej's book
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
    int len;
	struct hostent *host;//Get IP address
	struct sockaddr_in sin;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo("nunki.usc.edu", PHASE3TALKER1, &hints, &servinfo)) != 0) {
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
    //bind function
	bind(sockfd,(struct sockaddr *)&sin,sizeof(sin));
    len = sizeof(sin);

	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
    //Send the message to user1
	if ((numbytes = sendto(sockfd, textInfo, strlen(textInfo), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	 //Get the dynamic port number and print it out
	getsockname(sockfd,(struct sockaddr *)&sin, &len);
	printf("Phase 3: SuperNode sent the message %s on dynamic UDP port number %d\n",textInfo,ntohs(sin.sin_port));

	freeaddrinfo(servinfo);

	close(sockfd);

	return 0;
}
//Send the message to the user2 through UDP
int phase3tranTo2()
{   //The following connection setup code comes from Beej's book
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
    int len;
	struct hostent *host;//Get the  IP address
	struct sockaddr_in sin;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo("nunki.usc.edu", PHASE3TALKER2, &hints, &servinfo)) != 0) {
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
   //bind function
	bind(sockfd,(struct sockaddr *)&sin,sizeof(sin));
    len = sizeof(sin);

	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
    //Send the message to user2 
	if ((numbytes = sendto(sockfd, textInfo, strlen(textInfo), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
    //Get the dynamic port number and print it out
	getsockname(sockfd,(struct sockaddr *)&sin, &len);
	printf("Phase 3: SuperNode sent the message %s on dynamic UDP port number %d\n",textInfo,ntohs(sin.sin_port));

	freeaddrinfo(servinfo);

	close(sockfd);

	return 0;
}
//Send the message to user3 through UDP 
int phase3tranTo3()
{   //The following connection setup code comes from Beej's book
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
    int len;
	struct hostent *host;//Use for getting IP address
	struct sockaddr_in sin;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo("nunki.usc.edu", PHASE3TALKER3, &hints, &servinfo)) != 0) {
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
    //bind function
	bind(sockfd,(struct sockaddr *)&sin,sizeof(sin));
    len = sizeof(sin);

	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
    //Send the message to the user3
	if ((numbytes = sendto(sockfd, textInfo, strlen(textInfo), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
    //Get the dynamic port number and print it out
	getsockname(sockfd,(struct sockaddr *)&sin, &len);
	printf("Phase 3: SuperNode sent the message %s on dynamic UDP port number %d\n",textInfo,ntohs(sin.sin_port));

	freeaddrinfo(servinfo);

	close(sockfd);

	return 0;
}
//The following code is used for storing message coming from the users and the connection code comes from Beej's Guide book
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
	int anotherCounter = 0;//Counter to count how many users finish their message sending 
    struct hostent *host;//Get the IP address

	FILE *fpinfo;
	FILE *fptran;

	fpinfo=fopen("getInfo.txt","w");//use for storing the message coming from users

    host = gethostbyname("nunki.usc.edu");
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
	//When finish three users' sending message, it will jump the loop
	while (anotherCounter != 3)
	{   //Get the signal of which user sending its message
		if ((numbytes = recvfrom(sockfd, buf, 100 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	     }
       //Get the user1's message and if it gets the finish signal, it will jump out the loop
	    if (numbytes == 1)
	   {  
           do
          {
		      if ((numbytes = recvfrom(sockfd, buf, 100 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		        perror("recvfrom");
		        exit(1);
	            }
				  if (numbytes != 6)
			  {
				  buf[numbytes] = '\0';
				  printf("Phase 3: SuperNode received the message %s\n",buf);
				fprintf(fpinfo,"%s",buf);
			  }


			   if (numbytes == 6)
			     anotherCounter++;
           }while (numbytes != 6);
		   

	    }//Get the user2's message and if it gets the finish signal, it will jump out the loop
	    if (numbytes == 2)
	    {
			
			do
          {
		      if ((numbytes = recvfrom(sockfd, buf, 100 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		        perror("recvfrom");
		        exit(1);
	            }
			if (numbytes != 6)
			  {
				  buf[numbytes] = '\0';
				  printf("Phase 3: SuperNode received the message %s\n",buf);	     
                  fprintf(fpinfo,"%s",buf);
			  }

        
			   if (numbytes == 6)
			     anotherCounter++;
           }while (numbytes != 6);

		  
	    }
      //Get the user3's message and if it gets the finish signal, it will jump out the loop
	    if (numbytes == 3)
	    {
		
			do
          {
		      if ((numbytes = recvfrom(sockfd, buf, 100 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		        perror("recvfrom");
		        exit(1);
	            }
			  if (numbytes != 6)
			  {
				  buf[numbytes] = '\0';
				  printf("Phase 3: SuperNode received the message %s\n",buf);

				fprintf(fpinfo,"%s",buf);
			  }

			   if (numbytes == 6)
			     anotherCounter++;
           }while (numbytes != 6);

		   
	    }
	
	}
	
	fclose(fpinfo);

	close(sockfd);
    //Open the getInfo.txt to ready to send the message to the users
	fptran=fopen("getInfo.txt","r");

	while(fgets(textInfo,100,fptran))
    {
        //If the message will be send to user1, it will go another function
		if(textInfo[5] == '1')
        {
			
			phase3tranTo1();
		} //If the message will be send to user2, it will go another function
		if (textInfo[5] == '2')
		{
			
			phase3tranTo2();
		} //If the message will be send to user3, it will go another function
		if (textInfo[5] == '3')
		{
			
			phase3tranTo3();
		}
	}
	
    fclose(fptran);//close the file
	finishSignal();//Send the finish signal to the users 
	printf("End of Phase 3 for SuperNode.\n");
	return 0;
}

int main(void)
{   //The following connection setup code comes from Beej's book
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	char buf[MAXDATASIZE];
	int rv;
	int numbytes;

	int i;
	struct hostent *host;//Use for IP address
	int count = 0;//counter for receiving information from Login server

	char userName[3][50];//Store the username and IP address getting from the login server
	

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP


	host = gethostbyname("nunki.usc.edu"); //get the IP address

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

    //establish TCP connection
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
	
		}
		//print the TCP port number and IP address
      printf("Phase 2: SuperNode has TCP port 22563 and IP address:%s\n",inet_ntoa(*((struct in_addr*)host->h_addr_list[0])));
      //Get the IP address and username from login server and stor them into array
      while(count <= 2) {
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE, 0)) == -1) {
	              perror("recv");
	            exit(1);
			}
          
        	buf[numbytes] = '\0';
			strcpy(userName[count], buf);
			
         	
            count ++;
		
	}

    close(new_fd);
    printf("Phase 2: SuperNode received %d username/IP address pairs.\n",count);
	printf("End of Phase 2 for SuperNode.\n");

    phase3listener();//Jump to phase 3 for getting the message from users
	return 0;
}

