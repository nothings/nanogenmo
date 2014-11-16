#define STB_DEFINE
#include "stb.h" // http://github.com/nothings/stb


//  bass: G2..E4 (F2..F4)
//  soprano: D4..B5 (C4..C6)
//  mezzo-soprano: B3..G5 (A3..A5)



//    Basketball Game: The Opera

/*
    Libretto: sports announcers announcing a basketball game

    Couldn't find a thorough transcript on line, so will have to generate one.

    Strategy:

    * 1.  Take a real game play-by-play (PBP)
    * 2   Replace all player names since we're turning this into a fictional
            game and don't want to falsely attribute actions to real players
    * 3.  Parse the real PBP into a sequence of high-level events
      4.  Generate a plausible sequence of low-events leading to the high-level
      5.  Generate commentary for this in similar way to how a basketball
            videogame would, except we don't have to detect what's happening


*/

char *replace(char *buffer, char *src, char *repl)
{
   char *s;
   if (!strstr(buffer,src))
      return buffer;
   s = stb_dupreplace(buffer, src, repl);
   free(buffer);
   return s;
}

// cleveland = positive players, knocks = negative players
void remove_player_names(char **pbp, int n)
{
   int i;
   for (i=0; i < n; ++i) {
      char *str = pbp[i];
      str = strchr(strchr(str, '\t')+1, '\t')+1;
      str = strdup(str);
      str = replace(str, "\t", " ");

      str = replace(str, "Gibson"    , "#1");
      str = replace(str, "James"     , "#2");
      str = replace(str, "Hughes"    , "#3");
      str = replace(str, "Ilgauskas" , "#4");
      str = replace(str, "Gooden"    , "#5");

      str = replace(str, "Curry"     , "#-1");
      str = replace(str, "Marbury"   , "#-2");
      str = replace(str, "Crawford"  , "#-3");
      str = replace(str, "Randolph"  , "#-4");
      str = replace(str, "Richardson", "#-5");

      str = replace(str, "Dw. Jones" , "#6");
      str = replace(str, "Lee"       , "#-6");
      str = replace(str, "D. Jones"  , "#7");
      str = replace(str, "Robinson"  , "#-7");
      str = replace(str, "Marshall"  , "#8");
      str = replace(str, "Balkman"   , "#-8");
      str = replace(str, "D. Brown"  , "#9");
      str = replace(str, "Jones"     , "#-9");
      str = replace(str, "Newble"    , "#10");

      pbp[i] = str;
   }
}

#define TEAM_0   0  // CLE
#define TEAM_1   1  // NYK

enum {
   TEAMNAME_place,
   TEAMNAME_nick,
   TEAMNAME_full,

   TEAMNAME__count,
};

enum
{
   PLAYERNAME_full,
   PLAYERNAME_short,
};

char teamname[2][TEAMNAME__count][64];
char playername[25][2][64];

char *place[15] =
{
   "At-lan-ta",   "Kan-sas",    "I-da-ho",   "New Mex-i-co",
   "Maine",   "Ver-mont",   "Vir-gin-ia",   "New Jer-sey",
   "I-o-wa",   "Ne-va-da",   "Co-lum-bus",   "Jack-son-ville",
   "Nash-ville",   "O-ma-ha",   "Se-at-tle",
};

char *nick[22] =
{
   "Wart-hogs",   "Oc-e-lots",   "An-a-con-das",   "Pir-a-nha",   "El-eph-ants",
   "Ter-mites",   "Sloths",   "Fly-ing Fox-es",   "Bar-ra-cu-das",   "Ant-eat-ers",
   "But-ter-flies",   "Dach-shunds",   "Fla-min-gos",   "How-lers",   "Mon-it-ors",
   "El-em-ent-als",   "Bas-il-isks",   "Dra-gons",   "In-cu-bi",   "Quas-its",
   "Sham-blers",   "Arch-ons",
};

char *firstname[30] =
{
   "Ger-ald" , "Bro-der-ick", "Wy-att", "Jeff-ery", "Clark",
   "El-roy"  , "Oc-tav-i-o", "Floyd", "Dar-ren", "An-dre",
   "Mal-lard", "John-nie", "Jes-sie", "Rod-ney",  "Gar-rett",
   "Fran-cis", "Hersh-el", "James", "O-tis", "Scott",
   "Al-fred" , "Ken-dall", "Leif", "Sal", "Cal-vin",
   "Tim", "Gor-don", "El-vin", "Lar-ry", "El-vis",
};

