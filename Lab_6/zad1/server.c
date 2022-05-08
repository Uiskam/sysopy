#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Serwer odpowiada na komunikat klientów.
 * Zlecenia:
 *  - INIT
 *    klient:
 *      wysyła ID jego kolejki na serwer
 *    serwer:
 *      otwiera zadaną kolejkę
 *      wysyła klientowi przypisane mu ID
 *  otwarcie kolejki klienta (nr w kolejności zgłoszeń) i odsyła mu to ID
 *    (komunikacja ta zachodzi przez kolejke klienta)
 *  - LIST
 *    serwer:
 *      wypisanie aktywnych klientów
 *    klient:
 *      akutalizacja listy klientów
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
 * Priorytety:
 *  - STOP
 *  - LIST
 *  - reszta
 *
 *  Po uruchomieniu:
 *      - tworzy kolejkę, za pomocą której będzie odbierać komunikaty
 *
 *  Po otrzymaniu zlecenia serwer zapisuje do pliku z logiem:
 *  czas otrzymania zlecenia, id klienta i treść komunikatu.
 *
 *  obsługa sygnału CTR + Z == koniec pracy serwera, odebranie STOP od wszystkich klientów, usunięcie kolejki i koniec pracy
 */

int main() {

}