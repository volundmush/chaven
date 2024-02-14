/*

From: Christopher Feist <Christopher.Feist.feist@nt.com>

Recently I have been working on OLC delete commands since they would
be really handy.

I have come up with working versions of oedit_delete
and medit_delete. redit_delete is still in the works and I dont
think im going to bother with aedit_delete.

The basic premise is that you remove the vnum from the appropriate
hash and then delete all resets which refer to said vnum. This is
of course much easier to say then to do. The tricky part is
getting resets for objects inside other objects. Both of the
delete functions handle these cases.

I specificaly do not free up the index data because doing that
would require me to eliminate every instance of said vnum from
the mud. (because pIndexData still points to this memory location)
This isnt too bad for mobiles but for objects it really
really bites. If someone wants to code it up ill gladly take it
however. :)

I am submitting the two delete functions so that other muds besides
Aeon can test them and maybe force out any small bugs that I havent
caught. I STRONGLY suggest you read and understand what is happening
before you put these in. You will also have to add the hooks in
to call the functions yourself. (That should prevent most copy/paste
implementors from doing dumb things :P)

If you are trying to understand what is happening here I reccomend
uncommenting the debug code in oedit delete as it spews alot less
output then the medit delete code. I admit that neither of these
routines are particularily elegant coding examples however they
do work.

Lastly on Aeon there is a small glitch in medit_delete whereby
the person deleting the mobile somehow has his mount set to
a predictable value sometime after the exit of the function.
(i.e its something in our code not the function)

Unfortunatly I havent been able to figureout how/why/where this happens.
If anyone knows how to break on the change of the value
of a dereferenced pointer in gdb an example would be REALLY REALLY
appreciated. For those of you who have muds without mount code
(or with mount code) you will have to get rid of that little
section at the bottom of medit_delete since our mount code
isnt taken from a stock snippet.

Enjoy.

Narbo

Coder @ Aeon
telnet://mud.aeon.org:4000
*/

OEDIT( oedit_delete )
{
  OBJ_INDEX_DATA *pObj;
  OBJ_INDEX_DATA *iObj;
  OBJ_INDEX_DATA *sObj;
  RESET_DATA *pReset = NULL;
  RESET_DATA *prev = NULL;
  ROOM_INDEX_DATA *pRoom = NULL;
  char arg[MIL];
  char buf[MSL];
  int index, count, iHash, i;

  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  oedit delete [vnum]\n\r", ch );
    return FALSE;
  }

  one_argument( argument, arg );

  if( is_number( arg ) )
  {
    index = atoi( arg );
    pObj = get_obj_index( index );
  }
  else
  {
    send_to_char( "That is not a number.\n\r", ch );
    return FALSE;
  }

  SET_BIT( pObj->area->area_flags, AREA_CHANGED );

  /* Remove it from the object list */

  iHash = index % MAX_KEY_HASH;

  /* DEBUG CODE - uncomment this if you have doubts */
  /* printf("\nObject hash for location %d:\n", iHash);
for ( tObj = obj_index_hash[iHash]; tObj != NULL; tObj = tObj->next )
printf("name: %s vnum: %d\n", tObj->name, tObj->vnum ); */

  sObj = obj_index_hash[iHash];

  if( sObj->next == NULL ) /* only entry */
  obj_index_hash[iHash] = NULL;
  else if( sObj == pObj ) /* first entry */
  obj_index_hash[iHash] = pObj->next;
  else /* everything else */
  {
    for( iObj = sObj; iObj != NULL; iObj = iObj->next )
    {
      if( iObj ==  pObj )
      {
        sObj->next = pObj->next;
        break;
      }
      sObj = iObj;
    }
  }

  /* If you uncomment this you also need to
find every instance of the object that exists in
the mud and extract them otherwise each of thier
pIndexData will be pointing at free memory.
(Which may or may not contain the actual info)
As it is all the objects will be removed the reboot/login
automatically by fread_obj when it cant find the index */

  /* free_string( pObj->name );
free_string( pObj->short_descr );
free_string( pObj->description );

for( pAf = pObj->affected; pAf; pAf = pAf->next )
free_affect( pAf );

for( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next )
free_extra_descr( pExtra );

free( pObj ); */

  /* DEBUG CODE - uncomment this if you have doubts */
  /* printf("\nObject hash for location %d after removal:\n", iHash);
for ( tObj = obj_index_hash[iHash]; tObj != NULL; tObj = tObj->next )
printf("name: %s vnum: %d\n", tObj->name, tObj->vnum ); */

  /* DEBUG CODE */
  // printf( "\ntop_vnum_obj before: %d\n", top_vnum_obj );

  if( top_vnum_obj == index )
  for( i = 1; i < index; i++ )
  if( get_obj_index( i ) )
  top_vnum_obj = i;

  /* DEBUG CODE */
  // printf( "top_vnum_obj after: %d\n", top_vnum_obj );

  top_obj_index--;

  /* Now crush all resets */
  count = 0;
  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next )
    {
      prev = pRoom->reset_first;
      for( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
      {
        switch( pReset->command )
        {
        case 'O':
        case 'E':
        case 'P':
        case 'G':
          if( ( pReset->arg1 == index ) || ( (  pReset->command == 'P' ) && (pReset->arg3 == index ) ) )
          {
            // printf("\nprev: %d  prev->next: %d\n",prev, prev->next );

            /* DEBUG CODE - uncomment this if you have doubts */
            /* printf("\nReset info for room %d:\n",pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d memloc:%d\n", tReset->command, tReset->arg1, tReset ); */

            if( pRoom->reset_first == pReset )
            {
              pRoom->reset_first = pReset->next;
              if( !pRoom->reset_first )
              pRoom->reset_last = NULL;
            }
            else if( pRoom->reset_last == pReset )
            {
              pRoom->reset_last = prev;
              prev->next = NULL;
            }
            else
            {
              prev->next = prev->next->next;
            }

            count++;
            SET_BIT( pRoom->area->area_flags,AREA_CHANGED );

            /* DEBUG CODE - uncomment this if you have doubts */
            /* printf("\nReset info for room %d after removal:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d memloc:%d\n", tReset->command, tReset->arg1, tReset ); */

            // printf("\nprev: %d  prev->next: %d\n",prev, prev->next );
          }
        }
        prev = pReset;
      }
    }
  }

  sprintf( buf, "Removed object vnum ^C%d^x and ^C%d^x resets.\n\r", index,count );
  send_to_char( buf, ch );
  return TRUE;
}