char *lastname[30] =
{
   "Har-ri-son", "Starr", "Mann", "Pal-mer", "Sum-mers",
   "Maur", "Orr", "Evans", "Cale", "Byrne",
   "Hath-er-ley", "Gil-mour", "Torn", "Lan-ois", "Ken-ne-dy",
   "Cos-tel-lo", "Kai-ser", "Han-cock", "Jen-sen", "Gil-ber-to",
   "Walsh", "Zorn", "Mitch-ell", "Hat-field", "An-der-son",
   "Framp-ton", "Glass", "John-son", "Will-iams", "Lar-kin",
};

void generate_name(int t, int p, int n)
{
   sprintf(teamname[t][TEAMNAME_place], "%s", place[p]);
   sprintf(teamname[t][TEAMNAME_nick ], "the %s", nick[n]);
   sprintf(teamname[t][TEAMNAME_full ], "the %s %s", place[p], nick[n]);
}

void create_teams(void)
{
   int i;
   int p1,p2, n1,n2;
   int first[30], last[30];

   p1 = stb_rand() % 15;
   do
      p2 = stb_rand() % 15;
   while (p2 == p1);
   n1 = stb_rand() % 22;
   do
      n2 = stb_rand() % 22;
   while (n2 == n1);
   generate_name(TEAM_0, p1, n1);
   generate_name(TEAM_1, p2, n2);

#if 0
   for (i=0; i < 3; ++i)
      printf("%s ver-sus %s\n", teamname[0][i], teamname[1][i]);
#endif

   for (i=0; i < 30; ++i) {
      first[i] = i;
      last [i] = i;
   }

   stb_shuffle(first, 30, sizeof(first[0]), stb_rand());
   stb_shuffle(last , 30, sizeof(last [0]), stb_rand());
   for (i=0; i < 25; ++i) {
      sprintf(playername[i][PLAYERNAME_short], "%s", lastname[last[i]]);
      sprintf(playername[i][PLAYERNAME_full ], "%s %s", firstname[first[i]], lastname[last[i]]);
   }
}

#define PLAYER_OFFSET 10

enum
{
   S_none,
   S_substitute,
};
int last_sing;
int last_voice;

char *voicename[] =
{
   "Soprano",
   "Bass",
   "Mezzo",
};

int last_player=0;
enum
{
   V1,V2,V3
};
void sing(int voice, char *fmt, ...)
{
   char buffer[1024],*s,*t;
   int was_plural = 0, pcount = 0;
   va_list v;
   va_start(v, fmt);
   t = buffer;
   s = fmt;
   while (*s != 0) {
      if (*s == '$') {
         int n = va_arg(v,int);
         ++s;
         if (*s == '$') {
            strcpy(t, playername[n+PLAYER_OFFSET][PLAYERNAME_full ]);
            ++s;
         } else
            strcpy(t, playername[n+PLAYER_OFFSET][PLAYERNAME_short]);
         t += strlen(t);
         ++pcount;
         last_player = n;
      } else if (*s == '%') {
         int n = va_arg(v, int);
         ++s;
         if (n == last_player && pcount == 0)
            strcpy(t, "he");
         else
            strcpy(t, playername[n+PLAYER_OFFSET][PLAYERNAME_short]);
         t += strlen(t);
         ++pcount;
         last_player = n;
      } else if (*s == '#') {
         int n;
         n = va_arg(v,int);
         ++s;
         if (*s == '#') {
            strcpy(t, teamname[n][TEAMNAME_full]);
            ++s;
            was_plural = 1;
         } else {
            int n = stb_rand() & 1;
            strcpy(t, teamname[n][n]);
            was_plural = (n == 1);
         }
         t += strlen(t);
      } else if (*s == '[') {
         if (was_plural) {
            ++s;
            while (*s != '|')         s++;
            ++s;
            while (*s != ']') *t++ = *s++;
            ++s;
         } else {
            ++s;
            while (*s != '|') *t++ = *s++;
            ++s;
            while (*s != ']')        *s++;
            ++s;
         }
      } else {
         *t++ = *s++;
      }
   }
   va_end(v);
   *t = 0;
   assert(t < buffer+1024);
   printf("%s: %s\n", voicename[voice], buffer);
   last_voice = voice;
   last_sing = S_none;
   if (pcount > 1)
      last_player = 0;
}

int team[2][5];

int slot(int t, int player)
{
   int i;
   for (i=0; i < 5; ++i)
      if (team[t][i] == player)
         return i;
   assert(0);
   return 0;
}

