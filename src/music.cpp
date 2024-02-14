#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif
#ifndef WIN32
#include <sys/types.h>
#endif
#include "merc.h"
#include "music.h"
#include "recycle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__cplusplus)
extern "C" {
#endif

  int channel_songs[MAX_GLOBAL + 1];
  struct song_data song_table[MAX_SONGS];
  char *social_radio[15];
  char *snews_radio[15];
  char *news_radio[15];

  bool can_hear_music(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *target_room) {
    int i;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *temp_room;
    if (in_room == target_room)
    return TRUE;
    for (i = 0; i < 10; i++) {
      if ((pexit = in_room->exit[i]) != NULL && (pexit->wall < WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (temp_room = pexit->u1.to_room) != NULL) {
        if (temp_room == target_room)
        return TRUE;
      }
    }
    return FALSE;
  }

  void song_update(void) {
    CHAR_DATA *victim;
    CHAR_DATA *to;
    ROOM_INDEX_DATA *room;
    char buf[MAX_STRING_LENGTH];
    char *line;
    int i;

    /* do the global song, if any */
    if (channel_songs[1] >= MAX_SONGS)
    channel_songs[1] = -1;

    if (channel_songs[1] > -1) {
      if (channel_songs[0] >= MAX_LINES || channel_songs[0] >= song_table[channel_songs[1]].lines) {
        channel_songs[0] = -1;

        /* advance songs */
        for (i = 1; i < MAX_GLOBAL; i++)
        channel_songs[i] = channel_songs[i + 1];
        channel_songs[MAX_GLOBAL] = -1;
      }
      else {
        if (channel_songs[0] < 0) {
          sprintf(buf, "Music: %s, %s", song_table[channel_songs[1]].group, song_table[channel_songs[1]].name);
          channel_songs[0] = 0;
        }
        else {
          sprintf(buf, "Music: '%s'", song_table[channel_songs[1]].lyrics[channel_songs[0]]);
          channel_songs[0]++;
        }

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          victim = CH(*it);

          if ((*it)->connected == CON_PLAYING && !IS_FLAG(victim->comm, COMM_QUIET))
          act_new("$t", (*it)->character, buf, NULL, TO_CHAR, POS_SLEEPING);
        }
      }
    }

    for (ObjList::iterator it = object_list.begin(); it != object_list.end();
    ++it) {
      OBJ_DATA *obj = *it;

      if (obj->item_type != ITEM_JUKEBOX || obj->value[1] < 0)
      continue;

      if (obj->value[1] >= MAX_SONGS) {
        obj->value[1] = -1;
        continue;
      }

      /* find which room to play in */

      if ((room = obj->in_room) == NULL) {
        if (obj->carried_by == NULL)
        continue;
        else if ((room = obj->carried_by->in_room) == NULL)
        continue;
      }

      if (obj->value[0] == -10) {
        if (obj->value[1] > 14 || obj->value[1] < 0)
        obj->value[1] = 0;

        if (number_percent() % 2 == 0 && (news_radio[obj->value[1]] != NULL && news_radio[obj->value[1]][0] != '\0')) {
          sprintf(buf, "In News: %s", news_radio[obj->value[1]]);
          for (DescList::iterator it = descriptor_list.begin();
          it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;

            if (d->character != NULL && d->connected == CON_PLAYING) {
              to = d->character;
              if (IS_NPC(to))
              continue;
              if (to->in_room == NULL)
              continue;

              if (can_hear_music(to->in_room, room)) {
                send_to_char(buf, to);
              }
            }
          }
        }
        if (number_percent() % 2 == 0 && (snews_radio[obj->value[1]] != NULL && snews_radio[obj->value[1]][0] != '\0')) {
          sprintf(buf, "In Current Affairs: %s", snews_radio[obj->value[1]]);
          for (DescList::iterator it = descriptor_list.begin();
          it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;

            if (d->character != NULL && d->connected == CON_PLAYING) {
              to = d->character;
              if (IS_NPC(to))
              continue;
              if (to->in_room == NULL)
              continue;

              if (can_hear_music(to->in_room, room)) {
                send_to_char(buf, to);
              }
            }
          }
        }
        if (number_percent() % 2 == 0 && (social_radio[obj->value[1]] != NULL && social_radio[obj->value[1]][0] != '\0')) {
          sprintf(buf, "In Gossip: %s", social_radio[obj->value[1]]);
          for (DescList::iterator it = descriptor_list.begin();
          it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;

            if (d->character != NULL && d->connected == CON_PLAYING) {
              to = d->character;
              if (IS_NPC(to))
              continue;
              if (to->in_room == NULL)
              continue;

              if (can_hear_music(to->in_room, room)) {
                send_to_char(buf, to);
              }
            }
          }
        }
        obj->value[1]++;
        continue;
      }

      if (obj->value[0] < 0) {
        sprintf(buf, "%s starts playing %s, %s.\n\r", obj->short_descr, song_table[obj->value[1]].group, song_table[obj->value[1]].name);

        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;

          if (d->character != NULL && d->connected == CON_PLAYING) {
            to = d->character;
            if (IS_NPC(to))
            continue;
            if (to->in_room == NULL)
            continue;

            if (can_hear_music(to->in_room, room)) {
              send_to_char(buf, to);
            }
          }
        }
        obj->value[0] = 0;
        continue;
      }
      else {
        if (obj->value[0] >= MAX_LINES || obj->value[0] >= song_table[obj->value[1]].lines) {

          obj->value[0] = -1;

          /* scroll songs forward */
          obj->value[1] = obj->value[2];
          obj->value[2] = obj->value[3];
          obj->value[3] = obj->value[4];
          obj->value[4] = -1;
          continue;
        }

        line = song_table[obj->value[1]].lyrics[obj->value[0]];
        obj->value[0]++;
      }

      sprintf(buf, "%s plays: '%s'", obj->short_descr, line);
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to))
          continue;
          if (to->in_room == NULL)
          continue;

          if (can_hear_music(to->in_room, room)) {
            send_to_char(buf, to);
          }
        }
      }
    }
  }

  void load_songs(void) {
    FILE *fp;
    int count = 0, lines, i;
    char letter;

    /* reset global */
    for (i = 0; i <= MAX_GLOBAL; i++)
    channel_songs[i] = -1;

    if ((fp = fopen(MUSIC_FILE, "r")) == NULL) {
      bug("Couldn't open music file, no songs available.", 0);
      fclose(fp);
      return;
    }

    for (count = 0; count < MAX_SONGS; count++) {
      letter = fread_letter(fp);
      if (letter == '#') {
        if (count < MAX_SONGS)
        song_table[count].name = NULL;
        fclose(fp);
        return;
      }
      else
      ungetc(letter, fp);

      song_table[count].group = fread_string(fp);
      song_table[count].name = fread_string(fp);

      /* read lyrics */
      lines = 0;

      for (;;) {
        letter = fread_letter(fp);

        if (letter == '~') {
          song_table[count].lines = lines;
          break;
        }
        else
        ungetc(letter, fp);

        if (lines >= MAX_LINES) {
          bug("Too many lines in a song -- limit is  %d.", MAX_LINES);
          break;
        }

        song_table[count].lyrics[lines] = fread_string_eol(fp);
        lines++;
      }
    }
  }

  _DOFUN(do_play) {
    OBJ_DATA *juke;
    char *str, arg[MAX_INPUT_LENGTH];
    int song, i;
    bool global = FALSE;

    str = one_argument(argument, arg);

    for (juke = ch->in_room->contents; juke != NULL; juke = juke->next_content)
    if (juke->item_type == ITEM_JUKEBOX && can_see_obj(ch, juke))
    break;

    if (argument[0] == '\0') {
      send_to_char("Play what?\n\r", ch);
      return;
    }

    if (juke == NULL) {
      send_to_char("You see nothing to play.\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "radio")) {
      if (juke->value[0] != -10) {
        juke->value[0] = -10;
        juke->value[1] = number_range(0, 14);
        send_to_char("You turn on the radio.\n\r", ch);
      }
      else {
        juke->value[0] = -1;
        juke->value[1] = -1;
        send_to_char("You turn off the radio.\n\r", ch);
      }
      return;
    }

    if (!str_cmp(arg, "list")) {
      Buffer outbuf;
      char buf[MAX_STRING_LENGTH];
      int col = 0;
      bool artist = FALSE, match = FALSE;

      argument = str;
      argument = one_argument(argument, arg);

      if (!str_cmp(arg, "artist"))
      artist = TRUE;

      if (argument[0] != '\0')
      match = TRUE;

      sprintf(buf, "%s has the following songs available:\n\r", juke->short_descr);
      outbuf.strcat(capitalize(buf));

      for (i = 0; i < MAX_SONGS; i++) {
        if (song_table[i].name == NULL)
        break;

        if (artist && (!match || !str_prefix(argument, song_table[i].group)))
        sprintf(buf, "%-39s %-39s\n\r", song_table[i].group, song_table[i].name);
        else if (!artist && (!match || !str_prefix(argument, song_table[i].name)))
        sprintf(buf, "%-35s ", song_table[i].name);
        else
        continue;
        outbuf.strcat(buf);
        if (!artist && ++col % 2 == 0)
        outbuf.strcat("\n\r");
      }
      if (!artist && col % 2 != 0)
      outbuf.strcat("\n\r");

      page_to_char(outbuf, ch);
      return;
    }
    /*
if (!str_cmp(arg,"loud"))
{
if (!IS_IMMORTAL(ch) )
{
send_to_char( "Sorry, only Immortals can play loud music
now.\n\r", ch ); }
else {

argument = str;
global = TRUE;
}
}
*/

    if (argument[0] == '\0') {
      send_to_char("Play what?\n\r", ch);
      return;
    }

    if ((global && channel_songs[MAX_GLOBAL] > -1) || (!global && juke->value[4] > -1)) {
      send_to_char("The jukebox is full up right now.\n\r", ch);
      return;
    }

    for (song = 0; song < MAX_SONGS; song++) {
      if (song_table[song].name == NULL) {
        send_to_char("That song isn't available.\n\r", ch);
        return;
      }
      if (!str_prefix(argument, song_table[song].name))
      break;
    }

    if (song >= MAX_SONGS) {
      send_to_char("That song isn't available.\n\r", ch);
      return;
    }

    send_to_char("Coming right up.\n\r", ch);

    if (global) {
      for (i = 1; i <= MAX_GLOBAL; i++)
      if (channel_songs[i] < 0) {
        if (i == 1)
        channel_songs[0] = -1;
        channel_songs[i] = song;
        return;
      }
    }
    else {
      for (i = 1; i < 5; i++)
      if (juke->value[i] < 0) {
        if (i == 1)
        juke->value[0] = -1;
        juke->value[i] = song;
        return;
      }
    }
  }

  bool is_snews_radio_space() {
    int i;
    for (i = 0; i < 15; i++) {
      if (snews_radio[i] == NULL || snews_radio[i][0] == '\0')
      return TRUE;
    }
    return FALSE;
  }
  void add_to_snews_radio(char *arg) {
    int i;
    if (is_snews_radio_space()) {
      for (i = 0; i < 15; i++) {
        if (snews_radio[i] == NULL || snews_radio[i][0] == '\0') {
          free_string(snews_radio[i]);
          snews_radio[i] = str_dup(arg);
          return;
        }
      }
    }
    i = number_range(1, 1000) % 15;
    free_string(snews_radio[i]);
    snews_radio[i] = str_dup(arg);
  }
  bool is_news_radio_space() {
    int i;
    for (i = 0; i < 15; i++) {
      if (news_radio[i] == NULL || news_radio[i][0] == '\0')
      return TRUE;
    }
    return FALSE;
  }
  void add_to_news_radio(char *arg) {
    int i;
    if (is_news_radio_space()) {
      for (i = 0; i < 15; i++) {
        if (news_radio[i] == NULL || news_radio[i][0] == '\0') {
          free_string(news_radio[i]);
          news_radio[i] = str_dup(arg);
          return;
        }
      }
    }
    i = number_range(1, 1000) % 15;
    free_string(news_radio[i]);
    news_radio[i] = str_dup(arg);
  }
  bool is_social_radio_space() {
    int i;
    for (i = 0; i < 15; i++) {
      if (social_radio[i] == NULL || social_radio[i][0] == '\0')
      return TRUE;
    }
    return FALSE;
  }
  void add_to_social_radio(char *arg) {
    int i;
    if (is_social_radio_space()) {
      for (i = 0; i < 15; i++) {
        if (social_radio[i] == NULL || social_radio[i][0] == '\0') {
          free_string(social_radio[i]);
          social_radio[i] = str_dup(arg);
          return;
        }
      }
    }
    i = number_range(1, 1000) % 15;
    free_string(social_radio[i]);
    social_radio[i] = str_dup(arg);
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif
