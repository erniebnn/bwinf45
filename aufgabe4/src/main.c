#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/*
 * Petanque Playing Field (13x25):
 *
 *   | --------- 25 -------- |
 * - . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * 1 . . . . . . . . . . . . .
 * 3 . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * | . . . . . . . . . . . . .
 * - . . . . . . . . . . . . .
 * 
 * 1. Pick Direction
 * 2. Pick Row / Column
 * 3. Execute Throw
 * 4. Check Win / Draw Condition
 *
 */

#ifndef max
#  define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

const int WIN_STATUS_MIN_WIDTH  = 90;
const int WIN_STATUS_HEIGHT = 6;
const int WIN_BOARD_WIDTH   = 31;
const int WIN_BOARD_HEIGHT  = 17;
const int WIN_STDSCR_MIN_WIDTH  = max(WIN_STATUS_MIN_WIDTH, WIN_BOARD_WIDTH); // max because windows are vertically stacked
const int WIN_STDSCR_MIN_HEIGHT = WIN_STATUS_HEIGHT + WIN_BOARD_HEIGHT;

typedef enum Player {
    PLAYER_ONE,
    PLAYER_TWO,
    PLAYER_NONE,
} Player;

typedef enum GameStatus {
    GMSTAT_INTRO,
    GMSTAT_DIRECTION,
    GMSTAT_COORD,
    GMSTAT_EXEC,
    GMSTAT_DRAW,
    GMSTAT_END,
} GameStatus;

typedef enum BallKind : unsigned char {
    BALL_NONE    = 0,
    BALL_PLAYER1 = 1,
    BALL_PLAYER2 = 2,
    BALL_TARGET  = 3,
} BallKind;

typedef enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
} Direction;

char ballkindmap[] = {
    [BALL_NONE]    = '.',
    [BALL_PLAYER1] = 'O',
    [BALL_PLAYER2] = 'X',
    [BALL_TARGET]  = '#',
};

char* directionmap[] = {
    [DIR_UP]    = "Up"   ,
    [DIR_DOWN]  = "Down" ,
    [DIR_LEFT]  = "Left" ,
    [DIR_RIGHT] = "Right",
};

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

bool debug;
int blink_len;

static int randint(int p_lower, int p_upper) {
    return (rand() % (p_upper - p_lower)) + p_lower;
}

static bool is_off_board(int p_x, int p_y) {
    return (0 > p_x || p_x > 12 || 0 > p_y || p_y > 12);
}

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

void draw_status(Game* p_game) {
    if (p_game->status_cur == GMSTAT_INTRO) { return; }
    WINDOW* status_win = p_game->status_win;
    werase(status_win);
    box(status_win, 0, 0);
    const char* gmstatstrs[] = {
        "",
        "Pick your direction with the arrow keys / WASD, confirm with space!",
        "Select your starting position with the arrow keys / WASD, confirm with space!",
        "Executing your turn!",
        "Target got knocked from the board, it's a draw!",
        "Game over!",
    };
    if (p_game->status_cur != GMSTAT_END && p_game->status_cur != GMSTAT_DRAW) {
        mvwprintw(status_win, 1, 1, "Round %d/%d", p_game->round + 1, p_game->rounds);
        mvwprintw(status_win, 2, 1, "Player: Player%d", p_game->player_cur + 1);
    }
    mvwprintw(status_win, 3, 1, "Status: %s", gmstatstrs[p_game->status_cur]);
    if (p_game->status_cur == GMSTAT_DIRECTION) {
        mvwprintw(status_win, 4, 1, "Selected Direction: %s", directionmap[p_game->direction_cur]);
    }
    if (p_game->status_cur == GMSTAT_EXEC && p_game->miss) {
        mvwprintw(status_win, 4, 1, "Missed the board!");
    }
    if (p_game->status_cur == GMSTAT_END) {
        if (p_game->winner == PLAYER_NONE) {
            mvwprintw(status_win, 1, 1, "It's a draw!");
        }
        else {
            mvwprintw(status_win, 1, 1, "Player%d won!", (int) p_game->winner + 1);
        }
        mvwprintw(status_win, 2, 1, "Press q to exit!");
    }
    wnoutrefresh(status_win);
}

