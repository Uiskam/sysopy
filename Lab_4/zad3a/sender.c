#include "common.c"
/*
 * 2 programy sender i cacher
 * catcher uruchamia się pierwszy i wyspiuje swój PID i czeka na sygnały od sendera (i blokuje wszystko poza sigusr1/2
 * odbiera sygnały od sendera a później oddaje tyle ile wysłał
 * cathcer na końcu wypisuje liczbe odebrnacyh sygnałów i kończy działanie
 *
 * sigsuspend ma odpowiedać za czekanie na sygnały
 *
 * sender ma 3 parametry: PID catchera, ilośc syngałów do wysłania i tryb
 * senbder wysyłwa zadaną ilośc usr1 i usr2, które póxniej catcher ma mu odesłać
 * po otrzymaniu sygnałów od catchera wypisuje ile otrzymał a ile pownien był
 *
 *
 */
int main(int argc, char** argv) {
  bool use_sigqueue = is_arg(3, "sigqueue");
  bool use_sigrt    = is_arg(3, "sigrt");
  bool use_kill     = is_arg(3, "kill");

  pid_t pid = atoi(argv[1]);
  int sig_usr1_total = atoi(argv[2]);

  init(use_sigrt);

  if (use_kill or use_sigrt) {
    repeat(sig_usr1_total) kill(pid, SIG1);
    then kill(pid, SIG2);
  }
  else if (use_sigqueue) {
    repeat(sig_usr1_total) sigqueue(pid, SIG1, val);
    then sigqueue(pid, SIG2, val);
  }

  receive then 
    if (use_sigqueue)  printf("[sender]  received: %d (catcher %d)\n", received, sent);
    else printf("[sender]  received: %d\n", received);
}
