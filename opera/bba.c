#define STB_DEFINE
#include "stb.h" // http://github.com/nothings/stb

#pragma warning(disable:4244)

// TODO-ish: lyrics
//   don't do two "with"s in a row for rebound & possession
//   don't do two/three "balls" in a row for rebound, possession, and up the court
//   substitute "it" for "ball" in some cases
//   if we tie up the score in the first free throw, should definitely mention
//      the second free throw goes ahead
//   it shouldn't be the point guards doing the tip-off!
//   V1 announces time left in quarter while player 'brings ball up court' if
//      close to end of quarter

// TODO: music
//   reblock measures

//
//  have a whistle blow at each foul
//  track shot clock and have shot clock buzzer?


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

      4&5 are done jointly because I don't bother building an explicit
      data structure representing the events
*/


/*
  scoring:
   points for the night after scoring
   layup/field goal/3pt/total points for the night after such events
   layup/field goal/3pt/total points for the team after such events
   layup/field goal/3pt/total points compare halfs for player after such events
   layup/field goal/3pt/total points compare halfs for team after such events
   layup/field goal/3pt/total points compare halfs for teams after such events
   layup/field goal/3pt/total percentages for the night after such events
   layup/field goal/3pt/total percentages for the season after such events
   layup/field goal/3pt/total results for the previous night after such events
   1st point
   1st layup
   1st field goal
   1st 3 pointer
   1st point of the quarter
   assists MUST be reported

  free throws:
   free throw percentage for the night
   free throw percentage for the season
   free throws for the night
   free throws for the halves
   team free throw results for both teams
   fraction of points due to free throws

  rebounds:
   season rebounds
   rebounds for the night
   offensive vs defensive
   1st offensive
   1st defensive
   rebounds for each team
   rebounds for each half

  steal:
   steals for the night
   team steals
   steals for the season (avg)
   steals for each team
   steals for each half

  turnover:
   turnovers for the night
   turnovers for the team
   turnovers for each team
   turnovers for each half
   turnovers of this type

  process_foul:

*/

typedef struct
{
   int voice;
   char **syllables;
   char *cur_line;
} vocal_line;

vocal_line *vocals;

stb_sdict *syllable_map;
#if 0
typedef struct
{
   int note;
} pitch;
#endif

static char *lilynote[12] =
{
   "c",
   "cis",
   "d",
   "dis",
   "e",
   "f",

   "fis",
   "g",
   "gis",
   "a",
   "ais",
   "b",
};

FILE *f;

#define WHOLE_NOTE    96

void lily_print_duration(int duration)
{
   switch (duration) {
      case WHOLE_NOTE*1  : fprintf(f, "1" ); break;
      case WHOLE_NOTE*1/2: fprintf(f, "2 "); break;
      case WHOLE_NOTE*1/4: fprintf(f, "4 "); break;
      case WHOLE_NOTE*3/4: fprintf(f, "2. "); break;
      case WHOLE_NOTE*1/8: fprintf(f, "8 "); break;
      case WHOLE_NOTE*3/8: fprintf(f, "4. "); break;
      case WHOLE_NOTE*3/16: fprintf(f, "8. "); break;
      case WHOLE_NOTE*1/16: fprintf(f, "16 "); break;
      default: assert(0);
   }
}

void lily_print_note(int note)
{
   if (note > 0) {
      int octave = (note - 60 + 5*12) / 12;
      int pc;
      assert(note >= 12);
      pc = note-60+5*12 - 12*octave;
      assert(pc >= 0 && pc < 12);
      fprintf(f, "%s", lilynote[pc]);
      while (octave > 4) {
         fprintf(f, "'");
         --octave;
      }
      while (octave < 4) {
         fprintf(f, ",");
         ++octave;
      }
   } else {
      fprintf(f, "r");
   }
}

int aligning_time(int current_time)
{
   if (current_time % (WHOLE_NOTE/16))
      return WHOLE_NOTE/32;
   if (current_time % (WHOLE_NOTE/8))
      return WHOLE_NOTE/16;
   if (current_time % (WHOLE_NOTE/4))
      return WHOLE_NOTE/8;
   if (current_time % (WHOLE_NOTE/2))
      return WHOLE_NOTE/4;
   if (current_time % WHOLE_NOTE)
      return WHOLE_NOTE/2;

   return WHOLE_NOTE;
}

void lily_print_timed_chord(int current_time, int duration, int *notes, int num_notes)
{

}



#define OUTPUT_START   8
#define OUTPUT_END     40



#define SCALE_PITCHES   7

typedef struct
{
   int pc[SCALE_PITCHES];
} key;

typedef struct
{
   int scalenote[3];
} chord;

#define MAX_NOTES_PER_VOICE  4

typedef struct
{
   int scalenote;
   int duration;
} note;

typedef struct
{
   int midinote[4];
   int duration;
} voicenote;

typedef struct
{
   chord c;
   int   duration;
} timedchord;

int random(int range)
{
   return stb_rand() % range;
}

int random_range(int low, int high)
{
   return random(high-low+1) + low;
}

int do_print;

note *generate_phrase(int num_notes)
{
   int i;
   int dir;
   note *list=NULL;
   int p = stb_rand() % (SCALE_PITCHES*2);
   int high_cap, low_cap;
   dir = (stb_rand() & 128) ? 1 : -1;
   high_cap = SCALE_PITCHES * 2 - random(3);
   low_cap  = random(3);

   for (i=0; i < num_notes; ++i) {
      note n;
      int no_update=0;

      if (i!=0 && random(100) < 5) {
         // random momentary note out of range
         int temp = p - dir*random_range(3,7);
         if (temp < 0 || temp > SCALE_PITCHES*2) {
            temp = p + (p-temp);
            if (temp < 0 || temp > SCALE_PITCHES*2)
               temp = p;
         }
         n.scalenote = temp;
         no_update = 1;
      } else
         n.scalenote = p;

      // @TODO react to no_update

      if (random(100) < 20) {
         dir = -dir;
      }
      
      {
         if (random(100) < 30)
            p += 0;
         else if (random(100) < 50)
            p += dir;
         else if (random(100) < 50)
            p += dir*2;
         else if (random(100) < 50)
            p += dir*4;
         else if (random(100) < 50)
            p += dir*5;
         else
            p += dir*3;
      }
      assert(dir != 0);

      if (p > high_cap) {
         dir = -1;
         p = high_cap - random(2);
      }
      if (p < low_cap) {
         dir = 1;
         p = low_cap + random(2);
      }

      high_cap += random_range(-1,1);
      if (high_cap < SCALE_PITCHES*2-3 || high_cap > SCALE_PITCHES*2)
         high_cap = SCALE_PITCHES*2 - random(3);
      low_cap  += random_range(-1,1);
      if (low_cap < 0 || low_cap > 3)
         low_cap = random(3);

      if (random(100) < 5)
         n.duration = WHOLE_NOTE / 2;
      else if (random(100) < 10)
         n.duration = WHOLE_NOTE*3/8;
      else if (random(100) < 80)
         n.duration = WHOLE_NOTE / 8;
      else
         n.duration = WHOLE_NOTE / 4;
      if (i == num_notes-1)
         n.duration = WHOLE_NOTE*3/8;

      stb_arr_push(list, n);
   }
   return list;
}

int scale[15] = {
#if 1
   0,2,4, 5,7,9,11,
   12,14,16, 17,19,21,23,
   24
#else
   0,2,3, 6,7,9,10,
   12,14,15, 18,19,21,22,
   24
#endif
};

int midi_note_for_scale(int p, int range_low)
{
   int o = p / SCALE_PITCHES;
   int n = p % SCALE_PITCHES;
   int pitch = range_low + scale[n] + 12*o;
   return pitch;
}

int find_nearest_note_to_chord(int p, chord *chord)
{
   int i, best_dist = SCALE_PITCHES;
   int result;
   for (i=0; i < 3; ++i) {
      int dist = p - chord->scalenote[i];
      dist = dist % SCALE_PITCHES;
      if (dist >  SCALE_PITCHES/2) dist -= SCALE_PITCHES;
      if (dist < -SCALE_PITCHES/2) dist += SCALE_PITCHES;
      assert(dist >= -SCALE_PITCHES/2-1 && dist <= SCALE_PITCHES/2+1);
      if (p - dist < 0) dist += SCALE_PITCHES;
      if (p + dist > 2*SCALE_PITCHES) dist -= SCALE_PITCHES;
      if (abs(dist) < abs(best_dist))
         best_dist = dist;
   }
   result = p - best_dist;

   return result;
}

int phrase_duration(note *phrase, int len)
{
   int duration=0;
   int i;
   for (i=0; i < len; ++i)
      duration += phrase[i].duration;
   return duration;
}

voicenote *add_rests_at_end(voicenote *phrase, int duration)
{
   int i;
   int rest_length = WHOLE_NOTE;
   voicenote *rests = 0;
   int start_dur = duration;
   while (duration) {
      while (duration >= rest_length) {
         voicenote n = { {0}, rest_length };
         assert(n.duration > 0 && n.duration <= WHOLE_NOTE);
         stb_arr_push(rests, n);
         duration -= rest_length;
      }
      rest_length /= 2;
   }
   #if 1
   for (i=stb_arr_len(rests)-1; i >= 0; --i)
      stb_arr_push(phrase, rests[i]);
   stb_arr_free(rests);
   #endif

   for (i=0; i < stb_arr_len(phrase); ++i)
      assert(phrase[i].duration > 0 && phrase[i].duration <= WHOLE_NOTE);

   return phrase;      
}

voicenote *adapt_melody(chord *start_chord, chord *end_chord, note *phrase, int range_low)
{
   int i;
   voicenote *nlist=0;
   for (i=0; i < stb_arr_len(phrase); ++i) {
      voicenote v = { 0 };
      int sn = phrase[i].scalenote;

      if (i == 0) {
         sn = find_nearest_note_to_chord(sn, start_chord);
      } else if (i == stb_arr_len(phrase)-1) {
         sn = find_nearest_note_to_chord(sn, end_chord);
      }

      v.midinote[0] = midi_note_for_scale(sn, range_low);

      v.duration = phrase[i].duration;
      assert(v.duration > 0 && v.duration <= WHOLE_NOTE);
      stb_arr_push(nlist, v);
   }
   return nlist;
}

enum
{
   I_soprano,
   I_mezzo,
   I_bass,

   I_piano,
   I_flute,
   I_clarinet,
   I_trumpet,
   I_horn,
   I_harp,
   I_violin1,
   I_violin2,
   I_viola,
   I_cello,
   I_double_bass,

   I__count
};

#define MAX_VOICES_PER_INSTRUMENT  2

typedef struct
{
   voicenote *voice[I__count][MAX_VOICES_PER_INSTRUMENT];
} arranged_phrase;

void random_chord(chord *a)
{
   a->scalenote[0] = random(SCALE_PITCHES);
   a->scalenote[1] = a->scalenote[0] + 2;
   a->scalenote[2] = a->scalenote[0] + 4;
}

voicenote *make_chord_voice(timedchord *c, int low_range)
{
   int i,j;
   voicenote *nlist=0;
   for (i=0; i < stb_arr_len(c); ++i) {
      voicenote n={0};

      for (j=0; j < 3; ++j) {
         if (c[i].c.scalenote[j] < 0)
            n.midinote[j] = 0;
         else
            n.midinote[j] = midi_note_for_scale(c[i].c.scalenote[j], low_range);
         assert(n.midinote[j] >= 0 && n.midinote[j] < 128);
      }

      assert(n.midinote[j] >= 0 && n.midinote[j] < 128);

      n.duration = c[i].duration;
      stb_arr_push(nlist, n);
   }
   return nlist;
}

int find_final_chord(chord *end_chord, voicenote *phrase)
{
   int final_chord = stb_arr_len(phrase)-1;
   while (final_chord > stb_arr_len(phrase)/2+1) {
      if (phrase[final_chord].midinote[0] != phrase[final_chord-1].midinote[0])
         break;
      --final_chord;
   }
   return final_chord;
}

timedchord *make_timedchords(chord *start_chord, chord *end_chord, int final_chord_time, int total_time)
{
   timedchord *tc = NULL;
   //int align;
   assert(total_time % WHOLE_NOTE == 0);

   //align = WHOLE_NOTE/4;
   //final_chord_time = (final_chord_time + (align-1)) / align * align;

   if (final_chord_time % WHOLE_NOTE <= WHOLE_NOTE / 2 && final_chord_time >= WHOLE_NOTE) {
      if (random(100) < 85)
         final_chord_time -= final_chord_time % WHOLE_NOTE;
   }

   {
      timedchord t;
      t.c = *start_chord;
      t.duration = final_chord_time;
      stb_arr_push(tc, t);
      t.c = *end_chord;
      t.duration = total_time - final_chord_time;
      stb_arr_push(tc, t);
   }

   return tc;
}

int *compute_times(timedchord *changes)
{
   int duration=0, i;
   int *times=0;
   for (i=0; i < stb_arr_len(changes); ++i) {
      stb_arr_push(times, duration);
      duration += changes[i].duration;
   }
   stb_arr_push(times, duration);
   return times;
}

char *timing_patterns[] =
{
   "x..x..x.",
   "x.x.x...",
   "x...x.x.",
   "x.xx.xx.",
   "xxx.....",
   "xxx..x..",
   "xxx...x.",
};


enum
{
   MODE_quarters,
   MODE_eighths,
   MODE_sixteenths,
   MODE_pattern,

   MODE_silent,

   MODE__count
};

int instrument_mode[I__count];
int instrument_voice[I__count];

#define MIDDLE_C   60

int instrument_low[I__count] =
{
   MIDDLE_C,
   MIDDLE_C-3,
   MIDDLE_C-12,

   MIDDLE_C-12,

   MIDDLE_C+12,
   MIDDLE_C,

   MIDDLE_C-12,
   MIDDLE_C-24,

   MIDDLE_C,

   MIDDLE_C-5,
   MIDDLE_C-5,
   MIDDLE_C-12,
   MIDDLE_C-24,
   MIDDLE_C-29
};

