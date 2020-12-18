## Zadanie 2. POSIX (50%)

Zrealizuj zadanie analogiczne do Zadania 1, wykorzystując kolejki komunikatów POSIX.

Kolejka klienta powinna mieć losową nazwę zgodną z wymaganiami stawianymi przez POSIX. Na typ komunikatu można zarezerwować pierwszy bajt jego treści. Przed zakończeniem pracy klient wysyła do serwera komunikat informujący, że serwer powinien zamknąć po swojej stronie kolejkę klienta. Następnie klient zamyka i usuwa swoją kolejkę. Serwer przed zakończeniem pracy zamyka wszystkie otwarte kolejki i usuwa kolejkę, którą utworzył.