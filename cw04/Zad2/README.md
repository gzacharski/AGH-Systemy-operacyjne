## Zadanie 2. (35%)
Napisz program który tworzy proces potomny i wysyła do niego L sygnałów SIGUSR1, a następnie sygnał zakończenia wysyłania SIGUSR2. Potomek po otrzymaniu sygnałów SIGUSR1 od rodzica zaczyna je odsyłać do procesu macierzystego, a po otrzymaniu SIGUSR2 kończy pracę.

Proces macierzysty w zależności od argumentu Type (1,2,3) programu wysyła sygnały na trzy różne sposoby:

* SIGUSR1, SIGUSR2 za pomocą funkcji kill (15%)
* SIGUSR1, SIGUSR2 za pomocą funkcji kill, z tym, że proces macierzysty wysyła kolejny sygnał dopiero po otrzymaniu potwierdzenia odebrania poprzedniego (15%)
* wybrane 2 sygnały czasu rzeczywistego za pomocą kill (10%)

Program powinien wypisywać informacje o:

* liczbie wysłanych sygnałów do potomka
* liczbie odebranych sygnałów przez potomka
* liczbie odebranych sygnałów od potomka

Program kończy działanie po zakończeniu pracy potomka albo po otrzymaniu sygnału SIGINT (w tym wypadku od razu wysyła do potomka sygnał SIGUSR2, aby ten zakończył pracę. Wszystkie pozostałe sygnały są blokowane w procesie potomnym).

L i Type są argumentami programu.