static int pattern=0;
char *pick_a_random_pattern(int instrument)
{
   switch (instrument_mode[instrument]) {
      case MODE_silent    : return ".";
      case MODE_quarters  : return "xxxx"; 
      case MODE_eighths   : return "xxxxxxxx";
      case MODE_sixteenths: return "xxxxxxxxxxxxxxxx";
      case MODE_pattern   : return timing_patterns[pattern];
      default: assert(0); return 0;
   }
}

void init_instrument_modes(void)
{
   int i;
   int likelihood = random_range(20,80);

   for (i=0; i < I__count; ++i)
      if (random(100) < likelihood)
         instrument_mode[i] = random(MODE__count);
      else
         instrument_mode[i] = MODE_silent;

   for (i=0; i < I__count; ++i)
      instrument_voice[i] = i % 3;
   instrument_voice[I_piano] = -1;
}


timedchord *play_against_timedchords(timedchord *changes, int total_time, int instrument)
{
   int stepsize;
   char *pattern = pick_a_random_pattern(instrument);
   int *times = compute_times(changes);
   int voice_select = instrument_voice[instrument];
   timedchord *tc=0;
   int j=0, start, k, len = strlen(pattern);

   stepsize = WHOLE_NOTE/strlen(pattern);

   start=0;
   for(start=0; start*stepsize < total_time; ) {
      timedchord c = { { -1,-1,-1 }, 0 };
      k = start+1;
      // advance the offset to next 'x'
      while (pattern[k%len] != 'x' && k*stepsize < total_time)
         ++k;
      if (pattern[start%len] == 'x') {
         while (times[j+1] <= start*stepsize)
            ++j;
         if (voice_select == -1)
            c.c = changes[j].c;
         else
            c.c.scalenote[0] = changes[j].c.scalenote[voice_select];
      }
      if (k-start >= 5 && k-start != 8) {
         do {
            c.duration = 4 * stepsize;
            stb_arr_push(tc, c);
            c.c.scalenote[0] = c.c.scalenote[1] = c.c.scalenote[2] = -1;
            start += 4;
         } while (k-start > 4);
         c.duration = (k-start)*stepsize;
         stb_arr_push(tc,c);
      } else {
         c.duration = (k-start)*stepsize;
         stb_arr_push(tc, c);
      }
      start = k;
   }
   stb_arr_free(times);
   return tc;
}

arranged_phrase *arrange_phrase(chord *start_chord, chord *end_chord, note *phrase, int soloist)
{
   int duration, goal, fc, second_chord_time, i;
   timedchord *tc, *changes;
   arranged_phrase *p = malloc(sizeof(*p));
   memset(p,0,sizeof(*p));

   p->voice[soloist][0] = adapt_melody(start_chord, end_chord, phrase, 60);
   fc = find_final_chord(end_chord, p->voice[soloist][0]);

   duration = phrase_duration(phrase, stb_arr_len(phrase));
   goal = duration + WHOLE_NOTE*1/4;
   goal = (goal + WHOLE_NOTE-1)/WHOLE_NOTE * WHOLE_NOTE;

   second_chord_time = phrase_duration(phrase, fc);
   changes = make_timedchords(start_chord, end_chord, second_chord_time, goal);

   // @TODO: refine vocal melody based on changes

   p->voice[soloist][0] = add_rests_at_end(p->voice[soloist][0], goal-duration);

   for (i=I_piano; i < I__count; ++i) {
      tc = play_against_timedchords(changes, goal, i);
      p->voice[i][0] = make_chord_voice(tc, instrument_low[i] - instrument_low[i]%12);
   }

   {
      static int timer=0;
      if (timer == 0) {
         pattern = random_range(0,6);
         timer = 2;
      }
      --timer;
   }

   return p;
}

void fixup_key(void)
{
   int i;
   for (i=0; i < 2; ++i)
      scale[i] = scale[i + 7] - 12;
   for (i=7+2; i < 15; ++i)
      scale[i] = scale[i - 7] + 12;
}

void drunk_walk(int slot)
{
   int step = random(100) < 50 ? -1 : 1;
   int new_note = scale[slot] + step;
   if (new_note == scale[slot-1])
      return;
   if (new_note == scale[slot+1])
      return;
   scale[slot] = new_note;
}

int major[7] = { 0,2,4, 5,7,9,11 };
int reset_keyarr[9] = { 0,2,4, 5,7,9,11, 12, 14 };

void spread_key(void)
{
   int i;
   int newscale[7];
   float weight = random_range(25,75) / 100.0f;

   newscale[0] = (int) ceil(stb_lerp(0.66, scale[2], scale[1]));
   for (i=1; i < 7; ++i)
      newscale[i] = newscale[0] + major[i];

   for (i=0; i < 7; ++i)
      scale[2+i] = floor(stb_lerp(weight, scale[2+i], newscale[i]) + 0.5);

   for (i=1; i < 7; ++i)
      if (scale[2+i] <= scale[2+i-1])
         scale[2+i] = scale[2+i-1] + random_range(1,2);
   fixup_key();

   if (scale[8] >= scale[9]) {
      printf("[force] ");
      for (i=0; i < 7; ++i)
         scale[2+i] = newscale[i];
      fixup_key();
   }
}

void reset_key(void)
{
   int i;
   for (i=0; i < 9; ++i)
      scale[i] = reset_keyarr[i];
   fixup_key();
}


void randomize_key(void)
{
   int i;
   return;
   {
      if (random(100) < 50) {
         drunk_walk(random_range(2, 7+2-1));
         fixup_key();
         if (random(100) < 50) {
            drunk_walk(random_range(2, 7+2-1));
            fixup_key();
            if (random(100) < 50) {
               drunk_walk(random_range(2, 7+2-1));
               fixup_key();
               if (random(100) < 50) {
                  drunk_walk(random_range(2, 7+2-1));
                  fixup_key();
               }
            }
         }
         printf("drunk : ");
      } else {
         spread_key();
         printf("spread: ");
      }
   }
   for (i=0; i < 7; ++i)
      printf("%d ", scale[i]);
   printf("\n");
}

arranged_phrase **song;

void create_music(void)
{
   int i;

   for (i=0; i < stb_arr_len(vocals); ++i) {
      char **syl = vocals[i].syllables;
      arranged_phrase *p;
      chord a,b;
      note *nlist;

      if (i % 32 == 0)
         init_instrument_modes();

      if (i % 8 == 0) {
         reset_key();
      } else {
         if (i % 2 == 0)
            randomize_key();
      }

      nlist = generate_phrase(stb_arr_len(syl));
      random_chord(&a);
      do
         random_chord(&b);
      while (a.scalenote[0] == b.scalenote[0] && a.scalenote[1] == b.scalenote[1]);

      p = arrange_phrase(&a, &b, nlist, vocals[i].voice);
      if (i == OUTPUT_START && 0) {
         p->voice[I_mezzo][0] = p->voice[I_soprano][0];
         p->voice[I_bass ][0] = p->voice[I_soprano][0];
      }
      stb_arr_push(song, p);

   }
}

void do_instrument_music(int inst)
{
   int i,j;

   for (i=OUTPUT_START; i < OUTPUT_END && i < stb_arr_len(song); ++i) {
      int current_time = 0;
      //char **syl = vocals[i].syllables;
      voicenote *nlist = song[i]->voice[inst][0];

      if (nlist == NULL) {
         for (j=0; j < I__count; ++j) {
            if (song[i]->voice[j][0] != NULL) {
               nlist = song[i]->voice[j][0];
               break;
            }
         }
         assert(nlist != NULL);
         for (j=0; j < stb_arr_len(nlist); ++j) {
            int dur = nlist[j].duration;
            while (dur > 0) {
               int step_time = dur;
               if (current_time + dur > WHOLE_NOTE) {
                  step_time = aligning_time(current_time);
                  assert(current_time + step_time <= WHOLE_NOTE);
               }
               lily_print_note(0);
               lily_print_duration(step_time);
               dur -= step_time;
               current_time += step_time;
               if (current_time >= WHOLE_NOTE)
                  current_time -= WHOLE_NOTE;
            } 
            fprintf(f, "\n");
         }

      } else {
         for (j=0; j < stb_arr_len(nlist); ++j) {
            int dur = nlist[j].duration;
            while (dur > 0) {
               int step_time = dur;
               if (current_time + dur > WHOLE_NOTE) {
                  step_time = aligning_time(current_time);
                  assert(current_time + step_time <= WHOLE_NOTE);
               }
               if (nlist[j].midinote[1] != 0) {
                  int k;
                  fprintf(f, "<");
                  for (k=0; k < MAX_NOTES_PER_VOICE; ++k) {
                     if (nlist[j].midinote[k] == 0)
                        break;
                     lily_print_note(nlist[j].midinote[k]);
                     if (k+1 < MAX_NOTES_PER_VOICE && nlist[j].midinote[k+1])
                        fprintf(f, " ");
                  }
                  fprintf(f, ">");
               } else {
                  lily_print_note(nlist[j].midinote[0]);
               }
               lily_print_duration(step_time);
               dur -= step_time;
               current_time += step_time;
               if (current_time >= WHOLE_NOTE)
                  current_time -= WHOLE_NOTE;
               if (dur && nlist[j].midinote[0] != 0)
                  fprintf(f, " ~ ");
            }
            fprintf(f, "\n");
         }
      }
   }
}

// Lyr -- ics }\n");
void do_vocal_lyrics(int inst)
{
   int i,j;
   for (i=OUTPUT_START; i < OUTPUT_END && i < stb_arr_len(song); ++i) {
      if (vocals[i].voice == inst) {
         char **syl = vocals[i].syllables;
         for (j=0; j < stb_arr_len(syl); ++j) {
            char *raw_s = strdup(syl[j]), *s = raw_s;
            char *t;
            if (*s == '!') ++s;
            t = strchr(s, '-');
            if (t) {
               *t = 0;
               fprintf(f, "%s -- ", s);
            } else
               fprintf(f, "%s ", s);
            free(raw_s);
            fprintf(f, "\n");
         }
      } else {

      }
   }
}


//  bass: G2..E4 (F2..F4)
//  soprano: D4..B5 (C4..C6)
//  mezzo-soprano: B3..G5 (A3..A5)

#if 1
void write_music(void)
{
   int i;
   syllable_map = stb_sdict_new(1);

   create_music();

   f = fopen("music.ly", "w");
   fprintf(f, "\\version \"2.18.2\"\n\n");
   fprintf(f, "pianoRHMusic = { r }\n");

   for (i=0; i < I__count; ++i) {
      static char *name[I__count][2] = {
         { "sopranoMusic",   NULL, },
         { "mezzoMusic",     "\"treble_8\"" },
         { "vbassMusic",     "bass" },
         { "pianoLHMusic",   "bass" },
         { "fluteMusic",     NULL, },
         { "clarinetMusic",  NULL, },
         { "trumpetMusic",   NULL, },
         { "hornMusic",      NULL, },
         { "harpMusic",      NULL, },
         { "violinIMusic",   NULL, },
         { "violinIIMusic",  NULL, },
         { "violaMusic",     "alto" },
         { "celloMusic",     "bass" },
         { "bassMusic",     "\"bass_8\"" }
      };

      if (name[i][1])
         fprintf(f, "%s = { \\clef %s ", name[i][0], name[i][1]);
      else
         fprintf(f, "%s = { ", name[i][0], name[i][1]);

      do_instrument_music(i);

      fprintf(f, "}\n\n");
   }

   fprintf(f, "sopranoLyrics = \\lyricmode {\n"); do_vocal_lyrics(I_soprano); fprintf(f, "\n}\n");
   fprintf(f, "mezzoLyrics = \\lyricmode {\n")  ; do_vocal_lyrics(I_mezzo  ); fprintf(f, "\n}\n");
   fprintf(f, "vbassLyrics = \\lyricmode {\n")  ; do_vocal_lyrics(I_bass   ); fprintf(f, "\n}\n");
   fclose(f);
}
#else
void write_music(void)
{
   syllable_map = stb_sdict_new(1);


   f = fopen("music.ly", "w");
   fprintf(f, "\\version \"2.18.2\"\n\n");
   fprintf(f, "fluteMusic = \\relative c' { g'1 b }\n");
   fprintf(f, "clarinetMusic = \\relative c'' { bes1 d }\n");
   fprintf(f, "trumpetMusic = \\relative c { g''1 b }\n");
   fprintf(f, "hornMusic = \\relative c { d'1 fes }\n");
   fprintf(f, "pianoRHMusic = \\relative c { g''1 b }\n");
   fprintf(f, "pianoLHMusic = \\relative c { \\clef bass g1 b }\n");
   fprintf(f, "violinIMusic = \\relative c' { g'1 b }\n");
   fprintf(f, "violinIIMusic = \\relative c' { g'1 b }\n");
   fprintf(f, "violaMusic = \\relative c { \\clef alto g'1 b }\n");
   fprintf(f, "celloMusic = \\relative c { \\clef bass g1 b }\n");
   fprintf(f, "bassMusic = \\relative c { \\clef \"bass_8\" g,1 b }\n");
   fprintf(f, "harpMusic = \\relative c'' { e1 }\n");
   fprintf(f, "\n");
   fprintf(f, "sopranoMusic = {\n");
   //\\relative c'' { g'4 a b c b1 }\n");
   do_soprano_music();
   fprintf(f, "\n}\n");
   fprintf(f, "sopranoLyrics = \\lyricmode {\n");
   do_soprano_lyrics();
   fprintf(f, "\n}\n");
   fprintf(f, "vbassMusic = \\relative c { g'1 a4 b c b }\n");
   fprintf(f, "vbassLyrics =  \\lyricmode { Lyr -- ics }\n");
   fprintf(f, "mezzoMusic = \\relative c'' { \\clef \"treble_8\" g1 b1 a4 b c b }\n");
   fprintf(f, "mezzoLyrics =  \\lyricmode { Lyr -- ics }\n");
   fclose(f);
}
#endif


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
int hack_count[3][2];
int hack_count2[20];

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
   "Har-ri-son", "Starr", "Lake", "Pal-mer", "Sum-mers",
   "Maur", "Orr", "Ev-ans", "Cale", "Byrne",
   "Hath-er-ley", "Gil-mour", "Torn", "Lan-ois", "Ken-ne-dy",
   "Cos-tel-lo", "Kai-ser", "Han-son", "Jen-sen", "Gil-ber-to",
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
int last_voice;

