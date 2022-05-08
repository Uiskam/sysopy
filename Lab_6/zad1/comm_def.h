#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/ipc.h>

#ifndef SYSOPY_COMM_DEF_H
#define SYSOPY_COMM_DEF_H

#define STOP 1
#define LIST 2
#define INIT 3
#define TWOALL 4
#define TWOONE 5
#define MAXMSG 400
#define SERVER_QUE_ID ftok(getpwuid(getuid())->pw_dir,0)

struct msg {
    long mtype;         /* typ komunikatu   */
    char mtext[MAXMSG];      /* tresc komunikatu */
};



#endif //MYSYSOPY_COMM_DEF_H