void set_lineup(int t, int v1, int v2, int v3, int v4, int v5)
{
   team[t][0] = v1;
   team[t][1] = v2;
   team[t][2] = v3;
   team[t][3] = v4;
   team[t][4] = v5;
}

int score[2];

typedef struct
{
   int points, assists, rebounds, turnovers, steals, blocks;
   int layup_attempts, layup_made, layup_points;
   int jump_attempts, jump_made, jump_points;
   int ft_attempts, ft_made, ft_points;
   int f3_attempts, f3_made, f3_points;
} player_stats;

int cur_line;
int last_shot_time;
player_stats raw_stats[25];
player_stats *players = raw_stats+12;

enum
{
   SHOT_jump,
   SHOT_3pt,
   SHOT_layup,
};

struct
{
   int possess_team;
   int possess_player;
   float position; // at 1, the ball is under TEAM_0's target basket, at -1 under TEAM_1's
   int current_time;
   int possess_time;
   int possess_action_time;
} game_state;

int team_for_name(char *name)
{
   if (0==strcmp(name, "CLE")) return TEAM_0;
   if (0==strcmp(name, "NYK")) return TEAM_1;
   assert(0);
   return -1;
}

enum
{
   F_none,
   F_turnaround,
   F_turnaround_hook,
   F_hook,
   F_bank,
   F_fadeaway,
   F_reverse,
   F_tip,
   F_dunk,
   F_running,
   F_running_bank,
   F_putback,
   F_putback_dunk,
   F_driving,
   F_driving_finger_roll,
   F_alleyoop,
   F_alleyoop_dunk,
};
                      
enum
{
   FOUL_none,
   FOUL_personal,
   FOUL_shooting,
};

enum
{
   TURNOVER_traveling,
   TURNOVER_badpass,
   TURNOVER_foul,
   TURNOVER_out_of_bounds,
   TURNOVER_palming,
   TURNOVER_3sec,
   TURNOVER_stolen,
};

int history[5000][10];
int h_count[5000];
int done[5000];

// can only ever say this once
int once(int line)
{
   if (done[line])
      return 0;
   done[line] = 1;
   return 0;
}

int random_nonrepeat(int range, int line)
{
   int n, i;
  retry:
   n = stb_rand() % range;
   for (i=1; i < 10; ++i) {
      if (range == i)
         break;
      if (i > (range+1)/2)
         break;
      if (h_count[line] < i)
         break;
      if (h_count[line] >= i && history[line][i-1] == n)
         goto retry;
   }
   if (h_count[line] < 10)
      ++h_count[line];
   for (i=9; i > 0; --i)
      history[line][i] = history[line][i-1];
   history[line][0] = n;
   return n;
}

int random(int range)
{
   return stb_rand() % range;
}

#define random_nonrepeat(range) random_nonrepeat(range, __LINE__)
#define once()                  once(__LINE__)

int this_event_time, last_event_time = 0;
int last_sub_team;

int subs[10][2], subcount;
void enqueue_substitution(int p1, int p2)
{
   //printf("Sub %d with %d\n", p1, p2);
   subs[subcount][0] = p1;
   subs[subcount][1] = p2;
   ++subcount;
}

void now(int voice)
{
   switch (random_nonrepeat(12)) {
      case 0: sing(voice, "And now "); break;
      case 1: sing(voice, "It looks like "); break;
      case 2: sing(voice, "So "); break;
      default:
         break;
   }
}

void substitute(int tm, int prev_player, int new_player)
{
   int voice = random(100)<93 ? V1 : V2;

   if (last_sing == S_substitute) {
      // multiple subs in a row
      if (tm != last_sub_team)
         sing(last_voice, "And for #, ", tm);
      sing(last_voice, "$ is sub-sti-tu-ted by $.", new_player, prev_player);
   } else {
      switch (random_nonrepeat(6)) {
         case 0:             sing(voice, "$ is sub-sti-tu-ted in for $.", new_player, prev_player); break;
         case 1:             sing(voice, "$ is sub-sti-tu-ted by $.", prev_player, new_player); break;
         case 2: now(voice); sing(voice, "$ goes out, sub-sti-tu-ted by $.", prev_player, new_player); break;
         case 3: now(voice); sing(voice, "# put[s|] $ in for $.", tm, new_player, prev_player); break;
         case 4:             sing(voice, "It looks like # [is|are] go-ing to re-place $ with $.", tm, new_player, prev_player); break;
         case 5:             sing(voice, "$ subbed out, re-placed by $.", prev_player, new_player); break;
         case 6:             sing(voice, "For #, $ goes in for $.", tm, new_player, prev_player); break;
            break;
         default:
            assert(0);
      }
   }
   last_sing = S_substitute;
   last_sub_team = tm;
}