char *voicename[] =
{
   "Soprano",
   "Bass",
   "Mezzo",
};

int last_player=0;
int force_team_mode = -1;

vocal_line pending;

static char **syllablize(char *line)
{
   char syllable[64];
   char **list = NULL;
   int i,s;
   while (*line) {
      char *x;
      while (isspace(*line)) ++line;
      if (!*line) break;
      s = 0;
      while (!isspace(line[s]) && line[s] != '.' && line[s] != '-' && line[s])
         ++s;
      if (line[s] == '.' || line[s] == '-')
         ++s;
      for (i=0; i < s; ++i)
         syllable[i] = line[i];
      syllable[i] = 0;
      line += s;
      x = stb_dupreplace(syllable, "_", "...");
      stb_arr_push(list, x);
   }
   return list;
}

static void close_vocal_line(void)
{
   int i;
   vocal_line add;
   if (pending.cur_line) {
      if (stb_arr_len(pending.cur_line) == 1 && pending.cur_line[0] == '.') {
         i=0;
      } else {
         stb_arr_push(pending.cur_line, 0);
         add.voice = pending.voice;
         add.cur_line = strdup(pending.cur_line);
         add.syllables = syllablize(add.cur_line);
         #if 0
         printf("%s: ", voicename[pending.voice]);
         for (i=0; i < stb_arr_len(add.syllables); ++i)
            printf("%s ", add.syllables[i]);
         printf("\n");
         #endif
         stb_arr_push(vocals, add);
      }
      stb_arr_free(pending.cur_line);
      pending.cur_line = NULL;
      pending.voice = -1;
   }
}

static void add_lyrics(int voice, char *text)
{
   if (pending.voice >= 0)
      if (voice != pending.voice)
         close_vocal_line();

   while (*text) {
      if (pending.cur_line == NULL) {
         if (*text != ' ')
            stb_arr_push(pending.cur_line, toupper(*text));
      } else 
         stb_arr_push(pending.cur_line, *text);
      pending.voice = voice;
      if (*text == '.')
         close_vocal_line();
      ++text;
   }
}

/*
   a hundred and one
   a hundred and two
   a hundred and three
   a hundred and four
   a hundred and five
   a hundred and six
   a hundred and seven
   a hundred and eight
   a hundred and ten
   a hundred and eleven
    

*/

char *english_number(char *s, int n)
{
   char *a;
   if (n >= 100) {
      strcpy(s, "a hun-dred");
      s += strlen(s);
      if (n > 100) {
         strcpy(s, " and ");
         s += strlen(s);
         english_number(s, n-100);
         s += strlen(s);
      }
      return s;
   }
   if (n >= 20) {
      switch (n/10) {
         case 2: strcpy(s, "twen-ty"); break;
         case 3: strcpy(s, "thir-ty"); break;
         case 4: strcpy(s, "for-ty"); break;
         case 5: strcpy(s, "fif-ty"); break;
         case 6: strcpy(s, "six-ty"); break;
         case 7: strcpy(s, "se-ven-ty"); break;
         case 8: strcpy(s, "eigh-ty"); break;
         case 9: strcpy(s, "nine-ty"); break;
      }
      n -= (n/10)*10;
      s += strlen(s);
      if (n == 0)
         return s;
      strcpy(s, " ");
      s += strlen(s);
   }
   switch (n) {
      case 0: a = "zero"; break;
      case 1: a = "one"; break;
      case 2: a = "two"; break;
      case 3: a = "three"; break;
      case 4: a = "four"; break;
      case 5: a = "five"; break;
      case 6: a = "six"; break;
      case 7: a = "sev-en"; break;
      case 8: a = "eight"; break;
      case 9: a = "nine"; break;
      case 10: a = "ten"; break;
      case 11: a = "e-lev-en"; break;
      case 12: a = "twelve"; break;
      case 13: a = "thir-teen"; break;
      case 14: a = "four-teen"; break;
      case 15: a = "fif-teen"; break;
      case 16: a = "six-teen"; break;
      case 17: a = "se-ven-teen"; break;
      case 18: a = "eigh-teen"; break;
      case 19: a = "nine-teen"; break;
   }
   strcpy(s, a);
   return s + strlen(s);
}


int no_the;

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
      } else if (*s == '&') {
         int n;
         n = va_arg(v,int);
         ++s;
         if (*s == '&') {
            strcpy(t, teamname[n][TEAMNAME_full]);
            ++s;
            was_plural = 1;
         } else {
            int z = stb_rand() & 1;
            if (force_team_mode >= 0)
               z = force_team_mode;
            if (no_the && 0==strncmp(teamname[n][z], "the ", 4))
               strcpy(t, teamname[n][z]+4);
            else
               strcpy(t, teamname[n][z]);
            was_plural = (z == 1);
         }
         t += strlen(t);
         last_player = 0;
      } else if (*s == '#') {
         int n = va_arg(v,int);
         ++s;
         t = english_number(t, n);
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

   add_lyrics(voice, buffer);

   last_voice = voice;
   if (pcount > 1)
      last_player = 0;
}

int this_event_time, last_event_time = 0;
int last_sub_time;
int color_ok_flag;

int team_roster[2][5];

int slot(int t, int player)
{
   int i;
   for (i=0; i < 5; ++i)
      if (team_roster[t][i] == player)
         return i;
   assert(0);
   return 0;
}

void set_lineup(int t, int v1, int v2, int v3, int v4, int v5)
{
   team_roster[t][0] = v1;
   team_roster[t][1] = v2;
   team_roster[t][2] = v3;
   team_roster[t][3] = v4;
   team_roster[t][4] = v5;
}

static char *positions[5] = { "point guard", "shoot-ing guard", "small for-ward", "pow-er for-ward", "cen-ter" };

int score[2];

typedef struct
{
   int attempts;
   int made;
   int points;
} stat_makeable;

typedef struct
{
   stat_makeable game;
   stat_makeable quarter[4];
   stat_makeable half[2];

   int streak_made;
   int streak_length; // 0
} stat_streakable;

typedef struct
{
   int game;
   int quarter[4];
   int half[2];
} stat_plain;

enum
{
   SP_rebounds, SP_off_rebounds, SP_def_rebounds,
   SP_assists,
   SP_turnovers,
   SP_steals,
   SP_blocks,
   SP_fouls,

   SP__count,

   SS_layup, SS_fg, SS_jump, SS_f3, SS_ft, SS_total,

   SS__count,
};


// commentary perspective
enum
{
   ASPECT_game,
   ASPECT_half,
   ASPECT_quarter,
   ASPECT_streak,

   ASPECT_season,
   ASPECT_yesterday,

   ASPECT__count,
};

typedef struct
{
   stat_plain plain[SP__count];
   stat_streakable streakable[SS__count];

   int did_comment_when[SS__count][ASPECT__count];
} player_stats;

enum
{
   SG_tonight,
   SG_yesterday,
   SG_season,

   SG__count,
};

int cur_line;
int last_shot_time;
player_stats raw_stats[25][SG__count];
player_stats (*stats)[SG__count] = raw_stats+12;
int quarter; // [0..3]
int half;    // [0..1]

typedef struct
{
   int stat;
   int player;
   int aspect;
   float priority;
   float priority_interestingness; // 0..???, 0 is not interesting
   float priority_recent;          // 0..???, 0 is just said it
} color_statistic;

int color_timestamp;

static float compute_stat_interestingness(int stat, int player, int aspect)
{
   assert(aspect < ASPECT_season);
   if (stat < SP__count) {
      stat_plain *sp = &stats[player][0].plain[stat]; // @TODO aspect >= ASPECT_SEASON
      switch (aspect) {
         case ASPECT_game:
            return 10;
         case ASPECT_half:
            if (half == 0) return 0;
            if (sp->half[1] > sp->half[0]*2 && sp->half[0] >= 2)
               return 3*(sp->half[1] - sp->half[0]);
            if (this_event_time < 36*60) return 0;
            return 2*(sp->half[1] - sp->half[0]);
         case ASPECT_quarter:
            if (quarter != 1) return 0;
            if (sp->quarter[1] > sp->quarter[0]*2 && sp->quarter[0] >= 1)
               return 4*(sp->quarter[1] - sp->quarter[0]);
            if (this_event_time < 18*60) return 0;
            return 3*(sp->quarter[1] - sp->quarter[0]);
         case ASPECT_streak:
            return 0;
         default:
            assert(0);
      }
   } else {
      // @TODO
      return 0;
   }
   return 0;
}

float square(float x)
{
   return atan(x/4);
}

static void compute_statistic_priority(color_statistic *cs)
{
   int time_since_last, prev_timestamp;
   assert(cs->aspect < ASPECT_season);

   prev_timestamp = stats[cs->player]->did_comment_when[cs->stat][cs->aspect];
   time_since_last = color_timestamp - prev_timestamp;

   if (time_since_last < 2)
      cs->priority_recent = 0;
   else
      cs->priority_recent = square(time_since_last - 1); // @TODO run this through some function, like an expoential or square or something

   if (cs->aspect >= ASPECT_season) {
      if (prev_timestamp != 0)
         cs->priority_recent = 0;
   }

   cs->priority_interestingness = compute_stat_interestingness(cs->stat, cs->player, cs->aspect);

   cs->priority = cs->priority_interestingness * cs->priority_recent;
}

#define TEAM0_PLAYER     11
#define TEAM1_PLAYER    -11

int player_for_team[2] = { TEAM0_PLAYER, TEAM1_PLAYER };

int team_for_player(int player)
{
   return player<0 ? TEAM_1 : TEAM_0;
}

static color_statistic *color_add_analysis(color_statistic *list, int stat, int player)
{
   int p[2] = { player, player_for_team[team_for_player(player)] };
   int i,j;

   for (i=0; i < 2; ++i) {
      for (j=0; j < ASPECT_season; ++j) { // @TODO
         color_statistic cs;
         if (stat < SP__count && j == ASPECT_streak)
            continue;
         cs.player = p[i];
         cs.stat = stat;
         cs.aspect = j;
         compute_statistic_priority(&cs);
         if (cs.priority)
            stb_arr_push(list, cs);
      }
   }
   return list;
}

void update_plain_duration(stat_plain *sp, int count)
{
   sp->game += count;
   sp->half[half] += count;
   sp->quarter[quarter] += count;
}


void add_plain(int type, int player, int count)
{
   if (player) {
      int team_stat = player_for_team[team_for_player(player)];
      update_plain_duration(&stats[ player  ][SG_tonight].plain[type], count);
      update_plain_duration(&stats[team_stat][SG_tonight].plain[type], count);
   }
}

void update_stats_duration(stat_makeable *sm, int made, int points)
{
   sm->attempts += 1;
   sm->made     += made;
   sm->points   += points * made;
}

void update_streakable(int type, int player, int made, int points)
{
   stat_streakable *ss = &stats[player][SG_tonight].streakable[type];
   update_stats_duration(&ss->game, made, points);
   update_stats_duration(&ss->half[half], made, points);
   update_stats_duration(&ss->quarter[quarter], made, points);

   if (made != ss->streak_made) {
      ss->streak_made = made;
      ss->streak_length = 0;
   }
   ss->streak_length += 1;
}

void add_streakable(int type, int player, int made, int points)
{
   if (player) {
      update_streakable(type, player, made, points);
      update_streakable(type, player_for_team[team_for_player(player)], made, points);
   }   
}



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
   int after_rebound;
   int ball_alive;
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
   F_reverse=6,
   F_tip,
   F_dunk,
   F_running,
   F_running_bank=10,
   F_putback,
   F_putback_dunk,
   F_driving,
   F_driving_finger_roll,
   F_alleyoop=15,
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
   TURNOVER_traveling,      // 5
   TURNOVER_badpass,        // 3
   TURNOVER_foul,           // 2
   TURNOVER_out_of_bounds,  // 1
   TURNOVER_palming,        // 2
   TURNOVER_3sec,           // 1
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
      if (i == 2 && range == 3)
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

#define random_nonrepeat(range) random_nonrepeat(range, __LINE__)
#define once()                  once(__LINE__)


void disable_color(void)
{
   color_ok_flag = 0;
}

void enable_color(void)
{
   color_ok_flag = 1;
}

int can_do_color(void)
{
   return color_ok_flag;
}

int subs[10][2], subcount;


int force_announce_points=1;

int last_announced_score[2];

int has_announced_score(void)
{
   return last_announced_score[0] == score[0] && last_announced_score[1] == score[1];
}

void points_announce(int voice, int force)
{
   force_team_mode = stb_rand() & 1;
   if (random(100) < 50)
      voice = !voice;
   enable_color();
   if (score[0] == score[1] && score[0] >= 10 && random(100) < 90)
      if (force)
         sing(voice, "Both teams have #.", score[0]);
      else
         sing(voice, "The score is tied up, with # points for both teams.", score[0]);
   else if (score[0] > score[1])
      sing(voice, "The score is & #, & #.", TEAM_0, score[0], TEAM_1, score[1]);
   else
      sing(voice, "The score is & #, & #.", TEAM_1, score[1], TEAM_0, score[0]);

   last_announced_score[0] = score[0];
   last_announced_score[1] = score[1];
   force_team_mode = 0;
}

