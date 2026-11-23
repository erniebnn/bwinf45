#import "template.typ": conf

#show: conf.with(
  title: "45. Bundeswettbewerb Informatik, Aufgabe 4",
  team-id: "00291",
  team-name: "Einzelteam",
  authors: (
    "Erwin Raupach",
  ),
)

= Lösungsidee

All zu viel zu sagen zur Lösungsidee gibt es bei dieser Aufgabe ja nicht, immerhin ist das Konzept detailliert in der Aufgabe beschrieben. Die eine relevante Sache die mir einfällt, ist dass die Aufgabe bewusst die Anzahl der Runden weglässt. Ich habe mich dazu entschieden diese Entscheidung einfach an den Spieler weiterzugeben, zu beginn wird man gefragt wie viele Runden man spielen will (maximal 256).

= Umsetzung

Da ich diese Aufgabe, so wie alle anderen auch in C umsetzen wollte, und ich keine lust hatte mir irgend ein komplexeres Grafik-Framework anzueignen, habe ich mich dazu entschieden die Aufgabe mit `ncurses` im Terminal umzusetzen.

== Visuelle Präsentation

Auf dem Spielfeld befinden sich zwei Fenster, einmal oben das Status-Fenster, welches dem Spieler aktuelle Informationen über das Spiel mitteilt, und in der Mitte das eigentliche Petanque Spielfeld. Zu beginn des Spiels, und bei jeder Veränderung der Größe des Terminals wird überprüft, ob beide Fenster noch in das Terminal passen. Falls nicht wird eine Fehlermeldung angezeigt, und das Spiel gestoppt, bis das Terminal groß genug ist.

Das Petanque-Feld selbst ist dargestellt als 25x13 character großes Feld, horizontal ist immer eine Lücke zwischen den einzelnen Feldern gelassen, damit das Petanque-Feld in etwa quadratisch ist (die Terminal characters sind ja nicht im Seitenverhältnis 1:1). Ein Punkt `.` repräsentiert ein leeres Feld. Dazu habe ich mich entschieden, damit es besser ersichtlich ist wo die Felder jetzt tatsächlich liegen, als wenn ich Leerzeichen verwendet hätte. Die Zielkugel ist ein Hash `#`, und die Kugeln der Spieler sind ein `O` (Player 1) und ein `X` (Player 2).

== Spielablauf

Die beiden Spieler sind abwechselnd an der Reihe. Zuerst wird eine Richtung gewählt, und dann an der entsprechenden Kante des Spielfeldes ein Startfeld, von dem der Zug ausgeführt wird. Danach wird das Feld nach den gegebenen Regeln errechnet, auf dem die Kugel landet. Dieses blinkt für eine kurze Zeit, damit ersichtlich ist, wo die Kugel landet. Danach wird der Zug ausgeführt, Kugeln die im Weg liegen werden in Richtung des Wurfes verschoben.

Zuletzt wird geprüft ob die Win/Lose/Draw Bedingungen erfüllt sind, also ob die Runden vorbei sind, oder die Zielkugel vom Feld geschoben wurde. Falls ja wird das Spiel frühzeitig beendet, und das Ergebnis im Status-Fenster angezeigt.

== Anwendung

Jederzeit `q` um zu verlassen. Für die Richtungen und zum bewegen des Cursors die Pfeiltasten und zum Bestätigen die Leertaste.

`--debug` als Kommandozeilenargument sorgt dafür, dass die Züge nicht zur Seite abdriften und schneller durchgeführt werden.

= Werkzeuge

- Code Editor: Code - OSS 1.138.0
- C-Compiler: Clang 22.1.8
- Build System: GNU Make 4.4.1
- Version Control: Git 2.55.0
- Verwendete Libraries:
  - glibc (C Standartbibliothek)
  - ncurses 6.6-2

= Beispiele

== Beispielhafter Spielablauf (4 Runden)