void process_substitution(void)
{
   int j,i,k;
   for (j=0; j < subcount; ++j) {
      int p1 = subs[j][0];
      int p2 = subs[j][1];
      int t = p1 < 0 ? TEAM_1 : TEAM_0;
      assert(p1*p2 > 0);
      for (i=0; i < 5; ++i) {
         if (team[t][i] == p1) {
            team[t][i] = p2;
            break;
         }
      }
      assert(i != 5);

      // now veryify no repeats
      for (i=0; i < 5; ++i)
         for (k=0; k < 5; ++k)
            if (i != k)
               assert(team[t][i] != team[t][k]);

      substitute(t, p1, p2);
   }
   subcount = 0;
}

int who_was_fouled;
int foul, foul_player, foul_time;

void tpossess(int team)
{
   if (game_state.possess_team != team) {
      game_state.possess_team = team;
      game_state.possess_player = 0;
      game_state.possess_time = this_event_time;
      game_state.possess_action_time = this_event_time;
      sing(V1, "# [has|have] the ball.", team);
   }
}

int is_closer_than(float pos)
{
   if (game_state.possess_team == TEAM_0) {
      return game_state.position >  pos;
   } else {
      return game_state.position < -pos;
   }
}

int take_possession(int tm)
{
   if (game_state.possess_player == 0) {
      game_state.possess_player = team[tm][0];
      if (!is_closer_than(-0.5)) {
         sing(V1, "$ brings the ball up the court.", game_state.possess_player);
         return 1;
      } else {
         sing(V1, "$ gets the ball.", game_state.possess_player);
         return 0;
      }
   }
   return !is_closer_than(-0.5);
}

void pass_to(int tm, int player)
{
   if (game_state.possess_player == player)
      return;
   sing(V1, "$ pass-es to $.", game_state.possess_player, player);
   game_state.possess_player = player;
}

void run_play(int ball_holder, float position, int end_of_play_time)
{
   int tm = game_state.possess_team;
   // run a play so the ball ends up in ball_holder's hands at position,
   int delta_time = end_of_play_time - game_state.current_time;
   if (delta_time < 5 + random(3)) {
      take_possession(tm);
      if (random(100) < 50) {
         pass_to(tm, team[tm][random(5)]);
      }
      pass_to(tm, ball_holder);
   } else {
      if (take_possession(tm))
         delta_time -= 2 + random(1);
      while (delta_time >= 0) {
         pass_to(tm, team[tm][random(5)]);
         delta_time -= 2 + random(1) + random(3)*(random(3)==0);
      }
      pass_to(tm, ball_holder);
   }
   game_state.position = position;
   game_state.current_time = end_of_play_time;
}

float position_for_foul(void)
{
   return 0; // @TODO
}

void process_foul(int first_free_throw)
{
   if (!foul)
      return;

   // run a play up to the time the foul occurs
   if (game_state.possess_action_time < foul_time) {
      run_play(who_was_fouled, position_for_foul(), foul_time);
   }

   // if nobody has possession, give it to the point guard   
   if (!game_state.possess_player)
      game_state.possess_player = team[foul_player > 0][0];

   // if nobody was fouled, make it whoever had possession last,
   if (who_was_fouled == 0)
      who_was_fouled = game_state.possess_player;

   assert(who_was_fouled * foul_player < 0);

   if (first_free_throw && this_event_time != last_shot_time) {
      sing(V1, "% tries to take a shot but is fouled by $.", who_was_fouled, foul_player);
   }

   if (who_was_fouled == 0)
      sing(V1, "$ fouled nobody.", foul_player);
   else
      sing(V1, "$ fouled by $.", who_was_fouled, foul_player);
   who_was_fouled = 0;
   foul = 0;
   game_state.current_time = this_event_time;
}

int random_player(int tm, int player)
{
   int s = slot(tm, player);
   s += 1+random(4);
   s %= 5;
   return team[tm][s];
}

int defender(int player)
{
   int tm = player < 0 ? TEAM_1 : TEAM_0;
   int s = slot(tm, player);
   return team[!tm][s];
}


