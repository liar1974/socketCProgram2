a. Name: Chenchen Yang
b. The pdf is the description of this homework

c. First, I build the login server for users to login; meanwhile, the server will check the personal information from users is valid or not. Second, I build the connection between login server and supernode. It is for the server to send the correct user which IP address to supernode. Finally, I setup the connections between supernode and users. By using listener() and talker(), users can send their messages to supernode first and then supernode will deliver to the correct receiver later.



d. Login.c is the code for login server. It is the code to match whether the user send the valid information.Meanwhile,it will send user and IP address to supernode.
   Supernode.c is the code for building connection between server and supernode. Besides, there is a file called getInfo.txt that is the one for supernode to store the messages from users and then, supernode will send the messages to the user who should receive by checking this file. 
   User1, user2, and user3 are the codes for users. In addition, there are also the codes for building connection between users and supernode. 
   Makefile is the easier way to compile the codes.  



e. First, TA should run makefile to compile these code.Then, TA should run my login file and supernode file in 2 different terminals. Second, TA should open 3 more terminals as three users. Then, TA can run my user1, user2, and user3 files in order in different 3 terminals. By doing this, the server can check whether their personal information are correct or not. After the server make sure their info are correct, the server will send the IP addresses and these users to supernode. In the end, TA should run user1, user2, and user3 files in different terminals again in order to start their conversations. 



f. 
In the supernode, getInfo.txt will store the messages which are from users and supernode will read it to send the message to the correct receiver.The other messages are stored in string array.


g. 
If TA does not follow the instructions, then my project might fail. 
