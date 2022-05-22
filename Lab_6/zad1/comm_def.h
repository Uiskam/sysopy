#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/ipc.h>

#ifndef SYSOPY_COMM_DEF_H
#define SYSOPY_COMM_DEF_H

#define STOP 1
#define LIST 2
#define TWOALL 3
#define TWOONE 4

#define INIT 20
#define SERVER_SHUT_DOWN 30

#define SERVER_CAPACITY 10
#define MAXMSG 300
#define SERVER_QUE_KEY ftok(getpwuid(getuid())->pw_dir,0)
#define SERVER_WAIT_TIME 100

struct comm_msg {
    long mtype;         /* typ komunikatu   */
    long senderID;      /* id nadawcy */
    char mtext[MAXMSG];      /* treść komunikatu */
    int active_users[SERVER_CAPACITY];
};



#endif //SYSOPY_COMM_DEF_H
