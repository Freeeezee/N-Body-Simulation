# N-Body-Problem

## Installation

### Vorraussetzungen Windows
- Visual Studio Code 2022 ist installiert
- MS-MPI-SDK und MS-MPI Runtime sind installiert
- Grafiktreiber mit OpenCl Unterstützung für den vorhandenen Grafikkarten typ sind installiert

### Installationsschritte

1. Clone das Repo mit folgendem Command: `git clone --recursive https://github.com/KN-PACO/N-Body-Problem.git`
   <br> WICHTIG: Das `--recursive` flag sorgt dafür, dass auch die eingebundenen Repos mit geklont werden. Ohne diese läuft das Program nicht.
2. Öffne Visual Studio 2022, klicke auf "Open a local Folder" und wähle den Projekt Ordner aus (N-Body-Problem).
3. Stelle sicher, dass im Output Fenster unten am Ende steht: "CMake generation finished."
4. Klicke dann oben in auf das Configuration Drop-Down - üblicher weise ist dort x64-Debug ausgewählt - und gehe auf "Manage Configuration".
5. Klicke dort links auf das grüne "+" und wähle "x64-Release" aus.
6. Unter "Configuration type" wähle "Release" aus.
7. Drücke Strg-S um die neue Konfiguration zu speichern. Darauf hin wirst du bei Output sehen, dass das CMake file neu generiert wurde.
8. Wähle nun oben im Configuration Drop-Down die neu definierte "x64-Release" Konfiguration aus.
9. Wähle dann direkt rechts daneben im "Select Startup item" Drop-Down n_body_simulation.exe
10. Drücke nun rechts daneben auf "Run without Debugging" oder drücke alternativ Strg-F5.


