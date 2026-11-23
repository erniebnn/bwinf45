#import "template.typ": conf

#show: conf.with(
  title: "45. Bundeswettbewerb Informatik, Aufgabe 3",
  team-id: "00291",
  team-name: "Einzelteam",
  authors: (
    "Erwin Raupach",
  ),
)

= Lösungsidee

Runden werden eingeteilt in drei Arten:
- Relevant: das Ergebnis dieser Runde beeinflusst den Ausgang des Spiels immer
- Lokal Irrelevant: das Ergebnis dieser Runde beeinflusst den Ausgang des Spiels nicht immer
- Global Irrelevant: das Ergebnis dieser Runde beeinflusst den Ausgang des Spiels nie

Um dies nun für jede einzelne der N Runden zu bestimmen, wird für jede Runde jedes "Pregame" (Menge der möglichen Ergebnisse der vorherigen Runden) und jedes "Postgame" (Menge der möglichen Ergebnisse der folgenden Runden) generiert, und dann für jede Kombination von Pregame und Postgame der Ausgang überprüft, wenn die aktuelle Runde sowohl gewonnen als auch verloren wurde. Ausgang bezeichnet dabei sowohl das Gesamtergebnis (Gewonnen oder Verloren), als auch die länge des Spiels in Runden.

Sind diese Ausgänge, für jeweils alle Postgames gleich, so ist die Runde Lokal Irrelevant, da es (mindestens) ein Pregame gibt, für das der Ausgang der Runde Irrelevant ist.

Ist die Runde für alle Pregames Lokal Irrelevant, so ist sie Global Irrelevant, da es kein Pregame gibt, für welches die Runde eine Bedeutung im Ergebnis des Spiels hätte.

Dies wird für jede Runde, mit jeden Pre- und Postgame geprüft, und ausgegeben. Wenn für ein gegebenes Pregame allerdings schon ein Ergebnis des Spiels feststeht, so muss die Prüfung auf lokale Irrelevanz für dieses Pregame übersprungen werden, da diese Runde nicht stattfindet (das Spiel endet sobald sein Ergebnis feststeht).

= Umsetzung

Eine bestimmte Konfiguration eines Spiels wird als Array von booleans dargestellt:

$
("Pregame"..., X, "Postgame"...)
$

wobei $X$ einmal `true` (Gewonnen) und `false` (Verloren) ist. Dieses Array wird für alle Pre- und Postgames gebaut, und dann für beide $X$ auf Länge und Ergebnis geprüft. Das überprüfen einer Runde wird von der Funktion `game_otc` (@game_otc) übernommen.

Die enumeration aller Pre- und Postgames, als auch das überprüfen auf lokale und globale Irrelevanz wird von der Funktion `evaluate` (@evaluate) übernommen.

Die Überprüfung ob das aktuelle Pregame schon entschieden ist findet in der Funktion `game_determined` (@game_determined) statt.

= Werkzeuge

- Code Editor: Code - OSS 1.138.0
- C-Compiler: Clang 22.1.8
- Build System: GNU Make 4.4.1
- Version Control: Git 2.55.0
- Verwendete Libraries:
  - glibc (C Standartbibliothek)

= Beispiele

== punkte00.txt

#align(left)[
  ```
  $ bin/aufgabe3 assets/punkte00.txt 
  Rounds: 3, Points: 1 2 3 , Target: 4
  Locally irrelevant round 1: W X 
  ```
]

== punkte01.txt

#align(left)[
  ```
  $ bin/aufgabe3 assets/punkte01.txt 
  Rounds: 6, Points: 9 5 13 7 1 15 , Target: 22
  Locally irrelevant round 3: W L L X 
  Locally irrelevant round 3: L L W X 
  Locally irrelevant round 4: W L L L X 
  Locally irrelevant round 4: W W L L X 
  Locally irrelevant round 4: L L W L X 
  Locally irrelevant round 4: L W W L X 
  Locally irrelevant round 4: L L L W X 
  Locally irrelevant round 4: W L L W X 
  Locally irrelevant round 4: L W L W X 
  Locally irrelevant round 4: L L W W X 
  ```
]

== punkte02.txt

#align(left)[
  ```
  $ bin/aufgabe3 assets/punkte02.txt 
  Rounds: 4, Points: 10 10 1 10 , Target: 20
  Locally irrelevant round 2: W L X 
  Locally irrelevant round 2: L W X 
  Globally irrelevant round 2: L L X 
  ```
]

