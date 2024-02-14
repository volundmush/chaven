
#ifndef _MAGIC_H_
#define _MAGIC_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_SONGS	26
#define MAX_LINES	100 /* this boils down to about 1k per song */
#define MAX_GLOBAL	10  /* max songs the global jukebox can hold */

struct song_data
{
    char *group;
    char *name;
    char *lyrics[MAX_LINES];
    int lines;
};

extern struct song_data song_table[MAX_SONGS];

void song_update args( (void) );
void load_songs	args( (void) );

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif
