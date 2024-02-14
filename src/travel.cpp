#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <map>
#include "merc.h"
#include "olc.h"
#include "gsn.h"
#include "const.h"
#include "recycle.h"
#include "lookup.h"
#include "global.h"
#include "tables.h"

#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif



  vector<PLAYERROOM_TYPE *> PlayerroomVect;
  PLAYERROOM_TYPE *nullplayerroom;

#define PLAYERROOM_FILE "../data/playerrooms.txt"

  void biketowalk args((CHAR_DATA * ch));
  void pathtowalk args((CHAR_DATA * ch));
  void walktobus args((CHAR_DATA * ch));
  void planetowalk args((CHAR_DATA * ch));
  void unplace_car args((CHAR_DATA * ch));
  int speed_bonus args((CHAR_DATA * ch));
  void pile_out args((CHAR_DATA * ch));
  void ding_car args((CHAR_DATA * ch));
  bool valid_parking_spot args((ROOM_INDEX_DATA * room));
  void set_vehicle_location args((CHAR_DATA * ch, int val));
  bool driving_area args((ROOM_INDEX_DATA * room));

#define CAR_SPORT 1
#define CAR_SAFE 2
#define CAR_LUXURY 3
#define CAR_VAN 4
#define CAR_TOUGH 5
#define CAR_SPORTSBIKE 6
#define CAR_CRUISER 7

  const char *world_names[] = {"Earth",    "Other", "Wilds", "Godrealm", "Hell",  "Elsewhere"};

  bool room_empty(ROOM_INDEX_DATA *room) {
    CHAR_DATA *victim;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room == room)
      return FALSE;
    }
    return TRUE;
  }

  bool room_hostile(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    CHAR_DATA *victim;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != room)
      continue;

      if (IS_NPC(victim))
      continue;

      if (IS_FLAG(victim->comm, COMM_HOSTILE))
      return TRUE;
    }
    return FALSE;
  }

  bool sin_in_room(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    CHAR_DATA *victim;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room != room)
      continue;

      if (IS_NPC(victim))
      continue;

      if (IS_FLAG(victim->act, PLR_SINSPIRIT))
      return TRUE;
    }
    return FALSE;
  }

  bool room_ambush(ROOM_INDEX_DATA *room) {
    CHAR_DATA *victim;
    if (room == NULL)
    return FALSE;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->pcdata->ambush >= 50)
      return TRUE;

      if (!IS_FLAG(victim->act, PLR_GUEST) && victim->pcdata->bloodaura > 0)
      return TRUE;
    }
    return FALSE;
  }

  bool room_ambushing(ROOM_INDEX_DATA *room) {
    CHAR_DATA *victim;

    if (room == NULL)
    return FALSE;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->pcdata->ambush >= 2)
      return TRUE;

      if (!IS_FLAG(victim->act, PLR_GUEST) && victim->pcdata->bloodaura > 0)
      return TRUE;
    }
    return FALSE;
  }

  bool room_guest(ROOM_INDEX_DATA *room) {
    CHAR_DATA *victim;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (IS_FLAG(victim->act, PLR_GUEST))
      return TRUE;
    }
    return FALSE;
  }

  int pc_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_gm(victim))
      continue;

      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  int nonpatient_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_gm(victim))
      continue;
      if (clinic_patient(victim))
      continue;

      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  int nondetain_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_gm(victim))
      continue;
      if (victim->pcdata->detention_time > 0)
      continue;

      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  int power_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_gm(victim))
      continue;

      if (victim->in_room != room)
      continue;

      if (higher_power(victim))
      pop++;
    }
    return pop;
  }

  int abom_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_gm(victim))
      continue;

      if (victim->in_room != room)
      continue;

      if (is_abom(victim) && has_blackeyes(victim))
      pop++;
    }
    return pop;
  }

  int nonhelpless_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_gm(victim))
      continue;

      if (victim->in_room != room)
      continue;

      if (!is_helpless(victim))
      pop++;
    }
    return pop;
  }

  int npc_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (!IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  int room_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  bool other_players(CHAR_DATA *ch) {
    if (ch->in_room == NULL || ch->in_room->vnum < 300)
    return FALSE;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (is_dreaming(ch) && ch->pcdata->dream_room != victim->pcdata->dream_room)
      continue;
      if (!is_dreaming(ch) && victim->in_room != ch->in_room)
      continue;

      if (!same_player(ch, victim) && (ch->pcdata->account != victim->pcdata->account))
      return TRUE;
    }
    return FALSE;
  }

  int fight_room_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;

    for (CharList::iterator it = room->people->begin(); it != room->people->end();
    ++it) {
      victim = *it;

      if (victim == NULL)
      continue;

      if (IS_NPC(victim) && victim->race == RACE_CIVILIAN)
      continue;
      if (IS_NPC(victim) && victim->race == RACE_ANIMAL)
      continue;

      if (victim->in_room == NULL)
      continue;

      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  int getdistx(int vnum) {
    for (int i = 0; i < MAX_DIST; i++) {
      if (district_table[i].area == vnum)
      return district_table[i].x;
    }
    if (vnum >= 22 && vnum <= 25)
    return 0;

    return 5;
  }
  int getdisty(int vnum) {
    for (int i = 0; i < MAX_DIST; i++) {
      if (district_table[i].area == vnum)
      return district_table[i].y;
    }
    if (vnum >= 22 && vnum <= 25)
    return 0;

    return 5;
  }

  int travel_speed(CHAR_DATA *ch) {
    if (ch->pcdata->travel_type == TRAVEL_CAR) {
      return 60 - vehicle_quality(ch) * 2;
    }
    else if (ch->pcdata->travel_type == TRAVEL_BIKE)
    return 60 - vehicle_quality(ch) * 2;
    else if (ch->pcdata->travel_type == TRAVEL_FLY)
    return 45;
    else if (ch->pcdata->travel_type == TRAVEL_PLANE)
    return 10;

    return 60;
  }

  int traveling_x(CHAR_DATA *ch) {
    if (ch->pcdata->travel_from < 0 || ch->pcdata->travel_to < 0)
    return 30;
    if (get_room_index(ch->pcdata->travel_from) == NULL || get_room_index(ch->pcdata->travel_to) == NULL)
    return 30;
    if (ch->pcdata->travel_to < 2000)
    return 30;

    int fromx = getdistx(get_room_index(ch->pcdata->travel_from)->area->vnum);
    int fromy = getdisty(get_room_index(ch->pcdata->travel_from)->area->vnum);
    if (get_room_index(ch->pcdata->travel_from)->area->vnum == 4) {
      fromx = fromx - 500 + (get_room_index(ch->pcdata->travel_from)->locx * 10);
      fromy = fromy - 500 + (get_room_index(ch->pcdata->travel_from)->locy * 10);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum > 4 && get_room_index(ch->pcdata->travel_from)->area->vnum < 15) {
      fromx = fromx - 1250 + (get_room_index(ch->pcdata->travel_from)->locx * 25);
      fromy = fromy - 1250 + (get_room_index(ch->pcdata->travel_from)->locy * 25);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum == 2 || get_room_index(ch->pcdata->travel_from)->area->vnum == 3) {
      fromx = fromx - 150 + (get_room_index(ch->pcdata->travel_from)->locx);
      fromy = fromy - 150 + (get_room_index(ch->pcdata->travel_from)->locy);
    }

    int tox = getdistx(get_room_index(ch->pcdata->travel_to)->area->vnum);
    int toy = getdisty(get_room_index(ch->pcdata->travel_to)->area->vnum);

    if (get_room_index(ch->pcdata->travel_from)->area->vnum == 4) {
      tox = tox - 500 + (get_room_index(ch->pcdata->travel_to)->locx * 10);
      toy = toy - 500 + (get_room_index(ch->pcdata->travel_to)->locy * 10);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum > 4 && get_room_index(ch->pcdata->travel_from)->area->vnum < 15) {
      tox = tox - 1250 + (get_room_index(ch->pcdata->travel_to)->locx * 25);
      toy = toy - 1250 + (get_room_index(ch->pcdata->travel_to)->locy * 25);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum == 2 || get_room_index(ch->pcdata->travel_from)->area->vnum == 3) {
      tox = tox - 150 + (get_room_index(ch->pcdata->travel_to)->locx);
      toy = toy - 150 + (get_room_index(ch->pcdata->travel_to)->locy);
    }

    int proportion;
    int xmove;

    int totaldist = (tox - fromx) * (tox - fromx) + (toy - fromy) * (toy - fromy);

    totaldist = (int)(sqrt((double)totaldist));

    totaldist /= 2;

    int dist = totaldist - ch->pcdata->travel_time / travel_speed(ch);

    totaldist = UMAX(totaldist, 1);

    if (dist >= totaldist) {
      return tox;
    }
    else {
      proportion = 100 * dist / totaldist;
      xmove = (int)((tox - fromx) * proportion / 100);
      return (fromx + xmove);
    }
    return 30;
  }
  int traveling_y(CHAR_DATA *ch) {

    if (ch->pcdata->travel_from < 0 || ch->pcdata->travel_to < 0)
    return 30;
    if (get_room_index(ch->pcdata->travel_from) == NULL || get_room_index(ch->pcdata->travel_to) == NULL)
    return 30;
    if (ch->pcdata->travel_to < 2000)
    return 30;

    int fromx = getdistx(get_room_index(ch->pcdata->travel_from)->area->vnum);
    int fromy = getdisty(get_room_index(ch->pcdata->travel_from)->area->vnum);

    if (get_room_index(ch->pcdata->travel_from)->area->vnum == 4) {
      fromx = fromx - 500 + (get_room_index(ch->pcdata->travel_from)->locx * 10);
      fromy = fromy - 500 + (get_room_index(ch->pcdata->travel_from)->locy * 10);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum > 4 && get_room_index(ch->pcdata->travel_from)->area->vnum < 15) {
      fromx = fromx - 1250 + (get_room_index(ch->pcdata->travel_from)->locx * 25);
      fromy = fromy - 1250 + (get_room_index(ch->pcdata->travel_from)->locy * 25);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum == 2 || get_room_index(ch->pcdata->travel_from)->area->vnum == 3) {
      fromx = fromx - 150 + (get_room_index(ch->pcdata->travel_from)->locx);
      fromy = fromy - 150 + (get_room_index(ch->pcdata->travel_from)->locy);
    }

    int tox = getdistx(get_room_index(ch->pcdata->travel_to)->area->vnum);
    int toy = getdisty(get_room_index(ch->pcdata->travel_to)->area->vnum);

    if (get_room_index(ch->pcdata->travel_from)->area->vnum == 4) {
      tox = tox - 500 + (get_room_index(ch->pcdata->travel_to)->locx * 10);
      toy = toy - 500 + (get_room_index(ch->pcdata->travel_to)->locy * 10);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum > 4 && get_room_index(ch->pcdata->travel_from)->area->vnum < 15) {
      tox = tox - 1250 + (get_room_index(ch->pcdata->travel_to)->locx * 25);
      toy = toy - 1250 + (get_room_index(ch->pcdata->travel_to)->locy * 25);
    }
    else if (get_room_index(ch->pcdata->travel_from)->area->vnum == 2 || get_room_index(ch->pcdata->travel_from)->area->vnum == 3) {
      tox = tox - 150 + (get_room_index(ch->pcdata->travel_to)->locx);
      toy = toy - 150 + (get_room_index(ch->pcdata->travel_to)->locy);
    }

    int proportion;
    int ymove;

    int totaldist = (tox - fromx) * (tox - fromx) + (toy - fromy) * (toy - fromy);

    totaldist = (int)(sqrt((double)totaldist));

    totaldist /= 2;

    int dist = totaldist - ch->pcdata->travel_time / travel_speed(ch);

    totaldist = UMAX(totaldist, 1);

    if (dist >= totaldist) {
      return toy;
    }
    else {
      proportion = 100 * dist / totaldist;
      ymove = (int)((toy - fromy) * proportion / 100);
      return (fromy + ymove);
    }
    return 50;
  }

  int travel_time(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    int originx, originy;
    if (ch == NULL || ch->in_room == NULL)
    return 1;

    if (ch->pcdata->travel_time > 0) {
      originx = traveling_x(ch);
      originy = traveling_y(ch);
    }
    else {
      if (ch == NULL || ch->in_room == NULL || ch->in_room->area->vnum < 1 || ch->in_room->area->vnum > 30)
      return 1;

      if (ch->in_room == room)
      return 0;

      originx = getdistx(ch->in_room->area->vnum);
      originy = getdisty(ch->in_room->area->vnum);
      if (ch->in_room->area->vnum == 2 || ch->in_room->area->vnum == 3) {
        originx = originx - 150 + (ch->in_room->locx);
        originy = originy - 150 + (ch->in_room->locy);
      }
      else if (ch->in_room->area->vnum == 4) {
        originx = originx - 500 + (ch->in_room->locx * 10);
        originy = originy - 500 + (ch->in_room->locy * 10);
      }
      else if (ch->in_room->area->vnum > 4 && ch->in_room->area->vnum < 12) {
        originx = originx - 1250 + (ch->in_room->locx * 25);
        originy = originy - 1250 + (ch->in_room->locy * 25);
      }
      else if (ch->in_room->area->vnum > 25) {
        originx = originx + (ch->in_room->locx - 50) * 1200;
        originy = originy + (ch->in_room->locy - 50) * 1200;
      }
    }
    int destx = getdistx(room->area->vnum);
    int desty = getdisty(room->area->vnum);
    if (room->area->vnum == 2 || room->area->vnum == 3) {
      destx = destx - 150 + (room->locx);
      desty = desty - 150 + (room->locy);
    }
    else if (room->area->vnum == 4) {
      destx = destx - 500 + (room->locx * 10);
      desty = desty - 500 + (room->locy * 10);
    }
    else if (room->area->vnum > 4 && room->area->vnum < 12) {
      destx = destx - 1250 + (room->locx * 25);
      desty = desty - 1250 + (room->locy * 25);
    }
    else if (room->area->vnum > 25) {
      destx = destx + (room->locx - 50) * 1200;
      desty = desty + (room->locy - 50) * 1200;
    }

    //    printf_to_char(ch, "DX %d DY %d OX %d OY %d", destx, desty, originx, //    originy);

    int distx = originx - destx;
    int disty = originy - desty;
    distx = distx * distx;
    disty = disty * disty;
    int dist = distx + disty;
    dist = (int)sqrt((double)dist);

    dist /= 2;

    if (in_haven(ch->in_room) && (room->area->vnum == 2 || room->area->vnum == 3))
    dist /= 5;
    else if (in_haven(room) && (ch->in_room->area->vnum == 2 || ch->in_room->area->vnum == 3))
    dist /= 5;

    if (ch->in_room->area->vnum > 25 || room->area->vnum > 25)
    dist = UMAX(60, dist - 3000);

    return dist;
  }

  int get_po(ROOM_INDEX_DATA *room) {
    if (room->vnum == 2281)
    return 0;
    if (room->area->vnum >= 22 && room->area->vnum <= 30) {
      int returnval = room->vnum - 289999 + 5000;
      return returnval;
    }

    //    if(room->area->vnum == 2)
    return room->vnum - 30999;

    //    return room->vnum;
  }

  ROOM_INDEX_DATA *get_poroom(int val) {
    if (val == 0)
    return get_room_index(2281);

    if (val >= 15000)
    return get_room_index(val + 289999 - 5000);

    //    if(val >= 1 && val < 2000)
    return get_room_index(val + 30999);

    //    return NULL;
  }

  bool room_display(ROOM_INDEX_DATA *room, char *argument) {
    if (!str_cmp(argument, ""))
    return TRUE;

    if (!str_cmp(argument, "Public")) {
      if (!IS_SET(room->room_flags, ROOM_PUBLIC))
      return FALSE;
      else
      return TRUE;
    }
    else if (!str_cmp(argument, "Private")) {
      if (!IS_SET(room->room_flags, ROOM_PRIVATE))
      return FALSE;
      else
      return TRUE;
    }
    else if (!str_cmp(argument, "Locked")) {
      if (!IS_SET(room->room_flags, ROOM_LOCKED))
      return FALSE;
      else
      return TRUE;
    }
    for (int i = 0; i < MAX_DIST; i++) {
      if (!str_cmp(argument, district_table[i].name) || !str_cmp(argument, district_table[i].nickname)) {
        if (district_table[i].area == room->area->vnum)
        return TRUE;
      }
    }
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      if ((*it)->vnum == room->vnum) {
        if (!str_cmp((*it)->author, argument))
        return TRUE;
      }
    }

    return FALSE;
  }

  char *room_tag(CHAR_DATA *ch, ROOM_INDEX_DATA *room, ROOM_INDEX_DATA *in_room) {
    static char buf[MAX_INPUT_LENGTH];
    if (room == in_room)
    return "`cHere`x";

    if (in_haven(room) && in_haven(ch->in_room))
    return "`cHere`x";

    /*
if(room->time != in_room->time)
{
if(room->time == 3000)
return "Present";
if(room->time >= 0)
{
sprintf(buf, "%d AD", room->time);
return buf;
}
else
{
sprintf(buf, "%d BC", room->time*-1);
return buf;
}
}
*/
    if (in_room->area->vnum == DIST_DREAM || room->area->vnum == DIST_DREAM)
    return "";
    if (in_room->area->vnum == 12 || room->area->vnum == 12) {
      sprintf(buf, "%d Sec", travel_time(ch, room));
      return buf;
    }
    if (in_room->area->world != WORLD_ELSEWHERE && room->area->world != WORLD_ELSEWHERE && room->area->world != in_room->area->world) {
      return "---";
    }
    if (((in_room->area->vnum > 6 && in_room->area->vnum < 12 && !in_haven(in_room)) || (room->area->vnum > 6 && room->area->vnum < 12 && !in_haven(room))) && room->area->vnum != in_room->area->vnum) {
      sprintf(buf, "`g$`x%d", travel_time(ch, room) / 2);
      return buf;
    }
    else if (room->area->vnum > 25 || ch->in_room->area->vnum > 25) {
      sprintf(buf, "%d Hours", travel_time(ch, room) / 60);
      return buf;
    }
    else if (ch->in_room->area->vnum != room->area->vnum || room->area->vnum > 3) {
      sprintf(buf, "%d Min", travel_time(ch, room));
      return buf;
    }
    sprintf(buf, "%d Sec", travel_time(ch, room));
    return buf;
  }

  char *street_section(ROOM_INDEX_DATA *room) {
    static char buf[MSL];

    int x = room->locx;
    int y = room->locy;

    if (room->sector_type != SECT_STREET)
    return "";

    if (x >= 65 && y >= 65)
    sprintf(buf, ", Northeast Side, %s", room->area->name);
    else if (x >= 65 && y < 40)
    sprintf(buf, ", Southeast Side, %s", room->area->name);
    else if (x < 40 && y < 40)
    sprintf(buf, ", Southwest Side, %s", room->area->name);
    else if (x < 40 && y >= 65)
    sprintf(buf, ", Northwest Side, %s", room->area->name);
    else if (x >= 65)
    sprintf(buf, ", East Side, %s", room->area->name);
    else if (x < 40)
    sprintf(buf, ", West Side, %s", room->area->name);
    else if (y >= 65)
    sprintf(buf, ", North Side, %s", room->area->name);
    else if (y < 40)
    sprintf(buf, ", South Side, %s", room->area->name);
    else
    sprintf(buf, ", Central, %s", room->area->name);

    return buf;
  }

  ROOM_INDEX_DATA *get_street(ROOM_INDEX_DATA *room) {
    for (int i = 0; i < 10; i++) {
      if (IS_SET(room->room_flags, ROOM_PUBLIC))
      return room;

      if (room->exit[1] == NULL)
      return NULL;

      room = room->exit[1]->u1.to_room;
    }
    return room;
  }

  ROOM_INDEX_DATA *get_before_street(ROOM_INDEX_DATA *room) {
    ROOM_INDEX_DATA *temp = NULL;
    for (int i = 0; i < 10; i++) {
      if (IS_SET(room->room_flags, ROOM_PUBLIC))
      return temp;

      if (room->exit[1] == NULL)
      return NULL;

      temp = room;
      room = room->exit[1]->u1.to_room;
    }
    return temp;
  }

  int room_number(ROOM_INDEX_DATA *target, ROOM_INDEX_DATA *street) {
    ROOM_INDEX_DATA *room;
    int count = 1;

    if (target == NULL || street == NULL)
    return 0;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (room->area != street->area)
      continue;
      if (room == street)
      continue;
      if (IS_SET(room->room_flags, ROOM_INVISIBLE))
      continue;
      if (room->exit[1] == NULL || room->exit[1]->u1.to_room != street) {
        if (street->exit[1] == NULL || street->exit[1]->u1.to_room != room)
        continue;
      }

      if (room == target)
      return count;
      else
      count++;
    }
    return count;
  }

  int get_address_num(ROOM_INDEX_DATA *room) {
    ROOM_INDEX_DATA *temp;
    if (get_before_street(room) == NULL)
    temp = room;
    else
    temp = get_before_street(room);

    int base = get_street(room)->vnum % 27;

    base += 12;

    base +=
    room_number(temp, get_street(room)) * (get_street(room)->vnum % 13 + 3);

    return base;
  }

  char *get_address(ROOM_INDEX_DATA *room) {
    static char buf[MSL];

    if (room == NULL)
    return "";
    if (room->sector_type == SECT_STREET && IS_SET(room->room_flags, ROOM_PUBLIC))
    return street_section(room);

    if (get_street(room) == NULL)
    return "";
    if (get_street(room) == room)
    return "";
    if (IS_SET(room->room_flags, ROOM_PUBLIC))
    return "";
    ROOM_INDEX_DATA *temp = get_before_street(room);

    if (temp != NULL && temp != room && (temp->sector_type == SECT_ALLEY || temp->sector_type == SECT_STREET || temp->sector_type == SECT_TUNNELS || temp->sector_type == SECT_AIR || temp->sector_type == SECT_PARK)) {
      sprintf(buf, ", off %s", get_before_street(room)->name);
    }
    else if (room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY || room->sector_type == SECT_TUNNELS || room->sector_type == SECT_AIR || room->sector_type == SECT_PARK) {
      sprintf(buf, ", off %s", get_street(room)->name);
    }
    else
    sprintf(buf, ", %d %s", get_address_num(room), get_street(room)->name);

    return buf;
  }

  bool has_dream_exit(ROOM_INDEX_DATA *room, FANTASY_TYPE *fant) {
    if (room == NULL)
    return FALSE;
    for (int i = 0; i < 200; i++) {
      if (fant->exits[i] == room->vnum)
      return TRUE;
    }
    return FALSE;
  }

  _DOFUN(do_roomlist) {
    ROOM_INDEX_DATA *room;
    static char string[MSL];
    char buf[MSL];
    string[0] = '\0';

    room = get_room_index(2281);
    sprintf(buf, "\t\t[%4d] %s (%s)\n\r", get_po(room), room->name, room_tag(ch, room, ch->in_room));
    strcat(string, buf);

    FANTASY_TYPE *fant;
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if ((*it)->status != ROOM_PUBLIC && str_cmp(ch->name, (*it)->author))
      continue;

      if (room->area->vnum == 12 && !is_gm(ch))
      continue;

      if (room->area->vnum == 12 && str_cmp(ch->name, (*it)->author) && (*it)->status != ROOM_PUBLIC)
      continue;

      if (room->area->vnum == DIST_DREAM && !is_dreaming(ch))
      continue;
      //	if(is_dreaming(ch) && room->area->vnum != DIST_DREAM)
      //	    continue;

      if (room->area->vnum == DIST_DREAM && str_cmp(ch->name, (*it)->author)) {
        if ((fant = room_fantasy(room)) == NULL)
        continue;
        if (!part_of_fantasy(ch, fant))
        continue;
        if (room->vnum != fant->entrance && in_fantasy(ch) != fant)
        continue;
        if (has_dream_exit(room, fant))
        continue;
      }

      /*
if(!timetraveler(ch) && ch->in_room->time != room->time)
continue;

if(ch->in_room->time != 3000 && ch->in_room->time != room->time && room->vnum != ch->pcdata->portaled_from) continue;
*/
      sprintf(buf, "\t\t[%d] %s (%s)\n\r", get_po(room), room->name, room_tag(ch, room, ch->in_room));
      strcat(string, buf);
    }
    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
  }

  int dest_vnum(CHAR_DATA *ch, int point) {
    int i;
    ROOM_INDEX_DATA *room;
    for (i = 0; i < MAX_ROOMS; i++) {
      room = get_room_index(room_list[i]);
      if (point == i + 1)
      return room->vnum;
    }
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (IS_SET(room->room_flags, ROOM_PUBLIC))
      continue;
      if (room->area != ch->in_room->area)
      continue;
      if (point == i + 1)
      return room->vnum;
      i++;
    }
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (!IS_SET(room->room_flags, ROOM_PUBLIC))
      continue;
      if (point == i + 1)
      return room->vnum;
      i++;
    }
    return 0;
  }

  int highestoffense(CHAR_DATA *ch) {
    int i, max = 0;

    for (i = 0; i < DIS_USED; i++) {
      if (discipline_table[i].range != -1)
      continue;

      if (ch->disciplines[discipline_table[i].vnum] > max)
      max = ch->disciplines[discipline_table[i].vnum];
    }
    return max;
  }

  bool non_public_playerroom(ROOM_INDEX_DATA *room) {
    if (room->vnum == 2281)
    return FALSE;
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if (room->vnum == (*it)->vnum) {
        if ((*it)->status == ROOM_PUBLIC)
        return FALSE;
        else
        return TRUE;
      }
    }
    return TRUE;
  }

  _DOFUN(do_travel) {
    //    struct stat sb;
    char arg1[MSL];
    char arg2[MSL];
    int desti;
    argument = one_argument_nouncap(argument, arg1);
    int traveltype = TRAVEL_TAXI;
    int pass = 0;

    if (ch->race == RACE_FANTASY)
    return;

    if (is_ghost(ch)) {
      send_to_char("You feel unwilling and unable to part with Haven.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || ch->in_room->vnum < 200)
    return;

    if (in_fantasy(ch) != NULL) {
      send_to_char("You are asleep.\n\r", ch);
      return;
    }
    if (crisis_notravel == 1) {
      send_to_char("It doesn't work.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_HIDE)) {
      send_to_char("You would have to come out of hiding first.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_DEAD) || (ch->in_room != NULL && ch->in_room->vnum == 98)) {
      send_to_char("You're not a zombie.\n\r", ch);
      return;
    }
    if (is_helpless(ch) && ch->pcdata->dream_room == 0) {
      send_to_char("You're in no position to do that.\n\r", ch);
      return;
    }
    if (has_con(ch, SCON_NOMOVE)) {
      send_to_char("You can't do that now.\n\r", ch);
      return;
    }
    if (ch->in_room->vnum < 100 && !is_gm(ch)) {
      send_to_char("Wait until you're on grid first.\n\r", ch);
      return;
    }
    if (in_fight(ch)) {
      send_to_char("You're a bit busy.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "car")) {
      argument = one_argument_nouncap(argument, arg2);

      if (!has_active_vehicle(ch)) {
        send_to_char("You don't own a car.\n\r", ch);
        return;
      }
      if (vehicle_typeone(ch) == CAR_CRUISER || vehicle_typeone(ch) == CAR_SPORTSBIKE || vehicle_typeone(ch) == CAR_HORSE) {
        send_to_char("You don't own a car.\n\r", ch);
        return;
      }
      desti = atoi(arg2);
      traveltype = TRAVEL_CAR;
    }
    else if (!str_cmp(arg1, "motorcycle")) {
      argument = one_argument_nouncap(argument, arg2);

      if (!has_active_vehicle(ch)) {
        send_to_char("You don't own a bike.\n\r", ch);
        return;
      }
      if (vehicle_typeone(ch) != CAR_CRUISER && vehicle_typeone(ch) != CAR_SPORTSBIKE) {
        send_to_char("You don't own a bike.\n\r", ch);
        return;
      }

      desti = atoi(arg2);
      traveltype = TRAVEL_BIKE;
    }
    else if (!str_cmp(arg1, "horse")) {
      argument = one_argument_nouncap(argument, arg2);

      if (!has_active_vehicle(ch)) {
        send_to_char("You don't own a bike.\n\r", ch);
        return;
      }
      if (vehicle_typeone(ch) != CAR_HORSE) {
        send_to_char("You don't own a horse.\n\r", ch);
        return;
      }

      desti = atoi(arg2);
      traveltype = TRAVEL_HORSE;
    }
    else if (!str_cmp(arg1, "plane")) {
      argument = one_argument_nouncap(argument, arg2);

      desti = atoi(arg2);
      traveltype = TRAVEL_PLANE;
    }
    else if (!str_cmp(arg1, "pathing") || !str_cmp(arg1, "path")) {
      if (get_skill(ch, SKILL_PATHING) < 2) {
        send_to_char("You don't know how to do that.\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      desti = atoi(arg2);
      traveltype = TRAVEL_PATHING;
    }
    else if (is_number(arg1)) {
      desti = atoi(arg1);
      traveltype = TRAVEL_TAXI;
    }
    if (desti < 0) {
      send_to_char("Type roomlist for a list of rooms you can travel too.\n\r", ch);
      return;
    }
    if (in_haven(ch->in_room) && ch->in_room->vnum != 2281 && ch->in_room->vnum != 18999 && !is_gm(ch) && traveltype != TRAVEL_PATHING && !is_dreaming(ch)) {
      send_to_char("You have to get to the bus depot at Tranquil Land and Mariner's Highway first.\n\r", ch);
      return;
    }
    if (traveltype == TRAVEL_PATHING && institute_room(ch->in_room)) {
      send_to_char("You cannot path from within the institute.\n\r", ch);
      return;
    }

    int cab;
    ROOM_INDEX_DATA *room = NULL;
    if (get_poroom(desti) == NULL) {
      send_to_char("No such room.\n\r", ch);
      return;
    }
    room = get_poroom(desti);
    char buf[MSL];
    if (room == NULL) {
      send_to_char("No such room.\n\r", ch);
      return;
    }
    if (in_haven(room) && in_haven(ch->in_room)) {
      send_to_char("That's too close for you to travel too.\n\r", ch);
      return;
    }

    if (in_haven(room) && IS_FLAG(ch->comm, COMM_BANISHED)) {
      send_to_char("You're banished from Haven right now.\n\r", ch);
      return;
    }
    if (room->area->vnum == 12 && !is_gm(ch)) {
      send_to_char("There is no room there.\n\r", ch);
      return;
    }
    if (non_public_playerroom(room) && !is_gm(ch)) {
      send_to_char("There is no room there.\n\r", ch);
      return;
    }
    if (room->area->vnum == DIST_DREAM && !is_dreaming(ch)) {
      send_to_char("There is no room there.\n\r", ch);
      return;
    }
    if (room->area->vnum == DIST_DREAM && is_dreaming(ch) && ch->pcdata->dream_room != 0) {
      dream_move(ch, room);
      return;
    }
    if (is_asleep(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (room->area->world != WORLD_ELSEWHERE && room->area->world != in_world(ch)) {
      if (!is_gm(ch) || room->area->vnum > 25) {
        send_to_char("You need to find a path into that world first.\n\r", ch);
        return;
      }
    }

    if (room->sector_type == SECT_AIR && traveltype != TRAVEL_FLY) {
      send_to_char("I don't think you can jump that high.\n\r", ch);
      return;
    }
    if (IS_SET(room->room_flags, ROOM_INVISIBLE) && !is_gm(ch)) {
      send_to_char("There is no room there.\n\r", ch);
      return;
    }

    if (!timetraveler(ch) && ch->in_room->time != room->time) {
      send_to_char("There is no room there.\n\r", ch);
      return;
    }

    if (has_con(ch, SCON_LOCALMOVE)) {
      if (room->exit[1] == NULL || room->exit[1]->u1.to_room != ch->in_room) {
        if (ch->in_room->exit[1] == NULL || ch->in_room->exit[1]->u1.to_room != room) {
          send_to_char("You can't do that now.\n\r", ch);
          return;
        }
      }
    }

    if (((ch->in_room->area->vnum > 6 && ch->in_room->area->vnum < 12) || (room->area->vnum > 6 && room->area->vnum < 12)) && (!in_haven(ch->in_room) || !in_haven(room)) && room->area->vnum != ch->in_room->area->vnum && traveltype != TRAVEL_PLANE && traveltype != TRAVEL_PATHING && room->time == ch->in_room->time && !is_gm(ch)) {
      send_to_char("You'd need to get on a plane to go there. (travel plane <number>)\n\r", ch);
      return;
    }
    if ((ch->in_room->area->vnum >= 22 && ch->in_room->area->vnum <= 28) || (room->area->vnum >= 22 && room->area->vnum <= 28)) {
      if (event_cleanse == 1) {
        send_to_char("The gates are closed.\n\r", ch);
        return;
      }
      if (traveltype != TRAVEL_HORSE && traveltype != TRAVEL_HPASSENGER && traveltype != TRAVEL_PATHING && !is_gm(ch)) {
        send_to_char("You can't use those methods of travel in that world.\n\r", ch);
        return;
      }
    }
    else {
      if (traveltype == TRAVEL_HORSE || traveltype == TRAVEL_HPASSENGER) {
        send_to_char("That's a bit of an antiquated way to get around earth.\n\r", ch);
        return;
      }
    }

    bool timetraveling = FALSE;
    if (ch->in_room->time != room->time)
    timetraveling = TRUE;

    /*
if(!IS_SET(room->room_flags, ROOM_PUBLIC) && room->vnum !=
get_hq(ch->faction)->vnum)
{
if(room->exit[1] == NULL || room->exit[1]->u1.to_room !=
ch->in_room)
{
if(ch->in_room->exit[1] == NULL|| ch->in_room->exit[1]->u1.to_room != room)
{
send_to_char("You're not close enough to go there.\n\r", ch); return;
}
}

if(room->area != ch->in_room->area)
{
send_to_char("You're not close enough to go there.\n\r", ch);
return;
}
}
*/

    int time = travel_time(ch, room) * 60;
    if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
    time /= 60;

    if (ch->pcdata->travel_prepped == FALSE && (traveltype == TRAVEL_PATHING || traveltype == TRAVEL_HORSE)) {
      char buf[MSL];
      sprintf(buf, "%s %s %s", arg1, arg2, argument);
      ch->pcdata->process = PROCESS_TRAVEL_PREP;
      ch->pcdata->process_timer = 5;
      ch->pcdata->process_subtype = traveltype;
      free_string(ch->pcdata->process_argumentone);
      ch->pcdata->process_argumentone = str_dup(buf);
      send_to_char("You start preparing to travel.\n\r", ch);
      if (traveltype == TRAVEL_HORSE) {
        act("$n starts to saddle and prepare $s horse for travel.", ch, NULL, NULL, TO_ROOM);
      }
      else
      act("Reality starts to bend a little around $n.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else
    ch->pcdata->travel_prepped = FALSE;

    int tempcab;

    for (cab = INIT_CABS; cab < END_CABS && !room_empty(get_room_index(cab));
    cab++) {
    }

    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      CHAR_DATA *vch;
      vch = *it;
      ++it;

      if (vch == NULL)
      continue;

      if (vch->in_room == NULL)
      continue;

      if (IS_NPC(vch))
      continue;

      if (vch->in_room != ch->in_room) {
        if (vch->master != ch || vch->pcdata->travel_time > ch->pcdata->travel_time + 30 || ch->pcdata->travel_time <= 0) {
          continue;
        }
      }

      if (ch == vch)
      continue;

      if (is_gm(ch) && IS_FLAG(ch->comm, COMM_RUNNING) && !is_name(PERS(vch, ch), argument))
      continue;

      if (clinic_patient(vch))
      continue;

      if (vch->master != ch) {
        if (!is_gm(ch) || !IS_FLAG(ch->comm, COMM_RUNNING)) {
          if (!is_helpless(vch) || !is_name(PERS(vch, ch), argument)) {
            continue;
          }
        }
      }

      if (room->area->world > WORLD_EARTH && room->area->world <= WORLD_HELL && is_helpless(vch) && under_understanding(vch, ch))
      continue;
      if (room->area->world > WORLD_EARTH && room->area->world <= WORLD_HELL && is_helpless(vch) && under_limited(vch, ch))
      continue;

      if (!IS_NPC(vch)) {
        vch->pcdata->cansee = NULL;
        vch->pcdata->cansee2 = NULL;
        vch->pcdata->cansee3 = NULL;
      }

      if (IS_FLAG(vch->comm, COMM_STALL))
      REMOVE_FLAG(vch->comm, COMM_STALL);
      if (IS_FLAG(vch->comm, COMM_REPORTED))
      REMOVE_FLAG(vch->comm, COMM_REPORTED);

      if (!IS_NPC(vch)) {
        free_string(vch->pcdata->place);
        vch->pcdata->place = str_dup("");
        set_title(ch, ""); // Took set_title out of move_char_move - Discordance
      }

      if (is_helpless(vch) && in_haven(vch->in_room) && !in_haven(room) && under_understanding(vch, ch)) {
        continue;
      }
      if (is_helpless(vch) && in_haven(vch->in_room) && !in_haven(room) && under_limited(vch, ch)) {
        continue;
      }

      if (traveltype == TRAVEL_CAR) {

        if (is_name(PERS(vch, ch), argument) && pass < 4) {
          printf_to_char(vch, "You get into %s's %s.\n\r", PERS(ch, vch), vehicle_name(ch));
          vch->pcdata->travel_time =
          travel_time(ch, room) * (30 - vehicle_quality(ch) * 2);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_CPASSENGER;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          pass++;
          char_from_room(vch);
          char_to_room(vch, get_room_index(cab));
        }
        else if (has_active_vehicle(vch)) {
          printf_to_char(vch, "You get into your %s.\n\r", vehicle_name(vch));
          vch->pcdata->travel_time =
          travel_time(vch, room) * (30 - vehicle_quality(vch) * 2);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_CAR;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          for (tempcab = INIT_CABS;
          tempcab < END_CABS && !room_empty(get_room_index(tempcab));
          tempcab++) {
          }
          char_from_room(vch);
          char_to_room(vch, get_room_index(tempcab));
          cartowalk(vch);
        }

      }
      else if (traveltype == TRAVEL_BIKE) {
        if (is_name(PERS(vch, ch), argument) && pass < 1) {
          printf_to_char(vch, "You get onto %s's %s.\n\r", PERS(ch, vch), vehicle_name(ch));
          vch->pcdata->travel_time =
          travel_time(ch, room) * (30 - vehicle_quality(ch) * 2);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_BPASSENGER;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          pass++;
          char_from_room(vch);
          char_to_room(vch, get_room_index(cab));
        }
        else if (has_active_vehicle(vch)) {
          printf_to_char(vch, "You get onto your %s.\n\r", vehicle_name(vch));
          vch->pcdata->travel_time =
          travel_time(vch, room) * (30 - vehicle_quality(vch) * 2);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_BIKE;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          for (tempcab = INIT_CABS;
          tempcab < END_CABS && !room_empty(get_room_index(tempcab));
          tempcab++) {
          }
          char_from_room(vch);
          char_to_room(vch, get_room_index(tempcab));
          biketowalk(vch);
        }
      }
      else if (traveltype == TRAVEL_HORSE) {
        if (is_name(PERS(vch, ch), argument) && pass < 1) {
          printf_to_char(vch, "You get onto %s's %s.\n\r", PERS(ch, vch), vehicle_name(ch));
          vch->pcdata->travel_time =
          travel_time(ch, room) * (45 - vehicle_quality(ch) * 2);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_HPASSENGER;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          pass++;
          char_from_room(vch);
          char_to_room(vch, get_room_index(cab));
        }
        else if (has_active_vehicle(vch)) {
          printf_to_char(vch, "You get onto your %s.\n\r", vehicle_name(vch));
          vch->pcdata->travel_time =
          travel_time(vch, room) * (45 - vehicle_quality(vch) * 2);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_HORSE;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          for (tempcab = INIT_CABS;
          tempcab < END_CABS && !room_empty(get_room_index(tempcab));
          tempcab++) {
          }
          char_from_room(vch);
          char_to_room(vch, get_room_index(tempcab));
          biketowalk(vch);
        }

      }
      else if (traveltype == TRAVEL_PATHING) {
        if (is_name(PERS(vch, ch), argument) && pass < 1) {
          printf_to_char(vch, "You follow %s.\n\r", PERS(ch, vch));
          vch->pcdata->travel_time = travel_time(ch, room);
          vch->pcdata->travel_time = 300;
          vch->pcdata->travel_type = TRAVEL_PATHING;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          pass++;
          char_from_room(vch);
          char_to_room(vch, get_room_index(cab));
        }
        else if (get_skill(vch, SKILL_PATHING) >= 2) {
          printf_to_char(vch, "You walk into thin air.\n\r");
          vch->pcdata->travel_time = 300;
          vch->pcdata->travel_type = TRAVEL_PATHING;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          for (tempcab = INIT_CABS;
          tempcab < END_CABS && !room_empty(get_room_index(tempcab));
          tempcab++) {
          }
          char_from_room(vch);
          char_to_room(vch, get_room_index(tempcab));
          pathtowalk(vch);
        }
      }
      else if (traveltype == TRAVEL_CAB) {
        if (is_name(PERS(vch, ch), argument) && pass < 1) {
          printf_to_char(vch, "You get into a taxi with %s.\n\r", PERS(ch, vch));
          vch->pcdata->travel_time = travel_time(ch, room) * (30);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_CAB;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          pass++;
          char_from_room(vch);
          char_to_room(vch, get_room_index(cab));
        }
        else if (has_active_vehicle(vch)) {
          printf_to_char(vch, "You get into your %s.\n\r", vehicle_name(vch));
          vch->pcdata->travel_time =
          travel_time(vch, room) * (30 - vehicle_quality(vch) * 2);
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_CAR;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          for (tempcab = INIT_CABS;
          tempcab < END_CABS && !room_empty(get_room_index(tempcab));
          tempcab++) {
          }
          char_from_room(vch);
          char_to_room(vch, get_room_index(tempcab));
          cartowalk(vch);
        }
      }
      else if (traveltype == TRAVEL_FLY) {
        if (is_name(PERS(vch, ch), argument) && pass < 1) {
          printf_to_char(vch, "%s lifts you up into the air.\n\r", PERS(ch, vch));
          vch->pcdata->travel_time = travel_time(ch, room) * 25;
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_FLY;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          pass++;
          char_from_room(vch);
          char_to_room(vch, get_room_index(cab));
        }
        else if (get_skill(vch, SKILL_FLIGHT) > 2) {
          printf_to_char(vch, "You lift off into the air.\n\r");
          vch->pcdata->travel_time = travel_time(ch, room) * 25;
          if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
          vch->pcdata->travel_time /= 60;
          vch->pcdata->travel_type = TRAVEL_FLY;
          vch->pcdata->travel_to = room->vnum;
          vch->pcdata->travel_from = ch->in_room->vnum;
          char_from_room(vch);
          char_to_room(vch, get_room_index(cab));
          if (IS_FLAG(ch->act, PLR_SHADOW) && !IS_FLAG(ch->act, PLR_HIDE))
          SET_FLAG(ch->act, PLR_HIDE);
        }
      }
      else if (traveltype == TRAVEL_PLANE) {
        printf_to_char(
        vch, "You board the plane behind %s, paying %d for your ticket.\n\r", PERS(ch, vch), travel_time(vch, room) / 2);
        if (!is_helpless(vch))
        vch->pcdata->total_money -= 100 * travel_time(ch, room) / 2;
        else
        ch->pcdata->total_money -= 100 * travel_time(ch, room) / 2;
        vch->pcdata->travel_time = travel_time(ch, room) * 10;
        printf_to_char(vch, "As you get seated the pilot informs the cabin of an expected %d hour flight.\n\r", vch->pcdata->travel_time / 3600);
        vch->pcdata->travel_type = TRAVEL_PLANE;
        vch->pcdata->travel_to = room->vnum;
        vch->pcdata->travel_from = ch->in_room->vnum;
        pass++;
        char_from_room(vch);
        char_to_room(vch, get_room_index(cab));
      }
      else {
        if (time > 5)
        printf_to_char(vch, "You get on a bus to travel towards %s.\n\r", room->name);
        else
        printf_to_char(vch, "You start walking towards %s.\n\r", room->name);
        vch->pcdata->travel_time = travel_time(ch, room) * 60;
        if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
        vch->pcdata->travel_time /= 60;
        vch->pcdata->travel_type = TRAVEL_BUS;
        vch->pcdata->travel_to = room->vnum;
        vch->pcdata->travel_from = ch->in_room->vnum;

        char_from_room(vch);
        char_to_room(vch, get_room_index(cab));
        if (IS_FLAG(ch->act, PLR_SHADOW) && !IS_FLAG(ch->act, PLR_HIDE))
        SET_FLAG(ch->act, PLR_HIDE);
      }
      if (timetraveling == TRUE)
      vch->pcdata->portaled_from = vch->pcdata->travel_from;

      if (is_gm(ch) || timetraveling == TRUE)
      vch->pcdata->travel_time = 2;

      if (get_room_index(vch->pcdata->travel_to) != NULL && get_room_index(vch->pcdata->travel_to)->exit[1] != NULL && get_room_index(vch->pcdata->travel_to)->exit[1]->u1.to_room ==
          get_room_index(vch->pcdata->travel_from)) {
        vch->pcdata->travel_time = 1;
      }
      if (get_room_index(vch->pcdata->travel_from) != NULL && get_room_index(vch->pcdata->travel_from)->exit[1] != NULL && get_room_index(vch->pcdata->travel_from)->exit[1]->u1.to_room ==
          get_room_index(vch->pcdata->travel_to)) {
        vch->pcdata->travel_time = 1;
      }
    }
    if (!IS_NPC(ch)) {
      ch->pcdata->cansee = NULL;
      ch->pcdata->cansee2 = NULL;
      ch->pcdata->cansee3 = NULL;
    }

    if (IS_FLAG(ch->comm, COMM_REPORTED))
    REMOVE_FLAG(ch->comm, COMM_REPORTED);
    if (IS_FLAG(ch->comm, COMM_STALL))
    REMOVE_FLAG(ch->comm, COMM_STALL);

    if (!IS_NPC(ch)) {
      free_string(ch->pcdata->place);
      ch->pcdata->place = str_dup("");
      set_title(ch, ""); // Took set_title out of move_char_move - Discordance
    }

    if (traveltype == TRAVEL_CAR) {
      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        printf_to_char(ch, "You head outside and get into your %s.\n\r", vehicle_name(ch));
        sprintf(buf, "$n heads outside and gets into $s %s.", vehicle_name(ch));
      }
      else {
        printf_to_char(ch, "You get into your %s.\n\r", vehicle_name(ch));
        sprintf(buf, "$n gets into $s %s.", vehicle_name(ch));
      }
      unplace_car(ch);
      ch->pcdata->travel_time =
      travel_time(ch, room) * (30 - vehicle_quality(ch) * 2);
      if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
      ch->pcdata->travel_time /= 60;
      ch->pcdata->travel_type = TRAVEL_CAR;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      cartowalk(ch);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    else if (traveltype == TRAVEL_BIKE) {
      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        printf_to_char(ch, "You head outside and get onto your %s.\n\r", vehicle_name(ch));
        sprintf(buf, "$n heads outside and gets onto $s %s.", vehicle_name(ch));
      }
      else {
        printf_to_char(ch, "You get onto your %s.\n\r", vehicle_name(ch));
        sprintf(buf, "$n gets onto $s %s.", vehicle_name(ch));
      }
      unplace_car(ch);
      ch->pcdata->travel_time =
      travel_time(ch, room) * (30 - vehicle_quality(ch) * 2);
      if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
      ch->pcdata->travel_time /= 60;

      ch->pcdata->travel_type = TRAVEL_BIKE;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      biketowalk(ch);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    else if (traveltype == TRAVEL_HORSE) {
      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        printf_to_char(ch, "You head outside and get onto your %s.\n\r", vehicle_name(ch));
        sprintf(buf, "$n heads outside and gets onto $s %s.", vehicle_name(ch));
      }
      else {
        printf_to_char(ch, "You get onto your %s.\n\r", vehicle_name(ch));
        sprintf(buf, "$n gets onto $s %s.", vehicle_name(ch));
      }
      unplace_car(ch);
      ch->pcdata->travel_time =
      travel_time(ch, room) * (45 - vehicle_quality(ch) * 2);
      if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
      ch->pcdata->travel_time /= 60;

      ch->pcdata->travel_type = TRAVEL_HORSE;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      biketowalk(ch);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    else if (traveltype == TRAVEL_CAB) {
      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        printf_to_char(ch, "You head outside and get into a taxi.\n\r");
        sprintf(buf, "$n heads outside and gets into a taxi.");
      }
      else {
        printf_to_char(ch, "You get into a taxi.\n\r");
        sprintf(buf, "$n gets into a taxi.");
      }
      ch->pcdata->travel_time = travel_time(ch, room) * (30);
      if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
      ch->pcdata->travel_time /= 60;

      ch->money -= 10;
      ch->money -= ch->pcdata->travel_time / 10;
      ch->pcdata->travel_type = TRAVEL_CAB;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      cabtowalk(ch);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    else if (traveltype == TRAVEL_PATHING) {
      printf_to_char(ch, "You walk into thin air.\n\r");
      sprintf(buf, "$n walks into thin air.");
      ch->pcdata->travel_time = 300;

      ch->pcdata->travel_type = TRAVEL_PATHING;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      pathtowalk(ch);
      act(buf, ch, NULL, NULL, TO_ROOM);
    }
    else if (traveltype == TRAVEL_FLY) {
      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        printf_to_char(ch, "You head outside and lift off into the air.\n\r");
        sprintf(buf, "$n heads outside and lifts off into the air.");
      }
      else {
        printf_to_char(ch, "You lift off into the air.\n\r");
        sprintf(buf, "$n lifts off into the air.");
      }
      act(buf, ch, NULL, NULL, TO_ROOM);
      ch->pcdata->travel_time = travel_time(ch, room) * 25;
      if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
      ch->pcdata->travel_time /= 60;

      ch->pcdata->travel_type = TRAVEL_FLY;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      flytowalk(ch);
    }
    else if (traveltype == TRAVEL_PLANE) {
      if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) {
        printf_to_char(ch, "You head outside and hail a taxi to take you to the airport, paying $%d for your ticket.\n\r", travel_time(ch, room) / 2);
        sprintf(buf, "$n heads outside and hails a taxi to the airport.");
      }
      else {
        printf_to_char(ch, "You hail a taxi to take you to the airport, paying $%d for your ticket.\n\r", travel_time(ch, room) / 2);
        sprintf(buf, "$n hails a taxi to the airport.");
      }
      act(buf, ch, NULL, NULL, TO_ROOM);
      ch->pcdata->total_money -= 100 * travel_time(ch, room) / 2;
      ch->pcdata->travel_time = travel_time(ch, room) * 10;

      printf_to_char(ch, "As you get seated the pilot informs the cabin of an expected %d hour flight time.\n\r", ch->pcdata->travel_time / 3600);
      ch->pcdata->travel_type = TRAVEL_PLANE;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      planetowalk(ch);
    }
    else {
      if (time > 5)
      printf_to_char(ch, "You get on a bus to travel towards %s.\n\r", room->name);
      else
      printf_to_char(ch, "You start walking towards %s.\n\r", room->name);
      ch->pcdata->travel_time = travel_time(ch, room) * 60;
      if (ch->in_room->area->vnum == room->area->vnum && room->area->vnum <= 3)
      ch->pcdata->travel_time /= 60;
      ch->pcdata->travel_type = TRAVEL_BUS;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;
      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
      walktobus(ch);
      if (IS_FLAG(ch->act, PLR_SHADOW) && !IS_FLAG(ch->act, PLR_HIDE))
      SET_FLAG(ch->act, PLR_HIDE);
    }

    if (timetraveling == TRUE)
    ch->pcdata->portaled_from = ch->pcdata->travel_from;
    if (is_gm(ch) || timetraveling == TRUE)
    ch->pcdata->travel_time = 1;

    if (get_room_index(ch->pcdata->travel_to)->exit[1] != NULL && get_room_index(ch->pcdata->travel_to)->exit[1]->u1.to_room ==
        get_room_index(ch->pcdata->travel_from)) {
      ch->pcdata->travel_time = 1;
    }
    if (get_room_index(ch->pcdata->travel_from)->exit[1] != NULL && get_room_index(ch->pcdata->travel_from)->exit[1]->u1.to_room ==
        get_room_index(ch->pcdata->travel_to)) {
      ch->pcdata->travel_time = 1;
    }
  }

  _DOFUN(do_taxi) {
    char arg1[MSL];
    char buf[MSL];
    argument = one_argument_nouncap(argument, arg1);
    ROOM_INDEX_DATA *room;
    int i;
    int desti = 6730;
    int disty;
    int distx;
    int dist;
    int point = 0;
    int cab;

    send_to_char("You should check out the exciting new public transport system instead, walking.\n\r", ch);
    return;

    if (is_ghost(ch)) {
      send_to_char("You had not fare even for the ferryman.\n\r", ch);
      return;
    }

    if (is_hybrid(ch)) {
      send_to_char("The cabbie makes a point of avoiding you.\n\r", ch);
      return;
    }

    if (is_animal(ch)) {
      send_to_char("No cabbies are interested in picking you up, but maybe animal control is.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("No cabbie will pick you up while you're fighting.\n\r", ch);
      return;
    }

    if (is_pinned(ch)) {
      send_to_char("You're in no position to hail a cab.\n\r", ch);
      return;
    }

    if (is_helpless(ch)) {
      send_to_char("You're in no position to hail a cab.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "list")) {
      send_to_char("Taxi's currently run to the following locations\n\r", ch);
      for (i = 0; i < MAX_TAXIS; i++) {
        if ((room = get_room_index(taxi_table[i].vnum)) != NULL)
        printf_to_char(ch, "`W[`c%d`W]`x %s\n\r", i + 1, room->name);
      }
      return;
    }
    bool found = FALSE;
    for (i = 0; i < MAX_TAXIS; i++) {
      if (ch->in_room->vnum == taxi_table[i].vnum) {
        found = TRUE;
        point = i;
      }
    }

    if (found == FALSE) {
      send_to_char("You must be at an intersection.\n\r", ch);
      return;
    }
    if (!is_number(arg1)) {
      for (i = 0; i < MAX_TAXIS; i++) {
        if ((room = get_room_index(taxi_table[i].vnum)) != NULL) {
          if (!str_cmp(argument, room->name)) {
            desti = i + 1;
          }
        }
      }
    }
    else
    desti = atoi(arg1);

    if (desti < 1 || desti > MAX_TAXIS) {
      printf_to_char(ch, "Syntax: taxi 1 - %d\n\r", MAX_TAXIS);
      return;
    }
    desti--;
    distx = taxi_table[point].x - taxi_table[desti].x;
    disty = taxi_table[point].y - taxi_table[desti].y;
    distx = distx * distx;
    disty = disty * disty;
    dist = distx + disty;
    dist = (int)sqrt((double)dist);
    dist *= 2;
    dist =
    street_distance(ch->in_room, get_room_index(taxi_table[desti].vnum), ch);
    dist = UMAX(dist, 2);
    if (ch->money < dist * 5) {
      printf_to_char(ch, "That trip would cost %d.\n\r", dist / 20);
      return;
    }
    ch->money -= dist * 5;

    for (cab = INIT_CABS; cab < END_CABS && !room_empty(get_room_index(cab));
    cab++) {
    }

    ROOM_INDEX_DATA *travelroom = get_room_index(cab);

    free_string(travelroom->name);
    travelroom->name = str_dup("The back of a Taxi");
    free_string(travelroom->description);
    travelroom->description = str_dup("The backseat of this taxi is comfortable, if not a little worn, the black\nleather cracked in some places.  A cage protects the driver, save for a\nsmall little slot for someone to slide some cash through.  The streets of\nthe town slide by at a steady pace, the pair of fuzzy dice bouncing up and\ndown under rearview mirror.");
    free_string(travelroom->shroud);
    travelroom->shroud =
    str_dup("The backseat of the taxi looks twisted, like some sort of small, surreal\nchariot.  The seats, while comfortable, looks like some sort of moss\nunderneath you, and a rusted cage prevents you from moving over the seats.\nThe steering wheel moves on its own, and the rearview mirror shows an even\nmore twisted reflection of reality as the streets blur by.");
    if (!IS_SET(travelroom->room_flags, ROOM_INDOORS))
    SET_BIT(travelroom->room_flags, ROOM_INDOORS);

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;

      vch = CH(d);
      if (vch == NULL)
      continue;

      if (vch->in_room != ch->in_room)
      continue;

      if (ch == vch)
      continue;

      if (!is_name(PERS(vch, ch), argument) && !is_ghost(vch))
      continue;

      if (vch->master != ch && !is_ghost(vch) && (!is_gm(ch) || !IS_FLAG(ch->comm, COMM_RUNNING)))
      continue;
      if (is_ghost(vch) && vch->possessing != ch)
      continue;

      char_from_room(vch);
      char_to_room(vch, get_room_index(cab));

      printf_to_char(vch, "You jump in a taxi and head towards %s.\n\r", get_room_index(taxi_table[desti].vnum)->name);
      vch->pcdata->travel_time = dist;
      vch->pcdata->travel_type = TRAVEL_TAXI;
      vch->pcdata->travel_to = taxi_table[desti].vnum;
    }

    printf_to_char(
    ch, "You jump in a taxi, paying the driver %d, and head towards %s.\n\r", dist, get_room_index(taxi_table[desti].vnum)->name);
    sprintf(buf, "$n jumps in a taxi headed to %s.", get_room_index(taxi_table[desti].vnum)->name);
    act(buf, ch, NULL, NULL, TO_ROOM);
    dact(buf, ch, NULL, NULL, DISTANCE_MEDIUM);
    char_from_room(ch);
    char_to_room(ch, get_room_index(cab));
    ch->pcdata->travel_time = dist;
    ch->pcdata->travel_type = TRAVEL_TAXI;
    ch->pcdata->travel_to = taxi_table[desti].vnum;
  }

  void makevehicle(CHAR_DATA *ch) {
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;

    if (vehicle_typeone(ch) == CAR_HORSE)
    pMobIndex = get_mob_index(162);
    else if (vehicle_typeone(ch) == CAR_CRUISER || vehicle_typeone(ch) == CAR_SPORTSBIKE)
    pMobIndex = get_mob_index(161);
    else
    pMobIndex = get_mob_index(160);
    mob = create_mobile(pMobIndex);
    char_to_room(mob, ch->in_room);
    mob->hit = max_hp(mob);
    mob->ttl = 300;
    if (safe_strlen(vehicle_name(ch)) > 3) {
      free_string(mob->short_descr);
      mob->short_descr = str_dup(vehicle_name(ch));
      free_string(mob->name);
      mob->name = str_dup(vehicle_name(ch));
      free_string(mob->long_descr);
      mob->long_descr = str_dup(vehicle_name(ch));
    }
    else {
      free_string(mob->short_descr);
      mob->short_descr = str_dup("A car");
      free_string(mob->name);
      mob->name = str_dup("car");
    }

    char buf[MSL];
    free_string(mob->description);
    if (mob->pIndexData->vnum == 162)
    sprintf(buf, "%s %s\n%s\n\t`W|`R%s`W|`x\n", carqualityname(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch)), cartypename(vehicle_typeone(ch), vehicle_typetwo(ch)), vehicle_desc(ch), vehicle_lplate(ch));
    else
    sprintf(buf, "%s %s\n%s`x\n", carqualityname(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch)), cartypename(vehicle_typeone(ch), vehicle_typetwo(ch)), vehicle_desc(ch));

    mob->description = str_dup(buf);

    ch->your_car = mob;
    mob->your_car = ch;
    ch->car_inroom = ch->in_room->vnum;
  }

  int vehicle_quality(CHAR_DATA *ch) {
    return carqualitylevel(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch));
  }
  int max_passengers(CHAR_DATA *ch) {
    int typeone = vehicle_typeone(ch);
    int typetwo = vehicle_typetwo(ch);

    if (typeone == CAR_CRUISER || typeone == CAR_SPORTSBIKE)
    return 1;
    if (typeone == CAR_HORSE)
    return 1;

    if (typeone == CAR_SPORT && typetwo == -1)
    return 1;

    if (typeone == CAR_VAN || typetwo == CAR_VAN)
    return 8;

    return 4;
  }

  void remember_location(CHAR_DATA *ch, char *argument) {
    char arg1[MSL];
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    int val = atoi(arg1);

    if (ch->in_room->sector_type != SECT_STREET && ch->in_room->sector_type != SECT_ALLEY) {
      send_to_char("You can only use this on streets.\n\r", ch);
      return;
    }

    if (val < 1 || val > 10) {
      send_to_char("Walk remember (1-10) (name)\n\r", ch);
      return;
    }
    if (safe_strlen(arg2) < 2) {
      send_to_char("Walk remember (1-10) (name)\n\r", ch);
      return;
    }
    val--;

    free_string(ch->pcdata->drivenames[val]);
    ch->pcdata->drivenames[val] = str_dup(arg2);
    ch->pcdata->driveloc[val] = ch->in_room->vnum;
    send_to_char("Done.\n\r", ch);
  }

  bool knows_world(CHAR_DATA *ch, int world) {
    if (world == WORLD_EARTH)
    return TRUE;
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (knows_gateway(ch, (*it)->havenroom) && (*it)->world == world)
      return TRUE;
    }
    return FALSE;
  }

  bool can_world_travel(CHAR_DATA *ch, ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *to_room) {

    if (knows_world(ch, in_room->area->world) && knows_world(ch, to_room->area->world))
    return TRUE;
    return FALSE;
  }

  bool has_town_vehicle(CHAR_DATA *ch) {
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_status[i] == GARAGE_ACTIVE || ch->pcdata->garage_status[i] == GARAGE_GARAGED) {
        if (ch->pcdata->garage_typeone[i] != CAR_HORSE)
        return TRUE;
      }
    }
    return FALSE;
  }
  bool has_world_vehicle(CHAR_DATA *ch) {
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_status[i] == GARAGE_ACTIVE || ch->pcdata->garage_status[i] == GARAGE_GARAGED) {
        if (ch->pcdata->garage_typeone[i] == CAR_HORSE)
        return TRUE;
      }
    }
    return FALSE;
  }

  _DOFUN(do_newdrive) {
    char arg1[MSL];
    char buf[MSL];
    one_argument_nouncap(argument, arg1);
    ROOM_INDEX_DATA *room;
    bool stock = FALSE;
    int destivnum = -1;
    int i;
    int desti = 6730;
    int dist;
    int cab;

    if (!str_cmp(arg1, "slow")) {
      if (ch->pcdata->travel_time > 0 && (ch->pcdata->travel_type == TRAVEL_BPASSENGER || ch->pcdata->travel_type == TRAVEL_CPASSENGER))
      return;

      if (IS_FLAG(ch->comm, COMM_SLOW)) {
        REMOVE_FLAG(ch->comm, COMM_SLOW);
        send_to_char("You stop driving slowly.\n\r", ch);
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          CHAR_DATA *vch;
          DESCRIPTOR_DATA *d = *it;
          vch = CH(d);
          if (vch == NULL)
          continue;
          if (vch->in_room != ch->in_room) {
            continue;
          }
          if (ch == vch)
          continue;
          if (!IS_FLAG(vch->comm, COMM_SLOW))
          continue;
          REMOVE_FLAG(vch->comm, COMM_SLOW);
        }

      }
      else {
        SET_FLAG(ch->comm, COMM_SLOW);
        ch->pcdata->travel_slow = 3;
        send_to_char("You start driving slowly.\n\r", ch);
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          CHAR_DATA *vch;
          DESCRIPTOR_DATA *d = *it;
          vch = CH(d);
          if (vch == NULL)
          continue;
          if (vch->in_room != ch->in_room) {
            continue;
          }
          if (ch == vch)
          continue;
          vch->pcdata->travel_slow = 3;
          if (IS_FLAG(vch->comm, COMM_SLOW))
          continue;
          SET_FLAG(vch->comm, COMM_SLOW);
        }
      }
      return;
    }
    if (ch->in_room == NULL || ch->in_room->vnum < 200)
    return;

    if (!str_cmp(arg1, "remember")) {
      argument = one_argument_nouncap(argument, arg1);
      remember_location(ch, argument);
      return;
    }

    if (!str_cmp(arg1, "around")) {
      if (ch->pcdata->driving_around == TRUE) {
        ch->pcdata->driving_around = FALSE;
        send_to_char("You stop driving around.\n\r", ch);
        return;
      }
      else {
        ch->pcdata->driving_around = TRUE;
        send_to_char("You start driving around.\n\r", ch);
        return;
      }
    }

    if (is_ghost(ch)) {
      send_to_char("Your feet know not weariness and no vehicle would carry you.\n\r", ch);
      return;
    }

    if (in_fight(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (is_pinned(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (is_helpless(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (IS_FLAG(ch->act, PLR_SHROUD)) {
      send_to_char("Leave the nightmare first.\n\r", ch);
      return;
    }
    if (shipment_carrier(ch)) {
      send_to_char("You can't fit the shipment into your vehicle.\n\r", ch);
      return;
    }
    if (ch->shape != SHAPE_HUMAN) {
      send_to_char("Opposable thumbs would help.\n\r", ch);
      return;
    }

    if (!has_active_vehicle(ch)) {
      send_to_char("You don't have a vehicle.\n\r", ch);
      return;
    }
    if (clinic_patient(ch))
    return;

    if (IS_FLAG(ch->act, PLR_HIDE))
    REMOVE_FLAG(ch->act, PLR_HIDE);

    if (!str_cmp(arg1, "list")) {
      send_to_char("You can drive to the following locations. See (`chelp landmarks`x) for more.\n\r", ch);
      for (i = 0; i < MAX_TAXIS; i++) {
        if ((room = get_room_index(taxi_table[i].vnum)) != NULL)
        printf_to_char(ch, "`W[`c%d`W]`x %s\n\r", i + 1, room->name);
      }
      send_to_char("\n", ch);
      for (i = 0; i < 10; i++) {
        if (ch->pcdata->driveloc[i] > 0 && (room = get_room_index(ch->pcdata->driveloc[i])) != NULL) {
          printf_to_char(ch, "`W[`c%s`W]`x %s\n\r", ch->pcdata->drivenames[i], room->name);
        }
      }
      return;
    }
    bool found = FALSE;
    for (i = 0; i < MAX_TAXIS; i++) {
      if (ch->in_room->vnum == taxi_table[i].vnum) {
        found = TRUE;
      }
    }
    if (vehicle_location(ch) != 0) {
      if (get_room_index(vehicle_location(ch)) == NULL || (get_room_index(vehicle_location(ch))->sector_type != SECT_STREET && get_room_index(vehicle_location(ch))->sector_type != SECT_ALLEY))
      set_vehicle_location(ch, 0);
    }
    if (found == FALSE && vehicle_location(ch) == 0 && vehicle_typeone(ch) != CAR_HORSE) {
      send_to_char("You must be at an intersection.\n\r", ch);
      return;
    }
    if (ch->in_room->vnum != vehicle_location(ch) && vehicle_location(ch) != 0) {
      send_to_char("Your vehicle isn't here.\n\r", ch);
      return;
    }
    if (crisis_nodrive == 1 && vehicle_typeone(ch) != CAR_HORSE) {
      send_to_char("Your vehicle won't start.\n\r", ch);
      return;
    }

    ch->pcdata->driving_around = FALSE;
    if (!is_number(arg1)) {
      for (i = 0; i < MAX_TAXIS; i++) {
        if ((room = get_room_index(taxi_table[i].vnum)) != NULL) {
          if (!str_cmp(argument, room->name)) {
            desti = i + 1;
          }
        }
      }
      int dnumber = landmark_vnum(argument, ch);
      if (dnumber > 0) {
        room = get_room_index(dnumber);
        stock = TRUE;
      }
      for (i = 0; i < 10; i++) {
        if (!str_cmp(ch->pcdata->drivenames[i], arg1))
        desti = MAX_TAXIS + 1 + i;
      }
    }
    else if (landmark_vnum(argument, ch) > 0) {
      room = get_room_index(landmark_vnum(argument, ch));
      stock = TRUE;
    }
    else if (atoi(arg1) <= MAX_TAXIS)
    desti = atoi(arg1);

    if (stock == FALSE) {
      if ((desti < 1 || desti > MAX_TAXIS + 10)) {
        printf_to_char(ch, "Syntax: drive 1 - %d\n\r", MAX_TAXIS);
        return;
      }
      if (desti <= MAX_TAXIS)
      destivnum = taxi_table[desti - 1].vnum;
      else if (desti <= MAX_TAXIS + 10) {
        destivnum = ch->pcdata->driveloc[desti - MAX_TAXIS - 1];
        if (!valid_parking_spot(get_room_index(destivnum))) {
          send_to_char("There doesn't seem to be any parking in that area.\n\r", ch);
          return;
        }
      }
    }
    else {
      destivnum = room->vnum;
      if (!valid_parking_spot(get_room_index(destivnum))) {
        //            send_to_char("There doesn't seem to be any parking in that
        //            area.\n\r", ch); return;
      }
    }

    if (get_room_index(destivnum) == NULL)
    return;

    ROOM_INDEX_DATA *driveroom = get_room_index(destivnum);
    if (!has_town_vehicle(ch) && mist_level(driveroom) < 3 && driveroom->area->vnum == 13) {
      send_to_char("You can't ride your horse into town.\n\r", ch);
      return;
    }
    if (!has_world_vehicle(ch) && (driveroom->area->world == WORLD_OTHER || driveroom->area->world == WORLD_HELL || driveroom->area->world == WORLD_WILDS || driveroom->area->world == WORLD_GODREALM)) {
      send_to_char("Your vehicle can't cross worlds.\n\r", ch);
      return;
    }
    if (event_cleanse == 1 && (driveroom->area->world == WORLD_OTHER || driveroom->area->world == WORLD_HELL || driveroom->area->world == WORLD_WILDS || driveroom->area->world == WORLD_GODREALM)) {
      send_to_char("You can't cross worlds.\n\r", ch);
      return;
    }

    if(world_blocked(ch, driveroom->area->world)) {
      send_to_char("You can't go there.\n\r", ch);
      return;
    }

    if (driveroom->area->world == WORLD_EARTH && IS_FLAG(ch->act, PLR_STASIS))
    return;
    if (!can_world_travel(ch, ch->in_room, driveroom)) {
      send_to_char("You don't know how to get there.\n\r", ch);
      return;
    }

    if (is_helpless(ch) || is_ghost(ch) || in_fight(ch) || room_hostile(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch) && ch->pcdata->process_timer > 0) {
      ch->pcdata->process_timer = 0;
      send_to_char("You stop what you were doing.\n\r", ch);
    }

    ch->facing = DIR_UP;

    desti--;
    dist = street_distance(ch->in_room, get_room_index(destivnum), ch);
    dist = UMAX(dist, 2);

    unplace_car(ch);

    for (cab = INIT_CABS; cab < END_CABS && !room_empty(get_room_index(cab));
    cab++) {
    }

    ROOM_INDEX_DATA *travelroom = get_room_index(cab);

    free_string(travelroom->name);
    if (safe_strlen(vehicle_name(ch)) > 2)
    travelroom->name = str_dup(vehicle_name(ch));
    else
    travelroom->name = str_dup("Inside a Car");

    sprintf(buf, "%s %s\n%s\n\t`W|`R%s`W|`x\n", carqualityname(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch)), cartypename(vehicle_typeone(ch), vehicle_typetwo(ch)), vehicle_desc(ch), vehicle_lplate(ch));

    free_string(travelroom->description);
    travelroom->description = str_dup(buf);

    free_string(travelroom->shroud);
    travelroom->shroud = str_dup("");
    if (vehicle_typeone(ch) == CAR_SPORTSBIKE || vehicle_typeone(ch) == CAR_CRUISER || vehicle_typeone(ch) == CAR_HORSE) {
      if (IS_SET(travelroom->room_flags, ROOM_INDOORS))
      REMOVE_BIT(travelroom->room_flags, ROOM_INDOORS);
    }
    else {
      if (!IS_SET(travelroom->room_flags, ROOM_INDOORS))
      SET_BIT(travelroom->room_flags, ROOM_INDOORS);
    }
    int passengers = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end();) {
      CHAR_DATA *vch;
      vch = *it;
      ++it;

      if (vch == NULL)
      continue;

      if (vch->in_room == NULL)
      continue;

      if (IS_NPC(vch))
      continue;

      if (vch->in_room != ch->in_room)
      continue;

      if (ch == vch)
      continue;

      if (clinic_patient(vch))
      continue;

      if (!is_name(PERS(vch, ch), argument) && !is_ghost(vch))
      continue;

      if (vch->master != ch && !is_ghost(vch) && !is_helpless(vch) && (!is_gm(ch) || !IS_FLAG(ch->comm, COMM_RUNNING)))
      continue;
      if (is_ghost(vch) && vch->possessing != ch)
      continue;

      if (passengers > max_passengers(ch))
      continue;

      char_from_room(vch);
      char_to_room(vch, get_room_index(cab));
      if (!is_ghost(vch))
      passengers++;

      if (IS_FLAG(vch->comm, COMM_SLOW) != IS_FLAG(ch->comm, COMM_SLOW)) {
        if (IS_FLAG(ch->comm, COMM_SLOW))
        SET_FLAG(vch->comm, COMM_SLOW);
        else
        REMOVE_FLAG(vch->comm, COMM_SLOW);
      }
      if (IS_FLAG(vch->comm, COMM_STALL))
      REMOVE_FLAG(vch->comm, COMM_STALL);

      if (vehicle_typeone(ch) == CAR_HORSE) {
        if (safe_strlen(vehicle_name(ch)) > 2)
        printf_to_char(vch, "You get onto %s's %s and head towards %s.\n\r", PERS(ch, vch), vehicle_name(ch), get_room_index(destivnum)->name);
        else
        printf_to_char(vch, "You get onto %s's horse and head towards %s.\n\r", PERS(ch, vch), get_room_index(destivnum)->name);
        vch->pcdata->travel_time = dist;
        vch->pcdata->travel_type = TRAVEL_HPASSENGER;
      }
      else if (vehicle_typeone(ch) == CAR_SPORTSBIKE || vehicle_typeone(ch) == CAR_CRUISER) {
        if (safe_strlen(vehicle_name(ch)) > 2)
        printf_to_char(vch, "You get onto %s's %s and head towards %s.\n\r", PERS(ch, vch), vehicle_name(ch), get_room_index(destivnum)->name);
        else
        printf_to_char(vch, "You get onto %s's motorcycle and head towards %s.\n\r", PERS(ch, vch), get_room_index(destivnum)->name);
        vch->pcdata->travel_time = dist;
        vch->pcdata->travel_type = TRAVEL_BPASSENGER;
      }
      else {
        if (safe_strlen(vehicle_name(ch)) > 2)
        printf_to_char(vch, "You get into %s's %s and head towards %s.\n\r", PERS(ch, vch), vehicle_name(ch), get_room_index(destivnum)->name);
        else
        printf_to_char(vch, "You get into %s's car and head towards %s.\n\r", PERS(ch, vch), get_room_index(destivnum)->name);
        vch->pcdata->travel_time = dist;
        vch->pcdata->travel_type = TRAVEL_CPASSENGER;
      }
      vch->pcdata->travel_to = destivnum;
      vch->pcdata->travel_from = ch->in_room->vnum;
    }

    if (vehicle_typeone(ch) == CAR_HORSE) {
      if (safe_strlen(vehicle_name(ch)) > 2)
      printf_to_char(ch, "You get onto your %s and head towards %s.\n\r", vehicle_name(ch), get_room_index(destivnum)->name);
      else
      printf_to_char(ch, "You get onto your horse and head towards %s.\n\r", get_room_index(destivnum)->name);
      if (safe_strlen(vehicle_name(ch)) > 2)
      sprintf(buf, "$n gets onto $s %s heading towards %s.", vehicle_name(ch), get_room_index(destivnum)->name);
      else
      sprintf(buf, "$n gets onto $s horse heading towards %s.", get_room_index(destivnum)->name);
    }
    else if (vehicle_typeone(ch) == CAR_SPORTSBIKE || vehicle_typeone(ch) == CAR_CRUISER) {
      if (safe_strlen(vehicle_name(ch)) > 2)
      printf_to_char(ch, "You get onto your %s and head towards %s.\n\r", vehicle_name(ch), get_room_index(destivnum)->name);
      else
      printf_to_char(ch, "You get onto your motorcycle and head towards %s.\n\r", get_room_index(destivnum)->name);
      if (safe_strlen(vehicle_name(ch)) > 2)
      sprintf(buf, "$n gets onto $s %s heading towards %s.", vehicle_name(ch), get_room_index(destivnum)->name);
      else
      sprintf(buf, "$n gets onto $s motorcycle heading towards %s.", get_room_index(destivnum)->name);
    }
    else {
      if (safe_strlen(vehicle_name(ch)) > 2)
      printf_to_char(ch, "You get into your %s and head towards %s.\n\r", vehicle_name(ch), get_room_index(destivnum)->name);
      else
      printf_to_char(ch, "You get into your car and head towards %s.\n\r", get_room_index(destivnum)->name);
      if (safe_strlen(vehicle_name(ch)) > 2)
      sprintf(buf, "$n gets into $s %s heading towards %s.", vehicle_name(ch), get_room_index(destivnum)->name);
      else
      sprintf(buf, "$n gets into $s car heading towards %s.", get_room_index(destivnum)->name);
    }

    if (IS_FLAG(ch->comm, COMM_STALL))
    REMOVE_FLAG(ch->comm, COMM_STALL);

    makevehicle(ch);

    act(buf, ch, NULL, NULL, TO_ROOM);
    dact(buf, ch, NULL, NULL, DISTANCE_MEDIUM);
    ch->pcdata->travel_from = ch->in_room->vnum;
    char_from_room(ch);
    char_to_room(ch, get_room_index(cab));
    ch->pcdata->travel_time = dist;
    if (vehicle_typeone(ch) == CAR_HORSE)
    ch->pcdata->travel_type = TRAVEL_HORSE;
    else if (vehicle_typeone(ch) == CAR_SPORTSBIKE || vehicle_typeone(ch) == CAR_CRUISER)
    ch->pcdata->travel_type = TRAVEL_BIKE;
    else
    ch->pcdata->travel_type = TRAVEL_CAR;
    ch->pcdata->travel_to = destivnum;
    ch->pcdata->last_drove = ch->pcdata->travel_to;
    if (IS_AFFECTED(ch, AFF_LURED) && get_room_index(ch->pcdata->lured_room) != NULL) {
      int mindist = 1000;
      int minpointer = 0;
      for (int i = 0; i < MAX_TAXIS; i++) {
        if (get_dist(get_roomx(get_room_index(ch->pcdata->lured_room)), get_roomy(get_room_index(ch->pcdata->lured_room)), get_roomx(get_room_index(taxi_table[i].vnum)), get_roomy(get_room_index(taxi_table[i].vnum))) < mindist) {
          minpointer = i;
          mindist = get_dist(get_roomx(get_room_index(ch->pcdata->lured_room)), get_roomy(get_room_index(ch->pcdata->lured_room)), get_roomx(get_room_index(taxi_table[i].vnum)), get_roomy(get_room_index(taxi_table[i].vnum)));
        }
      }
      ch->pcdata->travel_to = taxi_table[minpointer].vnum;
      ch->pcdata->last_drove = ch->pcdata->travel_to;
      dist =
      street_distance(ch->in_room, get_room_index(ch->pcdata->travel_to), ch);
      dist = UMAX(dist, 2);
      ch->pcdata->travel_time = dist;
    }
  }

  _DOFUN(do_drive) {
    do_function(ch, &do_newdrive, argument);
    return;
  }

  _DOFUN(do_ride) {
    do_function(ch, &do_newdrive, argument);
    return;
  }

  _DOFUN(do_car) {}

  _DOFUN(do_bike) {}

  _DOFUN(do_speed) {
    if (ch->pcdata->ci_editing == 12) {
      ch->pcdata->ci_absorb = 1;
      int num = atoi(argument);
      if (num < 1 || num > 10) {
        send_to_char("Syntax: Speed (1-10)\n\r", ch);
        return;
      }
      if (ch->pcdata->ci_layer > max_operation_size(num))
      ch->pcdata->ci_layer = max_operation_size(num);
      ch->pcdata->ci_zips = num;
      send_to_char("Done.\n\r", ch);
      return;
    }
    int val = atoi(argument);
    if (val < 1 || val > 5) {
      send_to_char("Syntax: Speed 1-5\n\r", ch);
      return;
    }
    if (in_fight(ch) || battleground(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    ch->pcdata->default_speed = val;
    printf_to_char(ch, "Your default combat speed is now %d\n\r", ch->pcdata->default_speed);
  }

  void fread_playerroom(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    PLAYERROOM_TYPE *playerroom;

    playerroom = new_playerroom();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Author", playerroom->author, fread_string(fp));
        break;
      case 'C':
        KEY("Cooldown", playerroom->cooldown, fread_number(fp));
        break;
      case 'E':

        if (!str_cmp(word, "End")) {
          if (!playerroom->author) {
            bug("Fread_Playerroom: Name not found.", 0);
            free_playerroom(playerroom);
            return;
          }
          PlayerroomVect.push_back(playerroom);
          return;
        }
        break;
      case 'S':
        KEY("Status", playerroom->status, fread_number(fp));
        break;
      case 'V':
        KEY("Vnum", playerroom->vnum, fread_number(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_playerroom: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_playerrooms() {
    nullplayerroom = new_playerroom();
    FILE *fp;

    if ((fp = fopen(PLAYERROOM_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_Playerroomss: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PLAYERROOM")) {
          fread_playerroom(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_Playerrooms: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open playerrooms.txt", 0);
      exit(0);
    }
  }

  void save_playerrooms_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/playerrooms.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/playerrooms.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/playerrooms.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/playerrooms.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/playerrooms.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/playerrooms.txt");
    else
    sprintf(buf, "../data/back7/playerrooms.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open playerrooms.txt for writing", 0);
      return;
    }

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_playerrooms: Blank room in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      fprintf(fpout, "#PLAYERROOM\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Cooldown %d\n", (*it)->cooldown);
      fprintf(fpout, "Vnum %d\n", (*it)->vnum);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_playerrooms() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(PLAYERROOM_FILE, "w")) == NULL) {
      bug("Cannot open playerrooms.txt for writing", 0);
      return;
    }

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        bug("Save_playerrooms: Blank room in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      fprintf(fpout, "#PLAYERROOM\n");
      fprintf(fpout, "Author %s~\n", (*it)->author);
      fprintf(fpout, "Cooldown %d\n", (*it)->cooldown);
      fprintf(fpout, "Vnum %d\n", (*it)->vnum);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    save_playerrooms_backup();
  }

  void cleaning_update() {
    EXTRA_DESCR_DATA *ed;
    ROOM_INDEX_DATA *room;
    EXTRA_DESCR_DATA *ped = NULL;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if ((*it)->cooldown > 2)
      continue;

      if ((*it)->cooldown < 1)
      continue;

      if ((room = get_room_index((*it)->vnum)) == NULL)
      continue;
      /*
CHAR_DATA *to = get_char_world_pc("Raider");
printf_to_char(to, "Valid: %d, Cooldown: %d, vnum: %d, Name: %s", (*it)->valid, (*it)->cooldown, room->vnum, room->name);
*/
      for (int door = 0; door < 9; door++) {
        ROOM_INDEX_DATA *pToRoom;
        int rev = rev_dir[door];
        if (room->exit[door] == NULL)
        continue;

        pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */

        if (pToRoom != NULL) {
          if (pToRoom->exit[rev]) {
            free_exit(pToRoom->exit[rev]);
            pToRoom->exit[rev] = NULL;
          }
        }

        free_exit(room->exit[door]);
        room->exit[door] = NULL;
      }

      for (ed = room->extra_descr; ed; ed = ed->next) {
        if (ed->next != NULL)
        ped = ed;

        if (ed) {
          if (!ped)
          room->extra_descr = ed->next;
          else
          ped->next = ed->next;

          free_extra_descr(ed);
        }
      }
      for (ed = room->places; ed; ed = ed->next) {
        if (ed->next != NULL)
        ped = ed;

        if (ed) {
          if (!ped)
          room->places = ed->next;
          else
          ped->next = ed->next;

          free_extra_descr(ed);
        }
      }

      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
  }

  bool public_link(PLAYERROOM_TYPE *pproom, bool primary) {
    pproom->checked = TRUE;
    ROOM_INDEX_DATA *room = get_room_index(pproom->vnum);
    if (room == NULL)
    return FALSE;

    if (room->area->vnum == 12)
    return TRUE;

    if (pproom->status == ROOM_PUBLIC)
    return TRUE;

    if (primary == TRUE) {
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        (*it)->checked = FALSE;
      }
      pproom->checked = TRUE;
    }

    for (int door = 0; door <= 9; door++) {
      EXIT_DATA *pexit;
      if ((pexit = room->exit[door]) != NULL && pexit->u1.to_room != NULL) {
        if (pexit->wall == WALL_NONE || pexit->wallcondition != WALLCOND_HOLE) {
          for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
          it != PlayerroomVect.end(); ++it) {
            if (!(*it)->author || (*it)->author[0] == '\0') {
              continue;
            }
            if ((*it)->checked == TRUE)
            continue;

            if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
            continue;
            if ((*it)->vnum == pexit->u1.to_room->vnum) {
              if (public_link((*it), FALSE))
              return TRUE;
            }
          }
        }
      }
    }

    return FALSE;
  }

  void playerroom_update() {
    cleaning_update();

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if (!str_cmp((*it)->author, "Ouroboros"))
      continue;

      if (!str_cmp((*it)->author, "Tyr"))
      continue;

      if (get_char_world_new((*it)->author) != NULL && !IS_NPC(get_char_world_new((*it)->author))) {
        (*it)->cooldown = UMIN(20000, (*it)->cooldown + 25);
      }
      else {
        (*it)->cooldown -= 1;
        if ((*it)->cooldown <= 0)
        (*it)->valid = FALSE;
      }

      if (!public_link((*it), TRUE))
      (*it)->status = ROOM_PUBLIC;
    }
  }

  bool freeplayerroom(int vnum) {
    ROOM_INDEX_DATA *room;
    room = get_room_index(vnum);
    room = get_room_index(vnum);
    if (room == NULL) {
      create_room(vnum);
      room = get_room_index(vnum);
      if (room == NULL)
      return FALSE;
    }

    if (IS_SET(room->room_flags, ROOM_PERM))
    return FALSE;

    if (!room_empty(room))
    return FALSE;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if ((*it)->vnum == vnum)
      return FALSE;
    }
    if (room->x == 0 && room->y == 0) {
      room->x = -10000;
      room->y = -10000;
      room->z = -10000;
    }
    return TRUE;
  }

  int houserooms(CHAR_DATA *ch) {
    int count = 0;
    ROOM_INDEX_DATA *room;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      if (str_cmp((*it)->author, ch->name))
      continue;

      room = get_room_index((*it)->vnum);
      if (room->sector_type == SECT_HOUSE)
      count += 1;
    }
    return count;
  }

  int wealth_req(ROOM_INDEX_DATA *room) { return 0; }

  _DOFUN(do_location) {
    int j, i;
    if (ch->pcdata->ci_editing != 2) {
      send_to_char("You're not editing a room.\n\r", ch);
      return;
    }
    ch->pcdata->ci_absorb = 1;
    if (!str_cmp(argument, "adjacent")) {
      ch->pcdata->ci_area = ch->in_room->area->vnum;
      send_to_char("Done.\n\r", ch);
    }
    else {
      int area = 0;
      for (i = 0; i <= MAX_DIST; i++) {
        if (!str_cmp(argument, district_table[i].name) || !str_cmp(argument, district_table[i].nickname)) {
          area = district_table[i].area;
        }
      }
      if (area == 0) {

        send_to_char("Valid districts are: Boston, America, Mexico, Canada, SouthAmerica, Europe, Africa, Asia, Australia.\n\r", ch);
        return;
      }
      for (j = get_area_data(area)->min_vnum; !freeplayerroom(j); j++) {
      }
      if (j > get_area_data(area)->max_vnum) {
        send_to_char("No space.\n\r", ch);
        return;
      }
      else {
        ch->pcdata->ci_area = area;
        send_to_char("Done.\n\r", ch);
      }
    }
  }
  _DOFUN(do_year) {

    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    int year = ptm->tm_year + 1900;

    if (ch->pcdata->ci_editing != 2) {
      send_to_char("You're not editing a room.\n\r", ch);
      return;
    }
    ch->pcdata->ci_absorb = 1;

    if (!timetraveler(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }

    if (atoi(argument) > year) {
      send_to_char("You can't travel into the future.\n\r", ch);
      return;
    }

    ch->pcdata->ci_cost = atoi(argument);
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_quadrant) {
    if (ch->pcdata->ci_editing != 2) {
      send_to_char("You're not editing a room.\n\r", ch);
      return;
    }
    ch->pcdata->ci_absorb = 1;
    if (ch->pcdata->ci_status != 0 && ch->pcdata->ci_status != ROOM_PUBLIC) {
      send_to_char("This command is for setting public room quadrants, for private rooms use the exit command.\n\r", ch);
      return;
    }
    if (!str_cmp(argument, "ne")) {
      ch->pcdata->ci_x = 65 + (number_percent() % 21);
      ch->pcdata->ci_y = 65 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "nw")) {
      ch->pcdata->ci_x = 15 + (number_percent() % 21);
      ch->pcdata->ci_y = 65 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "sw")) {
      ch->pcdata->ci_x = 15 + (number_percent() % 21);
      ch->pcdata->ci_y = 15 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "se")) {
      ch->pcdata->ci_x = 65 + (number_percent() % 21);
      ch->pcdata->ci_y = 15 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "n")) {
      ch->pcdata->ci_x = 40 + (number_percent() % 21);
      ch->pcdata->ci_y = 65 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "s")) {
      ch->pcdata->ci_x = 40 + (number_percent() % 21);
      ch->pcdata->ci_y = 15 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "e")) {
      ch->pcdata->ci_x = 65 + (number_percent() % 21);
      ch->pcdata->ci_y = 40 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "w")) {
      ch->pcdata->ci_x = 15 + (number_percent() % 21);
      ch->pcdata->ci_y = 40 + (number_percent() % 21);
    }
    else if (!str_cmp(argument, "c")) {
      ch->pcdata->ci_x = 40 + (number_percent() % 21);
      ch->pcdata->ci_y = 40 + (number_percent() % 21);
    }
    else {
      send_to_char("Valid sections are: n, s, e, w, nw, ne, sw, se, c.\n\r", ch);
      return;
    }
    send_to_char("Done.\n\r", ch);
  }
  _DOFUN(do_status) {
    if (ch->pcdata->ci_editing != 2) {
      send_to_char("You're not editing a room.\n\r", ch);
      return;
    }
    ch->pcdata->ci_absorb = 1;

    if (!str_cmp(argument, "public")) {
      ch->pcdata->ci_status = ROOM_PUBLIC;
    }
    else if (!str_cmp(argument, "private")) {
      ch->pcdata->ci_status = ROOM_PRIVATE;
    }
    else if (!str_cmp(argument, "locked")) {
      ch->pcdata->ci_status = ROOM_LOCKED;
    }
    else if (!str_cmp(argument, "invisible") && is_gm(ch)) {
      ch->pcdata->ci_status = ROOM_INVISIBLE;
    }
    else {
      send_to_char("Syntax: Status public/private/locked/invisible/fae\n\r", ch);
      return;
    }
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_makeroom) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg4[MSL];
    char buf[MSL];
    EXTRA_DESCR_DATA *ed;
    EXTRA_DESCR_DATA *ped;
    PLAYERROOM_TYPE *playerroom;
    int i, j, cab;
    ROOM_INDEX_DATA *room;
    argument = one_argument_nouncap(argument, arg1);
    OBJ_DATA *obj;

    if (IS_SET(ch->in_room->room_flags, ROOM_KITCHEN) && !str_cmp(arg1, "food")) {
      ch->money -= 100;
      obj = create_object(get_obj_index(45250), 0);
      obj_to_char(obj, ch);
      send_to_char("You gain a loaf of bread.\n\r", ch);
      return;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_KITCHEN) && !str_cmp(arg1, "drink")) {
      ch->money -= 100;
      obj = create_object(get_obj_index(45251), 0);
      obj_to_char(obj, ch);
      send_to_char("You gain a glass of water.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "attack")) {
      do_function(ch, &do_custom, "create");
      return;
    }
    if (!str_cmp(arg1, "monster") && is_gm(ch)) {
      ch->pcdata->ci_editing = 4;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "blessing") && higher_power(ch)) {
      ch->pcdata->ci_editing = 17;
      ch->pcdata->ci_disclevel = 0;
      ch->pcdata->ci_zips = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "encounter")) {
      ch->pcdata->ci_discipline = number_range(5, 45);
      ch->pcdata->ci_editing = 18;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "patrol")) {
      ch->pcdata->ci_editing = 19;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "decree")) {
      ch->pcdata->ci_editing = 21;
      ch->pcdata->ci_discipline = 0;
      ch->pcdata->ci_disclevel = 0;
      send_to_char("Done.\n\r", ch);
      return;
    }



    if (!str_cmp(arg1, "destiny")) {
      for (int i = 0; i < 20; i++) {
        ch->pcdata->ci_include_type[i] = 0;
        ch->pcdata->ci_exclude_type[i] = 0;
        free_string(ch->pcdata->ci_includes[i]);
        ch->pcdata->ci_includes[i] = str_dup("");
        free_string(ch->pcdata->ci_excludes[i]);
        ch->pcdata->ci_excludes[i] = str_dup("-1 0 0 0");
      }
      ch->pcdata->ci_disclevel = 0;
      ch->pcdata->ci_zips = 0;
      ch->pcdata->ci_alcohol = 0;
      ch->pcdata->ci_discipline = 0;
      ch->pcdata->ci_vnum = 0;
      ch->pcdata->ci_editing = 16;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "minion")) {
      ch->pcdata->ci_editing = 5;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "ally")) {
      ch->pcdata->ci_editing = 6;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "news") && is_gm(ch)) {
      if (!has_plot(ch) && !crisis_runner(ch)) {
        send_to_char("You aren't currently running any plots.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 7;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "townnews") && IS_IMMORTAL(ch)) {
      ch->pcdata->ci_editing = 7;
      ch->pcdata->ci_stats[0] = -4;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "supernews") && IS_IMMORTAL(ch)) {
      ch->pcdata->ci_editing = 7;
      ch->pcdata->ci_stats[0] = -5;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "research")) {
      if (!is_gm(ch)) {
        if (in_fight(ch)) {
          send_to_char("You're a bit busy.\n\r", ch);
          return;
        }
        if (is_helpless(ch)) {
          send_to_char("You can't do that.\n\r", ch);
          return;
        }
        if (!library_room(ch->in_room)) {
          send_to_char("You need to be in a library to do that.\n\r", ch);
          return;
        }
        if (ch->played / 3600 < 50) {
          send_to_char("Wait until you've played a little longer first.\n\r", ch);
          return;
        }
      }

      ch->pcdata->ci_editing = 8;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "petition")) {
      ch->pcdata->ci_editing = 9;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "grave")) {
      if (!gravesite(ch->in_room)) {
        send_to_char("You need to be in a viable cemetary room first.\n\r", ch);
        return;
      }
      if (ch->played / 3600 < 100) {
        send_to_char("You're still too new to the game for that.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 10;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "mission")) {
      return;
      if (!has_trust(ch, TRUST_WAR, ch->faction)) {
        send_to_char("This command is for faction leaders.\n\r", ch);
        return;
      }
      if (clan_lookup(ch->faction)->resource < 10000) {
        send_to_char("Your faction needs to be at at least 100% resources first.\n\r", ch);
        return;
      }
      ch->pcdata->ci_editing = 11;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "operation")) {
      if (ch->faction == 0) {
        send_to_char("You need to be in a faction first.\n\r", ch);
        return;
      }
      if (clan_lookup(ch->faction)->weekly_ops >= 10) {
        send_to_char("Your faction's pathing facility is depleted.\n\r", ch);
        return;
      }
      ch->pcdata->ci_zips = 1;
      ch->pcdata->ci_covers = 5;
      ch->pcdata->ci_alcohol = 17;
      ch->pcdata->ci_vnum = 1;
      ch->pcdata->ci_layer = 1000;
      ch->pcdata->ci_editing = 12;
      send_to_char("Done.\n\r", ch);
      return;
    }
    if (!str_cmp(arg1, "camp")) {
      if (ch->in_room->sector_type != SECT_FOREST && ch->in_room->sector_type != SECT_PARK && ch->in_room->sector_type != SECT_BEACH) {
        send_to_char("You have to be in the forest or on the beach to do that.\n\r", ch);
        return;
      }
      if (room_in_school(ch->in_room->vnum)) {
        send_to_char("The groundskeeper wouldn't take kindly to that.\n\r", ch);
        return;
      }
      if (ch->money < 5000) {
        send_to_char("You need $50 to do that.\n\r", ch);
        return;
      }
      if (IS_SET(ch->in_room->room_flags, ROOM_CAMPSITE)) {
        send_to_char("There's already a campsite here.\n\r", ch);
        ;
        return;
      }
      SET_BIT(ch->in_room->room_flags, ROOM_CAMPSITE);
      ch->money -= 5000;
      act("You errect a campsite.", ch, NULL, NULL, TO_CHAR);
      act("$n errects a campsite.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    if (!str_cmp(arg1, "home")) {
      if (ch->in_room->sector_type != SECT_FOREST && ch->in_room->sector_type != SECT_PARK && ch->in_room->sector_type != SECT_BEACH) {
        send_to_char("You have to be in the forest or on the beach to do that.\n\r", ch);
        return;
      }
      if (!is_animal(ch)) {
        send_to_char("You need to be an animal to do that.\n\r", ch);
        return;
      }
      if (IS_SET(ch->in_room->room_flags, ROOM_ANIMALHOME)) {
        send_to_char("There's already an animal home here.\n\r", ch);
        ;
        return;
      }
      SET_BIT(ch->in_room->room_flags, ROOM_ANIMALHOME);
      act("You create a home for yourself.", ch, NULL, NULL, TO_CHAR);
      act("$n creates a home for $mself.", ch, NULL, NULL, TO_ROOM);
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      return;
    }

    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);
    int year = ptm->tm_year + 1900;

    ch->pcdata->ci_editing = 2;
    ch->pcdata->ci_status = ROOM_PRIVATE;
    ch->pcdata->ci_cost = year;

    send_to_char("Done.\n\r", ch);
    return;

    if (!str_cmp(arg1, "adjacent")) {
      for (j = ch->in_room->area->min_vnum; !freeplayerroom(j); j++) {
      }
      if (j > ch->in_room->area->max_vnum) {
        send_to_char("No space.\n\r", ch);
        return;
      }
      room = get_room_index(j);
      room->locx = ch->in_room->locx += number_range(-1, 1);
      room->locy = ch->in_room->locy += number_range(-1, 1);
    }
    else {
      int area = 0;
      for (i = 0; i < MAX_DIST; i++) {
        if (!str_cmp(arg1, district_table[i].name) || !str_cmp(arg1, district_table[i].nickname)) {
          area = district_table[i].area;
        }
      }
      if (area == 0) {
        send_to_char("Valid districts are: EastBronx, ebx, WestBronx, wbx, SouthBronx, sbx, NortheasternBrooklyn, nebk, CentralBrooklyn, cbk, SouthwesternBrooklyn, swbk, LowerManhattan, lmn, MidtownManhattan, mtmn, UpperEastSideManhattan, uemn, UpperWestSideManhattan, uwmn, CentralPark, cpmn, NorthernManhattan, nmn, NortheasternQueens, neqn, SouthwesternQueens, swqn, SoutheasternQueens, seqn, NorthwesternQueens, nwqn, NortheasternStatenIsland, nesi, SouthwesternStatenIsland, swsi.\n\r", ch);
        return;
      }
      for (j = get_area_data(area)->min_vnum; !freeplayerroom(j); j++) {
      }
      if (j > get_area_data(area)->max_vnum) {
        send_to_char("No space.\n\r", ch);
        return;
      }
      if (!str_cmp(arg3, "house")) {
        send_to_char("Houses can only be made as adjacent rooms.\n\r", ch);
        return;
      }
      room = get_room_index(j);
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "ne")) {
        room->locx = 65 + (number_percent() % 21);
        room->locy = 65 + (number_percent() % 21);
      }
      else if (!str_cmp(arg2, "nw")) {
        room->locx = 15 + (number_percent() % 21);
        room->locy = 65 + (number_percent() % 21);
      }
      else if (!str_cmp(arg2, "sw")) {
        room->locx = 15 + (number_percent() % 21);
        room->locy = 15 + (number_percent() % 21);
      }
      else if (!str_cmp(arg2, "se")) {
        room->locx = 65 + (number_percent() % 21);
        room->locy = 15 + (number_percent() % 21);
      }
      else {
        send_to_char("Valid quadrants are: nw, ne, sw, se.\n\r", ch);
        return;
      }
    }
    argument = one_argument_nouncap(argument, arg3);
    if (IS_SET(room->room_flags, ROOM_INDOORS))
    REMOVE_BIT(room->room_flags, ROOM_INDOORS);

    if (!str_cmp(arg3, "house")) {
      if (wealth_req(ch->in_room) > get_skill(ch, SKILL_WEALTH)) {
        send_to_char("You're not rich enough to live in that area.\n\r", ch);
        return;
      }
      SET_BIT(room->room_flags, ROOM_INDOORS);
      room->sector_type = SECT_HOUSE;
      room->size = 35;
      room->entryx = 0;
      room->entryy = 15;
    }
    else if (!str_cmp(arg3, "club")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);
      room->sector_type = SECT_CLUB;
      room->size = 35;
      room->entryx = 0;
      room->entryy = 15;
    }
    else if (!str_cmp(arg3, "restaurant")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);
      room->sector_type = SECT_RESTERAUNT;
      room->size = 40;
      room->entryx = 0;
      room->entryy = 20;
    }
    else if (!str_cmp(arg3, "shop")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_SHOP;
      room->size = 20;
      room->entryx = 10;
      room->entryy = 0;
    }
    else if (!str_cmp(arg3, "street")) {
      room->sector_type = SECT_STREET;
      room->size = 50;
      room->entryx = 25;
      room->entryy = 0;
    }
    else if (!str_cmp(arg3, "alley")) {
      room->sector_type = SECT_ALLEY;
      room->size = 20;
      room->entryx = 10;
      room->entryy = 0;
    }
    else if (!str_cmp(arg3, "warehouse")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_WAREHOUSE;
      room->size = 80;
      room->entryx = 0;
      room->entryy = 30;
    }
    else if (!str_cmp(arg3, "commercial")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_COMMERCIAL;
      room->size = 30;
      room->entryx = 5;
      room->entryy = 0;
    }
    else if (!str_cmp(arg3, "house")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_HOUSE;
      room->size = 30;
      room->entryx = 0;
      room->entryy = 15;
    }
    else if (!str_cmp(arg3, "park")) {
      room->sector_type = SECT_PARK;
      room->size = 80;
      room->entryx = 10;
      room->entryy = 0;
    }
    else if (!str_cmp(arg3, "tunnels")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_TUNNELS;
      room->size = 20;
      room->entryx = 0;
      room->entryy = 10;
    }
    else if (!str_cmp(arg3, "cafe")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_CAFE;
      room->size = 30;
      room->entryx = 0;
      room->entryy = 20;
    }
    else if (!str_cmp(arg3, "rooftop")) {
      room->sector_type = SECT_ROOFTOP;
      room->size = 40;
      room->entryx = 20;
      room->entryy = 20;
    }
    else if (!str_cmp(arg3, "basement")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_BASEMENT;
      room->size = 40;
      room->entryx = 20;
      room->entryy = 20;
    }
    else if (!str_cmp(arg3, "hospital")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_HOSPITAL;
      room->size = 50;
      room->entryx = 10;
      room->entryy = 20;
    }
    else if (!str_cmp(arg3, "bank")) {
      SET_BIT(room->room_flags, ROOM_INDOORS);

      room->sector_type = SECT_BANK;
      room->size = 30;
      room->entryx = 15;
      room->entryy = 15;
    }
    else if (!str_cmp(arg3, "air")) {
      room->sector_type = SECT_AIR;
      room->size = 80;
      room->entryx = 40;
      room->entryy = 40;
    }
    else {
      send_to_char("Valid room types are: Club, restaurant, shop, street, alley, warehouse, commerical, house, park, tunnels, cafe, rooftop, basement, hospital, bank, air.\n\r", ch);
      return;
    }
    if (IS_SET(room->room_flags, ROOM_PUBLIC))
    REMOVE_BIT(room->room_flags, ROOM_PUBLIC);
    if (IS_SET(room->room_flags, ROOM_PRIVATE))
    REMOVE_BIT(room->room_flags, ROOM_PRIVATE);
    if (IS_SET(room->room_flags, ROOM_LOCKED))
    REMOVE_BIT(room->room_flags, ROOM_LOCKED);

    for (ed = room->extra_descr; ed; ed = ed->next) {
      ped = ed;

      if (!ed) {
      }
      else {
        if (!ped)
        room->extra_descr = ed->next;
        else
        ped->next = ed->next;

        free_extra_descr(ed);
      }
    }

    argument = one_argument_nouncap(argument, arg4);

    if (!str_cmp(arg4, "public")) {
      SET_BIT(room->room_flags, ROOM_PUBLIC);
    }
    else if (!str_cmp(arg4, "private")) {
      SET_BIT(room->room_flags, ROOM_PRIVATE);
    }
    else if (!str_cmp(arg4, "locked")) {
      SET_BIT(room->room_flags, ROOM_LOCKED);
    }
    else {
      send_to_char("Valid public types are: Public, private, locked.\n\r", ch);
      return;
    }

    free_string(room->name);
    room->name = str_dup(argument);
    free_string(room->description);
    room->description = str_dup(" ");
    string_append(ch, &room->description);
    SET_BIT(room->area->area_flags, AREA_CHANGED);
    playerroom = new_playerroom();
    free_string(playerroom->author);
    playerroom->author = str_dup(ch->name);
    playerroom->cooldown = 10000;
    playerroom->vnum = room->vnum;
    PlayerroomVect.push_back(playerroom);

    if (room->sector_type != SECT_AIR || get_skill(ch, SKILL_FLIGHT) > 2) {
      for (cab = INIT_CABS; cab < END_CABS && !room_empty(get_room_index(cab));
      cab++) {
      }

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;

        vch = CH(d);
        if (vch == NULL)
        continue;

        if (vch->in_room != ch->in_room)
        continue;

        if (ch == vch)
        continue;

        if (!is_same_group(vch, ch))
        continue;

        printf_to_char(vch, "You head down into the subway to travel towards %s.\n\r", room->name);
        vch->pcdata->travel_time = travel_time(vch, room) * 60;
        vch->pcdata->travel_type = TRAVEL_TAXI;
        vch->pcdata->travel_to = room->vnum;
        vch->pcdata->travel_from = ch->in_room->vnum;

        char_from_room(vch);
        char_to_room(vch, get_room_index(cab));
      }

      printf_to_char(ch, "You head down into the subway to travel towards %s.\n\r", room->name);
      sprintf(buf, "$n leaves.");
      act(buf, ch, NULL, NULL, TO_ROOM);
      ch->pcdata->travel_time = travel_time(ch, room) * 60;
      ch->pcdata->travel_type = TRAVEL_TAXI;
      ch->pcdata->travel_to = room->vnum;
      ch->pcdata->travel_from = ch->in_room->vnum;

      char_from_room(ch);
      char_to_room(ch, get_room_index(cab));
    }
  }

  _DOFUN(do_myrooms) {
    char arg1[MSL];
    ROOM_INDEX_DATA *room;
    char arg2[MSL];
    argument = one_argument_nouncap(argument, arg1);
    int i = 1, j;
    if (!str_cmp(arg1, "list")) {
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        printf_to_char(ch, "[%02d][%d] %s\n\r", i, get_po(room), room->name);
        i++;
      }
    }
    else if (!str_cmp(arg1, "description")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          string_append(ch, &room->description);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "nightmaredescription")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          string_append(ch, &room->shroud);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "name")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          free_string(room->name);
          room->name = str_dup(argument);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "vnum") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          int nv = atoi(argument);
          printf_to_char(ch, "Changing vnum from %d to %d.\n\r", (*it)->vnum, nv);
          (*it)->vnum = nv;
          send_to_char("Done\n\r", ch);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "timezone")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          int timez = atoi(argument);
          if (timez > 16 || timez < -16) {
            send_to_char("Select a timezone between 12 and -12 GMT.\n\r", ch);
            return;
          }
          timez += 5;
          room->timezone = timez;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "dreamentrance")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          FANTASY_TYPE *fantasy = fetch_fantasy(ch, atoi(argument));

          if (fantasy == NULL) {
            send_to_char("No such dreamworld.\n\r", ch);
            return;
          }
          if (str_cmp(fantasy->author, ch->name)) {
            send_to_char("That isn't your dreamworld to edit.\n\r", ch);
            return;
          }
          fantasy->entrance = room->vnum;
          for (int x = 0; x < 100; x++) {
            if (fantasy->rooms[x] == room->vnum) {
              send_to_char("Entrance set.\n\r", ch);
              return;
            }
          }
          for (int x = 0; x < 100; x++) {
            if (fantasy->rooms[x] == 0) {
              fantasy->rooms[x] = room->vnum;
              send_to_char("Entrance set.\n\r", ch);
              return;
            }
          }
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "dreamroom")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          FANTASY_TYPE *fantasy = fetch_fantasy(ch, atoi(argument));

          if (fantasy == NULL) {
            send_to_char("No such dreamworld.\n\r", ch);
            return;
          }
          if (str_cmp(fantasy->author, ch->name)) {
            send_to_char("That isn't your dreamworld to edit.\n\r", ch);
            return;
          }
          for (int x = 0; x < 100; x++) {
            if (fantasy->rooms[x] == room->vnum) {
              fantasy->rooms[x] = 0;
              send_to_char("Removed from dreamworld.\n\r", ch);
              return;
            }
          }
          for (int x = 0; x < 100; x++) {
            if (fantasy->rooms[x] == 0) {
              fantasy->rooms[x] = room->vnum;
              send_to_char("Added to dreamworld.\n\r", ch);
              return;
            }
          }
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "cloudcover")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          int timez = atoi(argument);
          if (timez > 100 || timez < 0) {
            send_to_char("Select a number between 0 and 100.\n\r", ch);
            return;
          }
          room->cloud_cover = timez;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "clouddensity")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          int timez = atoi(argument);
          if (timez > 100 || timez < 0) {
            send_to_char("Select a number between 0 and 100.\n\r", ch);
            return;
          }
          room->cloud_density = timez;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "temperature")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          int timez = atoi(argument);
          room->temperature = timez;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "mist")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          int timez = atoi(argument);
          if (timez > 3 || timez < 0) {
            send_to_char("Select a number between 0 and 3.\n\r", ch);
            return;
          }
          room->mist_level = timez;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "raining")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          if (room->raining == 0) {
            room->raining = 1;
            room->cloud_cover = UMAX(room->cloud_cover, 80);
            room->cloud_density = UMAX(room->cloud_density, 50);
          }
          else
          room->raining = 0;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "hailing")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          if (room->hailing == 0) {
            room->hailing = 1;
            room->cloud_cover = UMAX(room->cloud_cover, 80);
            room->cloud_density = UMAX(room->cloud_density, 50);
          }
          else
          room->hailing = 0;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "snowing")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          if (room->snowing == 0) {
            room->snowing = 1;
            room->cloud_cover = UMAX(room->cloud_cover, 80);
            room->cloud_density = UMAX(room->cloud_density, 50);
          }
          else
          room->snowing = 0;
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "climate")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          if (get_loc(argument) == NULL) {
            send_to_char("No such territory.\n\r", ch);
            return;
          }
          assign_weather(room, get_loc(argument), FALSE);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "door")) {
      if (!str_cmp(argument, "")) {
        send_to_char("Syntax: myrooms door (roomnumber) (direction)\n\r", ch);
        return;
      }

      argument = one_argument_nouncap(argument, arg2);
      if (!is_number(arg2) || safe_strlen(argument) < 2) {
        send_to_char("Syntax: myrooms door (roomnumber) (direction)\n\r", ch);
        return;
      }
      j = atoi(arg2);
      bool changed = FALSE;
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          int value, rev;
          value = flag_value(exit_flags, "door");
          ROOM_INDEX_DATA *pToRoom;
          int door = -1;
          for (int f = 0; f < 9; f++) {
            if (!str_cmp(argument, dir_name[f][0]))
            door = f;
          }
          if (door < 0) {
            send_to_char("Exit doesn't exist.\n\r", ch);
            return;
          }

          if (!room->exit[door]) {
            send_to_char("Exit doesn't exist.\n\r", ch);
            return;
          }
          TOGGLE_BIT(room->exit[door]->rs_flags, value);
          room->exit[door]->exit_info = room->exit[door]->rs_flags;

          pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
          rev = rev_dir[door];

          if (pToRoom->exit[rev] != NULL) {
            pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
            pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
          }

          value = flag_value(exit_flags, "closed");

          TOGGLE_BIT(room->exit[door]->rs_flags, value);
          room->exit[door]->exit_info = room->exit[door]->rs_flags;

          pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */
          rev = rev_dir[door];

          if (pToRoom->exit[rev] != NULL) {
            pToRoom->exit[rev]->rs_flags = room->exit[door]->rs_flags;
            pToRoom->exit[rev]->exit_info = room->exit[door]->exit_info;
          }

          changed = TRUE;
        }
        i++;
      }
      if (changed == FALSE) {
        send_to_char("Syntax: myrooms door (roomnumber) (direction)\n\r", ch);
        return;
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);
    }
    else if (!str_cmp(arg1, "public")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          if (room->sector_type == SECT_HOUSE) {
            send_to_char("Houses can't be public.\n\r", ch);
            return;
          }
          if (IS_SET(room->room_flags, ROOM_PUBLIC)) {
            send_to_char("That's already a public room.\n\r", ch);
            return;
          }
          if (IS_SET(room->room_flags, ROOM_LOCKED))
          REMOVE_BIT(room->room_flags, ROOM_LOCKED);
          if (IS_SET(room->room_flags, ROOM_INVISIBLE))
          REMOVE_BIT(room->room_flags, ROOM_INVISIBLE);

          SET_BIT(room->room_flags, ROOM_PUBLIC);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "invisible") && is_gm(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {

          if (IS_SET(room->room_flags, ROOM_LOCKED)) {
            REMOVE_BIT(room->room_flags, ROOM_LOCKED);
          }
          else if (IS_SET(room->room_flags, ROOM_INVISIBLE)) {
            send_to_char("That's already an invisible room.\n\r", ch);
            return;
          }
          if (!IS_SET(room->room_flags, ROOM_INVISIBLE))
          SET_BIT(room->room_flags, ROOM_INVISIBLE);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "size") && is_gm(ch)) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          room->size = atoi(argument);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "private")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {

          if (IS_SET(room->room_flags, ROOM_LOCKED)) {
            REMOVE_BIT(room->room_flags, ROOM_LOCKED);
          }
          else if (!IS_SET(room->room_flags, ROOM_PUBLIC)) {
            send_to_char("That's already a private room.\n\r", ch);
            return;
          }
          if (IS_SET(room->room_flags, ROOM_INVISIBLE))
          REMOVE_BIT(room->room_flags, ROOM_INVISIBLE);

          if (IS_SET(room->room_flags, ROOM_PUBLIC))
          REMOVE_BIT(room->room_flags, ROOM_PUBLIC);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "indoors")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {

          if (IS_SET(room->room_flags, ROOM_INDOORS)) {
            send_to_char("That's already an indoors room.\n\r", ch);
            return;
          }
          SET_BIT(room->room_flags, ROOM_INDOORS);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "outdoors")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          if (!IS_SET(room->room_flags, ROOM_INDOORS)) {
            send_to_char("That's already a outdoors room.\n\r", ch);
            return;
          }
          REMOVE_BIT(room->room_flags, ROOM_INDOORS);
          send_to_char("Done\n\r", ch);
          SET_BIT(room->area->area_flags, AREA_CHANGED);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "delete")) {
      argument = one_argument_nouncap(argument, arg2);
      j = atoi(arg2);
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          (*it)->cooldown = 0;
          (*it)->valid = FALSE;

          for (int door = 0; door < 9; door++) {
            ROOM_INDEX_DATA *pToRoom;
            int rev = rev_dir[door];
            if (room->exit[door] == NULL)
            continue;

            pToRoom = room->exit[door]->u1.to_room; /* ROM OLC */

            if (pToRoom != NULL) {
              if (pToRoom->exit[rev]) {
                free_exit(pToRoom->exit[rev]);
                pToRoom->exit[rev] = NULL;
              }
            }

            free_exit(room->exit[door]);
            room->exit[door] = NULL;
          }

          send_to_char("Done\n\r", ch);
          return;
        }
        i++;
      }
    }
    else if (!str_cmp(arg1, "extra")) {
      EXTRA_DESCR_DATA *ed;

      if (!str_cmp(argument, "")) {
        send_to_char("Syntax: myrooms extra (roomnumber) (Existing or new keyword)\n\r", ch);
        return;
      }

      argument = one_argument_nouncap(argument, arg2);
      if (!is_number(arg2) || safe_strlen(argument) < 2) {
        send_to_char("Syntax: myrooms extra (roomnumber) (Existing or new keyword)\n\r", ch);
        return;
      }

      j = atoi(arg2);
      bool changed = FALSE;
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          for (ed = room->extra_descr; ed; ed = ed->next) {
            if (is_name(argument, ed->keyword))
            break;
          }

          if (!ed) {
            ed = new_extra_descr();
            ed->keyword = str_dup(argument);
            ed->next = room->extra_descr;
            room->extra_descr = ed;
            string_append(ch, &room->extra_descr->description);
          }
          else
          string_append(ch, &ed->description);

          changed = TRUE;
        }
        i++;
      }
      if (changed == FALSE) {
        send_to_char("Syntax: myrooms extra (roomnumber) (Existing or new keyword)\n\r", ch);
        return;
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "extradelete")) {
      EXTRA_DESCR_DATA *ed;
      EXTRA_DESCR_DATA *ped = NULL;

      if (!str_cmp(argument, "")) {
        send_to_char("Syntax: myrooms extradelete (roomnumber) (keyword)\n\r", ch);
        return;
      }

      argument = one_argument_nouncap(argument, arg2);
      if (!is_number(arg2) || safe_strlen(argument) < 2) {
        send_to_char("Syntax: myrooms extradelete (roomnumber) (keyword)\n\r", ch);
        return;
      }

      j = atoi(arg2);
      bool changed = FALSE;
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          for (ed = room->extra_descr; ed; ed = ed->next) {
            if (ed->next != NULL && is_name(argument, ed->next->keyword))
            ped = ed;
            if (is_name(argument, ed->keyword))
            break;
          }

          if (ed) {
            if (!ped)
            room->extra_descr = ed->next;
            else
            ped->next = ed->next;

            free_extra_descr(ed);
            changed = TRUE;
          }
        }
        i++;
      }
      if (changed == FALSE) {
        send_to_char("Syntax: myrooms extradelete (roomnumber) (keyword)\n\r", ch);
        return;
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "place")) {
      EXTRA_DESCR_DATA *ed;

      if (!str_cmp(argument, "")) {
        send_to_char("Syntax: myrooms place (roomnumber) (Existing or new keyword)\n\r", ch);
        return;
      }

      argument = one_argument_nouncap(argument, arg2);
      if (!is_number(arg2) || safe_strlen(argument) < 2) {
        send_to_char("Syntax: myrooms place (roomnumber) (Existing or new keyword)\n\r", ch);
        return;
      }

      j = atoi(arg2);
      bool changed = FALSE;
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          for (ed = room->places; ed; ed = ed->next) {
            if (is_name(argument, ed->keyword))
            break;
          }

          if (!ed) {
            ed = new_extra_descr();
            ed->keyword = str_dup(argument);
            ed->next = room->places;
            room->places = ed;
            string_append(ch, &room->places->description);
          }
          else
          string_append(ch, &ed->description);

          changed = TRUE;
        }
        i++;
      }
      if (changed == FALSE) {
        send_to_char("Syntax: myrooms place (roomnumber) (Existing or new keyword)\n\r", ch);
        return;
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else if (!str_cmp(arg1, "placedelete")) {
      EXTRA_DESCR_DATA *ed;
      EXTRA_DESCR_DATA *ped = NULL;

      if (!str_cmp(argument, "")) {
        send_to_char("Syntax: myrooms placedelete (roomnumber) (keyword)\n\r", ch);
        return;
      }

      argument = one_argument_nouncap(argument, arg2);
      if (!is_number(arg2) || safe_strlen(argument) < 2) {
        send_to_char("Syntax: myrooms placedelete (roomnumber) (keyword)\n\r", ch);
        return;
      }

      j = atoi(arg2);
      bool changed = FALSE;
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        room = get_room_index((*it)->vnum);
        if (room == NULL)
        continue;
        if (str_cmp((*it)->author, ch->name))
        continue;
        if (j == i) {
          for (ed = room->places; ed; ed = ed->next) {
            if (ed->next != NULL && is_name(argument, ed->next->keyword))
            ped = ed;
            if (is_name(argument, ed->keyword))
            break;
          }

          if (ed) {
            if (!ped)
            room->places = ed->next;
            else
            ped->next = ed->next;

            free_extra_descr(ed);
            changed = TRUE;
          }
        }
        i++;
      }
      if (changed == FALSE) {
        send_to_char("Syntax: myrooms placedelete (roomnumber) (keyword)\n\r", ch);
        return;
      }
      SET_BIT(room->area->area_flags, AREA_CHANGED);

    }
    else
    send_to_char("Syntax: myrooms list/myrooms description/name/delete/extra/extradelete/place/placedelete/door/indoors/outdoors/public/private/nightmaredescription/size/timezone/cloudcover/clouddensity/temperature/mist/raining/hailing/snowing/climate/dreamroom/dreamentrance (number)\n\r", ch);
  }

  void show_nearbyrooms(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room;
    int i, k = 2;

    for (i = 0; i < MAX_ROOMS; i++) {
      room = get_room_index(room_list[i]);
      if (room->area != ch->in_room->area)
      continue;
      if (room == ch->in_room)
      continue;

      if (room->exit[1] == NULL || room->exit[1]->u1.to_room != ch->in_room) {
        if (ch->in_room->exit[1] == NULL || ch->in_room->exit[1]->u1.to_room != room)
        continue;
      }

      if (k % 2 == 0)
      printf_to_char(ch, "[%d] %s, ", get_po(room), room->name);
      else
      printf_to_char(ch, "[%d] %s\n\r", get_po(room), room->name);
      i++;
      k++;
    }
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (room->area != ch->in_room->area)
      continue;
      if (room == ch->in_room)
      continue;
      if (IS_SET(room->room_flags, ROOM_INVISIBLE) && !is_gm(ch))
      continue;
      if (room->exit[1] == NULL || room->exit[1]->u1.to_room != ch->in_room) {
        if (ch->in_room->exit[1] == NULL || ch->in_room->exit[1]->u1.to_room != room)
        continue;
      }
      if (room->sector_type == SECT_STREET && IS_SET(room->room_flags, ROOM_PUBLIC)) {
        if (k % 2 == 0)
        printf_to_char(ch, "[%d] %s%s, ", get_po(room), room->name, get_address(room));
        else
        printf_to_char(ch, "[%d] %s%s\n\r", get_po(room), room->name, get_address(room));
        i++;
        k++;
      }
      else if (ch->in_room->sector_type == SECT_STREET && IS_SET(ch->in_room->room_flags, ROOM_PUBLIC)) {
        if (k % 2 == 0)
        printf_to_char(ch, "[%d] %s%s, ", get_po(room), room->name, get_address(room));
        else
        printf_to_char(ch, "[%d] %s%s\n\r", get_po(room), room->name, get_address(room));
        i++;
        k++;
      }
      else {
        if (k % 2 == 0)
        printf_to_char(ch, "[%d] %s, ", get_po(room), room->name, get_address(room));
        else
        printf_to_char(ch, "[%d] %s\n\r", get_po(room), room->name, get_address(room));
        i++;
        k++;
      }
    }
  }

  void subwaytowalk(CHAR_DATA *ch) {
    if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    REMOVE_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 80;
    ch->in_room->sector_type = SECT_STREET;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("The Sidewalk");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }
  void walktosubway(CHAR_DATA *ch) {
    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    SET_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_ALLEY;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("NYC Subway");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }
  void bustowalk(CHAR_DATA *ch) {
    if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    REMOVE_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 80;
    ch->in_room->sector_type = SECT_STREET;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("The Sidewalk");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }
  void walktobus(CHAR_DATA *ch) {
    if (ch->pcdata->travel_time <= 5) {
      bustowalk(ch);
      return;
    }
    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    SET_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_CAR;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("A bus");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }
  void cartowalk(CHAR_DATA *ch) {
    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    SET_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_CAR;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup(vehicle_name(ch));
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup(vehicle_desc(ch));
  }
  void cabtowalk(CHAR_DATA *ch) {
    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    SET_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_CAR;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("Back of a taxi cab");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }

  void biketowalk(CHAR_DATA *ch) {
    if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    REMOVE_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_CAR;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup(vehicle_name(ch));
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup(vehicle_desc(ch));
  }

  void pathtowalk(CHAR_DATA *ch) {
    if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    REMOVE_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_FOREST;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("A path through the gloomy forest");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }

  void planetowalk(CHAR_DATA *ch) {
    if (!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    SET_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_CAR;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("Commercial plane");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }

  void flytowalk(CHAR_DATA *ch) {
    if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
    REMOVE_BIT(ch->in_room->room_flags, ROOM_INDOORS);
    ch->in_room->size = 20;
    ch->in_room->sector_type = SECT_AIR;
    free_string(ch->in_room->name);
    ch->in_room->name = str_dup("Skies above NYC");
    free_string(ch->in_room->description);
    ch->in_room->description = str_dup("");
  }

  _DOFUN(do_rplist) {
    int rooms[20];
    int pops[20];
    int maxpop = 0;
    int maxvnum = 0;
    int i, j, k, point = 1;
    CHAR_DATA *target;
    ROOM_INDEX_DATA *room;
    for (i = 0; i < 20; i++) {
      rooms[i] = 0;
      pops[i] = 0;
    }
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (!d->character)
      continue;

      target = CH(d);

      if ((d->connected != CON_PLAYING && d->connected != CON_CREATION) || (IS_IMMORTAL(target) && target->invis_level > ch->level))
      continue;

      if (!IS_IMMORTAL(ch) && IS_FLAG(target->comm, COMM_WHOINVIS))
      continue;

      for (i = 0; i < 20; i++) {
        if (rooms[i] == target->in_room->vnum) {
          pops[i]++;
          break;
        }
        if (rooms[i] == 0) {
          rooms[i] = target->in_room->vnum;
          pops[i] = 1;
          break;
        }
      }
    }
    for (j = 0; j < 20; j++) {
      for (i = 0; i < 20; i++) {
        if (pops[i] > maxpop) {
          maxpop = pops[i];
          maxvnum = rooms[i];
        }
      }
      room = get_room_index(maxvnum);
      if (room == NULL || maxpop == 0)
      return;
      for (k = 0; k < MAX_ROOMS; k++) {
        if (room_list[k] == maxvnum)
        point = k + 1;
      }
      for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
      it != PlayerroomVect.end(); ++it) {
        if (!(*it)->author || (*it)->author[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
        continue;
        if (room == NULL)
        continue;

        if ((*it)->vnum == maxvnum)
        point = k + 1;
        k++;
      }
      printf_to_char(ch, "[%d] %s (%d)\n\r", point, room->name, maxpop);
      for (i = 0; i < 20; i++) {
        if (rooms[i] == maxvnum) {
          rooms[i] = 0;
          pops[i] = 0;
        }
      }
      maxvnum = 0;
      maxpop = 0;
    }
  }

  void move_closer(CHAR_DATA *ch, CHAR_DATA *victim, int dist) {
    if (in_fight(ch) || is_helpless(ch))
    return;
    if (dist == 0) {
      ch->x = victim->x;
      ch->y = victim->y;
    }
    else {
      if (ch->x > victim->x)
      ch->x = UMIN(victim->x, ch->x - dist);
      else if (ch->x < victim->x)
      ch->x = UMAX(victim->x, ch->x + dist);

      if (ch->y > victim->y)
      ch->y = UMIN(victim->y, ch->y - dist);
      else if (ch->y < victim->y)
      ch->y = UMAX(victim->y, ch->y + dist);
    }
  }
  int player_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;

      if (IS_IMMORTAL(victim))
      continue;

      if (IS_FLAG(victim->act, PLR_GM))
      continue;
      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  int where_pops(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;
      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  int where_pop(ROOM_INDEX_DATA *room) {
    int pop = 0;
    CHAR_DATA *victim;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->connected != CON_PLAYING)
      continue;

      victim = CH(d);

      if (victim == NULL)
      continue;

      if (IS_NPC(victim))
      continue;

      if (victim->in_room == NULL)
      continue;
      if (victim->in_room == room)
      pop++;
    }
    return pop;
  }

  _DOFUN(do_pwhere) {
    ROOM_INDEX_DATA *room;
    int i;

    for (i = 0; i < MAX_ROOMS; i++) {
      room = get_room_index(room_list[i]);
      if (where_pop(room) > 0 && room_display(room, argument)) {
        printf_to_char(ch, "[%d] ", get_po(room));
        printf_to_char(ch, "%s ", room->name);
        printf_to_char(ch, ", %s", room->area->name);
        printf_to_char(ch, "[%d Pop]\n\r", where_pops(room));

        //                    printf_to_char(ch, "[%d] %s (%d Pop)\n\r", //                    get_po(room), room->name, where_pops(room));
      }
    }
    send_to_char("\n\r", ch);
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (IS_SET(room->room_flags, ROOM_PUBLIC))
      continue;
      /*
if(room->area != ch->in_room->area)
continue;
if(room->x > ch->in_room->x + 5 || room->x < ch->in_room->x - 5)
continue;
if(room->y > ch->in_room->y + 5 || room->y < ch->in_room->y - 5)
continue;
*/
      if (where_pop(room) < 1)
      continue;
      if (room_display(room, argument)) {
        printf_to_char(ch, "[%d] ", get_po(room));
        printf_to_char(ch, "%s", room->name);
        printf_to_char(ch, "%s", get_address(room));
        printf_to_char(ch, ", %s", room->area->name);
        printf_to_char(ch, " [%d Pop]\n\r", where_pops(room));
      }
      //            printf_to_char(ch, "[%d] %s (%d Pop)\n\r", get_po(room), //            room->name, where_pops(room));
      i++;
    }
    send_to_char("\n\r", ch);
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (!IS_SET(room->room_flags, ROOM_PUBLIC))
      continue;
      if (where_pop(room) < 1)
      continue;
      if (room_display(room, argument)) {
        printf_to_char(ch, "[%d] ", get_po(room));
        printf_to_char(ch, "%s", room->name);
        printf_to_char(ch, "%s", get_address(room));
        printf_to_char(ch, ", %s", room->area->name);
        printf_to_char(ch, " [%d Pop]\n\r", where_pops(room));
      }
      //            printf_to_char(ch, "[%d] %s (%d Pop)\n\r", get_po(room), //            room->name, where_pops);
      i++;
    }
  }

  void message(char *argument, char *message) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];

    d.original = NULL;
    if ((victim = get_char_world_pc(argument)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Message");

      if (!load_char_obj(&d, argument)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }
    if (online) {
      printf_to_char(victim, "\n\r`WMessage`x: %s.`x\n\r", message);
    }
    else {
      sprintf(buf, "%s\n%s", victim->pcdata->messages, message);
      free_string(victim->pcdata->messages);
      victim->pcdata->messages = str_dup(buf);
      save_char_obj(victim, FALSE, FALSE);
      free_char(victim);
    }
  }

  int offline_skill(char *name, int skill) {

    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Offline Skill");

      if (!load_char_obj(&d, name)) {
        return 0;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return 0;
    }

    int val = get_skill(victim, skill);

    if (!online)
    free_char(victim);

    return val;
  }

  bool has_hellgate(ROOM_INDEX_DATA *room) {
    bool online = FALSE;
    CHAR_DATA *owner = NULL;
    DESCRIPTOR_DATA d;
    Buffer outbuf;
    char buf[MSL];
    bool val = FALSE;

    if (room->sector_type == SECT_HELLGATE)
    return TRUE;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0')
      continue;

      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if ((*it)->vnum != room->vnum)
      continue;

      if (room->sector_type != SECT_HOUSE)
      continue;

      d.original = NULL;
      if ((owner = get_char_world_pc((*it)->author)) != NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: Hellgate");

        if (!load_char_obj(&d, (*it)->author)) {
          return FALSE;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize((*it)->author));
        owner = d.character;
      }
      if (owner == NULL)
      return FALSE;

      if (IS_NPC(owner)) {
        if (!online)
        free_char(owner);

        return FALSE;
      }
    }
    if (!online)
    free_char(owner);

    return val;
  }

  void room_purge(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (str_cmp((*it)->author, ch->name))
      continue;
      (*it)->cooldown = 0;
      (*it)->valid = FALSE;
      send_to_char("Done\n\r", ch);
      return;
    }
  }

  _DOFUN(do_deleteroom) {
    ROOM_INDEX_DATA *room = get_poroom(atoi(argument));

    if (room == NULL)
    return;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      if ((*it)->vnum != room->vnum)
      continue;
      (*it)->cooldown = 0;
      (*it)->valid = FALSE;
      send_to_char("Done\n\r", ch);
      return;
    }
  }

  bool has_key(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      if ((*it)->vnum == room->vnum) {
        if (!str_cmp(ch->name, (*it)->author)) {
          return TRUE;
        }

        for (ed = room->extra_descr; ed; ed = ed->next) {
          if (is_name("!keys", ed->keyword)) {
            if (!is_name(ch->name, ed->description)) {
              return TRUE;
            }
          }
        }
      }
    }
    return FALSE;
  }

  bool is_tresspassable(ROOM_INDEX_DATA *room) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];

    if (room->sector_type == SECT_HOUSE)
    return TRUE;

    if (room->sector_type != SECT_SHOP && room->sector_type != SECT_CLUB && room->sector_type != SECT_RESTERAUNT && room->sector_type != SECT_COMMERCIAL && room->sector_type != SECT_CAFE)
    return FALSE;
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0')
      continue;

      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if ((*it)->vnum != room->vnum)
      continue;

      d.original = NULL;
      if ((victim = get_char_world_pc((*it)->author)) !=
          NULL) // Victim is online.
      online = TRUE;
      else {
        log_string("DESCRIPTOR: Tresspass");

        if (!load_char_obj(&d, (*it)->author)) {
          return FALSE;
        }

        sprintf(buf, "%s%s", PLAYER_DIR, capitalize((*it)->author));
        victim = d.character;
      }
      if (IS_NPC(victim)) {
        if (!online)
        free_char(victim);

        return FALSE;
      }
    }
    return FALSE;
  }

  bool directory_display(ROOM_INDEX_DATA *room, char *argument) {
    if (!str_cmp(argument, "all")) {
      if (room->sector_type == SECT_STREET || room->sector_type == SECT_ALLEY || room->sector_type == SECT_TUNNELS || room->sector_type == SECT_AIR || room->sector_type == SECT_PARK || room->sector_type == SECT_HOUSE)
      return FALSE;

      return TRUE;
    }

    if (room->sector_type == flag_value(sector_flags, argument))
    return TRUE;

    if (is_name(argument, room->name))
    return TRUE;

    return FALSE;
  }

  _DOFUN(do_directory) {
    ROOM_INDEX_DATA *room;
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;
      room = get_room_index((*it)->vnum);
      if (room == NULL)
      continue;
      if (IS_SET(room->room_flags, ROOM_INVISIBLE) && !is_gm(ch))
      continue;

      if (!timetraveler(ch) && ch->in_room->time != room->time)
      continue;

      if (ch->in_room->time != 3000 && ch->in_room->time != room->time && room->vnum != ch->pcdata->portaled_from)
      continue;

      if (room->exit[1] == NULL || !IS_SET(room->exit[1]->u1.to_room->room_flags, ROOM_PUBLIC) || room->exit[1]->u1.to_room->sector_type != SECT_STREET)
      continue;

      if (IS_SET(room->room_flags, ROOM_LOCKED))
      continue;

      if (room->sector_type == SECT_STREET)
      continue;

      if (directory_display(room, argument)) {

        printf_to_char(ch, "[%d] %s%s, %s (%s)\n\r", get_po(room), room->name, get_address(room), room->area->name, room_tag(ch, room, ch->in_room));
      }
    }
  }

  _DOFUN(do_out) {
    char buf[MSL];
    if (ch->in_room->exit[1] == NULL) {
      send_to_char("You don't see how to get out from here.\n\r", ch);
      return;
    }
    sprintf(buf, "%d", get_po(ch->in_room->exit[1]->u1.to_room));
    do_function(ch, &do_travel, buf);
  }

  _DOFUN(do_summon) {
    CHAR_DATA *victim;
    char arg1[MSL];

    argument = one_argument_nouncap(argument, arg1);

    if (!str_cmp(arg1, "mist")) {
      if (time_info.mist_timer > 5) {
        send_to_char("There's already a mist.\n\r", ch);
        return;
      }
      if (ch->pcdata->abommistcool > 0) {
        send_to_char("That ability is still on cooldown.\n\r", ch);
        return;
      }
      ch->pcdata->abommistcool = (60 * 12);
      time_info.mist_timer = UMAX(time_info.mist_timer + 15, 20);
      send_to_char("You summon a mist.\n\r", ch);
      WAIT_STATE(ch, PULSE_PER_SECOND * 10);
      return;
    }

    victim = get_char_world(ch, arg1);
    if (victim == NULL)
    return;
    if (!IS_IMMORTAL(ch)) {
      if (!is_gm(ch) && ch != victim && !higher_power(ch)) {
        send_to_char("This is a storyrunner command.\n\r", ch);
        return;
      }
      if (ch == victim)
      return;
      if (victim == NULL || IS_NPC(victim) || (get_gmtrust(ch, victim) < 0 && ch != victim)) {
        send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
        return;
      }
      if (IS_FLAG(victim->act, PLR_DEAD) || (victim->in_room != NULL && victim->in_room->vnum == 98)) {
        send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
        return;
      }

      if (victim->in_room->vnum == ROOM_INDEX_GENESIS) {
        send_to_char("They're not here or you're not trusted enough to do that.\n\r", ch);
        return;
      }
      if (ch->in_room->area->world != WORLD_ELSEWHERE && ch->in_room->area->vnum != 12 && victim->in_room->area->vnum != 12 && ch->in_room->area->world != in_world(victim) && victim->in_room->area->world != WORLD_ELSEWHERE) {
        send_to_char("They're on another world.\n\r", ch);
        return;
      }
      if (in_haven(victim->in_room) && (locked_room(victim->in_room, victim) || is_prisoner(victim))) {
        send_to_char("They're presently confined.\n\r", ch);
        return;
      }
      if(ch->in_room->area->vnum >= OTHER_FOREST_VNUM && ch->in_room->area->vnum <= HELL_FOREST_VNUM)
      {
        send_to_char("They're on another world.\n\r", ch);
        return;
      }
      else
      {
        if(victim->in_room->area->vnum >= OTHER_FOREST_VNUM && victim->in_room->area->vnum <= HELL_FOREST_VNUM)
        {
          send_to_char("They're on another world.\n\r", ch);
          return;
        }
      }

      if (in_fight(victim) || is_helpless(victim)) {
        send_to_char("They're a bit busy.\n\r", ch);
        return;
      }
      if (clinic_patient(victim)) {
        send_to_char("They cannot leave the institute.\n\r", ch);
        return;
      }
      if (room_hostile(victim->in_room) || is_pinned(victim)) {
        send_to_char("They're a bit busy.\n\r", ch);
        return;
      }

      if (same_player(ch, victim) && pc_pop(ch->in_room) < 1 && !IS_FLAG(victim->act, PLR_GUEST)) {
        send_to_char("Summon the other players in your plot first.\n\r", ch);
        return;
      }
      if (current_time < victim->pcdata->timebanished) {
        send_to_char("No.\n\r", ch);
        return;
      }
      if (victim->in_room != NULL && victim->in_room->vnum < 300)
      return;
      if (ch->in_room != NULL && ch->in_room->vnum < 300)
      return;

      if (get_gmtrust(ch, victim) < 2) {
        printf_to_char(
        victim, "%s would like to summon you, type 'agree' or 'disagree'\n\r", ch->name);
        printf_to_char(ch, "You attempt to summon them.\n\r");
        victim->pcdata->sr_connection = ch;
      }
      else {
        if (!IS_IMMORTAL(victim))
        act("$n leaves.", victim, NULL, NULL, TO_ROOM);
        char_from_room(victim);
        char_to_room(victim, ch->in_room);

        if (!IS_IMMORTAL(victim))
        act("$n arrives.", victim, NULL, NULL, TO_ROOM);
        send_to_char("You are summoned.\n\r", victim);
        victim->pcdata->sr_connection = NULL;
      }
    }
    else {
      if (!IS_IMMORTAL(victim))
      act("$n leaves.", victim, NULL, NULL, TO_ROOM);
      char_from_room(victim);
      char_to_room(victim, ch->in_room);

      if (!IS_IMMORTAL(victim))
      act("$n arrives.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You are summoned.\n\r", victim);
    }
  }

  _DOFUN(do_agree) {
    if (ch->pcdata->sr_connection == NULL || ch->pcdata->sr_connection->in_room == NULL) {
      send_to_char("There's nothing for you to agree to.\n\r", ch);
      return;
    }
    if (ch->in_room != NULL) {
      act("$n leaves.", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);
    }
    char_to_room(ch, ch->pcdata->sr_connection->in_room);

    act("$n arrives.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You are summoned.\n\r", ch);
    ch->pcdata->sr_connection = NULL;
  }

  _DOFUN(do_disagree) {
    if (ch->pcdata->sr_connection == NULL || ch->pcdata->sr_connection->in_room == NULL) {
      send_to_char("There's nothing for you to agree to.\n\r", ch);
      return;
    }
    printf_to_char(ch->pcdata->sr_connection, "%s doesn't want to be summoned.\n\r", ch->name);
    ch->pcdata->sr_connection = NULL;
    send_to_char("Done.\n\r", ch);
  }

  bool room_jeopardy(ROOM_INDEX_DATA *room) {

    if (IS_SET(room->room_flags, ROOM_PERM))
    return FALSE;

    if (room->area->vnum < 2)
    return FALSE;

    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->cooldown <= 0)
      continue;

      if ((*it)->vnum == room->vnum) {
        if ((*it)->cooldown < 4000)
        return TRUE;
        else
        return FALSE;
      }
    }
    return FALSE;
  }

  _DOFUN(do_adopt) {
    if (!room_jeopardy(ch->in_room)) {
      send_to_char("You can't adopt that room.\n\r", ch);
      return;
    }
    for (vector<PLAYERROOM_TYPE *>::iterator it = PlayerroomVect.begin();
    it != PlayerroomVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->vnum == ch->in_room->vnum) {
        free_string((*it)->author);
        (*it)->author = str_dup(ch->name);
        send_to_char("Room adopted!\n\r", ch);
        return;
      }
    }
  }

  int const trolly_stops[] = {2281,  1061,  3609,  2617,  14819, 14837, 2942, 2617,  3609,  3673,  5854,  1109,  15265, 1188, 1182,  15239, 15239, 15192, 15219, 15192, 1149, 15177, 1109,  5854,  3609};

  int trolly_at = 0;
  int trolly_timer = 0;
  int trolly_moving = 0;

  void trolly_message(int type) {

    if (type == 2) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;

        vch = CH(d);
        if (vch == NULL) {
          continue;
        }

        if (vch->in_room == NULL) {
          continue;
        }

        if (is_dreaming(vch))
        continue;
        if (vch->in_room->vnum == 19000) {
          send_to_char("The trolley starts moving again with a lurch.\n\r", vch);
        }
        if (vch->in_room->vnum == trolly_stops[trolly_at]) {
          send_to_char("The trolley pulls away.\n\r", vch);
        }
      }
    }
    else {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;

        vch = CH(d);
        if (vch == NULL) {
          continue;
        }

        if (vch->in_room == NULL) {
          continue;
        }
        if (is_dreaming(vch))
        continue;
        if (vch->in_room->vnum == 19000) {
          printf_to_char(vch, "The trolley pulls up at %s.\n\r", get_room_index(trolly_stops[trolly_at])->name);
        }
        if (vch->in_room->vnum == trolly_stops[trolly_at]) {
          send_to_char("The trolley pulls up here.\n\r", vch);
        }
        if ((vch->in_room->vnum == trolly_stops[trolly_at]) && IS_FLAG(vch->comm, COMM_BOARDING)) {
          do_function(vch, &do_board, "");
          REMOVE_FLAG(vch->comm, COMM_BOARDING);
        }
      }
    }
  }

  _DOFUN(do_disembark) {
    if (in_fight(ch) || is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || ch->in_room->vnum != 19000) {
      send_to_char("You're not on a trolley.\n\r", ch);
      return;
    }
    if (trolly_moving == 1) {
      send_to_char("You'll have to wait for it to stop first.\n\r", ch);
      return;
    }
    ROOM_INDEX_DATA *troom = ch->in_room;
    act("You disembark.", ch, NULL, NULL, TO_CHAR);
    act("$n gets off the trolley.", ch, NULL, NULL, TO_ROOM);
    if (is_possessed(ch) && get_possesser(ch) != NULL) {
      char_from_room(get_possesser(ch));
      char_to_room(get_possesser(ch), get_room_index(trolly_stops[trolly_at]));
    }
    char_from_room(ch);
    char_to_room(ch, get_room_index(trolly_stops[trolly_at]));
    act("$n gets off the trolley.", ch, NULL, NULL, TO_ROOM);
    CHAR_DATA *to;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if (d != NULL && d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;

        if (to == NULL)
        continue;
        if (IS_NPC(to))
        continue;

        if(to->in_room == troom && to->master == ch)
        {
          act("You disembark.", to, NULL, NULL, TO_CHAR);
          act("$n gets off the trolley.", to, NULL, NULL, TO_ROOM);
          if (is_possessed(to) && get_possesser(to) != NULL) {
            char_from_room(get_possesser(to));
            char_to_room(get_possesser(to), get_room_index(trolly_stops[trolly_at]));
          }
          char_from_room(to);
          char_to_room(to, get_room_index(trolly_stops[trolly_at]));
          act("$n gets off the trolley.", to, NULL, NULL, TO_ROOM);
        }
      }
    }

  }

  _DOFUN(do_board) {
    if (in_fight(ch) || is_helpless(ch)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }
    if (is_helpless(ch) || in_fight(ch) || room_hostile(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (ch->in_room == NULL || ch->in_room->vnum != trolly_stops[trolly_at] || trolly_moving == 1) {
      if (!IS_FLAG(ch->comm, COMM_BOARDING)) {
        SET_FLAG(ch->comm, COMM_BOARDING);
        send_to_char("You get ready to board the trolly.\n\r", ch);
      }
      else {
        REMOVE_FLAG(ch->comm, COMM_BOARDING);
        send_to_char("You stop preparing to board.\n\r", ch);
      }
      return;
    }
    act("You board the trolley.", ch, NULL, NULL, TO_CHAR);
    act("$n boards the trolley.", ch, NULL, NULL, TO_ROOM);

    for (CharList::iterator it = ch->in_room->people->begin();
    it != ch->in_room->people->end();) {
      CHAR_DATA *fch = *it;
      ++it;

      if (fch == NULL)
      break;

      if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch) && can_see_room(fch, get_room_index(19000))) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, get_room_index(19000));
      }
      else if (fch->master == ch && fch->position > POS_SITTING && !in_fight(ch)) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, get_room_index(19000));
      }
      else if (is_ghost(fch) && fch->possessing == ch) {
        act("You follow $N.", fch, NULL, ch, TO_CHAR);
        char_from_room(fch);
        char_to_room(fch, get_room_index(19000));
      }
    }

    char_from_room(ch);
    char_to_room(ch, get_room_index(19000));
    act("$n boards the trolley.", ch, NULL, NULL, TO_ROOM);
  }

  _DOFUN(do_adddesc) {
    ROOM_INDEX_DATA *room;
    if (is_dreaming(ch) && ch->pcdata->dream_room > 0)
    room = get_room_index(ch->pcdata->dream_room);
    else
    room = ch->in_room;

    smash_vector(argument);

    if (room == NULL)
    return;
    /*
if (ch->played / 3600 < 75 && ch->pcdata->account->maxhours < 100) {
send_to_char("You cannot do that yet.\n\r", ch);
return;
}
*/
    EXTRA_DESCR_DATA *ed;
    for (ed = room->extra_descr; ed; ed = ed->next) {
      if (is_name("temporary", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("temporary");
      ed->next = room->extra_descr;
      room->extra_descr = ed;
      string_append(ch, &room->extra_descr->description);
    }
    else
    string_append(ch, &ed->description);

    if (!IS_SET(room->area->area_flags, AREA_CHANGED))
    SET_BIT(room->area->area_flags, AREA_CHANGED);
  }

  _DOFUN(do_sradddesc) {
    if (!is_gm(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    EXTRA_DESCR_DATA *ed;
    for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
      if (is_name("!srtemporary", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("!srtemporary");
      ed->next = ch->in_room->extra_descr;
      ch->in_room->extra_descr = ed;
      string_append(ch, &ch->in_room->extra_descr->description);
    }
    else
    string_append(ch, &ed->description);

    if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
  }

  _DOFUN(do_sraddhidden) {
    if (!is_gm(ch)) {
      send_to_char("You can't do that.\n\r", ch);
      return;
    }
    if (!has_mystery(ch)) {
      send_to_char("You need to be running a mystery first.\n\r", ch);
      return;
    }
    EXTRA_DESCR_DATA *ed;
    for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
      if (is_name("!srhidden", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("!srhidden");
      ed->next = ch->in_room->extra_descr;
      ch->in_room->extra_descr = ed;
      string_append(ch, &ch->in_room->extra_descr->description);
    }
    else
    string_append(ch, &ed->description);

    if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
  }

  _DOFUN(do_addhidden) {
    EXTRA_DESCR_DATA *ed;
    for (ed = ch->in_room->extra_descr; ed; ed = ed->next) {
      if (is_name("!hidden", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("!hidden");
      ed->next = ch->in_room->extra_descr;
      ch->in_room->extra_descr = ed;
      string_append(ch, &ch->in_room->extra_descr->description);
    }
    else
    string_append(ch, &ed->description);

    if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
  }

  _DOFUN(do_lease) {}

  char *generate_lplate() {
    char buf[MSL];

    switch (number_percent() % 3) {
    case 0:
      sprintf(buf, "%03d %c%c%d", number_range(0, 999), ('A' + number_range(0, 25)), ('A' + number_range(0, 25)), number_range(0, 9));
      break;
    case 1:
      sprintf(buf, "%d%c%c %03d", number_range(0, 9), ('A' + number_range(0, 25)), ('A' + number_range(0, 25)), number_range(0, 999));
      break;
    default:
      sprintf(buf, "%d%c %c%c%02d", number_range(0, 9), ('A' + number_range(0, 25)), ('A' + number_range(0, 25)), ('A' + number_range(0, 25)), number_range(0, 99));
      break;
    }
    return str_dup(buf);
  }

  _DOFUN(do_testlplate) { printf_to_char(ch, "%s\n\r", generate_lplate()); }

  int cartype(char *argument) {
    if (!str_cmp(argument, "luxury"))
    return CAR_LUXURY;
    if (!str_cmp(argument, "safe"))
    return CAR_SAFE;
    if (!str_cmp(argument, "sport"))
    return CAR_SPORT;
    if (!str_cmp(argument, "van"))
    return CAR_VAN;
    if (!str_cmp(argument, "tough"))
    return CAR_TOUGH;
    if (!str_cmp(argument, "sportsbike"))
    return CAR_SPORTSBIKE;
    if (!str_cmp(argument, "cruiser"))
    return CAR_CRUISER;
    if (!str_cmp(argument, "horse"))
    return CAR_HORSE;

    return -1;
  }
  bool valid_carpair(int typeone, int typetwo) {
    if (typetwo == -1)
    return TRUE;

    if (typeone == typetwo)
    return FALSE;

    if (typeone == CAR_HORSE || typetwo == CAR_HORSE)
    return FALSE;

    if (typeone == CAR_SPORTSBIKE || typeone == CAR_CRUISER) {
      if (typetwo == CAR_SPORTSBIKE || typetwo == CAR_CRUISER)
      return TRUE;
    }
    else {
      if (typetwo != CAR_SPORTSBIKE && typetwo != CAR_CRUISER)
      return TRUE;
    }
    return FALSE;
  }
  char *default_carname(int type) {
    switch (type) {
    case CAR_LUXURY:
      return "luxury car";
      break;
    case CAR_SAFE:
      return "safe car";
      break;
    case CAR_SPORT:
      return "sports car";
      break;
    case CAR_VAN:
      return "van";
      break;
    case CAR_TOUGH:
      return "tough car";
      break;
    case CAR_SPORTSBIKE:
      return "sports motorcycle";
      break;
    case CAR_CRUISER:
      return "cruiser motorcycle";
      break;
    case CAR_HORSE:
      return "horse";
      break;
    }
    return "car";
  }

  bool has_lplate(CHAR_DATA *ch, char *argument) {
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(argument, ch->pcdata->garage_lplate[i]))
      return TRUE;
    }
    return FALSE;
  }
  char *new_lplate(CHAR_DATA *ch) {
    char plate[MSL];
    for (int i = 0; i < 10; i++) {
      sprintf(plate, "%s", generate_lplate());
      if (!has_lplate(ch, plate))
      return str_dup(plate);
    }
    return generate_lplate();
  }

  int carqualitylevel(int cost, int typeone, int typetwo) {
    int val = cost;
    if (typetwo != -1)
    val /= 2;

    if (typeone == CAR_SPORTSBIKE || typeone == CAR_CRUISER || typeone == CAR_HORSE) {
      if (val >= 25000)
      return 5;
      if (val >= 15000)
      return 4;
      if (val >= 7500)
      return 3;
      if (val >= 3000)
      return 2;
      return 1;
    }
    else {
      if (val >= 50000)
      return 5;
      if (val >= 30000)
      return 4;
      if (val >= 15000)
      return 3;
      if (val >= 5000)
      return 2;
      return 1;
    }
  }
  char *carqualityname(int cost, int typeone, int typetwo) {
    int level = carqualitylevel(cost, typeone, typetwo);
    if (typeone == CAR_HORSE) {
      if (level == 5)
      return "purebred";
      if (level == 4)
      return "well-bred";
      if (level == 3)
      return "healthy";
      if (level == 2)
      return "weak-looking";
      return "sick-looking";
    }
    if (level == 5)
    return "excellent";
    if (level == 4)
    return "high quality";
    if (level == 3)
    return "standard quality";
    if (level == 2)
    return "economy";

    return "junker";
  }

  char *cartypename(int typeone, int typetwo) {
    if (typeone == CAR_HORSE) {
      return "horse";
    }
    if (typeone == CAR_SPORT) {
      if (typetwo == -1)
      return "sports car";
      if (typetwo == CAR_SAFE)
      return "safe sports car";
      if (typetwo == CAR_LUXURY)
      return "luxury sports car";
      if (typetwo == CAR_VAN)
      return "sports van";
      if (typetwo == CAR_TOUGH)
      return "muscle car";

      return cartypename(typetwo, typeone);
    }
    if (typeone == CAR_SAFE) {
      if (typetwo == -1)
      return "safe car";
      if (typetwo == CAR_LUXURY)
      return "luxury safe car";
      if (typetwo == CAR_VAN)
      return "minivan";
      if (typetwo == CAR_TOUGH)
      return "tough safe car";

      return cartypename(typetwo, typeone);
    }
    if (typeone == CAR_LUXURY) {
      if (typetwo == -1)
      return "luxury car";
      if (typetwo == CAR_VAN)
      return "luxury van";
      if (typetwo == CAR_TOUGH)
      return "luxury tough car";

      return cartypename(typetwo, typeone);
    }
    if (typeone == CAR_VAN) {
      if (typetwo == -1)
      return "van";
      if (typetwo == CAR_TOUGH)
      return "tough van";

      return cartypename(typetwo, typeone);
    }
    if (typeone == CAR_TOUGH) {
      if (typetwo == -1)
      return "tough car";

      return cartypename(typetwo, typeone);
    }
    if (typeone == CAR_SPORTSBIKE) {
      if (typetwo == -1)
      return "sportsbike";
      if (typetwo == CAR_CRUISER)
      return "sports crusier";
      return cartypename(typetwo, typeone);
    }
    if (typeone == CAR_CRUISER) {
      if (typetwo == -1)
      return "cruiser motorcycle";
      return cartypename(typetwo, typeone);
    }
    return "car";
  }

  char *carstatusname(int status) {

    if (status == GARAGE_GARAGED)
    return "Garaged";
    if (status == GARAGE_ACTIVE)
    return "Active";
    if (status == GARAGE_LOANED)
    return "Loaned";
    if (status == GARAGE_REPAIR)
    return "Repairing";
    if (status == GARAGE_RECOVER)
    return "Recovering";
    if (status == GARAGE_RECOVERREPAIR)
    return "Recovering";
    if (status == GARAGE_LOANEDDAMAGED)
    return "Loaned";

    return "Inactive";
  }

  bool is_key_for(OBJ_DATA *obj, char *lplate) {
    EXTRA_DESCR_DATA *ed;
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+lplate", ed->keyword)) {
        if (!str_cmp(ed->description, lplate))
        return TRUE;
      }
    }
    return FALSE;
  }

  void garage_update(CHAR_DATA *ch, int amount) {
    if (IS_NPC(ch))
    return;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_timer[i] > 0) {
        ch->pcdata->garage_timer[i] -= amount;
        if (ch->pcdata->garage_timer[i] <= 0) {
          if (ch->pcdata->garage_status[i] == GARAGE_REPAIR)
          ch->pcdata->garage_status[i] = GARAGE_GARAGED;
          if (ch->pcdata->garage_status[i] == GARAGE_RECOVER)
          ch->pcdata->garage_status[i] = GARAGE_GARAGED;
          if (ch->pcdata->garage_status[i] == GARAGE_RECOVERREPAIR) {
            ch->pcdata->garage_status[i] = GARAGE_REPAIR;
            ch->pcdata->garage_timer[i] = (60 * 24 * 2);
          }
          ch->pcdata->garage_location[i] = 0;
        }
      }
    }
  }

  bool offlinekeycheck(char *name, char *lplate) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];
    if (safe_strlen(name) < 2)
    return FALSE;
    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Offline Key");

      if (!load_char_obj(&d, name)) {
        return FALSE;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return FALSE;
    }

    bool value = FALSE;

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(victim->pcdata->garage_lplate[i], lplate) && (victim->pcdata->garage_status[i] == GARAGE_LOANED || victim->pcdata->garage_status[i] == GARAGE_LOANEDDAMAGED))
      value = TRUE;
    }

    if (!online)
    free_char(victim);

    return value;
  }

  void logonkeyscheck(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;

      if (obj->item_type == ITEM_KEY) {

        EXTRA_DESCR_DATA *ed;
        for (ed = obj->extra_descr; ed; ed = ed->next) {
          if (is_name("+lplate", ed->keyword)) {
            if (!offlinekeycheck(obj->material, ed->description)) {
              extract_obj(obj);
              return;
            }
          }
        }
      }
    }
  }

  char *vehicle_lplate(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0) {
      EXTRA_DESCR_DATA *ed;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+lplate", ed->keyword)) {
          return ed->description;
        }
      }
    }

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return ch->pcdata->garage_lplate[i];
    }
    return "";
  }
  char *vehicle_desc(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0) {
      EXTRA_DESCR_DATA *ed;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+cardesc", ed->keyword)) {
          return ed->description;
        }
      }
    }

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return ch->pcdata->garage_desc[i];
    }
    return "";
  }
  char *vehicle_name(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0) {
      EXTRA_DESCR_DATA *ed;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+carname", ed->keyword)) {
          return ed->description;
        }
      }
    }

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return ch->pcdata->garage_name[i];
    }
    return "";
  }

  void rundown(CHAR_DATA *ch, CHAR_DATA *victim, bool on_purpose) {
    int vicdefense = 5;

    if (is_super(victim))
    vicdefense += 5;

    if (is_undead(victim))
    vicdefense += 10;

    if (is_abom(victim))
    vicdefense += 5;

    if (victim->position == POS_SITTING || victim->position == POS_RESTING || victim->position == POS_SLEEPING)
    vicdefense /= 2;

    int offense = 8;

    offense += speed_bonus(ch) * 2;
    offense += toughness_bonus(ch);

    offense += number_range(-5, 10);

    if (IS_FLAG(ch->comm, COMM_SLOW))
    offense /= 3;

    act("You hit $N.", ch, NULL, victim, TO_CHAR);
    act("You hit $N.", ch, NULL, victim, TO_ROOM);
    act("$n hits $N", ch->your_car, NULL, victim, TO_ROOM);
    dact("$n hits $N", ch->your_car, NULL, victim, DISTANCE_FAR);

    if (on_purpose == TRUE && public_room(victim->in_room)) {
      char buf[MSL];
      NEWS_TYPE *news;

      sprintf(buf, "A hit an run was reported at %s today, witnesses say a %s with the licence plate %s ran into %s.\n\r", victim->in_room->name, vehicle_name(ch), vehicle_lplate(ch), victim->pcdata->intro_desc);
      news = new_news();
      news->timer = 1500;
      free_string(news->message);
      news->message = str_dup(buf);
      free_string(news->author);
      news->author = str_dup("Town News");
      NewsVect.push_back(news);
    }

    if (offense / 2 > vicdefense) {
      wound_char_absolute(victim, 2);

      act("$n is left severely wounded.", victim, NULL, NULL, TO_ROOM);
      act("You are left severely wounded.", victim, NULL, NULL, TO_CHAR);
      if (number_percent() % 2 == 0)
      victim->pcdata->sleeping = 300;

    }
    else if (offense > vicdefense) {
      wound_char_absolute(victim, 1);
      act("$n is left mildly wounded.", victim, NULL, NULL, TO_ROOM);
      act("You are left mildly wounded.", victim, NULL, NULL, TO_CHAR);
      if (number_percent() % 6 == 0)
      victim->pcdata->sleeping = 100;
    }
    else {
      act("$n is left uninjured.", victim, NULL, NULL, TO_ROOM);
      act("You are left uninjured.", victim, NULL, NULL, TO_CHAR);
    }
    victim->position = POS_RESTING;
    WAIT_STATE(victim, PULSE_PER_SECOND * 10);
  }

  void npc_rundown(CHAR_DATA *victim) {
    int offense = 8;

    int vicdefense = 5;

    if (is_super(victim))
    vicdefense += 5;

    if (is_undead(victim))
    vicdefense += 10;

    if (is_abom(victim))
    vicdefense += 5;

    if (victim->position == POS_SITTING || victim->position == POS_RESTING || victim->position == POS_SLEEPING)
    vicdefense /= 2;
    offense += number_range(-5, 10);

    act("A car collides with $n before careening away.", victim, NULL, NULL, TO_ROOM);
    act("A car collides with you before careening away.", victim, NULL, NULL, TO_CHAR);
    dact("A car collides with $n before careening away.", victim, NULL, NULL, DISTANCE_FAR);

    if (offense / 2 > vicdefense) {
      wound_char_absolute(victim, 2);

      act("$n is left severely wounded.", victim, NULL, NULL, TO_ROOM);
      act("You are left severely wounded.", victim, NULL, NULL, TO_CHAR);
      if (number_percent() % 2 == 0)
      victim->pcdata->sleeping = 300;

    }
    else if (offense > vicdefense) {
      wound_char_absolute(victim, 1);
      act("$n is left mildly wounded.", victim, NULL, NULL, TO_ROOM);
      act("You are left mildly wounded.", victim, NULL, NULL, TO_CHAR);
      if (number_percent() % 6 == 0)
      victim->pcdata->sleeping = 100;
    }
    else {
      act("$n is left uninjured.", victim, NULL, NULL, TO_ROOM);
      act("You are left uninjured.", victim, NULL, NULL, TO_CHAR);
    }
    victim->position = POS_RESTING;
    WAIT_STATE(victim, PULSE_PER_SECOND * 10);
  }

  void collision_injury(CHAR_DATA *ch, int level) {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      int vlevel = level;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL)
      continue;
      if (vch->in_room != ch->in_room) {
        continue;
      }
      if (ch == vch)
      continue;

      if (vlevel > number_range(0, 2))
      wound_char_absolute(vch, 1);
      vlevel--;
      if (vlevel > number_range(0, 2))
      wound_char_absolute(vch, 2);
      vlevel--;
    }
    int chlevel = level;
    if (chlevel > number_range(0, 2))
    wound_char_absolute(ch, 1);
    chlevel--;
    if (chlevel > number_range(0, 2))
    wound_char_absolute(ch, 2);
    chlevel--;
  }

  void carversusman(CHAR_DATA *ch, CHAR_DATA *car) {
    CHAR_DATA *driver = car->your_car;

    int toughness = toughness_bonus(driver);

    if (get_skill(ch, SKILL_STRENGTH) < 8) {
      rundown(driver, ch, FALSE);
      return;
    }
    if (toughness > number_range(1, 10)) {
      rundown(driver, ch, FALSE);
      return;
    }

    if (number_percent() % 2 == 0)
    rundown(driver, ch, FALSE);

    act("You smash into $N and force it to stop.", ch, NULL, car, TO_CHAR);
    act("$n smashes into $N and forces an accident.", ch, NULL, car, TO_ROOM);
    dact("$n smashes into $N and forces an accident.", ch, NULL, car, DISTANCE_MEDIUM);
    act("$N crashes into the car, forcing an accident.", driver, NULL, ch, TO_CHAR);
    act("$N crashes into the car, forcing an accident.", driver, NULL, ch, TO_ROOM);

    collision_injury(driver, 1);
    pile_out(driver);
    ding_car(driver);
  }

  void carattack(CHAR_DATA *ch, CHAR_DATA *car) {
    CHAR_DATA *driver = car->your_car;

    if (has_sgun(ch) || has_lgun(ch)) {
      int dam =
      UMAX(get_disc(ch, DIS_RIFLES, FALSE), get_disc(ch, DIS_PISTOLS, FALSE));
      dam = UMAX(dam, get_disc(ch, DIS_SHOTGUNS, FALSE));
      dam = UMAX(dam, get_disc(ch, DIS_CARBINES, FALSE));

      int toughness = toughness_bonus(driver);

      if (toughness * 10 > dam || toughness >= 5) {
        act("You open fire on $N but aren't able to stop it.", ch, NULL, car, TO_CHAR);
        act("$n opens fire on $N but isn't able to stop it.", ch, NULL, car, TO_ROOM);
        dact("$n opens fire on $N but isn't able to stop it.", ch, NULL, car, DISTANCE_MEDIUM);
        act("Bullets wash over the car fired by $N.", driver, NULL, ch, TO_CHAR);
        act("Bullets wash over the car fire by $N.", driver, NULL, ch, TO_ROOM);
        WAIT_STATE(ch, PULSE_PER_SECOND * 5);

        if (ch->in_room == car->in_room)
        carversusman(ch, car);
      }
      else {
        act("You open fire on $N and force an accident.", ch, NULL, car, TO_CHAR);
        act("$n opens fire on $N and force an accident.", ch, NULL, car, TO_ROOM);
        dact("$n opens first on $N and forces an accident.", ch, NULL, car, DISTANCE_MEDIUM);
        act("Bullets wash over the car, fired by $N, forcing an accident.", driver, NULL, ch, TO_CHAR);
        act("Bullets wash over the car, fired by $N, forcing an accident", driver, NULL, ch, TO_ROOM);

        collision_injury(driver, 1);
        pile_out(driver);
        ding_car(driver);
      }
    }
    else {
      if (get_skill(ch, SKILL_CUSTOM) < 1) {
        if (ch->in_room != car->in_room) {
          send_to_char("They're too far away.\n\r", ch);
          return;
        }
        if (get_skill(ch, SKILL_STRENGTH) < 8) {
          send_to_char("That would be a bit crazy.\n\r", ch);
          return;
        }
        carversusman(ch, car);
      }
      else {
        if (toughness_bonus(driver) > number_range(0, 10)) {
          act("You unleash on $N but aren't able to stop it.", ch, NULL, car, TO_CHAR);
          act("$n blasts $N with supernatural energy but isn't able to stop it.", ch, NULL, car, TO_ROOM);
          dact("$n blasts $N with supernatural energy but isn't able to stop it.", ch, NULL, car, DISTANCE_MEDIUM);
          act("Energy washes over the car, launched by $N.", driver, NULL, ch, TO_CHAR);
          act("Energy washes over the car, launched by $N.", driver, NULL, ch, TO_ROOM);
          WAIT_STATE(ch, PULSE_PER_SECOND * 5);

          if (ch->in_room == car->in_room)
          carversusman(ch, car);
        }
        else {
          act("You unleash on $N and force an accident.", ch, NULL, car, TO_CHAR);
          act("$n blasts $N with supernatural energy and forces an accident.", ch, NULL, car, TO_ROOM);
          dact("$n blasts $N with supernatural energy and forces an accident.", ch, NULL, car, DISTANCE_MEDIUM);
          act("Energy washes over the car, launched by $N, forcing an accident.", driver, NULL, ch, TO_CHAR);
          act("Energy washes over the car, launched by $N, forcing an accident", driver, NULL, ch, TO_ROOM);

          collision_injury(driver, 1);
          pile_out(driver);
          ding_car(driver);
        }
      }
    }
  }

  bool is_car(CHAR_DATA *ch) {

    if (!IS_NPC(ch))
    return FALSE;

    if (ch->your_car == NULL)
    return FALSE;

    if (ch->your_car->in_room == NULL)
    return FALSE;

    if (IS_NPC(ch->your_car))
    return FALSE;

    return TRUE;
  }

  int vehicle_cost(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0)
    return obj->value[0];

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return ch->pcdata->garage_cost[i];
    }
    return 0;
  }

  int vehicle_typetwo(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0)
    return obj->value[2];

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return ch->pcdata->garage_typetwo[i];
    }
    return 0;
  }
  int vehicle_typeone(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0)
    return obj->value[1];

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return ch->pcdata->garage_typeone[i];
    }
    return 0;
  }

  bool fast_car(CHAR_DATA *ch) {
    if (IS_FLAG(ch->comm, COMM_SLOW))
    return FALSE;

    if ((ch->pcdata->travel_type == TRAVEL_CPASSENGER || ch->pcdata->travel_type == TRAVEL_BPASSENGER) && ch->pcdata->travel_time > 0) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;
        vch = CH(d);
        if (vch == NULL || IS_NPC(vch))
        continue;
        if (vch->in_room != ch->in_room) {
          continue;
        }
        if (ch == vch)
        continue;

        if (vch->pcdata->travel_type == TRAVEL_BIKE || vch->pcdata->travel_type == TRAVEL_CAR)
        return fast_car(vch);
      }
    }
    int typeone = vehicle_typeone(ch);
    int typetwo = vehicle_typetwo(ch);

    if (typeone != CAR_SPORT && typeone != CAR_SPORTSBIKE && typetwo != CAR_SPORT && typetwo != CAR_SPORTSBIKE)
    return FALSE;

    if (carqualitylevel(vehicle_cost(ch), typeone, typetwo) < 5)
    return FALSE;

    return TRUE;
  }

  bool has_active_vehicle(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0)
    return TRUE;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return TRUE;
    }
    return FALSE;
  }

  int vehicle_location(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0)
    return obj->value[4];

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      return ch->pcdata->garage_location[i];
    }
    return 0;
  }

  void set_vehicle_location(CHAR_DATA *ch, int val) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0)
    obj->value[4] = val;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      ch->pcdata->garage_location[i] = val;
    }
  }

  void unplace_car(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *room = ch->in_room;

    if (room == NULL)
    return;

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(vehicle_lplate(ch), room->vehicle_lplates[i])) {
        free_string(room->vehicle_names[i]);
        room->vehicle_names[i] = str_dup("");
        room->vehicle_cost[i] = 0;
      }
    }
  }

  void place_car(CHAR_DATA *ch, int location) {
    if (location == 0)
    return;

    ROOM_INDEX_DATA *room = get_room_index(location);
    if (room == NULL)
    return;

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(vehicle_lplate(ch), room->vehicle_lplates[i]))
      return;
    }

    for (int i = 0; i < 10; i++) {
      if (room->vehicle_cost[i] == 0) {
        free_string(room->vehicle_names[i]);
        room->vehicle_names[i] = str_dup(vehicle_name(ch));
        free_string(room->vehicle_lplates[i]);
        room->vehicle_lplates[i] = str_dup(vehicle_lplate(ch));
        free_string(room->vehicle_descs[i]);
        room->vehicle_descs[i] = str_dup(vehicle_desc(ch));
        free_string(room->vehicle_owners[i]);
        room->vehicle_owners[i] = str_dup(ch->name);
        room->vehicle_cost[i] = vehicle_cost(ch);
        room->vehicle_typeone[i] = vehicle_typeone(ch);
        room->vehicle_typetwo[i] = vehicle_typetwo(ch);
        return;
      }
    }
  }
  void park_car(CHAR_DATA *ch, int location) {
    if (location == 0)
    return;

    ROOM_INDEX_DATA *desti = get_room_index(location);

    if (desti->area->world > WORLD_EARTH && desti->area->world <= WORLD_HELL && vehicle_typeone(ch) != CAR_HORSE) {
      switch_vehicle(ch, TRUE);
    }
    if (desti->area->vnum == 13 && vehicle_typeone(ch) == CAR_HORSE) {
      switch_vehicle(ch, FALSE);
    }

    OBJ_DATA *obj;
    place_car(ch, location);
    ch->pcdata->last_drove = location;

    if (!IS_NPC(ch) && !IS_NPC(ch) && !is_ghost(ch) && in_haven(desti))
    last_public(ch, location);

    if (!in_haven(get_room_index(location)) && (get_room_index(location)->sector_type != SECT_STREET || get_room_index(location)->area->vnum > 25))
    return;
    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0) {
      obj->value[4] = location;
      return;
    }
    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE) {
        ch->pcdata->garage_location[i] = location;
        return;
      }
    }
  }

  int safety_bonus(CHAR_DATA *ch) {
    if (!has_active_vehicle(ch))
    return 0;

    if (vehicle_typeone(ch) != CAR_SAFE && vehicle_typetwo(ch) != CAR_SAFE)
    return 0;

    return carqualitylevel(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch));
  }

  int toughness_bonus(CHAR_DATA *ch) {
    if (!has_active_vehicle(ch))
    return 0;

    if (vehicle_typeone(ch) != CAR_TOUGH && vehicle_typetwo(ch) != CAR_TOUGH)
    return 0;

    return carqualitylevel(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch));
  }

  int speed_bonus(CHAR_DATA *ch) {
    if (!has_active_vehicle(ch))
    return 0;

    if (vehicle_typeone(ch) != CAR_SPORT && vehicle_typetwo(ch) != CAR_SPORTSBIKE && vehicle_typeone(ch) != CAR_SPORTSBIKE && vehicle_typetwo(ch) != CAR_SPORT)
    return 0;

    return carqualitylevel(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch));
  }

  int status_bonus(CHAR_DATA *ch) {
    if (!has_active_vehicle(ch))
    return 0;

    if (vehicle_typeone(ch) != CAR_LUXURY && vehicle_typetwo(ch) != CAR_LUXURY)
    return 0;

    return carqualitylevel(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch));
  }

  int cruiser_bonus(CHAR_DATA *ch) {
    if (!has_active_vehicle(ch))
    return 0;

    if (vehicle_typeone(ch) != CAR_CRUISER && vehicle_typetwo(ch) != CAR_CRUISER)
    return 0;

    return carqualitylevel(vehicle_cost(ch), vehicle_typeone(ch), vehicle_typetwo(ch));
  }

  int stakeout_bonus(CHAR_DATA *ch) {
    if (ch->in_room == NULL)
    return 0;

    for (int i = 0; i < 10; i++) {
      if (!str_cmp(ch->name, ch->in_room->vehicle_owners[i]) && ch->in_room->vehicle_cost[i] > 0) {
        if (ch->in_room->vehicle_typeone[i] == CAR_VAN || ch->in_room->vehicle_typetwo[i] == CAR_VAN) {
          if (carqualitylevel(ch->in_room->vehicle_cost[i], ch->in_room->vehicle_typeone[i], ch->in_room->vehicle_typetwo[i]) >= 5)
          return 4;
        }
        return 2;
      }
    }
    return 0;
  }

  int carcost(CHAR_DATA *ch, int point) {
    int amount = ch->pcdata->garage_cost[point];
    if (ch->pcdata->garage_typeone[point] == CAR_HORSE) {
      amount = amount * (100 + soc_wealth_mod(ch, FALSE)*10) / 100;

    }
    else {
      amount = amount * (100 + soc_wealth_mod(ch, TRUE)*10) / 100;

    }
    return amount;
  }

  int garage_charge(CHAR_DATA *ch) {
    int maxcar = -1;
    int maxval = 0;
    int cost = 0;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > maxval) {
        maxval = carcost(ch, i);
        maxcar = i;
      }
    }
    cost += maxval;
    for (int i = 0; i < 10; i++) {
      if (i != maxcar && ch->pcdata->garage_status[i] == GARAGE_LOANED)
      cost += carcost(ch, i);
      else if (i != maxcar) {
        int wealth = get_skill(ch, SKILL_WEALTH);
        if (wealth >= 5)
        cost += carcost(ch, i) / 10;
        else if (wealth >= 4)
        cost += carcost(ch, i) / 5;
        else if (wealth >= 3)
        cost += carcost(ch, i) / 4;
        else if (wealth >= 2)
        cost += carcost(ch, i) / 3;
        else if (wealth >= 1)
        cost += carcost(ch, i) / 2;
        else
        cost += carcost(ch, i) * 3 / 4;
      }
    }
    cost /= 200;
    if(cost > 100 && college_group(ch, FALSE) == COLLEGE_PREP)
    {
      cost = UMAX(100, cost-100);
    }
    return cost;
  }

  void pile_out(CHAR_DATA *ch) {
    ch->pcdata->last_drove = ch->your_car->in_room->vnum;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL)
      continue;
      if (vch->in_room != ch->in_room) {
        continue;
      }
      if (ch == vch)
      continue;

      char_from_room(vch);
      char_to_room(vch, get_room_index(ch->your_car->in_room->vnum));

      vch->pcdata->travel_time = -1;
      vch->pcdata->travel_to = -1;
      vch->pcdata->travel_type = -1;
      vch->pcdata->move_damage = 0;
    }
    char_from_room(ch);
    char_to_room(ch, ch->your_car->in_room);
    if (ch->your_car != NULL) {
      char_from_room(ch->your_car);
      char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
      ch->your_car->ttl = 2;
      ch->your_car->wounds = 4;
    }
    ch->pcdata->travel_time = -1;
    ch->pcdata->travel_to = -1;
    ch->pcdata->travel_type = -1;
    ch->pcdata->move_damage = 0;
  }

  void ding_car(CHAR_DATA *ch) {
    OBJ_DATA *obj;

    obj = get_held(ch, ITEM_KEY);

    if (obj != NULL && obj->value[0] > 0 && obj->value[3] == 0)
    obj->value[3] = 1;
    else {
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE) {
          ch->pcdata->garage_status[i] = GARAGE_REPAIR;
          ch->pcdata->garage_timer[i] = (60 * 24 * 2);
        }
      }
    }
  }

  void have_accident(CHAR_DATA *ch) {
    send_to_char("You have an accident.\n\r", ch);
    act("You have an accident.", ch, NULL, NULL, TO_ROOM);
    act("$n has an accident.", ch->your_car, NULL, NULL, TO_ROOM);
    dact("$n has an accident.", ch->your_car, NULL, NULL, DISTANCE_FAR);
    ding_car(ch);
    pile_out(ch);
  }

  void accident_check(CHAR_DATA *ch) {
    if (IS_FLAG(ch->comm, COMM_STALL))
    return;

    if (ch->in_room != NULL && ch->in_room->vnum >= 19000 && ch->in_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && driving_area(get_room_index(ch->pcdata->travel_to)) && driving_area(get_room_index(ch->pcdata->travel_from)) && driving_area(ch->your_car->in_room) && ch->in_room->vnum != ch->your_car->in_room->vnum) {

      if (under_opression(ch) && number_percent() % 38 == 0) {
        have_accident(ch);
        return;
      }
      if (!mist_room(ch->your_car->in_room))
      return;

      if (IS_FLAG(ch->comm, COMM_SLOW) || safety_bonus(ch) >= 5)
      return;

      if (number_percent() % 11 == 0 && number_range(1, 8) > safety_bonus(ch))
      have_accident(ch);
    }
  }

  _DOFUN(do_garage) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];

    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);

    if (is_helpless(ch) || is_ghost(ch) || in_fight(ch) || room_hostile(ch->in_room)) {
      send_to_char("Not now.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "drive")) {
      if (!can_logoff(ch)) {
        send_to_char("You have to be at home to do that.\n\r", ch);
        return;
      }
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage drive(1-10)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }
      if (ch->pcdata->garage_status[num] != GARAGE_GARAGED) {
        send_to_char("That can't be driven at the moment, repair or reclaim it first.\n\r", ch);
        return;
      }
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
        ch->pcdata->garage_status[i] = GARAGE_GARAGED;
      }
      ch->pcdata->garage_status[num] = GARAGE_ACTIVE;
      printf_to_char(ch, "You now drive %s\n\r", ch->pcdata->garage_name[num]);
      return;
    }
    if (!str_cmp(arg1, "view")) {
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->garage_cost[i] > 0) {
          if (ch->pcdata->garage_typeone[i] == CAR_HORSE) {
            printf_to_char(ch, "`c%d`W)`x %s\n`cCost:`x `g$%d`x \t`cType:`x %s %s \t`cStatus:`x %s\n%s\n\n\r", i + 1, ch->pcdata->garage_name[i], ch->pcdata->garage_cost[i], carqualityname(ch->pcdata->garage_cost[i], ch->pcdata->garage_typeone[i], ch->pcdata->garage_typetwo[i]), cartypename(ch->pcdata->garage_typeone[i], ch->pcdata->garage_typetwo[i]), carstatusname(ch->pcdata->garage_status[i]), ch->pcdata->garage_desc[i]);
          }
          else {
            printf_to_char(
            ch, "`c%d`W)`x %s\n`cCost:`x `g$%d`x \t`cType:`x %s %s \t`cStatus:`x%s\n%s\n`W|`R%s`W|`x\n\n\r", i + 1, ch->pcdata->garage_name[i], ch->pcdata->garage_cost[i], carqualityname(ch->pcdata->garage_cost[i], ch->pcdata->garage_typeone[i], ch->pcdata->garage_typetwo[i]), cartypename(ch->pcdata->garage_typeone[i], ch->pcdata->garage_typetwo[i]), carstatusname(ch->pcdata->garage_status[i]), ch->pcdata->garage_desc[i], ch->pcdata->garage_lplate[i]);
          }
        }
      }
      return;
    }
    if (!str_cmp(arg1, "unlease")) {
      if (!can_logoff(ch)) {
        send_to_char("You have to be at home to do that.\n\r", ch);
        return;
      }
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage unlease (1-10)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }
      if (ch->pcdata->garage_status[num] != GARAGE_ACTIVE && ch->pcdata->garage_status[num] != GARAGE_GARAGED) {
        send_to_char("You can't do that at the moment.\n\r", ch);
        return;
      }

      ch->pcdata->garage_cost[num] = 0;
      printf_to_char(ch, "You return %s\n\r", ch->pcdata->garage_name[num]);
      return;
    }
    if (!str_cmp(arg1, "name")) {
      if (!can_logoff(ch)) {
        send_to_char("You have to be at home to do that.\n\r", ch);
        return;
      }
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage name (1-10) (argument)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }
      if (safe_strlen(argument) < 4) {
        send_to_char("Syntax: garage name (1-10) (argument)\n\r", ch);
        return;
      }
      free_string(ch->pcdata->garage_name[num]);
      ch->pcdata->garage_name[num] = str_dup(argument);
      printf_to_char(ch, "You now drive %s\n\r", ch->pcdata->garage_name[num]);
      return;
    }
    if (!str_cmp(arg1, "describe")) {
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage describe (1-10)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }

      string_append(ch, &ch->pcdata->garage_desc[num]);
      return;
    }
    if (!str_cmp(arg1, "lease")) {
      argument = one_argument_nouncap(argument, arg3);

      int value = atoi(arg2);
      int typeone = cartype(arg3);
      int typetwo = cartype(argument);

      if (typeone == -1) {
        send_to_char("Valid types are: Safe, sport, Luxury, Van, Tough, Sportsbike, Cruiser, Horse\n\r", ch);
        return;
      }
      if (!valid_carpair(typeone, typetwo)) {
        send_to_char("Invalid type pairing.\n\r", ch);
        return;
      }
      if (value < 2500 && typeone != CAR_SPORTSBIKE && typeone != CAR_CRUISER) {
        send_to_char("The cheapest car is $2500\n\r", ch);
        return;
      }
      if (value < 1500) {
        send_to_char("The cheapest motorcycle is $1500\n\r", ch);
        return;
      }
      value /= 2;
      if (ch->pcdata->total_money < value) {
        printf_to_char(ch, "You'd need at least $%d in the bank for that.\n\r", value / 100);
        return;
      }
      for (int i = 0; i < 10; i++) {
        if (ch->pcdata->garage_cost[i] == 0) {
          ch->pcdata->garage_cost[i] = value * 2;
          ch->pcdata->garage_status[i] = GARAGE_GARAGED;
          ch->pcdata->garage_typeone[i] = typeone;
          ch->pcdata->garage_typetwo[i] = typetwo;
          ch->pcdata->garage_location[i] = 0;
          free_string(ch->pcdata->garage_name[i]);
          ch->pcdata->garage_name[i] = str_dup(default_carname(typeone));
          free_string(ch->pcdata->garage_lplate[i]);
          ch->pcdata->garage_lplate[i] = str_dup(new_lplate(ch));
          ch->pcdata->total_money -= value;
          printf_to_char(ch, "You lease a %s for $%d upfront.\n\r", default_carname(typeone), value / 100);
          return;
        }
      }
      send_to_char("You already have too many vehicles in your garage.\n\r", ch);
    }
    if (!str_cmp(arg1, "loan")) {
      OBJ_DATA *obj;
      EXTRA_DESCR_DATA *ed;
      if (!can_logoff(ch)) {
        send_to_char("You have to be at home to do that.\n\r", ch);
        return;
      }
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage loan (1-10)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }
      if (ch->pcdata->garage_status[num] != GARAGE_ACTIVE && ch->pcdata->garage_status[num] != GARAGE_GARAGED) {
        send_to_char("You can't loan that vehicle out at the moment.\n\r", ch);
        return;
      }
      ch->pcdata->garage_status[num] = GARAGE_LOANED;
      obj = create_object(get_obj_index(40), 0);
      obj_to_char(obj, ch);
      free_string(obj->material);
      obj->material = str_dup(ch->name);

      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+lplate", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+lplate");
        ed->next = obj->extra_descr;
        obj->extra_descr = ed;
        free_string(obj->extra_descr->description);
        obj->extra_descr->description = str_dup(ch->pcdata->garage_lplate[num]);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(ch->pcdata->garage_lplate[num]);
      }
      char buf[MSL];
      if (ch->pcdata->garage_typeone[num] != CAR_HORSE)
      sprintf(buf, "A set of keys for %s\n", ch->pcdata->garage_name[num]);
      else
      sprintf(buf, "The reins for %s\n", ch->pcdata->garage_name[num]);

      ed = NULL;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("all", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("all");
        ed->next = obj->extra_descr;
        obj->extra_descr = ed;
        free_string(obj->extra_descr->description);
        obj->extra_descr->description = str_dup(buf);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(buf);
      }
      ed = NULL;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+carname", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+carname");
        ed->next = obj->extra_descr;
        obj->extra_descr = ed;
        free_string(obj->extra_descr->description);
        obj->extra_descr->description = str_dup(ch->pcdata->garage_name[num]);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(ch->pcdata->garage_name[num]);
      }
      ed = NULL;
      for (ed = obj->extra_descr; ed; ed = ed->next) {
        if (is_name("+cardesc", ed->keyword))
        break;
      }

      if (!ed) {
        ed = new_extra_descr();
        ed->keyword = str_dup("+cardesc");
        ed->next = obj->extra_descr;
        obj->extra_descr = ed;
        free_string(obj->extra_descr->description);
        obj->extra_descr->description = str_dup(ch->pcdata->garage_desc[num]);
      }
      else {
        free_string(ed->description);
        ed->description = str_dup(ch->pcdata->garage_desc[num]);
      }

      obj->value[0] = ch->pcdata->garage_cost[num];
      obj->value[1] = ch->pcdata->garage_typeone[num];
      obj->value[2] = ch->pcdata->garage_typetwo[num];
      obj->value[4] = ch->pcdata->garage_location[num];
      if (ch->pcdata->garage_typeone[num] == CAR_HORSE) {
        printf_to_char(ch, "You get the reins for %s ready to loan to someone.\n\r", ch->pcdata->garage_name[num]);
        free_string(obj->short_descr);
        obj->short_descr = str_dup("some reins");
        free_string(obj->description);
        obj->description = str_dup("A set of reins");
        free_string(obj->name);
        obj->name = str_dup("set reins");
      }
      else
      printf_to_char(ch, "You get the keys for %s ready to loan to someone.\n\r", ch->pcdata->garage_name[num]);
      return;
    }
    if (!str_cmp(arg1, "reclaim")) {
      OBJ_DATA *obj;
      if (!can_logoff(ch)) {
        send_to_char("You have to be at home to do that.\n\r", ch);
        return;
      }
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage reclaim (1-10)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }
      if (ch->pcdata->garage_status[num] != GARAGE_LOANED && ch->pcdata->garage_status[num] != GARAGE_LOANEDDAMAGED) {
        send_to_char("That vehicle isn't loaned out.\n\r", ch);
        return;
      }
      bool haskey = FALSE;
      OBJ_DATA *obj_next;
      for (obj = ch->carrying; haskey == FALSE && obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        if (obj->item_type == ITEM_KEY && is_key_for(obj, ch->pcdata->garage_lplate[num])) {
          haskey = TRUE;
          if (obj->value[3] == 1)
          ch->pcdata->garage_status[num] = GARAGE_LOANEDDAMAGED;

          extract_obj(obj);
          break;
        }
      }
      if (haskey == TRUE) {
        if (ch->pcdata->garage_status[num] == GARAGE_LOANED)
        ch->pcdata->garage_status[num] = GARAGE_GARAGED;
        else {
          ch->pcdata->garage_status[num] = GARAGE_REPAIR;
          ch->pcdata->garage_timer[num] = (60 * 24 * 2);
        }
        printf_to_char(ch, "You put %s back in your garage.\n\r", ch->pcdata->garage_name[num]);
        return;
      }
      else {
        send_to_char("You don't have the key for that, try garage recover (number) instead to pay to have it returned.\n\r", ch);
        return;
        CHAR_DATA *victim;
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          if (d->connected != CON_PLAYING)
          continue;
          victim = CH(d);
          if (victim == NULL)
          continue;
          if (IS_NPC(victim))
          continue;
          haskey = FALSE;
          for (obj = victim->carrying; haskey == FALSE && obj != NULL;
          obj = obj_next) {
            obj_next = obj->next_content;
            if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
            continue;

            if (obj->item_type == ITEM_KEY && is_key_for(obj, ch->pcdata->garage_lplate[num])) {
              haskey = TRUE;
              if (obj->value[3] == 1)
              ch->pcdata->garage_status[num] = GARAGE_LOANEDDAMAGED;
              extract_obj(obj);
              break;
            }
          }
        }
        if (ch->pcdata->garage_status[num] == GARAGE_LOANED) {
          ch->pcdata->garage_status[num] = GARAGE_RECOVER;
          ch->pcdata->garage_timer[num] = (60 * 24);
        }
        else {
          ch->pcdata->garage_status[num] = GARAGE_RECOVERREPAIR;
          ch->pcdata->garage_timer[num] = (60 * 24);
        }
        ch->pcdata->total_money -= 20000;
        printf_to_char(ch, "You pay to have your %s recovered.\n\r", ch->pcdata->garage_name[num]);
        return;
      }
    }
    if (!str_cmp(arg1, "recover")) {
      OBJ_DATA *obj;
      if (!can_logoff(ch)) {
        send_to_char("You have to be at home to do that.\n\r", ch);
        return;
      }
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage reclaim (1-10)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }
      if (ch->pcdata->garage_status[num] != GARAGE_LOANED && ch->pcdata->garage_status[num] != GARAGE_LOANEDDAMAGED) {
        send_to_char("That vehicle isn't loaned out.\n\r", ch);
        return;
      }
      bool haskey = FALSE;
      OBJ_DATA *obj_next;
      for (obj = ch->carrying; haskey == FALSE && obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
        continue;

        if (obj->item_type == ITEM_KEY && is_key_for(obj, ch->pcdata->garage_lplate[num])) {
          haskey = TRUE;
          if (obj->value[3] == 1)
          ch->pcdata->garage_status[num] = GARAGE_LOANEDDAMAGED;

          extract_obj(obj);
          break;
        }
      }
      if (haskey == TRUE) {
        send_to_char("You already have the key for that, try garage reclaim (number) instead.\n\r", ch);
        return;
      }
      else {
        CHAR_DATA *victim;
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          if (d->connected != CON_PLAYING)
          continue;
          victim = CH(d);
          if (victim == NULL)
          continue;
          if (IS_NPC(victim))
          continue;
          haskey = FALSE;
          for (obj = victim->carrying; haskey == FALSE && obj != NULL;
          obj = obj_next)
          for (obj = victim->carrying; haskey == FALSE && obj != NULL;
          obj = obj_next) {
            obj_next = obj->next_content;
            if (IS_SET(obj->extra_flags, ITEM_WARDROBE))
            continue;

            if (obj->item_type == ITEM_KEY && is_key_for(obj, ch->pcdata->garage_lplate[num])) {
              haskey = TRUE;
              if (obj->value[3] == 1)
              ch->pcdata->garage_status[num] = GARAGE_LOANEDDAMAGED;
              extract_obj(obj);
              break;
            }
          }
        }
        if (ch->pcdata->garage_status[num] == GARAGE_LOANED) {
          ch->pcdata->garage_status[num] = GARAGE_RECOVER;
          ch->pcdata->garage_timer[num] = (60 * 24);
        }
        else {
          ch->pcdata->garage_status[num] = GARAGE_RECOVERREPAIR;
          ch->pcdata->garage_timer[num] = (60 * 24);
        }
        ch->pcdata->total_money -= 10000;
        printf_to_char(ch, "You pay to have your %s recovered.\n\r", ch->pcdata->garage_name[num]);
        return;
      }
    }
    if (!str_cmp(arg1, "locate")) {
      if (!is_number(arg2) || atoi(arg2) > 10 || atoi(arg2) < 1) {
        send_to_char("Syntax: garage reclaim (1-10)\n\r", ch);
        return;
      }
      int num = atoi(arg2);
      num--;
      if (ch->pcdata->garage_cost[num] <= 0) {
        send_to_char("No such vehicle in your garage.\n\r", ch);
        return;
      }
      ch->pcdata->total_money -= 500;
      if (ch->pcdata->garage_location[num] == 0) {
        send_to_char("That vehicle is in your garage.\n\r", ch);
        return;
      }
      ROOM_INDEX_DATA *orig = ch->in_room;
      ROOM_INDEX_DATA *newroom = get_room_index(ch->pcdata->garage_location[num]);
      if (newroom == NULL) {
        ch->pcdata->garage_location[num] = 0;
        send_to_char("That vehicle is in your garage.\n\r", ch);
        return;
      }
      char_from_room(ch);
      char_to_room(ch, newroom);
      maketownmap(ch);
      char_from_room(ch);
      char_to_room(ch, orig);
      return;
    }

    send_to_char("Garage lease/unlease/view/drive/loan/locate/reclaim/name/describe\n\r", ch);
  }

  bool valid_parking_spot(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return FALSE;
    int count = 0;
    for (int i = 0; i < 10; i++) {
      if (room->vehicle_cost[i] > 0)
      count++;
    }
    if (count >= 8)
    return FALSE;

    return TRUE;
  }

  _DOFUN(do_park) {
    char buf[MSL];

    if (ch->in_room != NULL && ch->in_room->vnum >= 19000 && ch->in_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && driving_area(get_room_index(ch->pcdata->travel_to)) && driving_area(get_room_index(ch->pcdata->travel_from)) && driving_area(ch->your_car->in_room) && ch->in_room->vnum != ch->your_car->in_room->vnum) {
      if (!valid_parking_spot(ch->your_car->in_room)) {
        send_to_char("You can't park here.\n\r", ch);
        return;
      }

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;
        vch = CH(d);
        if (vch == NULL)
        continue;
        if (vch->in_room != ch->in_room) {
          continue;
        }
        if (ch == vch)
        continue;

        if (vch->pcdata->travel_type == TRAVEL_CPASSENGER) {
          char_from_room(vch);
          char_to_room(vch, get_room_index(ch->your_car->in_room->vnum));
          if (!is_ghost(vch))
          act("$n steps out of a car.", ch, NULL, NULL, TO_ROOM);
        }
        else if (vch->pcdata->travel_type == TRAVEL_HPASSENGER) {
          char_from_room(vch);
          char_to_room(vch, get_room_index(ch->your_car->in_room->vnum));
          if (!is_ghost(vch))
          act("$n gets off a horse.", ch, NULL, NULL, TO_ROOM);
        }
        else {
          char_from_room(vch);
          char_to_room(vch, get_room_index(ch->your_car->in_room->vnum));
          if (!is_ghost(vch))
          act("$n gets off a motorcycle.", ch, NULL, NULL, TO_ROOM);
        }
        vch->pcdata->travel_time = -1;
        vch->pcdata->travel_to = -1;
        vch->pcdata->travel_type = -1;
        vch->pcdata->move_damage = 0;
        vch->pcdata->last_drove = vch->in_room->vnum;
        do_function(vch, &do_look, "auto");
      }
      char_from_room(ch);
      char_to_room(ch, ch->your_car->in_room);
      if (ch->pcdata->travel_type == TRAVEL_CAR)
      sprintf(buf, "$n steps out of $s %s.", vehicle_name(ch));
      else
      sprintf(buf, "$n gets off $s %s.", vehicle_name(ch));
      act(buf, ch, NULL, NULL, TO_ROOM);
      if (ch->your_car != NULL) {
        char_from_room(ch->your_car);
        char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
        ch->your_car->ttl = 2;
        ch->your_car->wounds = 4;
      }
      park_car(ch, ch->in_room->vnum);
      ch->pcdata->travel_time = -1;
      ch->pcdata->travel_to = -1;
      ch->pcdata->travel_type = -1;
      ch->pcdata->move_damage = 0;
      ch->pcdata->last_drove = ch->in_room->vnum;
      do_function(ch, &do_look, "auto");
      return;
    }
    send_to_char("You can't do that.\n\r", ch);
  }

  void force_park(CHAR_DATA *car) {
    CHAR_DATA *ch = car->your_car;
    if (ch->in_room != NULL && ch->in_room->vnum >= 19000 && ch->in_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && driving_area(get_room_index(ch->pcdata->travel_to)) && driving_area(get_room_index(ch->pcdata->travel_from)) && driving_area(ch->your_car->in_room) && ch->in_room->vnum != ch->your_car->in_room->vnum) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        CHAR_DATA *vch;
        DESCRIPTOR_DATA *d = *it;
        vch = CH(d);
        if (vch == NULL)
        continue;
        if (vch->in_room != ch->in_room) {
          continue;
        }
        if (ch == vch)
        continue;
        {
          char_from_room(vch);
          char_to_room(vch, get_room_index(ch->your_car->in_room->vnum));
        }
        vch->pcdata->travel_time = -1;
        vch->pcdata->travel_to = -1;
        vch->pcdata->travel_type = -1;
        vch->pcdata->move_damage = 0;
        do_function(vch, &do_look, "auto");
        vch->pcdata->last_drove = vch->in_room->vnum;
      }
      char_from_room(ch);
      char_to_room(ch, ch->your_car->in_room);
      if (ch->your_car != NULL) {
        char_from_room(ch->your_car);
        char_to_room(ch->your_car, get_room_index(ROOM_VNUM_LIMBO));
        ch->your_car->ttl = 2;
        ch->your_car->wounds = 4;
      }
      park_car(ch, ch->in_room->vnum);
      ch->pcdata->travel_time = -1;
      ch->pcdata->travel_to = -1;
      ch->pcdata->travel_type = -1;
      ch->pcdata->move_damage = 0;
      ch->pcdata->last_drove = ch->in_room->vnum;
      return;
    }
    send_to_char("You can't do that.\n\r", ch);
  }

  void forcecarkeys(CHAR_DATA *ch, CHAR_DATA *forcer) {
    OBJ_DATA *obj;
    EXTRA_DESCR_DATA *ed;
    int num = 0;

    for (int i = 0; i < 10; i++) {
      if (ch->pcdata->garage_cost[i] > 0 && ch->pcdata->garage_status[i] == GARAGE_ACTIVE)
      num = i;
    }

    if (ch->pcdata->garage_cost[num] <= 0) {
      send_to_char("They don't seem to have any keys on them.\n\r", forcer);
      return;
    }
    if (ch->pcdata->garage_status[num] != GARAGE_ACTIVE) {
      send_to_char("They don't seem to have any keys on them.\n\r", forcer);
      return;
    }

    ch->pcdata->garage_status[num] = GARAGE_LOANED;
    obj = create_object(get_obj_index(40), 0);
    obj_to_char(obj, ch);
    free_string(obj->material);
    obj->material = str_dup(ch->name);

    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+lplate", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("+lplate");
      ed->next = obj->extra_descr;
      obj->extra_descr = ed;
      free_string(obj->extra_descr->description);
      obj->extra_descr->description = str_dup(ch->pcdata->garage_lplate[num]);
      obj->extra_descr->description = str_dup(ch->pcdata->garage_lplate[num]);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(ch->pcdata->garage_lplate[num]);
    }
    char buf[MSL];
    sprintf(buf, "A set of keys for %s\n", ch->pcdata->garage_name[num]);
    ed = NULL;
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("all", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("all");
      ed->next = obj->extra_descr;
      obj->extra_descr = ed;
      free_string(obj->extra_descr->description);
      obj->extra_descr->description = str_dup(buf);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(buf);
    }
    ed = NULL;
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+carname", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("+carname");
      ed->next = obj->extra_descr;
      obj->extra_descr = ed;
      free_string(obj->extra_descr->description);
      obj->extra_descr->description = str_dup(ch->pcdata->garage_name[num]);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(ch->pcdata->garage_name[num]);
    }
    ed = NULL;
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("+cardesc", ed->keyword))
      break;
    }

    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = str_dup("+cardesc");
      ed->next = obj->extra_descr;
      obj->extra_descr = ed;
      free_string(obj->extra_descr->description);
      obj->extra_descr->description = str_dup(ch->pcdata->garage_desc[num]);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(ch->pcdata->garage_desc[num]);
    }

    obj->value[0] = ch->pcdata->garage_cost[num];
    obj->value[1] = ch->pcdata->garage_typeone[num];
    obj->value[2] = ch->pcdata->garage_typetwo[num];
    obj->value[4] = ch->pcdata->garage_location[num];
    printf_to_char(ch, "The keys for %s are stolen.\n\r", ch->pcdata->garage_name[num]);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    act("Searching $n reveals a set of keys.", ch, NULL, NULL, TO_ROOM);
  }

  _DOFUN(do_runinto) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    CHAR_DATA *victim;

    if (ch->in_room != NULL && ch->in_room->vnum >= 19000 && ch->in_room->vnum <= 19099 && ch->your_car != NULL && ch->your_car->in_room != NULL && driving_area(get_room_index(ch->pcdata->travel_to)) && driving_area(get_room_index(ch->pcdata->travel_from)) && driving_area(ch->your_car->in_room) && ch->in_room->vnum != ch->your_car->in_room->vnum) {
      if (!str_cmp(arg1, "wall")) {
        have_accident(ch);
        return;
      }

      if (vehicle_typeone(ch) == CAR_SPORTSBIKE || vehicle_typeone(ch) == CAR_CRUISER || vehicle_typeone(ch) == CAR_HORSE) {
        send_to_char("That would be a bit suicidal.\n\r", ch);
        return;
      }

      ROOM_INDEX_DATA *orig;
      orig = ch->in_room;
      char_from_room(ch);
      char_to_room(ch, ch->your_car->in_room);
      victim = get_char_room(ch, NULL, arg1);
      if (victim == NULL)
      victim = get_char_distance(ch, arg1, 4);
      char_from_room(ch);
      char_to_room(ch, orig);
      if (victim == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      if (!IS_NPC(victim) && ch->played / 3600 > 100) {
        char_from_room(ch->your_car);
        char_to_room(ch->your_car, victim->in_room);
        rundown(ch, victim, TRUE);
        if (toughness_bonus(ch) + safety_bonus(ch) < number_range(0, 6))
        have_accident(ch);
        return;
      }
      if (victim->in_room->sector_type != SECT_STREET) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }

      if (is_car(victim)) {

        char_from_room(ch->your_car);
        char_to_room(ch->your_car, victim->in_room);
        act("$n collides with $N", ch->your_car, NULL, victim, TO_ROOM);
        dact("$n collides with $N", ch->your_car, NULL, victim, DISTANCE_FAR);
        act("You collide with another vehicle.", ch, NULL, NULL, TO_CHAR);
        act("You collide with another vehicle.", ch, NULL, NULL, TO_ROOM);
        act("Another vehicle collides with you.", victim->your_car, NULL, NULL, TO_CHAR);
        act("Another vehicle collides with you.", victim->your_car, NULL, NULL, TO_ROOM);

        if (vehicle_typeone(victim->your_car) == CAR_SPORTSBIKE || vehicle_typeone(victim->your_car) == CAR_CRUISER || vehicle_typeone(victim->your_car) == CAR_HORSE) {
          collision_injury(victim->your_car, 2);

          if (toughness_bonus(ch) + safety_bonus(ch) < number_range(1, 10))
          have_accident(ch);
        }
        else {
          int chdeadly = toughness_bonus(ch) + speed_bonus(ch);
          int vicdeadly =
          toughness_bonus(victim->your_car) + speed_bonus(victim->your_car);

          if (IS_FLAG(ch->comm, COMM_SLOW))
          chdeadly /= 2;
          if (IS_FLAG(victim->your_car->comm, COMM_SLOW))
          vicdeadly /= 2;

          if (IS_FLAG(ch->comm, COMM_STALL))
          chdeadly /= 4;
          if (IS_FLAG(victim->your_car->comm, COMM_STALL))
          vicdeadly /= 4;

          chdeadly += number_range(-2, 5);

          if (chdeadly > vicdeadly) {
            if (safety_bonus(ch) > number_range(0, 10)) {
            }
            else
            collision_injury(ch, 1);

            if (safety_bonus(victim->your_car) > number_range(0, 10))
            collision_injury(victim->your_car, 1);
            else
            collision_injury(victim->your_car, 2);
          }
          else if (vicdeadly > chdeadly) {
            if (safety_bonus(ch) > number_range(0, 10))
            collision_injury(ch, 1);
            else
            collision_injury(ch, 2);

            if (safety_bonus(victim->your_car) > number_range(0, 10)) {
            }
            else
            collision_injury(victim->your_car, 1);
          }
          else {
            if (safety_bonus(ch) > number_range(0, 10))
            collision_injury(ch, 1);
            else
            collision_injury(ch, 2);

            if (safety_bonus(victim->your_car) > number_range(0, 10))
            collision_injury(victim->your_car, 1);
            else
            collision_injury(victim->your_car, 2);
          }
          ding_car(victim->your_car);
          pile_out(victim->your_car);
          ding_car(ch);
          pile_out(ch);
          return;
        }
      }
    }
    send_to_char("They're not here.\n\r", ch);
  }

  void dreamscape_message(CHAR_DATA *ch, int room, char *message) {
    FANTASY_TYPE *fant;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL)
      continue;
      if (IS_NPC(vch) || !is_dreaming(vch))
      continue;

      if (vch == ch)
      continue;

      bool fanblind = FALSE;
      if ((fant = in_fantasy(vch)) != NULL) {
        if (part_of_fantasy(vch, fant)) {
          for (int i = 0; i < 25; i++) {
            if (!str_cmp(vch->name, fant->participants[i]) && fant->participant_blind[i] == TRUE)
            fanblind = TRUE;
          }
        }
      }
      if (fanblind == TRUE)
      continue;

      if (vch->pcdata->dream_room == room)
      send_to_char(message, vch);
    }
  }

  char *dream_detail(CHAR_DATA *ch, char *world, int type) {
    for (vector<FANTASY_TYPE *>::iterator it = FantasyVect.begin();
    it != FantasyVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if (!str_cmp(world, (*it)->name)) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, (*it)->participants[i])) {
            if (type == DREAM_DETAIL_SHORT)
            return (*it)->participant_shorts[i];
            if (type == DREAM_DETAIL_NAME)
            return (*it)->participant_names[i];
            if (type == DREAM_DETAIL_FAME)
            return (*it)->participant_fames[i];
            if (type == DREAM_DETAIL_EQ)
            return (*it)->participant_eq[i];
            if (type == DREAM_DETAIL_DESC)
            return (*it)->participant_descs[i];
          }
        }
      }
    }
    return "";
  }

  char *dream_introduction(CHAR_DATA *ch) {
    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) != NULL) {
      if (part_of_fantasy(ch, fant)) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, fant->participants[i]))
          return fant->participant_shorts[i];
        }
      }
    }
    if (safe_strlen(ch->pcdata->dream_intro) < 2)
    return ch->name;
    return ch->pcdata->dream_intro;
  }

  char *dream_description(CHAR_DATA *ch) {
    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) != NULL) {
      if (part_of_fantasy(ch, fant)) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, fant->participants[i]))
          return fant->participant_descs[i];
        }
      }
    }
    return "";
  }

  char *dream_fame(CHAR_DATA *ch) {
    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) != NULL) {
      if (part_of_fantasy(ch, fant)) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, fant->participants[i]))
          return fant->participant_fames[i];
        }
      }
    }
    return "";
  }
  char *dream_eq(CHAR_DATA *ch) {
    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) != NULL) {
      if (part_of_fantasy(ch, fant)) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, fant->participants[i]))
          return fant->participant_eq[i];
        }
      }
    }
    return "";
  }

  char *dream_name(CHAR_DATA *ch) {
    FANTASY_TYPE *fant;
    if ((fant = in_fantasy(ch)) != NULL) {
      if (part_of_fantasy(ch, fant)) {
        for (int i = 0; i < 200; i++) {
          if (!str_cmp(ch->name, fant->participants[i]))
          return fant->participant_names[i];
        }
      }
    }
    return ch->pcdata->dream_description;
  }

  int dream_pop(CHAR_DATA *ch) {
    int pop = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *vch;
      DESCRIPTOR_DATA *d = *it;
      vch = CH(d);
      if (vch == NULL)
      continue;
      if (IS_NPC(vch) || !is_dreaming(vch))
      continue;

      if (vch == ch)
      continue;

      if (ch->pcdata->dream_link == vch)
      pop++;

      if (ch->pcdata->dream_room > 0 && ch->pcdata->dream_room == vch->pcdata->dream_room)
      pop++;
    }
    return pop;
  }

  CHAR_DATA *get_char_dream(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    char temp[MIL];
    CHAR_DATA *wch;
    int number;
    int count;

    if (ch->pcdata->dream_room == 0 && ch->pcdata->dream_link != NULL) {
      if (is_name(argument, dream_name(ch->pcdata->dream_link)))
      return ch->pcdata->dream_link;
    }

    number = number_argument(argument, arg);
    count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      wch = *it;

      if (IS_NPC(wch))
      continue;

      if (!is_dreaming(wch))
      continue;
      if (ch->pcdata->dream_room != wch->pcdata->dream_room)
      continue;

      remove_color(temp, dream_name(wch));

      if (wch->in_room == NULL || !is_name(arg, temp))
      continue;

      if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

      if (++count == number)
      return wch;
    }
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      wch = *it;

      if (IS_NPC(wch))
      continue;

      if (!is_dreaming(wch))
      continue;
      if (ch->pcdata->dream_room != wch->pcdata->dream_room)
      continue;

      remove_color(temp, dream_introduction(wch));

      if (wch->in_room == NULL || !is_name(arg, temp))
      continue;

      if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

      if (++count == number)
      return wch;
    }
    count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      wch = *it;

      if (IS_NPC(wch))
      continue;

      if (!is_dreaming(wch))
      continue;

      remove_color(temp, dream_name(wch));

      if (wch->in_room == NULL || !is_name(arg, temp))
      continue;

      if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

      if (++count == number)
      return wch;
    }
    count = 0;
    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      wch = *it;

      if (IS_NPC(wch))
      continue;

      if (!is_dreaming(wch))
      continue;

      remove_color(temp, dream_introduction(wch));

      if (wch->in_room == NULL || !is_name(arg, temp))
      continue;

      if (IS_IMMORTAL(ch) && IS_FLAG(wch->act, PLR_SPYSHIELD))
      continue;

      if (++count == number)
      return wch;
    }

    return NULL;
  }

  bool driving_area(ROOM_INDEX_DATA *room) {
    if (in_haven(room))
    return TRUE;

    if (room->area->vnum == OTHER_FOREST_VNUM)
    return TRUE;
    if (room->area->vnum == WILDS_FOREST_VNUM)
    return TRUE;
    if (room->area->vnum == GODREALM_FOREST_VNUM)
    return TRUE;

    return FALSE;
  }

#if defined(__cplusplus)
}
#endif
