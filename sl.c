#include <ctype.h>
#include <curses.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl.h"

#define VERSION "5.06"

void add_smoke(int y, int x);
void add_man(int y, int x);
int add_C51(int x);
int add_D51(int x);
int add_TGV(int x);
int add_artillery(int x);
int add_sl(int x);
void option(char *str);
int my_mvaddstr(int y, int x, char *str);
void disco_colors(int x);

int ACCIDENT  = 0;
int C51       = 0;
int NUMBER    = -1;
int DISCO     = 0;
int SIGNAL    = 1;
int FLY       = 0;
int LOGO      = 0;
int WIND      = 0;
int TGV       = 0;
int ARTILLERY = 0;

int DEFAULT_COLOR = -1;

int my_mvaddstr(int y, int x, char *str)
{
  for ( ; x < 0; ++x, ++str) {
    if (*str == '\0') {
      return ERR;
    }
  }
  for ( ; *str != '\0'; ++str, ++x) {
    if (mvaddch(y, x, *str) == ERR) {
      return ERR;
    }
  }
  return OK;
}

void option(char *str)
{
  extern int ACCIDENT, C51, DISCO, SIGNAL, FLY, LOGO, WIND, TGV;

  while (*str != '\0') {
    switch (*str) {
      case 'a': ACCIDENT  = 1;   break;
      case 'c': C51       = 1;   break;
      case 'G': TGV       = 1;   break;
      case 'd': DISCO     = 1;   break;
      case 'e': SIGNAL    = 0;   break;
      case 'F': FLY       = 1;   break;
      case 'l': LOGO      = 1;   break;
      case 'w': WIND      = 200; break;
      case 's': ARTILLERY = 1 ;  break;
      case 'v':
        puts("Version: " VERSION);
        exit(0);
        break;
      default:
        if (isdigit(*str)) {
          NUMBER = (NUMBER < 0 ? 0 : NUMBER*10) + *str - '0';
        }
        break;
    }
    str++;
  }
}

bool ncurses_prepare_colors(void)
{
  if (!has_colors()) {
    return false;
  }

  start_color();
  if( use_default_colors() != OK ) {
    DEFAULT_COLOR = COLOR_BLACK;
  } else {
    assume_default_colors(DEFAULT_COLOR, DEFAULT_COLOR);
  }
  return true;
}

void disco_colors(int x)
{
  if (DISCO && (x + INT_MAX/2) % 4 == 2) {
    attron(COLOR_PAIR((x + INT_MAX/2) / 16 % 4 + 1));
  }
}

int main(int argc, char *argv[])
{
  int x, i;
  int base_usleep = 40000;

  for (i = 1; i < argc; ++i) {
    if (*argv[i] == '-') {
      option(argv[i] + 1);
    }
  }
  initscr();
  if (DISCO == 1) {
    ncurses_prepare_colors();
    init_pair(4, COLOR_RED, DEFAULT_COLOR);
    init_pair(3, COLOR_YELLOW, DEFAULT_COLOR);
    init_pair(2, COLOR_GREEN, DEFAULT_COLOR);
    init_pair(1, COLOR_CYAN, DEFAULT_COLOR);
  }
  if (!SIGNAL) {
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGSTOP, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
  }
  cbreak();
  noecho();
  curs_set(0);
  nodelay(stdscr, TRUE);
  leaveok(stdscr, TRUE);
  scrollok(stdscr, FALSE);
  /*
   *   first Non prototype TGV was orange, first 1 was red
   */
  if (TGV == 1) {
    if (ncurses_prepare_colors()) {
      init_pair(1, COLOR_WHITE, DEFAULT_COLOR);
      init_pair(2, COLOR_YELLOW, DEFAULT_COLOR);
      base_usleep /= 2;
      if (WIND) {
        WIND = 150;
      }
    } else {
      TGV = 0;
    }
  }

  for (x = COLS - 1; ; --x) {
    if (LOGO == 1) {
      if (add_sl(x) == ERR) break;
    }
    else if (C51 == 1) {
      if (add_C51(x) == ERR) break;
    }
    else if (TGV == 1) {
      if (add_TGV(x) == ERR) break;
    }
    else if (ARTILLERY == 1) {
      if (add_artillery(x) == ERR) break;
    }
    else {
      if (add_D51(x) == ERR) break;
    }
    getch();
    refresh();
    usleep(base_usleep - (WIND * 100));
  }

  if (DISCO == 1 && !TGV) {
    attroff(COLOR_PAIR(1));
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
    attroff(COLOR_PAIR(4));
  }

  clear();
  endwin();

  return(EXIT_FAILURE);
}


