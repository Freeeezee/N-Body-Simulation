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
4. Wenn nicht bereits ausgewählt wähle im "Select Startup item" Drop-Down `n_body_simulation.exe`
5. Klicke dann oben in auf das Configuration Drop-Down - üblicher weise ist dort x64-Debug ausgewählt - und gehe auf "Manage Configuration".
6. Klicke dort links auf das grüne "+" und wähle "x64-Release" aus.
7. Unter "Configuration type" wähle "Release" aus.
8. Drücke Strg-S um die neue Konfiguration zu speichern. Darauf hin wirst du bei Output sehen, dass das CMake file neu generiert wurde.
9. Wähle nun oben im Configuration Drop-Down die neu definierte "x64-Release" Konfiguration aus.
10. Wähle dann direkt rechts daneben im "Select Startup item" Drop-Down `n_body_simulation.exe`
11. Drücke nun rechts daneben auf "Run without Debugging" oder drücke alternativ Strg-F5.

### Executables

- `n_body_simulation.exe`: Führt `main.cpp` aus. Rechnet eine Simulation durch und spielt sie anschließend in einem OpenGL Fenster ab. Verwendeter Algorithmus und Body-Eigenschaften können in der main Datei angepasst werden.
- `n_body_simulation_body_gen.exe`: Generiert eine Startmenge an Bodies zufällig und speichert sie in `generated_bodies.txt`. Kann in `n_body_simulation.exe` oder `n_body_simulation_mpi.exe` eingelesen werden.
- `n_body_simulation_mpi.exe`: Führt die Simulation in MPI aus. Benötigt eine `generated_bodies.txt` im Ausführungsverzeichnis. Ausgeführt durch `mpiexec -n <N> n_body_simulation_mpi.exe`. Verwendeter Algorithmus und Time-Steps können in `main-mpi.cpp` angepasst werden.
- `n_body_simulation_test.exe`: Führt die in `main-test.cpp` definierte Testsuite aus. Nach Ausführung werden Zeitmetriken und Abweichungskoeffizienten falls eine Baseline definiert wurde in `test_suite_analytics_*steps_*bodies.csv` gespeichert.
