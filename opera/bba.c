#define STB_DEFINE
#include "stb.h" // http://github.com/nothings/stb

#undef assert
#define assert(x) do { if (!(x)) { __asm int 3 } } while (0)

#pragma warning(disable:4244; disable:4305)

int g_songnum;

#define MIDDLE_C   60

#define OUTPUT_START   0
#define OUTPUT_END     1200

// TODO-ish: lyrics
//   don't do two "with"s in a row for rebound & possession
//   don't do two/three "balls" in a row for rebound, possession, and up the court
//   substitute "it" for "ball" in some cases
//   if we tie up the score in the first free throw, should definitely mention
//      the second free throw goes ahead
//   it shouldn't be the point guards doing the tip-off!
//   V1 announces time left in quarter while player 'brings ball up court' if
//      close to end of quarter

//
//  have a whistle blow at each foul
//  track shot clock and have shot clock buzzer?

// Pallacanestro: The Opera
//
// (primo secondo terzo quarto) periodo
// parte (prima seconda)

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
  *points for the night after scoring
  *layup/field goal/3pt/total points for the night after such events
  *layup/field goal/3pt/total points for the team after such events
  +layup/field goal/3pt/total points compare halfs for player after such events
  +layup/field goal/3pt/total points compare halfs for team after such events
  +layup/field goal/3pt/total points compare halfs for teams after such events
  *layup/field goal/3pt/total percentages for the night after such events
   layup/field goal/3pt/total percentages for the season after such events
   layup/field goal/3pt/total results for the previous night after such events
  *1st point
  *1st layup
  *1st field goal
  *1st 3 pointer
  *1st point of the quarter
   assists

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

enum
{
   SONG1,
   SONG2,
   SONG3,
   SONG4,
   SONG5,
   SONG6,
};

#define MAX_SONGS 6
int num_songs=0;
vocal_line *all_vocals[MAX_SONGS];
vocal_line *vocals; // the current vocals being musicified

stb_sdict *syllable_map;
#if 0
typedef struct
{
   int note;
} pitch;
#endif

enum
{
   KEY_sharp,
   KEY_flat,
};

static char *lilynote[2][12] =
{
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
   },
   {
      "c",
      "des",
      "d",
      "ees",
      "e",
      "f",

      "ges",
      "g",
      "aes",
      "a",
      "bes",
      "b",
   }
};

FILE *f;

#define WHOLE_NOTE    96

int triplet_stage=0;

void pre_duration(int duration)
{
   switch (duration) {
      case WHOLE_NOTE/6:
      case WHOLE_NOTE/12:
         if (triplet_stage == 0)
            fprintf(f, "\\tuplet 3/2 { ");
         ++triplet_stage;
   }
}

void post_duration(int duration)
{
   if (triplet_stage == 3) {
      fprintf(f, "} ");
      triplet_stage = 0;
   }
}

void lily_print_duration(int duration)
{
   switch (duration) {
      case WHOLE_NOTE*1  : fprintf(f, "1" ); break;
      case WHOLE_NOTE*1/2: fprintf(f, "2"); break;
      case WHOLE_NOTE*1/4: fprintf(f, "4"); break;
      case WHOLE_NOTE*3/4: fprintf(f, "2."); break;
      case WHOLE_NOTE*1/8: fprintf(f, "8"); break;
      case WHOLE_NOTE*3/8: fprintf(f, "4."); break;
      case WHOLE_NOTE*3/16: fprintf(f, "8."); break;
      case WHOLE_NOTE*1/16: fprintf(f, "16"); break;
      case WHOLE_NOTE/6:    fprintf(f, "4 "); break;
      case WHOLE_NOTE/12:   fprintf(f, "8 "); break;
      default: assert(0);
   }
}