void add_points(int team, int points)
{
   int force=1;
   int voice = stb_rand() % 3; // pick whoever didn't just talk
   int leader = (score[0] < score[1] ? TEAM_1 : score[0] > score[1] ? TEAM_0 : -1);
   int randomly;

   // @TODO variation
   score[team] += points;
   if (score[0] == score[1] && this_event_time > 12*60) {
      sing(voice, "That ties it up.");
      enable_color();
   } else if (leader == TEAM_0 && score[1] > score[0] && this_event_time > 6*60) {
      sing(voice, "That puts & ahead.", TEAM_1);
      enable_color();
   } else if (leader == TEAM_1 && score[0] > score[1] && this_event_time > 6*60) {
      sing(voice, "That puts & ahead.", TEAM_0);
      enable_color();
   } else {
      force = 0;
   }

   no_the = 1;

   if (points > 1)
      randomly = (random(100) < 30);
   else
      randomly = (random(100) < 60);

   if (random(3) == 0 || force || this_event_time > 45*60 || force_announce_points) 
      points_announce(voice, force);
   no_the = 0;

   game_state.ball_alive = 0;
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

int multi_mode;
void substitute(int prev_player, int new_player, int multi)
{
   int tm = prev_player < 0 ? TEAM_1 : TEAM_0;
   int voice = random(100)<93 ? V1 : V2;
   int s;

   assert(prev_player*new_player > 0);
   s = slot(tm, prev_player);
   team_roster[tm][s] = new_player;

   if (multi) {
      if (multi > 0) {
         switch (multi_mode) {
            case 0: sing(last_voice, "$ is re-placed by $", prev_player, new_player); break;
            case 1: sing(last_voice, "$ comes in for $", new_player, prev_player); break;
            case 2: sing(last_voice, "$ sub-sti-tut-ed for $", new_player, prev_player); break;
         }
         if (multi == 1)
            sing(last_voice, ", ");
      }
   } else {
      switch (random_nonrepeat(7)) {
         case 0:             sing(voice, "$ is sub-sti-tut-ed in for $."   ,     new_player , prev_player); break;
         case 1:             sing(voice, "$ is re-placed by $."            ,     prev_player, new_player ); break;
         case 2: now(voice); sing(voice, "$ goes out, sub-sti-tut-ed by $.",     prev_player, new_player ); break;
         case 3: now(voice); sing(voice, "& put[s|] $ in for $."           , tm, new_player , prev_player); break;
         case 5:             sing(voice, "$ subbed out, re-placed by $."   ,     prev_player, new_player ); break;
         case 6:             sing(voice, "For &, $ goes in for $.", tm     ,     new_player , prev_player); break;
         case 4:             sing(voice, "$ comes in for $."               ,     new_player , prev_player); break;
            break;
         default:
            assert(0);
      }
   }
}

int check_move(int i, int start, int end)
{
   int a;
   int tm = subs[start][0] < 0 ? TEAM_1 : TEAM_0;
   int other_old = -1, other_new = -1;
   // check if there is an event where either player is subbed again
   for (a=start; a < end; ++a) {
      if (a != i) {
         if (subs[a][0] == subs[i][1])
            other_new = a;
         if (subs[a][1] == subs[i][0])
            other_old = a;
      }
   }

   assert(other_old < 0 || other_new < 0);

   if (other_old >= 0) {
      char buffer[256];
      static char *and_while[2] = { "and", "while" };
      sprintf(buffer, "$ moves to %s, %s $ comes in as %s,",
         positions[slot(tm,subs[other_old][0])],
         and_while[stb_rand()&1],
         positions[slot(tm,subs[i][0])]);
      sing(V1, buffer, subs[i][1], subs[i][0]);
      return 1;
   }
   if (other_new >= 0) {
      return 1;
   }
   return 0;
}

void process_substitution(void)
{
   int j,i,k,t;

   if (subcount > 0 ) {
      if (game_state.ball_alive) {
         sing(V1, "There's a time out.");
         game_state.ball_alive = 0;
      }
      if (subcount > 1) {
         int tm = subs[0][0] < 0 ? TEAM_1 : TEAM_0;
         for (j=1; j < subcount; ++j)
            if (tm != subs[j][0] < 0 ? TEAM_1 : TEAM_0)
               break;

         if (j < subcount ? subcount >= 3 : subcount >= 2) {
            // there's at least two substitutions on one team
            multi_mode = random_nonrepeat(3);
         }

         if (j < subcount) {
            force_team_mode = stb_rand() & 1;
            if (j == 1) {
               switch (random_nonrepeat(3)) {
                  case 0: sing(V1, "& [is|are] go-ing to sub out $ and re-place him with $.", tm, subs[0][0], subs[0][1]); break;
                  case 1: sing(V1, "For &, $ comes out, re-placed by $.", tm, subs[0][0], subs[0][1]); break;
                  case 2: sing(V1, "Looking at &, $ is re-plac-ing $.", tm, subs[0][1], subs[0][0]); break;
                  default: assert(0);
               }
               substitute(subs[0][0], subs[0][1], -1);
            } else {
               sing(V1, "Sub-sti-tu-tions for &: ", tm);
               for (i=0; i < j; ++i) {
                  int suppress = check_move(i, 0,j);
                  substitute(subs[i][0], subs[i][1], suppress ? -1 : i==j-1 ? 2 : 1);
               }
               sing(last_voice, ".");
            }
            if (random(100) < 70)
               sing(last_voice, "And for &, ", !tm);
            else
               sing(last_voice, "For &, ", !tm);
            if ((subcount-j) == 1) {
               switch (random_nonrepeat(3)) {
                  case 0: sing(V1, "$ re-plac-es $.", tm, subs[j][1], subs[j][0]); break;
                  case 1: sing(V1, "$ re-placed by $.", tm, subs[j][0], subs[j][1]); break;
                  case 2: sing(V1, "$ comes in for $.", tm, subs[j][1], subs[j][0]); break;
               }
               substitute(subs[j][0], subs[j][1], -1);
            } else {
               for (i=j; i < subcount; ++i) {
                  int suppress = check_move(i, j,subcount);
                  substitute(subs[i][0], subs[i][1], suppress ? -1 : i==subcount-1 ? 2 : 1);
               }
               sing(last_voice, ".");
            }

            force_team_mode = -1;
         } else {
            sing(V1, "Sub-sti-tu-tions for &: ", tm);
            for (i=0; i < subcount; ++i) {
               int suppress = check_move(i, 0,subcount);
               substitute(subs[i][0], subs[i][1], suppress ? -1 : i==subcount-1 ? 2 : 1);
            }
            sing(last_voice, ".");
         }
      } else {
         substitute(subs[0][0], subs[0][1], 0);
      }

      // now veryify no repeats
      for (t=0; t < 2; ++t)
         for (i=0; i < 5; ++i)
            for (k=0; k < 5; ++k)
               if (i != k)
                  assert(team_roster[t][i] != team_roster[t][k]);

      game_state.position = 0;
      game_state.ball_alive = 1;
      subcount = 0;
      disable_color();
   }
}

void enqueue_substitution(int p1, int p2)
{
   if (last_sub_time != this_event_time && subcount)
      process_substitution();
   subs[subcount][0] = p1;
   subs[subcount][1] = p2;
   ++subcount;
   last_sub_time = this_event_time;
}

int who_was_fouled;
int foul, foul_player, foul_time;
int last_announced_possession_time;
int force_announce_flag;

int previous_pass;
void set_tpossess(int team, int force_announce)
{
   if (team != game_state.possess_team) {
      if (force_announce >= 0 && team != -1) {
         if (this_event_time > last_announced_possession_time + 90 || force_announce || force_announce_flag) {
            enable_color();
            switch (random_nonrepeat(4)) {
               case 0: sing(V1, "& [has|have] the ball.", team); break;
               case 1: sing(V1, "& with the ball.", team); break;
               case 2: sing(V1, "& [has|have] pos-ses-sion.", team); break;
               case 3: sing(V1, "& [has|have] con-trol of the ball.", team); break;
               default: assert(0);
            }
            last_announced_possession_time = this_event_time;
         }
      }
      game_state.possess_team = team;
      game_state.possess_player = 0;
      previous_pass = 0;
   }
   force_announce_flag = 0;
}

void tpossess(int team, int force_announce)
{
   if (game_state.possess_team != team) {
      set_tpossess(team, force_announce);
      game_state.possess_time = this_event_time;
      game_state.possess_action_time = this_event_time;
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

int compare_cs(const void *p, const void *q)
{
   color_statistic *a = (color_statistic *) p;
   color_statistic *b = (color_statistic *) q;
   return a->priority < b->priority ? -1 : a->priority > b->priority;
}

void color_report_analysis(color_statistic *list)
{
   int n;
   qsort(list, stb_arr_len(list), sizeof(*list), compare_cs);
   n = stb_arr_len(list)-1;
   // @TODO generate actual reports
   // @TODO do analysis for other events
   if (n >= 0) {
      n = n;
   }
   stb_arr_free(list);
}

int take_possession(int tm)
{
   if (game_state.possess_player == 0) {
      game_state.possess_player = team_roster[tm][random(100) < 95 ? 0 : 1];
      enable_color();
      if (!is_closer_than(-0.5)) {
         int p = game_state.possess_player;
         switch (random_nonrepeat(9)) {
            case 0: sing(V1, "$ brings the ball up the court.", p); break;
            case 1: sing(V1, "$ brings the ball up the court.", p); break;
            case 2: sing(V1, "$ brings the ball up.", p); break;
            case 3: sing(V1, "$ with the ball.", p); break;
            case 4: sing(V1, "$ with the ball.", p); break;
            case 5: sing(V1, "$ with the ball, brings it up court.", p); break;
            case 6: sing(V1, "$ has the ball.", p); break;
            case 7: sing(V1, "Now $ has the ball.", p); break;
            case 8: sing(V1, "Now $ with the ball.", p); break;
            default: assert(0);
         }
         game_state.position = 0;
         return 1;
      } else {
         int p = game_state.possess_player;
         switch (random_nonrepeat(5)) {
            case 0: sing(V1, "$ with the ball.", p); break;
            case 1: sing(V1, "$ has the ball.", p); break;
            case 2: sing(V1, "Now $ has the ball.", p); break;
            case 3: sing(V1, "$ receives the ball.", p); break;
         }
         return 0;
      }
   }
   return !is_closer_than(-0.5);
}

void pass_to(int tm, int player, float position, int chain)
{
   int pass_back = (player == previous_pass);
   assert(game_state.ball_alive);
   if (game_state.possess_player == player)
      return;
   enable_color();
   if (game_state.position == (tm ? -1.0f : 1.0f) && fabs(position) < 1.0)
      if (pass_back && random(100) < 70)
         switch (random_nonrepeat(2)) {
            case 0: sing(V1, "$ kicks it back out to $.", game_state.possess_player, player); break;
            case 1: sing(V1, "$ pass-es it back out to $.", game_state.possess_player, player); break;
            default: assert(0);
         }
      else
         switch (random_nonrepeat(3)) {
            case 0: sing(V1, "$ kicks it out to $.", game_state.possess_player, player); break;
            case 1: sing(V1, "$ pass-es it out to $.", game_state.possess_player, player); break;
            case 2: sing(V1, "Pass-es back out to $.", player); break;
            default: assert(0);
         }
   else if (fabs(game_state.position) < 1.0 && fabs(position) == 1.0)
      switch (random_nonrepeat(3)) {
         case 0: sing(V1, "$ pass-es it in to $.", game_state.possess_player, player); break;
         case 1: if (once())
                    sing(V1, "Bounce-pass inside to $.", player);
                 else
                    sing(V1, "$ with a pass inside to $.", game_state.possess_player, player);
                 break;
         case 2: sing(V1, "Makes an inside pass to $.", player); break;
         default: assert(0);
      }
   else
      //if (chain > 2) sing(V1, "to $.", player);
      if (pass_back && random(100) < 70)
         switch (random_nonrepeat(5)) {
            case 0: sing(V1, "Back to $.", player); break;
            case 1: sing(V1, "$ pass-es back to $.", game_state.possess_player, player); break;
            case 2: sing(V1, "And back to $.", player); break;
            case 3: sing(V1, "$ sends it back to $.", game_state.possess_player, player); break;
            case 4: sing(V1, "$ pass-es the ball back to $.", game_state.possess_player, player); break;
            default: assert(0);
         }
      else
         switch (random(5)) {
            case 0: sing(V1, "$ pass-es to $.", game_state.possess_player, player); break;
            case 1: sing(V1, "$ pass-es to $.", game_state.possess_player, player); break;
            case 2: sing(V1, "$ pass-es to $.", game_state.possess_player, player); break;
            case 3: sing(V1, "$ with a pass to $.", game_state.possess_player, player); break;
            case 4: sing(V1, "$ makes a pass to $.", game_state.possess_player, player); break;
            default: assert(0);
         }
   previous_pass = game_state.possess_player;
   game_state.possess_player = player;
   game_state.position = position;
}

void run_play(int ball_holder, float position, int end_of_play_time)
{
   int tm = game_state.possess_team;
   // run a play so the ball ends up in ball_holder's hands at position,
   // @TODO run actual plays  low-priority
   int delta_time = end_of_play_time - game_state.current_time;
   assert(game_state.ball_alive);
   if (delta_time < 5 + random(3)) {
      take_possession(tm);
      if (fabs(position - game_state.position) > 0.8f) {
         sing(V1, "$ brings the ball up the court.", game_state.possess_player);
         enable_color();
      }
      game_state.position = 0;
      if (random(100) < 50)
         pass_to(tm, team_roster[tm][random(5)], tm  ? -0.75f : 0.75f, 0);
      pass_to(tm, ball_holder, position, 0);
   } else {
      int n = 0;
      if (take_possession(tm))
         delta_time -= 2 + random(1);
      while (delta_time >= 0) {
         pass_to(tm, team_roster[tm][random(5)], tm ? -0.75f : 0.75f, n);
         delta_time -= 2 + random(1) + random(3)*(random(3)==0);
         delta_time -= 3+2*n; // @TODO tune number of passes (low-priority)
         n += 1;
      }
      pass_to(tm, ball_holder, position, n);
   }
   game_state.current_time = end_of_play_time;
}

void make_ball_alive(int tm)
{
   if (!game_state.ball_alive) {
      game_state.ball_alive = 1;
      game_state.position = tm ? 1.0f : -1.0f;
   }
}

float position_for_foul(void)
{
   return 0;
}

int team_fouls[2];
void fouled_shot(int player, int fouled_by);
void process_foul(int first_free_throw)
{
   int intentional;
   int tm = (foul_player < 0 ? TEAM_1 : TEAM_0);
   if (!foul)
      return;

   intentional = (foul == FOUL_personal) && (this_event_time > 48*60 - 30);

   make_ball_alive(!tm);
   if (intentional)
      game_state.position = 0;

   // run a play up to the time the foul occurs
   if (game_state.possess_action_time+1 < foul_time) {
      run_play(who_was_fouled, position_for_foul(), foul_time);
   }

   // if nobody has possession, give it to the point guard   
   if (!game_state.possess_player)
      game_state.possess_player = team_roster[foul_player > 0][random(100) < 90 ? 0 : 1];

   // if nobody was fouled, make it whoever had possession last,
   if (who_was_fouled == 0)
      who_was_fouled = game_state.possess_player;

   assert(who_was_fouled * foul_player < 0);

   if (foul == FOUL_personal) {
      if (first_free_throw && this_event_time != last_shot_time) {
         fouled_shot(who_was_fouled, foul_player);
      } else {
         if (who_was_fouled == 0) {
            sing(V1, "$ fouled nobody.", foul_player);
            assert(0);
         }
      }
   }

   team_fouls[tm] += 1;

   if (foul == FOUL_personal) {
      if (team_fouls[tm] > 5)
         assert(first_free_throw);
      else
         assert(!first_free_throw);
   }

   hack_count[foul][first_free_throw] += 1;
   if (foul == FOUL_shooting) {
      enable_color();
      if (random(100) < 40) {
         switch (random_nonrepeat(3)) {
            case 0: sing(V1, "Foul called a-gainst $.", foul_player); break;
            case 1: sing(V1, "Foul called on $.", foul_player); break;
            case 2: sing(V1, "$ is charg-ed with a foul.", foul_player); break;
            default: assert(0);
         }
      } else {
         switch (random_nonrepeat(6)) { // 20+
            case 0: sing(V1, "Looks like a foul on the play, a-gainst $.", foul_player); break;
            case 1: sing(V1, "Whis-tle was blown. Looks like a foul a-gainst $.", foul_player); break;
            case 2: sing(V1, "There's a foul on the play. $ with the per-son-al foul.", foul_player); break;
            case 3: sing(V1, "A foul's been called. $ is charged with the foul.", foul_player); break;
            case 4: sing(V1, "There was a foul there, looks like it's $ with the foul.", foul_player); break;
            case 5: sing(V1, "How-ev-er, $ is being charged with a foul.", foul_player); break;
            default: assert(0);
         }
      }
   } else {
      // personal
      enable_color();
      if (this_event_time > 48*60 - 30) {
         // 4 instances
         switch (random_nonrepeat(5)) {
            case 0: sing(V1, "In-ten-tion-al foul by $.", foul_player); break;
            case 1: sing(V1, "In-ten-tion-al foul by $.", foul_player); break;
            case 2: sing(V1, "$ with the in-ten-tion-al foul.", foul_player); break;
            case 3: sing(V1, "Foul called on $.", foul_player); break;
            case 4: sing(V1, "$ makes an in-ten-tion-al foul.", foul_player); break;
            default: assert(0);
         }
      } else {
         if (first_free_throw) {
            // 3 cases
            if (team_fouls[tm] == 6 && once())
               sing(V1, "That foul from $ is sixth team foul for &, so $ will get the chance at two points.", foul_player, tm, who_was_fouled);
            else
               switch (random_nonrepeat(3)) {
                  case 0: sing(V1, "Per-son-al foul on $, but & [has|have] # team fouls, so $ will get two free throws.", foul_player,  tm,      team_fouls[tm],   who_was_fouled); break;
                  case 1: sing(V1, "$ is called with a per-son-al foul, and $ will shoot two since & [has|have] # team fouls.", foul_player,  who_was_fouled, tm, team_fouls[tm]); break;
                  case 2: sing(V1, "Foul on the play. Per-son-al foul for $, but $ will go to the foul line.", foul_player, who_was_fouled); break;
                     break;
                  default: assert(0);
               }
         } else {
            switch (random_nonrepeat(6)) { // 10
               case 0: sing(V1, "Foul on the play. per-son-al foul for $, so & gets the ball.", foul_player, !tm); break;
               case 1: sing(V1, "The whistle was blown there. per-son-al foul called on $_ and & gets control.", foul_player, !tm); break;
               case 2: sing(V1, "There was a foul on the play. $ with the per-son-al foul. &['s|'] ball.", foul_player, !tm); break;
               case 3: sing(V1, "Foul called. Looks like $ with the per-son-al foul. &['s|'] ball.", foul_player, !tm); break;
               case 4: sing(V1, "Looks like a foul here. Yes, per-son-al foul for $. & gets the ball.", foul_player, !tm); break;
               case 5: sing(V1, "But a foul is called on $. & gets control of the ball.", foul_player, !tm); break;
               default: assert(0);
            }
            tpossess(!tm, -1);
         }
      }
   }
   game_state.ball_alive = 0;

   who_was_fouled = 0;
   foul = 0;
   game_state.current_time = this_event_time;
}

int random_player(int tm, int player)
{
   int s;
   if (player == 0)
      s = random(5);
   else {
      s = slot(tm, player);
      s += 1+random(4);
      s %= 5;
   }
   return team_roster[tm][s];
}

int defender(int player)
{
   int tm = player < 0 ? TEAM_1 : TEAM_0;
   int s = slot(tm, player);
   return team_roster[!tm][s];
}

static int made_counts[20];
int missed_counts[20];

void took_shot(int type, int player, int assist, int made, int blocker, int flavor);
void free_shot(int player, int shotnum, int total_shots, int made);
void offensive_foul(int player, int loose);
void rebound(int team, int player);
void turnover(int type, int team, int player, int stealer);
void announce_lineup(int full_name);
void do_timeout(int official);

int last_shot_points;

void fouled_shot(int player, int fouled_by)
{
   int tm = player < 0 ? TEAM_1 : TEAM_0;
   if (random(100) < 50) {
      // gets the pass outside and works it in
      if (random(100) < 50) {
         if (random(100) < 20) {
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "Look-ing for some-one to pass to."); break;
               case 1: if (can_do_color() && once()) {
                          sing(V2, "The de-fense is doing a great job.");
                          disable_color();
                       }
                       break;
               case 2: sing(V1, "$ can't find an-y-one.", player); break;
               default: assert(0);
            }
         }
         // randomly drive to the layup
         enable_color();
         switch (random_nonrepeat(8)) {
            case 0: sing(V1, "% pump fakes and slips by his de-fen-der_ ", player); break;
            case 1: sing(V1, "A nice dou-ble-move from $ to get by $_ ", player, defender(player)); break;
            case 2: sing(V1, "$ gets a pick from $_ ", player, team_roster[tm][random_player(tm,player)]); break;
            case 3: sing(V1, "Gets by his def-end-er_ ", player, defender(player)); break;
            case 4: sing(V1, "% drives to the bas-ket_ ", player); break;
            case 5: sing(V1, "Drives to the bas-ket_ ", player); break;
            case 6: sing(V1, "% gets to the bas-ket_ ", player); break;
            case 7: sing(V1, "% works his way to the bas-ket_ ", player); break;
            default: assert(0);
         }
      } else {
         sing(V1, "% ", player);
      }
      enable_color();
      switch (random_nonrepeat(9)) {
         case 0: sing(V1, "tries the lay up and con-tact on the shot."); break;
         case 1: sing(V1, "goes for a lay up, but it does-n't go in, but $ was all o-ver him.", fouled_by); break;
         case 2: sing(V1, "lays it up, but can't get it in."); break;
         case 3: sing(V1, "tries a lay up through a crowd of de-fen-ders, but mis-ses."); break;
         case 4: sing(V1, "looks for a lay up, but fouled on the play."); break;
         case 5: sing(V1, "tries the lay up but he's knocked o-ver."); break;
         case 6: sing(V1, "lays the ball up, but not even close."); break;
         case 7: sing(V1, "puts it up_ no good, but con-tact on the play."); break;
         case 8: sing(V1, "puts the ball up, but misses it."); break;
         default: assert(0);
      }
   } else {
      enable_color();
      if (random_nonrepeat(100) < 55) {
         switch (random_nonrepeat(3)) {
            case 0: sing(V1, "Jump shot_ "); break;
            case 1: sing(V1, "$ with a shot_ ", player); break;
            case 2: sing(V1, "% takes a shot_ ", player); break;
            default: assert(0);
         }
      } else {
         switch(random_nonrepeat(9)) {
            case 0: sing(V1, "$ with the pick, $ shoots_ ", random_player(tm,player), player); break;
            case 1: sing(V1, "Shoots off a screen from $_ ", random_player(tm,player)); break;
            case 2: sing(V1, "Pump fakes and takes a shot_ ");  break;
            case 3: sing(V1, "$ with a wide open look_ ", player); break;
            case 4: sing(V1, "% shoots from the key_ ", player); break;
            case 5: sing(V1, "Shoots from down low_ ", player); break;
            case 6: sing(V1, "Fakes and shoots_ "); break;
            case 7: sing(V1, "With a jump shot from in close_ ");  break;
            case 8: sing(V1, "For two points_ "); break;
            default: assert(0);
         }
      }

      switch (random_nonrepeat(7)) {
         case 0: sing(V1, "looks like contact from $_ ", fouled_by); break;
         case 1: sing(V1, "bad shot, looks like contact from $_ ", fouled_by); return;
         case 2: sing(V1, "no good."); return; 
         case 3: sing(V1, "$ was all over him_ ", fouled_by); break;
         case 4: sing(V1, "a bad shot, but $ was all over him_ ", fouled_by); return;
         case 5:
         case 6:
            // don't mention the contact
            break;
         default: assert(0);
      }

      enable_color();
      if (random(100) < 70) {
         switch (random_nonrepeat(5)) {
            case 0: sing(V1, "the shot miss-es.");  break;
            case 1: sing(V1, "the shot miss-es.");
            case 2: sing(V1, "it's no good."); break;
            case 3: sing(V1, "the shot is no good."); break;
            case 4: sing(V1, "it miss-es."); break;
            default: assert(0);
         }
      } else {
         switch (random_nonrepeat(6)) {
            case 0: sing(V1, "it hits the rim_ no good."); break;
            case 1: sing(V1, "not even close to going in."); break;
            case 2: sing(V1, "it bounc-es off the rim."); break;
            case 3: sing(V1, "it bounc-es a-way off the rim."); break;
            case 4: sing(V1, "hits the front of the rim and bounces away."); break;
            case 5: sing(V1, "bounces high and doesn't drop.");
                    break;
          
            default: assert(0);
         }
      }
   }
}

int suppress_rebound;
void took_shot(int type, int player, int assist, int made, int blocker, int flavor)
{
   int tm = (player < 0) ? TEAM_1 : TEAM_0;
   process_foul(0);
   process_substitution();

   slot(tm, player); // assert that the player is active on the team
                     
   make_ball_alive(tm);
   suppress_rebound = 0; // don't let rebound-suppression drag

   if (this_event_time < game_state.possess_time+5 && game_state.possess_action_time == game_state.possess_time) {
      //printf("  Line: %d\n", cur_line);
      //sing(V1, "!!! FAST BREAK !!!");
   }
   tpossess(tm, 0);

   if (made)
      ++made_counts[flavor];
   else
      ++missed_counts[flavor];

   if (type == SHOT_layup) {
      if (assist) {
         run_play(assist, tm == 1 ? -0.75f : 0.75f, this_event_time-1);

         enable_color();
         switch (flavor) {
            case F_none:
               switch(random_nonrepeat(3))
               {
                  case 0: sing(V1, "% pas-ses it in to $ who puts it up_ it's good.", assist, player); break;
                  case 1: sing(V1, "% with a nice bounce pass, and $ lays it in.", assist, player); break;
                  case 2: sing(V1, "% pas-ses it inside, $ with the ball, he goes for the lay up, two points.", assist, player); break;
                  default: assert(0);
               }
               break;
            case F_reverse:
               sing(V1,"% pas-ses it to $_ and he makes a nice re-verse lay up from un-der-neath.", assist, player);
               break;
            case F_dunk:
               switch(random_nonrepeat(4))
               {
                  case 0: sing(V1, "% with the pass in-side to $, !he smash-es it in.", assist, player); break;
                  case 1: sing(V1, "% gives it to $, open lane, !he dunks it one hand-ed.", assist, player); break;
                  case 2: sing(V1, "% makes a pass to $, he james it for an easy two.", assist, player); break;
                  case 3: sing(V1, "% pas-ses it to $, !he's wide o-pen, and jams it in.", assist, player); break;
                  default: assert(0);
               }
               break;
            case F_alleyoop:
               switch(random_nonrepeat(2))
               {
                  case 0: sing(V1, "% throws an all-ey oop to $, good for two points.", assist, player); break;
                  case 1: sing(V1, "% gives the all-ey oop to $, who cash-es it in for two.", assist, player); break;
               }
               break;
            case F_alleyoop_dunk:
               sing(V1, "% puts up an all-ey oop to $, !and he jams it in.", assist, player);
               sing(V3, "!Wow.");
               sing(V2, "That was a beau-ti-ful play. $ was in a per-fect po-si-tion, and $ saw it plain as day.", player, assist);
               disable_color();
               break;
         }
      } else {
         {
            int handled = 0;
            // some layup types need special handling
            enable_color();
            switch (flavor) {
               case F_tip:
                  handled = 1;
                  if (made) {
                     suppress_rebound = -1;
                     switch (random_nonrepeat(2)) {
                        case 0: sing(V1, "$ tips it in, and it's good.", player); break;
                        case 1: sing(V1, "$ tips the ball, that's two points.", player); break;
                        default: assert(0);
                     }
                  } else {
                     switch (random_nonrepeat(2)) {
                        case 0: sing(V1, "$ tips the ball, but can't get the points.", player); break;
                        case 1: sing(V1, "$ tips the ball look-ing for two, but it's no good.", player); break;
                        default: assert(0);
                     }
                  }
                  break;
               case F_putback:
                  handled = 1;
                  if (made) {
                     switch (random_nonrepeat(2)) {
                        case 0: sing(V1, "% lays it back up, and that one goes.", player); break;
                        case 1: sing(V1, "He gets it to drop."); break;
                        default: assert(0);
                     }
                  } else {
                     sing(V1, "% lays it up, but can't get it to go in.", player);
                  }
                  break;
               case F_putback_dunk:
                  handled = 1;
                  if (made) {
                     suppress_rebound = -1;
                     sing(V1, "!$ with the put-back jam.", player);
                     sing(V2, "That was done with au-thor-i-ty.");
                     disable_color();
                  } else {
                     assert(0);
                  }
                  break;
            }

            if (!handled) {
               if (player == game_state.after_rebound && game_state.position == (tm ? -1 : 1)) {
                  //printf("***       Offensive rebound shots        ***\n");
               }
            }

            if (handled)
               ; // handled above
            else if (random(100) < 40) {
               run_play(player, tm == 1 ? -1.0f : 1.0f, this_event_time-3);
               // run_play will print that it was an inside post

               // gets the pass in the post

               enable_color();
               switch (random_nonrepeat(5)) {
                  case 0: sing(V1, "% push-es in and ", player); break;
                  case 1: sing(V1, "% works it in_ ", player); break;
                  case 2: sing(V1, "From the post, % push-es in_", player); break;
                  case 3: sing(V1, "% goes under the bas-ket, ", player); break;
                  case 4: sing(V1, "% works back, turns, ", player); break;
                  default: assert(0);
               }

               enable_color();
               if (made) {
                  switch (random_nonrepeat(6)) {
                     case 0: sing(V1, "lays it up for an eas-y two points."); break;
                     case 1: sing(V1, "lays it up and it goes right in."); break;
                     case 2: sing(V1, "con-tes-ted shot_ the lay-up is good."); break;
                     case 3: sing(V1, "lays the ball up and it's good."); break;
                     case 4: sing(V1, "drops in an eas-y lay-up for two points."); break;
                     case 5: sing(V1, "and he lays it up for two."); break;
                     default: assert(0);
                  }
               } else {
                  switch (random_nonrepeat(9)) {
                     case 0: sing(V1, "tries the lay up and mis-ses."); break;
                     case 1: sing(V1, "goes for a lay up, but it does-n't go in."); break;
                     case 2: sing(V1, "lays it up, but it does-n't drop."); break;
                     case 3: sing(V1, "tries a lay up, but it mis-ses."); break;
                     case 4: sing(V1, "looks for a lay up, but no luck."); break;
                     case 5: sing(V1, "tries the lay up but can't get it to go in."); break;
                     case 6: sing(V1, "lays the ball up, but it's no good."); break;
                     case 7: sing(V1, "puts it up_ no good."); break;
                     case 8: sing(V1, "puts the ball up, but misses it."); break;
                     default: assert(0);
                  }
               }
            } else {
               int any=0;

               run_play(player, tm == 1 ? -0.8f : 0.8f, this_event_time-3);

               // gets the pass outside and works it in
               if (random(100) < 20) {
                  any = 1;
                  switch (random_nonrepeat(3)) {
                     case 0: sing(V1, "Look-ing for some-one to pass to."); break;
                     case 1: if (can_do_color() && once())
                                sing(V2, "The de-fense is doing a great job.");
                             break;
                     case 2: sing(V1, "$ can't find an-y-one.", player); break;
                     default: assert(0);
                  }
               }
               // randomly drive to the layup
               if (flavor != F_driving && flavor != F_driving_finger_roll) {
                  any = 1;
                  switch (random_nonrepeat(8)) {
                     case 0: sing(V1, "% pump fakes and slips by his de-fen-der_ ", player); break;
                     case 1: sing(V1, "A nice dou-ble-move from $ to get by $_ ", player, defender(player)); break;
                     case 2: sing(V1, "$ gets a pick from $_ ", player, team_roster[tm][random_player(tm,player)]); break;
                     case 3: sing(V1, "Gets by his def-end-er_ ", player, defender(player)); break;
                     case 4: sing(V1, "% drives to the bas-ket_ ", player); break;
                     case 5: sing(V1, "Drives to the bas-ket_ ", player); break;
                     case 6: sing(V1, "% gets to the bas-ket_ ", player); break;
                     case 7: sing(V1, "% works his way to the bas-ket_ ", player); break;
                     default: assert(0);
                  }
               }

               if (!any)
                  sing(V1, "% ", player);

               if (made) {
                  switch (flavor) {
                     case F_running:
                     case F_none:
                        switch (random_nonrepeat(6)) {
                           case 0: sing(V1, "lays it up for an eas-y two points."); break;
                           case 1: sing(V1, "lays it up and it goes right in."); break;
                           case 2: sing(V1, "con-tes-ted shot_ the lay-up is good."); break;
                           case 3: sing(V1, "lays the ball up and it's good."); break;
                           case 4: sing(V1, "drops in an eas-y lay-up for two points."); break;
                           case 5: sing(V1, "he's wide open as he lays it up for two."); break;
                           default: assert(0);
                        }
                        break;
                     case F_putback:
                     case F_putback_dunk:
                     case F_tip:
                        assert(0);
                     case F_dunk:
                        switch (random_nonrepeat(3)) {
                           case 0: sing(V1, "!is wide open and he slams it in for two."); break;
                           case 1: sing(V1, "!and slams it in to the bas-ket."); break;
                           case 2: sing(V1, "!and dunks it for an easy two."); break;
                        }
                        break;
                     case F_driving:
                        switch (random_nonrepeat(2)) {
                           case 0: sing(V1, "drives in to the basket, lays it up, it's good."); break;
                           case 1: sing(V1, "drives to the basket and lays it in."); break;
                           default: assert(0);
                        }
                        break;
                     case F_driving_finger_roll:
                        sing(V1, "drives to the bas-ket and lays it up and in.");
                        sing(V2, "He used a fing-er roll there, you don't see that a lot.");
                        disable_color();
                        break;

                     case F_reverse:             assert(0);      
                     case F_alleyoop:            assert(0);
                     case F_alleyoop_dunk:       assert(0);           
                     default:
                        assert(0);
                  }
               } else {
                  switch (flavor) {
                     case F_none:
                        if (blocker) {
                           switch (random_nonrepeat(4)) {
                              case 0: sing(V1, "goes for a lay up_ !blocked by $.", blocker); break;
                              case 1: sing(V1, "!tries the lay up but he's blocked by $.", blocker); break;
                              case 2: sing(V1, "looks for a lay up, but $ with the block.", blocker); break;
                              case 3: sing(V1, "tries to put the ball up but it's blocked by $.", blocker); break;
                              default: assert(0);
                           }
                           // 7 instances
                        } else {
                           switch (random_nonrepeat(9)) {
                              case 0: sing(V1, "tries the lay up and mis-ses."); break;
                              case 1: sing(V1, "goes for a lay up, but it does-n't go in."); break;
                              case 2: sing(V1, "lays it up, but it does-n't drop."); break;
                              case 3: sing(V1, "tries a lay up, but it mis-ses."); break;
                              case 4: sing(V1, "looks for a lay up, but no luck."); break;
                              case 5: sing(V1, "tries the lay up but can't get it to go in."); break;
                              case 6: sing(V1, "lays the ball up, but it's no good."); break;
                              case 7: sing(V1, "puts it up_ no good."); break;
                              case 8: sing(V1, "puts the ball up, but misses it."); break;
                              default: assert(0);
                           }
                        }
                        break;
                     case F_dunk:
                        sing(V1, "!goes for the dunk_ and he mis-ses it.", player);
                        sing(V2, "That's got-ta be em-bar-rass-ing");
                        disable_color();
                        break;

                     case F_tip:
                     case F_putback:
                     case F_putback_dunk:        assert(0);          
                     case F_driving:             assert(0);     
                     case F_driving_finger_roll: assert(0);                 
                     case F_alleyoop:            assert(0);
                     case F_alleyoop_dunk:       assert(0);           
                     case F_reverse:             assert(0);      
                     case F_running:             assert(0);      
                     case F_running_bank:        assert(0);          
                     default:
                        assert(0);
                  }
               }
            }
         }
      }
   } else {
      int said_shot=0;
      if (assist) {
         run_play(assist, tm ? -0.8f : 0.8f, this_event_time - random(4));
         pass_to(tm, player, tm ? -0.8f : 0.8f, 0);
      } else {
         if (type == SHOT_3pt)
            run_play(player, tm ? -0.7f : 0.7f, this_event_time);
         else
            run_play(player, tm ? -0.85f : 0.85f, this_event_time);
      }
      // at this point the player has the ball and can take a jump shot
      if (type == SHOT_3pt) {
         enable_color();
         switch (random_nonrepeat(7)) {
            case 0: sing(V1, "!For three_  "); break;
            case 1: sing(V1, "!% shoots for three_  ", player); break;
            case 2: sing(V1, "!Fakes his de-fen-der and goes for three_ "); break;
            case 3: sing(V1, "!% tries for three_ ", player); break;
            case 4: sing(V1, "!% look-ing for three_ ", player); break;
            case 5: sing(V1, "!Try-ing for three points_ "); break;
            case 6: sing(V1, "!Tries a three point shot_ "); said_shot=1; break;
            default: assert(0);
         }
      } else {
         enable_color();
         switch (flavor) {
            case F_none: 
            case F_bank:
               if (blocker) {
                  switch (random_nonrepeat(5)) {
                     case 0: sing(V1, "% goes for a shot, !but it's blocked by $.", player, blocker); break;
                     case 1: sing(V1, "% goes up_ !no, it's blocked. $ with the block.", player, blocker); break;
                     case 2: sing(V1, "% fakes, shoots. $ blocks the shot.", player, blocker); break;
                     case 3: if (once())
                                sing(V1, "Off a pick by $, ", random_player(tm,player));
                             sing(V1, "$ takes a shot_ !$ blocks it.", player, blocker);
                             break;
                     case 4: sing(V1, "% tries a shot, !denied. Blocked by $.", player, blocker);
                        break;
                     default: assert(0);
                  }
                  
               } else {
                  if (random_nonrepeat(100) < 45) {
                     switch (random_nonrepeat(3)) {
                        case 0: sing(V1, "Jump shot_ "); said_shot=1; break;
                        case 1: sing(V1, "$ with a shot_ ", player); said_shot=1; break;
                        case 2: sing(V1, "% takes a shot_ ", player); said_shot=1; break;
                        default: assert(0);
                     }
                  } else {
                     switch(random_nonrepeat(10)) {
                        // @TODO more shot descriptions   low-priority
                        case 0: sing(V1, "$ with the pick, $ shoots_ ", random_player(tm,player), player); break;
                        case 1: sing(V1, "Shoots off a screen from $_ ", random_player(tm,player)); break;
                        case 2: sing(V1, "Pump fakes and takes a shot_ "); said_shot = 1;  break;
                        case 3: sing(V1, "$ with a wide open look_ ", player); break;
                        case 4: sing(V1, "% shoots from the key_ ", player); break;
                        case 5: sing(V1, "Shoots from down low_ ", player); break;
                        case 6: sing(V1, "Fakes and shoots_ "); break;
                        case 7: sing(V1, "With a jump shot from in close_ ");  said_shot = 1; break;
                        case 8: sing(V1, "For two points_ "); break;
                        case 9: sing(V1, "$, looking for two_ ", player); break;
                        case 10:sing(V1, "For two_ "); break;
                        default: assert(0);
                     }
                  }
               }
               break;
            case F_running_bank:
               switch (random_nonrepeat(2)) {
                  case 0: sing(V1, "$ with a run-ning jump shot_ ", player); said_shot = 1; break;
                  case 1: sing(V1, "$ takes a run-ning jump shot_ ", player); said_shot = 1; break;
               }
               break;
            case F_turnaround:
               switch (random_nonrepeat(6)) {
                  case 0: sing(V1, "% shoots a turn around jump-er_ ", player); break;
                  case 1: sing(V1, "$ with the turn around jump-er_ ", player); break;
                  case 2: sing(V1, "The turn a-round jump-er_ "); break;
                  case 3: sing(V1, "A turn a-round jump-er_ "); break;
                  case 4: sing(V1, "Now a turn a-round jump-er_ "); break;
                  case 5: sing(V1, "A turn a-round jump-er from $_ ", player); break;
                  default: assert(0);
               }
               break;
            case F_fadeaway:
               switch (random_nonrepeat(3)) {
                  case 0: sing(V1, "$ with a fade a-way jump-er_ ", player); break;
                  case 1: sing(V1, "$ with the fade_ ", player); break;
                  case 2: sing(V1, "% fades and shoots_ ", player); break;
                  default: assert(0);
               }
               break;
            case F_hook:
               switch (random_nonrepeat(12)) {
                  case 0: sing(V1, "% tries a hook_ ", player); break;
                  case 1: sing(V1, "Now a hook shot_ "); said_shot = 1; break;
                  case 2: sing(V1, "Now $ with a hook_ ", player); break;
                  case 3: sing(V1, "% goes for the hook_ ", player); break;
                  case 4: sing(V1, "Try-ing the hook_ "); break;
                  case 5: sing(V1, "The hook shot_ "); break;
                  case 6: if (last_player == player)
                             sing(V1, "A hook shot_ ");
                          else
                             sing(V1, "A hook shot from $_ ", player);
                          break;
                  case 7: if (last_player == player)
                             sing(V1, "Puts up a hook_ ");
                          else
                             sing(V1, "$ puts up a hook_ ", player);
                          break;
                  case 8: sing(V1, "$ puts up a hook_ ", player); break;
                  case 9: sing(V1, "$ with the hook_ ", player); break;
                  case 10:sing(V1, "An un-ex-pect-ed hook from $_ ", player); break;
                  case 11: if (last_player == player)
                             sing(V1, "With a hook_ ");
                          else
                             sing(V1, "$, with a hook_ ", player);
                          break;
                  default: assert(0);
               }
               break;
            case F_turnaround_hook:
               switch (random_nonrepeat(3)) {
                  case 0: sing(V1, "% with a turn a-round hook_ ", player); break;
                  case 1: sing(V1, "% tries a turn a-round hook_ ", player); break;
                  case 2: sing(V1, "A turn a-round hook_ "); break;
                  default: assert(0);
               }
               break;

            default:
               assert(0);           
         }
      }
      if (blocker) {
         ; // do nothing
      } else if (made) {
         enable_color();
         if (flavor == F_bank || flavor == F_running_bank) {
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "off the glass and in."); break;
               case 2: sing(V1, "off the glass and it goes right in."); break;
               case 1: if (said_shot)
                          sing(V1, "banks it_ and it's good.");
                       else
                          sing(V1, "bank shot_ and it's good.");
                       break;
               default: assert(0);
            }
         } else if (random(100) < 55) {
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "and makes it."); break;
               case 1: sing(V1, "and it's good."); break;
               case 2: sing(V1, "and it goes in."); break;
               default: assert(0);
            }
         } else {
            switch (random_nonrepeat(12)) {
               case 0: sing(V1, "and it goes right in."); break;
               case 1: sing(V1, "it hits the rim and goes in."); break;
               case 2: sing(V1, "it rolls around the rim but goes in."); break;
               case 3: sing(V1, "right on the mon-ey.");
                       if (once()) {
                          sing(V2, "That's cash mon-ey right there.");
                          disable_color();
                       }
                       break;
               case 4: sing(V1, "takes a high bounce but goes in."); break;
               case 5: sing(V1, "that's all net."); break;
               case 6: if (said_shot)
                          sing(V1, "and makes it.");
                       else
                          sing(V1, "and makes the shot.");
                       break;
               case 7: sing(V1, "and puts it in the bas-ket."); break;
               case 8: sing(V1, "he nails it."); break;
               case 9: sing(V1, "and it's right in the basket."); break;
               case 10:sing(V1, "hits the far side of the rim and drops in."); break;
               case 11:sing(V1, "hits the near side of the rim but goes in."); break;
               default: assert(0);
            }
         }
      } else {
         enable_color();
         if (flavor == F_bank || flavor == F_running_bank) {
            switch (random_nonrepeat(4)) {
               case 0: if (said_shot)
                          sing(V1, "banks it_ and it bounc-es off the rim.");
                       else
                          sing(V1, "bank shot_ and it bounc-es off the rim.");
                       break;
               case 1: sing(V1, "off the glass, no good."); break;
               case 2: sing(V1, "hits the back-board but does-n't drop."); break;
               case 3: sing(V1, "off the glass, but miss-es."); break;
               default: assert(0);
            }
         } else if (random(100) < 70) {
            switch (random_nonrepeat(5)) {
               case 0: sing(V1, "miss-es.");  break;
               case 1: if (said_shot)
                          sing(V1, "it miss-es.");
                       else
                          sing(V1, "the shot miss-es.");
                       break;
               case 2: sing(V1, "it's no good."); break;
               case 3: if (said_shot)
                           sing(V1, "it's no good.");
                       else
                           sing(V1, "the shot's no good.");
                       break;
               case 4: sing(V1, "it miss-es."); break;
               default: assert(0);
            }
         } else {
            switch (random_nonrepeat(6)) {
               case 0: sing(V1, "it hits the rim_ no good."); break;
               case 1: sing(V1, "not even close to going in."); break;
               case 2: sing(V1, "bounc-es off the rim."); break;
               case 3: sing(V1, "bounc-es a-way off the rim."); break;
               case 4: sing(V1, "hits the front of the rim and bounces away."); break;
               case 5: if (random(100) < 5 && once())
                          sing(V1, "it misses the basket entirely.");
                       else
                          sing(V1, "bounces high and doesn't drop.");
                       break;
                
               default: assert(0);
            }
         }
      }
   }

   add_plain(SP_blocks , blocker, 1);
   add_plain(SP_assists, assist , 1);

   switch (type) {
      case SHOT_layup:
         add_streakable(SS_layup, player, made, 2);
         add_streakable(SS_fg   , player, made, 2);
         add_streakable(SS_total, player, made, 2);
         break;
      case SHOT_3pt:
         add_streakable(SS_f3   , player, made, 3);
         add_streakable(SS_fg   , player, made, 3);
         add_streakable(SS_total, player, made, 3);
         break;
      case SHOT_jump:
         add_streakable(SS_jump , player, made, 2);
         add_streakable(SS_fg   , player, made, 2);
         add_streakable(SS_total, player, made, 2);
         break;
   }

   if (made)
      add_points(tm, (type == SHOT_3pt ? 3 : 2));


   //  how the player's doing at making a given shot type
   //  

   if (made) {
      if (can_do_color()) {
         #if 0
         // @TODO: "he" should be player name if the score was announced
         if (stats[player].made == 1) {
            if (stats[player].attempts > 4)
               sing(V2, "That's the first shot he's made after # misses earlier.", stats[player].attempts-1);
            else if (stats[player].attempts)
               if (type == SHOT_3pt)
                  sing(V2, "For his first shot of the night he's sunk a three point-er.");
               else            
                  sing(V2, "That's his first shot of the night.");
            else
               sing(V2, "That's the first shot he's made.");
         } else {
            if (type == SHOT_3pt && stats[player].f3_attempts > 1) {
               sing(V2, "He's # for # so far to-night for three point-ers.", stats[player].f3_made, stats[player].f3_attempts);
            } else if (type == SHOT_jump && stats[player].jump_attempts > 2) {
               sing(V2, "He's # for # so far to-night from the field.", stats[player].jump_made, stats[player].jump_attempts);
            } else {
               sing(V2, "He's # for # so far to-night.", stats[player].made, stats[player].attempts);
            }
         }
         disable_color();
         #endif
      }
   }


   game_state.current_time = this_event_time;
   game_state.possess_action_time = this_event_time;
   game_state.position = (player < 0 ? -1.0f : 1.0f);
   if (made) {
      set_tpossess(!tm, 0);
      last_shot_points = (type == SHOT_3pt ? 3 : 2);
      game_state.ball_alive = 0;
   } else
      last_shot_points = 0;
   last_shot_time = this_event_time;
   game_state.after_rebound = 0;
   assert(!made || blocker == 0);
   //printf("Player %d took '%c' assisted by %d: %s\n", player, "j3Lda"[type], assist, made ? "made" : "missed");
}

