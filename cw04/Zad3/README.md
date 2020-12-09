## Zadanie 3. (50%)
Należy rozszerzyć interpreter poleceń z zadania 2 w zestawie 3 (Procesy) tak, by obsługiwał operator pipe - "|". Interpreter czyta kolejne linie z podanego pliku, każda linia ma format

```
prog1 arg1 ... argn1 | prog2 arg1 ... argn2 | ... | progN arg1 ... argnN
```

Dla takiej linii interpreter powinien uruchomić wszystkie N poleceń w osobnych procesach, zapewniając przy użyciu potoków nienazwanych oraz funkcji dup2, by wyjście standardowe procesu k było przekierowane do wejścia standardowego procesu (k+1). Można założyć ograniczenie górne na ilość obsługiwanych argumentów oraz ilość połączonych komend w pojedynczym poleceniu (co najmniej 5). Po uruchomieniu ciągu programów składających się na pojedczyne polecenie (linijkę) interpreter powinien oczekiwać na zakończenie wszystkich tych programów.

Uwaga: należy użyć pipe/fork/exec, nie popen