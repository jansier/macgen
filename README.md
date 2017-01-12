MacGen - makrogenerator

Autor:
	Jan Sierpina

Opis:
	Program rozpoznaje i przetwarza jednowierszowe dyrektywy #include i #define

Instrukcja:
	Program można skompilować za pomocą polecenia make.
	Program przyjmuje jeden parametr będący ścieżką do przetwarzanego pliku.
	Uruchamianie: ./macgen <scieżka do pliku>
	Przetworzone dane są wypisywane na standardowe wyjście.

Składnia dyrektyw:
	Analogiczna jak gcc

Testy:
	W folderze tests znajduje się parę przykładowych testów
	zarówno poprawnych jak i błędnych.
	Test poprawny: correct_1
	Testy błędne: error_1, error_2, error_3, error_4
	Program wypisuje treść błędu oraz wskazuje wiersz i dojście do pliku w którym wystąpił.

