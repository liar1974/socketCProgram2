/*
** Author:Chia-Chen, Wu
** Login.c 
** Check the username and password coming from users and give request and supernode port number and IP address to users
** Send the IP and username to supernode
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

#define MAXDATASIZE 100//max size the buffer can store
#define PORT "21563"  // static port number the user connect
#define PORT2 "22563"//static port number to connect supernode

#define BACKLOG 10	 // how many pending connections queue will hold

char userName[3][50];//Store the username coming from users
char *pass[3];//Store the password
char *interChange;//temporary to store the username

char *WrongUserName;//store the reject username
char *WrongPass;//store the reject password


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
//The following function is used for phase 2 
int subLogin()
{
    //The following connection setup code comes from Beej's book
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
    int j;
    struct hostent *host;//store the IP address
    socklen_t len;

   
 
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo("nunki.usc.edu", PORT2, &hints, &servinfo)) != 0) {
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
			perror("client: connect");
			continue;
		}

		break;
	}
	

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}


	freeaddrinfo(servinfo); // all done with this structure
    host = gethostbyname("nunki.usc.edu");//Show the IP address
	getsockname(sockfd,p->ai_addr, &len);//Get the dynamic port number
	printf("Phase 2: LogIn server has TCP port number %d and IP address %s\n",ntohs(((struct sockaddr_in *)(p->ai_addr))->sin_port),inet_ntoa(*((struct in_addr*)host->h_addr_list[0])));
   //Send the username and IP address to Supernode 
	for (j = 0;j <= 2; j++)
    {   
		
	  if (send(sockfd, userName[j], 50, 0) == -1)
		        perror("send");

	
    }
    
	printf("Phase 2: LogIn server sent %d username/IP address pairs to SuperNode.\n",j);
	close(sockfd);
    printf("End of Phase 2 for Login Server.\n");
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

	int flag = 0;//To justice reject or accept
	int i;
	struct hostent *host;//Get the IP address 
    
	char compareUserPass[100];//Get username and password from UserPassMatch.txt
    int count = 0;

	FILE *fp;
	
	
    //Open the file UserPassMatch.txt
    if((fp = fopen("UserPassMatch.txt", "r")) == NULL){
		printf("Can not open UserPassMatch.txt\n");
		exit(1);
	}
   
	

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP


	host = gethostbyname("nunki.usc.edu"); //get the IP address
    printf("Phase 1:LogIn server has TCP port 21563 and IP address %s\n",inet_ntoa(*((struct in_addr*)host->h_addr_list[0])));

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
    //If get three correct username and password pairs, it will jump out the loop
	while(count <= 2) {  
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
  

           //Get the username and password pair from users
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
	              perror("recv");
	            exit(1);
			}

        	buf[numbytes] = '\0';
            //Read the UserPassMatch.txt line by line and compare it to the receiving username and password pairs
            while(fgets(compareUserPass, 100, fp))
	        {
               
			   if(strcmp(compareUserPass,buf)==0)
			   {   //Send the accept to the user
				   if (send(new_fd, "Accept", 6, 0) == -1)
	                	perror("send");
	               //Split username and password and display it
			       interChange = strtok(compareUserPass," ");
			       strcpy(userName[count], interChange); 
				   pass[count] = strtok(NULL," ");
				   
                   printf("Phase 1: Authentication request. User:%s Password:%s User IP Addr:%s.Authorized:Accepted\n",userName[count],pass[count],inet_ntoa(*((struct in_addr*)host->h_addr_list[0])));
				
				   flag = 1;
                  
				   count ++;
                   printf("Phase 1: SuperNode IP Address:%s Port Number:3563 sent to the <User%d>\n",inet_ntoa(*((struct in_addr*)host->h_addr_list[0])),count);
				   break;
			   }
			  
	        }
	        if(flag == 0)
			{   //Send the reject to the user if request is rejected
				if(send(new_fd, "Reject", 6, 0) == -1)
					perror("send");
                //Split the username and password to display it
				WrongUserName = strtok(buf," ");
			    WrongPass =strtok(NULL," ");
                printf("Phase 1: Authentication request. User:%s Password:%s User IP Addr:%s.Authorized:Rejected\n",WrongUserName,WrongPass,inet_ntoa(*((struct in_addr*)host->h_addr_list[0])));
	
			}
			fseek(fp, 0,SEEK_SET);//Set the point at the beginning of the file
            flag = 0;
   
		close(new_fd);
	}
    
    printf("End of Phase 1 for Login Server\n");
    fclose(fp);
    close(sockfd);

    subLogin();//Go to Phase 2 function

	return 0;
}

