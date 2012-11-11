project:
	gcc -o Login Login.c -lsocket -lnsl -lresolv
	gcc -o Supernode Supernode.c -lsocket -lnsl -lresolv
	gcc -o user1 user1.c -lsocket -lnsl -lresolv
	gcc -o user2 user2.c -lsocket -lnsl -lresolv
	gcc -o user3 user3.c -lsocket -lnsl -lresolv