void took_shot(int type, int player, int assist, int made, int blocker, int flavor)
{
   int tm = (player < 0) ? TEAM_1 : TEAM_0;
   process_foul(0);
   process_substitution();

   slot(tm, player);

   if (this_event_time < game_state.possess_time+5 && game_state.possess_action_time == game_state.possess_time) {
      //printf("  Line: %d\n", cur_line);
      //sing(V1, "!!! FAST BREAK !!!");
   }
   tpossess(tm);

   if (type == SHOT_layup) {
      if (assist) {

      } else {
         run_play(player, tm < 0 ? -1.0f : 1.0f, this_event_time-3);
         if (random(100) < 50) {
            // gets the pass in the post
         } else {
            // gets the pass outside and works it in
            if (random(100) < 20) {
               switch (random_nonrepeat(3)) {
                  case 0: sing(V1, "Look-ing for some-one to pass to."); break;
                  case 1: if (once()) sing(V2, "The de-fense is doing a great job."); break;
                  case 2: sing(V1, "$ can't find any-one.", player);
               }
            }
            // randomly drive to the layup
            switch (random_nonrepeat(6)) {
               case 0: sing(V1, "% pump fakes and slips by his de-fen-der.", player); break;
               case 1: sing(V1, "A nice dou-ble-move from $ to get by $.", player, defender(player)); break;
               case 2: sing(V1, "$ gets a pick from $.", player, team[tm][random_player(tm,player)]); break;
               case 3: sing(V1, "Gets by his defender.", player, defender(player)); break;
               case 4: sing(V1, "% drives to the basket.", player);
               case 5: sing(V1, "Drives to the basket.", player);
               case 6: sing(V1, "% gets to the basket.", player);
                  ;
            }

            if (made) {
               switch (flavor) {
                  case F_none:
                     break;
                  case F_reverse:             assert(0);      
                  case F_tip:
                     break;
                  case F_dunk:
                     break;
                  case F_running:             assert(0);      
                  case F_running_bank:
                     break;
                  case F_putback:
                     break;
                  case F_putback_dunk:
                     break;
                  case F_driving:
                     break;
                  case F_driving_finger_roll:
                     break;
                  case F_alleyoop:            assert(0);
                  case F_alleyoop_dunk:       assert(0);           
                  default:
                     assert(0);
               }
            } else {
               switch (flavor) {
                  case F_none:
                     break;
                  case F_reverse:             assert(0);      
                  case F_tip:
                     break;
                  case F_dunk:
                     break;
                  case F_running:             assert(0);      
                  case F_running_bank:        assert(0);          
                  case F_putback:
                     break;
                  case F_putback_dunk:        assert(0);          
                  case F_driving:             assert(0);     
                  case F_driving_finger_roll: assert(0);                 
                  case F_alleyoop:            assert(0);
                  case F_alleyoop_dunk:       assert(0);           
                  default:
                     assert(0);
               }
            }
         }
      }
   } else {
      if (assist) {
         run_play(assist, tm < 0 ? -0.8f : 0.8f, this_event_time - random(4));
         pass_to(tm, player);
      } else {
         if (type == SHOT_3pt)
            run_play(player, tm < 0 ? -0.7f : 0.7f, this_event_time);
         else
            run_play(player, tm < 0 ? -0.85f : 0.85f, this_event_time);
      }
      // at this point the player has the ball and can take a jump shot
      if (type == SHOT_3pt) {
         switch (random_nonrepeat(7)) {
            case 0: sing(V1, "! For three...  "); break;
            case 1: sing(V1, "! % shoots for three...  ", player); break;
            case 2: sing(V1, "! Fakes his de-fen-der and goes for three... "); break;
            case 3: sing(V1, "! % tries for three... ", player); break;
            case 4: sing(V1, "! % look-ing for three points... ", player); break;
            case 5: sing(V1, "! Try-ing for three... "); break;
            case 6: sing(V1, "! Tries a three point shot... "); break;
         }
      } else {
         switch (flavor) {
            case F_none:
               break;
            case F_turnaround:
               break;
            case F_fadeaway:
            case F_hook:
            case F_turnaround_hook:
               break;
            case F_bank:
               break;
            case F_reverse:             assert(0);      
            case F_tip:                 assert(0);  
            case F_dunk:                assert(0);   
            case F_running:             assert(0);      
            case F_running_bank:        assert(0);          
            case F_putback:             assert(0);     
            case F_putback_dunk:        assert(0);          
            case F_driving:             assert(0);     
            case F_driving_finger_roll: assert(0);                 
            case F_alleyoop:
            case F_alleyoop_dunk:
               assert(0);           
         }
      }
   }

   if (made)
      sing(V1, "% shoots and makes it.", player);
   else
      sing(V1, "% shoots and misses.", player);
   game_state.current_time = this_event_time;
   game_state.possess_action_time = this_event_time;
   game_state.position = (player < 0 ? -1.0f : 1.0f);
   if (made) {
      game_state.possess_team = !tm;
      game_state.possess_player = 0;
   }
   last_shot_time = this_event_time;
   assert(!made || blocker == 0);
   //printf("Player %d took '%c' assisted by %d: %s\n", player, "j3Lda"[type], assist, made ? "made" : "missed");
}

void free_shot(int player, int shotnum, int total_shots, int made)
{
   who_was_fouled = player;
   process_foul(shotnum == 1);
   who_was_fouled = 0;
   process_substitution();
   if (made)
      sing(V1, "$ makes a free throw.", player, shotnum, total_shots);
   else
      sing(V1, "$ misses a free throw.", player, shotnum, total_shots);

   game_state.position = (player < 0 ? -1.0f : 1.0f);
   game_state.current_time = this_event_time;
   //printf("Player %d took %d of %d free throws: %s\n", player, shotnum, total_shots, made ? "made" : "missed");

   if (shotnum == total_shots) {
      if (made) {
         game_state.possess_team = player < 0 ? TEAM_0 : TEAM_1;
         game_state.possess_player = 0;
      } else {
         game_state.possess_team = -1;
         game_state.possess_player = 0;
         // should be an explicit rebound
      }
   }
}

void offensive_foul(int player)
{
   process_foul(0);
   process_substitution();
   sing(V1, "$ makes an offensive foul.", player);
   foul_time = this_event_time;
   tpossess(player < 0 ? TEAM_1 : TEAM_0);
   game_state.current_time = this_event_time;
}

void rebound(int team, int player)
{
   process_foul(0);
   process_substitution();
   if (player)
      sing(V1, "$ with the rebound.", player);
   else
      sing(V1, "# [takes|take] possession.", team);
   game_state.position = (game_state.possess_team == TEAM_0 ? 1.0f : -1.0f);
   tpossess(team);
   game_state.current_time = this_event_time;
   game_state.possess_action_time = this_event_time;
   game_state.possess_player = player;
}

void turnover(int type, int team, int player, int stealer)
{
   process_foul(0);
   process_substitution();
   if (stealer)
      sing(V1, "$ steals the ball from $.", stealer, player);
   else
      sing(V1, "$ loses the ball.", player);
   tpossess(!team);
   game_state.current_time = this_event_time;
   game_state.possess_player = stealer;
}

