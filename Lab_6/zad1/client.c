#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include "comm_def.h"
/*
 * Klient wysyła zlecenia, które zawierają:
 *  - typ wiadomości, typ zlecenia
 *  - treść, ID klienta
 *
 * Zlecenia KAŻDE zlecenie ma zawierać id klienta, od którego pochodząi jego rodzaj:
 *  - INIT
 *    klient:
 *      wysyła ID jego kolejki na serwer
 *    serwer:
 *      otwiera zadaną kolejkę
 *      wysyła klientowi przypisane mu ID (przez kolejke klienta)
 *  otwarcie kolejki klienta (nr w kolejności zgłoszeń) i odsyła mu to ID
 *    (komunikacja ta zachodzi przez kolejke klienta)
 *  - LIST
 *    serwer:
 *      wypisanie aktywnych klientów
 *  - 2ALL string
 *    klient:
 *      wiadomość
 *    serwer:
 *      przekazuje do wszystkich innych wiadomość, ID nadawcy i aktualna datę
 *  - 2ONE id_klient string,
 *    klient:
 *      ID adresata (z listy od serwera)
 *      wiadomość
 *    serwer:
 *      wysyła do adresata wiadomość, ID nadawcy i aktualną datę
 *  - STOP (jest wysyłany również w reakcji na SIGINT)
 *    klient:
 *      wysyła ten komunikat
 *      usuwa swoją kolejkę
 *      kończy pracę
 *    serwer:
 *      usuwa klienta z listy aktywnych
 *      usuwa kolejkę klienta z listy
 *
 * Po uruchomieniu klient:
 *  - tworzy kolejkę IPC (z unikalnym kluczem)
 *  - wysyła jej klucz komunikatem do serwera (komunikat INIT)
 *
 *  Po tym klient może wysyłać zlecenia na serwer (czytane ze stdin), (czyta typy komunikatów)
 */
int main() {
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    printf("%s is the value of $HOME\n",homedir);
    printf("%d %d %d %d %d\n",STOP, LIST, INIT, TWOALL, MAXMSG);
}