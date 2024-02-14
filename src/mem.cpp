#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include "merc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

#if defined(__cplusplus)
extern "C" {
#endif

  /*
* Globals
*/
  extern int top_reset;
  extern int top_area;
  extern int top_exit;
  extern int top_ed;
  extern int top_room;
  extern int top_mprog_index;
  extern int top_oprog_index;
  extern int top_rprog_index;

  AreaList area_free;
  EXIT_DATA *exit_free;
  ROOM_INDEX_DATA *room_index_free;
  OBJ_INDEX_DATA *obj_index_free;
  ShopList shop_free;
  MOB_INDEX_DATA *mob_index_free;
  RESET_DATA *reset_free;
  GROUP_INDEX_DATA *group_index_free;

  /*
EXTRA_DESCR_DATA        *       extra_descr_free;
HELP_DATA		*	help_free;
HELP_DATA		*	help_last;
*/
  void free_extra_descr args((EXTRA_DESCR_DATA * pExtra));
  void free_affect args((AFFECT_DATA * af));
  void free_mprog args((PROG_LIST * mp));
  void free_oprog args((PROG_LIST * op));
  void free_rprog args((PROG_LIST * rp));

  RESET_DATA *new_reset_data(void) {
    RESET_DATA *pReset;

    if (!reset_free) {
      pReset = (RESET_DATA *)alloc_perm(sizeof(*pReset));
      top_reset++;
    }
    else {
      pReset = reset_free;
      reset_free = reset_free->next;
    }

    pReset->next = NULL;
    pReset->command = 'X';
    pReset->arg1 = 0;
    pReset->arg2 = 0;
    pReset->arg3 = 0;
    pReset->arg4 = 0;

    return pReset;
  }

  void free_reset_data(RESET_DATA *pReset) {
    pReset->next = reset_free;
    reset_free = pReset;
    return;
  }

  AREA_DATA *new_area(void) {
    AREA_DATA *pArea;
    char buf[MIL];

    if (area_free.empty()) {
      pArea = (AREA_DATA *)alloc_perm(sizeof(*pArea));
      top_area++;
    }
    else {
      pArea = area_free.front();
      area_free.pop_front();
    }

    pArea->name = str_dup("New area");
    pArea->area_flags = AREA_ADDED;
    pArea->min_vnum = 0;
    pArea->max_vnum = 0;
    pArea->age = 0;
    pArea->nplayer = 0;
    pArea->empty = TRUE; /* ROM patch */
    sprintf(buf, "area%d.are", pArea->vnum);
    pArea->file_name = str_dup(buf);
    pArea->vnum = top_area - 1;
    pArea->world = 0;
    pArea->minx = 0;
    pArea->miny = 0;
    pArea->maxx = 0;
    pArea->maxy = 0;
    pArea->building = 0;

    return pArea;
  }

  void free_area(AREA_DATA *pArea) {
    free_string(pArea->name);
    free_string(pArea->file_name);

    area_free.push_back(pArea);
    return;
  }

  EXIT_DATA *new_exit(void) {
    EXIT_DATA *pExit;

    if (!exit_free) {
      pExit = (EXIT_DATA *)alloc_perm(sizeof(*pExit));
      top_exit++;
    }
    else {
      pExit = exit_free;
      exit_free = exit_free->next;
    }

    pExit->u1.to_room = NULL; /* ROM OLC */
    pExit->next_upd = NULL;
    pExit->next = NULL;
    /*  pExit->vnum         =   0;                        ROM OLC */
    pExit->exit_info = 0;
    pExit->key = 0;
    pExit->affected = NULL;
    SET_INIT(pExit->affected_by);
    pExit->keyword = &str_empty[0];
    pExit->description = &str_empty[0];
    pExit->rs_flags = 0;
    pExit->jump = 0;
    pExit->climb = 0;
    pExit->fall = 0;
    pExit->wall = 0;
    pExit->wallcondition = 0;
    pExit->doorbroken = 0;

    return pExit;
  }

  FEATURE_TYPE *new_feature(void) {
    FEATURE_TYPE *feature;

    feature = (FEATURE_TYPE *)alloc_perm(sizeof(*feature));

    feature->names = &str_empty[0];
    feature->shortdesc = &str_empty[0];
    feature->desc = &str_empty[0];
    feature->type = 0;
    feature->position = 0;
    feature->hp = 100;
    return feature;
  }

  void free_feature(FEATURE_TYPE *feature) {
    free_string(feature->names);
    free_string(feature->desc);
    free_string(feature->shortdesc);
    feature->valid = FALSE;
    return;
  }
  void free_exit(EXIT_DATA *pExit) {
    free_string(pExit->keyword);
    free_string(pExit->description);

    pExit->next = exit_free;
    exit_free = pExit;
    return;
  }

  ROOM_INDEX_DATA *new_room_index(void) {
    ROOM_INDEX_DATA *pRoom;
    int door;

    if (!room_index_free) {
      pRoom = (ROOM_INDEX_DATA *)alloc_perm(sizeof(*pRoom));
      top_room++;
    }
    else {
      pRoom = room_index_free;
      room_index_free = room_index_free->next;
    }

    pRoom->next = NULL;
    pRoom->people = new CharList;
    pRoom->contents = NULL;
    pRoom->extra_descr = NULL;
    pRoom->places = NULL;
    pRoom->area = NULL;

    for (door = 0; door < MAX_DIR; door++)
    pRoom->exit[door] = NULL;
    for (door = 0; door < 10; door++)
    pRoom->features[door] = NULL;

    pRoom->name = &str_empty[0];
    pRoom->subarea = &str_empty[0]; // Discordance
    // pRoom->player_description   =   &str_empty[0];  //playerbase descriptions
    // pRoom->player_shroud		=   &str_empty[0];  //playerbase shroud
    // descriptions
    pRoom->description = &str_empty[0];
    pRoom->shroud = &str_empty[0];
    pRoom->owner = &str_empty[0];
    for (int y = 0; y < 10; y++) {
      pRoom->vehicle_names[y] = str_dup("");
      pRoom->vehicle_lplates[y] = str_dup("");
      pRoom->vehicle_descs[y] = str_dup("");
      pRoom->vehicle_owners[y] = str_dup("");
      pRoom->vehicle_cost[y] = 0;
      pRoom->vehicle_typeone[y] = 0;
      pRoom->vehicle_typetwo[y] = 0;
    }
    for (int y = 0; y < 3; y++) {
      pRoom->smell_desc[y] = str_dup("");
      pRoom->smell_dir[y] = 0;
    }

    pRoom->vnum = 0;
    pRoom->room_flags = 0;
    pRoom->light = 0;
    pRoom->sector_type = 0;
    pRoom->clan = 0;
    pRoom->locx = 0;
    pRoom->locy = 0;
    pRoom->x = 0;
    pRoom->y = 0;
    pRoom->z = 0;
    pRoom->size = 0;
    pRoom->entryx = 0;
    pRoom->timezone = 0;
    pRoom->entryy = 0;
    pRoom->cloud_cover = 0;
    pRoom->cloud_density = 0;
    pRoom->temperature = 0;
    pRoom->raining = 0;
    pRoom->hailing = 0;
    pRoom->snowing = 0;
    pRoom->mist_level = 0;
    pRoom->encroachment = 0;
    pRoom->security = 0;
    pRoom->toughness = 0;
    pRoom->decor = 0;
    pRoom->level = 0;
    pRoom->mana_rate = 100;
    pRoom->observing = NULL;
    pRoom->time = 3000;
    return pRoom;
  }

  // Shroud doesn't get freed in here.  Why not? - Disco
  void free_room_index(ROOM_INDEX_DATA *pRoom) {
    int door;
    EXTRA_DESCR_DATA *pExtra;
    RESET_DATA *pReset;

    free_string(pRoom->name);
    free_string(pRoom->description);
    free_string(pRoom->subarea); // Discordance
    // free_string( pRoom->player_description ); //playerbase descriptions
    // free_string( pRoom->player_shroud ); //playerbase shroud descriptions
    free_string(pRoom->owner);
    free_rprog(pRoom->rprogs);

    for (door = 0; door < MAX_DIR; door++) {
      if (pRoom->exit[door])
      free_exit(pRoom->exit[door]);
    }
    for (door = 0; door < 10; door++) {
      if (pRoom->features[door])
      free_feature(pRoom->features[door]);
    }

    for (pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    for (pExtra = pRoom->places; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
      free_reset_data(pReset);
    }

    delete pRoom->people;

    pRoom->next = room_index_free;
    room_index_free = pRoom;
    return;
  }

  GROUP_INDEX_DATA *new_group_index(void) {
    GROUP_INDEX_DATA *pGroup;

    if (!group_index_free) {
      pGroup = (GROUP_INDEX_DATA *)alloc_perm(sizeof(*pGroup));
      top_group_index++;
    }
    else {
      pGroup = group_index_free;
      group_index_free = group_index_free->next;
    }

    pGroup->next = NULL;
    pGroup->vnum = 0;

    for (int x = 0; x < 6; x++) {
      pGroup->member_vnum[x] = 0;
      pGroup->member_position[x] = 0;
      pGroup->member_bonus[x] = 0;
    }

    return pGroup;
  }

  void free_group_index(GROUP_INDEX_DATA *pGroup) {
    pGroup->next = group_index_free;
    group_index_free = pGroup;
    return;
  }

  extern AFFECT_DATA *affect_free;

  SHOP_DATA *new_shop(void) {
    SHOP_DATA *pShop;
    int buy;

    if (shop_free.empty()) {
      pShop = (SHOP_DATA *)alloc_perm(sizeof(*pShop));
      top_shop++;
    }
    else {
      pShop = shop_free.front();
      shop_free.pop_front();
    }

    pShop->keeper = 0;

    for (buy = 0; buy < MAX_TRADE; buy++)
    pShop->buy_type[buy] = 0;

    pShop->profit_buy = 100;
    pShop->profit_sell = 100;
    pShop->open_hour = 0;
    pShop->close_hour = 23;
    pShop->owner = 0;
    pShop->proceed = 0;

    return pShop;
  }

  void free_shop(SHOP_DATA *pShop) {
    shop_free.push_front(pShop);
    return;
  }

  OBJ_INDEX_DATA *new_obj_index(void) {
    OBJ_INDEX_DATA *pObj;
    int value;

    if (!obj_index_free) {
      pObj = (OBJ_INDEX_DATA *)alloc_perm(sizeof(*pObj));
      top_obj_index++;
    }
    else {
      pObj = obj_index_free;
      obj_index_free = obj_index_free->next;
    }

    pObj->next = NULL;
    pObj->extra_descr = NULL;
    pObj->affected = NULL;
    pObj->area = NULL;
    pObj->name = str_dup("no name");
    pObj->short_descr = str_dup("(no short description)");
    pObj->description = str_dup("(no description)");
    pObj->vnum = 0;
    pObj->item_type = ITEM_TRASH;
    pObj->extra_flags = 0;
    pObj->wear_flags = 0;
    pObj->count = 0;
    pObj->size = 0;
    pObj->cost = 0;
    pObj->material = str_dup("");       /* ROM */
    pObj->condition = 100;              /* ROM */
    for (value = 0; value < 6; value++) /* 6 - Patterns */
    pObj->value[value] = 0;
    pObj->rot_timer = -1;
    pObj->buff = 0;
    pObj->faction = 0;
    pObj->load_chance = 100;
    return pObj;
  }

  void free_obj_index(OBJ_INDEX_DATA *pObj) {
    EXTRA_DESCR_DATA *pExtra;
    AFFECT_DATA *pAf;

    free_string(pObj->name);
    free_string(pObj->short_descr);
    free_string(pObj->description);
    free_oprog(pObj->oprogs);

    for (pAf = pObj->affected; pAf; pAf = pAf->next) {
      free_affect(pAf);
    }

    for (pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    pObj->next = obj_index_free;
    obj_index_free = pObj;
    return;
  }

  MOB_INDEX_DATA *new_mob_index(void) {
    MOB_INDEX_DATA *pMob;

    if (!mob_index_free) {
      pMob = (MOB_INDEX_DATA *)alloc_perm(sizeof(*pMob));
      top_mob_index++;
    }
    else {
      pMob = mob_index_free;
      mob_index_free = mob_index_free->next;
    }

    pMob->next = NULL;
    pMob->spec_fun = NULL;
    pMob->pShop = NULL;
    pMob->area = NULL;
    pMob->player_name = str_dup("no name");
    pMob->short_descr = str_dup("(no short description)");
    pMob->long_descr = str_dup("(no long description)\n\r");
    pMob->description = &str_empty[0];
    pMob->vnum = 0;
    pMob->ttl = 0;
    pMob->count = 0;
    pMob->sex = 0;
    pMob->level = 0;
    SET_INIT(pMob->act);
    SET_FLAG(pMob->act, ACT_IS_NPC);
    SET_INIT(pMob->affected_by);
    pMob->race = race_lookup("normal"); /* - Hugin */
    pMob->form = 0;                     /* ROM patch -- Hugin */
    pMob->parts = 0;                    /* ROM patch -- Hugin */
    pMob->material = str_dup("");       /* -- Hugin */
    pMob->off_flags = 0;                /* ROM patch -- Hugin */
    pMob->start_pos = POS_STANDING;     /*  -- Hugin */
    pMob->default_pos = POS_STANDING;   /*  -- Hugin */
    pMob->wealth = 0;

    for (int b = 0; b < MAX_DIS; b++)
    pMob->disciplines[b] = 0;

    for (int a = 0; a < 10; a++)
    pMob->targeting[a] = NULL;

    return pMob;
  }

  void free_mob_index(MOB_INDEX_DATA *pMob) {
    free_string(pMob->player_name);
    free_string(pMob->short_descr);
    free_string(pMob->long_descr);
    free_string(pMob->description);
    free_mprog(pMob->mprogs);

    free_shop(pMob->pShop);

    pMob->next = mob_index_free;
    mob_index_free = pMob;
    return;
  }

  ProgList mpcode_free;
  ProgList opcode_free;
  ProgList rpcode_free;

  PROG_CODE *new_mpcode(void) {
    PROG_CODE *NewCode;

    if (mpcode_free.empty()) {
      NewCode = (PROG_CODE *)alloc_perm(sizeof(*NewCode));
      top_mprog_index++;
    }
    else {
      NewCode = mpcode_free.front();
      mpcode_free.pop_front();
    }

    NewCode->vnum = 0;
    NewCode->code = str_dup("");

    return NewCode;
  }

  void free_mpcode(PROG_CODE *pMcode) {
    free_string(pMcode->code);
    mpcode_free.push_front(pMcode);
    return;
  }

  PROG_CODE *new_opcode(void) {
    PROG_CODE *NewCode;

    if (rpcode_free.empty()) {
      NewCode = (PROG_CODE *)alloc_perm(sizeof(*NewCode));
      top_oprog_index++;
    }
    else {
      NewCode = opcode_free.front();
      opcode_free.pop_front();
    }

    NewCode->vnum = 0;
    NewCode->code = str_dup("");

    return NewCode;
  }

  PROG_CODE *new_rpcode(void) {
    PROG_CODE *NewCode;

    if (rpcode_free.empty()) {
      NewCode = (PROG_CODE *)alloc_perm(sizeof(*NewCode));
      top_rprog_index++;
    }
    else {
      NewCode = rpcode_free.front();
      rpcode_free.pop_front();
    }

    NewCode->vnum = 0;
    NewCode->code = str_dup("");

    return NewCode;
  }

  void free_opcode(PROG_CODE *pOcode) {
    free_string(pOcode->code);
    opcode_free.push_front(pOcode);
    return;
  }

  void free_rpcode(PROG_CODE *pRcode) {
    free_string(pRcode->code);
    rpcode_free.push_front(pRcode);
    return;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif
