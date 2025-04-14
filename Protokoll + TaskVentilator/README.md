[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/H1vNwaly)

Protokoll zur Systemprogrammierung:

Folgende Themen wurden in der Übung adressiert:

Wir haben uns mit der Interprozesskommunikation über Message Queues beschäfitgt, also wie Prozesse über Nachrichten kommunizieren.
man pages wurden erklärt und wie man diese sinnvoll verwendet.
Managen von Queues
Prozesssteuerung mittels sleep() und wait()
Verschiedene neue Libraries

Was wurde gelernt?

Es wurden einige neue Libraries verwendet, die davor noch nicht bekannt waren wie zum Beispiel: mqueue.h um die Message Queues zu implementieren, oder unistd.h um fork() zu verwenden und einen Prozess zu starten.
Außerdem die sys/wait.h Library um die Makros für wait zu implementieren.
Wie man Message Queues initialisiert und zwischen Prozessen nutzt.
Wie man mit mehreren Prozessen arbeitet und ihre Lebenszyklen verwaltet.
Das umgehen mit man pages.
Welche Fehlerquellen bei Queue Operationen auftreten können wie zum Beispiel Queue schon vorhanden oder Queue voll, usw..
Die Library fcntl.h muss eingebunden werden um die O_ Konstanten zu verwenden. Diese steuern wie die Queue geöffnet werden soll. Hier wird zum Beispiel die Queue im Read only geöffnet: mqd_t command_queue = mq_open("/mq_r47587", O_RDONLY);
S_IRWXU ist der Zugriffsmodus der alle permissions gibt

Erkenntnisse aus der Übung:

Es ist wichtig, Queues nach der Nutzung sauber zu schließen und zu löschen (mq_close, mq_unlink), um Konflikte bei erneutem Start zu vermeiden.
Kommunikation funktioniert auch zwischen völlig unabhängigen Prozessen, solange sie denselben Queue-Namen verwenden.

Schwierigkeiten der Übung:

Während der Übung mitzukommen ohne vorkenntnisse der Implementierten Libraries und sich somit ein Verständnis über diverse Funktionen zu bilden.
Zurechtfinden in den verschiedenen man pages fällt am Anfang schwer.
Das Timing der Queue Kommunikation muss durchdacht sein, um verlorene Nachrichten zu vermeiden.
Wenn Prozesse hängen, ist die Fehlersuche teilweise sehr schwer.

Folgendes wurde mitgenommen für die Hausübung (Task Ventilator):

Das Konzept und Verständnis von Message Queues ist sehr wichtig.
Die Worker-Prozesse sollen in der Hausübung die Aufgaben so empfangen wie wir es in der Übung umgesetzt haben.
Die erlernten Libraries werden wieder angewendet werden müssen.