Initiales Spielfeld:
```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . # . . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Nach 1. Zug Spieler 1 (Richtung Oben, von unter der Zielkugel):

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . # . . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Nach 1. Zug Spieler 2 (Richtung Rechts, von links der Zielkugel)

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . X . . . . . . . .  │
│  . . . . . # . . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Nach 2. Zug Spieler 1 (Richtung Oben, von unter der Zielkugel):

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . X # . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Zielkugel wurde um 1 Feld nach oben verschoben!

Nach 2. Zug Spieler 2 (Richtung Oben, von unter der Zielkugel):

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . X . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . X # . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Nach 3. Zug Spieler 1 (Richtung Oben, von eins Rechts von unter der Zielkugel):

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . X O . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . X # . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Nach 3. Zug Spieler 2 (Richtung Oben, von eins Rechts von unter der Zielkugel):

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . X O . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . X # . . . . . . .  │
│  . . . . . O X . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Nach 4. Zug Spieler 1 (Richtung Oben, von unter der Zielkugel):

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O O . . . . . .  │
│  . . . . . X O . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . X # . . . . . . .  │
│  . . . . . O X . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Nach 4. Zug Spieler 2 (Richtung Oben, von unter der Zielkugel):

```
┌─────────────────────────────┐
│                             │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . . O . . . . . . .  │
│  . . . . . X O . . . . . .  │
│  . . . . . X O . . . . . .  │
│  . . . . . . . . . . . . .  │
│  . . . . X # . . . . . . .  │
│  . . . . . O X . . . . . .  │
│  . . . . . . . . . . . . .  │
│                             │
└─────────────────────────────┘
```

Zwei Kugeln wurden nach oben verschoben!

Statusanzeige nach dem Ende des Spiels:

```
┌──────────────────────────────────────────────────────────────────────────────┐
│It's a draw!                                                                  │
│Press q to exit!                                                              │
│Status: Game over!                                                            │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```

= Quellcode

== Spiel Zustand `struct`:

```c
typedef struct Game {
    bool screen_valid;
    WINDOW* status_win;
    WINDOW* board_win;

    int round;
    int rounds;
    Player player_cur;
    Player winner;
    GameStatus status_cur;

    int target_x;
    int target_y;
    BallKind board[13][13];

    bool transition_state;

    Direction direction_cur;
    int shoot_x;
    int shoot_y;
    int hit_x;
    int hit_y;

    int blink_counter;
    bool miss;
} Game;
```

== Window Positioning Logic

```c
static void reconfigure_windows(Game* p_game) {
    int maxx = getmaxx(stdscr), maxy = getmaxy(stdscr);
    if (maxx < WIN_STDSCR_MIN_WIDTH || maxy < WIN_STDSCR_MIN_HEIGHT) {
        p_game->screen_valid = false;
        werase(stdscr);
        mvwprintw(stdscr, 0, 0, "Terminal Size too small, needed W: %d H: %d, got W: %d H: %d", WIN_STDSCR_MIN_WIDTH, WIN_STDSCR_MIN_HEIGHT, maxx, maxy);
        wnoutrefresh(stdscr);
    }
    else {
        p_game->screen_valid = true;
        werase(stdscr);
        wnoutrefresh(stdscr);
        // Status Window
        int status_origin_x = 0, status_origin_y = 0;
        int status_width = maxx, status_height = WIN_STATUS_HEIGHT;
        if (p_game->status_win) {
            mvwin(p_game->status_win, status_origin_y, status_origin_x);
            wresize(p_game->status_win, status_height, status_width);
        }
        else {
            p_game->status_win = newwin(status_height, status_width, status_origin_y, status_origin_x);
        }

        // Board Window
        int board_origin_x = (maxx - WIN_BOARD_WIDTH) / 2, board_origin_y = (maxy - WIN_STATUS_HEIGHT - WIN_BOARD_HEIGHT) / 2 + WIN_STATUS_HEIGHT;
        int board_width = WIN_BOARD_WIDTH, board_height = WIN_BOARD_HEIGHT;
        if (p_game->board_win) {
            mvwin(p_game->board_win, board_origin_y, board_origin_x);
            wresize(p_game->board_win, board_height, board_width);
        }
        else {
            p_game->board_win = newwin(board_height, board_width, board_origin_y, board_origin_x);
        }
    }
}
```

== Main Update Loop

```c
static void update(Game* p_game) {
    if (p_game->status_cur == GMSTAT_EXEC) {
        if (p_game->blink_counter < blink_len) {
            p_game->blink_counter++;
        }
        else {
            Direction dir_cur = p_game->direction_cur;
            int step_x = (dir_cur == DIR_RIGHT) ? 1 : (dir_cur == DIR_LEFT) ? -1 : 0;
            int step_y = (dir_cur == DIR_DOWN ) ? 1 : (dir_cur == DIR_UP  ) ? -1 : 0;
            int cur_x = p_game->hit_x, cur_y = p_game->hit_y;
            BallKind cur_ball;
            BallKind next_ball = (p_game->player_cur == PLAYER_ONE) ? BALL_PLAYER1 : BALL_PLAYER2;
            while (next_ball != BALL_NONE && !is_off_board(cur_x, cur_y)) {
                cur_ball = p_game->board[cur_y][cur_x];
                p_game->board[cur_y][cur_x] = next_ball;
                if (next_ball == BALL_TARGET) {
                    p_game->target_x = cur_x;
                    p_game->target_y = cur_y;
                }
                cur_x += step_x; cur_y += step_y;
                next_ball = cur_ball;
            }
            if (next_ball == BALL_TARGET) {
                p_game->status_cur = GMSTAT_DRAW;
            }
            else {
                p_game->player_cur = (p_game->player_cur == PLAYER_ONE) ? PLAYER_TWO : PLAYER_ONE;
                p_game->round += (p_game->player_cur == PLAYER_ONE) ? 1 : 0;
                if (p_game->round == p_game->rounds) {
                    int target_x = p_game->target_x, target_y = p_game->target_y;
                    BallKind left  = is_off_board(target_x - 1, target_y    ) ? BALL_NONE : p_game->board[target_y    ][target_x - 1];
                    BallKind right = is_off_board(target_x + 1, target_y    ) ? BALL_NONE : p_game->board[target_y    ][target_x + 1];
                    BallKind up    = is_off_board(target_x    , target_y - 1) ? BALL_NONE : p_game->board[target_y - 1][target_x    ];
                    BallKind down  = is_off_board(target_x    , target_y + 1) ? BALL_NONE : p_game->board[target_y + 1][target_x    ];
                    int count_player1 = 0, count_player2 = 0;
                    count_player1 += (left  == BALL_PLAYER1) ? 1 : 0;
                    count_player1 += (right == BALL_PLAYER1) ? 1 : 0;
                    count_player1 += (up    == BALL_PLAYER1) ? 1 : 0;
                    count_player1 += (down  == BALL_PLAYER1) ? 1 : 0;
                    count_player2 += (left  == BALL_PLAYER2) ? 1 : 0;
                    count_player2 += (right == BALL_PLAYER2) ? 1 : 0;
                    count_player2 += (up    == BALL_PLAYER2) ? 1 : 0;
                    count_player2 += (down  == BALL_PLAYER2) ? 1 : 0;
                    if (count_player1 > count_player2) {
                        p_game->winner = PLAYER_ONE;
                    }
                    else if (count_player1 < count_player2) {
                        p_game->winner = PLAYER_TWO;
                    }
                    else {
                        p_game->winner = PLAYER_NONE;
                    }
                    p_game->status_cur = GMSTAT_END;
                }
                else {
                    p_game->status_cur = GMSTAT_DIRECTION;
                }
            }
        }
    }
}
```