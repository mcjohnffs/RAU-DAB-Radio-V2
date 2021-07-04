# Installation der Entwicklungsumgebung

Die Software kann entweder in der Arduino IDE oder in Visual Studio Code bearbeitet und kompiliert werden.

## Arduino IDE

### Voraussetzungen




-[Arduino IDE Software](http://www.arduino.cc/en/main/software) ab Version 1.8.X

-[Arduino ESP32 Core](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)

Damit sich der ESP32-Mikrocontroller mit der Arduino Umgebung versteht, wird hierzu der dafür vom Hersteller entwickelte Core für die Arduino Umgebung benötigt.

### Installation und Konfiguration des Arduino ESP32 Core

* Arduino IDE starten und im Menütab "File" -> "Preferences" öffnen.

![Ard_1](/assets/images/preferences.png)

* Bei "Additional Boards Manager URLs" den Link zum Core Release einfügen -> 
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json ![Ard_2](/assets/images/Corelink.png)

Anschliessend auf "OK" klicken und das Menü verlassen.

* Im Menütab "Tools/Board:...." dann "Boards Manager" ![Ard_3](/assets/images/boardsmanager.png)

Anschliessend öffnet sich der "Boards Manager". Im Suchfenster "ESP32" suchen und installieren.![Ard_3_1](/assets/images/boardsinstall.png)

* Unter "Tools/Board:..." auf "ESP32 Arduino" gehen und "ESP32 Dev Module" auswählen. Nun ist das Board mit den dazugehörigen Einstellungen ausgewählt und für Kompilierung und Upload bereit.

### Kompilieren und Flashen in der Arduino IDE

## Visual Studio Code mit Arduino (Extension)

### Voraussetzungen


-[Visual Studio Code Software](https://code.visualstudio.com/download)

-Marketplace Link [VSC Arduino Extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino)

Um die Entwicklung per Arduino Umgebung zu ermöglichen wird eine zusätzliche Extension benötigt. Diese kann direkt in der Software-Umgebung oder per Marketplace Link installiert werden.

!!! important "Wichtig"
    Arduino IDE inkl. ESP32 Arduino Core muss als erstes installiert werden um das Kompilieren und Uploaden von Arduino Code mit der Extension in Visual Studio Code zu ermöglichen.

 

### Kompilieren und Flashen in Visual Studio Code