== punkte03.txt

#align(left)[
  ```
  $ bin/aufgabe3 assets/punkte03.txt 
  Rounds: 18, Points: 3 5 2 8 12 10 4 2 7 8 5 3 2 6 9 4 3 12 , Target: 2
  ```
]

== punkte04.txt

#align(left)[
  ```
  $ bin/aufgabe3 assets/punkte04.txt 
  Rounds: 15, Points: 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 , Target: 428
  ```
]

== punkte05.txt

#align(left)[
  ```
  $ bin/aufgabe3 assets/punkte05.txt
  // view aufgabe3/doc/log_punkte05.txt
  ```
]

== punkte06.txt

#align(left)[
  ```
  $ bin/aufgabe3 assets/punkte06.txt
  Rounds: 9, Points: 20 20 20 20 1 2 2 2 2 , Target: 40
  Globally irrelevant round 4: L L L L X 
  Globally irrelevant round 5: L L L L L X 
  Globally irrelevant round 6: L L L L L L X 
  Globally irrelevant round 7: L L L L L L L X 
  Globally irrelevant round 8: L L L L L L L L X 
  ```
]

= Quellcode

== Game Outcome <game_otc>

```c
u32 game_otc(GameConfig* p_cfg, bool* p_game, bool* r_otc) {
    u64 cur_points = 0;
    for (u32 i = 0; i < p_cfg->rounds; i++) {
        if (p_game[i]) { cur_points += p_cfg->points[i]; }
        if (cur_points >= p_cfg->target) {
          *r_otc = true;
          return i;
        }
        if (cur_points + p_cfg->remaining_points[i] < p_cfg->target) {
          *r_otc = false;
          return i;
        }
    }
    ipanic("Unreachable code executed!");
}
```

== Evaluate <evaluate>

```c
void evaluate(GameConfig* p_cfg) {
    bool* cur_game = (bool*) calloc(p_cfg->rounds, sizeof(*cur_game));
    assert(cur_game != NULL, "Failed to allocate memory!");
    for (u32 rnd = 0; rnd < p_cfg->rounds; rnd++) {
        u32 postgame_len = p_cfg->rounds - rnd - 1;
        bool globally_irrelevant = true;
        for (u64 pregame = 0; pregame < (1 << rnd); pregame++) {
            if (!game_determined(p_cfg, cur_game, rnd)) {
                bool locally_irrelevant = true;
                for (u64 postgame = 0; postgame < (1 << postgame_len); postgame++) {
                    bool otc0, otc1;
                    cur_game[rnd] = false;
                    u32 len0 = game_otc(p_cfg, cur_game, &otc0);
                    cur_game[rnd] = true;
                    u32 len1 = game_otc(p_cfg, cur_game, &otc1);
                    print_locally_irrelevant(p_cfg, cur_game, (u32) -1); printf("\n");
                    if (otc0 != otc1 || len0 != len1) { locally_irrelevant = false; }
                    increment_game(p_cfg, cur_game, rnd + 1);
                }
                if (locally_irrelevant) {
                    printf("Locally irrelevant round %d: ", rnd);
                    print_irrelevant(p_cfg, cur_game, rnd);
                    printf("\n");
                }
                else {
                    globally_irrelevant = false;
                }
            }
            increment_game(p_cfg, cur_game, 0);
        }
        if (globally_irrelevant) {
            printf("Globally irrelevant round %d: ", rnd);
            print_irrelevant(p_cfg, cur_game, rnd);
            printf("\n");
        }
    }
    free(cur_game);
}
```

== Game Determined <game_determined>

```c
bool game_determined(GameConfig* p_cfg, bool* p_game, u32 p_rnd) {
    if (p_rnd == 0) {
      return (p_cfg->target == 0) || (p_cfg->remaining_points[0] + p_cfg->points[0] == 0);
    }
    u32 cur_points = 0;
    for (u32 i = 0; i < p_rnd; i++) {
        cur_points += p_game[i] ? p_cfg->points[i] : 0;
    }
    if (cur_points >= p_cfg->target) { return true; }
    if (cur_points + p_cfg->remaining_points[p_rnd - 1] < p_cfg->target) { return true; }
    return false;
}
```