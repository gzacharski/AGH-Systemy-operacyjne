## Zadanie 2. Operacje na strukturze katalogów (45%)
Napisz program wyszukujący w drzewie katalogu (ścieżka do katalogu jest pierwszym argumentem programu), w zależności od wartości drugiego argumentu ('<', '>','=') , pliki zwykłe z datą modyfikacji wcześniejszą, późniejszą lub równą dacie podanej jako trzeci argument programu. Program ma wypisać na standardowe wyjście następujące informacje znalezionych plików:

* ścieżka bezwzględna pliku
* rozmiar w bajtach
* prawa dostępu do pliku (w formacie używanym przez ls -l, np. r--r--rw- oznacza prawa odczytu dla wszystkich i zapisu dla właściciela)
* datę ostatniej modyfikacji

Ścieżka podana jako argument wywołania może być względna lub bezwzględna. Program powinien wyszukiwać tylko pliki zwyczajne (bez dowiązań, urządzeń blokowych, itp). Program nie powinien podążać za dowiązaniami symbolicznymi do katalogów.

Program należy zaimplementować w dwóch wariantach:

1. Korzystając z funkcji opendir, readdir oraz funkcji z rodziny stat (25%)
2. Korzystając z funkcji nftw (20%)

W ramach testowania funkcji utwórz w badanej strukturze katalogów jakieś dowiązania symboliczne.