int last_free_made;
int did_announce_last_free;

void free_shot(int player, int shotnum, int total_shots, int made)
{
   int n;
   int team = player < 0 ? TEAM_1 : TEAM_0;
   int announce=0;

   who_was_fouled = player;
   process_foul(shotnum == 1);
   who_was_fouled = 0;
   process_substitution();

   game_state.ball_alive = 0;

   set_tpossess(team, -1);

   if (made)
      n = random_nonrepeat(3);
   else
      n = random_nonrepeat(3);

   if (shotnum == 1) {
      enable_color();
      switch (random_nonrepeat(4)) {
         case 0: sing(V1, "$ is ready for the free throw. ", player); break;
         case 1: sing(V1, "$ at the free throw line. ", player); break;
         case 2: if (total_shots == 1)
                    sing(V1, "$ at the line for the free throw.", player);
                 else
                    sing(V1, "$ at the line for # free throws.", player, total_shots);
                 break;
         case 3: if (total_shots == 1)
                    sing(V1, "$ ready to shoot a free throw.", player);
                 else
                    sing(V1, "$ ready to shoot # free throws.", player, total_shots);
                 break;
         default: assert(0);
      }

      if (total_shots == 1) {
         if (last_shot_points == 3)
            sing(V1, "%'s looking to add a fourth point_ ", player);
         else {
            assert(last_shot_points == 2);
            sing(V1, "%'s looking to turn it into three_ ", player);
         }
         enable_color();
         if (made)
            switch (n) {
               case 0: sing(V1, "and makes it."); break;
               case 1: sing(V1, "and it goes in."); break;
               case 2: sing(V1, "and it's good."); break;
               default: assert(0);
            }
         else
            switch (n) {
               case 0: sing(V1, "misses it."); break;
               case 1: sing(V1, "it doesn't drop."); break;
               case 2: sing(V1, "but it's no good."); break;
            }
      } else {
         enable_color();
         if (made)
            switch (n) {
               case 0: sing(V1, "makes the first one."); break;
               case 1: sing(V1, "and the first one goes in."); break;
               case 2: sing(V1, "and the first one is good."); break;
               default: assert(0);
            }
         else
            switch (n) {
               case 0: sing(V1, "misses the first one."); break;
               case 1: sing(V1, "the first one doesn't drop."); break;
               case 2: sing(V1, "but the first one is no good."); break;
            }
      }
   } else {
      enable_color();
      if (last_free_made)
         if (made)
            switch (n) {
               case 0: sing(V1, "And he makes the sec-ond as well."); break;
               case 1: sing(V1, "And that one goes in too."); break;
               case 2: sing(V1, "And that one is also good."); break;
               default: assert(0);
            }
         else
            switch (n) {
               case 0: sing(V1, "But he can't sink the sec-ond."); break;
               case 1: sing(V1, "But the sec-ond one does-n't drop."); break;
               case 2: sing(V1, "But the sec-ond shot is no good."); break;
            }
      else
         if (made)
            switch (n) {
               case 0: sing(V1, "But he makes the sec-ond."); break;
               case 1: sing(V1, "But the sec-ond one goes in."); break;
               case 2: sing(V1, "But the sec-ond one is good."); break;
               default: assert(0);
            }
         else
            switch (n) {
               case 0: sing(V1, "And he can't sink the sec-ond one ei-ther."); break;
               case 1: sing(V1, "And the sec-ond one also does-n't drop."); break;
               case 2: sing(V1, "And no good with the sec-ond shot."); break;
            }
   }


   if (shotnum == total_shots) {
      if (total_shots == 2) {
         if (made || (last_free_made && !did_announce_last_free))
            force_announce_points = 1;
      } else {
         if (made)
            force_announce_points = 1;
      }
   }

   if (made)
      add_points(team, 1);
   else if (force_announce_points)
      points_announce(V2, 0);

   force_announce_points = 0;
   did_announce_last_free = 0;
   if (made && has_announced_score())
      did_announce_last_free = 1;

   game_state.position = (player < 0 ? -1.0f : 1.0f);
   game_state.current_time = this_event_time;

   add_streakable(SS_ft, player, made, 1);
   add_streakable(SS_total, player, made, 1);

   if (shotnum == total_shots) {
      if (made) {
         set_tpossess(player < 0 ? TEAM_0 : TEAM_1, 0);
      } else {
         force_announce_flag = 1;
         // should be followed by an explicit rebound
         game_state.ball_alive = 1;
      }
   }
   game_state.after_rebound = 0;
   last_free_made = made;

   // spurious rebounds between free throws should be ignored
   if (shotnum == 1 && total_shots == 2)
      suppress_rebound = 1;
   if (shotnum == 2 && total_shots == 2)
      suppress_rebound = 0;
}