int add_sl(int x)
{
  static char *sl[LOGOPATTERNS][LOGOHEIGHT + 1] =
  {
    {LOGO1, LOGO2, LOGO3, LOGO4, LWHL11, LWHL12, DELLN},
    {LOGO1, LOGO2, LOGO3, LOGO4, LWHL21, LWHL22, DELLN},
    {LOGO1, LOGO2, LOGO3, LOGO4, LWHL31, LWHL32, DELLN},
    {LOGO1, LOGO2, LOGO3, LOGO4, LWHL41, LWHL42, DELLN},
    {LOGO1, LOGO2, LOGO3, LOGO4, LWHL51, LWHL52, DELLN},
    {LOGO1, LOGO2, LOGO3, LOGO4, LWHL61, LWHL62, DELLN}
  };

  static char *coal[LOGOHEIGHT + 1] =
  {
    LCOAL1, LCOAL2, LCOAL3, LCOAL4, LCOAL5, LCOAL6, DELLN
  };

  static char *car[LOGOHEIGHT + 1] =
  {
    LCAR1, LCAR2, LCAR3, LCAR4, LCAR5, LCAR6, DELLN
  };

  if (NUMBER < 0) {
    NUMBER = 2;
  }

  int i, j, y, py = 0;
  int LOGOLENGTH = 42 + 21*NUMBER;

  if (x < - LOGOLENGTH) {
    return ERR;
  }
  y = LINES / 2 - 3;

  if (FLY == 1) {
    y = (x / 6) + LINES - (COLS / 6) - LOGOHEIGHT;
    py = 2;
  }
  for (i = 0; i <= LOGOHEIGHT; ++i) {
    my_mvaddstr(y + i, x, sl[(LOGOLENGTH + x) / 3 % LOGOPATTERNS][i]);
    my_mvaddstr(y + i + py, x + 21, coal[i]);
    for (j = 2; j <= NUMBER + 1; ++j) {
      my_mvaddstr(y + i + py*j, x + 21*j, car[i]);
    }
  }
  if (ACCIDENT == 1) {
    add_man(y + 1, x + 14);
    for (j = 2; j <= NUMBER + 1; ++j) {
      add_man(y + 1 + py*j, x + 3 + 21*j);
      add_man(y + 1 + py*j, x + 11 + 21*j);
    }
  }
  add_smoke(y - 1, x + LOGOFUNNEL);
  disco_colors(x + LOGOFUNNEL);
  return OK;
}


int add_D51(int x)
{
  static char *d51[D51PATTERNS][D51HEIGHT + 1] =
  {
    {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7, D51WHL11, D51WHL12, D51WHL13, D51DEL},
    {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7, D51WHL21, D51WHL22, D51WHL23, D51DEL},
    {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7, D51WHL31, D51WHL32, D51WHL33, D51DEL},
    {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7, D51WHL41, D51WHL42, D51WHL43, D51DEL},
    {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7, D51WHL51, D51WHL52, D51WHL53, D51DEL},
    {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7, D51WHL61, D51WHL62, D51WHL63, D51DEL}
  };
  static char *coal[D51HEIGHT + 1] =
  {
    COAL01, COAL02, COAL03, COAL04, COAL05, COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL
  };

  if (NUMBER < 0) {
    NUMBER = 1;
  }

  int y, i, j, dy = 0;
  int D51LENGTH = 54 + 29*NUMBER;

  if (x < - D51LENGTH) {
    return ERR;
  }
  y = LINES / 2 - 5;

  if (FLY == 1) {
    y = (x / 7) + LINES - (COLS / 7) - D51HEIGHT;
    dy = 1;
  }
  for (i = 0; i <= D51HEIGHT; ++i) {
    my_mvaddstr(y + i, x, d51[(D51LENGTH + x) % D51PATTERNS][i]);
    for (j = 1; j <= NUMBER; ++j) {
      my_mvaddstr(y + i + dy*j, x + 24 + 29*j, coal[i]);
    }
  }
  if (ACCIDENT == 1) {
    add_man(y + 2, x + 43);
    add_man(y + 2, x + 47);
  }
  add_smoke(y - 1, x + D51FUNNEL);
  disco_colors(x + D51FUNNEL);
  return OK;
}

