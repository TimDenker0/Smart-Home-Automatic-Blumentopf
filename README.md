# SHAB - Code Dokumentation

Die aktuelle Version des Programms ist [SHAB_-_15_-_Finaler_Code.ino](SHAB - Code/SHAB_-_15_-_Finaler_Code.ino).

## Inhaltsverzeichnis

1. Bibliotheken & Treiber

2. Aufbau des Programms
    - Erster Programmabschnitt: Initialisierung 
    - Zweiter Programmabschnitt: Instanzierung
    - Dritter Programmabschnitt: Ausführung
    - Vierter Programmabschnitt: Hilfsmethoden
    
3. Einstellungsmöglichkeiten



## Bibliotheken & Treiber

Für das kommunizieren mit dem Microcontroller wird der [CP210X USB-Treiber](https://dl.espressif.com/dl/package_esp32_index.json) benötigt.

Weiter wurden folgende Bibiliotheken verwendet:
- "esp32" von Espressif Systems (Version 1.0.6)
- "PubSubClient" von Nick O'Leary (Version 2.8.0)
- "Adafruit Unified Sensor" von Adafruit (Version 1.1.5)
- "DHT sensor library" von Adafruit (Version 1.4.4)
- "Wifi" von Arduino (Version 1.2.7) [Bulit-In]
- "Arduino_Multiwifi" von Arduino (Version 1.0.0)
- "NTPClient" von Fabrice Weinberg (Version 3.2.1)
- "Time" von Paul Stoffregen (Version 1.6.1)

## Aufbau des Programms
Das Programm kann in vier wesentliche Abschnitte unterteilt werden. Zunächst werden die verwendeten Bibliotheken initialisiert. Anschließend werden diese in der setup()-Methode Instanziiert und schließlich in der Laufmethode (void()) verwendet. Im letzten Abschnitt befinden sich Hilfsmethoden, welche in der Laufmethode benötigt werden.  

### Erster Programmabschnitt: Initialisierung 
Im ersten Programmabschnitt werden die genannten Bibliotheken eingebunden und globale Variablen für die Speicherung von Werten zur Steuerung der einzelnen Motoren bereitgestellt. Darunter fällt die Deklarierung der Variablen, die die Pins für den Anschluss der Sensoren und Motoren nach Schaltplan vorbereiteten. Zudem werden hier die Verbindungen für das WLAN durch "WifiMulti", zu "Node-RED" durch "PubSubClient" und zu einem NTP Server, welcher mit "Time" aktuelle Datumsangaben inkl. der Uhrzeit liefert, geknüpft.  

### Zweiter Programmabschnitt: Instanzierung
Im zweiten Programmabschnitt wird zunächst die Datenrate für die serielle Datenübertragung auf 9600 Baud festgelgt. Anschließend werden die bibliothekenspezifischen Methoden zur Instanziierung der Lüftungs-, Pumpen-, LED-, WLAN-, Zeit- und Node-RED-Variablen aufgerufen. Dies geschieht in der setup()-Methode.  

### Dritter Programmabschnitt: Ausführung
Der dritte Programmabschnitt bezieht sich auf die Laufmethode void(). In dieser kommt nun die dauerhafte Funktion des Programms zu tragen, da diese immer wieder von vorne durchgelaufen wird, bis man den Microcontroller neustartet oder vom Strom nimmt. Um die Belastung auf den Microcontroller so gering zu halten, wie nötig, wurde eine Zeitabfrage eingebaut, welche anders als eine Zeitverzögerung mit der in Arduino implementierten Methode delay(), keinen Programmstop erzwingt. Bei der Zeitabfrage wird die Laufzeit des Programms in Millisekunden in einer globalen Variable zwischengespeichert, bis dann das gewünschte Intervall erreicht ist, anschließend wird die globale Variable wieder überschrieben. Der große Vorteil ist, dass so durch eine einfache Abfrage die Methoden in dem Programmkörper nur ein Mal in einem bestimmten Intervall aufgerufen wird und gleichzeitg das Programm weiterläuft. Die Verwendung von "delay()" führte zu Fehlern bei der Steuerung der Motoren.  

Nachfolgend werden die neu auszulesenden Werte von den Sensoren in den dafür vorgesehenen globalen Variablen gespeichert. Alte Werte werden überschrieben.  

Die Aktualisierung erfolgt bevor anschließend die Methoden zur Steuerung der Motoren aufgerufen werden. Darunter fallen die Steuerungen für die Lüftung, Pumpe und LED. Außerdem wird überprüft, ob noch eine Internetverbindung besteht. Falls dies an dieser Stelle nicht der Fall sein sollte, wird ein Versuch unternommen, sich neu zu verbinden.  

Dies ist für die folgende Übermittlung der Daten an Node-RED essentiell.  



### Vierter Programmabschnitt: Hilfsmethoden
Die Hilfsmethode für die Übermittlung der Daten an Node-RED ("sendNodeRed()") hat als Übergabeparameter die Luftfeuchtigkeit, die Temperatur, die Bodenfeuchtigkeit und den Füllstand des Wassertankes. Die Bodenfeuchtigkeit und der Füllstand des Wassertankes werden jeweils von zwei Sensoren gemessen. Das hat zur Folge, dass eine Ungleichverteilung des Wassers beim Gießen im Topf bemerkt werden kann und das der Wasserstand für eine Meldung sorgt, die einen über einen Füllstand von ein drittel und ein sechstel informiert. Diese Meldung erfolgt über Node-RED zu Discord. In der Methode erfolgt die Veröffentlichung der gemessenen Daten in jeweils eigenen Kategorien.  

Die folgende Methode "sendNodeRedWarning()" hat als Übergabeparameter die Sensordaten der beiden Wassertankfüllstand-Sensoren. Dabei werden von der Mehtode "sendNodeRed()" unabhängig Warnungen an Node-RED gesendet, wenn die jeweiligen Füllstände unterschritten werden.  

Innerhalb der Methode "reconnect()" wird für fünf Sekunden probiert sich erneut mit dem WLAN zu verbinden.  

Die Methode "callback()" wartet durchgehend auf eingehende Nachrichten von der Noder-RED-Instanz. Dies ermöglicht es, dass auf dem Discordserver eine Nachricht zur Statusabfrage geschickt werden kann und das Programm dementsprechend darauf reagiert.  

Die Methode "ledSteuerung()" ist zur Steuerung der LED da. Dabei erfolgt dies nach Abfrage der aktuellen Uhrzeit und abhängig von dieser. Eine kontrollierte Zeitsteuerung der LED ist hier eingebunden. Die LED-Methode hat als Übergabe einen Wert für einen Steuerchanel. Dieser ist unterscheidet sich von dem der Pumpensteuerung und dem der Lüftersteuerung, damit diese separat angesteuert werden können.  

Die Methode "pumpenSteuerung()" bekommt als Übergabeprarameter auch einen Chanel und zusätzlich noch die Werte der beiden Bodenfeuchtigkeitssensoren. Der Chanel ist ebenfalls von dem der Lüftersteuerung verschieden. Die Sensoren übergeben einen Wert, welcher dann für die Abfrage der Anschaltung der Pumpe benutzt wird. Am Anfang der Methode wird eine Zeitabfrage, wie bereits vorher erläutert, eingebaut, welche für ein fünfzehnsekündiges Warten der Methode sorgt. Dies ist für das Versickern des Wassers eingebaut, damit die Sensoren akurate Werte von der Bodenfeuchtigkeit übertragen können. Das Pumpen wird programmtechnisch nach zwei Sekunden beendet.  

Die Steueung der Lüfter erfolgt duch die Methode "turbinenSteuerung()", dieser wird der Chanel und die gemessene Temperatur in Celsius übergeben.
Dabei erfolgt die Intesitätseintstellung der Lüftung durch Bedingungen abhängig von der Temperatur. Auf diesem Chanel liegen beide Lüfter die Instanziiert wurden.  



## Einstellungsmöglichkeiten

Zeitsteuerung der LED: Grenzen der Abfrage (int: Uhrzeit von 0 bis 24) [Zeile 442].  
Intensität der LED: ledValue (int: 0 <= ledValue <= 255) [Zeile 443].  

Länge des Intervalls fürs aktive Pumpen: intervalPumpe (long: in ms) [Zeile 130].  
Länge des Intervalls fürs warten der Pumpe: intervalPumpe7 (long: in ms) [Zeile 133].  
Intensität der Pumpe: pumpeValue (int: 0 <= pumpeValue <= 255) [Zeile 467].  
Grenzwert zum Beginnen vom Pumpen: Untere Grenzen der Abfragen (int: 0 < x < 2500; empfohlen: 1900) [Zeile 466]. 

Intensitätssteuerung der Lüfter: turbinenValue (int: 0 <= turbinenValue <= 255) [Zeilen 492 und 495].  
Temperaturgrenzwerte: Untere Grenzen der Abfragen (float: 0 bis 50) [Zeilen 491 und 494].  
Abfrageintervall des Temperatursensors: intervalTemperatur (long: in ms) [Zeile 124].  