static void process_event(char *text)
{
   int score_to_be_1=0, score_to_be_2=0;
   int h,m,s, tim;
   sscanf(text, "%d:%d:%d", &h, &m, &s);

   tim = 60*60*h + 60*m + s;
   this_event_time = (48*60) - tim;

   text += 9;
   if (text[0] == '[') {
      char buffer[512];
      int assist=0, t=-1, blocker=0;
      int v[9] = { 0 }, n;
      char teamname[4] = { 0 }, *as;
      if (3 == sscanf(text, "[%3c %d-%d]", teamname, &score_to_be_1, &score_to_be_2)) {
         t = team_for_name(teamname);
      } else if (1 == sscanf(text, "[%3c]", teamname)) {
         t = team_for_name(teamname);
      } else
         assert(0);
      text = strchr(text, ']')+2;
      strcpy(buffer, text);
      text = buffer;
      as = strstr(text, "Assist:");
      if (as) {
         n = sscanf(as, "Assist: #%d (%d AST)", &assist, v+7);
         assert(n == 2);
         strcpy(as, " 737"); // append a 737 to the string to force sscanf to match the end of strings
      } else {
         as = strstr(text, "Block:");
         if (as) {
            n = sscanf(as, "Block: #%d (%d BLK)", &blocker, v+6);
            assert(n == 2);
            strcpy(as, " 737");
         } else {
            strcat(text, " 737");
         }
      }

      // regular shots
      if (3 == sscanf(text, "#%d 3pt Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_3pt, v[0], assist, 1, blocker, F_none);
      } else if (3 == sscanf(text, "#%d Jump Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_none);
      } else if (3 == sscanf(text, "#%d Turnaround Jump Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_turnaround);
      } else if (3 == sscanf(text, "#%d Turnaround Hook Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_turnaround_hook);
      } else if (3 == sscanf(text, "#%d Jump Bank Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_bank);
      } else if (3 == sscanf(text, "#%d Jump Hook Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_hook);
      } else if (3 == sscanf(text, "#%d Fade Away Jumper Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_fadeaway);
      } else if (3 == sscanf(text, "#%d Reverse Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_reverse);
      } else if (3 == sscanf(text, "#%d Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_none);
      } else if (3 == sscanf(text, "#%d Tip Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_tip);
      } else if (3 == sscanf(text, "#%d Dunk Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_dunk);
      } else if (3 == sscanf(text, "#%d Running Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_running);
      } else if (3 == sscanf(text, "#%d Running Bank shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_running_bank);
      } else if (3 == sscanf(text, "#%d Putback Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_putback);
      } else if (3 == sscanf(text, "#%d Putback Dunk Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_putback_dunk);
      } else if (3 == sscanf(text, "#%d Driving Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_driving);
      } else if (3 == sscanf(text, "#%d Driving Finger Roll Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_driving_finger_roll);
      } else if (3 == sscanf(text, "#%d Alley Oop Layup Shot: made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_alleyoop);
      } else if (3 == sscanf(text, "#%d Alley Oop Layup shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_alleyoop);
      } else if (3 == sscanf(text, "#%d Alley Oop Dunk Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_alleyoop_dunk);

      } else if (2 == sscanf(text, "#%d 3pt Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_3pt, v[0], assist, 0, blocker, F_none);
      } else if (2 == sscanf(text, "#%d Jump Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 0, blocker, F_none);
      } else if (2 == sscanf(text, "#%d Turnaround Jump Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 0, blocker, F_turnaround);
      } else if (2 == sscanf(text, "#%d Jump Hook Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 0, blocker, F_hook);
      } else if (2 == sscanf(text, "#%d Jump Bank Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 0, blocker, F_bank);
      } else if (2 == sscanf(text, "#%d Layup Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_none);
      } else if (2 == sscanf(text, "#%d Putback Layup Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_putback);
      } else if (2 == sscanf(text, "#%d Tip Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_tip);
      } else if (2 == sscanf(text, "#%d Dunk Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_dunk);

      // these never happen in this game, so I don't bother writing dialog for them
#if 0
      } else if (2 == sscanf(text, "#%d Turnaround Hook Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 0, blocker, F_turnaround_hook);
      } else if (2 == sscanf(text, "#%d Fade Away Jumper Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 0, blocker, F_fadeaway);
      } else if (2 == sscanf(text, "#%d Reverse Layup Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_reverse);
      } else if (2 == sscanf(text, "#%d Driving Layup Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_driving);
      } else if (2 == sscanf(text, "#%d Running Layup Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_running);
      } else if (2 == sscanf(text, "#%d Alley Oop Layup Shot: Missed %d", v+0, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 0, blocker, F_alleyoop);
#endif

      // free throws
      } else if (4 == sscanf(text, "#%d Free Throw %d of %d missed %d", v+0, v+1, v+2, v+8)) {
         free_shot(v[0], v[1], v[2], 0);
      } else if (5 == sscanf(text, "#%d Free Throw %d of %d (%d PTS) %d", v+0, v+1, v+2, v+3, v+8)) {
         free_shot(v[0], v[1], v[2], 1);

      // foul
      } else if (3 == sscanf(text, "#%d Foul:Personal (%d PF) %d", v+0, v+1, v+8)) {
         foul = FOUL_personal;
         foul_player = v[0];
         foul_time = this_event_time;
      } else if (3 == sscanf(text, "#%d Foul:Shooting (%d PF) %d", v+0, v+1, v+8)) {
         foul = FOUL_shooting;
         foul_player = v[0];
         foul_time = this_event_time;
      } else if (3 == sscanf(text, "#%d Foul:Loose Ball (%d PF) %d", v+0, v+1, v+8)) {
         offensive_foul(v[0]); // @TODO
      } else if (3 == sscanf(text, "#%d Foul:Offensive (%d PF) %d", v+0, v+1, v+8)) {
         offensive_foul(v[0]);
      } else if (2 == sscanf(text, "#%d Violation:Defensive Goaltending  %d", v+0,v+8)) {

      // rebounds
      } else if (1 == sscanf(text, "Team Rebound %d", v+8)) {
         rebound(t, 0);
      } else if (4 == sscanf(text, "#%d Rebound (Off:%d Def:%d) %d", v+0,v+1,v+2, v+8)) {
         rebound(t, v[0]);

      // turnovers
      } else if (3 == sscanf(text, "#%d Turnover:Traveling (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_traveling, t, v[0], 0);
      } else if (3 == sscanf(text, "#%d Turnover:Foul (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_foul     , t, v[0], 0);
      } else if (5 == sscanf(text, "#%d Turnover:Bad Pass (%d TO) Steal:#%d (%d ST) %d", v+0,v+1,v+2,v+3,v+8)) {
         turnover(TURNOVER_badpass  , t, v[0], v[2]);
      } else if (3 == sscanf(text, "#%d Turnover:Bad Pass (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_badpass  , t, v[0], 0);
      } else if (3 == sscanf(text, "#%d Turnover:Out of Bounds Lost Ball Turnover (%d TO) %d", v+0,v+1, v+8)) {
         turnover(TURNOVER_out_of_bounds, t, v[0], 0);
      } else if (3 == sscanf(text, "#%d Turnover:Palming Turnover (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_palming, t, v[0], 0);
      } else if (3 == sscanf(text, "#%d Turnover:3 Second Violation (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_3sec   , t, v[0], 0);
      } else if (5 == sscanf(text, "#%d Turnover:Lost Ball (%d TO) Steal:#%d (%d ST) %d", v+0,v+1,v+2,v+3,v+8)) {
         turnover(TURNOVER_stolen , t, v[0], v[2]);

      // misc
      } else if (3 == sscanf(text, "#%d Substitution replaced by #%d %d", v+0, v+1, v+8)) {
         enqueue_substitution(v[0],v[1]);
      } else if (1 == sscanf(text, "Team Timeout:Regular %d", v+8)) {
         process_substitution();
         // do nothing
      } else if (1 == sscanf(text, "Team Timeout:Short %d", v+8)) {
         process_substitution();
         // do nothing
      } else {
         printf("!!!   missing handler for    !!!!!!!!\n      %s\n", text);
         v[8] = 737;
      }
      assert(v[8] == 737);
   } else {
      int v[5];
      process_substitution();
      if (0 == strcmp(text, "Start of 1st Quarter")) {
         set_lineup(TEAM_0,  1,  2,  3,  4,  5);
         set_lineup(TEAM_1, -1, -2, -3, -4, -5);
      } else if (0 == strcmp(text, "End of 1st Quarter")) {
         sing(V1, "That's the end of the first quar-ter.");
         set_lineup(TEAM_0,  1,  7,  9,  4,  8); // 7 unknown
         set_lineup(TEAM_1, -9, -2, -6, -4, -7);
      } else if (0 == strcmp(text, " Timeout: Official")) {
         static int which;
         if (which == 0) {
            set_lineup(TEAM_0, 1, 2, 3, 4, 5);
            set_lineup(TEAM_1, -6, -2, -7, -4, -5);
            // -6 -2 -5 -7 -4
         } else {
            set_lineup(TEAM_0, 7, 2, 9, 6, 8);
            set_lineup(TEAM_1, -6, -2, -7, -4, -9);
         }
         ++which;
      } else if (0 == strcmp(text, "End of 2nd Quarter")) {
         sing(V1, "That's the end of the first half.");
         set_lineup(TEAM_0, 1, 2, 3, 4, 5);
         set_lineup(TEAM_1, -1, -2, -3, -4, -5); 
      } else if (0 == strcmp(text, "End of 3rd Quarter")) {
         sing(V1, "That's the end of the third quar-ter.");
         set_lineup(TEAM_0, 7, 2, 9, 6, 8);
         set_lineup(TEAM_1, -6, -7, -3, -4, -9);
      } else if (0 == strcmp(text, "End of 4th Quarter")) {
         sing(V1, "That's the end of the game.");
         // no lineup changes
      } else if (2 == sscanf(text, " Jump Ball #%d vs #%d", v+0,v+1)) {
         // announce the lineup
         sing(V1, "It's $ ver-sus $ to start the game.", v[0], v[1]);
         sing(V1, "The tip off goes to #", TEAM_0); // hardcoded to this specific game instead of detecting from transcript
         game_state.possess_team   = TEAM_0;
         game_state.possess_player = 0;
         game_state.position       = -0.5;
      } else {
         assert(0);
      }
   }
   last_event_time = this_event_time;
}


int main(int argc, char **argv)
{
   char *filename = "knicks_at_cavs_20071102.txt";
   int n,i;
   char **pbp = stb_stringfile(filename, &n);
   
   remove_player_names(pbp, n);

   stb_srand(time(NULL));
   create_teams();

   game_state.possess_team = -1;
   game_state.current_time = 0;
   for (i=0; i < n; ++i) {
      cur_line = i+1;
      process_event(pbp[i]);
   }
   return 0;
}