void regular_foul(int type, int player)
{
   if (subcount && this_event_time > last_sub_time)
      process_substitution();
   foul = type;
   foul_player = player;
   foul_time = this_event_time;
   game_state.ball_alive = 0;
}

int off_count;
void offensive_foul(int player, int loose)
{
   process_foul(0);
   process_substitution();

   enable_color();
   if (loose) {
      sing(V1, "There's a foul, plen-ty of con-tact. It's $. $ gets a loose ball foul.", player, player);
   } else {
      switch (off_count) {
         case 0:
            run_play(player, 0.6f, this_event_time);
            sing(V1, "There's a whis-tle on the play. Looks like charging by $.", player);
            break;
         case 1:
            sing(V1, "There's a foul, looks like $ with an illegal screen.", player);
            break;
         case 2:
            assert(0); // you'll need to add more random variation instead of hard-coding to this game
      }
      ++off_count;
   }
   
   foul_time = this_event_time;
   tpossess(player < 0 ? TEAM_1 : TEAM_0, stb_rand() & 1);
   add_plain(SP_fouls, player, 1);
   game_state.current_time = this_event_time;
   game_state.after_rebound = 0;
   game_state.ball_alive = 0;
}

void rebound(int team, int player)
{
   int announce = -1;
   int offensive;
   if (suppress_rebound > 0)
      return;

   if (suppress_rebound < 0) {
      suppress_rebound = 0;
      if (!game_state.possess_team == (player < 0 ? TEAM_1 : TEAM_0))
         add_plain(SP_off_rebounds, player, 1);
      else
         add_plain(SP_def_rebounds, player, 1);
      add_plain(SP_rebounds, player, 1);
      return;
   }

   offensive = (game_state.possess_team == team);
   process_foul(0);
   process_substitution();

   assert(game_state.ball_alive);
   if (player) {
      enable_color();
      if (offensive)
         if (player == game_state.possess_player && random(100) < 50)
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "Gets the ball back."); break;
               case 1: sing(V1, "Gets his own rebound."); break;
               case 2: sing(V1, "$ gets the ball again.", player); break;
               default: assert(0);
            }
         else
            switch (random_nonrepeat(4)) {
               case 0: sing(V1, "$ with the of-fen-sive re-bound.", player); break;
               case 1: sing(V1, "$ gets the of-fen-sive re-bound.", player); break;
               case 2: sing(V1, "Of-fen-sive re-bound by $.", player); break;
               case 3: sing(V1, "Of-fen-sive re-bound from $.", player); break;
               default :assert(0);
            }
      else {
         if (random(100) < 40)
            sing(V1, "$ with the re-bound.", player);
         else
            switch (random_nonrepeat(5)) {
               case 0: sing(V1, "$ with a re-bound.", player); break;
               case 1: sing(V1, "$ comes down with the ball.", player); break;
               case 2: sing(V1, "$ gets the ball.", player); break;
               case 3: sing(V1, "$ grabs the ball.", player); break;
               case 4: sing(V1, "$ pulls down the ball.", player); break;
               default: assert(0);
            }
         announce = 1;
      }
   } else if (offensive) {
      enable_color();
      if (random(100) < 60)
         sing(V1, "& get[s|] the ball back.", team);
      else
         switch (random_nonrepeat(3)) {
            case 0: sing(V1, "The ball goes out of bounds. Last touched by $, so & get it back.", random_player(!team,0), team);
                    game_state.position = 0.75;
                    game_state.ball_alive = 0;
                    break;
            case 1: sing(V1, "Two play-ers from & come down with the ball.", team); break;
            case 2: sing(V1, "It's still &'s ball.", team); break;
            default: assert(0);
         }
   } else {
      enable_color();
      sing(V1, "& [takes|take] possession.", team);
   }
   game_state.position = (game_state.possess_team == TEAM_0 ? 1.0f : -1.0f);
   if (offensive)
      add_plain(SP_off_rebounds, player, 1);
   else
      add_plain(SP_def_rebounds, player, 1);
   add_plain(SP_rebounds, player, 1);
   tpossess(team, announce);

   if (can_do_color()) {
      color_statistic *list = NULL;
      ++color_timestamp;
      if (offensive)
         list = color_add_analysis(list, SP_off_rebounds, player);
      else
         list = color_add_analysis(list, SP_def_rebounds, player);
      list = color_add_analysis(list, SP_rebounds, player);
      color_report_analysis(list);
   }

   game_state.current_time = this_event_time;
   game_state.possess_action_time = this_event_time;
   game_state.possess_player = player;
   if (player)
      game_state.after_rebound = player;
   else
      game_state.after_rebound = 0;
}