static void draw_board(Game* p_game) {
    WINDOW* board_win = p_game->board_win;
    werase(board_win);
    box(board_win, 0, 0);
    if (p_game->status_cur == GMSTAT_INTRO) {
        mvwprintw(board_win, 1, 1, "Welcome to computer Petanque!");
        mvwprintw(board_win, 2, 1, "Please Enter the number of");
        mvwprintw(board_win, 3, 1, "rounds you want to play,");
        mvwprintw(board_win, 4, 1, "and press space to accept!");
        mvwprintw(board_win, 5, 1, "(max 256): %d", p_game->rounds);
    }
    else {
        for (int y = 0; y < 13; y++) {
            for (int x = 0; x < 13; x++) {
                mvwaddch(board_win, 2 + y, 3 + 2 * x, ballkindmap[p_game->board[y][x]]);
            }
        }
        if (p_game->status_cur == GMSTAT_COORD) {
            if (p_game->direction_cur == DIR_UP) {
                mvwaddch(board_win, WIN_BOARD_HEIGHT - 2, 2 * p_game->shoot_x + 3, '^');
            }
            else if (p_game->direction_cur == DIR_DOWN) {
                mvwaddch(board_win, 1, 2 * p_game->shoot_x + 3, 'v');
            }
            else if (p_game->direction_cur == DIR_LEFT) {
                mvwaddch(board_win, p_game->shoot_y + 2, WIN_BOARD_WIDTH - 2, '<');
            }
            else {
                mvwaddch(board_win, p_game->shoot_y + 2, 1, '>');
            }
        }
        if (p_game->status_cur == GMSTAT_EXEC) {
            if (!p_game->miss) {
                if (p_game->blink_counter % 21 <= 10) wattron(board_win, A_REVERSE);
                int win_x = 2 * p_game->hit_x + 3, win_y = p_game->hit_y + 2;
                mvwaddch(p_game->board_win, win_y, win_x, ballkindmap[p_game->board[p_game->hit_y][p_game->hit_x]]);
                wattroff(board_win, A_REVERSE);
            }
        }
    }
    wnoutrefresh(board_win);
}

static void handle_keypress(Game* p_game, int p_ch) {
    if (p_game->status_cur == GMSTAT_INTRO) {
        if ('0' <= p_ch && p_ch <= '9') {
            int digit = p_ch - '0';
            p_game->rounds *= 10;
            p_game->rounds += digit;
            if (p_game->rounds > 256) {
                p_game->rounds = 256;
            }
        }
        else if (p_ch == KEY_BACKSPACE) {
            p_game->rounds /= 10;
        }
        else if (p_ch == ' ') {
            p_game->transition_state = true;
        }
    }
    else if (p_game->status_cur == GMSTAT_DIRECTION) {
        switch (p_ch) {
            case KEY_UP:
            case 'w':
                p_game->direction_cur = DIR_UP;
                break;
            case KEY_DOWN:
            case 's':
                p_game->direction_cur = DIR_DOWN;
                break;
            case KEY_LEFT:
            case 'a':
                p_game->direction_cur = DIR_LEFT;
                break;
            case KEY_RIGHT:
            case 'd':
                p_game->direction_cur = DIR_RIGHT;
                break;
            case ' ':
                p_game->transition_state = true;
                break;
        }
    }
    else if (p_game->status_cur == GMSTAT_COORD) {
        if (p_ch == ' ') {
            p_game->transition_state = true;
        }
        if (p_game->direction_cur == DIR_LEFT || p_game->direction_cur == DIR_RIGHT) {
            switch (p_ch) {
                case KEY_UP:
                case 'w':
                    p_game->shoot_y -= (p_game->shoot_y > 0) ? 1 : 0;
                    break;
                case KEY_DOWN:
                case 's':
                    p_game->shoot_y += (p_game->shoot_y < 12) ? 1 : 0;
            }
        }
        else {
            switch (p_ch) {
                case KEY_LEFT:
                case 'a':
                    p_game->shoot_x -= (p_game->shoot_x > 0) ? 1 : 0;
                    break;
                case KEY_RIGHT:
                case 'd':
                    p_game->shoot_x += (p_game->shoot_x < 12) ? 1 : 0;
            }
        }
    }
}