int add_TGV(int x)
{
  static char *tgv[TGVPATTERNS][TGVHEIGHT + 1] =
  {
    {TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6, TGVWHL1, TGVDEL},
    {TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6, TGVWHL2, TGVDEL}
  };
  static char *vagoon[TGVHEIGHT + 1] =
  {
    TGVVAG0, TGVVAG1, TGVVAG2, TGVVAG3, TGVVAG4, TGVVAG5, TGVVAG6, TGVVAG7, TGVDEL
  };

  if (NUMBER < 0) {
    NUMBER = 1;
  }

  int y, i, j, dy = 0;
  int TGVLENGTH = 54 + 57 * NUMBER;

  if (x < - TGVLENGTH) {
    return ERR;
  }
  y = LINES / 2 - 5;

  if (FLY == 1) {
    y = (x / 7) + LINES - (COLS / 7) - TGVHEIGHT;
    dy = 1;
  }
  attron(COLOR_PAIR(2));
  for (i = 0; i <= TGVHEIGHT; ++i) {
    my_mvaddstr(y + i, x, tgv[(TGVLENGTH + x) % TGVPATTERNS][i]);
    for (j = 1; j <= NUMBER; ++j) {
      my_mvaddstr(y + i + dy*j, x + 55*j, vagoon[i]);
    }
  }

  if (ACCIDENT == 1) {
    add_man(y + 2, x + 14);
    for (j = 1; j <= NUMBER; ++j) {
      add_man(y + dy*j + 3, x + 30 + 55*j);
      add_man(y + dy*j + 3, x + 35 + 55*j);
      add_man(y + dy*j + 3, x + 40 + 55*j);
      add_man(y + dy*j + 3, x + 45 + 55*j);
    }
  }
  attroff(COLOR_PAIR(2));

  return OK;
}

int add_artillery(int x)
{
  static char *locomotive[ARTILLERYPATTERNS][ARTILLERYHEIGHT + 1] =
  {
    {ARTILLERYLOC00, ARTILLERYLOC01, ARTILLERYLOC02, ARTILLERYLOC03, ARTILLERYLOC04, ARTILLERYLOC05, ARTILLERYLOC06, ARTILLERYLOC07, ARTILLERYLOC08, ARTILLERYLOC09, ARTILLERYLOC10, ARTILLERYLOC11, ARTILLERYLOC12, ARTILLERYLOC13, ARTILLERYWHL1, ARTILLERYDEL},
    {ARTILLERYLOC00, ARTILLERYLOC01, ARTILLERYLOC02, ARTILLERYLOC03, ARTILLERYLOC04, ARTILLERYLOC05, ARTILLERYLOC06, ARTILLERYLOC07, ARTILLERYLOC08, ARTILLERYLOC09, ARTILLERYLOC10, ARTILLERYLOC11, ARTILLERYLOC12, ARTILLERYLOC13, ARTILLERYWHL2, ARTILLERYDEL}
  };
  static char *wagons[ARTILLERYHEIGHT + 1] =
  {
    ARTILLERYWAG00, ARTILLERYWAG01, ARTILLERYWAG02, ARTILLERYWAG03, ARTILLERYWAG04, ARTILLERYWAG05, ARTILLERYWAG06, ARTILLERYWAG07, ARTILLERYWAG08, ARTILLERYWAG09, ARTILLERYWAG10, ARTILLERYWAG11, ARTILLERYWAG12, ARTILLERYWAG13, ARTILLERYWAG14, ARTILLERYDEL
  };

  if (NUMBER < 0) {
    NUMBER = 1;
  }

  int y, i, j, dy = 0;
  int ARTILLERYLENGTH = 54 + 57 * NUMBER;

  if (x < - ARTILLERYLENGTH) {
    return ERR;
  }
  y = LINES / 2 - 12;

  for (i = 0; i <= ARTILLERYHEIGHT; ++i) {
    my_mvaddstr(y + i, x, locomotive[(ARTILLERYLENGTH + x) % ARTILLERYPATTERNS][i]);
    for (j = 1; j <= NUMBER; ++j) {
      my_mvaddstr(y + i + dy*j, x + 55*j, wagons[i]);
    }
  }

  if (ACCIDENT == 1) {
    add_man(y + 9, x + 14);
    for (j = 1; j <= NUMBER; ++j) {
      add_man(y + dy*j + 6, x + 2 + 55*j);
    }
  }

  disco_colors(x);
  return OK;
}