void turnover(int type, int team, int player, int stealer)
{
   int run_to;
   process_foul(0);
   process_substitution();
   make_ball_alive(team);
   run_to = player;
   if (type == TURNOVER_out_of_bounds || type == TURNOVER_3sec)
      run_to = random_player(team, player);
   run_play(run_to, team < 0 ? -0.6f : 0.6f, this_event_time);
   enable_color();
   if (stealer)
      // ~10
      switch (random_nonrepeat(10)) {
         case 0: sing(V1, "$ steals the ball from $.", stealer, player); break;
         case 1: sing(V1, "$ steals the ball.", stealer); break;
         case 2: sing(V1, "Stolen by $.", stealer); break;
         case 3: sing(V1, "$ with the steal.", stealer); break;
         case 4: sing(V1, "$ with a steal.", stealer); break;
         case 5: sing(V1, "A steal by $.", stealer); break;
         case 6: sing(V1, "And $ makes a steal.", stealer); break;
         case 7: sing(V1, "$ steals it from $.", stealer, player); break;
         case 8: sing(V1, "And $ with the steal.", stealer); break;
         case 9: sing(V1, "$ gets a steal.", stealer); break;
      }
   else {
      ++hack_count2[type];
      switch (type) {
         case TURNOVER_traveling:      // 5
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "Tra-vel-ing is called on $.", player); break;
               case 1: sing(V1, "$ is called for tra-vel-ing.", player); break;
               case 2: sing(V1, "And there's the call. Tra-vel-ing.", player); break;
               default: assert(0);
            }
            break;
         case TURNOVER_badpass:        // 3
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "$ makes a bad pass, out of bounds.", player); break;
               case 1: sing(V1, "$ throws to $ but he's off the mark and it goes out of bounds.", player, random_player(team,player)); break;
               case 2: sing(V1, "$ look-ing for $ but throws it out of bounds.", player, random_player(team,player)); break;
               default: assert(0);
            }
            break;
         case TURNOVER_foul:           // 2
            // this has already been printed in offensive_foul()
            break;
         case TURNOVER_out_of_bounds:  // 1
            sing(V1, "$ pass-es to $. $ los-es con-trol and it goes out of bounds.", game_state.possess_player, player, player);
            break;
         case TURNOVER_palming:        // 2
            switch (random_nonrepeat(2)) {
               case 0: sing(V1, "And $ gets charged with palm-ing.", player); break;
               case 1: sing(V1, "Whis-tle on the play. It's palm-ing against $.", player); break;
               default: assert(0);
            }
            break;
         case TURNOVER_3sec:           // 1
            sing(V1, "And there's a whis-tle. Three sec-ond vi-o-la-tion by $.", player);
            break;
      }
   }
   add_plain(SP_turnovers, player , 1);
   add_plain(SP_steals   , stealer, 1);
   tpossess(!team, 1);
   game_state.current_time = this_event_time;
   game_state.possess_player = stealer;
   game_state.after_rebound = 0;
}