static void transition_state(Game* p_game) {
    if (!p_game->transition_state) { return; }
    if (p_game->status_cur == GMSTAT_INTRO) {
        p_game->status_cur = GMSTAT_DIRECTION;
    }
    else if (p_game->status_cur == GMSTAT_DIRECTION) {
        p_game->status_cur = GMSTAT_COORD;
        p_game->shoot_x = (p_game->direction_cur == DIR_LEFT) ? 12 : 0;
        p_game->shoot_y = (p_game->direction_cur == DIR_UP  ) ? 12 : 0;
    }
    else if (p_game->status_cur == GMSTAT_COORD) {
        p_game->status_cur = GMSTAT_EXEC;
        int in_dir = randint(1, 6), off_dir = debug ? 0 : randint(0, 3) - 1;
        Direction dir_cur = p_game->direction_cur;
        if (dir_cur == DIR_UP || dir_cur == DIR_DOWN) {
            p_game->hit_x = p_game->shoot_x + off_dir;
            p_game->hit_y = p_game->shoot_y + in_dir * (dir_cur == DIR_DOWN ? 1 : -1);
        }
        else {
            p_game->hit_x = p_game->shoot_x + in_dir * (dir_cur == DIR_RIGHT ? 1 : -1);
            p_game->hit_y = p_game->shoot_y + off_dir;
        }
        p_game->miss = is_off_board(p_game->hit_x, p_game->hit_y);
        p_game->blink_counter = 0;
    }
    p_game->transition_state = false;
}

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
                p_game->round++;
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

static void new_board(Game* p_game) {
    memset(p_game->board, BALL_NONE, 13 * 13 * sizeof(p_game->board[0][0]));
    int x = randint(0, 13), y = randint(0, 13);
    p_game->target_x = x;
    p_game->target_y = y;
    p_game->board[y][x] = BALL_TARGET;
}

int main(int p_argc, const char** p_argv) {
    srand((unsigned int) time(NULL));

    if (p_argc == 2 && (strcmp(p_argv[1], "--debug") == 0)) {
        debug = true;
        blink_len = 5;
    }
    else {
        debug = false;
        blink_len = 40;
    }

    initscr();
    keypad(stdscr, true);
    curs_set(0);
    timeout(50);

    Game game = (Game) {
        .screen_valid = false,
        .status_win = NULL,
        .board_win = NULL,
        .round = 0,
        .rounds = 0,
        .player_cur = PLAYER_ONE,
        .winner = PLAYER_NONE,
        .status_cur = GMSTAT_INTRO,
        .board = {},
        .transition_state = false,
        .direction_cur = DIR_UP,
        .shoot_x = 0,
        .shoot_y = 0,
        .hit_x = 0,
        .hit_y = 0,
        .blink_counter = 0,
        .miss = false,
    };
    
    reconfigure_windows(&game);
    new_board(&game);

    bool running = true;
    while (running) {
        int ch = getch();
        switch (ch) {
            case KEY_RESIZE:
                reconfigure_windows(&game);
                break;
            case 'q':
                running = false;
                break;
            case ERR:
                break;
            default:
                if (game.screen_valid) handle_keypress(&game, ch);
        }
        if (game.screen_valid) {
            werase(stdscr);
            wnoutrefresh(stdscr);
            transition_state(&game);
            update(&game);
            draw_status(&game);
            draw_board(&game);
        }
        doupdate();
    }

    delwin(game.status_win);
    delwin(game.board_win);
    endwin();

    return 0;
}