MEDIT( medit_delete )
{
  MOB_INDEX_DATA *pMob;
  MOB_INDEX_DATA *sMob;
  MOB_INDEX_DATA *iMob;
  RESET_DATA *pReset = NULL;
  RESET_DATA *prev= NULL;
  ROOM_INDEX_DATA *pRoom = NULL;
  char arg[MIL];
  char buf[MSL];
  int index, count, iHash, i;
  int dobj[100]; /* I highly doubt one mobile will have 100 unique object resets */
  bool foundmob = FALSE;
  bool exist = FALSE;
  bool foundobj = FALSE;

  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  medit delete [vnum]\n\r", ch );
    return FALSE;
  }

  one_argument( argument, arg );

  if( is_number( arg ) )
  {
    index = atoi( arg );
    pMob = get_mob_index( index );
  }
  else
  {
    send_to_char( "That is not a number.\n\r", ch );
    return FALSE;
  }

  if( !pMob )
  {
    send_to_char( "No such mobile.\n\r", ch );
    return FALSE;
  }

  SET_BIT( pMob->area->area_flags, AREA_CHANGED );

  /* Remove it from the object list */

  iHash = index % MAX_KEY_HASH;

  /* DEBUG CODE - uncomment this if you have doubts */
  /* printf("\nMobile hash for location %d:\n", iHash);
for ( tMob = mob_index_hash[iHash]; tMob != NULL; tMob = tMob->next )
printf("short_desc: %s  vnum: %d\n", tMob->short_descr, tMob->vnum );*/

  sMob = mob_index_hash[iHash];

  if( sMob->next == NULL ) /* only entry */
  mob_index_hash[iHash] = NULL;
  else if( sMob == pMob ) /* first entry */
  mob_index_hash[iHash] = pMob->next;
  else /* everything else */
  {
    for( iMob = sMob; iMob != NULL; iMob = iMob->next )
    {
      if( iMob ==  pMob )
      {
        sMob->next = pMob->next;
        break;
      }
    }
  }

  /* See oedit_delete for why i dont free pMob here */

  /* DEBUG CODE - uncomment this if you have doubts */
  /* printf("\nMobile hash for location %d after removal:\n", iHash);
for ( tMob = mob_index_hash[iHash]; tMob != NULL; tMob = tMob->next )
printf("short_desc: %s  vnum: %d\n", tMob->short_descr, tMob->vnum );*/

  if( top_vnum_mob == index )
  for( i = 1; i < index; i++ )
  if( get_obj_index( i ) )
  top_vnum_obj = i;

  top_mob_index--;

  /* Now crush all resets */
  count = 0;
  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next )
    {
      dobj[0] = -1;
      prev = pRoom->reset_first;
      for( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
      {
        switch( pReset->command )
        {
        case 'M':
          if( pReset->arg1 == index )
          {
            foundmob = TRUE;

            /* DEBUG CODE - uncomment this if you have doubts */
            /* printf("\nReset info for room %d:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1 ); */

            if( pRoom->reset_first == pReset )
            {
              pRoom->reset_first = pReset->next;
              if( !pRoom->reset_first )
              pRoom->reset_last = NULL;
            }
            else if( pRoom->reset_last == pReset )
            {
              pRoom->reset_last = prev;
              prev->next = NULL;
            }
            else
            prev->next = prev->next->next;

            count++;
            SET_BIT( pRoom->area->area_flags,AREA_CHANGED );

            /* DEBUG CODE - uncomment this if you have doubts */
            /* printf("\nReset info for room %d after removal:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset; tReset = tReset->next )
printf("command: %c  vnum: %d\n", tReset->command, tReset->arg1 ); */
          }
          else
          foundmob = FALSE;

          break;
        case 'E':
        case 'G':
          if( foundmob )
          {
            // printf( "Removing: command: %c  vnum: %d\n", pReset->command, pReset->arg1 );

            /* DEBUG CODE - uncomment this if you have doubts */
            /* printf("\nReset info for room %d:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset; tReset = tReset->next )
printf("command: %c  vnum: %d\n", tReset->command, tReset->arg1 ); */

            exist = FALSE;

            for( i = 0; dobj[i] != -1; i++ )
            {
              if( dobj[i] == pReset->arg1 )
              {
                exist = TRUE;
                break;
              }
            }

            if( !exist )
            {
              dobj[i] = pReset->arg1;
              dobj[i + 1] = -1;

              /* DEBUG CODE */
              /* for( i = 0; dobj[i] != -1; i++ )
printf( "dobj[%d] : %d\n", i,dobj[i] ); */
            }

            if( pRoom->reset_first == pReset )
            {
              pRoom->reset_first = pReset->next;
              if( !pRoom->reset_first )
              pRoom->reset_last = NULL;
            }
            else if( pRoom->reset_last == pReset )
            {
              pRoom->reset_last = prev;
              prev->next = NULL;
            }
            else
            prev->next = prev->next->next;

            count++;
            SET_BIT( pRoom->area->area_flags,AREA_CHANGED );

            /* DEBUG CODE - uncomment this if you havedoubts */
            /* printf("\nReset info for room %d afterremoval:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1 ); */
          }

          break;
        case 'P':
          foundobj = FALSE;

          for( i = 0; dobj[i] != -1; i++ )
          if( dobj[i] == pReset->arg3 )
          foundobj = TRUE;

          if( foundobj )
          {
            printf( "Removing: command: %c  vnum: %d\n",pReset->command, pReset->arg1 );

            /* DEBUG CODE - uncomment this if you havedoubts */
            /* printf("\nReset info for room %d:\n",pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1 ); */

            if( pRoom->reset_first == pReset )
            {
              pRoom->reset_first = pReset->next;
              if( !pRoom->reset_first )
              pRoom->reset_last = NULL;
            }
            else if( pRoom->reset_last == pReset )
            {
              pRoom->reset_last = prev;
              prev->next = NULL;
            }
            else
            prev->next = prev->next->next;

            count++;
            SET_BIT( pRoom->area->area_flags,AREA_CHANGED );

            /* DEBUG CODE - uncomment this if you havedoubts */
            /* printf("\nReset info for room %d afterremoval:\n", pRoom->vnum );
for( tReset = pRoom->reset_first; tReset;tReset = tReset->next )
printf("command: %c  vnum: %d\n",tReset->command, tReset->arg1 ); */
          }
        }
        prev = pReset;
      }
    }
  }

  sprintf( buf, "Removed mobile vnum ^C%d^x and ^C%d^x resets.\n\r", index,count );
  send_to_char( buf, ch );
  return TRUE;
}


--

0o0o0o0o0o0o0o0o0o0o0o00o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0
o Chris Feist      GCS/GMUd-s+:-a--C++++USUH$ULU+++P++L++E-W-N++o?Kw-- o
o Dept 7M52        O?M++V-PS+PEY+PGP+t+5+X-Rtv+b++DI++D++Ge++@hr--!y+  0
0 SS7 Verification                                                     o
o feist@nortel.ca  George Bush: "It's amazing how many people beat you 0
0                  at golf now that you're no longer president."       o
o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0o0

