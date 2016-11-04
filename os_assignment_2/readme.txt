===========================USAGE=============================
Compile the code: 
	Exec "make" in the folder.

Server: 
	Exec "./server" in the terminal, 
	the first server will ask for abuffer size.

Client:
	Exec "./client <client id>" in the terminal,
	if there is already a job with the same id exists in the queue,
	the program will report an error and exit.




===========================client============================
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./client 1
Connecting to server
Client 1 has 4 pages to print, puts request in Buffer
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./client 2
Connecting to server
Client 2 has 8 pages to print, puts request in Buffer
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./client 3
Connecting to server
Client 3 has 7 pages to print, puts request in Buffer
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./client 4
Connecting to server
Client 4 has 2 pages to print, puts request in Buffer
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./client 5
Connecting to server
Client 5 has 5 pages to print, puts request in Buffer
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./client 6
Connecting to server
Client 6 has 2 pages to print, puts request in Buffer




===========================server 1============================
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./server 
This is your first print server
Please initialize the number slots you want in the buffer
6
Initializing shared memory region
buffer size is 6
server id: 1
Printer 1 starts printing 4 pages from client 1
Printer 1 finishes printing 4 pages from client 1
No request in buffer, Printer sleeps
Printer 1 starts printing 2 pages from client 4
Printer 1 finishes printing 2 pages from client 4
Printer 1 starts printing 5 pages from client 5
Printer 1 finishes printing 5 pages from client 5
No request in buffer, Printer sleeps




===========================server 2============================
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./server 
buffer size is 1
server id: 2
Printer 2 starts printing 4 pages from client 2
Printer 2 finishes printing 4 pages from client 2
No request in buffer, Printer sleeps
^CYour last server quited, unlink the shared memory
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./server 
buffer size is 6
server id: 2
Printer 2 starts printing 8 pages from client 2
Printer 2 finishes printing 8 pages from client 2
Printer 2 starts printing 2 pages from client 6
Printer 2 finishes printing 2 pages from client 6
No request in buffer, Printer sleeps




===========================server 3============================
xji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./server 
buffer size is 1
server id: 3
Printer 3 starts printing 1 pages from client 3
Printer 3 finishes printing 1 pages from client 3
No request in buffer, Printer sleeps
Printer 3 starts printing 1 pages from client 5
Printer 3 finishes printing 1 pages from client 5
No request in buffer, Printer sleeps
^Cxji12@cs-7 ~/Desktop/os_assignments/os_assignment_2 $ ./server 
buffer size is 6
server id: 3
Printer 3 starts printing 7 pages from client 3
Printer 3 finishes printing 7 pages from client 3
No request in buffer, Printer sleeps