void do_timeout(int team)
{
   if (team < 0)
      switch (random_nonrepeat(2)) {
         case 0: sing(V1, "There's an of-fi-cial time out."); break;
         case 1: sing(V1, "That's the of-fi-cial time out."); break;
      }
   else
      sing(V1, "& [calls|call] a time out.", team);
   game_state.position = 0;
   game_state.ball_alive = 0;
}

void announce_lineup(int full_name)
{
   int i,j,k,style, style2;
   int nametype = full_name ? PLAYERNAME_full : PLAYERNAME_short;

   force_team_mode = stb_rand() & 1;

   j = stb_rand() & 1;

   style = random_nonrepeat(3);
   style2 = random_nonrepeat(3);

   if (full_name)
      sing(V1, "To-night's start-ing line up.");

   for (i=0; i < 2; ++i, j = !j) {
      if (i == 0) {
         switch (style) {
            case 0: sing(V1, "For &, we have ", j); break;
            case 1: sing(V1, "For &, ", j); break;
            case 2: sing(V1, "& will be ", j); break;
            default: assert(0);
         }
      } else {
         switch (style) {
            case 0: sing(V1, "And for &, we have ", j); break;
            case 1: sing(V1, "Whereas for &, ", j); break;
            case 2: sing(V1, "For &, we have ", j); break;
            default: assert(0);
         }
      }
      for (k=0; k < 5; ++k) {
         char buffer[256];
         if (k == 4) {
            sing(V1, "And ");
         }
         if (full_name) {
            switch (style2) {
               case 0: sprintf(buffer, "$$ at %s%c ", positions[k], ",."[k==4]); break;
               case 1: sprintf(buffer, "at %s, $$. ", positions[k]); break;
               case 2: sprintf(buffer, "%s, $$.", positions[k]); break;
               default: assert(0);
            }
         } else {
            switch (style2) {
               case 0: sprintf(buffer, "$ at %s%c ", positions[k], ",."[k==4]); break;
               case 1: sprintf(buffer, "at %s, $. ", positions[k]); break;
               case 2: sprintf(buffer, "%s, $.", positions[k]); break;
               default: assert(0);
            }
         }
         sing(V1, buffer, team_roster[j][k]);
      }
   }
   enable_color();

   force_team_mode = -1;
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
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Jump Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_none);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Turnaround Jump Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_turnaround);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Turnaround Hook Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_turnaround_hook);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Jump Bank Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_bank);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Jump Hook Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_hook);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Fade Away Jumper Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_fadeaway);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Reverse Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_reverse);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_none);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Tip Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_tip);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Dunk Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_dunk);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Running Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_running);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Running Bank shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_running_bank);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Putback Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_putback);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Putback Dunk Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_putback_dunk);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Driving Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_driving);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Driving Finger Roll Layup Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_driving_finger_roll);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Alley Oop Layup Shot: made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_alleyoop);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Alley Oop Layup shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_alleyoop);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);
      } else if (3 == sscanf(text, "#%d Alley Oop Dunk Shot: Made (%d PTS) %d", v+0,v+1, v+8)) {
         took_shot(SHOT_layup, v[0], assist, 1, blocker, F_alleyoop_dunk);
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[1]);

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
         assert(stats[v[0]][0].streakable[SS_total].game.points == v[3]);

      // foul
      } else if (3 == sscanf(text, "#%d Foul:Personal (%d PF) %d", v+0, v+1, v+8)) {
         regular_foul(FOUL_personal, v[0]);
      } else if (3 == sscanf(text, "#%d Foul:Shooting (%d PF) %d", v+0, v+1, v+8)) {
         regular_foul(FOUL_shooting, v[0]);
      } else if (3 == sscanf(text, "#%d Foul:Loose Ball (%d PF) %d", v+0, v+1, v+8)) {
         offensive_foul(v[0], 1);
      } else if (3 == sscanf(text, "#%d Foul:Offensive (%d PF) %d", v+0, v+1, v+8)) {
         offensive_foul(v[0], 0);
      } else if (2 == sscanf(text, "#%d Violation:Defensive Goaltending  %d", v+0,v+8)) {

      // rebounds
      } else if (1 == sscanf(text, "Team Rebound %d", v+8)) {
         rebound(t, 0);
      } else if (4 == sscanf(text, "#%d Rebound (Off:%d Def:%d) %d", v+0,v+1,v+2, v+8)) {
         rebound(t, v[0]);
         //assert(stats[v[0]].off_rebounds == v[1]);
         //assert(stats[v[0]].def_rebounds == v[2]);

      // turnovers
      } else if (3 == sscanf(text, "#%d Turnover:Traveling (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_traveling, t, v[0], 0);
         //assert(stats[v[0]].turnovers == v[1]);
      } else if (3 == sscanf(text, "#%d Turnover:Foul (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_foul     , t, v[0], 0);
         //assert(stats[v[0]].turnovers == v[1]);
      } else if (5 == sscanf(text, "#%d Turnover:Bad Pass (%d TO) Steal:#%d (%d ST) %d", v+0,v+1,v+2,v+3,v+8)) {
         turnover(TURNOVER_badpass  , t, v[0], v[2]);
         //assert(stats[v[0]].turnovers == v[1]);
         //assert(stats[v[2]].steals    == v[3]);
      } else if (3 == sscanf(text, "#%d Turnover:Bad Pass (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_badpass  , t, v[0], 0);
         //assert(stats[v[0]].turnovers == v[1]);
      } else if (3 == sscanf(text, "#%d Turnover:Out of Bounds Lost Ball Turnover (%d TO) %d", v+0,v+1, v+8)) {
         turnover(TURNOVER_out_of_bounds, t, v[0], 0);
         //assert(stats[v[0]].turnovers == v[1]);
      } else if (3 == sscanf(text, "#%d Turnover:Palming Turnover (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_palming, t, v[0], 0);
         //assert(stats[v[0]].turnovers == v[1]);
      } else if (3 == sscanf(text, "#%d Turnover:3 Second Violation (%d TO) %d", v+0,v+1,v+8)) {
         turnover(TURNOVER_3sec   , t, v[0], 0);
         //assert(stats[v[0]].turnovers == v[1]);
      } else if (5 == sscanf(text, "#%d Turnover:Lost Ball (%d TO) Steal:#%d (%d ST) %d", v+0,v+1,v+2,v+3,v+8)) {
         turnover(TURNOVER_stolen , t, v[0], v[2]);
         //assert(stats[v[0]].turnovers == v[1]);
         //assert(stats[v[2]].steals    == v[3]);

      // misc
      } else if (3 == sscanf(text, "#%d Substitution replaced by #%d %d", v+0, v+1, v+8)) {
         enqueue_substitution(v[0],v[1]);
      } else if (1 == sscanf(text, "Team Timeout:Regular %d", v+8)) {
         process_substitution();
         do_timeout(t);
         // do nothing
      } else if (1 == sscanf(text, "Team Timeout:Short %d", v+8)) {
         process_substitution();
         do_timeout(t);
         // do nothing
      } else {
         printf("!!!   missing handler for    !!!!!!!!\n      %s\n", text);
         v[8] = 737;
      }
      assert(v[8] == 737);

      #if 0
      if (assist)
         assert(stats[assist].assists == v[7]);  
      if (blocker)
         assert(stats[blocker].blocks == v[6]);
      #endif
   } else {
      int v[5];
      process_substitution();
      if (0 == strcmp(text, "Start of 1st Quarter")) {
         half = quarter = 0;
         sing(V1, "Wel-come to to-night's Bas-ket-ball Bas-ket-ball As-so-ci-a-tion game.");
         sing(V1, "Fea-tur-ing && ver-sus &&.", 0, 1);
         set_lineup(TEAM_0,  1,  2,  3,  4,  5);
         set_lineup(TEAM_1, -1, -2, -3, -4, -5);
         team_fouls[0] = team_fouls[1] = 0;
         announce_lineup(1);
      } else if (0 == strcmp(text, "End of 1st Quarter")) {
         sing(V1, "That's the end of the first quar-ter.");
         quarter = 1;
         set_lineup(TEAM_0,  1,  7,  9,  4,  8); // 7 unknown
         set_lineup(TEAM_1, -9, -2, -6, -4, -7);
         announce_lineup(0);
         team_fouls[0] = team_fouls[1] = 0;
         set_tpossess(TEAM_1, -1);
         game_state.position = 1;
      } else if (0 == strcmp(text, " Timeout: Official")) {
         static int which;
         do_timeout(-1);
         if (which == 0) {
            set_lineup(TEAM_0, 1, 2, 3, 4, 5);
            set_lineup(TEAM_1, -6, -2, -7, -4, -5);
            // -6 -2 -5 -7 -4
            announce_lineup(0);
         } else {
            set_lineup(TEAM_0, 7, 2, 9, 6, 8);
            set_lineup(TEAM_1, -6, -2, -7, -4, -9);
            announce_lineup(0);
         }
         ++which;
      } else if (0 == strcmp(text, "End of 2nd Quarter")) {
         sing(V1, "That's the end of the first half.");
         quarter = 2;
         half = 1;
         set_lineup(TEAM_0, 1, 2, 3, 4, 5);
         set_lineup(TEAM_1, -1, -2, -3, -4, -5); 
         announce_lineup(0);
         team_fouls[0] = team_fouls[1] = 0;
         set_tpossess(TEAM_1, -1);
         game_state.position = 1;
      } else if (0 == strcmp(text, "End of 3rd Quarter")) {
         sing(V1, "That's the end of the third quar-ter.");
         quarter = 3;
         set_lineup(TEAM_0, 7, 2, 9, 6, 8);
         set_lineup(TEAM_1, -6, -7, -3, -4, -9);
         announce_lineup(0);
         team_fouls[0] = team_fouls[1] = 0;
         set_tpossess(TEAM_0, -1);
         game_state.position = -1;
      } else if (0 == strcmp(text, "End of 4th Quarter")) {
         sing(V1, "That's the end of the game.");
         // no lineup changes
      } else if (2 == sscanf(text, " Jump Ball #%d vs #%d", v+0,v+1)) {
         // announce the lineup
         sing(V1, "It's $ ver-sus $ to start the game.", v[0], v[1]);
         sing(V1, "The tip off goes to &.", TEAM_0); // hardcoded to this specific game instead of detecting from transcript
         set_tpossess(TEAM_0, -1);
         game_state.position = -0.5;
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
   //stb_srand(0);
   create_teams();

   set_tpossess(-1, 0);
   game_state.current_time = 0;
   for (i=0; i < n; ++i) {
      cur_line = i+1;
      process_event(pbp[i]);
   }
   if (pending.voice >= 0)
      close_vocal_line();

   write_music();
   return 0;
}