int current_key_is_flat;
void lily_print_note(int note)
{
   if (note > 0) {
      int octave = (note - 60 + 5*12) / 12;
      int pc;
      assert(note >= 12);
      pc = note-60+5*12 - 12*octave;
      assert(pc >= 0 && pc < 12);
      fprintf(f, "%s", lilynote[current_key_is_flat][pc]);
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

enum
{
   D_ppp,
   D_pp,
   D_p,
   D_mp,
   D_mf,
   D_f,
   D_ff,
   D_fff,
};


typedef struct
{
   int midinote[4];
   int duration;
   int volume;   // -5 .. 5
   int melisma; // true if it's a melisma attached to previous note
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

float cur_intensity;

note *generate_phrase(int num_notes, char **syl)
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

      #if 1
      if (i < num_notes-1) {
         char *s = syl[i] + strlen(syl[i]);
         if (s[-1] == ',') {
            if (n.duration < WHOLE_NOTE/2)
               n.duration = WHOLE_NOTE*3/8;
         } else if (s[-1] == '.' && s[-2] == '.') {
            if (n.duration < WHOLE_NOTE/2)
               n.duration = WHOLE_NOTE/2;
         }
      }
      #endif

      stb_arr_push(list, n);
   }
   return list;
}

int scale[15] = {
   0,2,4, 5,7,9,11,
   12,14,16, 17,19,21,23,
   24
};

int cmajor[7] = { 0,2,4,5,7,9,11 };

int key_scale[][7] =
{
   { 0,2,4,5,7,9,11 },
   { 0,2,3,5,7,8,11 },
   { 0,2,3,5,7,9,11 },
   { 1,2,4,5,7,9,11 },
   { 0,2,3,6,7,9,10, },
   { 0,1,4,5,7,8,11, },
};

#define NUM_SCALES  (sizeof(key_scale)/sizeof(key_scale[0]))

void set_key(int key, int *output)
{
   int notes[7];
   int i;
   int wscale = key / 12;
   int offset = key % 12;
   for (i=0; i < 7; ++i) {
      notes[i] = key_scale[wscale][i] + offset;
      if (notes[i] >= 12)
         notes[i] -= 12;
   }
   qsort(notes, 7, sizeof(notes[0]), stb_intcmp(0));
   for (i=1; i < 7; ++i)
      assert(notes[i-1] != notes[i]);
   for (i=0; i < 15; ++i) {
      output[i] = notes[i%7] + 12 * (i/7);
   }
}

int key_change_likelihood(int *from, int *to)
{
   int n=0;
   int i,j;
   for (i=0; i < 7; ++i)
      for (j=0; j < 7; ++j)
         n += (from[i] == to[j]);
   assert(n <= 7);
   if (n < 4)
      return 0;
   if (n == 7)
      return 0;
   return n-2;
}

int exotic_compatible_key(int key, int prefer)
{
   int i, r;
   int myscale[16];
   int distribution[NUM_SCALES*12+1] = { 0 };
   for (i=0; i < NUM_SCALES*12; ++i) {
      set_key(i, myscale);
      distribution[i] = key_change_likelihood(scale, myscale);
   }
   for (i=0; i < NUM_SCALES*12; ++i)
      distribution[i] *= distribution[i]; // MSE
   distribution[prefer] *= 4;
   for (i=12; i < NUM_SCALES*12; ++i)
      distribution[prefer] *= 4;
   for (i=1; i < NUM_SCALES*12; ++i)
      distribution[i] += distribution[i-1];
   r = random(distribution[NUM_SCALES*12-1]);
   for (i=0; i < NUM_SCALES*12; ++i)
      if (r < distribution[i])
         return i;
   assert(0);
}

int printable_key(int key)
{
   int i,j,k, best_key = -1, best_accidentals = 5000;
   int exotic_scale[16];
   int print_scale[16];
   set_key(key, exotic_scale);
   for (i=0; i < 12; ++i) {
      set_key(i, print_scale);
      // try mapping print_scale[j] to exotic_scale[0]
      for (j=0; j < 7; ++j) {
         int accidentals = 0;
         for (k=0; k < 7; ++k) {
            int dist = abs(print_scale[(j+k)%7] - exotic_scale[k]);
            if (dist > 6) dist = abs(dist - 12);
            accidentals += dist;
         }
         if (accidentals < best_accidentals) {
            best_key = i;
            best_accidentals = accidentals;
         }
      }
   }
   assert(best_key >= 0);
   return best_key;
}


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

int voicenote_phrase_duration(voicenote *phrase, int len)
{
   int duration=0;
   int i;
   for (i=0; i < len; ++i)
      duration += phrase[i].duration;
   return duration;
}

int timedchord_duration(timedchord *tc, int len)
{
   int duration = 0, i;
   for (i=0; i < len; ++i)
      duration += tc[i].duration;
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

int allow_chromatic_down(voicenote *a, voicenote *b, voicenote *c, voicenote *d)
{
   if (b->midinote[0] == c->midinote[0])
      return a->midinote[0] > b->midinote[0] && d->midinote[0] > c->midinote[0];
   return 0;
}

int allow_chromatic_passing(voicenote *a, voicenote *b, voicenote *c, voicenote *d)
{
   int delta = c->midinote[0] - b->midinote[0];
   if (abs(delta) != 2)
      return 0;
   
   if (delta < 0)
      return a->midinote[0] > b->midinote[0] && c->midinote[0] > d->midinote[0];
   else
      return a->midinote[0] < b->midinote[0] && c->midinote[0] < d->midinote[0];
}

int allow_triplet(voicenote *a, voicenote *b, int tm)
{
   if (a->duration == WHOLE_NOTE/2) {
      if (tm != 0 && tm != WHOLE_NOTE/2)
         return 0;
   } else if (a->duration == WHOLE_NOTE/4) {
      if (tm % (WHOLE_NOTE/4) != 0)
         return 0;
   } else
      return 0;

   return abs(a->midinote[0] - b->midinote[0]) >= 5;
}


int match_scale(int note)
{
   int i;
   int offset=0;
   for (offset=0; offset < 12; ++offset) {
      int p = (note+offset) % 12;
      for (i=0; i < 7; ++i)
         if (scale[i] % 12 == p)
            return (note+offset);
      p = (note-offset) % 12;
      for (i=0; i < 7; ++i)
         if (scale[i] % 12 == p)
            return note-offset;
   }
   assert(0);
   return note;
}

voicenote *ornament_melody(voicenote *notes, timedchord *changes, int range_low, int range_high, uint8 *protected_notes)
{
   int i, tm=0;
   voicenote *output = NULL;
   for (i=0; i < stb_arr_len(notes); ++i) {
      if (i >= 1 && i < stb_arr_len(notes)-1 && !protected_notes[i] && notes[i].duration >= WHOLE_NOTE/8 && notes[i].duration < WHOLE_NOTE/2) {
         switch (random(3)) {
            case 0:
               if (allow_chromatic_down(&notes[i-1], &notes[i], &notes[i+1], &notes[i+2]) && random(100) < 80) {
                  voicenote n = notes[i];
                  n.duration /= 2;
                  stb_arr_push(output, n);
                  n.midinote[0] -= 1;
                  n.melisma = 1;
                  stb_arr_push(output, n);
               } else
                  stb_arr_push(output, notes[i]);
               break;
            case 1: {
               if (allow_chromatic_passing(&notes[i-1], &notes[i], &notes[i+1], &notes[i+2]) && random(100) < 30) {
                  voicenote n = notes[i];
                  n.duration /= 2;
                  stb_arr_push(output, n);
                  n.midinote[0] = (notes[i].midinote[0] + notes[i+1].midinote[0])/2;
                  n.melisma = 1;
                  stb_arr_push(output, n);
               } else
                  stb_arr_push(output, notes[i]);
               break;
            }
            case 2:
               if (allow_triplet(&notes[i], &notes[i+1], tm)) {
                  int delta = notes[i+1].midinote[0] - notes[i].midinote[0];
                  int n2 = match_scale(notes[i].midinote[0] + delta*1/3);
                  int n3 = match_scale(notes[i].midinote[0] + delta*2/3);
                  if (n2 == n3 || n2 == notes[i].midinote[0] || n3 == notes[i+1].midinote[0]) {
                     stb_arr_push(output, notes[i]);
                  } else {
                     voicenote n = notes[i];
                     n.duration /= 3;
                     stb_arr_push(output, n);
                     n.melisma = 1;
                     n.midinote[0] = n2;
                     stb_arr_push(output, n);
                     n.midinote[0] = n3;
                     stb_arr_push(output, n);
                  }
               } else
                  stb_arr_push(output, notes[i]);
               break;
            case 3:
               stb_arr_push(output, notes[i]);
               break;
            default: assert(0);
         }         
      } else {
         stb_arr_push(output, notes[i]);
      }
      tm += notes[i].duration;
   }
   for (i=0; i < stb_arr_len(output); ++i)
      assert(output[i].duration > 0 && output[i].duration <= WHOLE_NOTE);
   return output;
}

voicenote *adapt_melody(timedchord *changes, note *phrase, int range_low, int range_high, chord *final_note_chord)
{
   uint8 *protected_notes = NULL;
   chord cur_chord;
   int flag_newchord = 1;
   int i, n=0;
   int offset=0;
   int chord_end_time;
   int tm=0;
   voicenote *nlist=0, *final;
   if (range_low < MIDDLE_C)
      offset = -12;
   cur_chord = changes[0].c;
   chord_end_time = changes[0].duration;

   for (i=0; i < stb_arr_len(phrase); ++i) {
      voicenote v = { 0 };
      int sn = phrase[i].scalenote;

      while (tm >= chord_end_time) {
         assert(n < stb_arr_len(changes));
         cur_chord = changes[n].c;
         chord_end_time += changes[n].duration;
         flag_newchord = 1;
      }

      if (flag_newchord) {
         sn = find_nearest_note_to_chord(sn, &cur_chord);
         stb_arr_push(protected_notes, 1);
      } else {
         stb_arr_push(protected_notes, 0);
      }

      v.midinote[0] = midi_note_for_scale(sn, 60) + offset;
      if (v.midinote[0] < range_low) {
         offset += 12;
         v.midinote[0] += 12;
      }
      if (v.midinote[0] > range_high) {
         offset -= 12;
         v.midinote[0] -= 12;
      }

      v.duration = phrase[i].duration;
      v.volume   = D_f;
      assert(v.duration > 0 && v.duration <= WHOLE_NOTE);
      stb_arr_push(nlist, v);

      tm += phrase[i].duration;
      flag_newchord = 0;
   }

   final = ornament_melody(nlist, changes, range_low, range_high, protected_notes);
   stb_arr_free(protected_notes);
   stb_arr_free(nlist);

   *final_note_chord = cur_chord;
   return final;
}

voicenote *song3_adapt_melody(chord *start_chord, chord *end_chord, note *phrase, int range_low, int range_high)
{
   int i;
   int offset=0;
   voicenote *nlist=0;
   if (range_low < MIDDLE_C)
      offset = -12;
   for (i=0; i < stb_arr_len(phrase); ++i) {
      voicenote v = { 0 };
      int sn = phrase[i].scalenote;

      if (i == 0) {
         sn = find_nearest_note_to_chord(sn, start_chord);
      } else if (i == stb_arr_len(phrase)-1) {
         sn = find_nearest_note_to_chord(sn, end_chord);
      }

      v.midinote[0] = midi_note_for_scale(sn, 60) + offset;
      if (v.midinote[0] < range_low) {
         offset += 12;
         v.midinote[0] += 12;
      }
      if (v.midinote[0] > range_high) {
         offset -= 12;
         v.midinote[0] -= 12;
      }

      v.duration = phrase[i].duration;
      v.volume   = D_f;
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

//  bass: G2..E4 (F2..F4)
//  soprano: D4..B5 (C4..C6)
//  mezzo-soprano: B3..G5 (A3..A5)

int max_range[I__count][2] =
{
   { MIDDLE_C   ,   MIDDLE_C+24+0 },  //    I_soprano,        
   { MIDDLE_C- 3,   MIDDLE_C+12+9 },  //    I_mezzo,          
   { MIDDLE_C-19,   MIDDLE_C   +5 },  //    I_bass,           
                                         
   { MIDDLE_C-36-3, MIDDLE_C+4    },    //    I_piano, left-hand only
   { MIDDLE_C,      MIDDLE_C+36+5 },    //    I_flute
   { MIDDLE_C-10,   MIDDLE_C+36   },    //    I_clarinet,       
   { MIDDLE_C- 6,   MIDDLE_C+24+2 },    //    I_trumpet,        
   { MIDDLE_C-24-2, MIDDLE_C+12+4 },    //    I_horn,           
   { MIDDLE_C-36,   MIDDLE_C+24+5 },    //    I_harp,           
   { MIDDLE_C-5,    MIDDLE_C+36+5 },    //    I_violin1,        
   { MIDDLE_C-5,    MIDDLE_C+36+5 },    //    I_violin2,        
   { MIDDLE_C-12,   MIDDLE_C+24+11 },   //    I_viola,          
   { MIDDLE_C-24,   MIDDLE_C+12+11 },   //    I_cello,          
   { MIDDLE_C-31,   MIDDLE_C+12    },   //    I_double_bass,    
};

int range[I__count][2] =
{
   { MIDDLE_C+2 ,   MIDDLE_C+23 },  //    I_soprano,        
   { MIDDLE_C-1 ,   MIDDLE_C+19 },  //    I_mezzo,          
   { MIDDLE_C-17,   MIDDLE_C +4 },  //    I_bass,           
                                         
   { MIDDLE_C-24-3, MIDDLE_C+4    },    //    I_piano, left-hand only
   { MIDDLE_C+7,    MIDDLE_C+24+5 },    //    I_flute
   { MIDDLE_C-3,    MIDDLE_C+24   },    //    I_clarinet,       
   { MIDDLE_C-3,    MIDDLE_C+12+7 },    //    I_trumpet,        
   { MIDDLE_C-24-2, MIDDLE_C+4 },    //    I_horn,           
   { MIDDLE_C-3,    MIDDLE_C+24+5 },    //    I_harp,           
   { MIDDLE_C-5,    MIDDLE_C+12+9 },    //    I_violin1,        
   { MIDDLE_C-5,    MIDDLE_C+12+9 },    //    I_violin2,        
   { MIDDLE_C-12,   MIDDLE_C+12   },   //    I_viola,          
   { MIDDLE_C-12-5, MIDDLE_C+7    },   //    I_cello,          
   { MIDDLE_C-24-5, MIDDLE_C-5    },   //    I_double_bass,    
};

int sixteenth_tempo_threshhold_chord[I__count] =
{
   0,0,0,

   110,
   0,
   0,
   0,
   0,
   90,

   0,
   0,
   0,
   0,
   0,
};

int sixteenth_tempo_threshhold[I__count] =
{
   0,0,0,

   140,  // piano
   90,
   100,
   100,
   90,   // horn
   110,  // harp

   150,
   150,
   150,
   140,
   130,
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

void song3_random_chord(chord *a)
{
   a->scalenote[0] = random(SCALE_PITCHES);
   a->scalenote[1] = a->scalenote[0] + 2;
   a->scalenote[2] = a->scalenote[0] + 4;
}

char *arp_table[][12] =
{
      { // 2/4
         "0123:3456",
         "6543:3210",
         "3210:1020",
         "0123:4546",
         "0123:4565",
         "6543:2101",
      },
      { // 2/4 not really arpeggios
         "321:0202",
         "0123:2020",
         "3:0303",
         "0:3030",
         "00000:32100000",
         "00000:00000123",
      },
      { // 6/8 
         "012:345654",
         "654:321012",
         "012:345",
         "543:210",
      },
      { // 6/8 not really arpeggios
         "321:000123",
         "012:333210",
         "010:323010",
         "123:030212",
         "321:030312",
         "321:010012",
         "012:345354",
      },
      { // 7/8
         "212:0123212",
         "210:3210210",
         "101:3210101",
         "223:0011223",
      },
      { // 5/8
         "212:01212",
         "232:01232",
         "121:30121",
         "010:32010",
         "010:20010",
      },
};

char *fix_sixteenths_arp(char *s)
{
   if (s == arp_table[0][0] || s == arp_table[0][1])
      return s;
   return NULL;
}

int arp_table_len[] = { 6,6,4,7,4,5 };

int instr;
char *instrument_arp[I__count];

int div_floor(int x, int y)
{
   if (x < 0)
      if (x % y == 0)
         return x/y;
      else
         return x/y - 1;
   else
      return x/y;
}
// 
int scale_note_for_chord(timedchord *c, int note)
{
   int cnote = (note + 3*100) % 3;
   int octave = div_floor(note - cnote, 3);
   return c->c.scalenote[cnote] + SCALE_PITCHES * octave;
}


// scale notes 0..6
//   transform to:  MIDDLE_C + scale[0] .. MIDDLE_C + scale[6

int midi_note_for_full_scale(int note)
{
   int snote = (note + SCALE_PITCHES*100) % SCALE_PITCHES;
   int octave = div_floor(note - snote, SCALE_PITCHES);
   return MIDDLE_C + scale[snote] + octave * 12;
}


voicenote *make_chord_voice(timedchord *c, int low_range, int high_range, int vm, int inst, timedchord *corn)
{
   char *arp = instrument_arp[inst];
   char *arp_loop = arp ? strchr(arp, ':') + 1 : NULL;
   int i,j,k;
   int vm_index = 0;
   int arp_index = 1;
   int total_duration=0;
   int arpstart;
   int octave_offset[3] = { 0,0,0 };
   int starting_note[3] = { 0,0,0 }; // if arpeggiating, the 'base' note to arpeggiate from
   int current_note[3] = { 0,0,0 }; // if not arpeggiating, the midinote used to minimize octave jumps
   int prev_note[3] = { 0,0,0 };
   voicenote *nlist=0;

   if (arp) {
      if (*arp == ':')
         arpstart = arp[1] - '0';
      else
         arpstart = arp[0] - '0';
   } else
      c = corn;

   // pick initial pitches

   if (g_songnum == SONG5 || arp)
      if (vm & (vm-1))
         vm = 1 << random(3);
   for (i=0; i < 3; ++i) {
      if (vm & (1 << i)) {
         for (k=0; k < stb_arr_len(c); ++k) {
            if (c[k].c.scalenote[i] >= 0) {
               int d;
               int n = c[k].c.scalenote[i], p;
               int note, snote, cnote;
               int r = random_range(low_range, high_range);
               n %= SCALE_PITCHES;
               p = scale[n] % 12;
               for (d=0; d < 12; ++d) {
                  if ((r+d)%12 == p && r+d <= high_range) {
                     note = r+d;
                     break;
                  }
                  if ((r-d)%12 == p && r-d >= low_range) {
                     note = r-d;
                     break;
                  }
               }
               assert(d < 12);

               for (snote = -8*SCALE_PITCHES; snote <= 8 * SCALE_PITCHES; ++snote)
                  if (midi_note_for_full_scale(snote) == note)
                     break;
               assert(snote <= 8 * SCALE_PITCHES);

               if (arp) {
                  for (cnote = -8*3; cnote <= 8*3; ++cnote)
                     if (scale_note_for_chord(&c[k], cnote) == snote)
                        break;

                  starting_note[i] = cnote;

                  assert(midi_note_for_full_scale(scale_note_for_chord(&c[k], cnote)) == note);
               } else {
                  prev_note[i] = midi_note_for_full_scale(c[k].c.scalenote[i]);
                  current_note[i] = note;
               }
               break;
            }
         }
      }
   }

   for (i=0; i < stb_arr_len(c); ++i) {
      voicenote n={0};
      if (c[i].c.scalenote[0] >= 0) {
         k = 0;
         for (j=0; j < 3; ++j) {
            if (vm & (1 << j)) {
               if (arp) {
                  //int
                  int nt;
                  int p;
                  int mn;
                  int a;

                  if (*arp == ':') ++arp;
                  assert(*arp >= '0' && *arp <= '9');
                  a = *arp - '0';
                  ++arp;
                  if (*arp == 0) arp = arp_loop;
                  p = starting_note[j] + (a - arpstart);
            
                  nt = scale_note_for_chord(&c[i], p);

                  mn = midi_note_for_full_scale(nt);

                  while (mn + octave_offset[j] < low_range) {
                     octave_offset[j] += 12;
                  }
                  while (mn + octave_offset[j] > high_range) {
                     octave_offset[j] -= 12;
                  }
                  mn += octave_offset[j];
                  assert(mn >= low_range && mn <= high_range);

                  n.midinote[k++] = mn;
                  //midi_note_for_scale(c[i].c.scalenote[j], low_range);
               } else {
               // no arpeggio, use actual notes in the chords which can be variants
                  int mn;

                  if (i == 0) {
                     int nt   = midi_note_for_full_scale(c[i].c.scalenote[j]);
                     prev_note[j] = nt;
                     mn = current_note[j];
                     assert(mn % 12 == nt % 12);
                  } else {
                     int prev = prev_note[j];
                     int nt   = midi_note_for_full_scale(c[i].c.scalenote[j]);

                     mn = current_note[j] + (nt - prev);
                     prev_note[j] = nt;

                     assert(mn % 12 == nt % 12);
                  }

                  current_note[j] = mn;

                  while (mn + octave_offset[j] < low_range) {
                     octave_offset[j] += 12;
                  }
                  while (mn + octave_offset[j] > high_range) {
                     octave_offset[j] -= 12;
                  }
                  mn += octave_offset[j];
                  assert(mn >= low_range && mn <= high_range);

                  n.midinote[k++] = mn;
               }
            }
         }
      }

      n.duration = c[i].duration;
      n.volume   = (inst == I_piano && ((vm) & (vm-1))==0) ? D_mf : D_p; // boost piano if it's a single note
      stb_arr_push(nlist, n);
   }
   return nlist;
}

voicenote *song3_make_chord_voice(timedchord *c, int low_range, int high_range, int vm, int arpnum)
{
   int i,j,k;
   int arp_index = 1;
   int total_duration=0;
   int midi_note[3]={0,0,0};
   voicenote *nlist=0;

   // pick initial pitches
   for (i=0; i < 3; ++i) {
      if (vm & (1 << i)) {
         for (k=0; k < stb_arr_len(c); ++k) {
            if (c[k].c.scalenote[i] >= 0) {
               int d;
               int n = c[k].c.scalenote[i], p;
               int r = random_range(low_range, high_range);
               n %= SCALE_PITCHES;
               p = scale[n] % 12;
               for (d=0; d < 12; ++d) {
                  if ((r+d)%12 == p && r+d <= high_range) {
                     midi_note[i] = r+d;
                     break;
                  }
                  if ((r-d)%12 == p && r-d >= low_range) {
                     midi_note[i] = r-d;
                     break;
                  }
               }
               assert(d < 12);
            }
         }
      }
   }

   for (i=0; i < stb_arr_len(c); ++i) {
      voicenote n={0};
      k = 0;
      for (j=0; j < 3; ++j) {
         if (vm & (1 << j))
            if (c[i].c.scalenote[j] >= 0) {
               //int
               int nt;
               int p;
               
               nt = c[i].c.scalenote[j];  
               p = scale[nt % SCALE_PITCHES] % 12;
               if (midi_note[j] % 12 != p) {
                  int offset = p - (midi_note[j] % 12);
                  if (offset >  6) offset -= 12;
                  if (offset < -6) offset += 12;
                  midi_note[j] += offset;
                  if (midi_note[j] > high_range) midi_note[j] -= 12;
                  if (midi_note[j] < low_range ) midi_note[j] += 12;
                  assert(midi_note[j] >= low_range && midi_note[j] <= high_range);
               }
               n.midinote[k++] = midi_note[j];
               //midi_note_for_scale(c[i].c.scalenote[j], low_range);
               assert(midi_note[j] >= low_range && midi_note[j] <= high_range);
            }
      }

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

timedchord *make_timedchords(int final_chord_time, int total_time, chord *final_chord, chord *first_chord)
{
   int tm=0, first=1;

   timedchord t;
   timedchord *tc = NULL;

   chord a,b, *start_chord = &a, *end_chord = &b;

   a = *first_chord;
   do
      random_chord(&b);
   while (a.scalenote[0] == b.scalenote[0] && a.scalenote[1] == b.scalenote[1]);

   //int align;
   assert(total_time % WHOLE_NOTE == 0);

   //align = WHOLE_NOTE/4;
   //final_chord_time = (final_chord_time + (align-1)) / align * align;

   if (final_chord_time % WHOLE_NOTE <= WHOLE_NOTE / 2 && final_chord_time >= WHOLE_NOTE) {
      if (random(100) < 95)
         final_chord_time -= final_chord_time % WHOLE_NOTE;
   }

   t.c = *start_chord;
   if (final_chord_time >= WHOLE_NOTE)
      t.duration = WHOLE_NOTE;
   else
      t.duration = final_chord_time;
   stb_arr_push(tc, t);

   tm += t.duration;

   while (tm + WHOLE_NOTE <= final_chord_time) {
      assert(tm % WHOLE_NOTE == 0);
      if (g_songnum == SONG5)
         random_chord(&t.c);
      else if (random(100) < 50 && first) {
         random_chord(&t.c);
         first = 0;
      }
      t.duration = WHOLE_NOTE;
      stb_arr_push(tc, t);
      tm += t.duration;
   }

   if (tm < final_chord_time) {
      t.duration = final_chord_time - tm;
      stb_arr_push(tc, t);
      tm += t.duration;
   }
   assert(tm == final_chord_time);

   t.c = *end_chord;
   t.duration = total_time - final_chord_time;
   stb_arr_push(tc, t);

   return tc;
}

timedchord *song3_make_timedchords(chord *start_chord, chord *end_chord, int final_chord_time, int total_time)
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

char *song3_timing_patterns[] =
{
   "x..x..x.",
   "x.x.x...",
   "x...x.x.",
   "x.xx.xx.",
   "xxx.....",
   "xxx..x..",
   "xxx...x.",
};

char *timing_patterns[] =
{
   "X.rX.rX.",
   "X.X.X..r",
   "X...X.X.",
   "X.XX.XXr",
   "XXX...r.",
   "XXX.rX..",
   "XXX...Xr",

   "r.X.r.X.",
   "r...X...",
   "X.......",
   "X...X...",
   "Xr..Xr..",
   "X.r.X.r.",
   "X...r...",
   "X.r...X.",
   "X...X.X.",
   "X.r.X.X.",
   "XXXr.XX.",
   "XX.XX.X.",
   "XXrXrXrX",
   "XXX.XXX.",
   "XXX..rXX",
   "XX..XX..",
   "XX..XrX.",

   // sixteenths patterns must be last
   "XXXXX...XXXXX...",
   "XXX.X.X.X.X.X.X.",
   "X.X.X.XXX.X.X.X.",
   "X.X.X.X.X.X.XXXX",
};

#define SONG5_TIMING_PATTERNS 7
#define SONG3_TIMING_PATTERNS 7

#define TIMING_PATTERNS (sizeof(timing_patterns)/sizeof(timing_patterns[0]))

#define NUM_SIXTEENTH_PATTERNS  4
#define FIRST_SIXTEENTH_PATTERN (TIMING_PATTERNS-NUM_SIXTEENTH_PATTERNS)

enum
{
   MODE_pattern,
   MODE_quarters,
   MODE_eighths,
   MODE_sixteenths,

   MODE_silent,

   MODE__count
};

int arpeggio_chance[6][5] =
{
   { 0,  0,  60,  15, 0, },
   { 0,  0,  60,  30, 0, },
   { 0,  0,  60,  30, 0, },   // song3
   { 0,  0,  60,  30, 0, },
   { 0, 50, 100, 100, 0, },   // song5
   { 0,  0,  60,  50, 0, },
};



char *structures[4][16] =
{
   { "ABAA", "AABA", "ABBA", "ABCA", },
   { "AABA", "AABB", "ABBA", "ABAB", "ABCA", "ABAC", },
   { "AABA", "AABB", "ABAB", "ABAB", "ABCA", "ABAC", "AABC" },
   { "AABA", "AABB", "ABAB", "ABCA", "ABCD", "ABAC" },
};

int num_structures[4] =
{
   4,
   6,
   7,
   6,
};

stb_sdict *gendict;
int novel_state[8];
typedef struct
{
   unsigned char state[8];
} structure_desc;

structure_desc *generate(structure_desc *output, char prefix[16], int depth, int maxdepth, structure_desc desc)
{
   int i;
   structure_desc *expanded = stb_sdict_get(gendict, prefix);
   if (!expanded) {
      char * pattern;

      expanded = NULL;

      desc.state[maxdepth-depth] = novel_state[depth];
      if (++novel_state[depth] == 256)
         novel_state[depth] = 0;

      if (depth != maxdepth) {
         pattern = structures[depth][random(4)];
         for (i=0; i < 4; ++i) {
            prefix[depth] = pattern[i];
            prefix[depth+1] = 0;
            expanded = generate(expanded, prefix, depth+1, maxdepth, desc);
         }
      } else {
         stb_arr_push(expanded, desc);
      }
      prefix[depth] = 0;
      stb_sdict_add(gendict, prefix, expanded);
   }
   for (i=0; i < stb_arr_len(expanded); ++i)
      stb_arr_push(output, expanded[i]);
   return output;
}

structure_desc *generate_structure(int depth)
{
   structure_desc d = { 0 };
   char prefix[16];
   structure_desc *output;
   memset(novel_state, 0, sizeof(novel_state));
   gendict = stb_sdict_new(1);
   output = generate(NULL, prefix, 0, depth, d);
   return output;
}

#define MAX_ARPEGGIO 4
int instrument_part[I__count];
int instrument_voice[I__count];

#define NUM_PARTS  4
int part_mode[NUM_PARTS];
int part_orn[NUM_PARTS];
int num_parts;

int pattern2[3];
int part_pattern[NUM_PARTS];

char *pattern_for_part(int part)
{
   switch (part_mode[part]) {
      case MODE_silent    : return ".";
      case MODE_quarters  : return "XXXX"; 
      case MODE_eighths   : return "XXXXXXXX";
      case MODE_sixteenths: return "XXXXXXXXXXXXXXXX";
      case MODE_pattern   : return timing_patterns[pattern2[part_pattern[part]]];
      default: assert(0); return 0;
   }
}

char *song3_pattern_for_part(int part)
{
   switch (part_mode[part]) {
      case MODE_silent    : return ".";
      case MODE_quarters  : return "xxxx"; 
      case MODE_eighths   : return "xxxxxxxx";
      case MODE_sixteenths: return "xxxxxxxxxxxxxxxx";
      case MODE_pattern   : return song3_timing_patterns[pattern2[part_pattern[part]]];
      default: assert(0); return 0;
   }
}

int my_tempo = 120;

int song5_random_mode(void)
{
   float t;
   int i, r;
   int distribution[5];

   t = stb_linear_remap(cur_intensity, 0.2, 1.0, 0, 1);
   t = stb_clamp(cur_intensity, 0, 1);

   distribution[MODE_silent]    = 0;
   distribution[MODE_eighths]   = 100;
   distribution[MODE_sixteenths]= stb_lerp(t*t,   5, 300);
   distribution[MODE_quarters]  = stb_lerp(t  ,  80, 5);
   distribution[MODE_pattern]   = stb_lerp(t  , 150, 10);
   for (i=1; i < MODE__count; ++i)
      distribution[i] += distribution[i-1];      
   r = random(distribution[MODE__count-1]);
   for (i=0; i < MODE__count; ++i)
      if (r < distribution[i]) {
         assert(i < 4);
         return i;
      }
   assert(0);
   return MODE_eighths;
}

int random_mode(void)
{
   int r = random(100);

   if (r < 35)
      return MODE_pattern;
   if (r < 65)
      return MODE_quarters;
   if (r < 90)
      return MODE_eighths;
   return MODE_sixteenths;
}

int song3_random_mode(void)
{
   int r = random(100);
   if (r < 50)
      return MODE_eighths;
   if (r < 80)
      return MODE_quarters;
   return MODE_sixteenths;
}

int song_orn_max[6] = { 50, 40, 100, 50, 100, 33 };
//int song_orn_max[6] = { 33,33,33,33,33,33 };

#define NUM_ORNAMENTS_PER_SECTION  8
int section_orn[NUM_ORNAMENTS_PER_SECTION];

void set_parts(void)
{
   int likelihood = 30 + (int) (60*cur_intensity);
   int n = 1, i, j;

   memset(part_pattern, 0, sizeof(part_pattern));
   if (g_songnum == SONG2) {
      for (i=0; i < NUM_PARTS; ++i)
         part_pattern[i] = random(3);
   }
   
   if (random(100) < likelihood) {
      ++n;
      if (random(100) < likelihood)
         ++n;
   }
   if (g_songnum == SONG5) {
      num_parts = 4;
      for (i=0; i < 4; ++i)
         part_mode[i] = i;
      return;
   }
   n = 3;
   num_parts = n;
   for (i=0; i < n; ++i) {
      int r;
      int tries=0;
      for(;;) {
         r = random_mode();
         for (j=0; j < i; ++j)
            if (r == part_mode[j])
               break;
         if (j == i)
            break;
         ++tries;
         if (tries > 100)
            break;
      }

      part_mode[i] = r;
   }

   for (i=0; i < n; ++i) {
      // meaningful values are 19..33
      part_orn[i] = section_orn[random(NUM_ORNAMENTS_PER_SECTION)];
   }

   if (g_songnum == SONG2) {
      for (i=0; i < n; ++i) {
         if (random(100) < likelihood)
            part_mode[i] = MODE_pattern;
      }
   }
   assert(num_parts <= NUM_PARTS);
}

void song3_set_parts(void)
{
   int likelihood = 30 + (int) (60*cur_intensity);
   int n = 1, i, j;
   #if 0
   if (random(100) < likelihood) {
      ++n;
      if (random(100) < likelihood)
         ++n;
   }
   #else
   n = 3;
   #endif
   num_parts = n;
   for (i=0; i < n; ++i) {
      int r;
      for(;;) {
         r = song3_random_mode();
         for (j=0; j < i; ++j)
            if (r == part_mode[j])
               break;
         if (j == i)
            break;
      }

      part_mode[i] = r;
   }
   assert(num_parts < NUM_PARTS);
}

typedef struct
{
   int part,voice;
} partvoice;

typedef struct
{
   char *arp[4];
} subsection_info;

subsection_info *cur_section;
int instrument_flags[I__count];

int part_priority[4] = { 1,0,2,3 };

int swap_violin_parts(void)
{
   if (instrument_part[I_violin1] < 0 && instrument_part[I_violin2] >= 0)
      return 1;
   if (instrument_part[I_violin1] >= 0 && instrument_part[I_violin2] >= 0)
      if (part_priority[part_mode[instrument_part[I_violin2]]] >= part_priority[part_mode[instrument_part[I_violin1]]])
         return 1;
   return 0;
}

float song_phase;
float section_arpeggio_modifier = 1.0;

int arp_phase_shift_chance(void)
{
   if (g_songnum == SONG5) {
      //     X   X
      //    X   X X
      //   X   X   X
      //  X   X     X
      float num = song_phase * 5;
      int segment = floor(num);
      float t = num - segment;
      if (segment == 4)
         return stb_lerp(t, 100, 0);
      else if (segment == 0)
         return stb_lerp(t, 0, 75);
      else if (segment == 1 || segment == 3)
         return stb_lerp(t, 25, 75);
      else
         return stb_lerp(t, 50, 100);
   } else
      return 0;
}

int is_part_sixteenths(int part)
{
   if (part_mode[part] == MODE_sixteenths)
      return 1;
   if (g_songnum == SONG3 || g_songnum == SONG5)
      return 0;
   if (part_mode[part] == MODE_pattern)
      if (part_pattern[part] >= FIRST_SIXTEENTH_PATTERN)
         return 1;
   return 0;
}

void init_instrument_modes(void)
{
   int i,j,k;
   int likelihood;
   int num_instruments;
   int cur=0;
   int *ilist=NULL;
   
   char *part_arp[NUM_PARTS];

   for (i=0; i < NUM_PARTS; ++i) {
      part_arp[i] = cur_section->arp[random(4)];

      if (g_songnum != SONG5) {
         char *s = part_arp[i];
         int initial_phase = strchr(s, ':') - s;
         if (initial_phase) {
            if (random(100) < 50)
               s += initial_phase;
            part_arp[i] = s;
         }
      }
   }

   if (g_songnum == SONG5) {
      part_arp[0] = cur_section->arp[0];
      part_arp[1] = cur_section->arp[1];
   }

   if (g_songnum == SONG3) {
      likelihood = 30 + (int) (60*cur_intensity);
      num_instruments = stb_linear_remap(likelihood+random(30), 30, 120, 2, I__count-3);
   } else {
      likelihood = 30 + (int) (60*cur_intensity);
      num_instruments = stb_linear_remap(likelihood+random(20), 20, 120, 1, I__count-1);
      num_instruments = stb_clamp(num_instruments, 2, I__count-3);
   }

   //num_instruments = I__count-3;
   //likelihood=120;

   i = 3;   
   if (g_songnum == SONG5)
      i = 5;
   for (; i > 0; --i) {
      int r = random(I__count);
      instrument_flags[r] = !instrument_flags[r];
   }
   for(;;) {
      int r;
      cur = 0;
      for (i=3; i < I__count; ++i)
         cur += instrument_flags[i];
      if (cur == num_instruments)
         break;
      r = random(I__count);
      instrument_flags[r] = (cur < num_instruments);
   }

   for (i=3; i < I__count; ++i) {
      if (g_songnum == SONG5)
         instrument_part[i] = instrument_flags[i] ? song5_random_mode() : -1;
      else
         instrument_part[i] = instrument_flags[i] ? random(num_parts) : -1;
      assert(instrument_part[i] < num_parts);

      if (instrument_part[i] >= 0) {
         if (g_songnum == SONG3) {
            instrument_voice[i] = 1 << random(3);
         } else {
            instrument_voice[i] = 1 << random(3);
         }
         if ((i == I_piano || i == I_harp) && random(100) < 50 && (my_tempo < sixteenth_tempo_threshhold_chord[i] || !is_part_sixteenths(instrument_part[i]))) {
            instrument_voice[i] = random_range(1,7);
         }

         if (my_tempo > sixteenth_tempo_threshhold[i] && is_part_sixteenths(instrument_part[i])) {
            instrument_part[i] = g_songnum == SONG5 ? song5_random_mode() : random(num_parts);
            if (my_tempo > sixteenth_tempo_threshhold[i] && is_part_sixteenths(instrument_part[i])) {
               if (g_songnum == SONG5)
                  instrument_part[i] = random_range(1,2);
               else
                  instrument_part[i] = -1;
               continue;
            }
         }
         stb_arr_push(ilist, i);
      }
   }

   if (g_songnum != SONG5 && g_songnum == SONG2) {
      stb_shuffle(ilist, stb_arr_len(ilist), sizeof(ilist[0]), stb_rand());

      k = 0;
      for (i=0; i < num_parts; ++i) {
         int req_voice[3] = { 1,1,1 };
         if (random(100) > likelihood)
            req_voice[random(3)] = 0;
         for (j=0; j < 3; ++j) {
            if (req_voice[j]) {
               int inst;
               int m = 1 << j;
               if (k >= stb_arr_len(ilist))
                  goto done;

               inst = ilist[k];
               if (is_part_sixteenths(i)) {
                  int z=0;
                  while (my_tempo > sixteenth_tempo_threshhold[inst]) {
                     stb_arr_delete(ilist, k);
                     stb_arr_push(ilist, inst);
                     ++z;
                     if (z == 10)
                        goto skip_part;
                  }
               }
               k++;
               if (inst == I_piano || inst == I_harp) {
                  if (my_tempo < sixteenth_tempo_threshhold_chord[inst] || !is_part_sixteenths(i)) {
                     if (j == 0) {
                        if (req_voice[1])
                           m |= 1 << 1;
                     }
                     if (j < 2)
                        if (req_voice[2])
                           m |= 1 << 2;
                     j = 3; // terminate j loop when we continue
                  }
               }
               instrument_part[inst] = i;
               instrument_voice[inst] = m;
            }
         }
        skip_part:
         ;
      }
     done:
      ;
   }

   for (i=0; i < I__count; ++i) {
      instrument_arp[i] = NULL;
      if (instrument_part[i] >= 0) {
         int p = instrument_part[i];
         if (random(100) < arpeggio_chance[g_songnum][part_mode[p]] * section_arpeggio_modifier) {
            instrument_arp[i] = part_arp[p];
            if (g_songnum != SONG5 && is_part_sixteenths(p)) {
               instrument_arp[i] = fix_sixteenths_arp(instrument_arp[i]);
            }
         }
       
         if (g_songnum == SONG5 && part_mode[p] == MODE_sixteenths && instrument_arp[i] == NULL)
            assert(0);

         if (g_songnum == SONG5 && random(100) < 40)
            instrument_arp[i] = cur_section->arp[random(4)];
               
         if (g_songnum == SONG5 && instrument_arp[i] != NULL && random(100) < arp_phase_shift_chance()) {
            char *s = instrument_arp[i];
            int initial_phase = strchr(s, ':') - s;
            if (initial_phase) {
               s += random(initial_phase);
               instrument_arp[i] = s;
            }
         }
      }
   }

#if 0
   if (g_songnum == SONG5) {
      int modes[5] = { 0,0,0,0,0 };
      float total=0;
      for (i=3; i < I__count; ++i) {
         if (instrument_part[i] >= 0) {
            modes[part_mode[instrument_part[i]]] += 1;  
            total += 1;
         }
      }
      printf("%3d %2d %d %d %d %d  %4.2f %4.2f %4.2f %4.2f\n", likelihood, (int) total, modes[0], modes[1], modes[2], modes[3], modes[0]/total, modes[1]/total, modes[2]/total, modes[3]/total);
   }
#endif

   if (swap_violin_parts()) {
      stb_swap(&instrument_flags[I_violin1], &instrument_flags[I_violin2], sizeof(instrument_flags[0]));
      stb_swap(&instrument_part [I_violin1], &instrument_part [I_violin2], sizeof(instrument_part[0]));
      stb_swap(&instrument_voice[I_violin1], &instrument_voice[I_violin2], sizeof(instrument_voice[0]));
   }

   stb_arr_free(ilist);
}

int count_repeated_durations(timedchord *part, int note_num, int step)
{
   int tm = 0, i, count=0;
   for (i=note_num; tm < WHOLE_NOTE; i += step) {
      if (part[i].c.scalenote[0] < 0)
         return count;
      if (part[i].duration == part[note_num].duration)   
         ++count;
      else
         return count;
      tm += part[i].duration;
   }
   return count;
}

int find_last_note(timedchord *part, int note_num)
{
   int tm = 0, i;
   for (i=note_num; tm < WHOLE_NOTE; ++i) {
      tm += part[i].duration;
   }
   return i-1;
}

chord next_phrase_chord;

enum {
   OW_every_measure,
   OW_after_chord_change,
   OW_before_chord_change,
};

int is_same_chord(chord *a, chord *b)
{
   assert(a->scalenote[0] >= 0);
   assert(b->scalenote[0] >= 0);
   return 0 == memcmp(a->scalenote, b->scalenote, sizeof(a->scalenote));
}

int is_ornamentation_when_true(timedchord *part, int i, int when, int final_added_measures)
{
   int j;

   if (part[i].c.scalenote[0] < 0)
      return 0;

   if (final_added_measures > 0) {
      if (i == stb_arr_len(part)-1)
         return 0;
      if (final_added_measures == 3 && i == stb_arr_len(part)-2)
         return 0;
   }

   switch (when) {
      case OW_every_measure      :
         return 1;
      case OW_before_chord_change: {
         int tm = part[i].duration;
         for (j=i+1; j < stb_arr_len(part) && tm < WHOLE_NOTE; ++j) {
            if (part[j].c.scalenote[0] >= 0) {
               if (!is_same_chord(&part[i].c, &part[j].c))
                  return 0;
            }
            tm += part[j].duration;
         }
         if (j == stb_arr_len(part))
            return is_same_chord(&part[i].c, &next_phrase_chord);
         return !is_same_chord(&part[i].c, &part[j].c);
         break;
      }
      case OW_after_chord_change :
         if (i == 0)
            return 1;
         for (j=i-1; j >= 0; --j) {
            if (part[j].c.scalenote[0] < 0)
               continue;
            return !is_same_chord(&part[i].c, &part[j].c);
         }
         return 0;
      default: assert(0); return 0;
   }
}

typedef struct
{
   int mode;
   int param;
   int when;
} ornamentation_param;

enum
{
   OM_none,
   OM_scale_up,
   OM_scale_down,
   OM_step_up,
   OM_step_down,
   OM_from_above,
   OM_from_below,
   OM_end_trans,
};

ornamentation_param mpo(int mode, int param, int when)
{
   ornamentation_param po;
   po.mode = mode;
   po.param = param;
   po.when = when;
   return po;
}

#define SAFE_PITCH(x)  ((x)+24)

ornamentation_param get_ornamentation_param(int ornamentation_number, int duration, int end_duration)
{
   if (ornamentation_number < 20) {
      if (end_duration == WHOLE_NOTE/8) {
         switch (ornamentation_number) {
            case 14: return mpo(  OM_end_trans, 2, OW_before_chord_change);
            case 15: return mpo(  OM_end_trans, 2, OW_before_chord_change);
            case 16: return mpo(  OM_end_trans, 1, OW_before_chord_change);
            case 17: return mpo(  OM_end_trans, 1, OW_before_chord_change);
            case 18: return mpo(  OM_end_trans, 4, OW_every_measure);
            case 19: return mpo(  OM_end_trans, 8, OW_every_measure);
         }
      } else if (end_duration == WHOLE_NOTE/16) {
         switch (ornamentation_number) {
            case 14: return mpo(  OM_end_trans, 2, OW_before_chord_change);
            case 15: return mpo(  OM_end_trans, 4, OW_before_chord_change);
            case 16: return mpo(  OM_end_trans, 6, OW_before_chord_change);
            case 17: return mpo(  OM_end_trans, 8, OW_before_chord_change);
            case 18: return mpo(  OM_end_trans, 4, OW_every_measure);
            case 19: return mpo(  OM_end_trans, 8, OW_every_measure);
         }
      }
   } else {
      if (duration == WHOLE_NOTE/8) {
         switch (ornamentation_number) {
            case 20: return mpo( OM_scale_up  , 4, OW_after_chord_change);
            case 21: return mpo( OM_scale_up  , 4, OW_every_measure);
            case 22: return mpo( OM_scale_down, 4, OW_after_chord_change);
            case 23: return mpo( OM_scale_down, 4, OW_every_measure);
            case 24: return mpo( OM_scale_up  , 8, OW_after_chord_change);
            case 25: return mpo( OM_scale_up  , 8, OW_every_measure);
            case 26: return mpo( OM_scale_down, 8, OW_after_chord_change);
            case 27: return mpo( OM_scale_down, 8, OW_every_measure);

            case 28: return mpo( OM_step_up   , 0, OW_after_chord_change);
            case 29: return mpo( OM_step_down , 0, OW_after_chord_change);

            case 30: return mpo( OM_from_above, 0, OW_after_chord_change);
            case 31: return mpo( OM_from_below, 0, OW_after_chord_change);
         }
      } else {
         switch (ornamentation_number) {
            case 20: return mpo( OM_scale_up  , 4, OW_after_chord_change);
            case 21: return mpo( OM_scale_up  , 4, OW_every_measure);
            case 22: return mpo( OM_scale_down, 4, OW_after_chord_change);
            case 23: return mpo( OM_scale_down, 4, OW_every_measure);
            case 24: return mpo( OM_scale_up  , 8, OW_after_chord_change);
            case 25: return mpo( OM_scale_up  , 8, OW_every_measure);
            case 26: return mpo( OM_scale_down, 8, OW_after_chord_change);
            case 27: return mpo( OM_scale_down, 8, OW_every_measure);
            case 28: return mpo( OM_scale_up  ,13, OW_after_chord_change);
            case 29: return mpo( OM_scale_up  ,13, OW_every_measure);
            case 30: return mpo( OM_scale_down,13, OW_after_chord_change);
            case 31: return mpo( OM_scale_down,13, OW_every_measure);
            case 32: return mpo( OM_step_up   , 0, OW_after_chord_change);
            case 33: return mpo( OM_step_down , 0, OW_after_chord_change);
         }
      }
   }
   return mpo(OM_none,0,0);
}

timedchord *ornament_part(timedchord *part, int partnum, int final_added_measures)
{
   int at_change = 1;
   int tm = 0;
   int i, last_tm = WHOLE_NOTE-1;
   timedchord *output = NULL;
   int *measure_start = NULL;

   for (i=0; i < stb_arr_len(part); ++i) {
      if (tm % WHOLE_NOTE < last_tm % WHOLE_NOTE) {
         stb_arr_push(measure_start, i);
         assert(tm%WHOLE_NOTE == 0);
      }
      last_tm = tm;
      tm += part[i].duration;
   }

   for (i=0; i < stb_arr_len(part); ++i) {
      timedchord tc = part[i];
      if (tc.c.scalenote[0] >= 0) tc.c.scalenote[0] = SAFE_PITCH(tc.c.scalenote[0]);
      if (tc.c.scalenote[1] >= 0) tc.c.scalenote[1] = SAFE_PITCH(tc.c.scalenote[1]);
      if (tc.c.scalenote[2] >= 0) tc.c.scalenote[2] = SAFE_PITCH(tc.c.scalenote[2]);
      stb_arr_push(output, tc);
   }

   for (i=0; i < stb_arr_len(measure_start); ++i) {
      int start_note = measure_start[i];
      int end_note   = find_last_note(part, measure_start[i]);
      int start_count = count_repeated_durations(part, start_note,  1);
      int end_count   = count_repeated_durations(part, end_note  , -1);
      int whole_measure = (start_note+start_count == end_note+1);

      ornamentation_param po;
      po = get_ornamentation_param(part_orn[partnum], part[start_note].duration, end_count ? part[end_note].duration : 0);

      if (po.mode != OM_none && is_ornamentation_when_true(part, start_note, po.when, final_added_measures)) {

         if (whole_measure)
            assert(start_count == end_count);
         else
            assert(start_count + end_count <= end_note - start_note + 1); // sum of beginning and ending runs can't be longer than whole measure

         if (part[start_note].duration <= WHOLE_NOTE/8) {
            if (start_count > po.param) start_count = po.param;
            if (end_count   > po.param)   end_count = po.param;
            switch (po.mode) {
               case OM_none:
                  break;
               case OM_scale_up: {
                  int i;
                  for (i=0; i < start_count; ++i) {
                     output[start_note+i].c.scalenote[0] -= (start_count-i);
                     output[start_note+i].c.scalenote[1] -= (start_count-i);
                     output[start_note+i].c.scalenote[2] -= (start_count-i);
                  }
                  break;
               }
               case OM_scale_down: {
                  int i;
                  for (i=0; i < start_count; ++i) {
                     output[start_note+i].c.scalenote[0] += (start_count-i);
                     output[start_note+i].c.scalenote[1] += (start_count-i);
                     output[start_note+i].c.scalenote[2] += (start_count-i);
                  }
                  break;
               }
               case OM_step_up: {
                  if (start_count >= 2) {
                     output[start_note+1].c.scalenote[0] += 1;
                     output[start_note+1].c.scalenote[1] += 1;
                     output[start_note+1].c.scalenote[2] += 1;
                  }
                  break;
               }
               case OM_step_down: {
                  if (start_count >= 2) {
                     output[start_note+1].c.scalenote[0] -= 1;
                     output[start_note+1].c.scalenote[1] -= 1;
                     output[start_note+1].c.scalenote[2] -= 1;
                  }
                  break;
               }
               case OM_from_above: {
                  if (start_count >= 2) {
                     output[start_note+0].c.scalenote[0] += 1;
                     output[start_note+0].c.scalenote[1] += 1;
                     output[start_note+0].c.scalenote[2] += 1;
                  }
                  break;
               }
               case OM_from_below: {
                  if (start_count >= 2) {
                     output[start_note+0].c.scalenote[0] -= 1;
                     output[start_note+0].c.scalenote[1] -= 1;
                     output[start_note+0].c.scalenote[2] -= 1;
                  }
                  break;
               }
               case OM_end_trans: {
                  int j,k;
                  if (end_count) {
                     for (j=0; j < 3; ++j) {
                        int prev_note = SAFE_PITCH(part[end_note].c.scalenote[j]);
                        int next_note =
                           (i == stb_arr_len(measure_start)-1 ? SAFE_PITCH(next_phrase_chord.scalenote[j])
                                                              : SAFE_PITCH(part[measure_start[i+1]].c.scalenote[j]));
                        if (i != stb_arr_len(measure_start)-1)
                           assert(measure_start[i+1] == end_note+1);
                        if (prev_note != next_note) {
                           int tlen = abs(po.param), dir;
                           dir = (prev_note < next_note) ? 1 : -1;
                           if (end_count < tlen)
                              tlen = end_count;
                           if (po.param > 0)
                              if (abs(prev_note - next_note) < tlen)
                                 tlen = abs(prev_note - next_note);
                           for (k=0; k < tlen; ++k)
                              output[end_note-k].c.scalenote[j] = next_note - (k+1) * dir;
                        }
                     }
                  }
                  break;
               }
            }
         }
      }
   }

   assert(timedchord_duration(output, stb_arr_len(output)) == timedchord_duration(part, stb_arr_len(part)));
   return output;
}

int should_ornament_part(int part)
{
   char *pattern;
   if (part_mode[part] != MODE_pattern)
      return 1;
   pattern = pattern_for_part(part);

   if (!strcmp(pattern, "XXX.XXX.")) return 1;
   if (!strcmp(pattern, "XXX..rXX")) return 1;
   if (!strcmp(pattern, "XXXXX...XXXXX...")) return 1;
   if (!strcmp(pattern, "X.X.X.X.X.X.XXXX")) return 1;

   return 0;
}

timedchord *play_against_timedchords(timedchord *changes, int total_time, int part, int final_added_measures, timedchord **ornamented)
{
   int stepsize;
   char *pattern = pattern_for_part(part);
   int *times = compute_times(changes);
   int voice_select = 7;
   timedchord *tc=0, *final;
   int j=0, start, k, len = strlen(pattern);
   int last_measure_start, z;

   z = timedchord_duration(changes, stb_arr_len(changes));
   if (final_added_measures)
      last_measure_start = z - WHOLE_NOTE;
   else
      last_measure_start = z + WHOLE_NOTE; // should never trigger

   stepsize = WHOLE_NOTE/strlen(pattern);

   start=0;
   for(start=0; start*stepsize < total_time; ) {
      timedchord c = { { -1,-1,-1 }, 0 };
      k = start+1;
      // advance the offset to next 'x' or 'r'
      while (pattern[k%len] == '.' && k*stepsize < total_time)
         ++k;

      if (pattern[start%len] == 'X') {
         int n=0,m;
         while (times[j+1] <= start*stepsize)
            ++j;
         for (m=0; m < 3; ++m)
            if (voice_select & (1 << m))
               c.c.scalenote[n++] = changes[j].c.scalenote[m];
      }

      {
         if (start*stepsize >= last_measure_start) {
            c.duration = WHOLE_NOTE/2; // @TODO randomize per song
            stb_arr_push(tc, c);
            c.c.scalenote[0] = c.c.scalenote[1] = c.c.scalenote[2] = -1;
            stb_arr_push(tc, c);
            break;
         } else {
            c.duration = (k-start)*stepsize;
            stb_arr_push(tc, c);
         }
      }
      start = k;
   }
   stb_arr_free(times);

   if (g_songnum != SONG5 && should_ornament_part(part)) {
      final = ornament_part(tc, part, final_added_measures);
      *ornamented = final;
   } else
      *ornamented = tc;
   return tc;
}

timedchord *song3_play_against_timedchords(timedchord *changes, int total_time, int part)
{
   int stepsize;
   char *pattern = song3_pattern_for_part(part);
   int *times = compute_times(changes);
   int voice_select = 7;
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
         int n=0,m;
         while (times[j+1] <= start*stepsize)
            ++j;
         for (m=0; m < 3; ++m)
            if (voice_select & (1 << m))
               c.c.scalenote[n++] = changes[j].c.scalenote[m];
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

#define SOLOIST_INTRO  -1

arranged_phrase *arrange_phrase(note *phrase, int soloist, int last_phrase)
{
   int duration, goal, fc, desired_chord_time, i;
   int final_added_measures = 0;
   chord last_note_chord;
   timedchord *tc, *changes, **plist = NULL, **plisto = NULL;
   arranged_phrase *p = malloc(sizeof(*p));
   memset(p,0,sizeof(*p));

   duration = phrase_duration(phrase, stb_arr_len(phrase));
   goal = duration + WHOLE_NOTE*1/4;
   goal = (goal + WHOLE_NOTE-1)/WHOLE_NOTE * WHOLE_NOTE;
   if (last_phrase)
      final_added_measures = random_range(1,6);
   goal += final_added_measures * WHOLE_NOTE;

   changes = make_timedchords(duration, goal, &last_note_chord, &next_phrase_chord);

   random_chord(&next_phrase_chord);

   p->voice[soloist][0] = adapt_melody(changes, phrase, range[soloist][0], range[soloist][1], &last_note_chord);
   fc = find_final_chord(&last_note_chord, p->voice[soloist][0]);

   if (g_songnum != SONG1) {
      desired_chord_time = phrase_duration(phrase, fc);

      {
         int i = stb_arr_len(changes);
         int end_chord_time = timedchord_duration(changes, i-1);
         int move_chord_time = desired_chord_time - end_chord_time;
         if (changes[i-2].duration + move_chord_time > 0) {
            changes[i-2].duration += move_chord_time;
            changes[i-1].duration -= move_chord_time;
         }
      }
   }

   p->voice[soloist][0] = add_rests_at_end(p->voice[soloist][0], goal-duration);

   for (i=0; i < num_parts; ++i) {
      timedchord *opart;
      timedchord *part = play_against_timedchords(changes, goal, i, final_added_measures, &opart);
      stb_arr_push(plist, part);
      stb_arr_push(plisto, opart);
   }

   for (i=I_piano; i < I__count; ++i) {
      int part = instrument_part[i];
      if (part == -1) {
         p->voice[i][0] = add_rests_at_end(NULL, goal);
      } else {
         tc = plist[part];
         instr = i;
         if (i == 8)
            i=i;
         p->voice[i][0] = make_chord_voice(tc, range[i][0], range[i][1], instrument_voice[i], i, plisto[part]);
      }
   }
   stb_arr_free(plist);

   {
      static int timer=0;
      if (timer == 0) {
         switch (g_songnum) {
            case SONG3:  pattern2[0] = random(SONG3_TIMING_PATTERNS); break;
            case SONG5:  pattern2[0] = random(SONG5_TIMING_PATTERNS); break;
            default:     pattern2[0] = random(TIMING_PATTERNS);
                         pattern2[1] = random(TIMING_PATTERNS);
                         pattern2[2] = random(TIMING_PATTERNS);
                         break;
         }
         timer = 2;
      }
      --timer;
   }

   return p;
}

arranged_phrase *song3_arrange_phrase(chord *start_chord, chord *end_chord, note *phrase, int soloist)
{
   int duration, goal, fc, second_chord_time, i;
   timedchord *tc, *changes, **plist = NULL;
   arranged_phrase *p = malloc(sizeof(*p));
   memset(p,0,sizeof(*p));

   p->voice[soloist][0] = song3_adapt_melody(start_chord, end_chord, phrase, range[soloist][0], range[soloist][1]);
   fc = find_final_chord(end_chord, p->voice[soloist][0]);

   duration = phrase_duration(phrase, stb_arr_len(phrase));
   goal = duration + WHOLE_NOTE*1/4;
   goal = (goal + WHOLE_NOTE-1)/WHOLE_NOTE * WHOLE_NOTE;

   second_chord_time = phrase_duration(phrase, fc);
   changes = song3_make_timedchords(start_chord, end_chord, second_chord_time, goal);

   p->voice[soloist][0] = add_rests_at_end(p->voice[soloist][0], goal-duration);

   for (i=0; i < num_parts; ++i) {
      timedchord *part = song3_play_against_timedchords(changes, goal, i);
      stb_arr_push(plist, part);
   }

   for (i=I_piano; i < I__count; ++i) {
      int part = instrument_part[i];
      if (part == -1) {
         p->voice[i][0] = add_rests_at_end(NULL, goal);
      } else {
         tc = plist[part];
         instr = i;
         p->voice[i][0] = song3_make_chord_voice(tc, range[i][0], range[i][1], instrument_voice[i], 0);
      }
   }
   stb_arr_free(plist);

   {
      static int timer=0;
      if (timer == 0) {
         pattern2[0] = random_range(0,6);
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

#if 0
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
#endif

arranged_phrase **song;
int has_midpoint;
float start, end, midpoint, midpoint_value;
int tooth1, tooth2;
float tooth1weight, tooth2weight;

void set_intensity_curve(float s, float e, float m, float mv)
{
   start = s;
   end   = e;
   midpoint = m;
   midpoint_value = mv;
   has_midpoint = 1;
}

void set_intensity_line(float s, float e)
{
   start = s;
   end   = e;
   has_midpoint = 0;
}

void set_teeth(int t1, float w1, int t2, float w2)
{
   tooth1 = t1;
   tooth1weight = w1;
   tooth2 = t2;
   tooth2weight = w2;
}

void clear_teeth(void)
{
   tooth1weight = 0;
   tooth2weight = 0;
}

float get_intensity_base(int i, int num)
{
   if (has_midpoint) {
      float t = stb_linear_remap(i, 0, num-1, 0, 1);
      if (t < midpoint)
         return stb_linear_remap(t, 0, midpoint, start, midpoint_value);
      else
         return stb_linear_remap(t, midpoint, 1, midpoint_value, end);
   } else
      return stb_linear_remap(i, 0, num-1, start, end);
}

float get_intensity(int i, int num)
{
   float b = get_intensity_base(i, num);
   b += stb_linear_remap(i % tooth1, 0, tooth1-1, 0, tooth1weight);
   b += stb_linear_remap(i % tooth2, 0, tooth2-1, 0, tooth2weight);
   return b;
}

typedef struct
{
   int key;
   char *arps[4];
   int ornamentation[NUM_ORNAMENTS_PER_SECTION];
} section_info;


section_info    section_config[1024];
subsection_info phrase_config[1024];

char *song_arp_table[32];

int modulation_suitability[12] =
{
   0,      // c -> c
    30,
    70,    // c -> d
    80,
    50,    // c -> e

   100,    // c -> f
     0,
   100,    // c -> g
    50,
    80,
    70,
    30,
};

int key_conflict(int a, int b)
{
   int dist = a-b;
   if (dist < 0) dist += 12;
   if (a == b)
      return 100;
   return 100-modulation_suitability[dist];
}

int find_structure_for_structure(structure_desc *structure, int find_index, int find_value, int return_index)
{
   int i;
   for (i=0; i < stb_arr_len(structure); ++i) {
      if (find_value == structure[i].state[find_index])
         return structure[i].state[return_index];
   }
   assert(0);
   return 0;
}

int *key_for_phrase;
void do_song(int songnum)
{
   int SONG4_NUM_CHANGES=1;
   int i,j,k, num_structure=0, num_structure2=0;
   int last_phrasetype = -1;
   int last_subsection = -1;
   int last_halfsubsection = -1;
   int last_key = -1, key;
   int keylist[4];
   int num_song_arps=0;
   int use_arp[6][32] = { 0 };
   int ornament_list[16];
   int simple_keys[2];

   structure_desc *structure;

   int instrument_change_time[6] =
   {
      8, 4, 8, 8, 16, 8
   };

   key_for_phrase = NULL;

   // pick a key with at most 3 #/b
   simple_keys[0] = (random_range(-3,3) * 7) % 12;

   // go 3,1,-1,-3 steps around the circle of fifths
   simple_keys[1] = (simple_keys[0] + (random_range(0,3)*2 - 3)*7) % 12;

   for (i=0; i < 2; ++i)
      if (simple_keys[i] < 0)
         simple_keys[i] += 12;

   switch (songnum) {
      case 4:
         for (i=0; i < 3; ++i) {
            use_arp[0][random(arp_table_len[0])] = 1;
            use_arp[2][random(arp_table_len[2])] = 1;
         }
         break;
      case 0:
      case 1:
      case 3:
      case 5:
         for (i=0; i < 4; ++i) {
            use_arp[0][random(arp_table_len[0])] = 1;
         }
         break;
      default: assert(0);
   }

   for (i=0; i < 6; ++i)
      for (j=0; j < arp_table_len[i]; ++j)
         if (use_arp[i][j])
            song_arp_table[num_song_arps++] = arp_table[i][j];

   if (stb_arr_len(vocals) < 64*2)
      structure = generate_structure(2);
   else if (stb_arr_len(vocals) < 256*2)
      structure = generate_structure(3); 
   else
      structure = generate_structure(4);

   assert(stb_arr_len(structure) <= stb_arr_len(vocals)/2);


   if (songnum == SONG4) {
      SONG4_NUM_CHANGES = random_range(15,30);
      //assert(stb_arr_len(vocals) > 80*2);
   }

   for(i=0; i < stb_arr_len(structure) != 0; ++i) {
      if (structure[i].state[0] > num_structure)
         num_structure = structure[i].state[0];
      if (structure[i].state[1] > num_structure2)
         num_structure2 = structure[i].state[1];
   }

   for (i=0; i < 4; ++i) {
      int badness[12];
      int sum = 0, wt;
      for (k=0; k < 12; ++k) {
         badness[k] = 100;
         for (j=0; j < i; ++j)
            badness[k] -= key_conflict(k, keylist[j]);
         if (badness[k] < 0)
            badness[k] = 0;
         sum += badness[k];
      }
      if (sum == 0)
         k = keylist[random(i)];
      else {
         wt = random(sum);
         sum = 0;
         for (k=0; k < 12; ++k) {
            sum += badness[k];
            if (wt < sum)
               break;
         }
      }
      keylist[i] = k;
   }

   for (i=0; i < 16; ++i)
      ornament_list[i] = random_range(19, song_orn_max[songnum]);

   for (i=0; i <= num_structure2; ++i) {
      int j;
      for (j=0; j < NUM_ORNAMENTS_PER_SECTION; ++j) {
         section_config[i].ornamentation[j] = ornament_list[random(16)];
      }
      if (songnum == 4) {
         int tab0,tab1;
         if (i == 0) {
            tab0 = 0;
            tab1 = 2;
         } else if (i == 1) {
            tab0 = random_range(0,3);
            tab1 = random_range(4,5);
         } else {
            tab1 = random_range(0,5);
            if (tab1 < 2)
               tab0 = random_range(2,3);
            else if (tab1 < 4)
               tab0 = random_range(0,1);
            else
               tab0 = random_range(0,3);
         }
         section_config[i].key = keylist[random(4)];
         section_config[i].arps[0] = arp_table[tab0][random(arp_table_len[tab0])];
         do {
            section_config[i].arps[1] = arp_table[tab0][random(arp_table_len[tab0])];
         } while (section_config[i].arps[0] == section_config[i].arps[1]);
         section_config[i].arps[2] = arp_table[tab1][random(arp_table_len[tab1])];
         do {
            section_config[i].arps[3] = arp_table[tab1][random(arp_table_len[tab1])];
         } while (section_config[i].arps[2] == section_config[i].arps[3]);
      } else
         section_config[i].key = 0;
   }

   for (i=0; i <= num_structure; ++i) {
      if (songnum == 4) {
         int s2 = find_structure_for_structure(structure, 0, i, 1);
         assert(s2 <= num_structure2);
         if (random(100) < 80) {
            phrase_config[i].arp[0] = section_config[s2].arps[random_range(0,1)];
            phrase_config[i].arp[1] = section_config[s2].arps[random_range(2,3)];
         } else {
            int j;
            for (j=0; j < 4; ++j)
               phrase_config[i].arp[j] = section_config[s2].arps[j];
            stb_shuffle(phrase_config[i].arp, 4, sizeof(phrase_config[i].arp[0]), stb_rand());
         }
         phrase_config[i].arp[2] = phrase_config[i].arp[0];
         phrase_config[i].arp[3] = phrase_config[i].arp[1];
      } else
         for (j=0; j < 4; ++j)
            phrase_config[i].arp[j] = song_arp_table[random(num_song_arps)];
   }

   random_chord(&next_phrase_chord); // @TODO start & end on same chord

   for (i=0; i < stb_arr_len(vocals); ++i) {
      char **syl = vocals[i].syllables;
      arranged_phrase *p;
      note *nlist;
      int subsection = (int) stb_linear_remap(i, 0, stb_arr_len(vocals), 0, stb_arr_len(structure));
      int half_subsection = (int) stb_linear_remap(i, 0, stb_arr_len(vocals), 0, 2*stb_arr_len(structure));
      int phrasetype = structure[subsection].state[0];

      song_phase = stb_linear_remap(i, 0, stb_arr_len(vocals), 0, 1);


      //printf("%d\n", subsection);

      cur_section = &phrase_config[phrasetype];

      cur_intensity = get_intensity(i, stb_arr_len(vocals));

      key = section_config[structure[subsection].state[1]].key;

      if (songnum != SONG3 && songnum != SONG5 && songnum != SONG2) {
         float f = (float) i / stb_arr_len(vocals), t;
         f *= 3;
         t = f;
         if (t > 1)
            t = t-f;
         if (t < 0)
            t = -f;
         assert(t >= 0 && t <= 1.01);
         if (f > 2)
            t = stb_lerp(t, 0.5, 0.75);
         else
            t = stb_lerp(t, 0.5, 1.0);
         section_arpeggio_modifier = t;
      } else
         section_arpeggio_modifier = 1;

      if (songnum == SONG1 || songnum == SONG6) {
         key = simple_keys[0];
         if (i > stb_arr_len(vocals)/4 && i < stb_arr_len(vocals)*3/4)
            key = simple_keys[1];
      }

      memcpy(section_orn, section_config[structure[subsection].state[1]].ornamentation, sizeof(section_orn));

      if (i != 0 && songnum == SONG4) {
         if ((int) stb_linear_remap(i  ,0,stb_arr_len(vocals),0,SONG4_NUM_CHANGES) ==
             (int) stb_linear_remap(i-1,0,stb_arr_len(vocals),0,SONG4_NUM_CHANGES)) {
            key = exotic_compatible_key(last_key, -1);
         }
      }

      if (key != last_key)
         set_key(key, scale);
      last_key = key;

      stb_arr_push(key_for_phrase, printable_key(key));

      if (songnum == SONG5) {
         if (subsection != last_subsection)
            set_parts();
         if (half_subsection != last_halfsubsection)
            init_instrument_modes();
      } else if (i % instrument_change_time[songnum] == 0) {
         set_parts();
         init_instrument_modes();
      }

      nlist = generate_phrase(stb_arr_len(syl), syl);

      p = arrange_phrase(nlist, vocals[i].voice, i == stb_arr_len(vocals)-1);
      stb_arr_push(song, p);

      last_subsection = subsection;
      last_phrasetype = phrasetype;
      last_halfsubsection = half_subsection;
   }
}

void song3_do_song(void)
{
   int i;
   int songnum = 2;

   int instrument_change_time[6] =
   {
      32, 16, 8, 32, 16, 8
   };

   for (i=0; i < stb_arr_len(vocals); ++i) {
      char **syl = vocals[i].syllables;
      arranged_phrase *p;
      chord a,b;
      note *nlist;
      cur_intensity = get_intensity(i, stb_arr_len(vocals));
      // printf("%d\n", (int) (100*cur_intensity));

      if (i % instrument_change_time[songnum] == 0) {
         song3_set_parts();
         init_instrument_modes();
      }

      nlist = generate_phrase(stb_arr_len(syl), syl);
      song3_random_chord(&a);
      do
         song3_random_chord(&b);
      while (a.scalenote[0] == b.scalenote[0] && a.scalenote[1] == b.scalenote[1]);

      p = song3_arrange_phrase(&a, &b, nlist, vocals[i].voice);
      if (i == OUTPUT_START && 0) {
         p->voice[I_mezzo][0] = p->voice[I_soprano][0];
         p->voice[I_bass ][0] = p->voice[I_soprano][0];
      }
      stb_arr_push(song, p);
   }
}

void create_music(int songnum)
{
   song = NULL;
   g_songnum = songnum;

   switch (songnum) {
      case SONG1: set_intensity_line(0.0, 0.25);            set_teeth(16, 0.2, 64, 0.2); break;
      case SONG4: set_intensity_line(0.1, 0.3);             set_teeth( 8, 0.2,128, 0.4); break;
      case SONG3: set_intensity_curve(0.2, 0.6, 0.9, 0.4);  set_teeth(32, 0.2, 64, 0.3); break;
      case SONG2: set_intensity_line(0.5, 0.95);            set_teeth(16, 0.2, 32, 0.2); break;
      case SONG5: set_intensity_line(0.2, 0.95);            set_teeth( 8, 0.1, 16, 0.1); break;
      case SONG6: set_intensity_curve(0.5, 0.5, 0.9, 1.0);  set_teeth(4, 0.2, 16, 0.2);  break;
      default: assert(0);
   }

   if (songnum == SONG3)
      song3_do_song();
   else
      do_song(songnum);
}

void do_instrument_music(int inst)
{
   int i,j;
   int cur_volume = 99;

   for (i=OUTPUT_START; i < OUTPUT_END && i < stb_arr_len(song); ++i) {
      int current_time = 0;
      int in_melisma = 0;
      //char **syl = vocals[i].syllables;
      voicenote *nlist = song[i]->voice[inst][0];

      // C F Bb Eb Ab Db (Gb)
      // C G D A E B F#

      // C Db E Eb F F# G Ab A Bb B

      if (i == OUTPUT_START || (key_for_phrase[i] != key_for_phrase[i-1])) {
         current_key_is_flat = 0;
         switch (key_for_phrase[i]) {
            case  0: fprintf(f, "\\key c   \\major "); break;
            case  1: fprintf(f, "\\key des \\major "); current_key_is_flat = 1; break;
            case  2: fprintf(f, "\\key d   \\major "); break;
            case  3: fprintf(f, "\\key ees \\major "); current_key_is_flat = 1; break;
            case  4: fprintf(f, "\\key e   \\major "); break;
            case  5: fprintf(f, "\\key f   \\major "); current_key_is_flat = 1; break;
            case  6: fprintf(f, "\\key fis \\major "); break;
            case  7: fprintf(f, "\\key g   \\major "); break;
            case  8: fprintf(f, "\\key aes \\major "); current_key_is_flat = 1; break;
            case  9: fprintf(f, "\\key a   \\major "); break;
            case 10: fprintf(f, "\\key bes \\major "); current_key_is_flat = 1; break;
            case 11: fprintf(f, "\\key b   \\major "); break;
         }
      }

      if (nlist == NULL) {
         int duration;
         for (j=0; j < I__count; ++j) {
            if (song[i]->voice[j][0] != NULL) {
               nlist = song[i]->voice[j][0];
               break;
            }
         }
         duration = voicenote_phrase_duration(nlist, stb_arr_len(nlist));
         nlist = add_rests_at_end(NULL, duration);
      }

      for (j=0; j < stb_arr_len(nlist); ++j) {
         int dur = nlist[j].duration;

         pre_duration(dur);

         if (!in_melisma &&  nlist[j].melisma)
            fprintf(f, "( ");
         in_melisma = nlist[j].melisma;

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
            if (nlist[j].midinote[0] != 0 && nlist[j].volume != cur_volume) {
               static char *dynamics[] = {
                  "ppp",
                  "pp",
                  "p",
                  "mp",
                  "mf",
                  "f",
                  "ff",
                  "fff",
               };
               cur_volume = nlist[j].volume;
               fprintf(f, "\\%s", dynamics[cur_volume]);
            }
            fprintf(f, " ");
            dur -= step_time;
            current_time += step_time;
            if (current_time >= WHOLE_NOTE)
               current_time -= WHOLE_NOTE;
            if (dur && nlist[j].midinote[0] != 0)
               fprintf(f, " ~ ");
         }
         if (in_melisma  && (j+1 == stb_arr_len(nlist) || !nlist[j+1].melisma))
            fprintf(f, ") ");

         post_duration(dur);
         fprintf(f, "\n");
      }
   }
}

// Lyr -- ics }\n");
void do_vocal_lyrics(int inst)
{
   int i,j, count=0;
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
               count += 2;
            } else {
               fprintf(f, "%s ", s);
               count += 1;
            }
            free(raw_s);
            if (count > 12) {
               fprintf(f, "\n");
               count = 0;
            }
         }
         if (count)
            fprintf(f, "\n");
      } else {

      }
   }
}


//  bass: G2..E4 (F2..F4)
//  soprano: D4..B5 (C4..C6)
//  mezzo-soprano: B3..G5 (A3..A5)

int tempos[] = { 90, 95, 100, 105, 110, 115, 120, 125, 130, 135 };

char *songnames[] =
{
   "Primo Periodo",
   "Secondo Periodo, Parte Prima",
   "Secondo Periodo, Parte Seconda",
   "Terzo Periodo",
   "Quarto Periodo, Parte Prima",
   "Quarto Periodo, Parte Seconda"
};

#if 1
void write_music(int songnum)
{
   int i;
   syllable_map = stb_sdict_new(1);

   my_tempo = tempos[songnum];
   create_music(songnum);

   f = fopen(stb_sprintf("music_%d.ly",songnum+1), "w");
   fprintf(f, "\\version \"2.18.2\"\n\n");
   fprintf(f, "mytempo = %d\n", my_tempo);
   fprintf(f, "pianoRHMusic = { r }\n");

   fprintf(f, "piecename = \"%s\"\n", songnames[songnum]);



   for (i=0; i < I__count; ++i) {
      static char *name[I__count][2] = {
         { "sopranoMusic",   NULL, },
         { "mezzoMusic",     NULL, },
         { "vbassMusic",     "bass" },
         { "pianoLHMusic",   "bass" },
         { "fluteMusic",     NULL, },
         { "clarinetMusic",  NULL, },
         { "trumpetMusic",   NULL, },
         { "hornMusic",      "bass", },
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

      if (i == I_flute)
         fprintf(f, "  \\tempo 4 = %d\n", my_tempo);

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
int hack_count[3][24];
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
   "Framp-ton", "Ray-nor", "John-son", "Will-iams", "Lar-kin",
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
   "Mezzo",
   "Bass",
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
         if (0 == strcmp(pending.cur_line + stb_arr_len(pending.cur_line) - 2, "_.")) {
            stb_arr_pop(pending.cur_line);
            stb_arr_last(pending.cur_line) = 0;
         }
         add.voice = pending.voice;

         add.cur_line = strdup(pending.cur_line);
         add.syllables = syllablize(add.cur_line);

         #if 0
         if (pending.voice >= 0) {
            printf("%s: ", voicename[pending.voice]);
            for (i=0; i < stb_arr_len(add.syllables); ++i)
               printf("%s ", add.syllables[i]);
            printf("\n");
         }
         #endif

         stb_arr_push(all_vocals[g_songnum], add);
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

void start_song(void)
{
   int i;
   g_songnum = num_songs++;
   for (i=random_range(1,16); i >= 0; --i) {
      add_lyrics(-1, "la ");
   }
   add_lyrics(-1, "la.");
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

char *ordinal_number(char *s, int n)
{
   switch (n) {
      case  1: strcpy(s, "first"); break;
      case  2: strcpy(s, "second"); break;
      case  3: strcpy(s, "third"); break;
      case  4: strcpy(s, "fourth"); break;
      case  5: strcpy(s, "fifth"); break;
      case  6: strcpy(s, "sixth"); break;
      case  7: strcpy(s, "seventh"); break;
      case  8: strcpy(s, "eighth"); break;
      case  9: strcpy(s, "ninth"); break;
      case 10: strcpy(s, "tenth"); break;
      case 11: strcpy(s, "eleventh"); break;
      case 12: strcpy(s, "twelfth"); break;
      case 13: strcpy(s, "thirteenth"); break;
      case 14: strcpy(s, "fourteenth"); break;
      case 15: strcpy(s, "fifteenth"); break;
      default: assert(0);
   }
   return s + strlen(s);
}

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
   V1,   // play-by-play
   V2,   // criticism of play
   V3,   // stats
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
         assert(n != 0);
         if (*s == '$') {
            strcpy(t, playername[n+PLAYER_OFFSET][PLAYERNAME_full ]);
            ++s;
         } else if (s[0] == '\'' && s[1] == 's') {
            strcpy(t, playername[n+PLAYER_OFFSET][PLAYERNAME_short]);
            t += strlen(t);
            if (t[-1] == 's')
               strcpy(t, "'");
            else
               strcpy(t, "'s");
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
         if (s[0] == 't' && s[1] == 'h') {
            s += 2;
            t = ordinal_number(t, n);
         } else
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
   SP_ft_visits,

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

#if 0
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
      cs->priority_recent = square(time_since_last - 1);

   if (cs->aspect >= ASPECT_season) {
      if (prev_timestamp != 0)
         cs->priority_recent = 0;
   }

   cs->priority_interestingness = compute_stat_interestingness(cs->stat, cs->player, cs->aspect);

   cs->priority = cs->priority_interestingness * cs->priority_recent;
}
#endif

#define TEAM0_PLAYER     11
#define TEAM1_PLAYER    -11

int player_for_team[2] = { TEAM0_PLAYER, TEAM1_PLAYER };

int team_for_player(int player)
{
   return player<0 ? TEAM_1 : TEAM_0;
}

#if 0
color_statistic *active_list = NULL;

static voied color_add_analysis(int stat, int player)
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
            stb_arr_push(active_listlist, cs);
      }
   }
   return list;
}
#endif

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

#define MAX_LINES  20000

int history[MAX_LINES][10];
int h_count[MAX_LINES];
char done[MAX_LINES];
char never[MAX_LINES][16];

// can only ever say this once
int once(int line)
{
   if (done[line])
      return 0;
   done[line] = 1;
   return 1;
}

int random_neverrepeat(int range, int line)
{
   int i,n;
   for (i=0; i < range*2; ++i) {
      n = stb_rand() % range;
      if (never[line][n])
         continue;
      never[line][n] = 1;
      return n;
   }
   return -1;
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

#define random_neverrepeat(range) random_neverrepeat(range, __LINE__)
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

int did_announce_score;
void points_announce(int voice, int force)
{
   did_announce_score = 1;

   force_team_mode = stb_rand() & 1;

   assert(voice != V2);

   enable_color();

   // @TODO variations
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

void announce_score(int voice)
{
   points_announce(voice, 1);
}

int pending_announce;

void add_points(int team, int points, int forbid_announce)
{
   int force=1;
   int voice = V3;
   int leader = (score[0] < score[1] ? TEAM_1 : score[0] > score[1] ? TEAM_0 : -1);
   int randomly;

   if (random(100) < 30)
      voice = V1;

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

   if (random(3) == 0 || force || this_event_time > 45*60 || force_announce_points) {
      if (forbid_announce && !force)
         pending_announce = voice+1;
      else
         points_announce(voice, force);
   }
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
                  case 0: sing(V1, "$ re-plac-es $.", subs[j][1], subs[j][0]); break;
                  case 1: sing(V1, "$ re-placed by $.", subs[j][0], subs[j][1]); break;
                  case 2: sing(V1, "$ comes in for $.", subs[j][1], subs[j][0]); break;
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

#if 0
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
#endif

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
         #if 0
         if (random(100) < 50)
            sing(V2, "O-pin-ion here. I have a lot of o-pin-ions. I am glad to share them with you."); // @MORECOLOR
         else
            sing(V3, "Sta-tis-tic here. This is com-pli-ca-ted. In-deed."); // @MORECOLOR
         #endif
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
         #if 0
         if (random(100) < 50)
            sing(V2, "O-pin-ion here. I have a lot of o-pin-ions. I am glad to share them with you."); // @MORECOLOR
         else
            sing(V3, "Sta-tis-tic here. This is com-pli-ca-ted. In-deed."); // @MORECOLOR
         #endif
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
   if (game_state.position == (tm ? -1.0f : 1.0f) && fabs(position) < 1.0) {
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
   } else if (fabs(game_state.position) < 1.0 && fabs(position) == 1.0) {
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
      if (random(100) < 20) {
         if (once()) sing(V2, "That was a beau-ti-ful bounce pass.");
         else if (once()) sing(V2, "Good bounce pass.");
         else if (once()) sing(V2, "That was well placed.");
         disable_color();
      }
   } else
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

   if (can_do_color() && random(1000) < 15) {
      if (once()) sing(V2, "That was a cra-zy chance he took with that pass.");
      else if (once()) sing(V2, "Nice pass.");
      else if (once()) sing(V2, "Ris-ky choice, that pass.");
      disable_color();
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
int foul_mode;
struct
{
   int active;
   int type, player, assist, made, blocker, flavor;
} pending_comment;


void no_process_foul(void)
{
   assert(!foul);
}
int color_from_queue=0;
void color_shot_commentary(int type, int player, int assist, int made, int blocker, int flavor, int queue);

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
      if (who_was_fouled == 0)
         who_was_fouled = team_roster[!tm][0];
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
      if (this_event_time > 48*60 - 30 && score[tm] <= score[!tm]+1) {
         // 4 instances
         switch (random_nonrepeat(5)) {
            case 0: sing(V1, "In-ten-tion-al foul by $.", foul_player); break;
            case 1: sing(V1, "In-ten-tion-al foul by $.", foul_player); break;
            case 2: sing(V1, "$ with the in-ten-tion-al foul.", foul_player); break;
            case 3: sing(V1, "Foul called on $.", foul_player); break;
            case 4: sing(V1, "$ makes an in-ten-tion-al foul.", foul_player); break;
            default: assert(0);
         }

         disable_color();
         switch (random_neverrepeat(3)) {
            case -1: break;
            case  0: sing(V2, "They have to foul at this point if they want to have a chance to catch up."); break;
            case  1: sing(V2, "That's what you've got-ta do."); break;
            case  2: break;
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
                  case 2: sing(V1, "Foul on the play. Per-son-al foul for $, so $ will go to the foul line.", foul_player, who_was_fouled); break;
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

   if (foul_mode) {
      if (foul_mode > 0) {
         switch (random_nonrepeat(3)) {
            case 0: sing(V2, "Smart play by $ in-stead of let-ting $ get the easy two.", foul_player, who_was_fouled); break;
            case 1: sing(V2, "As a de-fend-er you've just got to do that some-times."); break;
            case 2: break;
            default: assert(0);
         }
      } else {
         switch (random_nonrepeat(3)) {
            case 0: sing(V2, "I think that was too ag-gres-sive on the de-fend-er's part.", foul_player); break;
            case 1: sing(V2, "It looks like $ is hav-ing a rough time cov-er-ing $.", foul_player, who_was_fouled); break;
            case 2: break;
            default: assert(0);
         }
      }
      foul_mode = 0;
   }

   if (pending_comment.active) {
      pending_comment.active = 0;
      color_from_queue = 1;
      color_shot_commentary(pending_comment.type, pending_comment.player, pending_comment.assist, pending_comment.made, pending_comment.blocker, pending_comment.flavor, 0);
      assert(pending_comment.active == 0);
   }

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
            case 2: sing(V1, "$ gets a pick from $_ ", player, random_player(tm,player)); break;
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
         case 8: sing(V1, "puts the ball up, but mis-ses it."); break;
         default: assert(0);
      }
      foul_mode = 1;
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
         case 1: sing(V1, "bad shot, looks like contact from $. ", fouled_by); return;
         case 2: sing(V1, "no good."); return; 
         case 3: sing(V1, "$ was all over him_ ", fouled_by); break;
         case 4: sing(V1, "a bad shot, but $ was all over him. ", fouled_by); return;
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
      foul_mode = -1;
   }
}

stat_streakable *SKSTAT(int player, int stat)
{
   return &stats[player][SG_tonight].streakable[stat];
}


stat_makeable *SSTAT(int player, int stat)
{
   return &stats[player][SG_tonight].streakable[stat].game;
}

stat_plain *PSTAT(int player, int stat)
{
   return &stats[player][SG_tonight].plain[stat];
}

void shot_stats(int player, int type, int made)
{
/*
  *points for the night after scoring
  *layup/field goal/3pt/total points for the night after such events
  *layup/field goal/3pt/total points for the team after such events
  *layup/field goal/3pt/total percentages for the night after such events
  *1st point
  *1st layup
  *1st field goal
  *1st 3 pointer

  +layup/field goal/3pt/total points compare halfs for player after such events
  +layup/field goal/3pt/total points compare halfs for team after such events
  +layup/field goal/3pt/total points compare halfs for teams after such events
*/
   if (SKSTAT(player_for_team[0], SS_fg)->quarter[quarter].made +
       SKSTAT(player_for_team[1], SS_fg)->quarter[quarter].made == 1)
   {
      if (SKSTAT(player_for_team[0], SS_ft)->quarter[quarter].made == 0
          && SKSTAT(player_for_team[1], SS_ft)->quarter[quarter].made == 0)
      {
         if (random(100) < 70) {
            switch (quarter) {
               case 0: sing(V3, "That's the first points of the game."); break;
               case 1: sing(V3, "$ has the first points of the sec-ond quar-ter.", player); break;
               case 2: sing(V3, "$ gets the second half started with # points.", player, SKSTAT(player, SS_fg)->quarter[quarter].points); break;
               case 3: sing(V3, "# points from $ o-pens the sco-ring in the fi-nal quar-ter.", SKSTAT(player, SS_fg)->quarter[quarter].points, player); break;
               default: assert(0);
               break;
            }
            return;
         }
      }
   }

   if (this_event_time < 90)
      return;

   if (SSTAT(player, SS_fg)->made == 1) {
      if (random(100) < 60 || SSTAT(player, SS_fg)->attempts == 1) {
         if (SSTAT(player, SS_fg)->attempts > 3)
            // 1..3
            switch (random_nonrepeat(3)) {
               case 0: sing(V3, "That's the first shot he's made after # misses earlier.", SSTAT(player, SS_fg)->attempts-1); return;
               case 1: sing(V3, "That's $'s first bas-ket to-night on his #th attempt.", player, SSTAT(player, SS_fg)->attempts); return;
               case 2: sing(V3, "On his #th try % fi-nal-ly gets the ball to go in.", SSTAT(player, SS_fg)->attempts, player); return;
               default: assert(0);
            }
         else if (SSTAT(player, SS_fg)->attempts == 1) {
            if (type == SHOT_3pt) {
               if (once()) {
                  sing(V3, "For $'s first shot of the night he's sunk a three point-er.", player);
                  return;
               }
            } else {
               if (once()) {
                  sing(V3, "That's $'s first shot of the night.", player);
                  return;
               }
            }
         } else { // made 1 of 2 or 3
            if (type == SHOT_3pt) {
               if (once()) {
                  sing(V3, "$'s first made shot to-night is a three point-er. He's # for # o-ver-all.", player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts);
                  return;
               }
            } else {
               switch(random_nonrepeat(5)) {
                  default: assert(0);
                  case 0: sing(V3, "That's $'s first bas-ket to-night.", player); break;
                  case 1: sing(V3, "$'s first made shot here.", player); break;
                  case 2: sing(V3, "$ is # for #.", player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
                  case 3: sing(V3, "That's the first shot $ has made.", player); break;
                  case 4: sing(V3, "The first bas-ket from $, put-ting him at # for # so far to-night.", player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
               }
               return;
            }
         }
      }
   }

   if (SSTAT(player, SS_fg)->made == 0) {
      // ~10
      switch (random_nonrepeat(5)) {
         default:
         case 0:
         case 1:
         case 2:
         case 3:
         case 4:  break;  // @TODO
      }
      return;
   }

   if (type == SHOT_3pt ? SSTAT(player, SS_f3)->attempts > 1 : SSTAT(player, SS_fg)->attempts > 1) { 
      // ~130 times
      ++hack_count[0][0];
      if (random(100) < 40) {
         if (type == SHOT_3pt) {
            // 3..12 times if random(100) < 30 above
            switch (random_nonrepeat(6)) {
               case 0: sing(V3, "$ is # for # to-night for three point-ers.", player, SSTAT(player,SS_f3)->made, SSTAT(player,SS_f3)->attempts); break;
               case 1: sing(V3, "$ is # for # for three point shots.",        player, SSTAT(player,SS_f3)->made, SSTAT(player,SS_f3)->attempts); break;
               case 2: sing(V3, "For three points, $ is # for # to-night.",   player, SSTAT(player,SS_f3)->made, SSTAT(player,SS_f3)->attempts); break;
               case 3: sing(V3, "$ is # for # for three points to-night."   , player, SSTAT(player,SS_f3)->made, SSTAT(player,SS_f3)->attempts); break;
               case 4: sing(V3, "$ is # for # at three point-ers."   ,        player, SSTAT(player,SS_f3)->made, SSTAT(player,SS_f3)->attempts); break;
               case 5: sing(V3, "For three points, $ is at # for #.",         player, SSTAT(player,SS_f3)->made, SSTAT(player,SS_f3)->attempts); break;
            }
            return;
         } else {
            // 30..50 times if random(100) < 30 above
            switch (random_nonrepeat(6)) {
               case 0: sing(V3, "$ is # for # to-night.",                player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
               case 1: sing(V3, "$ is # for # to-night from the field.", player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
               case 2: sing(V3, "$ has shot # for # so far.",            player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
               case 3: sing(V3, "$ has shot # for # so far to-night.",   player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
               case 4: sing(V3, "That puts $ at # for #.",               player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
               case 5: sing(V3, "That puts $ at # for # tonight.",       player, SSTAT(player,SS_fg)->made, SSTAT(player,SS_fg)->attempts); break;
            }
            return;
         }
      }
   }

   // didn't say anything
}

int wide_open, contested, drive, got_by, picked;
void color_shot_commentary(int type, int player, int assist, int made, int blocker, int flavor, int queue)
{
   int team;

   if (queue) {
      pending_comment.active = 1;
      pending_comment.type = type;
      pending_comment.player = player;
      pending_comment.assist = assist;
      pending_comment.made = made;
      pending_comment.blocker = blocker;
      pending_comment.flavor = flavor;
      return;
   }

   if (!can_do_color())
      return;

   team = (player < 0) ? TEAM_1 : TEAM_0;

   // if !made, we do color commentary in the rebound
   if (made) {
      // other status information about shot so color doesn't conflict with pbp
      // int wide_open, contested, drive, got_by;
      if (assist) {
         // assisted:
         //   layup: 2*none, reverse, 3*dunk
         //   3pt: 17
         //   jump: 9*none, 2*hook
         switch (type) {
            case SHOT_layup:
               switch (flavor) {
                  default: assert(0);
                  case F_none:
                     switch (random_neverrepeat(3)) {
                        default: break;
                        case 0: sing(V2, "Good work by $ and by $.", player, assist); break;
                        case 1: sing(V2, "I did-n't think that was go-ing to go in. Cred-it to both play-ers for mak-ing it work."); break;
                        case 2: sing(V2, "Sol-id work by both play-ers on that play."); break;
                     }
                     break;
                  case F_reverse:
                     if (once()) {
                        sing(V2, "Good pass by $ to $ as he was head-ing under the bas-ket. And an ef-fort-less look-ing shot by $.", assist, player, player);
                     }
                     break;
                  case F_dunk:
                     switch (random_neverrepeat(3)) {
                        default: break;
                        case 0: sing(V2, "That was a great pass by $. He saw the o-pen man and got him the ball.", assist); break;
                        case 1: sing(V2, "That is how the game is played. Per-fect team-work there."); break;
                        case 2: sing(V2, "I'm not sure how $ got so o-pen, but once he was, $ played it per-fect-ly.", player, assist); break;
                     }
                     break;
                  case F_alleyoop:
                     if (once()) {
                        sing(V2, "$ placed the ball just right for $.", assist, player);
                     }
                     break;
               }
               break;
            case SHOT_jump :
               switch (flavor) {
                  default: assert(0);
                  case F_none:
                  case F_hook:
                     switch (random_neverrepeat(11)) {
                        default: break;
                        case  0: sing(V2, "A per-fect pass from $ to set $ up.", assist, player); break;
                        case  1: sing(V2, "That was a great as-sist by $.", assist); break;
                        case  2: sing(V2, "$ was wide o-pen.", player); break;
                        case  3: sing(V2, "$ had no-bod-y around him.", player); break;
                        case  4: sing(V2, "Nice assist from $. $ was in the right spot.", assist, player); break;
                        case  5: sing(V2, "Good assist by $. He found $ ready to make the shot.", assist, player); break;
                        case  6: sing(V2, "$ just does-n't seem to care wheth-er there are def-end-ers a-round him or not.", player); break;
                        case  7: sing(V2, "Great pass by $.", assist); break;
                        case  8: sing(V2, "You don't see plays like that ver-y of-ten."); break;
                        case  9:
                        case 10: break; // @TODO
                     }
                     break;
               }
               break;
            case SHOT_3pt  : 
               switch (random_neverrepeat(17)) {
                  default: break;
                  case  0: sing(V2, "When you can make threes like that, your team-mates bet-ter pass it to you."); break;
                  case  1: sing(V2, "Good read by $, find-ing $ read-y to shoot the three.", assist, player); break;
                  case  2: sing(V2, "$ was wide o-pen there. Good play by $ get-ting him the ball.", player, assist); break;
                  case  3: sing(V2, "I'm sur-prised $ tried that three point-er, but he proved me wrong.", player); break;
                  case  4: sing(V2, "Great pass from $ to his o-pen team-mate.", assist); break;
                  case  5: sing(V2, "The & de-fense has got to do bet-ter than that.", !team); break;
                  case  6: sing(V2, "I don't think an-y-body out there thought that was good de-fense."); break;
                  case  7: sing(V2, "A ris-ky shot by $ but he's got the guts to make it work.", player); break;
                  case  8: sing(V2, "Good pass by $.", assist); break;
                  case  9: sing(V2, "Nice pass by $.", assist); break;
                  case 10: sing(V2, "Good pass by $. And a res-pect-a-ble shot by $, of course.", assist, player); break;
                  case 12: sing(V2, "He's got such a pol-ished shot from out there, you want to see $ take as man-y as he can.", player); break;
                  case 13:
                  case 14:
                  case 15:
                  case 16: break; // @TODO
               }
               break;
         }
      } else {
         // unassisted:
         //   layup: 7*none, 3*tip, 2*running,2*putback,1*putback_dunk,1*driving,1*driving_finger_roll
         //   3pt: 3
         //   jump: 13*none, 1*turnaround hook, 1*hook, 1*bank, 3*fadeaway, 2*running_bank
         switch (type) {
            case SHOT_layup:
               switch (flavor) {
                  default: assert(0);
                  case F_driving_finger_roll:
                     break;          // has already commented on this
                  case F_none:
                  case F_running:
                  case F_driving:
                     switch (random_neverrepeat(10)) {
                        default: break;
                        case 0: sing(V2, "The lane was wide o-pen for him there."); break;
                        case 1: sing(V2, "I'm not sure how $ made that shot, but he sure did.", player); break;
                        case 2: sing(V2, "$ just did-n't give the def-end-ers e-nough time to re-act.", player); break;
                        case 3: sing(V2, "You've got to re-spect the way $ makes his own lane.", player); break;
                        case 4: sing(V2, "$ found the lane and the two points were kind of an in-ev-it-a-bil-it-y.", player); break;
                        case 5: sing(V2, "There's real-ly not much you can say about that.", player); break; // what a terrible announcer!
                        case 6:
                        case 7:
                        case 8:
                        case 9: break; // @TODO
                     }
                     break;
                  case F_tip:
                     switch (random_neverrepeat(3)) {
                        default: break;
                        case  0: sing(V2, "I'm not sur-prised to see that play from $.", player); break;
                        case  2: sing(V2, "$ just re-fused to be boxed out.", player); break;
                        case  1: sing(V2, "That is just so-lid bas-ket-ball.");
                                 sing(V1, "The kind you ex-pect from The Bas-ket-ball Bas-ket-ball As-so-ci-a-tion.");
                                 sing(V2, "Ex-act-ly.");
                                 break;
                     }
                     break;
                  case F_putback_dunk:
                     if (once()) {
                        sing(V2, "I don't know where the de-fen-ders were there. $ had to-tal free-dom.", player);
                     }
                     break;
                  case F_putback:
                     switch (random_neverrepeat(2)) {
                        default: break;
                        case  0: sing(V2, "Somehow $ managed to an-ti-ci-pate how the ball would bounce.", player); break;
                        case  1: sing(V2, "That's just a case of $ be-ing in the right place at the right time.", player); break;
                     }
                     break;
               }
               break;
            case SHOT_jump :
               switch (flavor) {
                  default: assert(0);
                  case F_fadeaway:
                     switch (random_neverrepeat(3)) {
                        default: break;
                        case 0: sing(V2, "$ picked the op-tim-al time to de-ploy a fade.", player); break;
                        case 1: sing(V2, "That's one way to sur-prise your def-end-er and make a bas-ket."); break;
                        case 2: sing(V2, "How can you keep your hand in your op-po-nent's face when he's got a shot like that."); break;
                     }
                     break;
                  case F_running_bank:
                     switch (random_neverrepeat(2)) {
                        default: break;
                        case 0: sing(V2, "That was a great play by $, he left his def-end-er scram-bling to try to stop him.", player); break;
                        case 1: sing(V2, "You don't see a lot of bank shots like that, but it's a great tool to have in your ar-sen-al."); break;
                     }
                     break;
                  case F_none:
                  case F_bank:
                  case F_turnaround_hook:
                  case F_hook:
                     switch (random_neverrepeat(16)) {
                        default: break;
                        case  0: sing(V2, "His def-end-er left $ with too much space there.", player); break; 
                        case  1: sing(V2, "That was-n't his best shot, but it went in an-y-way."); break;
                        case  2: sing(V2, "That was a ris-ky shot for $ to take, but it worked out.", player); break;
                        case  3: sing(V2, "I think you have to take that shot if you have the chance."); break;
                        case  4: sing(V2, "I'm sure $ felt pres-sure from the shot clock, but he made it work.", player); break;
                        case  5: sing(V2, "$ sure makes it look eas-y, does-n't he.", player); break;
                        case  6: sing(V2, "Sol-id work from the field by $.", player); break;
                        case  7: sing(V2, "Fun-da-men-tals, you've got to know your fun-da-men-tals, and $ sure does.", player); break;
                        case  8: sing(V2, "You can't leave $ o-pen to take a shot like that.", player); break;
                        case  9: sing(V2, "That's got to be a trick-y shot to make, but $ was on the mon-ey.", player); break;
                        case 10: sing(V2, "$ looks like he's shoot-ing with ex-treme con-fi-dence now.", player); break;
                        case 11: sing(V2, "You can't de-fend like that. You got to put more pres-sure on $.", player); break;
                        case 12: sing(V2, "$ was un-der pres-sure there but he de-liv-ered a fine shot.", player); break;
                        case 13: sing(V2, "The de-fen-der did ev-ery-thing right but $ had the shot dialed in.", player); break;
                        case 14:
                        case 15: break; // @TODO
                     }
                     break;
               }
               break;
            case SHOT_3pt  : 
               switch (random_neverrepeat(3)) {
                  default: break;
                  case  0: sing(V2, "$ does-n't nor-mal-ly make that shot.", player); break;
                  case  1: sing(V2, "That was an im-pres-sive shot by $.", player); break;
                  case  2: sing(V2, "You don't see a three point shot from that far ev-er-y day.", player); break;
               }
               break;
         }
      }

   }


   color_from_queue = 0;

   if (made)
      shot_stats(player, type, made);
}

int suppress_rebound;
int will_foul;

int shot_type, shot_player, shot_flavor;

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

   shot_type = type;
   shot_player = player;
   shot_flavor = flavor;

   wide_open = 0;
   contested = 0;
   drive = 0;
   got_by = 0;
   picked = 0;

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
                  // @TODO: there is one tip shot that's not preceded by a missed shot
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
                  //printf("***       Offensive rebound shots        ***\n"); // trying to detect fast break
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
                     case 2: sing(V1, "con-tes-ted shot_ the lay-up is good."); contested=1; break;
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
                     case 8: sing(V1, "puts the ball up, but mis-ses it."); break;
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
                     case 0: sing(V1, "% pump fakes and slips by his de-fen-der_ ", player); got_by=1; break;
                     case 1: sing(V1, "A nice dou-ble-move from $ to get by $_ ", player, defender(player)); got_by=1; break;
                     case 2: sing(V1, "$ gets a pick from $_ ", player, random_player(tm,player)); break;
                     case 3: sing(V1, "Gets by his def-end-er_ ", player, defender(player)); got_by=1; break;
                     case 4: sing(V1, "% drives to the bas-ket_ ", player); drive=1; break;
                     case 5: sing(V1, "Drives to the bas-ket_ ", player); drive=1; break;
                     case 6: sing(V1, "% gets to the bas-ket_ ", player); drive=1; break;
                     case 7: sing(V1, "% works his way to the bas-ket_ ", player); drive=1; break;
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
                           case 2: sing(V1, "con-tes-ted shot_ the lay-up is good."); contested=1; break;
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
                           case 0: sing(V1, "!is wide open and he slams it in for two."); wide_open=1; break;
                           case 1: sing(V1, "!and slams it in to the bas-ket."); break;
                           case 2: sing(V1, "!and dunks it for an easy two."); break;
                        }
                        break;
                     case F_driving:
                        switch (random_nonrepeat(2)) {
                           case 0: sing(V1, "drives in to the basket, lays it up, it's good."); drive=1; break;
                           case 1: sing(V1, "drives to the basket and lays it in."); drive=1; break;
                           default: assert(0);
                        }
                        break;
                     case F_driving_finger_roll:
                        sing(V1, "drives to the bas-ket and lays it up and in."); drive=1;
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
                        if (once()) sing(V2, "That's got-ta be em-bar-rass-ing");
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
            case 2: if (assist)
                       { sing(V1, "He's wide o-pen, goes for three_ "); wide_open=1; }
                    else
                       { sing(V1, "!Fakes his de-fen-der and goes for three_ "); wide_open=1; }
                    break;
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
                     case 3: if (once()) {
                                sing(V1, "Off a pick by $, ", random_player(tm,player));
                                picked=1;
                             }
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
                        case 0: if (assist)
                                   sing(V1, "$ shoots_ ", player);
                                else
                                   { sing(V1, "$ with the pick, $ shoots_ ", random_player(tm,player), player); picked=1; }
                                break;
                        case 1: if (assist)
                                   sing(V1, "$ shoots_ ", player);
                                else
                                   { sing(V1, "Shoots off a screen from $_ ", random_player(tm,player)); picked=1; }
                                break;
                        case 2: if (assist)
                                   sing(V1, "$ shoots_ ", player);
                                else
                                   { sing(V1, "Pump fakes and takes a shot_ "); said_shot = 1; picked=1; }
                                break;
                        case 3: sing(V1, "$ with a wide open look_ ", player); wide_open=1; break;
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
      
         if (assist) {
            switch (random_nonrepeat(6)) {
               case 0: sing(V1, "$ with the assist.", assist); break;
               case 1: sing(V1, "$ with the assist.", assist); break;
               case 2: sing(V1, "Assisted by $.", assist); break;
               case 3: sing(V1, "$ credited with an assist.", assist); break;
               case 4: sing(V1, "With an assist by $.", assist); break;
               case 5: sing(V1, "$ with the assist on the play.", assist); break;
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

   did_announce_score = 0;
   if (made)
      add_points(tm, (type == SHOT_3pt ? 3 : 2), 1);


   //  how the player's doing at making a given shot type
   //  

   color_shot_commentary(type, player, assist, made, blocker, flavor, will_foul != 0);

   if (pending_announce) {
      points_announce(pending_announce-1, 0);
      pending_announce = 0;
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

void puts_it_up(void)
{
   switch (random_nonrepeat(5)) {
      case 0: sing(V1, "puts it up_"); break;
      case 1: sing(V1, "takes the shot_"); break;
      case 2: sing(V1, "shot is up_"); break;
      case 3: sing(V1, "puts up a shot_"); break;
      case 4: sing(V1, "puts the shot up_"); break;
      default: assert(0);
   }
}

void free_shot(int player, int shotnum, int total_shots, int made)
{
   int n;
   int team = player < 0 ? TEAM_1 : TEAM_0;
   int announce=0;

   who_was_fouled = player;
   process_foul(shotnum == 1 ? total_shots : 0);
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

      {
         if (SSTAT(player, SS_ft)->attempts == 0) {
            if (this_event_time > 2*60) {
               switch (random_nonrepeat(6)) {
                  case 0: if (total_shots == 2)
                              sing(V3, "These are his first free throws of the night.");
                           else
                              sing(V3, "This is his first free throw of the night.");
                           break;
                  case 1: sing(V3, "His first attempt to-night."); break;
                  case 2: sing(V3, "This is his first time at the line."); break;
                  case 3: if (total_shots == 2)
                              sing(V3, "These are his first tries to-night.");
                           else
                              sing(V3, "This is his first try to-night.");
                           break;
                  case 4: sing(V3, "His first time free throw this game."); break;
                  case 5: if (total_shots == 2)
                             sing(V3, "These are his first free throws this game.");
                           else
                             sing(V3, "This is his first free throw this game.");
                           break;
                  default: assert(0);
               }
            }
         } else if (SSTAT(player, SS_ft)->attempts == 2 && SSTAT(player, SS_ft)->made == 0) {
            // trying for first points
            switch (random_nonrepeat(2)) {
               case 0: sing(V3, "Look-ing for his first points from the line. He missed his first two."); break;
               case 1: sing(V3, "He's missed his pre-vi-ous two attempts. He's look-ing to turn that a-round."); break;
               default: assert(0);
            }
         } else if (SSTAT(player, SS_ft)->attempts == SSTAT(player, SS_ft)->made && SSTAT(player, SS_ft)->attempts >= 4) {
            if (SSTAT(player, SS_ft)->attempts > 4) {
               sing(V3, "This is his third vis-it to the free throw line, and he made all # of those points.", SSTAT(player, SS_ft)->made);
            } else {
               switch (random_nonrepeat(2)) {
                  case 0: sing(V3, "He's made ev-ery one of his # attempts so far.", SSTAT(player, SS_ft)->made); break;
                  case 1: sing(V3, "He's hit all # of his free throws tonight.", SSTAT(player, SS_ft)->made); break;
                  default: assert(0);
               }
            }
         } else {
            if (random(100) < 70) {
               switch (random_nonrepeat(5)) {
                  case 0: sing(V3, "He's # for # from the line tonight.", SSTAT(player,SS_ft)->made, SSTAT(player,SS_ft)->attempts); break;
                  case 1: sing(V3, "$ is # for # from the free throw line tonight.", player, SSTAT(player,SS_ft)->made, SSTAT(player,SS_ft)->attempts); break;
                  case 2: sing(V3, "He's # for # from the line so far this game.", SSTAT(player,SS_ft)->made, SSTAT(player,SS_ft)->attempts); break;
                  case 3: sing(V3, "He's made # of # free throws tonight.", SSTAT(player,SS_ft)->made, SSTAT(player,SS_ft)->attempts); break;
                  case 4: sing(V3, "He's shot # for # from the line here tonight.", SSTAT(player,SS_ft)->made, SSTAT(player,SS_ft)->attempts); break;
                  default: assert(0);
               }
            }
         }
      }

      if (total_shots == 1) {
         if (last_shot_points == 3)
            sing(V1, "%'s look-ing to add a fourth point_ ", player);
         else {
            assert(last_shot_points == 2);
            sing(V1, "%'s look-ing to turn it into three_ ", player);
         }
         puts_it_up();
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
               case 0: sing(V1, "mis-ses it."); break;
               case 1: sing(V1, "it doesn't drop."); break;
               case 2: sing(V1, "but it's no good."); break;
            }
      } else {
         puts_it_up();
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
               case 0: sing(V1, "mis-ses the first one."); break;
               case 1: sing(V1, "the first one doesn't drop."); break;
               case 2: sing(V1, "but the first one is no good."); break;
            }
      }
   } else {
      enable_color();
      puts_it_up();
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

   // second quarter and game is close
   if (this_event_time > 3*12*60 && abs(score[0] - score[1]) < 8) {
      if (abs(score[0] - score[1]) < 3) {
         if (made) {
            if (score[team] > score[!team]) {
               if (score[team] >= score[!team]+2) {
                  switch (random_neverrepeat(2)) {
                     case -1: break;
                     case 0: sing(V2, "That's an im-por-tant shot to keep the lead."); break;
                     case 1: sing(V2, "That was a cru-cial shot for them."); break;
                     default: assert(0);
                  }
               } else {
                  // unreached
                  assert(0);
               }
            } else {
               if (score[team]+2 == score[!team])
                  sing(V2, "He had to make that shot, keep-ing them with-in one.");
               else
                  sing(V2, "That was a cru-cial shot for him to make."); // unused
            }
         } else {
            if (score[team]+1 == score[!team])
               sing(V2, "He could have tied it up with that one.");
            else
               sing(V2, "He real-ly want-ed to make that one."); // unused
         }
      } else {
         // score delta less than 8, more than 2
         if (made) {
            if (shotnum == total_shots && total_shots == 2) {
               if (last_free_made) {
                  // made this one, made the previous one
                  if (score[team] > score[!team]) {
                     switch (random_neverrepeat(3)) { // 3
                        default: assert(0);
                        case -1: break;
                        case  0: sing(V2, "Ma-king both of those keeps them well out in front."); break;
                        case  1: sing(V2, "Those were pret-ty im-por-tant shots for him to make."); break;
                        case  2: break;
                     }
                  } else {
                     switch (random_neverrepeat(2)) { // 2
                        default: assert(0);
                        case -1: break;
                        case  0: sing(V2, "Ma-king both of those was crit-i-cal."); break;
                        case  1: sing(V2, "If they want to catch up, he's got to make those shots. And he did."); break;
                     }
                  }
               } else {
                  // made this one, missed the previous one
                  if (score[team] > score[!team]) {
                     sing(V2, "At least he got the sec-ond one in, but you can't af-ford to miss a free throw when your lead is so small.");
                  } else {
                     switch (random_neverrepeat(3)) { // 3
                        default: assert(0);
                        case -1: break;
                        case  0: sing(V2, "That first miss is go-ing to weigh heav-il-y on his mind I'm sure."); break;
                        case  1: sing(V2, "At least he got that one, but he's got to be mad at him-self for mis-sing the first."); break;
                        case  2: break;
                     }
                  }
               }
            } else {
               // first shot (of 1 or 2)
               switch (random_neverrepeat(5)) { // 5
                  default: assert(0);
                  case -1: break;
                  case  0: sing(V2, "He's un-stop-pa-ble from the free throw line."); break;
                  case  1: sing(V2, "He's had trou-ble at the free throw line be-fore, but that was so-lid"); break;
                  case  2: break;
                  case  3: break;
                  case  4: break;
               }
            }
         } else {
            // missed, score delta less than 8, more than 2
            if (shotnum == total_shots && total_shots == 2) {
               if (last_free_made) {
                  if (score[team] > score[!team])
                     sing(V2, "He would have liked to have made that one too and push the lead a little farther.");
                  else
                     assert(0);
               } else {
                  assert(0);
               }
            } else {
               // first shot (of 1 or 2)
               if (score[team] > score[!team]) {
                  sing(V2, "If he'd made that he could have pushed the lead that much farther.");
               } else {
                  switch (random_neverrepeat(3)) { // 3
                     default: assert(0);
                     case -1: break;
                     case  0: sing(V2, "That hurts."); break;
                     case  1: sing(V2, "They need to sink these free chan-ces if they want to catch up."); break;
                     case  2: sing(V2, "They real-ly could have used that."); break;
                        break;
                  }
               }
            }
         }
      }
   } else {
       // that should help his stats
      if (made) {
         if (shotnum == total_shots && total_shots == 2) {
            if (last_free_made) {
               switch (random_neverrepeat(6)) { // 6
                  default: assert(0);
                  case -1: break;
                  case  0: sing(V2, "That should help his stats."); break;
                  case  1: sing(V2, "He needs to do that more of-ten."); break;
                  case  2: sing(V2, "You can't com-plain about that."); break;
                  case  3: break;
                  case  4: break;
                  case  5: break;
               }
            } else {
               switch (random_neverrepeat(3)) { // 3
                  default: assert(0);
                  case -1: break;
                  case  0: sing(V2, "That's bet-ter than none, but he can't be hap-py with him-self."); break;
                  case  1: sing(V2, "At least he made the second one."); break;
                  case  2: break;
               }
            }
         } else {
            switch (random_neverrepeat(11)) { // 11
               default: assert(0);
               case -1: break;
               case  0: sing(V2, "Good form from the line."); break;
               case  1: sing(V2, "It's al-ways a plea-sure to watch him shoot free throws."); break;
               case  2: break;
               case  3: break;
               case  4: break;
               case  5: break;
               case  6: break;
               case  7: break;
               case  8: break;
               case  9: break;
               case 10: break;  // @MORECOLOR
            }
         }
      } else {
         // missed
         if (shotnum == total_shots && total_shots == 2) {
            if (last_free_made) {
               switch (random_neverrepeat(2)) { // 2
                  default: assert(0);
                  case -1: break;
                  case  0: sing(V2, "You'd like to con-vert two every trip, but some-times it just does-n't work out."); break;
                  case  1: break;  // @MORECOLOR
               }
            } else {
               switch (random_neverrepeat(3)) { // 3
                  default: assert(0);
                  case -1: break;
                  case  0: sing(V2, "You know your team is-n't gon-na be hap-py with you miss-ing both free throws."); break;
                  case  1: break;
                  case  2: break;  // @MORECOLOR
               }
            }
         } else {
            switch (random_neverrepeat(7)) { // 7
               default: assert(0);
               case -1: break;
               case  0: sing(V2, "I'm sur-prised to see him miss that.");
               case  1: break;
               case  2: break;
               case  3: break;
               case  4: break;
               case  5: break;
               case  6: break;  // @MORECOLOR
            }
         }
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
      add_points(team, 1, 0);
   else if (force_announce_points)
      points_announce(V3, 0);

   force_announce_points = 0;
   did_announce_last_free = 0;
   if (made && has_announced_score())
      did_announce_last_free = 1;

   game_state.position = (player < 0 ? -1.0f : 1.0f);
   game_state.current_time = this_event_time;

   add_streakable(SS_ft, player, made, 1);
   add_streakable(SS_total, player, made, 1);

   if (shotnum == total_shots) {
      add_plain(SP_ft_visits, player, 1);
      if (made) {
         set_tpossess(player < 0 ? TEAM_0 : TEAM_1, 0);

         // we don't do stats here because we did stats BEFORE he shot
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
   no_process_foul();
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

#if 0
   // only three total calls to this function, so let's not waste time on it since time is precious at this point
   switch (random_nonrepeat(20)) {
      default: break;
      case 0: sing(V2, "");
   if (random(100) < 50)
      sing(V2, "I have an op-in-ion on of-fen-sive fouls. I have a lot of o-pin-ions. I am glad to share them with you.");
   else
      sing(V3, "I have some stats. This is com-pli-ca-ted. In-deed.");
#endif
   
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
   no_process_foul();
   process_substitution();

   assert(game_state.ball_alive);
   if (player) {
      enable_color();
      if (offensive) {
         if (player == game_state.possess_player && random(100) < 50) {
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "Gets the ball back."); break;
               case 1: sing(V1, "Gets his own rebound."); break;
               case 2: sing(V1, "$ gets the ball again.", player); break;
               default: assert(0);
            }
         } else {
            switch (random_nonrepeat(4)) {
               case 0: sing(V1, "$ with the of-fen-sive re-bound.", player); break;
               case 1: sing(V1, "$ gets the of-fen-sive re-bound.", player); break;
               case 2: sing(V1, "Of-fen-sive re-bound by $.", player); break;
               case 3: sing(V1, "Of-fen-sive re-bound from $.", player); break;
               default :assert(0);
            }
         }
      } else {
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
   } else {
      if (offensive) {
         enable_color();
         if (random(100) < 60) {
            sing(V1, "& get[s|] the ball back.", team);
         } else {
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "The ball goes out of bounds. Last touched by $, so & get it back.", random_player(!team,0), team);
                       game_state.position = 0.75;
                       game_state.ball_alive = 0;
                       break;
               case 1: sing(V1, "Two play-ers from & come down with the ball.", team); break;
               case 2: sing(V1, "It's still &'s ball.", team); break;
               default: assert(0);
            }
         }
      } else {
         enable_color();
         sing(V1, "& [takes|take] possession.", team);
      }
   }
   game_state.position = (game_state.possess_team == TEAM_0 ? 1.0f : -1.0f);
   if (offensive)
      add_plain(SP_off_rebounds, player, 1);
   else
      add_plain(SP_def_rebounds, player, 1);
   add_plain(SP_rebounds, player, 1);

   // color commentary and stats here

   shot_stats(shot_player, shot_type, 0);

   if (offensive) {
      #if 0
      if (random(100) < 50)
         sing(V2, "There's an in-ter-est-ing fact a-bout of-fen-sive re-bounds. I have a lot of o-pin-ions. I am glad to share them with you."); // @MORECOLOR
      #endif
   } else {
      // we know nothing about the play except that it was a miss (given the rebound)
      // so we probably want to know if it was a layup, jump shot, 3-pointer, and
      // comment on that here

      // def rebound on:
      //    layup: tip, dunk, putback, 5*none
      //    3pt: 11
      //    jump: 31*none, 3*turnaround, 2*hook, 2*bank
      switch (shot_type) {
         case SHOT_layup:
            switch (shot_flavor) {
               case F_tip: sing(V2, "$ had an eas-y op-por-tun-i-ty there.", shot_player); break;
               case F_dunk: break; // this has already been commented on
               case F_putback: sing(V2, "$ thought he had that.", shot_player); break;
               case F_none:
                  switch (random_neverrepeat(5)) {
                     default: break;
                     case 0:  sing(V2, "That's not the kind of shot you ex-pect $ to miss.", shot_player); break;
                     case 1:  sing(V2, "Sol-id de-fense on that play."); break;
                     case 2:  sing(V2, "You know, you're not go-ing to win games if you can't lay that in."); break;
                     case 3:  sing(V2, "$ drove strong but some-times that is-n't e-nough.", shot_player); break;
                     case 4:  sing(V2, "You've got-ta give cred-it to the de-fen-ders there."); break;
                  }
                  break;
            }
            break;
         case SHOT_jump :
            switch (random_neverrepeat(31)) {
               case 0:  sing(V2, "That's not a shot you'd think $ would miss.", shot_player); break;
               case 1:  sing(V2, "% was wide o-pen there; you could-n't ask for a bet-ter look.", shot_player); break;
               case 2:  sing(V2, "That would have been a tough shot to make."); break;
               case 3:  sing(V2, "The de-fen-der was in his face so it's no sur-prise he could-n't land that."); break;
               case 4:  sing(V2, "You real-ly should-n't miss a wide o-pen look like that."); break;
               case 5:  sing(V2, "I think he rushed that shot."); break;
               case 6:  sing(V2, "Good job by the de-fen-der get-ting a hand in his face."); break;
               case 7:  sing(V2, "With the hand in his face % did-n't have much chance there.", shot_player); break;
               // @TODO more
               default: break;
            }
            break;
         case SHOT_3pt  : {
            switch (random_neverrepeat(11)) {
               default: break;
               case 0:  sing(V2, "That was a tough shot."); break;
               case 1:  sing(V2, "The de-fense was no-where near %, but I guess we know why.", shot_player); break;
               // @TODO more
            }
            break;
         }
      }
   }

   tpossess(team, announce);

   if (player) {
      // don't do game stats until later
      if (this_event_time > 12 * 60 && random(100) < 5) {
         int r1,r2;
         char *s = NULL;
         if (random(100) < 80) {
            if (offensive) {
               r1 = PSTAT(player_for_team[ team], SP_off_rebounds)->game;
               r2 = PSTAT(player_for_team[!team], SP_off_rebounds)->game;
               switch (random_nonrepeat(3)) {  // 2..3
                  case 0: s = "& [has|have] picked up # of-fen-sive re-bounds this game, where-as & [has|have] #."; break;
                  case 1: s = "In terms of of-fen-sive re-bounds, & ha[s|ve] # and & ha[s|ve] #."; break;
                  case 2: s = "& [has|have] gotten # of-fen-sive re-bounds to & with #."; break;
               }
            } else {
               r1 = PSTAT(player_for_team[ team], SP_def_rebounds)->game;
               r2 = PSTAT(player_for_team[!team], SP_def_rebounds)->game;
               switch (random_nonrepeat(4)) {  // 2..6
                  case 0: s = "& [has|have] # de-fen-sive re-bounds this game, where-as & [has|have] #."; break;
                  case 1: sing(V3, "& [has|have] pulled down # de-fen-sive re-bounds, ver-sus # for &.", team, r1, r2, !team); break;
                  case 2: sing(V3, "That's # de-fen-sive boards for & to # for &.", r1, team, r2, !team); break;
                  case 3: if (abs(r1-r2) >= 5)
                             if (r1 < r2)
                                sing(V3, "& [has|have] only got-ten # de-fen-sive re-bounds com-pared to # for &.", team, r1, r2, !team);
                             else
                                sing(V3, "& [has|have] got-ten # de-fen-sive re-bounds, com-pared to only # for &.", team, r1, r2, !team);
                          break;
               }
            }
         } else {
            r1 = PSTAT(player_for_team[ team], SP_rebounds)->game;
            r2 = PSTAT(player_for_team[!team], SP_rebounds)->game;
            if (once()) {
               s = "& ha[s|ve] # re-bounds this game, where-as & ha[s|ve] #.";
            }
         }
         if (s)
            sing(V3, s, team, r1, !team, r2);
      } else if (this_event_time > 4 * 60 && random(100) < (offensive ? 25 : 25)) { // don't do player stats in first few minutes
         if (random(100) < 60) {
            if (offensive) {
               // 1..4
               switch (random_nonrepeat(4)) {
                  case 0: sing(V3, "That's his #th of-fen-sive re-bound.", PSTAT(player, SP_off_rebounds)->game); break;
                  case 1: sing(V3, "His #th of-fen-sive re-bound.", PSTAT(player, SP_off_rebounds)->game); break;
                  case 2: sing(V3, "That's his #th of-fen-sive re-bound this game.", PSTAT(player, SP_off_rebounds)->game); break;
                  case 3: sing(V3, "His #th of-fen-sive re-bound of the game.", PSTAT(player, SP_off_rebounds)->game); break;
               }
            } else {
               // 4..6
               switch (random_nonrepeat(6)) {
                  case 0: sing(V3, "That's his #th de-fen-sive re-bound.", PSTAT(player, SP_def_rebounds)->game); break;
                  case 1: sing(V3, "His #th de-fen-sive re-bound.", PSTAT(player, SP_def_rebounds)->game); break;
                  case 2: sing(V3, "That'll be his #th de-fen-sive re-bound.", PSTAT(player, SP_def_rebounds)->game); break;
                  case 3: sing(V3, "That's his #th de-fen-sive re-bound tonight.", PSTAT(player, SP_def_rebounds)->game); break;
                  case 4: sing(V3, "His #th de-fen-sive re-bound this game.", PSTAT(player, SP_def_rebounds)->game); break;
                  case 5: sing(V3, "That'll be his #th de-fen-sive re-bound tonight.", PSTAT(player, SP_def_rebounds)->game); break;
               }
            }
         } else {
            switch (random_nonrepeat(8)) { // 5..8
               case 0: sing(V3, "His #th re-bound to-night.", PSTAT(player, SP_rebounds)->game); break;
               case 1: sing(V3, "That's his #th re-bound to-night.", PSTAT(player, SP_rebounds)->game); break;
               case 2: sing(V3, "His #th re-bound for the night.", PSTAT(player, SP_rebounds)->game); break;
               case 3: sing(V3, "That's his #th re-bound for the night.", PSTAT(player, SP_rebounds)->game); break;
               case 4: sing(V3, "His #th re-bound this game.", PSTAT(player, SP_rebounds)->game); break;
               case 5: sing(V3, "That's his #th re-bound this game.", PSTAT(player, SP_rebounds)->game); break;
               case 6: sing(V3, "His #th re-bound.", PSTAT(player, SP_rebounds)->game); break;
               case 7: sing(V3, "That's his #th re-bound.", PSTAT(player, SP_rebounds)->game); break;
               default: assert(0);
            }
         }
      }
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
   if (stealer) {
      int multi;
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

      if (PSTAT(stealer, SP_steals)->game == 1)
         multi = random_neverrepeat(5);
      else
         multi = -1;

      if (multi >= 0 && multi <= 1)
         switch (multi) {
            case 0: sing(V2, "A-noth-er steal by $, he's own-ing the court to-day.", stealer); break;
            case 1: sing(V2, "% [is|are] go-ing to have to do some-thing a-bout his steals.", team); break;
         }
      else
         switch (random_neverrepeat(10)) {
            default: break;
            case 0: sing(V2, "Great move by $ there.", stealer); break;
            case 1: sing(V2, "$ left him-self o-pen to that.", player); break;
            case 2: sing(V2, "$ just got right on that ball.", stealer); break;
            case 3: sing(V2, "$ makes it look so eas-y.", stealer); break;
         }
   } else {
      ++hack_count2[type];
      switch (type) {
         case TURNOVER_traveling:      // 5
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "Tra-vel-ing is called on $.", player); break;
               case 1: sing(V1, "$ is called for tra-vel-ing.", player); break;
               case 2: sing(V1, "And there's the call. Tra-vel-ing.", player); break;
               default: assert(0);
            }
            switch (random_neverrepeat(4)) {
               default: break;
               case 0: sing(V2, "I'm not sure a-bout that call."); break;
               case 1: sing(V2, "Not a good move on his part."); break;
               case 2: break;
               case 3: break;
            }
            break;
         case TURNOVER_badpass:        // 3
            switch (random_nonrepeat(3)) {
               case 0: sing(V1, "$ makes a bad pass, out of bounds.", player); break;
               case 1: sing(V1, "$ throws to $ but he's off the mark and it goes out of bounds.", player, random_player(team,player)); break;
               case 2: sing(V1, "$ look-ing for $ but throws it out of bounds.", player, random_player(team,player)); break;
               default: assert(0);
            }
            switch (random_neverrepeat(3)) {
               default: break;
               case 0: sing(V2, "I'm not sure what $ was think-ing there.", player); break;
               case 1: sing(V2, "That was not a good choice on his part."); break;
               case 2: sing(V2, "Good job by the de-fen-der there forc-ing that turn-o-ver."); break;
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

   if (stealer) {
      if (PSTAT(stealer, SP_steals)->game > 1)
         switch (random_nonrepeat(4)) {
            case 0: sing(V3, "That's his #th steal of the game.", PSTAT(stealer, SP_steals)->game); break;
            case 1: sing(V3, "His #th steal tonight.", PSTAT(stealer, SP_steals)->game); break;
            case 2: sing(V3, "That'll be his #th steal here tonight.", PSTAT(stealer, SP_steals)->game); break;
            case 3: sing(V3, "His #th steal of the game.", PSTAT(stealer, SP_steals)->game); break;
         }
      else if (PSTAT(player_for_team[0], SP_steals)->game && PSTAT(player_for_team[1], SP_steals)->game && random(100) < 20) {
         int r1 = PSTAT(player_for_team[!team], SP_steals)->game;
         int r2 = PSTAT(player_for_team[ team], SP_steals)->game;
         switch (random_nonrepeat(3)) {
            case 0: sing(V3, "The #th steal by %.", r1, !team); break;
            case 1: sing(V3, "% [has|have] stolen #, while % [has|have] # steals.", !team, r1, team, r2); break;
            case 2: sing(V3, "That's the #th steal for %, ver-sus # for %.", r1, !team, r2, team); break;
         }
      }
   }

   tpossess(!team, 1);
   game_state.current_time = this_event_time;
   game_state.possess_player = stealer;
   game_state.after_rebound = 0;
}

void do_timeout(int team)
{
   process_foul(0);
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
               case 0: sprintf(buffer, "$$ at %s. ", positions[k]); break;
               case 1: sprintf(buffer, "at %s, $$. ", positions[k]); break;
               case 2: sprintf(buffer, "%s, $$.", positions[k]); break;
               default: assert(0);
            }
         } else {
            switch (style2) {
               case 0: sprintf(buffer, "$ at %s. ", positions[k]); break;
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
         took_shot(SHOT_jump, v[0], assist, 1, blocker, F_running_bank); // is this really a jump shot, or is it a layup ?!?
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

         if (tim == 5*60+25) {
            start_song();
         }
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
         start_song();
         sing(V1, "Wel-come to to-night's Bas-ket-ball Bas-ket-ball As-so-ci-a-tion game.");
         sing(V1, "Fea-tur-ing && ver-sus &&.", 0, 1);
         set_lineup(TEAM_0,  1,  2,  3,  4,  5);
         set_lineup(TEAM_1, -1, -2, -3, -4, -5);
         team_fouls[0] = team_fouls[1] = 0;
         announce_lineup(1);
      } else if (0 == strcmp(text, "End of 1st Quarter")) {
         sing(V1, "That's the end of the first quar-ter.");
         quarter = 1;
         start_song();
         sing(V1, "Now the start of the sec-ond quar-ter.");
         set_lineup(TEAM_0,  1,  7,  9,  4,  8); // 7 unknown
         set_lineup(TEAM_1, -9, -2, -6, -4, -7);
         announce_lineup(0);
         announce_score(V1);
         team_fouls[0] = team_fouls[1] = 0;
         set_tpossess(TEAM_1, -1);
         game_state.position = 1;
      } else if (0 == strcmp(text, " Timeout: Official")) {
         static int which;
         do_timeout(-1);
         if (which == 0) {
            start_song();
            sing(V1, "Now con-tin-u-ing the sec-ond quar-ter.");
            set_lineup(TEAM_0, 1, 2, 3, 4, 5);
            set_lineup(TEAM_1, -6, -2, -7, -4, -5);
            // -6 -2 -5 -7 -4
            announce_lineup(0);
         } else {
            sing(V1, "Now con-tin-u-ing the fourth quar-ter.");
            set_lineup(TEAM_0, 7, 2, 9, 6, 8);
            set_lineup(TEAM_1, -6, -2, -7, -4, -9);
            announce_lineup(0);
         }
         announce_score(V1);
         ++which;
      } else if (0 == strcmp(text, "End of 2nd Quarter")) {
         sing(V1, "That's the end of the first half.");
         quarter = 2;
         start_song();
         sing(V1, "Here's the start of the third quar-ter.");
         half = 1;
         set_lineup(TEAM_0, 1, 2, 3, 4, 5);
         set_lineup(TEAM_1, -1, -2, -3, -4, -5); 
         announce_lineup(0);
         announce_score(V1);
         team_fouls[0] = team_fouls[1] = 0;
         set_tpossess(TEAM_1, -1);
         game_state.position = 1;
      } else if (0 == strcmp(text, "End of 3rd Quarter")) {
         sing(V1, "That's the end of the third quar-ter.");
         quarter = 3;
         start_song();
         sing(V1, "We're a-bout to start the fi-nal quar-ter.");
         announce_score(V1);
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

      
   stb_shuffle(tempos, sizeof(tempos)/sizeof(tempos[0]), sizeof(tempos[0]), stb_rand());

   remove_player_names(pbp, n);

   stb_srand(time(NULL));             
   if (argc > 1)
      stb_srand(atoi(argv[1]));
   create_teams();

   generate_structure(3);

   set_tpossess(-1, 0);
   game_state.current_time = 0;
   for (i=0; i < n; ++i) {
      cur_line = i+1;
      will_foul = 0;
      if (i+1 < n)
         if (strstr(pbp[i+1], "Foul:Shooting") != NULL)
            if (strncmp(pbp[i], pbp[i+1], 8)==0)
               will_foul = 1;
      process_event(pbp[i]);
   }
   if (pending.voice >= 0)
      close_vocal_line();

   assert(num_songs <= MAX_SONGS);
   for (i=0; i < num_songs; ++i) {
      vocals = all_vocals[i];
      write_music(i);
   }
   return 0;
}
