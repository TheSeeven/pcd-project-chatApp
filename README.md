# pcd-project-chatApp
This is a project for my university for the Concurential Programing subject. I worked in a team with my coleagues Edward, Mile, Paul

Documentation:

    Tema proiect: Aplicatie de chat cu trimitere de fisiere criptate si procesare de imagini

Limbaje si tehnologii:  C, Python, SQLLite3, GitHub

Componenta echipei:	 - Perianu Leon 	 	   ---	server, client Admin
		          	     - Tudic-Iovan Edward	 ---	server, client INET (Unix)
                     - Lepoiev Miodrag 	   ---	Client INET, Interfata INET
                     - Calinoiu Paul 		   ---	REST API, Interfata REST, Documentatie

Informatii de configurare: Prin teminalul de comanda Linux se va porni fiecare aplicatie (Server, Client INET Unix si Other, Client Admin)

	ServerINET:

-La startarea serverului, acesta creeaza 2 thread-uri pentru pornirea si ascultarea clientului de Admin si Clientului de INET. Pentru initializarea serverului ne folosim de o conexiune de tip socket, iar pentru  a retine o lista de clienti cu o conexiune activa ne folosim de mutex lock si unlock.

-Ambele servere, cel de INET si cel de Admin, folosesc protocoale de tip TCP/IP. 

-Dupa startarea serverului, acesta incepe sa asculte pentru o conexiune de la un client. In momentul logarii unui client, serverul trimite un token unic catre client pentru a i se autoriza conexiunea, dupa care acesta creeaza un thread special pentru clientul conectat.

-Dupa conectare, serverul adauga clientul la lista de clienti conectati printr-un mutex, dupa care incepe sa asculte pentru mesaje de la acesta. 

-In momentul in care primeste un mesaj de la client, serverul verifica header-ul mesajului pentru a afla ce functie trebuie apelata, urmand procesarea requestului.

-O data cu validarea conexiunii, serverul verifica lista de prieteni a utilizatorului si lista de non-prieteni.
-Indata ce clientul alege un prieten cu care sa vorbeasca, serverul face verificarea mesajelor anterioare intre cele 2 persoane.

	ServerAdminUNIX:	

-Serverul de Admin comunica cu clientul Admin prntr-o conexiune formata local ce trimite un fisier in care sa face schimbul de date.

-Functiile principale ale ChatRoom-ului sunt urmatoarele:
	- getconnectedusers -  trimite un request de la client la server pentru a vizualiza clientii conectati in acea sesiune.
	- disconnectuser – trimite un request de la client la server pentru a deconecta un client activ in sesiunea curenta.
	Se apeleaza astfel: disconnectuser;<username>
	- blockuser – trimite un request de la client la server pentru a bloca un utilizator si a preveni orice alta viitoare incercare de a se conecta acesta la server.
	Se apeleaza astfel: blockuser;<username>
- unblockuser - trimite un request de la client la server pentru a debloca un utilizator ce a fost blocat.
Se apeleaza astfel: unblockuser;<username>
- getusers - trimite un request de la client la server pentru a vizualiza toti utilizatorii din sistem
Se apeleaza astfel: getusers
- getblockedusers - trimite un request de la client la server pentru a vizualiza toti utilizatorii blocati din sistem
Se apeleaza astfel: getblockedusers

	ClientUNIX:
	
-Acesta trimite request-uri catre serverul UNIX, iar bazat pe ce primeste inapoi, face o parsare a header-ului si trimite informatiile catre functia “prettyPrint”.

-“prettyPrint” are rolul de a afisa orice informatie primita de la server intr-o maniera eleganta si usor de inteles.

-Daca utilizatorul trimite o comanda gresita, o functie de help poate fi apelata si printeaza pe ecran toate comenzile valabile utilizatorului. 

	ClientINET:

-O data ce serverul este pornit, se porneste si clientul, dupa care porneste interfata de logare. O data ce serverul trimite token-ul de autentificare, functia de login face tranzitia inspre server si astepta verificarea datelor, dupa care afiseaza interfata de chatting.

-Pentru a trimite mesaje, clientul copiaza persoana din lista de prieteni, dupa care se face trimiterea mesajului catre server si acesta mai departe il trimite inspre baza de date.

-Clientul creeaza 2 thread-uri in momentul initializarii. Primul thread se porneste o data la 20 de secunde si updateaza lista de utilizatori care ii sunt prieteni si concomitent lista cu utilizatori care NU sunt prieteni. Al doilea thread realizeaza verificari pentru a vedea daca serverul a trimis vreun mesaj.

-De fiecare data cand serverul trimite un mesaj, clientul face parsarea mesajului pentru a detecta header-ul specific fisierului/mesajului in scopul realizarii functiei cerute.