int add_C51(int x)
{
  static char *c51[C51PATTERNS][C51HEIGHT + 1] =
  {
    {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7, C51WH11, C51WH12, C51WH13, C51WH14, C51DEL},
    {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7, C51WH21, C51WH22, C51WH23, C51WH24, C51DEL},
    {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7, C51WH31, C51WH32, C51WH33, C51WH34, C51DEL},
    {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7, C51WH41, C51WH42, C51WH43, C51WH44, C51DEL},
    {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7, C51WH51, C51WH52, C51WH53, C51WH54, C51DEL},
    {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7, C51WH61, C51WH62, C51WH63, C51WH64, C51DEL}
  };
  static char *coal[C51HEIGHT + 1] =
  {
    COALDEL, COAL01, COAL02, COAL03, COAL04, COAL05, COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL
  };

  if (NUMBER < 0) {
    NUMBER = 1;
  }

  int y, i, j, dy = 0;
  int C51LENGTH = 58 + 29*NUMBER;

  if (x < - C51LENGTH) {
    return ERR;
  }
  y = LINES / 2 - 5;

  if (FLY == 1) {
    y = (x / 7) + LINES - (COLS / 7) - C51HEIGHT;
    dy = 1;
  }
  for (i = 0; i <= C51HEIGHT; ++i) {
    my_mvaddstr(y + i, x, c51[(C51LENGTH + x) % C51PATTERNS][i]);
    for (j = 1; j <= NUMBER; j++) {
      my_mvaddstr(y + i + dy*j, x + 26 + 29*j, coal[i]);
    }
  }
  if (ACCIDENT == 1) {
    add_man(y + 3, x + 45);
    add_man(y + 3, x + 49);
  }
  add_smoke(y - 1, x + C51FUNNEL);
  disco_colors(x + C51FUNNEL);
  return OK;
}


void add_man(int y, int x)
{
  static char *man[2][2] = {{"", "(O)"}, {"Help!", "\\O/"}};
  int i;

  for (i = 0; i < 2; ++i) {
    my_mvaddstr(y + i, x, man[(42 + 21*NUMBER + x) / 12 % 2][i]);
  }
}


void add_smoke(int y, int x)
#define SMOKEPTNS        16
{
  static struct smokes {
    int y, x;
    int ptrn, kind;
  } S[1000];
  static int sum = 0;
  static char *Smoke[2][SMOKEPTNS] =
  {
    { "(   )", "(    )", "(    )", "(   )", "(  )",
      "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
      "O"    , "O"     , "O"     , "O"    , "O"   ,
      " "
    },
    { "(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
      "(@@)" , "(@)"   , "(@)"   , "@@"   , "@@"  ,
      "@"    , "@"     , "@"     , "@"    , "@"   ,
      " "
    }
  };
  static char *Eraser[SMOKEPTNS] =
  {
    "     ", "      ", "      ", "     ", "    ",
    "    " , "   "   , "   "   , "  "   , "  "  ,
    " "    , " "     , " "     , " "    , " "   ,
    " "
  };
  static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3};
  int i;

  if (x % 4 == 0) {
    for (i = 0; i < sum; ++i) {
      my_mvaddstr(S[i].y, S[i].x, Eraser[S[i].ptrn]);
      S[i].y    -= dy[S[i].ptrn];
      S[i].x    += dx[S[i].ptrn];
      S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
      my_mvaddstr(S[i].y, S[i].x, Smoke[S[i].kind][S[i].ptrn]);
    }
    my_mvaddstr(y, x, Smoke[sum % 2][0]);
    S[sum].y = y;
    S[sum].x = x;

    S[sum].ptrn = 0;
    S[sum].kind = sum % 2;

    sum ++;
  }
}
