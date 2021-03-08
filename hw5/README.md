# ID1217 Homework 5

## Assignment 1 Distributed client-server (distPair1.c)
In this assignment the teacher process will work as the server, and the students as clients. 
### Algorithm
The teacher will wait for a message from any source (all students), it will recieve messages with student ranks which are put into a buffer with two slots. Once a message is recieved it will check whether the buffer is full, in which case it will message the student processes whose ranks are in the buffer that they are working with eachother. The buffer will then be cleared and the teacher will repeat the process if there are students left. The teacher will also check how many students are left before ,messaging students, if there is only one student left and no other student in the buffer, the teacher will message this student with it's own rank indicating the student will work alone.

The students will send a message to the teacher with their rank, and will after that wait for a message back from the teacher. The message recieved from the teacher will contain the rank of the student process they are working with.

## Assignment 1 Distributed p2p (distPair2.c)
In this assignment the teacher and students are all peers.

### Algorithm
The algorithm is initiated with the teacher process sending a message to a random student with a buffer. The buffer is of size n (for n processes). The buffer indicates which students  are taken and and which are free. The indexes of the buffer are used to identify the student processes by rank. The values in the buffer will be either 0 (meaning they are free) or the rank of their partner. The teacher will initalize all indexes to 0, excpet for index 0 which is the rank of the teacher.

All students will initially be waiting for a message. The buffer is passed to the students via messages. The student will first check in the buffer if they themselves are taken, in which case it will randomly notify a non-taken student it's their turn to choose a student. In the other case when the student is not taken, it means they are to choose a partner. The student will randomly pick a free student from the buffer, the student will then update their spot in the buffer with their partners rank, and their partners spot with their own rank. The student will then message their partner with the updated buffer, this lets the partner process know it has been taken and by who. There is also the case where a student process is messaged and it sees it's index in the buffer has not been taken, but there are no other free spots in the buffer. In this case it means there was an uneven number of student, and this student will have to work alone. It will not send a message in this case.