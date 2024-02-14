/*
World shaping goodness - Discordance

Things to do:
Address cordinate to roomnumber conversion without having to manually
plan everything out because it's annoying Automagically generate rooms for the
woods based on whether or not there's something else built there or it's been
deforested and made into a field Automagically have trees/grass grow up in
previously deforested/mowed areas if it remains unpopulated for a while Scale
the grid automagically based on current use Automagically build new streets
when necessary when scaling up beyond initial street structure (maybe with bank
of tree names for streets and founding father surnames for avenues)

*/

#ifndef WIN32
#include <sys/stat.h>
#endif
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
#include "recycle.h"
#include "lookup.h"
#include "global.h"


#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif



  int other_cloud_cover = 0;
  int other_cloud_density = 0;
  int other_temperature = 0;
  int other_raining = 0;
  int other_hailing = 0;
  int other_snowing = 0;

  int wilds_cloud_cover = 0;
  int wilds_cloud_density = 0;
  int wilds_temperature = 0;
  int wilds_raining = 0;
  int wilds_hailing = 0;
  int wilds_snowing = 0;

  int godrealm_cloud_cover = 0;
  int godrealm_cloud_density = 0;
  int godrealm_temperature = 0;
  int godrealm_raining = 0;
  int godrealm_hailing = 0;
  int godrealm_snowing = 0;

  int hell_cloud_cover = 0;
  int hell_cloud_density = 0;
  int hell_temperature = 0;
  int hell_raining = 0;
  int hell_hailing = 0;
  int hell_snowing = 0;

  int get_coordx(int vnum) {
    int x, y;

    if (vnum < 100000000) {
      return 0;
    }

    y = vnum % 1000;
    vnum -= y;
    vnum /= 1000;
    x = vnum % 1000;

    return x;
  }

  int get_coordy(int vnum) {
    int y;

    if (vnum < 100000000)
    return 0;

    y = vnum % 1000;

    return y;
  }

  int get_coordz(int vnum) {
    int x, y, z;

    if (vnum < 100000000)
    return 0;

    y = vnum % 1000;
    vnum -= y;
    vnum /= 1000;

    x = vnum % 1000;
    vnum -= x;
    vnum /= 1000;

    z = vnum % 100;

    return z;
  }

  // stock descriptions to cut down on resource use
  char *stock_description(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    char buf[MSL];
    strcpy(buf, "");
    ROOM_INDEX_DATA *room_template;

    // greenspace
    if (strcasestr(room->name, "delete") != NULL) {
      return "";
    }

    if (battleground(room))
    return "";

    tm *ptm;
    ptm = gmtime(&current_time);

    if (institute_room(room)) {
      return "";
    }

    if (room->area->vnum >= HAVEN_TOWN_VNUM && room->area->vnum <= HELL_FOREST_VNUM && room->area->vnum != HAVEN_OCEAN_VNUM) {
      if (room->sector_type == SECT_FOREST) {
        if (room->area->vnum == OTHER_FOREST_VNUM) {
          sprintf(
          buf, "The forest extends through this area and as far as the eye can see, ancient trees with lush foliage surround you, rising high above your head and higher still, almost surreally, the light from the sky casting the area in a dreamy glow.  The ground beneath is soft, and the flowers that blossom here are of species that undocumented by the modern world, while the moss-ridden trunks of the trees provide obstacles that must be climbed or vaulted over, making travel here a formidable challenge.  The woodland creatures here don't run at the sight of people, but instead stop what they're doing to observe them curiously.");
        }
        else if (room->area->vnum == GODREALM_FOREST_VNUM) {
          sprintf(
          buf, "The trees in this area are lush and plenty, but give each other breathing room so that travelers can gaze at the sky.  There are no weeds anywhere in sight, and the grass seems meticulous, yet somehow untouched, forming vast meadows and easy to navigate hills filled with flowerbeds that naturally form beautifully along the landscape.  The trees themselves have very little stray branches, and those that do bear fruit that never appears to rot. Unseen birds sit high in the surrounding foilage, chirping merrily.  On occasion, they appear to be singing to each other, or maybe even the travelers who pass through here.");
        }
        else if (room->area->vnum == WILDS_FOREST_VNUM) {
          sprintf(
          buf, "The forest extends through this area and as far as the eye can see, an excessive amount trees with wild foilage surround you, the canopy above forming a screen that causes any natural lighting to be dimmer, painting what should be lush plantlife something more primal.  The ground beneath is rugged and difficult to tread on, covered in vines, suspicious looking plants and the labyrinthine trunks and fallen branches on the ground renders traveling a dozen feet directly in any direction impossible and making those who wander through the area step carefully.  Falling here doesn't seem like it would be very safe.  Hissing is almost a constant all around you, and you have the distinct feeling you're being watched.");
        }
        else if (room->area->vnum == HELL_FOREST_VNUM) {
          sprintf(
          buf, "Almost all ambient light is swallowed up by this unnaturally dark forest. The trees are all black or dark grey and their leaves entirely light-drinking black. Yet throughout this dark monochromatic and oppressive forest are splashes of almost too bright colours, fruits and flowers in bright reds and blues and yellows that tempt visitors to touch and taste and smell, usually to their peril. Serpents seem unusually plentiful in the black forest, blending in exceptionally well with their environs. In the east a constant red glow and sporadic thunder emanate from the eternally raging hell wars.");
        }
        else if (ptm->tm_mon == 10 || ptm->tm_mon == 9 || ptm->tm_mon == 8) // Fall
        {
          if (IS_FLAG(ch->act, PLR_SHROUD)) {
            sprintf(
            buf, "The forest extends through this area and as far as the eye can see, swirling mist obscures the ground while twisted and gnarled old  trees surround you. Each one with knots and swirls that makes them look as if they have faces that watch your progress greedily. Barely any light  makes it into the forest and that which does is muted, the trunks of the trees continue on forever making seeing more than a dozen feet in any  direction impossible and turning the whole place in a ominous and confusing maze. Strange sounds echo between the trunks of the trees, snarls  and growls, strange moans and other sounds that are almost but not quite like that of an animal. Shapes move in the distance too, shadows in the  mist, some small, but others not while faintly glowing eyes peer from the shadows in the trees above. Glowing small shapes of many varied colors  flit through the branches of the old trees, and faint giggling sounds can be heard from them, filled with playfulness edged in malice.");
          }
          else {
            sprintf(buf, "The forest extends through this area and as far as the eye can see, old trees with brown leaves surround you, the light filtering through the limited canopy in beams. The ground beneath is soft and littered with fallen leaves that make every footfall echo. The trunks of the trees continue on forever making seeing more than a dozen feet in any direction impossible, and making those who wander through the area take meandering paths.");
          }
        }
        else if (ptm->tm_mon == 11 || ptm->tm_mon == 0 || ptm->tm_mon == 1) // Winter
        {
          if (IS_FLAG(ch->act, PLR_SHROUD)) {
            sprintf(
            buf, "The forest extends through this area and as far as the eye can see, swirling mist obscures the ground while twisted and gnarled old  trees surround you. Each one with knots and swirls that makes them look as if they have faces that watch your progress greedily. Barely any light  makes it into the forest and that which does is muted, the trunks of the trees continue on forever making seeing more than a dozen feet in any  direction impossible and turning the whole place in a ominous and confusing maze. Strange sounds echo between the trunks of the trees, snarls  and growls, strange moans and other sounds that are almost but not quite like that of an animal. Shapes move in the distance too, shadows in the  mist, some small, but others not while faintly glowing eyes peer from the shadows in the trees above. Glowing small shapes of many varied colors  flit through the branches of the old trees, and faint giggling sounds can be heard from them, filled with playfulness edged in malice.");
          }
          else {
            sprintf(buf, "The forest extends through this area and as far as the eye can see, old trees devoid of leaves surround you, the weak winter sun filtering down through their branches. Frost edges the branches and the ground beneath is lined in soft white snow, small and sometimes large animal tracks crossing it's otherwise pristine surface. The trunks of the trees continue on forever making seeing more than a dozen feet in any direction impossible, and making those who wander through the area take meandering paths.");
          }
        }
        else if (ptm->tm_mon == 2 || ptm->tm_mon == 3 || ptm->tm_mon == 4) // Spring
        {
          if (IS_FLAG(ch->act, PLR_SHROUD)) {
            sprintf(
            buf, "The forest extends through this area and as far as the eye can see, swirling mist obscures the ground while twisted and gnarled old  trees surround you. Each one with knots and swirls that makes them look as if they have faces that watch your progress greedily. Barely any light  makes it into the forest and that which does is muted, the trunks of the trees continue on forever making seeing more than a dozen feet in any  direction impossible and turning the whole place in a ominous and confusing maze. Strange sounds echo between the trunks of the trees, snarls  and growls, strange moans and other sounds that are almost but not quite like that of an animal. Shapes move in the distance too, shadows in the  mist, some small, but others not while faintly glowing eyes peer from the shadows in the trees above. Glowing small shapes of many varied colors  flit through the branches of the old trees, and faint giggling sounds can be heard from them, filled with playfulness edged in malice.");
          }
          else {
            sprintf(
            buf, "The forest extends through this area and as far as the eye can see, old trees with vibrant, pale green leaves just starting to develop on their branches surround you, the sun filtering down through the canopy to form sunbeams all around. The ground beneath is soft and lined with small plants just coming into bloom as well as mushrooms and fallen branches. The trunks of the trees continue on forever making seeing more than a dozen feet in any direction impossible and making those who wander through the area take meandering paths. Squirrels and other small creatures dart about in the branches of the trees.");
          }
        }
        else // Summer
        {
          if (IS_FLAG(ch->act, PLR_SHROUD)) {
            sprintf(
            buf, "The forest extends through this area and as far as the eye can see, swirling mist obscures the ground while twisted and gnarled old  trees surround you. Each one with knots and swirls that makes them look as if they have faces that watch your progress greedily. Barely any light  makes it into the forest and that which does is muted, the trunks of the trees continue on forever making seeing more than a dozen feet in any  direction impossible and turning the whole place in a ominous and confusing maze. Strange sounds echo between the trunks of the trees, snarls  and growls, strange moans and other sounds that are almost but not quite like that of an animal. Shapes move in the distance too, shadows in the  mist, some small, but others not while faintly glowing eyes peer from the shadows in the trees above. Glowing small shapes of many varied colors  flit through the branches of the old trees, and faint giggling sounds can be heard from them, filled with playfulness edged in malice.");
          }
          else {
            sprintf(
            buf, "The forest extends through this area and as far as the eye can see, old trees with lush foliage surround you, the light from above  glimmers as it makes its way through the canopy, coming down in sunbeams that spot the area. The ground beneath is soft and lined with small plants,  mushrooms and fallen branches. The trunks of the trees continue on forever making seeing more than a dozen feet directly in any direction impossible  and making those who wander through the area take meandering paths. Squirrels and other small creatures dart about in the branches of the trees.");
          }
        }
      }
      else if (room->sector_type == SECT_PARK) {
        if ((room_template = get_room_index(101)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
        /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"A clover rich field of grey-green grass is
obscured by the rolling mist that spills along the earth.  Patches of
low growing wild flowers and dandelions poke up through the clouds when
the season is right and lend color to the otherwise somber expanse. The
dense lawn thins in the shade of large and gnarled trees that spot the
landscape where they have been allowed to grow alone or in pairs, but
separated from the darker woods.  Their twisted limbs, stretching
skyward writhe in agony and their knots and whorls contort into dread
faces that scrutinize passersby with a fierce intensity.  Small, glowing forms flit around their branches.");
}
else {
sprintf(buf,"A clover rich field of grey-green grass
splays out underfoot.  Patches of low growing wild flowers and
dandelions are not uncommon when the season is right and lend color to
the otherwise somber expanse.  The dense lawn thins in the shade of
large and gnarled trees that spot the landscape where they have been
allowed to grow alone or in pairs, but separated from the darker woods.
Their twisted limbs, stretching skyward with apparent agony, are a
common sight along roadways.");
}
*/
      }
      else if (room->sector_type == SECT_WATER) {
        // Unity Cove
        if (!str_cmp(room->subarea, "Cove")) {
          if ((room_template = get_room_index(102)) != NULL) {
            if (IS_FLAG(ch->act, PLR_SHROUD))
            strcpy(buf, room_template->shroud);
            else {
              strcpy(buf, room_template->description);
            }
          }
          /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"Swirls of white mist cover the surface of the ocean, making its
surface almost impossible to see. The currents are noticeable, always seeming to
tug at people as if trying and pull them further out, to lose them. The rocky
bluffs looming ominously over the water to the west, while the dark forest
surrounds the town to the north. The crashing of waves is all around, sounding
almost like a voice that entreats those who hear it to swim out, out and out
always further to a promise of a wonderful and beautiful death.");
}
else {
sprintf(buf,"The salty water is always cool, no matter the air temperature. The
currents are generally light but still noticeable, ebbing and swirling around
you. To the north, the white sandy beach beckons while rocky bluffs to the west
block out some of the otherwise grand and unobstructed view of the sky. The
rolling waves continue as far as the eye can see to the south and east, while
the verdant green of the forests can be seen surrounding the town further to the
north, past the beachfront.");
}
*/
        }
        else if (!str_cmp(room->subarea, "Coast")) {
          if ((room_template = get_room_index(103)) != NULL) {
            if (IS_FLAG(ch->act, PLR_SHROUD))
            strcpy(buf, room_template->shroud);
            else {
              strcpy(buf, room_template->description);
            }
          }
          /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"The shore remains in sight and near, if
shrouded in a constant haze that seems to permeate the town.  It
wafts off the shoreline and spills in rolling clouds that obscure the
sky and mask the water.  Though it rolls and whirls along the
surface, it never quite seems to touch as if the mixing of the two
bodies of mystery would be taboo.");
}
else {
sprintf(buf,"The shore remains in sight and near, a
comforting and stable constant amoung the shifting waves.  They slide
along the surface in rythmic, almost predictable patterns to crash
along the rocky Haven shoreline before retreating back into the
endless expanse of the cool blue-green.");
}
*/
        }
        // River
        else {
          if ((room_template = get_room_index(104)) != NULL) {
            if (IS_FLAG(ch->act, PLR_SHROUD))
            strcpy(buf, room_template->shroud);
            else {
              strcpy(buf, room_template->description);
            }
          }
          /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf, "The surface of the cool water is shrouded in mist as it flows
quickly to the east, swirling and forming small whirlpools as it goes. The
sounds of the water moving are almost musical, the sighs and gurgles blending
together harmoniously as it rushes past. Several dark and twisted trees line the
banks of the river, shadowy shapes flitting constantly amongst their
branches.");
}
else {
sprintf(buf, "The cool water flows quickly but not dangerously so, meandering
toward the east. The water is clear enough that the bottom can usually be seen, lined with small pebbles, while the banks meet up with the soft grasses of the
field. The odd tree lines the worn footpath of the river, roots visible under
the water as their branches loom overhead.");
}
*/
        }
      }
      else if (room->sector_type == SECT_UNDERWATER) {
        if ((room_template = get_room_index(105)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
        /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf, "The clear cool water of the river swirls around you as it makes
its way to the east and south. The skies and tree branches can be seen above
you, somehow ominous through the distortion of the water, while to either side
and below you can see the smooth pebbles that form the bottom of the river. The
odd small green plant bullies its way through the pebbles along the riverbed, watery arms weakly snatching at your limbs. Glimpses of tiny, glowing fish are
often seen darting back and forth through the water.");
}
else {
sprintf(buf,"The clear cool water of the river swirls around you as it makes its
way to the east and south. The skies and tree branches can be seen above you, distorted through the water's surface, while to either side and below you can
see the smooth pebbles that form the bottom of the river. The odd small green
plant bullies its way through the pebbles along the riverbed. Glimpses of tiny, shimmering silver fish are often seen darting back and forth through the
water.");
}
*/
      }
      else if (room->sector_type == SECT_ROCKY) {
        if ((room_template = get_room_index(106)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
        /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf, "Smooth pebbles punctuated by large and sharper rocks line the
ground here as the bluffs continue to slope upwards the further south you go.
The mist is sparse up here but still exists, clinging stubbornly to the gaps and
shadows between rocks. The sounds of the misty ocean below beckon you, seeming
almost like a voice that urges you to jump and dive and never surface again.");
}
else {
sprintf(buf,"Smooth pebbles punctuated by larger and sharper rocks line the
ground here as the bluffs continue to slope upwards the further south you go.
The sound of the ocean crashing against the rocks below ever present. The view
is grand, the ocean can be seen continuing forever to the south and east, while
the town wrapped in its verdant forest is small but clear to the north.");
}
*/
      }
      else if (room->sector_type == SECT_BEACH) {
        if ((room_template = get_room_index(107)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
        /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"Soft dark sand crunches and sinks lightly underfoot, its shine
obscured by the pure white mist that swirls in from the ocean to the south, casting it in an ominous shade. To the north, the town can just barely be seen
while the dark forest extends far to the east and west of it. Strange sounds
come from the ocean, the crashing of the waves sounding almost like a voice that
entreats those who hear to enter it while promising a wonderful death.");
}
else {
sprintf(buf,"Soft brown sand crunches and sinks lightly underfoot, eagerly
reflecting light and spotted with small stones and seashells. To the south and
east the ocean stretches as far as the eye can see, bordered to the west by the
slopes of the rocky bluffs. To the north, the tops of the buildings in town can
just barely be seen, along with the deep green of the forest that surrounds
it.");
}
*/
      }
      else if (room->sector_type == SECT_AIR) {
        if ((room_template = get_room_index(108)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
        /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"The air up here is crisp and clean, and the wind makes it feel
cooler than it is at ground level. Below, the mist-shrouded streets stretch out, winding through the town. The treetops seem to twist and follow you as you move
through the air, while strange lights and sounds emanate from ground level.");
}
else {
sprintf(buf,"The air up here is crisp and clean, and the wind makes it feel
cooler than it is at ground level. Below, the streets stretch out, winding
through the town, and the branches of some of the taller trees compete with the
larger buildings as they reach up towards you from below. In the distance to the
south, the deep blue of the ocean can be seen reflecting the light while the
deep green of the forest stretches out around the town in all other
directions.");
}
*/
      }
      else if (room->sector_type == SECT_STREET) {
        if ((room_template = get_room_index(109)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
        /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"Dark cobbled roads are partially hidden by wisps of white mist that
ebb and swirl near the ground. Stone footpaths line either side, punctuated
regularly by black iron gas streetlamps choked by ivy vines. They cast
flickering orange circles of light on the ground that seem to reach for
passersby, drawing them in. Where the road is widest, small stone plinths appear
with old twisted trees planted inside them, their knots and swirls forming faces
with eyes that seem to follow you wherever you move.");
}
else {
sprintf(buf,"Smooth asphalt roads continue through this part of town, bordered
on either side by well maintained concrete sidewalks. The aluminum streetlights
are painted a deep green and appear regularly along the side, illuminating the
street in spots of warm electric light when it's dark. Where the street is
widest small median islands appear with old twisted trees planted in them. The
buildings that line the street seem quaint, but well maintained.");
}
*/
      }
      else if (room->sector_type == SECT_TUNNELS) {
        if ((room_template = get_room_index(110)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
        /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"This tunnel is wreathed in mist that swirls and flows with the
slightest movement. The passage itself is made from pale stone, with ornate
symbols etched into it at regular intervals. Arches, carved with patterns of
ivy, are used for support. The flowing water makes strange gurgling noises, almost as if alive, and shadows seem to constantly move in the corner of your
eye.");
}
else {
sprintf(buf,"This sewer tunnel is well maintained but clearly old, rendered from
worn stone with regular arches set at regular intervals for support. The
rainwater washes down the center of the tunnel heading south, while small
walkways to either side allow careful passage; the stone is slippery when wet, and pale moss grows unevenly here.");
}
*/
      }
      else if (room->sector_type == SECT_ALLEY) {
        if ((room_template = get_room_index(112)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
      }
      else if (room->sector_type == SECT_CEMETARY) {
        if ((room_template = get_room_index(113)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
      }
      else if (room->sector_type == SECT_PARKING) {
        if ((room_template = get_room_index(114)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
      }
      else if (room->sector_type == SECT_SIDEWALK) {
        if ((room_template = get_room_index(115)) != NULL) {
          if (IS_FLAG(ch->act, PLR_SHROUD))
          strcpy(buf, room_template->shroud);
          else {
            strcpy(buf, room_template->description);
          }
        }
      }
    }
    // ocean
    else if (room->area->vnum == HAVEN_OCEAN_VNUM) {
      if ((room_template = get_room_index(111)) != NULL) {
        if (IS_FLAG(ch->act, PLR_SHROUD))
        strcpy(buf, room_template->shroud);
        else {
          strcpy(buf, room_template->description);
        }
      }
      /*
if(IS_FLAG(ch->act, PLR_SHROUD)) {
sprintf(buf,"Haze and mist obscure sight in all directions and the muted water
below it trembles ominously as if in anticipation.  What's left of the visible
sky blends all but seamlessly with the ocean and the seemingly persistent ebb, coaxes all else further out to sea.");
}
else {
sprintf(buf,"The expanse opens up and the shore starts to fade away until it
becomes an ever shrinking line on the horizon.  For miles in the distance, indeed, perhaps forever, the cool blue-green wrinkles and shifts.");
}
*/
    }
    // private property
    else if (room->area == get_vnum_area(HavenPrivateProperty)) {
    }
    // haven government and business
    else if (room->area == get_vnum_area(HavenCityBuildings)) {
    }
    // the other
    else if (room->area == get_vnum_area(TheOther)) {
    }
    // the wilds
    else if (room->area == get_vnum_area(TheWilds)) {
    }
    // the godrealm
    else if (room->area == get_vnum_area(TheGodrealm)) {
    }
    // send_to_char(buf, ch);

    return str_dup(buf);
  }

  // for turning rooms back into forest - Discordance
  void reset_room_index(ROOM_INDEX_DATA *room) {
    if (room == NULL) {
      return;
    }

    EXTRA_DESCR_DATA *pExtra;
    RESET_DATA *pReset;
    if (strcasestr(room->name, "delete") == NULL) {
      free_string(room->name);
      room->name = str_dup("");
    }

    free_string(room->description);
    room->description = str_dup("");
    free_string(room->owner);
    room->owner = str_dup("");
    free_rprog(room->rprogs);
    /*
for(int door = 0;door<=9;door++) {
ROOM_INDEX_DATA *pToRoom;
int rev = rev_dir[door];
if(room->exit[door] == NULL)
continue;


if(pToRoom->area == get_vnum_area(HavenCityBuildings) || pToRoom->area
== get_vnum_area(HavenPrivateProperty)) { continue;
}

if(pToRoom != NULL) {
if ( pToRoom->exit[rev] ) {
free_exit( pToRoom->exit[rev] );
pToRoom->exit[rev] = NULL;
}
}

free_exit( room->exit[door] );
room->exit[door] = NULL;
}
*/
    for (pExtra = room->extra_descr; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    for (pExtra = room->places; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    for (pReset = room->reset_first; pReset; pReset = pReset->next) {
      free_reset_data(pReset);
    }

    room->sector_type = SECT_FOREST;

    if (IS_SET(room->room_flags, ROOM_STASH))
    TOGGLE_BIT(room->room_flags, ROOM_STASH);

    if (IS_SET(room->room_flags, ROOM_INDOORS))
    TOGGLE_BIT(room->room_flags, ROOM_INDOORS);

    if (IS_SET(room->room_flags, ROOM_DARK))
    TOGGLE_BIT(room->room_flags, ROOM_DARK);

    if (IS_SET(room->room_flags, ROOM_LIGHTON))
    TOGGLE_BIT(room->room_flags, ROOM_LIGHTON);

    if (IS_SET(room->room_flags, ROOM_LIGHTOFF))
    TOGGLE_BIT(room->room_flags, ROOM_LIGHTOFF);

    if (IS_SET(room->room_flags, ROOM_PUBLIC))
    TOGGLE_BIT(room->room_flags, ROOM_PUBLIC);

    if (IS_SET(room->room_flags, ROOM_UNLIT))
    TOGGLE_BIT(room->room_flags, ROOM_UNLIT);

    return;
  }

  bool protected_offworld_room(ROOM_INDEX_DATA *room) {
    if (room->area->vnum == 22) {
      if (room->x >= 1000024 && room->x <= 1000026 && room->y >= 1000024 && room->y <= 1000026)
      return TRUE;
    }
    if (room->area->vnum == 23) {
      if (room->x >= 2000024 && room->x <= 2000026 && room->y >= 2000024 && room->y <= 2000026)
      return TRUE;
    }
    if (room->area->vnum == 24) {
      if (room->x >= 3000024 && room->x <= 3000026 && room->y >= 3000024 && room->y <= 3000026)
      return TRUE;
    }
    if (room->area->vnum == 25) {
      if (room->x >= 4000024 && room->x <= 4000026 && room->y >= 4000024 && room->y <= 4000026)
      return TRUE;
    }

    return FALSE;
  }

  void reset_forest_room(ROOM_INDEX_DATA *room) {
    if (room == NULL || !room) {
      return;
    }
    if (protected_offworld_room(room))
    return;
    room->encroachment = 0;
    //    fix_room_dirs(room);
    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      if (room->exit[door] != NULL) {
        pToRoom = room->exit[door]->u1.to_room;
        if (pToRoom != NULL && !protected_offworld_room(pToRoom)) {
          if (room->z > 0)
          room->exit[door]->fall = 2;
          room->exit[door]->wall = 0;
          if (IS_SET(room->exit[door]->exit_info, EX_CLOSED))
          REMOVE_BIT(room->exit[door]->exit_info, EX_CLOSED);
          if (IS_SET(room->exit[door]->exit_info, EX_ISDOOR))
          REMOVE_BIT(room->exit[door]->exit_info, EX_ISDOOR);
          if (IS_SET(room->exit[door]->exit_info, EX_LOCKED))
          REMOVE_BIT(room->exit[door]->exit_info, EX_LOCKED);
          if (IS_SET(room->exit[door]->exit_info, EX_CURTAINS))
          REMOVE_BIT(room->exit[door]->exit_info, EX_CURTAINS);
          if (IS_SET(room->exit[door]->exit_info, EX_HIDDEN))
          REMOVE_BIT(room->exit[door]->exit_info, EX_HIDDEN);
        }
      }
    }
    if (room->z == 1) {
      room->sector_type = SECT_AIR;
      free_string(room->name);
      room->name = str_dup("The Skies");
    }
    else if (room->z == 0) {
      if (room->area->world != WORLD_EARTH || (room->sector_type != SECT_WATER && room->sector_type != SECT_UNDERWATER && room->sector_type != SECT_SHALLOW)) {
        room->sector_type = SECT_FOREST;
        free_string(room->name);
        room->name = str_dup("The Forest");
      }
    }
    else {
      if (room->area->world != WORLD_EARTH || room->sector_type != SECT_UNDERWATER)
      kill_room(room);
    }
  }

  void kill_room(ROOM_INDEX_DATA *room) {
    if (room == NULL || !room) {
      return;
    }
    OBJ_DATA *obj_next;
    OBJ_DATA *obj;

    for (CharList::iterator it = room->people->begin();
    it != room->people->end();) {
      CHAR_DATA *victim = *it;
      ++it;

      if (IS_NPC(victim)) {
        save_char_obj(victim, TRUE, FALSE);
        extract_char(victim, TRUE);
      }
    }

    for (obj = room->contents; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
        obj_from_room(obj);
        obj_to_room(obj, get_room_index(1));
      }
      else
      extract_obj(obj);
    }

    EXTRA_DESCR_DATA *pExtra;
    if (strcasestr(room->name, "delete") == NULL) {
      free_string(room->name);
      room->name = str_dup("");
    }

    free_string(room->description);
    room->description = str_dup("");
    free_string(room->owner);
    room->owner = str_dup("");
    free_rprog(room->rprogs);

    if (room) {
      for (int door = 0; door <= 9; door++) {
        ROOM_INDEX_DATA *pToRoom;
        int rev = rev_dir[door];
        if (room->exit[door] == NULL)
        continue;
        pToRoom = room->exit[door]->u1.to_room;

        if (pToRoom != NULL) {
          if (pToRoom->exit[rev]) {
            free_exit(pToRoom->exit[rev]);
            pToRoom->exit[rev] = NULL;
          }
        }

        free_exit(room->exit[door]);
        room->exit[door] = NULL;
      }
    }

    for (pExtra = room->extra_descr; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    for (pExtra = room->places; pExtra; pExtra = pExtra->next) {
      free_extra_descr(pExtra);
    }

    if (room->reset_first == NULL && room->reset_last != NULL)
    room->reset_first = room->reset_last;
    for (; room->reset_last != NULL;) {
      room->reset_first = room->reset_first->next;
      if (!room->reset_first)
      room->reset_last = NULL;
    }
    /*
for ( pReset = room->reset_first; pReset; pReset = pReset->next ){
free_reset_data( pReset );
}
*/
    room->sector_type = SECT_FOREST;

    room->x = -999;
    room->y = -999;
    room->z = -999;

    if (IS_SET(room->room_flags, ROOM_STASH))
    TOGGLE_BIT(room->room_flags, ROOM_STASH);

    if (IS_SET(room->room_flags, ROOM_INDOORS))
    TOGGLE_BIT(room->room_flags, ROOM_INDOORS);

    if (IS_SET(room->room_flags, ROOM_DARK))
    TOGGLE_BIT(room->room_flags, ROOM_DARK);

    if (IS_SET(room->room_flags, ROOM_LIGHTON))
    TOGGLE_BIT(room->room_flags, ROOM_LIGHTON);

    if (IS_SET(room->room_flags, ROOM_LIGHTOFF))
    TOGGLE_BIT(room->room_flags, ROOM_LIGHTOFF);

    if (IS_SET(room->room_flags, ROOM_PUBLIC))
    TOGGLE_BIT(room->room_flags, ROOM_PUBLIC);

    if (IS_SET(room->room_flags, ROOM_UNLIT))
    TOGGLE_BIT(room->room_flags, ROOM_UNLIT);

    return;
  }

  int get_coord(int room_number) {
    int coord = 0;

    // harvesting coordinate component and tier
    if (room_number < HavenUnderground) {
      // Incompatible areas
      return 0;
    }
    else if (room_number < HavenUnderground + 5000000) {
      // Haven underground
      coord = room_number - HavenUnderground;
      // check in private property and government
      if (get_room_index(coord + HavenCityBuildings) != NULL || get_room_index(coord + HavenPrivateProperty) != NULL) {
        return 0;
      }
    }
    else if (room_number < HavenStreets + 1000000) {
      // Haven streets
      coord = room_number - HavenStreets + HavenStreetsModifier;
    }
    else if (room_number < HavenSkies + 4000000) {
      // Haven skies
      coord = room_number - HavenSkies + HavenSkiesModifier;
      // check private property and government
      if (get_room_index(coord + HavenCityBuildings) != NULL || get_room_index(coord + HavenPrivateProperty) != NULL) {
        return 0;
      }
    }
    else if (room_number < HavenWoods + 1000000) {
      // Haven greenspace
      coord = room_number - HavenWoods + HavenWoodsModifier;
      // setting exits for greenspace when stuff is in the way is bad
      if (get_room_index(coord + HavenStreets - HavenStreetsModifier) != NULL || get_room_index(coord + HavenCityBuildings) != NULL || get_room_index(coord + HavenPrivateProperty) != NULL) {
        return 0;
      }
    }
    else if (room_number < HavenPrivateProperty + 10000000) {
      // Haven private property
      coord = room_number - HavenPrivateProperty;
      // check in streets and government for conflicts
      if (get_room_index(coord + HavenStreets - HavenStreetsModifier) != NULL || get_room_index(coord + HavenCityBuildings) != NULL) {
        return 0;
      }
    }
    else if (room_number < HavenCityBuildings + 10000000) {
      // Haven government and business
      coord = room_number - HavenCityBuildings;
      // check in streets
      if (get_room_index(coord + HavenStreets - HavenStreetsModifier) != NULL) {
        return 0;
      }
    }
    else if (room_number < HavenOcean + 6000000) {
      // Haven Ocean
      coord = room_number - HavenOcean;
    }

    return coord;
  }

  // makes actual exits between rooms and decides which area it goes to
  void link_room(int origin_Vnum, int coord, int direction) {
    int destination_Vnum, coord_modifier;
    ROOM_INDEX_DATA *destination_RID;

    if (direction == DIR_NORTH) {
      coord_modifier = 1;
    }
    else if (direction == DIR_EAST) {
      coord_modifier = -1000;
    }
    else if (direction == DIR_SOUTH) {
      coord_modifier = -1;
    }
    else if (direction == DIR_WEST) {
      coord_modifier = 1000;
    }
    else if (direction == DIR_NORTHWEST) {
      coord_modifier = 1001;
    }
    else if (direction == DIR_NORTHEAST) {
      coord_modifier = -999;
    }
    else if (direction == DIR_SOUTHEAST) {
      coord_modifier = -1001;
    }
    else if (direction == DIR_SOUTHWEST) {
      coord_modifier = 999;
    }
    else if (direction == DIR_UP) {
      coord_modifier = 1000000;
    }
    else if (direction == DIR_DOWN) {
      coord_modifier = -1000000;
    }

    // private prop
    destination_Vnum = coord + coord_modifier + HavenPrivateProperty;
    destination_RID = get_room_index(destination_Vnum);
    if (destination_RID != NULL && strcasestr(destination_RID->name, "delete") == NULL) {
      make_exit(origin_Vnum, destination_Vnum, direction, CONNECT_NODOOR);
    }
    else {
      // government
      destination_Vnum = coord + coord_modifier + HavenCityBuildings;
      destination_RID = get_room_index(destination_Vnum);
      if (destination_RID != NULL && strcasestr(destination_RID->name, "delete") == NULL) {
        make_exit(origin_Vnum, destination_Vnum, direction, CONNECT_NODOOR);
      }
      else {
        // streets/underground/air
        destination_Vnum =
        coord + coord_modifier + HavenStreets - HavenStreetsModifier;
        destination_RID = get_room_index(destination_Vnum);
        if (destination_RID != NULL && strcasestr(destination_RID->name, "delete") == NULL) {
          make_exit(origin_Vnum, destination_Vnum, direction, CONNECT_NODOOR);
        }
        else {
          // greenspace
          destination_Vnum =
          coord + coord_modifier + HavenWoods - HavenWoodsModifier;
          destination_RID = get_room_index(destination_Vnum);
          if (destination_RID != NULL && strcasestr(destination_RID->name, "delete") == NULL) {
            make_exit(origin_Vnum, destination_Vnum, direction, CONNECT_NODOOR);
          }
          else {
            // ocean
            destination_Vnum = coord + coord_modifier + HavenOcean;
            destination_RID = get_room_index(destination_Vnum);
            if (destination_RID != NULL && strcasestr(destination_RID->name, "delete") == NULL) {
              make_exit(origin_Vnum, destination_Vnum, direction, CONNECT_NODOOR);
            }
          }
        }
      }
    }
  }

  // integrates rooms into surroundings; calls link_room
  void integrate_room(int room_number) {
    ROOM_INDEX_DATA *room;

    int coord, i, z;
    bool no_exit = FALSE;

    room = get_room_index(room_number);

    if (room == NULL || strcasestr(room->name, "delete")) {
      return;
    }

    /*
if (room->area == get_vnum_area(HavenWoods) || room->area ==
get_vnum_area(HavenOcean) || room->area == get_vnum_area(HavenSkies) || room->area == get_vnum_area(HavenUnderground)) { reset_room_index(room);
}
*/

    coord = get_coord(room_number);

    // north direction
    no_exit = FALSE;

    for (z = 0; z < 10000000; z = z + 1000000) {
      for (i = 999; i < 1000000; i = i + 1000) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
    }

    if (no_exit == FALSE) {
      link_room(room_number, coord, DIR_NORTH);
    }

    // east direction
    no_exit = FALSE;

    for (z = 0; z < 10000000; z = z + 1000000) {
      for (i = 0; i <= 999; i = i + 1) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
    }
    if (no_exit == FALSE) {
      link_room(room_number, coord, DIR_EAST);
    }

    // south direction
    for (z = 0; z < 10000000; z = z + 1000000) {
      for (i = 0; i < 1000000; i = i + 1000) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
    }
    if (no_exit == FALSE) {
      link_room(room_number, coord, DIR_SOUTH);
    }

    // west direction
    no_exit = FALSE;
    for (z = 0; z < 1000000; z = z + 1000000) {
      for (i = 999000 + z; i <= 1000000 + z; i = i + 1) {
        if (coord == i) {
          no_exit = TRUE;
        }
      }
    }

    if (no_exit == FALSE) {
      link_room(room_number, coord, DIR_WEST);
    }

    // northwest direction
    no_exit = FALSE;
    for (z = 0; z < 1000000; z = z + 1000000) {
      for (i = 999; i < 1000000; i = i + 1000) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
      for (i = 999000 + z; i <= 1000000 + z; i = i + 1) {
        if (coord == i) {
          no_exit = TRUE;
        }
      }
    }
    if (no_exit == FALSE) {
      link_room(room_number, coord, DIR_NORTHWEST);
    }

    // northeast direction
    no_exit = FALSE;
    for (z = 0; z < 10000000; z = z + 1000000) {
      for (i = 999; i < 1000000; i = i + 1000) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
      for (i = 0; i <= 999; i = i + 1) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
    }
    if (no_exit == FALSE) {
      link_room(room_number, coord, DIR_NORTHEAST);
    }

    // southeast direction
    no_exit = FALSE;
    for (z = 0; z < 10000000; z = z + 1000000) {
      for (i = 0; i < 1000000; i = i + 1000) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
      for (i = 0; i <= 999; i = i + 1) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
    }

    if (coord > 100 && no_exit == FALSE) {
      link_room(room_number, coord, DIR_SOUTHEAST);
    }

    // southwest direction
    no_exit = FALSE;
    for (z = 0; z < 10000000; z = z + 1000000) {
      for (i = 0; i < 1000000; i = i + 1000) {
        if (coord == i + z) {
          no_exit = TRUE;
        }
      }
      for (i = 999000 + z; i <= 1000000 + z; i = i + 1) {
        if (coord == i) {
          no_exit = TRUE;
        }
      }
    }

    if (no_exit == FALSE) {
      link_room(room_number, coord, DIR_SOUTHWEST);
    }

    // up direction
    // if(coord>0 && coord<9000000) {
    link_room(room_number, coord, DIR_UP);
    //}

    // down direction
    // if(coord>1000000 && coord<10000000) {
    link_room(room_number, coord, DIR_DOWN);
    //}
    return;
  }

  // creates a room
  void create_room(int iVnum) {
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int iHash;
    if (get_room_index(iVnum) != NULL)
    return;
    pArea = get_vnum_area(iVnum);
    pRoom = new_room_index();
    pRoom->area = pArea;
    pRoom->vnum = iVnum;

    // I have no idea how this part works.  It indexes the rooms I think -
    // Discordance
    iHash = iVnum % MAX_KEY_HASH;
    pRoom->next = room_index_hash[iHash];
    room_index_hash[iHash] = pRoom;
    return;
  }

  // Ocean rooms
  void pour_ocean(int i) {
    ROOM_INDEX_DATA *room;

    // if room doesn't exist
    if ((room = get_room_index(i)) == NULL) {
      //&& if street doesn't exist
      if ((room = get_room_index(i - HavenOcean + HavenStreets -
              HavenStreetsModifier)) == NULL) {
        //&& if forest doesn't exist
        if ((room = get_room_index(i - HavenOcean + HavenWoods)) == NULL) {
          //&& if private property doesn't exist
          if ((room = get_room_index(i - HavenOcean + HavenPrivateProperty)) ==
              NULL) {
            //&& if government and business doesn't exist
            if ((room = get_room_index(i - HavenOcean + HavenCityBuildings)) ==
                NULL) {
              create_room(i);
              integrate_room(i);
            }
          }
        }
      }
    }
    else {
      integrate_room(i);
      if (i < HavenOcean + 5000000) {
        if ((get_room_index(i + 1000000)) != NULL) {
          room->sector_type = SECT_UNDERWATER;
          free_string(room->name);
          room->name = str_dup("`CU`cn`Cd`ce`Cr `ct`Ch`ce `CS`ce`Ca`x");
          room->description = str_dup("");
          room->shroud = str_dup("");
          SET_BIT(room->room_flags, ROOM_UNLIT);
          room->size = 100;
        }
      }
      else {
        room->sector_type = SECT_WATER;
        free_string(room->name);
        room->name = str_dup("`WThe `BOcean`x");
        room->description = str_dup("");
        room->shroud = str_dup("");
        SET_BIT(room->room_flags, ROOM_UNLIT);
        room->size = 100;
      }
    }
    return;
  }

  // This part creates forest rooms
  void grow_forest(int i) {
    ROOM_INDEX_DATA *room;
    int coord;

    coord = get_coord(i);

    // if room doesn't exist
    if ((room = get_room_index(i)) == NULL) {
      //&& if street doesn't exist
      if ((room = get_room_index(coord + HavenStreets - HavenStreetsModifier)) ==
          NULL) {
        //&& if private property doesn't exist
        if ((room = get_room_index(coord + HavenPrivateProperty)) == NULL) {
          //&& if government and business doesn't exist
          if ((room = get_room_index(coord + HavenCityBuildings)) == NULL) {
            //&& we're not in the ocean
            if ((room = get_room_index(coord + HavenOcean)) == NULL) {
              create_room(i);
              integrate_room(i);
            }
          }
        }
      }
    }
    // if room exists already
    else {
      // specific subarea mapping
      if (room->description == str_dup("") || room->size == 0) {
        /*
if ((i>= && i<=)) {
integrate_room(i);
room->sector_type = SECT_BEACH;
free_string(room->name);
room->name = str_dup("`gW`di`gl`dl`go`dw`x `cCove`x
beach"); room->description=str_dup(""); room->shroud=str_dup("");
SET_BIT(room->room_flags, ROOM_UNLIT);
SET_BIT(room->room_flags, ROOM_PUBLIC);
room->size=100;
}
*/
        if (room->sector_type == SECT_ROCKY) {
          integrate_room(i);
          room->sector_type = SECT_ROCKY;
          free_string(room->name);
          room->name = str_dup("`WRocky `YS`yh`Yo`yr`Ye`x");
          room->description = str_dup("");
          room->shroud = str_dup("");
          SET_BIT(room->room_flags, ROOM_UNLIT);
          SET_BIT(room->room_flags, ROOM_PUBLIC);
          room->size = 100;
        }
        else if (room->sector_type == SECT_WATER) {
          if (room->name == str_dup("`YC`yo`Ya`ys`Yt`ya`Yl `CW`ca`Ct`ce`Cr`x")) {
            reset_room_index(room);
            room->name = str_dup("delete");

            create_room(coord + HavenOcean);
            if ((room = get_room_index(coord + HavenOcean)) != NULL) {
              room->sector_type = SECT_WATER;
              room->name = str_dup("`WThe `BOcean`x");
              room->description = str_dup("");
              room->shroud = str_dup("");
              SET_BIT(room->room_flags, ROOM_UNLIT);
              room->size = 100;
            }
          }
          else {
            integrate_room(i);
            room->sector_type = SECT_WATER;
            free_string(room->name);
            room->name = str_dup("`WThe `CR`ci`Cv`ce`Cr`x");
            room->description = str_dup("");
            room->shroud = str_dup("");
            SET_BIT(room->room_flags, ROOM_UNLIT);
            SET_BIT(room->room_flags, ROOM_PUBLIC);
            room->size = 100;
          }
        }
        else if (room->sector_type == SECT_PARK) {
          integrate_room(i);
          room->sector_type = SECT_PARK;
          free_string(room->name);
          room->name = str_dup("`WHaven `GF`gi`Ge`gl`Gd`x");
          room->description = str_dup("");
          room->shroud = str_dup("");
          SET_BIT(room->room_flags, ROOM_UNLIT);
          SET_BIT(room->room_flags, ROOM_PUBLIC);
          room->size = 100;
        }
        else if (room->sector_type == SECT_BEACH) {
          integrate_room(i);
          room->sector_type = SECT_BEACH;
          free_string(room->name);
          room->name = str_dup("`gW`wi`gll`wo`gw `YC`yo`Yv`ye`x");
          room->description = str_dup("");
          room->shroud = str_dup("");
          SET_BIT(room->room_flags, ROOM_UNLIT);
          SET_BIT(room->room_flags, ROOM_PUBLIC);
          room->size = 100;
        }
        // else if(room->sector_type == SECT_FOREST){
        else {
          integrate_room(i);
          room->sector_type = SECT_FOREST;
          free_string(room->name);
          room->name = str_dup("`GT`gh`Ge `GF`go`Dr`ge`Gs`gt`x");
          room->description = str_dup("");
          room->shroud = str_dup("");
          SET_BIT(room->room_flags, ROOM_UNLIT);
          room->size = 100;
        }
      }
    }
    return;
  }

  // makes sky rooms
  void paint_skies(int i) {
    int coord;
    ROOM_INDEX_DATA *room;

    coord = i - HavenSkies + 6000000;
    // if room doesn't exist
    if ((room = get_room_index(i)) == NULL && (room = get_room_index(HavenPrivateProperty + coord)) == NULL && (room = get_room_index(HavenCityBuildings + coord)) == NULL) {
      // if room exists below
      if ((room = get_room_index(coord + HavenStreets - 5000000 - 1000000)) !=
          NULL // Street room is below
          || (room = get_room_index(coord + HavenPrivateProperty - 1000000)) !=
          NULL // priv prop room is below
          || (room = get_room_index(coord + HavenCityBuildings - 1000000)) !=
          NULL // government and business is below
          || (room = get_room_index(coord + HavenWoods - 5000000 - 1000000)) !=
          NULL) { // greenspace is below
        // if story > 1 and a room exists below which isn't a sky room
        if (coord < 7000000) {
          // create a sky room
          create_room(i);
          integrate_room(i);
        }
        else {
          if ((room = get_room_index(coord - 7000000 + HavenSkies)) == NULL) {
            // create sky in current room and each adjacent room so long as not
            // occupied Center room
            create_room(i);
            integrate_room(i);
            // NW
            if ((room = get_room_index(HavenPrivateProperty + coord + 1001)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord + 1001)) ==
                NULL && (room = get_room_index(i + 1001)) == NULL) {
              create_room(i + 1001);
              integrate_room(i + 1001);
            }
            // N
            if ((room = get_room_index(HavenPrivateProperty + coord + 1)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord + 1)) == NULL && (room = get_room_index(i + 1)) == NULL) {
              create_room(i + 1);
              integrate_room(i + 1);
            }
            // NE
            if ((room = get_room_index(HavenPrivateProperty + coord - 999)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord - 999)) ==
                NULL && (room = get_room_index(i - 999)) == NULL) {
              create_room(i - 999);
              integrate_room(i - 999);
            }
            // E
            if ((room = get_room_index(HavenPrivateProperty + coord - 1000)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord - 1000)) ==
                NULL && (room = get_room_index(i - 1000)) == NULL) {
              create_room(i - 1000);
              integrate_room(i - 1000);
            }
            // SE
            if ((room = get_room_index(HavenPrivateProperty + coord - 1001)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord - 1001)) ==
                NULL && (room = get_room_index(i - 1001)) == NULL) {
              create_room(i - 1001);
              integrate_room(i - 1001);
            }
            // S
            if ((room = get_room_index(HavenPrivateProperty + coord - 1)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord - 1)) == NULL && (room = get_room_index(i - 1)) == NULL) {
              create_room(i - 1);
              integrate_room(i - 1);
            }
            // SW
            if ((room = get_room_index(HavenPrivateProperty + coord + 999)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord + 999)) ==
                NULL && (room = get_room_index(i + 999)) == NULL) {
              create_room(i + 999);
              integrate_room(i + 999);
            }
            // W
            if ((room = get_room_index(HavenPrivateProperty + coord + 1000)) ==
                NULL && (room = get_room_index(HavenCityBuildings + coord + 1000)) ==
                NULL && (room = get_room_index(i + 1)) == NULL) {
              create_room(i + 1);
              integrate_room(i + 1);
            }
          }
        }
      }
    }
    else if ((room = get_room_index(i)) != NULL) {
      if (room->description == str_dup("") || room->size == 0) {
        integrate_room(i);
        room->sector_type = SECT_AIR;
        free_string(room->name);
        room->name = str_dup("`CSkies`x");
        room->description = str_dup("");
        room->shroud = str_dup("");
        SET_BIT(room->room_flags, ROOM_UNLIT);
        SET_BIT(room->room_flags, ROOM_PUBLIC);
        room->size = 100;
      }
    }
    return;
  }

  void make_tunnels() {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area->vnum == 17 && pRoomIndex->sector_type == SECT_STREET) {
          smartdemolish(pRoomIndex, DIR_DOWN, 1);
        }
      }
    }
  }

  void make_skies() {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (get_roomz(pRoomIndex) == 0) {
          smartdemolish(pRoomIndex, DIR_UP, 1);
        }
      }
    }
  }

  bool straightroom(ROOM_INDEX_DATA *room) {

    if (room->sector_type == SECT_BEACH)
    return TRUE;

    for (int door = 0; door <= 9; door++) {
      if (room->exit[door] != NULL && room->exit[door]->u1.to_room != NULL) {
        if (room->exit[door]->u1.to_room->sector_type == SECT_BEACH)
        return TRUE;

        if (room->sector_type != SECT_WATER && room->sector_type != SECT_UNDERWATER && (room->exit[door]->u1.to_room->sector_type == SECT_WATER || room->exit[door]->u1.to_room->sector_type == SECT_UNDERWATER))
        return TRUE;

        if (room->sector_type != room->exit[door]->u1.to_room->sector_type)
        return TRUE;
      }
    }

    return FALSE;
  }

  void level_forest() {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        int x = get_roomx(pRoomIndex);
        int y = get_roomy(pRoomIndex);
        if (x <= 550 && x >= 450 && y <= 550 && y >= 450)
        continue; // In town.
        /*
int dists[10];

dists[0] = get_dist(x, y, 450, 450);
dists[1] = get_dist(x, y, 550, 450);
dists[2] = get_dist(x, y, 450, 550);
dists[3] = get_dist(x, y, 550, 550);
dists[4] = get_dist(x, y, 500, 450);
dists[5] = get_dist(x, y, 500, 550);
dists[6] = get_dist(x, y, 550, 500);
dists[7] = get_dist(x, y, 450, 500);
dists[8] = get_dist(x, y, 450, 525);
dists[9] = get_dist(x, y, 450, 575);
*/
        if (pRoomIndex->sector_type != SECT_FOREST) {
          pRoomIndex->level = 0;
          continue;
        }
        int dist = 1000;

        int newy = 450;
        int newx = 450;
        for (newy = 450; newy <= 550; newy += 5) {
          for (newx = 450; newx <= 550; newx += 5) {
            if (get_dist(x, y, newx, newy) < dist)
            dist = get_dist(x, y, newx, newy);
          }
        }
        /*
for(int i=0;i<10;i++)
{
if(dists[i] < dist && i < 8)
dist = dists[i];
}
*/
        int level = (int)(cbrt(dist) * sqrt(dist) * dist);
        level = level * 5 / 4;
        level = UMAX(20, level + 20);
        level = UMIN(level, 750);
        pRoomIndex->level = level;
      }
    }
  }

  void make_loops() {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (get_roomy(pRoomIndex) == 580) {
          if (get_roomx(pRoomIndex) > 500) {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6), DIR_NORTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6 - 1000), DIR_NORTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6 + 1000), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5 - 1000), DIR_NORTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5), DIR_NORTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5 - 2000), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
          }
          else {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6), DIR_NORTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6 - 1000), DIR_NORTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6 + 1000), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5 + 1000), DIR_NORTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5 + 2000), DIR_NORTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
          }
        }
        else if (get_roomy(pRoomIndex) == 420) {
          if (get_roomx(pRoomIndex) > 500) {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6), DIR_SOUTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6 - 1000), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6 + 1000), DIR_SOUTHEAST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5 - 1000), DIR_SOUTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5 - 2000), DIR_SOUTHEAST, CONNECT_ONEWAY);
            }
          }
          else {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6), DIR_SOUTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6 - 1000), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6 + 1000), DIR_SOUTHEAST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5 + 1000), DIR_SOUTH, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5 + 2000), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5), DIR_SOUTHEAST, CONNECT_ONEWAY);
            }
          }
        }
        else if (get_roomx(pRoomIndex) == 580) {
          if (get_roomy(pRoomIndex) > 500) {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6000), DIR_WEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6000 - 1), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6000 + 1), DIR_NORTHWEST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5000 - 1), DIR_WEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5000 - 2), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5000), DIR_NORTHWEST, CONNECT_ONEWAY);
            }
          }
          else {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6000), DIR_WEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6000 - 1), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 6000 + 1), DIR_NORTHWEST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5000 + 1), DIR_WEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5000), DIR_SOUTHWEST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum - 5000 + 2), DIR_NORTHWEST, CONNECT_ONEWAY);
            }
          }
        }
        else if (get_roomx(pRoomIndex) == 420) {
          if (get_roomy(pRoomIndex) > 500) {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6000), DIR_EAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6000 - 1), DIR_SOUTHEAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6000 + 1), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5000 - 1), DIR_EAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5000 - 2), DIR_SOUTHEAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5000), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
          }
          else {
            if (straightroom(pRoomIndex)) {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6000), DIR_EAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6000 - 1), DIR_SOUTHEAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 6000 + 1), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
            else {
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5000 + 1), DIR_EAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5000), DIR_SOUTHEAST, CONNECT_ONEWAY);
              make_exit(pRoomIndex->vnum, (pRoomIndex->vnum + 5000 + 2), DIR_NORTHEAST, CONNECT_ONEWAY);
            }
          }
        }
      }
    }
  }

  _DOFUN(do_tyrbeach) {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    int num = atoi(arg1);

    for (iHash = num * 1000; iHash < UMIN(num * 1000 + 1000, MAX_KEY_HASH);
    iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (get_roomy(pRoomIndex) <= 452 && get_roomz(pRoomIndex) == atoi(argument)) {
          smartdemolish(pRoomIndex, DIR_DOWN, 5);
        }
      }
    }

    /*
for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
{
for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex =
pRoomIndex->next )
{
if ( get_roomy(pRoomIndex) <= 452 && get_roomz(pRoomIndex) == 5)
{
smartdemolish(pRoomIndex, DIR_DOWN, 5);
}
}
}

for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
{
for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex =
pRoomIndex->next )
{
if ( get_roomy(pRoomIndex) <= 452 && get_roomz(pRoomIndex) == 4)
{
smartdemolish(pRoomIndex, DIR_DOWN, 5);
}
}
}
for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
{
for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex =
pRoomIndex->next )
{
if ( get_roomy(pRoomIndex) <= 452 && get_roomz(pRoomIndex) == 3)
{
smartdemolish(pRoomIndex, DIR_DOWN, 5);
}
}
}
for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
{
for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex =
pRoomIndex->next )
{
if ( get_roomy(pRoomIndex) <= 452 && get_roomz(pRoomIndex) == 2)
{
smartdemolish(pRoomIndex, DIR_DOWN, 5);
}
}
}
for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
{
for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex =
pRoomIndex->next )
{
if ( get_roomy(pRoomIndex) <= 452 && get_roomz(pRoomIndex) == 6)
{
smartdemolish(pRoomIndex, DIR_DOWN, 5);
}
}
}
*/
  }

  // keeps overlapping areas in sync and builds rooms if appropriate; calls
  // integrate_room and create_room
  void world_sync(char type[MSL]) {
    ROOM_INDEX_DATA *room;
    int i, x, y, z;

    // Streets
    if (!str_cmp(type, "streets") || !str_cmp(type, "street")) {
      for (i = HavenStreets; i < HavenStreets + 1000000; i++) {
        if ((room = get_room_index(i)) == NULL) {
          continue;
        }

        if (room->description == str_dup("") || room->size == 0) {
          // reset_room_index(room);
          integrate_room(i);
          room->description = str_dup("");
          room->shroud = str_dup("");
          room->sector_type = SECT_STREET;
          room->size = 100;
        }
      }
    }
    else if (!str_cmp(type, "clearwoods")) {
      for (x = HavenWoods + 420000; x <= HavenWoods + 580000; x = x + 1000) {
        for (y = x + 420; y <= x + 580; y++) {
          if ((room = get_room_index(y)) != NULL && room->sector_type == SECT_FOREST && room->description == str_dup("")) {
            reset_room_index(room);
            room->name = str_dup("delete");
          }
        }
      }
    }
    else if (!str_cmp(type, "forest") || !str_cmp(type, "woods") || !str_cmp(type, "fields") || !str_cmp(type, "field")) {
      // Buildable Fields and Forest and Water
      for (x = HavenWoods + 420000; x <= HavenWoods + 580000; x = x + 1000) {
        for (y = x + 420; y <= x + 580; y++) {
          // runs twice to make sure newly created rooms get set up
          grow_forest(y);
          grow_forest(y);
        }
      }
    }
    else if (!str_cmp(type, "clearskies")) {
      for (z = 0; z < 10000000; z = z + 1000000) {
        for (x = HavenSkies + 420000 + z; x <= HavenSkies + 580000 + z;
        x = x + 1000) {
          for (y = x + 420; y <= x + 580; y++) {
            if ((room = get_room_index(y)) != NULL && room->sector_type == SECT_AIR && room->description == str_dup("")) {
              reset_room_index(room);
              room->name = str_dup("delete");
            }
          }
        }
      }
    }
    // Sky
    else if (!str_cmp(type, "sky") || !str_cmp(type, "skies")) {
      for (z = 0; z < 10000000; z = z + 1000000) {
        for (x = HavenSkies + 420000 + z; x <= HavenSkies + 580000 + z;
        x = x + 1000) {
          for (y = x + 420; y <= x + 580; y++) {
            // runs twice to make sure newly created rooms get set up
            paint_skies(y);
            paint_skies(y);
          }
        }
      }
    }
    else if (!str_cmp(type, "clearwater")) {
      for (z = 0; z < 10000000; z = z + 1000000) {
        for (x = HavenOcean + 420000 + z; x <= HavenOcean + 580000 + z;
        x = x + 1000) {
          for (y = x + 420; y <= x + 580; y++) {
            if ((room = get_room_index(y)) != NULL && room->sector_type == SECT_WATER && room->description == str_dup("")) {
              reset_room_index(room);
              room->name = str_dup("delete");
            }
          }
        }
      }
    }
    // The Ocean
    else if (!str_cmp(type, "ocean") || !str_cmp(type, "sea")) {
      for (z = 4000000; z <= 5000000; z = z + 1000000) {
        for (x = HavenOcean + 420000 + z; x <= HavenOcean + 580000 + z;
        x = x + 1000) {
          for (y = x + 420; y <= x + 580; y++) {
            // runs twice to make sure newly created rooms get set up
            pour_ocean(y);
            pour_ocean(y);
          }
        }
      }
    }
    else if (!str_cmp(type, "tunnels")) {
      make_tunnels();
    }

    else if (!str_cmp(type, "edges")) {
      make_loops();
    }

    else if (!str_cmp(type, "levels")) {
      level_forest();
    }

    return;
  }

  _DOFUN(do_wonderland) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    world_sync(arg1);
    /*
ROOM_INDEX_DATA *room;
ROOM_INDEX_DATA *roomtwo;
EXIT_DATA *pexit;

int i;

if(!str_cmp(argument, "1"))
{
for(i=1000;i<1999;i++)
{
room = get_room_index(i);
if(room == NULL)
continue;

if(room->sector_type == SECT_STREET)
{
free_string(room->description);
room->description = str_dup("Smooth asphalt roads continue
through this part of town, bordered on either\nside by well maintained
concrete sidewalks. The aluminum streetlights are\npainted a deep green and
appear regularly along the side, illuminating the\nstreet in spots of warm
electric light when it's dark. Where the street is\nwidest, small median
islands appear, with old twisted trees planted in them.\nThe buildings that
line the street seem quaint but well maintained.\n");
free_string(room->shroud);
room->shroud = str_dup("Dark cobbled roads are partially
hidden by wisps of white mist that ebb and\nswirl near the ground. Stone
footpaths line either side, punctuated regularly\nby black iron gas
streetlamps choked by ivy vines. They cast flickering orange\ncircles of
light on the ground that seem to reach for passersby, drawing them\nin.
Where the road is widest, small stone plinths appear with old
twisted\ntrees planted inside them, their knots and swirls forming faces
with eyes\nthat seem to follow you wherever you move.\n");
}
}
}
if(!str_cmp(argument, "2"))
{
for(i=2000;i<2999;i++)
{
room = get_room_index(i);
if(room == NULL)
continue;

if(room->sector_type == SECT_TUNNELS)
{
free_string(room->name);
room->name = str_dup("`cStone`D Tunnel`x");
free_string(room->description);
room->description = str_dup("This sewer tunnel is well
maintained but clearly old, rendered from worn stone\nwith regular arches
set at regular intervals for support. The rainwater washes\ndown the center
of the tunnel heading south, while small walkways to either\nside allow
careful passage; the stone is slippery when wet, and pale moss\ngrows
unevenly here.\n"); free_string(room->shroud); room->shroud = str_dup("This
tunnel is wreathed in mist that swirls and flows with the
slightest\nmovement. The passage itself is made from pale stone, with
ornate symbols\netched into it at regular intervals. Arches, carved with
patterns of ivy, are\nused for support. The flowing water makes strange
gurgling noises, almost as\nif alive, and shadows seem to constantly move
in the corner of your eye.\n");
}
}
}
if(!str_cmp(argument, "3"))
{
for(i=3000;i<5000;i++)
{
room = get_room_index(i);
if(room == NULL)
continue;

if(room->sector_type == SECT_AIR)
{
pexit = room->exit[DIR_DOWN];
if(pexit == NULL)
continue;

roomtwo = pexit->u1.to_room;
if(roomtwo == NULL)
continue;

if(roomtwo->vnum >= 1000 && roomtwo->vnum < 1999)
{
free_string(room->name);
room->name = str_dup("`CSkies`x");
free_string(room->description);
room->description = str_dup("The air up here is crisp and
clean, and the wind makes it feel cooler than it\nis at ground level.
Below, the streets stretch out, winding through the town,\nand the branches
of some of the taller trees compete with the larger buildings\nas they
reach up towards you from below. In the distance to the south, the
deep\nblue of the ocean can be seen reflecting the light while the deep
green of the\nforest stretches out around the town in all other
directions.\n"); free_string(room->shroud); room->shroud = str_dup("The air
up here is crisp and clean, and the wind makes it feel cooler than it\nis
at ground level. Below, the mist-shrouded streets stretch out, winding\nthrough the town. The treetops seem to twist and follow you as you
move through\nthe air, while strange lights and sounds emanate from ground
level.\n");
}
else if(roomtwo->vnum >= 10000 && roomtwo->vnum < 11999)
{
free_string(room->name);
room->name = str_dup("`CSkies`x");
free_string(room->description);
room->description = str_dup("The air up here is crisp and
clean, and the wind makes it feel cooler than it\nis at ground level.
Below, the park stretches out, fields of green punctuated\nby old trees.
Buildings and winding streets can be seen nearby, while further\nto the
south the deep blue of the ocean can be seen reflecting the light\nwhile
the deep green of the forest stretches out around the town in all
other\ndirections.\n"); free_string(room->shroud); room->shroud =
str_dup("The air up here is crisp and clean, and the wind makes it feel
cooler than it\nis at ground level. Below, the mist-shrouded roads stretch
out, winding through\nthe town. The treetops seem to twist and follow you
as you move through the air\nwhile strange lights and sounds emanate from
ground level.\n");
}
else if(roomtwo->vnum >= 12000 && roomtwo->vnum < 13999)
{
free_string(room->name);
room->name = str_dup("`CSkies`x");
free_string(room->description);
room->description = str_dup("The air up here is crisp and
clean and the wind makes it feel cooler than it\nis at ground level. Below, the ocean stretches out forever, waves of deepest\nblue crashing and
swirling. To the north, the small buildings of the town\nwrapped in green
forest can be clearly seen, while rocky bluffs to the west\nborder the
ocean.\n"); free_string(room->shroud); room->shroud = str_dup("The air up
here is crisp and clean, and the wind makes it feel cooler than\nit is at
ground level. Below, the mist-shrouded ocean stretches out forever,\nwaves
of deepest blue crashing and swirling. To the north, the small
buildings\nof the town wrapped in green forest can be clearly seen, while
rocky bluffs to\nthe west border the ocean. The swirl of the water below
you sounds almost like\na voice, entreating you to head further out, further and further towards the\npromise of a wonderful and beautiful
death.\n");
}
}
}
}
if(!str_cmp(argument, "4"))
{
for(i=10000;i<11999;i++)
{
room = get_room_index(i);
if(room == NULL)
continue;

if(room->sector_type == SECT_PARK)
{
if(room->vnum < 10080)
{
free_string(room->name);
room->name = str_dup("`yHazelwood `GP`ga`Gr`gk`x");
free_string(room->description);
room->description = str_dup("Rich green fields stretch out
all around you, the grass underfoot short but\nsoft, and large twisted
trees decorate the area sporadically. Squirrels are a\ncommon sight, darting up and around the trees while the buildings of the town\nare
visible in the distance.\n"); free_string(room->shroud); room->shroud =
str_dup("Rich green fields stretch out all around you, partially obscured
by the mist\nthat swirls through the area. The grass underfoot is short but
soft, and large\ntwisted trees dot the area sporadically, their knots and
whorls twisted into\ndread faces that observe you with a detached curiosity
as small glowing forms\nflit around their swaying branches.\n");
}
else
{
free_string(room->name);
room->name = str_dup("`DHaven `GF`gi`Ge`gl`Gd`x");
free_string(room->description);
room->description = str_dup("Rich green fields stretch out
all around you, the grass underfoot is short but\nsoft, and large twisted
trees decorate the area sporadically. Well-worn fields\ncan be seen nearby
with goalposts waiting for a team to come make them useful\nagain. Picnic
tables and stone grills are set up at regular intervals.\n");
free_string(room->shroud);
room->shroud = str_dup("Rich green fields stretch out all
around you, partially obscured by the mist\nthat swirls throughout the
area. The grass underfoot is short but soft, and\nlarge twisted trees dot
the area sporadically, their knots and whorls twisted\ninto dread faces
that observe you with detached curiosity as small glowing\nforms flit
around their branches.\n");
}
}
if(room->sector_type == SECT_WATER)
{
free_string(room->name);
room->name = str_dup("`BRiver`x");
free_string(room->description);
room->description = str_dup("The cool water flows quickly
but not dangerously so, meandering toward the\neast. The water is clear
enough that the bottom can usually be seen, lined with\nsmall pebbles, while the banks meet up with the soft grasses of the field. The\nodd tree
lines the worn footpath of the river, roots visible under the water
as\ntheir branches loom overhead.\n"); free_string(room->shroud);
room->shroud = str_dup("The surface of the cool water is
shrouded in mist as it flows quickly to the\neast, swirling and forming
small whirlpools as it goes. The sounds of the water\nmoving are almost
musical, the sighs and gurgles blending together harmoniously\nas it rushes
past. Several dark and twisted trees line the banks of the river,\nshadowy
shapes flitting constantly amongst their branches.\n");
}
if(room->sector_type == SECT_UNDERWATER)
{
free_string(room->name);
room->name = str_dup("`bUnder`x the `BRiver`x");
free_string(room->description);
room->description = str_dup("The clear cool water of the
river swirls around you as it makes its way to the\neast and south. The
skies and tree branches can be seen above you, distorted\nthrough the
water's surface, while to either side and below you can see the\nsmooth
pebbles that form the bottom of the river. The odd small green
plant\nbullies its way through the pebbles along the riverbed. Glimpses of
tiny,\nshimmering silver fish are often seen darting back and forth through
the water.\n"); free_string(room->shroud); room->shroud = str_dup("The
clear cool water of the river swirls around you as it makes its way to\nthe
east and south. The skies and tree branches can be seen above you, somehow\nominous through the distortion of the water, while to either side
and below\nyou can see the smooth pebbles that form the bottom of the
river. The odd\nsmall green plant bullies its way through the pebbles along
the riverbed,\nwatery arms weakly snatching at your limbs. Glimpses of
tiny, glowing\nfish are often seen darting back and forth through the
water.\n");
}

}
}
if(!str_cmp(argument, "5"))
{
for(i=12000;i<13999;i++)
{
room = get_room_index(i);
if(room == NULL)
continue;

if(room->sector_type == SECT_BEACH)
{
free_string(room->name);
room->name = str_dup("`BUnity `YC`yo`Yv`ye`x");
free_string(room->description);
room->description = str_dup("Soft white sand crunches and
sinks lightly underfoot, eagerly reflecting light\nand spotted with small
stones and seashells. To the south and east the ocean\nstretches as far as
the eye can see, bordered to the west by the slopes of\nthe rocky bluffs.
To the north, the tops of the buildings in town can just\nbarely be seen, along with the deep green of the forest that surrounds it.\n");
free_string(room->shroud);
room->shroud = str_dup("Soft pale sand crunches and sinks
lightly underfoot, its shine obscured by\nthe pure white mist that swirls
in from the ocean to the south, casting it\nin an ominous shade. To the
north, the town can just barely be seen while the\ndark forest extends far
to the east and west of it. Strange sounds come from\nthe ocean, the
crashing of the waves sounding almost like a voice that\nentreats those who
hear to enter it while promising a wonderful death.\n");
}
if(room->sector_type == SECT_WATER)
{
free_string(room->name);
room->name = str_dup("`CThe `BOcean`x");
free_string(room->description);
room->description = str_dup("The salty water is always
cool, no matter the air temperature. The currents\nare generally light but
still noticeable, ebbing and swirling around you. To\nthe north, the white
sandy beach beckons while rocky bluffs to the west\nblock out some of the
otherwise grand and unobstructed view of the sky. The\nrolling waves
continue as far as the eye can see to the south and east,\nwhile the
verdant green of the forests can be seen surrounding the town\nfurther to
the north, past the beachfront.\n"); free_string(room->shroud);
room->shroud = str_dup("Swirls of white mist cover the
surface of the ocean, making its surface almost\nimpossible to see. The
currents are noticeable, always seeming to tug at\npeople as if trying and
pull them further out, to lose them. The rocky bluffs\nlooming ominously
over the water to the west, while the dark forest surrounds\nthe town to
the north. The crashing of waves is all around, sounding almost\nlike a
voice that entreats those who hear it to swim out, out and out
always\nfurther to a promise of a wonderful and beautiful death.\n");
}
if(room->sector_type == SECT_UNDERWATER)
{
free_string(room->name);
room->name = str_dup("`DUnder`x `bthe `BOcean`x");
free_string(room->description);
room->description = str_dup("The salty water is always
cool no matter the air temperature; it ebbs and\nswirls around you with
currents that are light but noticeable. Above you, the\nsky is visible
through the distortion of the water while below and all around\nthe water
seems to continue forever into the darkness.\n");
free_string(room->shroud);
room->shroud = str_dup("The salty water is always cool no
matter the air temperature; it ebbs and\nswirls around you with noticeable
currents, always seeming to tug at people to\ntry and pull them further
out, to lose them. The area to the west is\nparticularly dark as rocks
block passage, while an almost faint light can be\nseen further to the
south and east, almost begging to be explored. The swirl\nof the water
around you sounds almost like a voice, entreating you to head\nfurther out, further and further towards the promise of a wonderful and\nbeautiful
death.\n");
}
if(room->sector_type == SECT_ROCKY)
{
free_string(room->name);
room->name = str_dup("`DBluffs`x");
free_string(room->description);
room->description = str_dup("Smooth pebbles punctuated by
larger and sharper rocks line the ground here as\nthe bluffs continue to
slope upwards the further south you go. The sound of\nthe ocean crashing
against the rocks below ever present. The view is grand,\nthe ocean can be
seen continuing forever to the south and east, while the\ntown wrapped in
its verdant forest is small but clear to the north.\n");
free_string(room->shroud);
room->shroud = str_dup("Smooth pebbles punctuated by large
and sharper rocks line the ground here as\nthe bluffs continue to slope
upwards the further south you go. The mist is\nsparse up here but still
exists, clinging stubbornly to the gaps and shadows\nbetween rocks. The
sounds of the misty ocean below beckon you, seeming almost\nlike a voice
that urges you to jump and dive and never surface again.\n");
}

}
}
if(!str_cmp(argument, "6"))
{
for(i=14000;i<18999;i++)
{
room = get_room_index(i);
if(room == NULL)
continue;

if(room->sector_type == SECT_FOREST)
{
free_string(room->name);
room->name = str_dup("`GT`gh`Ge `GF`go`Dr`ge`Gs`gt`x");
free_string(room->description);
room->description = str_dup("The forest extends through
this area and as far as the eye can see, old trees\nwith lush foliage
surround you, the light from above glimmers as it makes its\nway through
the canopy, coming down in sunbeams that spot the area. The ground\nbeneath
is soft and lined with small plants, mushrooms and fallen branches.\nThe
trunks of the trees continue on forever making seeing more than a dozen
feet\ndirectly in any direction impossible and making those who wander
through the\narea take meandering paths. Squirrels and other small
creatures dart about in\nthe branches of the trees.\n");
free_string(room->shroud);
room->shroud = str_dup("The forest extends through this
area and as far as the eye can see, swirling\nmist obscures the ground
while twisted and gnarled old trees surround you. Each\none with knots and
swirls that makes them look as if they have faces that watch\nyour progress
greedily. Barely any light makes it into the forest and that which\ndoes is
muted, the trunks of the trees continue on forever making seeing more\nthan
a dozen feet in any direction impossible and turning the whole place in
a\nominous and confusing maze. Strange sounds echo between the trunks of
the trees,\nsnarls and growls, strange moans and other sounds that are
almost but not quite\nlike that of an animal. Shapes move in the distance
too, shadows in the mist,\nsome small, but others not while faintly glowing
eyes peer from the shadows in\nthe trees above. Glowing small shapes of
many varied colors flit through the\nbranches of the old trees, and faint
giggling sounds can be heard from them,\nfilled with playfulness edged in
malice.\n");
}
}
}
*/
  }

  // I'm invading your file, mwahahahahaha.

  vector<LOCATION_TYPE *> locationVect;

  LOCATION_TYPE *nulllocation;

  void fread_location(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    LOCATION_TYPE *location;

    location = new_location();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'B':
        KEY("BattleClimate", location->battleclimate, fread_number(fp));
        KEY("BaseType", location->base_type, fread_number(fp));
        if (!str_cmp(word, "Basecore")) {
          location->base_faction_core = fread_number(fp);
          free_string(location->base_desc_core);
          location->base_desc_core = fread_string(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Basesect")) {
          location->base_faction_sect = fread_number(fp);
          free_string(location->base_desc_sect);
          location->base_desc_sect = fread_string(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Basecult")) {
          location->base_faction_cult = fread_number(fp);
          free_string(location->base_desc_cult);
          location->base_desc_cult = fread_string(fp);
          fMatch = TRUE;
        }

        break;
      case 'C':
        KEY("Continent", location->continent, fread_number(fp));
        break;
      case 'O':
        if (!str_cmp(word, "Other")) {
          int i;
          for (i = 0; i < 10 && location->other_amount[i] > 0; i++) {
          }
          location->other_amount[i] = fread_number(fp);
          location->other_name[i] = fread_string(fp);
          fMatch = TRUE;
        }
        KEY("Order", location->order, fread_number(fp));
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!location->name) {
            bug("Fread_location: Name not found.", 0);
            free_location(location);
            return;
          }
          locationVect.push_back(location);
          return;
        }
        break;
      case 'F':
        KEY("Fixtures", location->fixtures, fread_string(fp));
        break;
      case 'G':
        KEY("Government", location->government, fread_string(fp));
        break;
      case 'H':
        KEY("Hand", location->hand, fread_number(fp));
        break;
      case 'L':
        KEY("Lockout", location->lockout, fread_number(fp));
        break;
      case 'N':
        KEY("Name", location->name, fread_string(fp));
        KEY("Notes", location->notes, fread_string(fp));
        break;
      case 'P':
        KEY("People", location->people, fread_string(fp));
        KEY("Place", location->place, fread_string(fp));
        if (!str_cmp(word, "Phil")) {
          for (int i = 0; i < 20; i++)
          location->phil_amount[i] = fread_number(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Plant")) {
          int i;
          for (i = 0; i < 20 && safe_strlen(location->plant_desc[i]) > 1; i++) {
          }
          free_string(location->plant_desc[i]);
          location->plant_desc[i] = fread_string(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Place")) {
          int i;
          for (i = 0; i < 20 && safe_strlen(location->place_desc[i]) > 1; i++) {
          }
          free_string(location->place_desc[i]);
          location->place_desc[i] = fread_string(fp);
          fMatch = TRUE;
        }
        break;
      case 'S':
        KEY("Status", location->status, fread_number(fp));
        KEY("StatusTimer", location->status_timer, fread_number(fp));
        break;
      case 'T':
        KEY("Temple", location->temple, fread_number(fp));
        KEY("Timezone", location->timezone, fread_number(fp));
        KEY("Timeline", location->timeline, fread_string(fp));
        break;
      case 'X':
        KEY("XCoord", location->x, fread_number(fp));
        break;
      case 'Y':
        KEY("YCoord", location->y, fread_number(fp));
      }

      if (!fMatch) {
        sprintf(buf, "Fread_location: no match: %s", word);
        bug(buf, 0);
      }
    }
  }

  void load_locations() {
    nulllocation = new_location();
    FILE *fp;

    if ((fp = fopen(LOCATION_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_locations: # not found.", 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "LOCATION")) {
          fread_location(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_locations: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open locations.txt", 0);
      exit(0);
    }
  }

  void save_locations_backup() {
    FILE *fpout;
    int i = 0;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/locations.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/locations.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/locations.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/locations.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/locations.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/locations.txt");
    else
    sprintf(buf, "../data/back7/locations.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open locations.txt for writing", 0);
      return;
    }

    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        bug("Save_stories: Blank location in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      fprintf(fpout, "#LOCATION\n");
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Continent %d\n", (*it)->continent);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Notes %s~\n", (*it)->notes);
      fprintf(fpout, "Hand %d\n", (*it)->hand);
      fprintf(fpout, "Order %d\n", (*it)->order);
      fprintf(fpout, "Temple %d\n", (*it)->temple);
      fprintf(fpout, "Lockout %d\n", (*it)->lockout);
      fprintf(fpout, "XCoord %d\n", (*it)->x);
      fprintf(fpout, "YCoord %d\n", (*it)->y);
      fprintf(fpout, "BattleClimate %d\n", (*it)->battleclimate);
      fprintf(fpout, "Timezone %d\n", (*it)->timezone);
      fprintf(fpout, "Government %s~\n", (*it)->government);
      fprintf(fpout, "Fixtures %s~\n", (*it)->fixtures);
      fprintf(fpout, "People %s~\n", (*it)->people);
      fprintf(fpout, "Place %s~\n", (*it)->place);
      fprintf(fpout, "Timeline %s~\n", (*it)->timeline);
      fprintf(fpout, "StatusTimer %d\n", (*it)->status_timer);
      fprintf(fpout, "BaseType %d\n", (*it)->base_type);
      fprintf(fpout, "Phil");
      for (i = 0; i < 20; i++)
      fprintf(fpout, " %d", (*it)->phil_amount[i]);
      fprintf(fpout, "\n");

      for (i = 0; i < 10; i++) {
        if ((*it)->other_amount[i] > 0)
        fprintf(fpout, "Other %d %s~\n", (*it)->other_amount[i], (*it)->other_name[i]);
      }
      fprintf(fpout, "BattleClimate %d\n", (*it)->battleclimate);

      if ((*it)->base_faction_core > 0 && clan_lookup((*it)->base_faction_core) != NULL)
      fprintf(fpout, "Basecore %d %s~\n", (*it)->base_faction_core, (*it)->base_desc_core);

      if ((*it)->base_faction_cult > 0 && clan_lookup((*it)->base_faction_cult) != NULL)
      fprintf(fpout, "Basecult %d %s~\n", (*it)->base_faction_cult, (*it)->base_desc_cult);

      if ((*it)->base_faction_sect > 0 && clan_lookup((*it)->base_faction_sect) != NULL)
      fprintf(fpout, "Basesect %d %s~\n", (*it)->base_faction_sect, (*it)->base_desc_sect);

      for (i = 0; i < 20; i++) {
        if (safe_strlen((*it)->plant_desc[i]) > 1)
        fprintf(fpout, "Plant %s~\n", (*it)->plant_desc[i]);
        if (safe_strlen((*it)->place_desc[i]) > 1)
        fprintf(fpout, "Place %s~\n", (*it)->place_desc[i]);
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_locations() {
    FILE *fpout;
    int i = 0;
    if ((fpout = fopen(LOCATION_FILE, "w")) == NULL) {
      bug("Cannot open locations.txt for writing", 0);
      return;
    }

    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        bug("Save_stories: Blank location in vector", i);
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      fprintf(fpout, "#LOCATION\n");
      fprintf(fpout, "Name %s~\n", (*it)->name);
      fprintf(fpout, "Continent %d\n", (*it)->continent);
      fprintf(fpout, "Lockout %d\n", (*it)->lockout);
      fprintf(fpout, "Status %d\n", (*it)->status);
      fprintf(fpout, "Notes %s~\n", (*it)->notes);

      fprintf(fpout, "Hand %d\n", (*it)->hand);
      fprintf(fpout, "Order %d\n", (*it)->order);
      fprintf(fpout, "Temple %d\n", (*it)->temple);
      fprintf(fpout, "XCoord %d\n", (*it)->x);
      fprintf(fpout, "YCoord %d\n", (*it)->y);
      fprintf(fpout, "BattleClimate %d\n", (*it)->battleclimate);
      fprintf(fpout, "Timezone %d\n", (*it)->timezone);
      fprintf(fpout, "Government %s~\n", (*it)->government);
      fprintf(fpout, "Fixtures %s~\n", (*it)->fixtures);
      fprintf(fpout, "People %s~\n", (*it)->people);
      fprintf(fpout, "Place %s~\n", (*it)->place);
      fprintf(fpout, "Timeline %s~\n", (*it)->timeline);
      fprintf(fpout, "StatusTimer %d\n", (*it)->status_timer);
      fprintf(fpout, "BaseType %d\n", (*it)->base_type);
      fprintf(fpout, "Phil");
      for (i = 0; i < 20; i++)
      fprintf(fpout, " %d", (*it)->phil_amount[i]);
      fprintf(fpout, "\n");

      for (i = 0; i < 10; i++) {
        if ((*it)->other_amount[i] > 0)
        fprintf(fpout, "Other %d %s~\n", (*it)->other_amount[i], (*it)->other_name[i]);
      }

      if ((*it)->base_faction_core > 0)
      fprintf(fpout, "Basecore %d %s~\n", (*it)->base_faction_core, (*it)->base_desc_core);

      if ((*it)->base_faction_cult > 0)
      fprintf(fpout, "Basecult %d %s~\n", (*it)->base_faction_cult, (*it)->base_desc_cult);

      if ((*it)->base_faction_sect > 0)
      fprintf(fpout, "Basesect %d %s~\n", (*it)->base_faction_sect, (*it)->base_desc_sect);

      for (i = 0; i < 20; i++) {
        if (safe_strlen((*it)->plant_desc[i]) > 1)
        fprintf(fpout, "Plant %s~\n", (*it)->plant_desc[i]);
        if (safe_strlen((*it)->place_desc[i]) > 1)
        fprintf(fpout, "Place %s~\n", (*it)->place_desc[i]);
      }

      fprintf(fpout, "End\n\n");
    }

    fprintf(fpout, "#END\n");
    fclose(fpout);

    save_locations_backup();
  }

  vector<GATEWAY_TYPE *> gatewayVect;

  GATEWAY_TYPE *nullgateway;


  void shuffle_gateway(GATEWAY_TYPE *gate)
  {
    log_string("GATESHUFFLE");

    gate->lastused = current_time;
    ROOM_INDEX_DATA *hroom = get_room_index(gate->havenroom);
    int dir = gate->direction;
    ROOM_INDEX_DATA *wroom = get_room_index(gate->worldroom);
    if(hroom->sector_type == SECT_STREET || wroom->sector_type == SECT_STREET)
    return;
    if(number_percent() % 2 == 0)
    {
      ROOM_INDEX_DATA *newdoor = get_lair_room(wroom->area->world);
      if (newdoor != NULL) {
        wroom->exit[rev_dir[dir]]->u1.to_room =
        cardinal_room(wroom, rev_dir[dir]);
        newdoor->exit[rev_dir[dir]]->u1.to_room = hroom;
        gate->worldroom = newdoor->vnum;
      }
    }
    else
    {
      ROOM_INDEX_DATA *newearth = directional_outerforest(dir);
      if (newearth != NULL) {
        hroom->exit[dir]->u1.to_room = cardinal_room(hroom, dir);
        newearth->exit[dir]->u1.to_room = wroom;
        gate->havenroom = newearth->vnum;
      }
    }
    if (!IS_SET(hroom->area->area_flags, AREA_CHANGED))
    SET_BIT(hroom->area->area_flags, AREA_CHANGED);
    if (!IS_SET(wroom->area->area_flags, AREA_CHANGED))
    SET_BIT(wroom->area->area_flags, AREA_CHANGED);

  }

  void fix_gateway(GATEWAY_TYPE *gate)
  {
    //char logs[MSL];
    //if(gate->havenroom == 196418)
    //log_string("Here");
    ROOM_INDEX_DATA *hroom = get_room_index(gate->havenroom);
    int dir = gate->direction;
    ROOM_INDEX_DATA *wroom = get_room_index(gate->worldroom);
    if(hroom == NULL)
    {
      //if(gate->havenroom == 196418)
      //log_string("NULL Hroom");
      if(wroom != NULL)
      wroom->exit[rev_dir[dir]]->u1.to_room = cardinal_room(wroom, rev_dir[dir]);
      //gate->valid = FALSE;
      return;
    }
    if(wroom == NULL)
    {
      //if(gate->havenroom == 196418)
      //log_string("NULL wroom");
      if(hroom != NULL)
      hroom->exit[dir]->u1.to_room = cardinal_room(hroom, dir);
      //gate->valid = FALSE;
      return;
    }

    hroom->exit[dir]->u1.to_room = wroom;
    wroom->exit[rev_dir[dir]]->u1.to_room = hroom;
    //if(gate->havenroom == 196418)
    //sprintf(logs, "hroom vnum: %d, wroom vnum %d, hroom to room %d, wroom to room %d", hroom->vnum, wroom->vnum, hroom->exit[dir]->u1.to_room->vnum, wroom->exit[rev_dir[dir]]->u1.to_room->vnum);

    if (!IS_SET(hroom->area->area_flags, AREA_CHANGED))
    SET_BIT(hroom->area->area_flags, AREA_CHANGED);
    if (!IS_SET(wroom->area->area_flags, AREA_CHANGED))
    SET_BIT(wroom->area->area_flags, AREA_CHANGED);

  }



  void fread_gateway(FILE *fp) {
    char buf[MSL];
    const char *word;
    bool fMatch;
    GATEWAY_TYPE *gateway;
    gateway = new_gateway();

    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'E':
        if (!str_cmp(word, "End")) {
          gatewayVect.push_back(gateway);
          return;
        }
        break;
      case 'D':
        KEY("Direction", gateway->direction, fread_number(fp));
        break;
      case 'L':
        KEY("LastUsed", gateway->lastused, fread_number(fp));
        break;
      case 'H':
        KEY("Havenroom", gateway->havenroom, fread_number(fp));
        break;
      case 'W':
        KEY("World", gateway->world, fread_number(fp));
        KEY("Worldroom", gateway->worldroom, fread_number(fp));
        break;
      }
      if (!fMatch) {
        sprintf(buf, "Fread_gateway: no match: %s", word);
        bug(buf, 0);
      }
    }
  }
  void load_gateways() {
    nullgateway = new_gateway();
    FILE *fp;

    if ((fp = fopen(GATEWAY_FILE, "r")) != NULL) {
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }
        if (letter != '#') {
          bug("Load_gateways: # not found.", 0);
          break;
        }
        word = fread_word(fp);
        if (!str_cmp(word, "GATEWAY")) {
          fread_gateway(fp);
          continue;
        }
        else if (!str_cmp(word, "END")) {
          break;
        }
        else {
          bug("Load_gateways: bad section.", 0);
          continue;
        }
      }
      fclose(fp);

      // Check to see if you need to close after a dlopen.
    }
    else {
      bug("Cannot open gateways.txt", 0);
      exit(0);
    }
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      fix_gateway((*it));
    }
  }

  _DOFUN(do_fixgatewayrun)
  {
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      fix_gateway((*it));
    }
  }

  void save_gateways_backup() {
    FILE *fpout;
    char buf[MSL];
    if (time_info.day % 7 == 0)
    sprintf(buf, "../data/back1/gateways.txt");
    else if (time_info.day % 6 == 0)
    sprintf(buf, "../data/back2/gateways.txt");
    else if (time_info.day % 5 == 0)
    sprintf(buf, "../data/back3/gateways.txt");
    else if (time_info.day % 4 == 0)
    sprintf(buf, "../data/back4/gateways.txt");
    else if (time_info.day % 3 == 0)
    sprintf(buf, "../data/back5/gateways.txt");
    else if (time_info.day % 2 == 0)
    sprintf(buf, "../data/back6/gateways.txt");
    else
    sprintf(buf, "../data/back7/gateways.txt");

    if ((fpout = fopen(buf, "w")) == NULL) {
      bug("Cannot open gateways.txt for writing", 0);
      return;
    }

    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      continue;
      fprintf(fpout, "#GATEWAY\n");
      fprintf(fpout, "World %d\n", (*it)->world);
      fprintf(fpout, "Direction %d\n", (*it)->direction);
      fprintf(fpout, "Havenroom %d\n", (*it)->havenroom);
      fprintf(fpout, "Worldroom %d\n", (*it)->worldroom);
      fprintf(fpout, "LastUsed %d\n", (*it)->lastused);
      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
  }

  void save_gateways() {
    FILE *fpout;
    if ((fpout = fopen(GATEWAY_FILE, "w")) == NULL) {
      bug("Cannot open gateways.txt for writing", 0);
      return;
    }


    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if((*it)->lastused == 0)
      (*it)->lastused = current_time;

      if((*it)->lastused > 0 && (*it)->lastused < (current_time - (3600*24*14)) && number_percent() % 11 == 0)
      shuffle_gateway((*it));

      fprintf(fpout, "#GATEWAY\n");
      fprintf(fpout, "World %d\n", (*it)->world);
      fprintf(fpout, "Direction %d\n", (*it)->direction);
      fprintf(fpout, "Havenroom %d\n", (*it)->havenroom);
      fprintf(fpout, "Worldroom %d\n", (*it)->worldroom);
      fprintf(fpout, "LastUsed %d\n", (*it)->lastused);
      fprintf(fpout, "End\n\n");
    }
    fprintf(fpout, "#END\n");
    fclose(fpout);
    save_gateways_backup();
  }

  bool valid_gatedir(ROOM_INDEX_DATA *room, int dir) {
    if (room->x > 35) {
      if (dir == DIR_WEST)
      return FALSE;
    }
    if (room->x < 35) {
      if (dir == DIR_EAST)
      return FALSE;
    }
    if (room->y < 35) {
      if (dir == DIR_NORTH)
      return FALSE;
    }
    if (room->y > 35) {
      if (dir == DIR_SOUTH)
      return FALSE;
    }
    if (room->x > 35 && room->y > 35) {
      if (dir == DIR_SOUTHWEST)
      return FALSE;
    }
    if (room->x < 35 && room->y < 35) {
      if (dir == DIR_NORTHEAST)
      return FALSE;
    }
    if (room->x > 35 && room->y < 35) {
      if (dir == DIR_NORTHWEST)
      return FALSE;
    }
    if (room->x < 35 && room->y > 35) {
      if (dir == DIR_SOUTHEAST)
      return FALSE;
    }
    return TRUE;
  }

  char *const status_phases[] = {"None",          "`cPeaceful`x", "`cPeaceful`x", "`rContested`x", "`RWar`x",      "`cPeaceful`x"};
  char *const continent_phases[] = {
    "None",  "North America", "South America", "Europe", "Asia",  "Africa",        "Australasia",   "Wilds", "Other", "Godrealm",      "Hell"};

  LOCATION_TYPE *get_loc(char *argument) {
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if (!str_prefix(argument, (*it)->name))
      return (*it);
    }
    return NULL;
  }
  int get_continent(char *arg) {
    if (!str_cmp(arg, "NorthAmerica") || !str_cmp(arg, "north"))
    return CONTINENT_NA;
    if (!str_cmp(arg, "SouthAmerica") || !str_cmp(arg, "south"))
    return CONTINENT_SA;
    if (!str_cmp(arg, "africa"))
    return CONTINENT_AFRICA;
    if (!str_cmp(arg, "europe"))
    return CONTINENT_EU;
    if (!str_cmp(arg, "asia"))
    return CONTINENT_ASIA;
    if (!str_cmp(arg, "australia") || !str_cmp(arg, "australasia"))
    return CONTINENT_AUS;
    if (!str_cmp(arg, "wilds"))
    return CONTINENT_WILDS;
    if (!str_cmp(arg, "other"))
    return CONTINENT_OTHER;
    if (!str_cmp(arg, "godrealm"))
    return CONTINENT_GODREALM;
    if (!str_cmp(arg, "hell"))
    return CONTINENT_HELL;

    return 0;
  }
  int get_locstatus(char *arg) {
    if (!str_cmp(arg, "governed"))
    return STATUS_GOVERN;
    if (!str_cmp(arg, "shared"))
    return STATUS_SHARED;
    if (!str_cmp(arg, "contested"))
    return STATUS_CONTEST;
    if (!str_cmp(arg, "war"))
    return STATUS_WAR;
    if (!str_cmp(arg, "wild"))
    return STATUS_WILD;
    return 0;
  }

  int get_locrep(LOCATION_TYPE *loc, int type) {
    int val = 0;

    if (type == 10) {
      val = loc->hand;
      if (event_dominance == 1)
      val += 15;
    }
    if (type == 11) {
      val = loc->order;
      if (event_aegis == 1)
      val += 15;
    }
    if (type == 12) {
      val = loc->temple;
      if (event_cleanse == 1)
      val += 15;
    }

    if (type < 10)
    val = loc->other_amount[type];

    if (val > 95)
    val = 95;
    if (val < 0)
    val = 0;

    return val;
  }

  void territory_plus(LOCATION_TYPE *loc, int type) {
    loc->phil_amount[type]++;
    if (loc->phil_amount[type] > 100)
    loc->phil_amount[type] = 100;
  }
  void antagonist_plus(LOCATION_TYPE *loc, FACTION_TYPE *fac) {
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(loc->other_name[i], fac->name)) {
        loc->other_amount[i]++;
        return;
      }
    }
    for (int i = 0; i < 10; i++) {
      if (loc->other_amount[i] < 1) {
        free_string(loc->other_name[i]);
        loc->other_name[i] = str_dup(fac->name);
        loc->other_amount[i] = 1;
        return;
      }
    }
  }

  void boost_territory(LOCATION_TYPE *loc, int type, int amount, bool loop) {

    for (int i = 0; i < amount; i++)
    territory_plus(loc, type);

    if (amount >= 5 && loop == TRUE) {
      int temp = amount % 5;
      int newt = amount - temp;
      newt /= 5;

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != loc->continent)
        continue;

        if ((*it) == loc)
        continue;

        if (!str_cmp((*it)->name, loc->name))
        continue;

        boost_territory((*it), type, newt, FALSE);
      }
    }
  }

  void territory_minus(LOCATION_TYPE *loc, int type) {
    if (loc->phil_amount[type] < 2)
    return;
    loc->phil_amount[type]--;
  }

  void antagonist_minus(LOCATION_TYPE *loc) {
    for (int i = 0; i < 10; i++) {
      if (loc->other_amount[i] > 0)
      loc->other_amount[i]--;
    }
  }

  void hit_territory(LOCATION_TYPE *loc, int type, int amount, bool loop) {

    for (int i = 0; i < amount; i++)
    territory_minus(loc, type);

    if (amount >= 5 && loop == TRUE) {
      int temp = amount % 5;
      int newt = amount - temp;
      newt /= 5;

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != loc->continent)
        continue;

        if ((*it) == loc)
        continue;

        if (!str_cmp((*it)->name, loc->name))
        continue;

        hit_territory((*it), type, newt, FALSE);
      }
    }
  }

  int hour_with_timezone(int hour, int timezone) {
    hour += timezone;
    if (hour >= 24)
    hour = hour % 24;
    if (hour < 0)
    hour += 24;
    return hour;
  }

  void show_territory_to_char(CHAR_DATA *ch, LOCATION_TYPE *loc) {
    char buf[MSL];
    char string[MSL];
    strcpy(string, "");
    strcpy(buf, "");

    sprintf(buf, "%s, %s.\nStatus: %s, Current Time: %d hundred hours\n", loc->name, continent_phases[loc->continent], status_phases[loc->status], hour_with_timezone(get_hour(NULL), loc->timezone));
    strcat(string, buf);
    if (border_territory(loc)) {
      sprintf(buf, "It is a `rBorder Territory`x.\n\n");
      strcat(string, buf);
    }

    sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
    strcat(string, buf);

    if (loc->lockout > current_time) {
      sprintf(buf, "Locked out for another %d days.\n\r", ((int)(loc->lockout - current_time)) / (3600 * 24));
      strcat(string, buf);
    }

    for (int i = 0; i < 10; i++) {
      if (loc->other_amount[i] > 0) {
        sprintf(buf, " %s(`R%d%%`x)", loc->other_name[i], get_locrep(loc, i));
        strcat(string, buf);
      }
    }

    strcat(string, "\n\r");
    sprintf(buf, "`WGovernment:`x\n%s\n\r", loc->government);
    strcat(string, buf);
    sprintf(buf, "`WNotable People:`x\n%s\n\r", loc->people);
    strcat(string, buf);
    for (int i = 0; i < 20; i++) {
      if (safe_strlen(loc->plant_desc[i]) > 2) {
        strcat(string, loc->plant_desc[i]);
        strcat(string, "\n");
      }
    }
    sprintf(buf, "`WPlaces of Interest:`x\n%s\n\r", loc->place);
    strcat(string, buf);
    for (int i = 0; i < 20; i++) {
      if (safe_strlen(loc->place_desc[i]) > 2) {
        strcat(string, loc->place_desc[i]);
        strcat(string, "\n");
      }
    }
    sprintf(buf, "`WFixtures:`x\n%s\n\r", loc->fixtures);
    strcat(string, buf);
    if (loc->base_faction_core != 0 && clan_lookup(loc->base_faction_core) != NULL) {
      sprintf(buf, "`WControlling Faction:`x %s\n%s\n\r", clan_lookup(loc->base_faction_core)->name, loc->base_desc_core);
      strcat(string, buf);
    }
    if (loc->base_faction_cult != 0 && clan_lookup(loc->base_faction_cult) != NULL) {
      sprintf(buf, "`WControlling Cult:`x %s\n%s\n\r", clan_lookup(loc->base_faction_cult)->name, loc->base_desc_cult);
      strcat(string, buf);
    }
    if (loc->base_faction_sect != 0 && clan_lookup(loc->base_faction_sect) != NULL) {
      sprintf(buf, "`WControlling Sect:`x %s\n%s\n\r", clan_lookup(loc->base_faction_sect)->name, loc->base_desc_sect);
      strcat(string, buf);
    }

    sprintf(buf, "`WNotes:`x\n%s\n\r", loc->notes);
    strcat(string, buf);
    sprintf(buf, "`WTimeline:`x\n%s\n\r", loc->timeline);
    strcat(string, buf);
    page_to_char(string, ch);
    //    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
  }

  bool can_edit_territory(CHAR_DATA *ch, LOCATION_TYPE *loc) {
    if (IS_IMMORTAL(ch))
    return TRUE;

    if (!str_cmp(ch->pcdata->finale_location, loc->name) && ch->pcdata->finale_timer > 0)
    return TRUE;

    if (!str_cmp(ch->name, "Pandora"))
    return TRUE;

    return FALSE;
  }

  void add_to_timeline(LOCATION_TYPE *loc, char *argument) {
    char buf[MSL];
    time_t east_time;
    east_time = current_time;
    char tmp[MSL];
    char datestr[MSL];
    sprintf(tmp, "%s", (char *)ctime(&east_time));

    sprintf(datestr, "%c%c%c %c%c%c %c%c %c%c%c%c", tmp[0], tmp[1], tmp[2], tmp[4], tmp[5], tmp[6], tmp[8], tmp[9], tmp[20], tmp[21], tmp[22], tmp[23]);

    sprintf(buf, "%s\n%s: %s", loc->timeline, datestr, argument);
    free_string(loc->timeline);
    loc->timeline = str_dup(buf);
  }

  _DOFUN(do_territory) {
    char arg[MSL];
    char buf[MSL];

    if (!str_cmp(argument, "fixlocks") && IS_IMMORTAL(ch)) {

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;
        (*it)->lockout = current_time;
      }
    }

    if (!str_cmp(argument, "update") && IS_IMMORTAL(ch)) {
      territory_update();
    }
    if (!str_cmp(argument, "initial") && IS_IMMORTAL(ch)) {
      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;
        for (int i = 0; i < 10; i++) {
          if (str_cmp((*it)->other_name[i], "The Dynasty")) {
            (*it)->other_amount[i] = 0;
            free_string((*it)->other_name[i]);
            (*it)->other_name[i] = str_dup("");
          }
        }
      }
      return;
    }

    if (ch->pcdata->ci_editing == 21) {
      LOCATION_TYPE *loc;
      ch->pcdata->ci_absorb = 1;
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      int num = number_from_territory(loc);
      if(!border_territory(loc)) {
        send_to_char("That is not a border territory.\n\r", ch);
        return;
      }
      if(!territory_leader(ch, loc))
      {
        send_to_char("You are not one of the border lords of that territory.\n\r", ch);
        return;
      }
      ch->pcdata->ci_disclevel = num;
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (ch->pcdata->ci_editing == 23) {
      LOCATION_TYPE *loc;
      ch->pcdata->ci_absorb = 1;
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      int num = number_from_territory(loc);
      free_string(ch->pcdata->ci_name);
      ch->pcdata->ci_name = str_dup(loc->name);
      send_to_char("Done.\n\r", ch);
      return;
    }

    if (ch->pcdata->ci_editing == 11) {
      LOCATION_TYPE *loc;
      ch->pcdata->ci_absorb = 1;
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      free_string(ch->pcdata->ci_name);
      ch->pcdata->ci_name = str_dup(loc->name);
      send_to_char("Done.\n\r", ch);
      show_territory_to_char(ch, loc);
      return;
    }
    if (ch->pcdata->ci_editing == 12) {
      LOCATION_TYPE *loc;
      ch->pcdata->ci_absorb = 1;
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      if (loc->lockout > current_time) {
        send_to_char("That territory is still locked out from operations.\n\r", ch);
        return;
      }
      if (loc->continent == CONTINENT_WILDS && !world_access(ch, WORLD_WILDS)) {
        send_to_char("You do not know how to get to the wilds.\n\r", ch);
        return;
      }
      if (loc->continent == CONTINENT_OTHER && !world_access(ch, WORLD_OTHER)) {
        send_to_char("You do not know how to get to the other.\n\r", ch);
        return;
      }
      if (loc->continent == CONTINENT_GODREALM && !world_access(ch, WORLD_GODREALM)) {
        send_to_char("You do not know how to get to the godrealm.\n\r", ch);
        return;
      }
      if (loc->continent == CONTINENT_HELL && !world_access(ch, WORLD_HELL)) {
        send_to_char("You do not know how to get to hell.\n\r", ch);
        return;
      }

      free_string(ch->pcdata->ci_short);
      ch->pcdata->ci_short = str_dup(loc->name);
      ch->pcdata->ci_discipline2 = 0;
      send_to_char("Done.\n\r", ch);
      show_territory_to_char(ch, loc);
      return;
    }
    argument = one_argument_nouncap(argument, arg);

    if (ch->pcdata->territory_editing[TEDIT_NUMBER] > 0 && ch->pcdata->territory_editing[TEDIT_TIMER] > 0) {
      if (!str_cmp(arg, "refer") || !str_cmp(arg, "transfer")) {
        CHAR_DATA *victim;
        if ((victim = get_char_world(ch, argument)) == NULL || IS_NPC(victim)) {
          send_to_char("They're not here.\n\r", ch);
          return;
        }
        for (int i = 0; i < 15; i++) {
          victim->pcdata->territory_editing[i] = ch->pcdata->territory_editing[i];
          ch->pcdata->territory_editing[i] = 0;
        }
        send_to_char("Done.\n\r", ch);
        printf_to_char(
        victim, "You are transfered the ability to update territory %s.\n\r", territory_by_number(victim->pcdata->territory_editing[TEDIT_NUMBER])
        ->name);
        return;
      }
      if (!str_cmp(arg, "timeline") && ch->pcdata->territory_editing[TEDIT_TIMELINE] > 0) {
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: territory timeline (new message)\n\r", ch);
          return;
        }
        if(safe_strlen(argument) > 160) {
          send_to_char("That message is too long, timeline entries should be a max of two lines.\n\r", ch);
          return;
        }
        add_to_timeline(
        territory_by_number(ch->pcdata->territory_editing[TEDIT_NUMBER]), argument);
        send_to_char("Done\n\r", ch);
        ch->pcdata->territory_editing[TEDIT_TIMELINE] = 0;
        return;
      }
      if (!str_cmp(arg, "news") && ch->pcdata->territory_editing[TEDIT_NEWS] > 0) {
        ch->pcdata->ci_editing = 7;
        ch->pcdata->ci_stats[0] = -2;
        bust_a_prompt(ch);
        ch->pcdata->territory_editing[TEDIT_NEWS] = 0;
        return;
      }
      if ((!str_cmp(arg, "place") || !str_cmp(arg, "setup")) && ch->pcdata->territory_editing[TEDIT_SETUP] > 0) {
        int point = 0;
        LOCATION_TYPE *floc =
        territory_by_number(ch->pcdata->territory_editing[TEDIT_NUMBER]);
        for (int i = 0; i < 20; i++) {
          if (safe_strlen(floc->place_desc[i]) < 2) {
            point = i;
            i = 100;
          }
        }
        string_append(ch, &floc->place_desc[point]);
        send_to_char("Enter the description of the new territory location.\n\r", ch);
        ch->pcdata->territory_editing[TEDIT_SETUP] = 0;
        return;
      }
      if ((!str_cmp(arg, "plant") || !str_cmp(arg, "npc")) && ch->pcdata->territory_editing[TEDIT_PLANT] > 0) {
        int point = 0;
        LOCATION_TYPE *floc =
        territory_by_number(ch->pcdata->territory_editing[TEDIT_NUMBER]);
        for (int i = 0; i < 20; i++) {
          if (safe_strlen(floc->plant_desc[i]) < 2) {
            point = i;
            i = 100;
          }
        }
        string_append(ch, &floc->plant_desc[point]);
        send_to_char("Enter the description of the new territory NPC.\n\r", ch);
        ch->pcdata->territory_editing[TEDIT_PLANT] = 0;
        return;
      }
      if ((!str_cmp(arg, "base") || !str_cmp(arg, "establish")) && ch->pcdata->territory_editing[TEDIT_ESTABLISH] > 0) {
        LOCATION_TYPE *floc =
        territory_by_number(ch->pcdata->territory_editing[TEDIT_NUMBER]);
        FACTION_TYPE *fac = clan_lookup(ch->pcdata->factionwin);

        if (generic_faction_vnum(fac->vnum)) {
          string_append(ch, &floc->base_desc_core);
          floc->base_faction_core = ch->pcdata->factionwin;
          floc->base_type = FACTION_CORE;
          send_to_char("Enter the description of the new foothold base.\n\r", ch);
          ch->pcdata->territory_editing[TEDIT_ESTABLISH] = 0;
          return;
        }
        else if (fac->type == FACTION_CULT) {
          string_append(ch, &floc->base_desc_cult);
          floc->base_faction_cult = ch->pcdata->factionwin;
          floc->base_type = FACTION_CULT;
          send_to_char("Enter the description of the new foothold base.\n\r", ch);
          ch->pcdata->territory_editing[TEDIT_ESTABLISH] = 0;
          return;
        }
        else if (fac->type == FACTION_SECT) {
          string_append(ch, &floc->base_desc_sect);
          floc->base_faction_sect = ch->pcdata->factionwin;
          floc->base_type = FACTION_SECT;
          send_to_char("Enter the description of the new foothold base.\n\r", ch);
          ch->pcdata->territory_editing[TEDIT_ESTABLISH] = 0;
          return;
        }
        else {
          send_to_char("You cannot establish a foothold here.\n\r", ch);
          return;
        }
      }
      send_to_char("`WSyntax:`x Territory timeline (message)/territory transfer/news/plant/setup/establish.\n\r", ch);
    }

    LOCATION_TYPE *loc;
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    char arg4[MSL];
    char arg5[MSL];
    char arg6[MSL];
    char string[MSL];

    strcpy(string, "");
    strcpy(buf, "");

    if (!str_cmp(arg, "create") && IS_IMMORTAL(ch)) {
      loc = new_location();
      free_string(loc->name);
      loc->name = str_dup(argument);
      locationVect.push_back(loc);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg, "conquer") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL) {
        send_to_char("No such territory\n\r", ch);
        return;
      }
      loc->base_faction_core = 26;
      return;
    }
    else if (!str_cmp(arg, "newbase") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL) {
        send_to_char("No such territory\n\r", ch);
        return;
      }
      free_string(loc->base_desc_core);
      loc->base_desc_core = str_dup("");

      string_append(ch, &loc->base_desc_core);
      return;
    }
    else if (!str_cmp(arg, "boost") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL) {
        send_to_char("No such territory\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      if (!IS_IMMORTAL(ch) && atoi(argument) > ch->pcdata->tboosts) {
        printf_to_char(ch, "You can only do a %d percent boost.\n\r", ch->pcdata->tboosts);
        return;
      }
      if (!str_cmp(arg2, "hand"))
      boost_territory(loc, 1, atoi(argument), TRUE);
      else if (!str_cmp(arg2, "order"))
      boost_territory(loc, 2, atoi(argument), TRUE);
      else if (!str_cmp(arg2, "temple"))
      boost_territory(loc, 3, atoi(argument), TRUE);
      else {
        send_to_char("Territory boost (territory) hand/order/temple (amount)\n\r", ch);
        return;
      }
      if (!IS_IMMORTAL(ch))
      ch->pcdata->tboosts -= atoi(argument);
    }
    else if (!str_cmp(arg, "hit") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL) {
        send_to_char("No such territory\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);

      if (!IS_IMMORTAL(ch) && atoi(argument) > ch->pcdata->tboosts) {
        printf_to_char(ch, "You can only do a %d percent hit.\n\r", ch->pcdata->tboosts);
        return;
      }

      if (!str_cmp(arg2, "hand"))
      hit_territory(loc, 1, atoi(argument), TRUE);
      else if (!str_cmp(arg2, "order"))
      hit_territory(loc, 2, atoi(argument), TRUE);
      else if (!str_cmp(arg2, "temple"))
      hit_territory(loc, 3, atoi(argument), TRUE);
      else {
        send_to_char("Territory hit (territory) hand/order/temple (amount)\n\r", ch);
        return;
      }
      if (!IS_IMMORTAL(ch))
      ch->pcdata->tboosts -= atoi(argument);

    }
    else if (!str_cmp(arg, "set") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL) {
        send_to_char("No such territory\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      if (!str_cmp(arg2, "continent")) {
        loc->continent = get_continent(argument);
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "status")) {
        loc->status = get_locstatus(argument);
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "timezone")) {
        loc->timezone = atoi(argument) + 5;
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "climate")) {
        if (!str_cmp(argument, "forest")) {
          loc->battleclimate = BATTLE_FOREST;
        }
        else if (!str_cmp(argument, "field")) {
          loc->battleclimate = BATTLE_FIELD;
        }
        else if (!str_cmp(argument, "desert"))
        loc->battleclimate = BATTLE_DESERT;
        else if (!str_cmp(argument, "town"))
        loc->battleclimate = BATTLE_TOWN;
        else if (!str_cmp(argument, "city"))
        loc->battleclimate = BATTLE_CITY;
        else if (!str_cmp(argument, "mountains"))
        loc->battleclimate = BATTLE_MOUNTAINS;
      }
      else if (!str_cmp(arg2, "name")) {
        free_string(loc->name);
        loc->name = str_dup(argument);
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "coord")) {
        argument = one_argument_nouncap(argument, arg3);
        loc->x = atoi(arg3);
        loc->y = atoi(argument);
        send_to_char("Done.", ch);
      }
      else if (!str_cmp(arg2, "hand")) {
        loc->hand = atoi(argument);
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "temple")) {
        loc->temple = atoi(argument);
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "order")) {
        loc->order = atoi(argument);
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "other")) {
        bool found = FALSE;
        argument = one_argument_nouncap(argument, arg3);
        for (int i = 0; i < 10; i++) {
          if (!str_cmp(loc->other_name[i], argument)) {
            found = TRUE;
            loc->other_amount[i] = atoi(arg3);
            send_to_char("Done.", ch);
          }
        }
        if (found == FALSE) {
          int j;
          for (j = 0; j < 10 && loc->other_amount[j] != 0; j++) {
          }
          if (j < 10) {
            free_string(loc->other_name[j]);
            loc->other_name[j] = str_dup(argument);
            loc->other_amount[j] = atoi(arg3);
            send_to_char("Done.", ch);
          }
        }
      }
    }
    else if (!str_cmp(arg, "setup") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL)
      return;
      argument = one_argument_nouncap(argument, arg2);
      argument = one_argument_nouncap(argument, arg3);
      argument = one_argument_nouncap(argument, arg4);
      argument = one_argument_nouncap(argument, arg5);
      argument = one_argument_nouncap(argument, arg6);

      loc->continent = get_continent(arg2);
      loc->status = get_locstatus(arg3);
      loc->hand = atoi(arg4);
      loc->order = atoi(arg5);
      loc->temple = atoi(arg6);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg, "swap") && IS_IMMORTAL(ch)) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL)
      return;

      argument = one_argument_nouncap(argument, arg2);

      LOCATION_TYPE *loc2;
      loc2 = get_loc(arg2);
      if (loc2 == NULL)
      return;

      int temphand = loc->hand;
      int temporder = loc->order;
      int temptemple = loc->temple;
      int temcont = loc->continent;
      int tempstatus = loc->status;
      char *tempname;
      tempname = str_dup(loc->name);

      loc->hand = loc2->hand;
      loc->temple = loc2->temple;
      loc->order = loc2->order;
      loc->continent = loc2->continent;
      loc->status = loc2->status;
      free_string(loc->name);
      loc->name = str_dup(loc2->name);

      loc2->hand = temphand;
      loc2->order = temporder;
      loc2->temple = temptemple;
      loc2->continent = temcont;
      loc2->status = tempstatus;
      free_string(loc2->name);
      loc2->name = str_dup(tempname);
      send_to_char("Done.", ch);
    }
    if (!str_cmp(arg, "set")) {
      argument = one_argument_nouncap(argument, arg1);
      loc = get_loc(arg1);
      if (loc == NULL) {
        send_to_char("No such territory\n\r", ch);
        return;
      }
      argument = one_argument_nouncap(argument, arg2);
      if (!can_edit_territory(ch, loc)) {
        send_to_char("You have to finish a finale adventure in this territory before making this sort of change.\n\r", ch);
        return;
      }
      if (!str_cmp(arg2, "status")) {
        loc->status = get_locstatus(argument);
        send_to_char("Done", ch);
      }
      else if (!str_cmp(arg2, "other")) {
        bool found = FALSE;
        argument = one_argument_nouncap(argument, arg3);
        for (int i = 0; i < 10; i++) {
          if (!str_cmp(loc->other_name[i], argument)) {
            found = TRUE;
            loc->other_amount[i] = atoi(arg3);
            send_to_char("Done.", ch);
          }
        }
        if (found == FALSE) {
          int j;
          for (j = 0; j < 10 && loc->other_amount[j] != 0; j++) {
          }
          if (j < 5) {
            free_string(loc->other_name[j]);
            loc->other_name[j] = str_dup(argument);
            loc->other_amount[j] = atoi(arg3);
            send_to_char("Done.", ch);
          }
        }
      }
    }
    else if (!str_cmp(arg, "notes") || !str_cmp(arg, "note")) {
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      if (!can_edit_territory(ch, loc)) {
        send_to_char("You have to finish a finale adventure in this territory before making this sort of change.\n\r", ch);
        return;
      }
      string_append(ch, &loc->notes);
    }
    else if (!str_cmp(arg, "government")) {
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      if (!can_edit_territory(ch, loc)) {
        send_to_char("You have to finish a finale adventure in this territory before making this sort of change.\n\r", ch);
        return;
      }

      string_append(ch, &loc->government);
    }
    else if (!str_cmp(arg, "people")) {
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      if (!can_edit_territory(ch, loc)) {
        send_to_char("You have to finish a finale adventure in this territory before making this sort of change.\n\r", ch);
        return;
      }

      string_append(ch, &loc->people);
    }
    else if (!str_cmp(arg, "places")) {
      loc = get_loc(argument);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      if (!can_edit_territory(ch, loc)) {
        send_to_char("You have to finish a finale adventure in this territory before making this sort of change.\n\r", ch);
        return;
      }

      string_append(ch, &loc->place);
    }
    else if (!str_cmp(arg, "timeline")) {
      argument = one_argument_nouncap(argument, arg2);
      loc = get_loc(arg2);
      if (loc == NULL) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      if (can_edit_territory(ch, loc)) {
        string_append(ch, &loc->timeline);
        return;
      }
      if (safe_strlen(argument) > 2 && is_gm(ch) && ch->pcdata->astatus > 0) {
        add_to_timeline(loc, argument);
        return;
      }
      send_to_char("You have to finish an adventure plot first.\n\r", ch);
    }
    else if (!str_cmp(arg, "refer")) {
      if (ch->pcdata->finale_timer <= 0) {
        send_to_char("You have no territory editing powers to transfer.\n\r", ch);
        return;
      }
      CHAR_DATA *victim;
      if ((victim = get_char_world(ch, argument)) == NULL || IS_NPC(victim)) {
        send_to_char("They're not here.\n\r", ch);
        return;
      }
      victim->pcdata->finale_timer = ch->pcdata->finale_timer;
      free_string(victim->pcdata->finale_location);
      victim->pcdata->finale_location = str_dup(ch->pcdata->finale_location);
      ch->pcdata->finale_timer = 0;
      printf_to_char(victim, "You have been given the authority to freely edit territory %s, or you can use territory refer (person) to hand this power off to someone else.\n\r", victim->pcdata->finale_location);
      send_to_char("Done.\n\r", ch);
    }
    else if (!str_cmp(arg, "info")) {
      loc = get_loc(argument);
      if (loc == NULL || loc->continent == 0) {
        send_to_char("No such territory.\n\r", ch);
        return;
      }
      show_territory_to_char(ch, loc);
      return;
    }
    else if (!str_cmp(arg, "lockout")) {
      char arg3[MSL];
      argument = one_argument_nouncap(argument, arg3);
      loc = get_loc(argument);
      if (loc == NULL || !is_number(arg3) || atoi(arg3) < 1) {
        send_to_char("Syntax: Territory lockout (number of days) (territory)\n\r", ch);
        return;
      }
      if (loc->lockout > current_time - (3600 * 24 * 3)) {
        send_to_char("That territory was locked out too recently.\n\r", ch);
        return;
      }
      int days = atoi(arg3);
      int cost = 20;
      for (int i = 1; i < days; i++)
      cost = cost * 3 / 2;
      FACTION_TYPE *fac = clan_lookup(ch->faction);
      if (fac == NULL || !has_trust(ch, TRUST_WAR, fac->vnum)) {
        send_to_char("You're not trusted to do that.\n\r", ch);
        return;
      }
      if (fac->axes[AXES_COMBAT] == AXES_FARLEFT)
      cost = cost * 150 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_MIDLEFT)
      cost = cost * 130 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_NEARLEFT)
      cost = cost * 110 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_FARRIGHT)
      cost = cost * 50 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_MIDRIGHT)
      cost = cost * 70 / 100;
      else if (fac->axes[AXES_COMBAT] == AXES_NEARRIGHT)
      cost = cost * 90 / 100;
      if (fac->resource < 10000 + cost) {
        printf_to_char(ch, "That would cost %d resources.\n\r", cost * 10);
        return;
      }
      use_resources(cost, fac->vnum, NULL, "locking down a territory.");
      loc->lockout = current_time + (3600 * 24 * days);
      printf_to_char(ch, "Territory locked down for %d resources.\n\r", cost * 10);
      return;
    }
    else if (!str_cmp(arg, "North")) {
      strcat(string, "`W\tNorth America`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_NA)
        continue;

        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);

        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "south")) {
      strcat(string, "`W\tSouth America`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_SA)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Europe")) {
      strcat(string, "`W\tEurope`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_EU)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Africa")) {
      strcat(string, "`W\tAfrica`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_AFRICA)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Asia")) {
      strcat(string, "`W\tAsia`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_ASIA)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }
        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Australia") || !str_cmp(arg, "Australasia")) {
      strcat(string, "`W\tAustralasia`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_AUS)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Wilds")) {
      strcat(string, "`W\tWilds`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_WILDS)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Other")) {
      strcat(string, "`W\tOther`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_OTHER)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Godrealm")) {
      strcat(string, "`W\tGodrealm`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_GODREALM)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "Hell")) {
      strcat(string, "`W\tHell`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_HELL)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
    else if (!str_cmp(arg, "")) {
      strcat(string, "`W\tNorth America`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_NA)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }

      strcat(string, "`W\tSouth America`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_SA)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }

      strcat(string, "`W\tEurope`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_EU)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      strcat(string, "`W\tAfrica`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_AFRICA)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      strcat(string, "`W\tAsia`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_ASIA)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      strcat(string, "`W\tAustralasia`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_AUS)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      strcat(string, "`W\tWilds`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_WILDS)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      strcat(string, "`W\tOther`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_OTHER)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      strcat(string, "`W\tGodrealm`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_GODREALM)
        continue;
        loc = *it;

        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);

        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }

        strcat(string, "\n\n\r");
      }
      strcat(string, "`W\tHell`x\n\r");

      for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
      it != locationVect.end(); ++it) {
        if (!(*it)->name || (*it)->name[0] == '\0') {
          continue;
        }
        if ((*it)->valid == FALSE)
        continue;

        if ((*it)->continent != CONTINENT_HELL)
        continue;
        loc = *it;

        sprintf(buf, "%s, Status: %s\n\r", (*it)->name, status_phases[(*it)->status]);
        strcat(string, buf);
        sprintf(buf, "Cult Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\nSect Support: `r%s`x: %d%%, `g%s`x: %d%%, `m%s`x: %d%%\n `DThe Hand`x: %d%%, `WThe Order`x: %d%%, `gThe Temple`x: %d%%\n", alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDELEFT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDERIGHT, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT], alliance_names(time_info.cult_alliance_issue, ALLIANCE_SIDEMID, time_info.cult_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDELEFT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDELEFT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDERIGHT, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDERIGHT + 10], alliance_names(time_info.sect_alliance_issue, ALLIANCE_SIDEMID, time_info.sect_alliance_type), loc->phil_amount[ALLIANCE_SIDEMID + 10], loc->phil_amount[FACTION_HAND], loc->phil_amount[FACTION_ORDER], loc->phil_amount[FACTION_TEMPLE]);
        strcat(string, buf);
        for (int i = 0; i < 5; i++) {
          if (get_locrep((*it), i) > 0) {
            sprintf(buf, " %s(`R%d%%`x)", (*it)->other_name[i], get_locrep((*it), i));
            strcat(string, buf);
          }
        }
      }
      strcat(string, "\n\n\r");

      page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
    }
  }

  /*
int find_unused_vnum_slow(int min, int max, int number)
{
bool used;
for(int i=min;i<=max;i++)
{
if(get_room_index(i) == NULL)
{
used = FALSE;
for(int x=i+1;x<i+number-1;x++)
{
if(get_room_index(x) != NULL)
{
ROOM_INDEX_DATA *temp = get_room_index(x);
if(temp->x != -999 || temp->y != -999 || temp->z != 999)
used = TRUE;
}
if(used == FALSE)
return i;
}
}
return -1;
}
*/
  int find_unused_vnum_slow(int min, int max, int number) {
    bool used;
    for (int i = min; i <= max; i++) {
      ROOM_INDEX_DATA *temp1 = get_room_index(i);
      if (temp1 == NULL || (temp1->x == -999 && temp1->y == -999 && temp1->z == -999)) {
        used = FALSE;
        if (number > 1) {
          for (int x = i + 1; x < i + number - 1; x++) {
            ROOM_INDEX_DATA *temp2 = get_room_index(x);
            if (temp2 != NULL && (temp2->x != -999 || temp2->y != -999 || temp2->z != -999))
            used = TRUE;
          }
        }
        if (used == FALSE)
        return i;
      }
    }
    return -1;
  }

  int find_unused_vnum_fast(int min, int max, int number) {
    if (max - min <= 1) {
      bool used;
      if (get_room_index(min) == NULL) {
        used = FALSE;
        for (int x = min + 1; x < min + number - 1; x++) {
          if (get_room_index(x) != NULL)
          used = TRUE;
        }
        if (used == FALSE)
        return min;
      }
      return find_unused_vnum_slow(min, max, number);
    }
    int average = (min + max) / 2;
    if (get_room_index(average) == NULL) {
      return find_unused_vnum_fast(min, average, number);
    }
    else {
      return find_unused_vnum_fast(average, max, number);
    }
  }
  int find_unused_vnum(int min, int max, int number) {
    // if(number_percent() % 5 == 0)
    return find_unused_vnum_slow(min, max, number);
    // else
    // return find_unused_vnum_fast(min, max, number);
  }

  ROOM_INDEX_DATA *room_by_coordinates(int x, int y, int z) {
    AREA_DATA *pArea;
    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;

      if (pArea->minx == -1 && pArea->miny == -1 && pArea->maxx == -1 && pArea->maxy == 1)
      continue;

      if (pArea->minx == 0 && pArea->miny == 0 && pArea->maxx == 0 && pArea->maxy == 0) {
        for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
          if (get_room_index(i) != NULL) {
            ROOM_INDEX_DATA *room = get_room_index(i);
            if (room->x == x && room->y == y && room->z == z)
            return room;
          }
        }
      }
      if (pArea->minx <= x && pArea->maxx >= x && pArea->miny <= y && pArea->maxy >= y) {
        for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
          if (get_room_index(i) != NULL) {
            ROOM_INDEX_DATA *room = get_room_index(i);
            if (room->x == x && room->y == y && room->z == z)
            return room;
          }
        }
      }
    }
    return NULL;
  }

  bool coordinates_exist(int x, int y, int z) {
    AREA_DATA *pArea;
    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;

      if (pArea->minx == -1 && pArea->miny == -1 && pArea->maxx == -1 && pArea->maxy == 1)
      continue;

      if (pArea->minx == 0 && pArea->miny == 0 && pArea->maxx == 0 && pArea->maxy == 0) {
        for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
          if (get_room_index(i) != NULL) {
            ROOM_INDEX_DATA *room = get_room_index(i);
            if (room->x == x && room->y == y && room->z == z)
            return TRUE;
          }
        }
      }
      if (pArea->minx <= x && pArea->maxx >= x && pArea->miny <= y && pArea->maxy >= y) {
        for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
          if (get_room_index(i) != NULL) {
            ROOM_INDEX_DATA *room = get_room_index(i);
            if (room->x == x && room->y == y && room->z == z)
            return TRUE;
          }
        }
      }
    }
    return FALSE;
  }

  void linkuproom(ROOM_INDEX_DATA *room, bool internal) {
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *check;
    for (AreaList::iterator it = area_list.begin(); it != area_list.end(); ++it) {
      pArea = *it;
      if (pArea->minx == -1 && pArea->miny == -1 && pArea->maxx == -1 && pArea->maxy == 1)
      continue;
      if (internal == TRUE && pArea != room->area)
      continue;
      if (pArea->vnum == 0 && pArea != room->area)
      continue;
      if (pArea->minx == 0 && pArea->miny == 0 && pArea->maxx == 0 && pArea->maxy == 0) {
        for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
          if ((check = get_room_index(i)) != NULL) {
            if (check->x == room->x && check->y == room->y + 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_NORTH, CONNECT_NODOOR);
            if (check->x == room->x && check->y == room->y - 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_SOUTH, CONNECT_NODOOR);
            if (check->x == room->x + 1 && check->y == room->y + 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_NORTHEAST, CONNECT_NODOOR);
            if (check->x == room->x + 1 && check->y == room->y && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_EAST, CONNECT_NODOOR);
            if (check->x == room->x + 1 && check->y == room->y - 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_SOUTHEAST, CONNECT_NODOOR);
            if (check->x == room->x - 1 && check->y == room->y - 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_SOUTHWEST, CONNECT_NODOOR);
            if (check->x == room->x - 1 && check->y == room->y && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_WEST, CONNECT_NODOOR);
            if (check->x == room->x - 1 && check->y == room->y + 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_NORTHWEST, CONNECT_NODOOR);
            if (check->x == room->x && check->y == room->y && check->z == room->z + 1)
            make_exit(room->vnum, check->vnum, DIR_UP, CONNECT_NODOOR);
            if (check->x == room->x && check->y == room->y && check->z == room->z - 1)
            make_exit(room->vnum, check->vnum, DIR_DOWN, CONNECT_NODOOR);
          }
        }
      }
      if (pArea->minx <= room->x + 1 && pArea->maxx >= room->x - 1 && pArea->miny <= room->y + 1 && pArea->maxy >= room->y - 1) {
        for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
          if ((check = get_room_index(i)) != NULL) {
            if (check->x == room->x && check->y == room->y + 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_NORTH, CONNECT_NODOOR);
            if (check->x == room->x && check->y == room->y - 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_SOUTH, CONNECT_NODOOR);
            if (check->x == room->x + 1 && check->y == room->y + 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_NORTHEAST, CONNECT_NODOOR);
            if (check->x == room->x + 1 && check->y == room->y && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_EAST, CONNECT_NODOOR);
            if (check->x == room->x + 1 && check->y == room->y - 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_SOUTHEAST, CONNECT_NODOOR);
            if (check->x == room->x - 1 && check->y == room->y - 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_SOUTHWEST, CONNECT_NODOOR);
            if (check->x == room->x - 1 && check->y == room->y && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_WEST, CONNECT_NODOOR);
            if (check->x == room->x - 1 && check->y == room->y + 1 && check->z == room->z)
            make_exit(room->vnum, check->vnum, DIR_NORTHWEST, CONNECT_NODOOR);
            if (check->x == room->x && check->y == room->y && check->z == room->z + 1)
            make_exit(room->vnum, check->vnum, DIR_UP, CONNECT_NODOOR);
            if (check->x == room->x && check->y == room->y && check->z == room->z - 1)
            make_exit(room->vnum, check->vnum, DIR_DOWN, CONNECT_NODOOR);
          }
        }
      }
    }
  }

  void makesky(int x, int y, int z, int area, bool overwrite) {
    if (z <= 0)
    return;
    ROOM_INDEX_DATA *toremove;
    AREA_DATA *pArea = get_area_data(area);
    int vnum = find_unused_vnum(pArea->min_vnum, pArea->max_vnum, 1);

    if (vnum == -1)
    return;
    if ((toremove = room_by_coordinates(x, y, z)) != NULL) {
      if (overwrite == TRUE)
      kill_room(toremove);
      else
      return;
    }
    create_room(vnum);

    ROOM_INDEX_DATA *newroom = get_room_index(vnum);

    if (newroom == NULL)
    return;
    newroom->x = x;
    newroom->y = y;
    newroom->z = z;
    newroom->sector_type = SECT_AIR;
    free_string(newroom->name);
    newroom->name = str_dup("`CSkies`x");
    newroom->description = str_dup("");
    newroom->shroud = str_dup("");
    SET_BIT(newroom->room_flags, ROOM_UNLIT);
    newroom->size = 50;
    linkuproom(newroom, FALSE);
  }

  void airbubble(ROOM_INDEX_DATA *room, bool overwrite) {
    if (room->exit[DIR_UP] == NULL && room->z >= 0)
    makesky(room->x, room->y, room->z + 1, room->area->vnum, overwrite);
    if (room->exit[DIR_NORTH] == NULL && room->z >= 1)
    makesky(room->x, room->y + 1, room->z, room->area->vnum, overwrite);
    if (room->exit[DIR_NORTHEAST] == NULL && room->z >= 1)
    makesky(room->x + 1, room->y + 1, room->z, room->area->vnum, overwrite);
    if (room->exit[DIR_EAST] == NULL && room->z >= 1)
    makesky(room->x + 1, room->y, room->z, room->area->vnum, overwrite);
    if (room->exit[DIR_SOUTHEAST] == NULL && room->z >= 1)
    makesky(room->x + 1, room->y - 1, room->z, room->area->vnum, overwrite);
    if (room->exit[DIR_SOUTH] == NULL && room->z >= 1)
    makesky(room->x, room->y - 1, room->z, room->area->vnum, overwrite);
    if (room->exit[DIR_SOUTHWEST] == NULL && room->z >= 1)
    makesky(room->x - 1, room->y - 1, room->z, room->area->vnum, overwrite);
    if (room->exit[DIR_WEST] == NULL && room->z >= 1)
    makesky(room->x - 1, room->y, room->z, room->area->vnum, overwrite);
    if (room->exit[DIR_NORTHWEST] == NULL && room->z >= 1)
    makesky(room->x - 1, room->y + 1, room->z, room->area->vnum, overwrite);
  }

  void build_basement(PROP_TYPE *prop) {
    for (int x = prop->minx; x <= prop->maxx; x++) {
      for (int y = prop->miny; y <= prop->maxy; y++) {
        ROOM_INDEX_DATA *oroom = room_by_coordinates(x, y, 0);
        if (oroom == NULL)
        continue;
        ROOM_INDEX_DATA *droom = room_by_coordinates(x, y, -1);
        if (droom == NULL) {
          smartdig(oroom, DIR_DOWN, oroom->area->vnum);
          oroom->exit[DIR_DOWN]->wall = WALL_BRICK;
          oroom->exit[DIR_DOWN]->u1.to_room->exit[DIR_UP]->wall = WALL_BRICK;
        }
      }
    }
    for (int x = prop->minx; x <= prop->maxx; x++) {
      for (int y = prop->miny; y <= prop->maxy; y++) {
        ROOM_INDEX_DATA *oroom = room_by_coordinates(x, y, -1);
        if (oroom == NULL)
        continue;
        ROOM_INDEX_DATA *droom = room_by_coordinates(x, y, -2);
        if (droom == NULL) {
          smartdig(oroom, DIR_DOWN, oroom->area->vnum);
          oroom->exit[DIR_DOWN]->wall = WALL_BRICK;
          oroom->exit[DIR_DOWN]->u1.to_room->exit[DIR_UP]->wall = WALL_BRICK;
        }
      }
    }
  }

  _DOFUN(addbasements) {

    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      if ((*it)->minz == -3)
      build_basement(*it);
    }
    send_to_char("Done.", ch);
  }

  void smartdig(ROOM_INDEX_DATA *orig, int direction, int area) {
    ROOM_INDEX_DATA *toremove;
    ROOM_INDEX_DATA *newroom;
    int xmod = 0;
    int ymod = 0;
    int zmod = 0;
    char buf[MSL];
    sprintf(buf, "SmartDig: Starting %d %d.", direction, area);
    //    log_string(buf);

    switch (direction) {
    case DIR_DOWN:
      zmod = -1;
      break;
    case DIR_UP:
      zmod = 1;
      break;
    case DIR_NORTH:
      ymod = 1;
      break;
    case DIR_SOUTH:
      ymod = -1;
      break;
    case DIR_EAST:
      xmod = 1;
      break;
    case DIR_WEST:
      xmod = -1;
      break;
    case DIR_NORTHEAST:
      ymod = 1;
      xmod = 1;
      break;
    case DIR_NORTHWEST:
      ymod = 1;
      xmod = -1;
      break;
    case DIR_SOUTHEAST:
      ymod = -1;
      xmod = 1;
      break;
    case DIR_SOUTHWEST:
      ymod = -1;
      xmod = -1;
      break;
    }
    AREA_DATA *pArea = get_area_data(area);
    int vnum = find_unused_vnum(pArea->min_vnum, pArea->max_vnum, 1);
    if (vnum == -1) {
      sprintf(buf, "SmartDig: No unusued vnum found.");
      log_string(buf);
      return;
    }

    if ((toremove = room_by_coordinates(orig->x + xmod, orig->y + ymod, orig->z + zmod)) != NULL) {
      if (toremove->sector_type == SECT_STREET) {
        sprintf(buf, "SmartDig: Trying to build over a street.");
        log_string(buf);
        return; // Can't build over streets.
      }
      if (toremove->sector_type == SECT_WATER) {
        sprintf(buf, "SmartDig: Trying to build over water.");
        log_string(buf);
        return; // Can't build over water.
      }
      if (toremove->sector_type == SECT_UNDERWATER) {
        sprintf(buf, "SmartDig: Trying to build over water.");
        log_string(buf);
        return; // Can't build over water.
      }

      kill_room(toremove);
    }
    create_room(vnum);

    newroom = get_room_index(vnum);

    if (newroom == NULL) {
      sprintf(buf, "SmartDig: Couldn't create room.");
      log_string(buf);
      return;
    }
    sprintf(buf, "SmartDig: Room made %d.", newroom->vnum);
    //    log_string(buf);

    newroom->x = orig->x + xmod;
    newroom->y = orig->y + ymod;
    newroom->z = orig->z + zmod;

    linkuproom(newroom, FALSE);
    if (orig->area == newroom->area) {
      newroom->size = orig->size;
      newroom->sector_type = orig->sector_type;
      for (int j = ROOM_DARK; j <= ROOM_STASH; j++) {
        if (IS_SET(orig->room_flags, j) && !IS_SET(newroom->room_flags, j))
        SET_BIT(newroom->room_flags, j);
      }
      free_string(newroom->name);
      newroom->name = str_dup(orig->name);
      free_string(newroom->description);
      newroom->description = str_dup(orig->description);
      free_string(newroom->shroud);
      newroom->shroud = str_dup(orig->shroud);
    }

    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      int rev = rev_dir[door];
      if (newroom->exit[door] == NULL)
      continue;

      pToRoom = newroom->exit[door]->u1.to_room; /* ROM OLC */

      if (pToRoom != NULL) {
        if (IS_SET(newroom->room_flags, ROOM_INDOORS) !=
            IS_SET(pToRoom->room_flags, ROOM_INDOORS)) {
          newroom->exit[door]->wall = WALL_BRICK;

          if (pToRoom->exit[rev]) {
            pToRoom->exit[rev]->wall = WALL_BRICK;
          }
        }

        if ((pToRoom->sector_type == SECT_AIR || pToRoom->sector_type == SECT_ATMOSPHERE) && IS_SET(newroom->room_flags, ROOM_INDOORS)) {
          newroom->exit[door]->wall = WALL_BRICK;

          if (pToRoom->exit[rev]) {
            pToRoom->exit[rev]->wall = WALL_BRICK;
          }
        }
      }
    }

    if (newroom->z > -1) {
      airbubble(newroom, TRUE);
    }
  }

  int appropriate_sector(ROOM_INDEX_DATA *room) {
    int x = get_roomx(room);
    int y = get_roomy(room);
    int z = get_roomz(room);

    if (z == 6)
    return SECT_AIR;
    else if (z > 6)
    return SECT_ATMOSPHERE;

    if (x <= 555 && x >= 453 && y <= 546 && y >= 453) {
      if (z <= 4) {
        if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->u1.to_room != NULL && room->exit[DIR_UP]->u1.to_room->sector_type == SECT_STREET)
        return SECT_TUNNELS;
        else if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->u1.to_room != NULL && room->exit[DIR_UP]->u1.to_room->sector_type == SECT_WATER)
        return SECT_UNDERWATER;
        else if (room->exit[DIR_UP] != NULL && room->exit[DIR_UP]->u1.to_room != NULL && room->exit[DIR_UP]->u1.to_room->sector_type == SECT_UNDERWATER)
        return SECT_UNDERWATER;
        else
        return -1;
      }

      return SECT_FOREST;
    }
    else {
      if (z == 5) {
        if (y < 452 && x >= 490 && x <= 492 && y > 446)
        return SECT_ROCKY;
        else if (y <= 446 && x >= 491 && x <= 492 && y > 442)
        return SECT_ROCKY;
        else if (y <= 442 && x >= 492 && x <= 492 && y >= 440)
        return SECT_ROCKY;

        if (y < 453 && y > 450) // Beach?
        {
          return SECT_BEACH;
        }
        else if (y <= 450) // Ocean
        return SECT_WATER;
        else
        return SECT_FOREST;
      }
      if (z < 5) {
        if (y < 452 && x >= 490 && x <= 492 && y > 446)
        return -1;
        else if (y <= 446 && x >= 491 && x <= 492 && y > 442)
        return -1;
        else if (y <= 442 && x >= 492 && x <= 492 && y >= 440)
        return -1;

        if (y < 450 && z == 4)
        return SECT_UNDERWATER;
        if (y < 449 && z < 4)
        return SECT_UNDERWATER;
        if (y < 448)
        return SECT_UNDERWATER;

        return -1;
      }
    }
    return SECT_FOREST;
  }

  void room_to_default(ROOM_INDEX_DATA *room) {
    if (room->sector_type == SECT_BEACH && get_roomx(room) <= 492 && get_roomx(room) >= 465) {
      free_string(room->name);
      room->name = str_dup("`gW`di`gl`gl`do`gw`x `cCove`x `Yb`ye`Ya`yc`Yh`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_UNLIT);
      SET_BIT(room->room_flags, ROOM_PUBLIC);
      room->size = 100;
    }
    else if (room->sector_type == SECT_BEACH) {
      free_string(room->name);
      room->name = str_dup("`gHaven `YS`yh`Yo`yr`Ye`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_UNLIT);
      room->size = 100;
    }
    else if (room->sector_type == SECT_ROCKY) {
      free_string(room->name);
      room->name = str_dup("`cHaven`x `DB`wl`Du`wf`Df`ws`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_UNLIT);
      SET_BIT(room->room_flags, ROOM_PUBLIC);
      room->size = 100;

    }
    else if (room->sector_type == SECT_WATER) {
      free_string(room->name);
      room->name = str_dup("`cHaven`x `YC`yo`Ya`ys`Yt`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_UNLIT);
      room->size = 100;

    }
    else if (room->sector_type == SECT_PARK) {
      free_string(room->name);
      room->name = str_dup("`DHaven `GF`gi`Ge`gl`Gd`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_UNLIT);
      room->size = 100;
    }
    else if (room->sector_type == SECT_FOREST) {
      free_string(room->name);
      room->name = str_dup("`GT`gh`Ge `GF`go`Dr`ge`Gs`gt`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_UNLIT);
      room->size = 100;
    }
    else if (room->sector_type == SECT_TUNNELS) {
      free_string(room->name);
      room->name = str_dup("`cStone`D Tunnel`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_DARK);
      SET_BIT(room->room_flags, ROOM_INDOORS);
      room->size = 80;
    }
    else if (room->sector_type == SECT_UNDERWATER && get_roomy(room) < 450) {
      free_string(room->name);
      room->name = str_dup("`DUnder`b The `BOcean`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      if (get_roomz(room) == -1)
      SET_BIT(room->room_flags, ROOM_UNLIT);
      else
      SET_BIT(room->room_flags, ROOM_DARK);
      room->size = 150;
    }
    else if (room->sector_type == SECT_UNDERWATER) {
      free_string(room->name);
      room->name = str_dup("`DUnder`b The `CRiver`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      if (get_roomz(room) == -1)
      SET_BIT(room->room_flags, ROOM_UNLIT);
      else
      SET_BIT(room->room_flags, ROOM_DARK);
      room->size = 150;
    }
    else if (room->sector_type == SECT_AIR || room->sector_type == SECT_ATMOSPHERE) {
      free_string(room->name);
      room->name = str_dup("`CSkies`x");
      room->description = str_dup("");
      room->shroud = str_dup("");
      SET_BIT(room->room_flags, ROOM_UNLIT);
      room->size = 100;
    }
  }

  void delair(int vnum) {
    int truevnum = vnum + 100000000;

    ROOM_INDEX_DATA *airroom = get_room_index(truevnum);

    if (airroom == NULL)
    return;

    bool found = FALSE;
    for (int i = 0; i < 10; i++) {
      if (airroom->exit[i] != NULL && airroom->exit[i]->u1.to_room != NULL && airroom->exit[i]->u1.to_room->sector_type != SECT_AIR && airroom->exit[i]->u1.to_room->sector_type != SECT_ATMOSPHERE)
      found = TRUE;
    }
    if (found == TRUE)
    return;

    kill_room(airroom);
  }

  void smartdemolish(ROOM_INDEX_DATA *orig, int direction, int area) {
    ROOM_INDEX_DATA *toremove;
    ROOM_INDEX_DATA *newroom;
    int xmod = 0;
    int ymod = 0;
    int zmod = 0;
    switch (direction) {
    case DIR_DOWN:
      zmod = -1;
      break;
    case DIR_UP:
      zmod = 1;
      break;
    case DIR_NORTH:
      ymod = 1;
      break;
    case DIR_SOUTH:
      ymod = -1;
      break;
    case DIR_EAST:
      xmod = -1;
      break;
    case DIR_WEST:
      xmod = 1;
      break;
    case DIR_NORTHEAST:
      ymod = 1;
      xmod = -1;
      break;
    case DIR_NORTHWEST:
      ymod = 1;
      xmod = 1;
      break;
    case DIR_SOUTHEAST:
      ymod = -1;
      xmod = -1;
      break;
    case DIR_SOUTHWEST:
      ymod = -1;
      xmod = 1;
      break;
    }
    if (get_roomz(orig) + zmod > 0)
    area = 1;
    /*
if(get_roomz(orig) == 6 && direction != DIR_DOWN)
area = 1;
if(get_roomz(orig) == 5 && direction == DIR_UP)
area = 1;

if(get_roomz(orig) > 6)
area = 1;
*/

    int vnum = get_coord_vnum(get_roomx(orig) + xmod, get_roomy(orig) + ymod, get_roomz(orig) + zmod);
    int truevnum = vnum + area * 100000000;

    for (int i = 1; i < 10; i++) {
      if ((toremove = get_room_index(vnum + i * 100000000)) != NULL) {
        if (toremove->sector_type == SECT_STREET)
        return; // Can't build over streets.
        kill_room(toremove);
      }
    }
    create_room(truevnum);
    //    integrate_room(truevnum);
    newroom = get_room_index(truevnum);

    if (newroom == NULL)
    return;

    newroom->sector_type = appropriate_sector(newroom);
    if (newroom->sector_type == -1) {
      newroom->sector_type = SECT_FOREST;
      kill_room(newroom);
      return;
    }

    room_to_default(newroom);

    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      int rev = rev_dir[door];
      if (newroom->exit[door] == NULL)
      continue;

      pToRoom = newroom->exit[door]->u1.to_room; /* ROM OLC */

      if (pToRoom != NULL) {
        if (IS_SET(newroom->room_flags, ROOM_INDOORS) !=
            IS_SET(pToRoom->room_flags, ROOM_INDOORS)) {
          newroom->exit[door]->wall = WALL_BRICK;

          if (pToRoom->exit[rev]) {
            pToRoom->exit[rev]->wall = WALL_BRICK;
          }
        }
        if ((pToRoom->sector_type == SECT_AIR || pToRoom->sector_type == SECT_ATMOSPHERE) && IS_SET(newroom->room_flags, ROOM_INDOORS)) {
          newroom->exit[door]->wall = WALL_BRICK;

          if (pToRoom->exit[rev]) {
            pToRoom->exit[rev]->wall = WALL_BRICK;
          }
        }

        if (newroom->sector_type == SECT_ROCKY && pToRoom->sector_type == SECT_WATER && get_roomy(newroom) < 443) {
          if (pToRoom->exit[DIR_DOWN] != NULL && pToRoom->exit[DIR_DOWN]->u1.to_room != NULL) {
            free_exit(pToRoom->exit[rev]);
            pToRoom->exit[rev] = NULL;
            free_exit(newroom->exit[door]);
            newroom->exit[door] = NULL;
            make_exit(newroom->vnum, pToRoom->exit[DIR_DOWN]->u1.to_room->vnum, door, CONNECT_ONEWAY);
            newroom->exit[door]->fall = 10;
          }
        }
      }
    }

    if (get_roomz(newroom) > 0) {
      int value = newroom->vnum - area * 100000000;
      value += 1000000;
      delair(value);
      delair(value + 1);
      delair(value - 1);
      delair(value + 1000);
      delair(value - 1000);
      delair(value + 1000 + 1);
      delair(value + 1000 - 1);
      delair(value - 1000 + 1);
      delair(value - 1000 - 1);
    }
  }

  int territory_count(void) {
    int count = 0;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->continent == 0 && str_cmp((*it)->name, "Haven"))
      continue;
      count++;
    }
    return count;
  }

  LOCATION_TYPE *territory_by_number(int number) {
    int count = 1;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->continent == 0 && str_cmp((*it)->name, "Haven"))
      continue;

      if (count == number)
      return (*it);

      count++;
    }
    return NULL;
  }

  int number_from_territory(LOCATION_TYPE *terr) {
    int count = 1;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->continent == 0 && str_cmp((*it)->name, "Haven"))
      continue;

      if (!str_cmp((*it)->name, terr->name))
      return count;

      count++;
    }
    return 0;
  }

  int first_terr_points(LOCATION_TYPE *loc) {
    int max = 0;
    for (int i = 0; i < 10; i++) {
      if (loc->other_amount[i] > max) {
        max = loc->other_amount[i];
      }
    }
    if (loc->hand >= loc->order && loc->hand >= loc->temple && loc->hand >= max)
    return loc->hand;
    if (loc->order >= loc->hand && loc->order >= loc->temple && loc->temple >= max)
    return loc->order;
    if (loc->temple >= loc->hand && loc->temple >= loc->order && loc->temple >= max)
    return loc->temple;

    return max;
  }

  int second_terr_points(LOCATION_TYPE *loc) {
    int high = first_terr_points(loc);
    int max = 0;
    for (int i = 0; i < 10; i++) {
      if (loc->other_amount[i] > max && max != high) {
        max = loc->other_amount[i];
      }
    }
    if (loc->hand >= loc->order && loc->hand >= loc->temple && loc->hand >= max && loc->hand != high)
    return loc->hand;
    if (loc->order >= loc->hand && loc->order >= loc->temple && loc->temple >= max && loc->order != high)
    return loc->order;
    if (loc->temple >= loc->hand && loc->temple >= loc->order && loc->temple >= max && loc->temple != high)
    return loc->temple;

    return max;
  }

  char *first_dom_terr(LOCATION_TYPE *loc) {
    int point = 0;
    int max = 0;
    for (int i = 0; i < 10; i++) {
      if (loc->other_amount[i] > max) {
        max = loc->other_amount[i];
        point = i;
      }
    }
    if (loc->hand >= loc->order && loc->hand >= loc->temple && loc->hand >= max)
    return "The Hand";
    if (loc->order >= loc->hand && loc->order >= loc->temple && loc->temple >= max)
    return "The Order";
    if (loc->temple >= loc->hand && loc->temple >= loc->order && loc->temple >= max)
    return "The Temple";

    return loc->other_name[point];
  }
  char *second_dom_terr(LOCATION_TYPE *loc) {
    int high = first_terr_points(loc);
    int point = 0;
    int max = 0;
    for (int i = 0; i < 10; i++) {
      if (loc->other_amount[i] > max && max != high) {
        max = loc->other_amount[i];
        point = i;
      }
    }
    if (loc->hand >= loc->order && loc->hand >= loc->temple && loc->hand >= max && loc->hand != high)
    return "The Hand";
    if (loc->order >= loc->hand && loc->order >= loc->temple && loc->temple >= max && loc->order != high)
    return "The Order";
    if (loc->temple >= loc->hand && loc->temple >= loc->order && loc->temple >= max && loc->temple != high)
    return "The Temple";

    return loc->other_name[point];
  }

  bool is_terr_war() {
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->continent == 0)
      continue;
      if (border_territory((*it)))
      continue;

      if ((*it)->status == STATUS_WAR)
      return TRUE;
    }
    return FALSE;
  }

  int number_unrest() {
    int count = 0;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->continent == 0)
      continue;

      if ((*it)->status == STATUS_CONTEST && !border_territory(*it))
      count++;
    }
    return count;
  }

  void update_territory(LOCATION_TYPE *loc) {
    char buf[MSL];

    loc->status_timer += number_range(1, 3);
    if (loc->continent == 0)
    return;

    if (loc->base_faction_core != 0 && clan_lookup(loc->base_faction_core) == NULL) {
      loc->base_faction_core = 0;
      free_string(loc->base_desc_core);
      loc->base_desc_core = str_dup("");
    }
    if (loc->base_faction_sect != 0 && clan_lookup(loc->base_faction_sect) == NULL) {
      loc->base_faction_sect = 0;
      free_string(loc->base_desc_sect);
      loc->base_desc_sect = str_dup("");
    }
    if (loc->base_faction_cult != 0 && clan_lookup(loc->base_faction_cult) == NULL) {
      loc->base_faction_cult = 0;
      free_string(loc->base_desc_cult);
      loc->base_desc_cult = str_dup("");
    }

    if (loc->status == STATUS_GOVERN || loc->status == STATUS_SHARED) {
    }
    if (loc->status == STATUS_CONTEST || loc->status == STATUS_WILD) {
    }
    if (loc->status == STATUS_WAR) {
    }

    if (loc->status == STATUS_WAR) {
      if (number_percent() % 7 == 0) {
        loc->status = STATUS_GOVERN;
        sprintf(
        buf, "The war in %s has ended, with peace being established once more.", loc->name);
        super_news(buf);
        return;
      }
    }
    else if (loc->status == STATUS_CONTEST) {
      if (number_percent() % 2 == 0 && !is_terr_war()) {
        loc->lockout = current_time;
        loc->status = STATUS_WAR;
        sprintf(buf, "The unrest in %s has escalated into full supernatural war.", loc->name);
        super_news(buf);
        return;
      }
      else if (number_percent() % 3 == 0 && !border_territory(loc)) {
        loc->status = STATUS_GOVERN;
        sprintf(buf, "The supernatural unrest in %s has ended, with peace being established once more.", loc->name);
        super_news(buf);
        return;
      }
    }
    else if (loc->status == STATUS_GOVERN || loc->status == STATUS_SHARED || loc->status == STATUS_WILD) {
      if ((number_percent() % 7 == 0 && number_unrest() < 6) || border_territory(loc)) {
        loc->lockout = current_time;
        loc->status = STATUS_CONTEST;
        sprintf(buf, "The peace in %s has broken, with growing supernatural unrest.", loc->name);
        super_news(buf);
        return;
      }
      else if (number_percent() % 29 == 0 && number_unrest() > 6) {
        loc->lockout = current_time;
        loc->status = STATUS_CONTEST;
        sprintf(buf, "The peace in %s has broken, with growing supernatural unrest.", loc->name);
        super_news(buf);
        return;
        ;
      }
    }
  }

  void territory_update() {
    int daystochange = 18;
    LOCATION_TYPE *loc;
    LOCATION_TYPE *maxgovern;
    int maxgovernamount = 0;
    LOCATION_TYPE *maxcontest;
    int maxcontestamount = 0;
    char buf[MSL];

    log_string("TERRITORY UPDATE");
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        bug("Save_stories: Blank location in vector", 1);
        continue;
      }
      if ((*it)->valid == FALSE || (*it)->continent == 0)
      continue;
      loc = (*it);
      loc->status_timer += number_range(1, 3);
      if (loc->status == 0)
      loc->status = 1;
      if (loc->status == STATUS_WAR && loc->status_timer >= daystochange * 2) {
        loc->status = STATUS_GOVERN;
        loc->status_timer = 0;
        sprintf(
        buf, "The war in %s has ended, with peace being established once more.", loc->name);
        super_news(buf);
      }
      if (loc->status == STATUS_CONTEST && loc->status_timer > daystochange * 2 && !border_territory(loc)) {
        loc->status = STATUS_GOVERN;
        loc->status_timer = 0;
        sprintf(buf, "The supernatural unrest in %s has ended, with peace being established once more.", loc->name);
        super_news(buf);
      }
      if (loc->status == STATUS_CONTEST && loc->status_timer > maxcontestamount) {
        maxcontest = loc;
        maxcontestamount = loc->status_timer;
      }
      if (loc->status == STATUS_GOVERN && loc->status_timer > maxgovernamount) {
        maxgovern = loc;
        maxgovernamount = loc->status_timer;
      }
      if (border_territory(loc)) {
        sprintf(buf, "BORDER: %s, %d, %d", loc->name, loc->status, loc->status_timer);
        log_string(buf);
      }
      if ((loc->status == STATUS_GOVERN || loc->status == STATUS_SHARED || loc->status == 0 || loc->status == STATUS_WILD) && border_territory(loc) && loc->status_timer > 20) {
        loc->status = STATUS_CONTEST;
        loc->status_timer = 0;
        sprintf(buf, "The peace in %s has broken, with growing supernatural unrest.", loc->name);
        super_news(buf);
      }
    }
    if (maxcontestamount > 0 && !is_terr_war()) {
      maxcontest->status = STATUS_WAR;
      maxcontest->status_timer = 0;
      sprintf(buf, "The unrest in %s has escalated into full supernatural war.", maxcontest->name);
      super_news(buf);
    }
    if (maxgovernamount > 0 && number_unrest() < 2) {
      maxgovern->status = STATUS_CONTEST;
      maxgovern->status_timer = 0;
      sprintf(buf, "The peace in %s has broken, with growing supernatural unrest.", maxgovern->name);
      super_news(buf);
    }
  }

  int tempmod(int hour) {
    switch (hour) {
    case 3:
      return -10;
      break;
    case 2:
    case 4:
      return -8;
      break;
    case 1:
    case 5:
      return -7;
      break;
    case 0:
    case 24:
    case 6:
      return -5;
      break;
    case 23:
    case 7:
      return -3;
      break;
    case 22:
    case 8:
      return -1;
      break;
    case 21:
    case 9:
      return 1;
      break;
    case 20:
    case 10:
      return 3;
      break;
    case 19:
    case 11:
      return 5;
      break;
    case 18:
    case 12:
      return 8;
      break;
    case 17:
    case 13:
      return 10;
      break;
    case 16:
    case 14:
      return 12;
      break;
    case 15:
      return 14;
      break;
    }
    return 0;
  }

  int const month_temperature[] = {32, 33, 41, 52, 62, 72, 77, 74, 68, 59, 47, 35};

  void assign_weather(ROOM_INDEX_DATA *room, LOCATION_TYPE *territory, bool climate) {
    if (room == NULL)
    return;
    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);

    room->timezone = territory->timezone;

    int month = ptm->tm_mon;

    if (territory->continent == CONTINENT_WILDS)
    month += 5;
    else if (territory->continent == CONTINENT_OTHER)
    month += 1;
    else if (territory->continent == CONTINENT_GODREALM)
    month -= 2;
    else if (territory->continent == CONTINENT_HELL)
    month += 9;

    if (month < 0)
    month += 12;
    if (month > 11)
    month -= 12;

    int temp = month_temperature[month];

    temp += tempmod(get_hour(room));

    int baseshift = 54 - month_temperature[month];

    int basey = 425;

    int yshift = territory->y - basey;
    // New zealand is 371

    temp += baseshift * yshift / 150;

    temp += 2;

    int equatorial = territory->y - 400;

    if (equatorial < 0)
    equatorial *= -1;

    temp -= equatorial / 10;

    if (territory->x >= 720 && territory->x <= 865 && temp < 50)
    temp = temp * 4 / 3;

    if (climate == TRUE) {
      if (territory->battleclimate == BATTLE_MOUNTAINS)
      temp -= 10;
      if (territory->battleclimate == BATTLE_DESERT)
      temp += 10;
      if (territory->battleclimate == BATTLE_TUNDRA)
      temp -= 20;
    }
    if (!str_cmp(territory->name, "Northgard, Godrealm"))
    temp -= 30;
    room->temperature = temp;

    room->cloud_cover = 100 - temp;
    room->cloud_cover += number_range(-50, 50);
    room->cloud_density = 100 - temp;
    room->cloud_density += number_range(-30, 30);

    if (climate == TRUE) {
      if (territory->battleclimate == BATTLE_DESERT) {
        room->cloud_density -= 50;
        room->cloud_cover -= 20;
      }
      if (territory->battleclimate == BATTLE_FOREST || territory->battleclimate == BATTLE_LAKE) {
        room->cloud_density += 10;
      }
      if (territory->battleclimate == BATTLE_MOUNTAINS || territory->battleclimate == BATTLE_TUNDRA) {
        room->cloud_density += 30;
        room->cloud_cover += 20;
      }
    }
    room->cloud_cover = UMAX(0, room->cloud_cover);
    room->cloud_cover = UMIN(100, room->cloud_cover);
    room->cloud_density = UMAX(0, room->cloud_density);
    room->cloud_density = UMIN(100, room->cloud_density);

    int hailchance = 0;
    int rainchance = 0;
    int snowchance = 0;

    if (room->cloud_cover >= 100 && room->cloud_density >= 90 && room->temperature < 35)
    hailchance = 5;

    if (room->cloud_cover >= 80 && room->cloud_density >= 80 && room->temperature < 40 && room->temperature > 20)
    snowchance = 30;

    if (room->cloud_cover >= 80 && room->cloud_density >= 70 && room->temperature > 45)
    rainchance = 65;

    if (climate == TRUE && territory->battleclimate == BATTLE_MOUNTAINS)
    snowchance *= 3;
    if (climate == TRUE && territory->battleclimate == BATTLE_TUNDRA)
    snowchance *= 5;

    if (climate == TRUE && territory->battleclimate == BATTLE_DESERT) {
      hailchance = 0;
      snowchance = 0;
      rainchance = UMIN(rainchance, 5);
    }
    if (!str_cmp(territory->name, "Northgard, Godrealm"))
    snowchance *= 3;

    if (hailchance > (number_range(1, 9473) % 100))
    room->hailing = 1;
    else if (snowchance > (number_range(1, 8563) % 100))
    room->snowing = 1;
    else if (rainchance > (number_range(1, 5838) % 100))
    room->raining = 1;
  }

  char *weather_forecast(LOCATION_TYPE *territory, int havenhour) {
    tm *ptm;
    time_t east_time;

    east_time = current_time;
    ptm = gmtime(&east_time);

    int hour = havenhour + territory->timezone;

    if (hour >= 24)
    hour -= 24;
    if (hour < 0)
    hour += 24;

    int month = ptm->tm_mon;

    if (territory->continent == CONTINENT_WILDS)
    month += 5;
    else if (territory->continent == CONTINENT_OTHER)
    month += 1;
    else if (territory->continent == CONTINENT_GODREALM)
    month -= 2;
    else if (territory->continent == CONTINENT_HELL)
    month += 9;

    if (month < 0)
    month += 12;
    if (month > 11)
    month -= 12;

    int temp = month_temperature[month];

    temp += tempmod(hour);

    int baseshift = 54 - month_temperature[month];

    int basey = 425;

    int yshift = territory->y - basey;
    // New zealand is 371

    temp += baseshift * yshift / 150;

    temp += 2;

    int equatorial = territory->y - 400;

    if (equatorial < 0)
    equatorial *= -1;

    temp -= equatorial / 10;

    if (territory->x >= 720 && territory->x <= 865 && temp < 50)
    temp = temp * 4 / 3;

    if (territory->battleclimate == BATTLE_MOUNTAINS)
    temp -= 10;
    if (territory->battleclimate == BATTLE_DESERT)
    temp += 10;
    if (territory->battleclimate == BATTLE_TUNDRA)
    temp -= 20;

    int cloud_cover = 0;
    int cloud_density = 0;

    cloud_cover = 100 - temp;
    cloud_density = 100 - temp;

    if (territory->battleclimate == BATTLE_DESERT) {
      cloud_density -= 50;
      cloud_cover -= 20;
    }
    if (territory->battleclimate == BATTLE_FOREST || territory->battleclimate == BATTLE_LAKE) {
      cloud_density += 10;
    }

    cloud_cover = UMAX(0, cloud_cover);
    cloud_cover = UMIN(100, cloud_cover);
    cloud_density = UMAX(0, cloud_density);
    cloud_density = UMIN(100, cloud_density);

    int hailchance = 0;
    int rainchance = 0;
    int snowchance = 0;

    if (cloud_cover >= 100 && cloud_density >= 90 && temp < 35)
    hailchance = 5;

    if (cloud_cover >= 80 && cloud_density >= 80 && temp < 40 && temp > 20)
    snowchance = 30;

    if (cloud_cover >= 80 && cloud_density >= 70 && temp > 45)
    rainchance = 65;

    if (territory->battleclimate == BATTLE_MOUNTAINS)
    snowchance *= 3;
    if (territory->battleclimate == BATTLE_TUNDRA)
    snowchance *= 5;

    if (territory->battleclimate == BATTLE_DESERT) {
      hailchance = 0;
      snowchance = 0;
      rainchance = UMIN(rainchance, 5);
    }
    char buf[MSL];
    buf[0] = '\0';
    char message[MSL];
    temp += 2;
    temp = temp - (temp % 5);
    int cel = temp - 32;
    cel *= 5;
    cel /= 9;

    sprintf(message, "It is expected to be about %dF(%dC) degrees, ", temp, cel);
    strcat(buf, message);
    if (cloud_cover < 20)
    strcat(buf, "with clear skies, and a ");
    else {
      if (cloud_cover >= 75)
      strcat(buf, "and heavily overcast, with a ");
      else
      strcat(buf, "and somewhat overcast, with a ");
    }
    int precip = UMAX(hailchance, snowchance);
    precip = UMAX(precip, rainchance);
    if (hailchance > rainchance && hailchance > snowchance)
    sprintf(message, "hail");
    else if (snowchance > rainchance && snowchance > hailchance)
    sprintf(message, "snow");
    else
    sprintf(message, "precipitation");
    if (precip >= 75)
    strcat(buf, "very high chance of ");
    else if (precip >= 30)
    strcat(buf, "chance of ");
    else
    strcat(buf, "low chance of ");

    strcat(buf, message);
    strcat(buf, ".");

    return str_dup(buf);
  }

  void skyfix(AREA_DATA *pArea, int type) {
    int countunused = 0;
    if (type == 1) {
      for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
        if (get_room_index(i) != NULL) {
          ROOM_INDEX_DATA *room = get_room_index(i);
          if (room->z == 1 && strcasestr(room->name, "skies") != NULL) {
            kill_room(room);
          }
          countunused = 0;
        }
        else
        countunused++;
      }
    }
    if (type == 2) {
      countunused = 0;
      for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
        if (get_room_index(i) != NULL) {
          ROOM_INDEX_DATA *room = get_room_index(i);
          if (room->z == 0) {
            airbubble(room, TRUE);
          }
          countunused = 0;
          /*
for(int door = 0;door<=9;door++) {
ROOM_INDEX_DATA *pToRoom;
int rev = rev_dir[door];
if(room->exit[door] == NULL)
continue;
pToRoom = room->exit[door]->u1.to_room;

if(pToRoom != NULL) {
if ( pToRoom->exit[rev] ) {
free_exit( pToRoom->exit[rev] );
pToRoom->exit[rev] = NULL;
}
}
}
linkuproom(room);
if(room->exit[DIR_DOWN] != NULL)
{
room->x = room->exit[DIR_DOWN]->u1.to_room->x;
room->y = room->exit[DIR_DOWN]->u1.to_room->y;
}
}
*/
        }
        else
        countunused++;
      }
    }
  }

  _DOFUN(do_skyfix) {
    if (!str_cmp(argument, "delete"))
    skyfix(ch->in_room->area, 1);
    if (!str_cmp(argument, "remake"))
    skyfix(ch->in_room->area, 2);
  }

  void nukez(AREA_DATA *pArea, int layer) {
    int countunused = 0;
    for (int i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
      if (get_room_index(i) != NULL) {
        ROOM_INDEX_DATA *room = get_room_index(i);
        if (room->z == layer) {
          kill_room(room);
        }
        countunused = 0;
      }
      else {
        countunused++;
      }
    }
  }

  _DOFUN(do_nukez) {
    int layer = atoi(argument);

    if (layer <= 100 && layer >= -100) {
      nukez(ch->in_room->area, layer);
    }
    else {
      send_to_char("Z must be between -100 and 100", ch);
    }
  }

  _DOFUN(do_linkuproom) { linkuproom(ch->in_room, FALSE); }
  _DOFUN(do_airbubble) { airbubble(ch->in_room, TRUE); }

  int smartcreate(int x, int y, int z, int area) {
    char buf[MSL];
    ROOM_INDEX_DATA *toremove;
    ROOM_INDEX_DATA *newroom;
    AREA_DATA *pArea = get_area_data(area);
    int vnum = find_unused_vnum(pArea->min_vnum, pArea->max_vnum, 1);
    if (vnum == -1) {
      sprintf(buf, "SmartCreate: No unusued vnum found.");
      log_string(buf);
      return -1;
    }
    if ((toremove = room_by_coordinates(x, y, z)) != NULL) {
      if (toremove->sector_type == SECT_STREET) {
        sprintf(buf, "SmartCreate: Trying to build over a street.");
        log_string(buf);
        return -1; // Can't build over streets.
      }
      if (toremove->sector_type == SECT_WATER) {
        sprintf(buf, "SmartCreate: Trying to build over water.");
        log_string(buf);
        return -1; // Can't build over water.
      }
      if (toremove->sector_type == SECT_UNDERWATER) {
        sprintf(buf, "SmartCreate: Trying to build over water.");
        log_string(buf);
        return -1; // Can't build over water.
      }
      kill_room(toremove);
    }
    create_room(vnum);

    newroom = get_room_index(vnum);

    if (newroom == NULL) {
      sprintf(buf, "SmartCreate: Couldn't create room.");
      log_string(buf);
      return -1;
    }

    newroom->x = x;
    newroom->y = y;
    newroom->z = z;

    linkuproom(newroom, TRUE);
    if (newroom->z > -1) {
      airbubble(newroom, TRUE);
    }
    return newroom->vnum;
  }

  void turn_into_forest(int vnum) {
    ROOM_INDEX_DATA *room = get_room_index(vnum);
    room->size = 50;
    room->sector_type = SECT_FOREST;
    SET_BIT(room->room_flags, ROOM_UNLIT);
    free_string(room->name);
    room->name = str_dup("`GT`gh`Ge `GF`go`Dr`ge`Gs`gt`x");
  }

  void turn_into_water(int vnum) {
    ROOM_INDEX_DATA *room = get_room_index(vnum);
    room->size = 50;
    room->sector_type = SECT_UNDERWATER;
    SET_BIT(room->room_flags, ROOM_DARK);
    free_string(room->name);
    room->name = str_dup("`DUnder the `BOcean`x");
  }

  void purge_offworld_exits(ROOM_INDEX_DATA *first, bool both) {
    for (int i = 0; i < 10; i++) {
      if (first->exit[i] == NULL)
      continue;
      if (first->exit[i]->u1.to_room == NULL)
      continue;
      ROOM_INDEX_DATA *ToRoom = first->exit[i]->u1.to_room;
      ROOM_INDEX_DATA *roomone = NULL;
      ROOM_INDEX_DATA *roomtwo = NULL;
      if (first->area->world != ToRoom->area->world) {
        if (i == DIR_NORTH) {
          roomone = room_by_coordinates(first->x, first->y + 1, first->z);
          roomtwo = room_by_coordinates(ToRoom->x, ToRoom->y - 1, ToRoom->z);
        }
        if (i == DIR_NORTHEAST) {
          roomone = room_by_coordinates(first->x + 1, first->y + 1, first->z);
          roomtwo = room_by_coordinates(ToRoom->x - 1, ToRoom->y - 1, ToRoom->z);
        }
        if (i == DIR_EAST) {
          roomone = room_by_coordinates(first->x + 1, first->y, first->z);
          roomtwo = room_by_coordinates(ToRoom->x - 1, ToRoom->y, ToRoom->z);
        }
        if (i == DIR_SOUTHEAST) {
          roomone = room_by_coordinates(first->x + 1, first->y - 1, first->z);
          roomtwo = room_by_coordinates(ToRoom->x - 1, ToRoom->y + 1, ToRoom->z);
        }
        if (i == DIR_SOUTH) {
          roomone = room_by_coordinates(first->x, first->y - 1, first->z);
          roomtwo = room_by_coordinates(ToRoom->x, ToRoom->y + 1, ToRoom->z);
        }
        if (i == DIR_SOUTHWEST) {
          roomone = room_by_coordinates(first->x - 1, first->y - 1, first->z);
          roomtwo = room_by_coordinates(ToRoom->x + 1, ToRoom->y + 1, ToRoom->z);
        }
        if (i == DIR_WEST) {
          roomone = room_by_coordinates(first->x - 1, first->y, first->z);
          roomtwo = room_by_coordinates(ToRoom->x + 1, ToRoom->y, ToRoom->z);
        }
        if (i == DIR_NORTHWEST) {
          roomone = room_by_coordinates(first->x - 1, first->y + 1, first->z);
          roomtwo = room_by_coordinates(ToRoom->x + 1, ToRoom->y - 1, ToRoom->z);
        }
        if (i == DIR_UP) {
          roomone = room_by_coordinates(first->x, first->y, first->z + 1);
          roomtwo = room_by_coordinates(ToRoom->x, ToRoom->y, ToRoom->z - 1);
        }
        if (i == DIR_DOWN) {
          roomone = room_by_coordinates(first->x, first->y, first->z - 1);
          roomtwo = room_by_coordinates(ToRoom->x, ToRoom->y, ToRoom->z + 1);
        }

        if (roomone != NULL) {
          first->exit[i]->u1.to_room = roomone;
          first->exit[i]->orig_door = i;
          if (roomone->exit[rev_dir[i]] == NULL)
          roomone->exit[rev_dir[i]] = new_exit();
          roomone->exit[rev_dir[i]]->u1.to_room = first;
        }
        if (roomtwo != NULL && both == TRUE) {
          ToRoom->exit[rev_dir[i]]->u1.to_room = roomtwo;
          ToRoom->exit[rev_dir[i]]->orig_door = rev_dir[i];
          if (roomtwo->exit[i] == NULL)
          roomtwo->exit[i] = new_exit();
          roomtwo->exit[i]->u1.to_room = ToRoom;
        }
      }
    }
  }

  ROOM_INDEX_DATA *cardinal_room(ROOM_INDEX_DATA *first, int dir) {
    ROOM_INDEX_DATA *roomone = NULL;

    for (int i = 0; i < 10; i++) {
      if (i != dir)
      continue;
      if (first->exit[i] == NULL)
      continue;
      if (first->exit[i]->u1.to_room == NULL)
      continue;
      if (i == DIR_NORTH) {
        roomone = room_by_coordinates(first->x, first->y + 1, first->z);
      }
      if (i == DIR_NORTHEAST) {
        roomone = room_by_coordinates(first->x + 1, first->y + 1, first->z);
      }
      if (i == DIR_EAST) {
        roomone = room_by_coordinates(first->x + 1, first->y, first->z);
      }
      if (i == DIR_SOUTHEAST) {
        roomone = room_by_coordinates(first->x + 1, first->y - 1, first->z);
      }
      if (i == DIR_SOUTH) {
        roomone = room_by_coordinates(first->x, first->y - 1, first->z);
      }
      if (i == DIR_SOUTHWEST) {
        roomone = room_by_coordinates(first->x - 1, first->y - 1, first->z);
      }
      if (i == DIR_WEST) {
        roomone = room_by_coordinates(first->x - 1, first->y, first->z);
      }
      if (i == DIR_NORTHWEST) {
        roomone = room_by_coordinates(first->x - 1, first->y + 1, first->z);
      }
      if (i == DIR_UP) {
        roomone = room_by_coordinates(first->x, first->y, first->z + 1);
      }
      if (i == DIR_DOWN) {
        roomone = room_by_coordinates(first->x, first->y, first->z - 1);
      }
    }
    return roomone;
  }

  bool is_cardinal_room(ROOM_INDEX_DATA *first, ROOM_INDEX_DATA *second, int dir) {

    for (int i = 0; i < 10; i++) {
      if (i != dir)
      continue;
      if (first->exit[i] == NULL)
      continue;
      if (first->exit[i]->u1.to_room == NULL)
      continue;
      if (i == DIR_NORTH) {
        if (second->x == first->x && second->y == first->y + 1 && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_NORTHEAST) {
        if (second->x == first->x + 1 && second->y == first->y + 1 && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_EAST) {
        if (second->x == first->x + 1 && second->y == first->y && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_SOUTHEAST) {
        if (second->x == first->x + 1 && second->y == first->y - 1 && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_SOUTH) {
        if (second->x == first->x && second->y == first->y - 1 && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_SOUTHWEST) {
        if (second->x == first->x - 1 && second->y == first->y - 1 && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_WEST) {
        if (second->x == first->x - 1 && second->y == first->y && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_NORTHWEST) {
        if (second->x == first->x - 1 && second->y == first->y + 1 && second->z == first->z)
        return TRUE;
      }
      if (i == DIR_UP) {
        if (second->x == first->x && second->y == first->y && second->z == first->z + 1)
        return TRUE;
      }
      if (i == DIR_DOWN) {
        if (second->x == first->x && second->y == first->y && second->z == first->z - 1)
        return TRUE;
      }
    }
    return FALSE;
  }

  _DOFUN(do_forestclear) {
    for (vector<PROP_TYPE *>::iterator it = PropVect.begin();
    it != PropVect.end(); ++it) {
      PROP_TYPE *prop = (*it);

      if (prop->valid == FALSE || (prop->type_special != PROPERTY_INNERFOREST && prop->type_special != PROPERTY_OUTERFOREST))
      continue;

      reclaim_property(prop);
    }
    for (int i = 16; i < 29; i++) {
      AREA_DATA *pArea = get_area_data(i);
      for (int vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++) {
        ROOM_INDEX_DATA *room;
        if ((room = get_room_index(vnum)) != NULL) {
          reclaim_room(room);
          purge_offworld_exits(room, TRUE);
        }
      }
    }
  }
  _DOFUN(do_forestbuild) {
    int built = 0;
    AREA_DATA *pArea = ch->in_room->area;
    turn_into_forest(ch->in_room->vnum);
    airbubble(ch->in_room, TRUE);
    if (pArea->building == 0) {
      pArea->building = ch->in_room->vnum;
      //	pArea->building = pArea->min_vnum;
      //	for(;get_room_index(pArea->building+1) != NULL && //get_room_index(pArea->building+1)->z == 0;) 	    pArea->building++;
    }

    printf_to_char(ch, "Starting building at room %d(%d, %d, %d)\n\r", pArea->building, get_room_index(pArea->building)->x, get_room_index(pArea->building)->y, get_room_index(pArea->building)->z);
    for (int i = 0; i < 100; i++) {
      ROOM_INDEX_DATA *previous = get_room_index(pArea->building);
      int x = previous->x;
      int y = previous->y;
      int z = previous->z;

      if (x >= pArea->maxx && y >= pArea->maxy) {
        printf_to_char(ch, "Area finished at room %d(%d, %d, %d)\n\r", previous->vnum, previous->x, previous->y, previous->z);
        return;
      }
      else if (x >= pArea->maxx) {
        built = smartcreate(pArea->minx, y + 1, z, pArea->vnum);
      }
      else {
        built = smartcreate(x + 1, y, z, pArea->vnum);
      }
      if (built == -1) {
        printf_to_char(ch, "Something went wrong at %d %d %d\n\r", x, y, z);
        return;
      }
      turn_into_forest(built);
      pArea->building = built;
    }
    printf_to_char(ch, "Finishing building at room %d(%d, %d, %d)\n\r", pArea->building, get_room_index(pArea->building)->x, get_room_index(pArea->building)->y, get_room_index(pArea->building)->z);
  }

  _DOFUN(do_trasharea) {
    for (int i = ch->in_room->area->min_vnum; i < ch->in_room->area->max_vnum;
    i++) {
      if (get_room_index(i) != NULL && ch->in_room->vnum != i)
      kill_room(get_room_index(i));
    }
  }

  _DOFUN(do_fetchroom) {
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    argument = one_argument_nouncap(argument, arg1);
    argument = one_argument_nouncap(argument, arg2);
    argument = one_argument_nouncap(argument, arg3);

    ROOM_INDEX_DATA *room =
    room_by_coordinates(atoi(arg1), atoi(arg2), atoi(arg3));
    if (room == NULL) {
      send_to_char("Unfound", ch);
      return;
    }
    printf_to_char(ch, "Found: %d\n\r", room->vnum);
  }

  _DOFUN(do_makeedges) {
    /*
int maxy = 130;
int miny = -59;
int minx = -58;
int maxx = 92;
*/
    int maxy = 4000050;
    int miny = 4000000;
    int maxx = 4000050;
    int minx = 4000000;

    int coast = 72;
    ROOM_INDEX_DATA *room;
    ROOM_INDEX_DATA *to;

    int z = atoi(argument);
    for (int x = minx; x <= maxx; x++) {
      if (!coordinates_exist(x, maxy, z))
      continue;
      room = room_by_coordinates(x, maxy, z);
      if (room == NULL)
      continue;
      int y = room->y;

      if (room->exit[DIR_NORTH] == NULL) {
        room->exit[DIR_NORTH] = new_exit();
        if (x > coast - 2 && x < coast + 2)
        to = room_by_coordinates(x, y - 5, z);
        else if (x < minx + 5)
        to = room_by_coordinates(x + 1, y - 5, z);
        else if (x > maxx - 5)
        to = room_by_coordinates(x - 1, y - 5, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x + 1, y - 5, z);
        else
        to = room_by_coordinates(x - 1, y - 5, z);
        room->exit[DIR_NORTH]->u1.to_room = to;
        room->exit[DIR_NORTH]->orig_door = DIR_NORTH;
      }
      if (room->exit[DIR_NORTHWEST] == NULL) {
        room->exit[DIR_NORTHWEST] = new_exit();
        if (x > coast - 2 && x < coast + 2)
        to = room_by_coordinates(x - 1, y - 5, z);
        else if (x < minx + 5)
        to = room_by_coordinates(x, y - 5, z);
        else if (x > maxx - 5)
        to = room_by_coordinates(x - 2, y - 5, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x, y - 5, z);
        else
        to = room_by_coordinates(x - 2, y - 5, z);
        room->exit[DIR_NORTHWEST]->u1.to_room = to;
        room->exit[DIR_NORTHWEST]->orig_door = DIR_NORTHWEST;
      }
      if (room->exit[DIR_NORTHEAST] == NULL) {
        room->exit[DIR_NORTHEAST] = new_exit();
        if (x > coast - 2 && x < coast + 2)
        to = room_by_coordinates(x + 1, y - 5, z);
        else if (x < minx + 5)
        to = room_by_coordinates(x + 2, y - 5, z);
        else if (x > maxx - 5)
        to = room_by_coordinates(x, y - 5, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x + 2, y - 5, z);
        else
        to = room_by_coordinates(x, y - 5, z);
        room->exit[DIR_NORTHEAST]->u1.to_room = to;
        room->exit[DIR_NORTHEAST]->orig_door = DIR_NORTHEAST;
      }
    }
    for (int x = minx; x <= maxx; x++) {
      if (!coordinates_exist(x, miny, z))
      continue;
      room = room_by_coordinates(x, miny, z);
      if (room == NULL)
      continue;
      int y = room->y;

      if (room->exit[DIR_SOUTH] == NULL) {
        room->exit[DIR_SOUTH] = new_exit();
        if (x > coast - 2 && x < coast + 2)
        to = room_by_coordinates(x, y + 5, z);
        else if (x < minx + 5)
        to = room_by_coordinates(x + 1, y + 5, z);
        else if (x > maxx - 5)
        to = room_by_coordinates(x - 1, y + 5, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x + 1, y + 5, z);
        else
        to = room_by_coordinates(x - 1, y + 5, z);
        room->exit[DIR_SOUTH]->u1.to_room = to;
        room->exit[DIR_SOUTH]->orig_door = DIR_SOUTH;
      }
      if (room->exit[DIR_SOUTHWEST] == NULL) {
        room->exit[DIR_SOUTHWEST] = new_exit();
        if (x > coast - 2 && x < coast + 2)
        to = room_by_coordinates(x - 1, y + 5, z);
        else if (x < minx + 5)
        to = room_by_coordinates(x, y + 5, z);
        else if (x > maxx - 5)
        to = room_by_coordinates(x - 2, y + 5, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x, y + 5, z);
        else
        to = room_by_coordinates(x - 2, y + 5, z);
        room->exit[DIR_SOUTHWEST]->u1.to_room = to;
        room->exit[DIR_SOUTHWEST]->orig_door = DIR_SOUTHWEST;
      }
      if (room->exit[DIR_SOUTHEAST] == NULL) {
        room->exit[DIR_SOUTHEAST] = new_exit();
        if (x > coast - 2 && x < coast + 2)
        to = room_by_coordinates(x + 1, y + 5, z);
        else if (x < minx + 5)
        to = room_by_coordinates(x + 2, y + 5, z);
        else if (x > maxx - 5)
        to = room_by_coordinates(x, y + 5, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x + 2, y + 5, z);
        else
        to = room_by_coordinates(x, y + 5, z);
        room->exit[DIR_SOUTHEAST]->u1.to_room = to;
        room->exit[DIR_SOUTHEAST]->orig_door = DIR_SOUTHEAST;
      }
    }
    for (int y = miny; y <= maxy; y++) {
      if (!coordinates_exist(maxx, y, z))
      continue;

      room = room_by_coordinates(maxx, y, z);
      if (room == NULL)
      continue;
      int x = room->x;

      if (room->exit[DIR_EAST] == NULL) {
        room->exit[DIR_EAST] = new_exit();
        if (y < miny + 5)
        to = room_by_coordinates(x - 5, y + 1, z);
        else if (y > maxy - 5)
        to = room_by_coordinates(x - 5, y - 1, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x - 5, y + 1, z);
        else
        to = room_by_coordinates(x - 5, y - 1, z);
        room->exit[DIR_EAST]->u1.to_room = to;
        room->exit[DIR_EAST]->orig_door = DIR_EAST;
      }
      if (room->exit[DIR_NORTHEAST] == NULL) {
        room->exit[DIR_NORTHEAST] = new_exit();
        if (y < miny + 5)
        to = room_by_coordinates(x - 5, y + 2, z);
        else if (y > maxy - 5)
        to = room_by_coordinates(x - 5, y, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x - 5, y + 2, z);
        else
        to = room_by_coordinates(x - 5, y, z);
        room->exit[DIR_NORTHEAST]->u1.to_room = to;
        room->exit[DIR_NORTHEAST]->orig_door = DIR_NORTHEAST;
      }
      if (room->exit[DIR_SOUTHEAST] == NULL) {
        room->exit[DIR_SOUTHEAST] = new_exit();
        if (y < miny + 5)
        to = room_by_coordinates(x - 5, y, z);
        else if (y > maxy - 5)
        to = room_by_coordinates(x - 5, y - 2, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x - 5, y, z);
        else
        to = room_by_coordinates(x - 5, y - 2, z);
        room->exit[DIR_SOUTHEAST]->u1.to_room = to;
        room->exit[DIR_SOUTHEAST]->orig_door = DIR_SOUTHEAST;
      }
    }
    for (int y = miny; y <= maxy; y++) {
      if (!coordinates_exist(minx, y, z))
      continue;

      room = room_by_coordinates(minx, y, z);
      if (room == NULL)
      continue;
      int x = room->x;

      if (room->exit[DIR_WEST] == NULL) {
        room->exit[DIR_WEST] = new_exit();
        if (y < miny + 5)
        to = room_by_coordinates(x + 5, y + 1, z);
        else if (y > maxy - 5)
        to = room_by_coordinates(x + 5, y - 1, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x + 5, y + 1, z);
        else
        to = room_by_coordinates(x + 5, y - 1, z);
        room->exit[DIR_WEST]->u1.to_room = to;
        room->exit[DIR_WEST]->orig_door = DIR_WEST;
      }
      if (room->exit[DIR_NORTHWEST] == NULL) {
        room->exit[DIR_NORTHWEST] = new_exit();
        if (y < miny + 5)
        to = room_by_coordinates(x + 5, y + 2, z);
        else if (y > maxy - 5)
        to = room_by_coordinates(x + 5, y, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x + 5, y + 2, z);
        else
        to = room_by_coordinates(x + 5, y, z);
        room->exit[DIR_NORTHWEST]->u1.to_room = to;
        room->exit[DIR_NORTHWEST]->orig_door = DIR_NORTHWEST;
      }
      if (room->exit[DIR_SOUTHWEST] == NULL) {
        room->exit[DIR_SOUTHWEST] = new_exit();
        if (y < miny + 5)
        to = room_by_coordinates(x + 5, y, z);
        else if (y > maxy - 5)
        to = room_by_coordinates(x + 5, y - 2, z);
        else if (number_percent() % 2 == 0)
        to = room_by_coordinates(x + 5, y, z);
        else
        to = room_by_coordinates(x + 5, y - 2, z);
        room->exit[DIR_SOUTHWEST]->u1.to_room = to;
        room->exit[DIR_SOUTHWEST]->orig_door = DIR_SOUTHWEST;
      }
    }
    send_to_char("Complete.\n\r", ch);
  }

  bool outofarealink(ROOM_INDEX_DATA *room) {
    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      if (room->exit[door] == NULL)
      continue;
      pToRoom = room->exit[door]->u1.to_room;
      if (pToRoom == NULL)
      continue;

      if (pToRoom->area->vnum != room->area->vnum)
      return TRUE;
    }
    return FALSE;
  }
  bool validlinkroom(ROOM_INDEX_DATA *room) {
    if (outofarealink(room))
    return FALSE;

    for (int door = 0; door <= 9; door++) {
      ROOM_INDEX_DATA *pToRoom;
      if (room->exit[door] == NULL)
      continue;
      pToRoom = room->exit[door]->u1.to_room;
      if (pToRoom == NULL)
      continue;

      if (outofarealink(pToRoom))
      return FALSE;
    }
    return TRUE;
  }

  bool linkupzone(ROOM_INDEX_DATA *room, int world) {
    if (world == WORLD_HELL) {
      if (room->y > -59 && room->y <= -44)
      return TRUE;
    }
    if (world == WORLD_GODREALM) {
      if (room->y < 130 && room->y >= 115)
      return TRUE;
    }
    if (world == WORLD_WILDS) {
      if (room->x > -58 && room->x <= -43 && room->y > 35)
      return TRUE;
      if (room->y < 130 && room->y >= 115 && room->x < -10)
      return TRUE;
    }
    if (world == WORLD_OTHER) {
      if (room->x > -58 && room->x <= -43 && room->y < 35)
      return TRUE;
      if (room->y > -59 && room->y <= -44 && room->x < -10)
      return TRUE;
    }
    return FALSE;
  }

  ROOM_INDEX_DATA *fetchlinkroom(int area) {
    AREA_DATA *pArea = get_area_data(area);
    for (int i = pArea->min_vnum; i < pArea->max_vnum; i++) {
      if (get_room_index(i) != NULL) {
        if (get_room_index(i)->z != 0)
        continue;

        if (pArea->vnum == HELL_FOREST_VNUM) {
          if (get_room_index(i)->y < 4000025)
          continue;
        }
        if (pArea->vnum == WILDS_FOREST_VNUM) {
          if (get_room_index(i)->y > 2000007 || get_room_index(i)->x < 2000023)
          continue;
        }
        if (pArea->vnum == GODREALM_FOREST_VNUM) {
          if (get_room_index(i)->y > 3000005)
          continue;
        }
        if (pArea->vnum == OTHER_FOREST_VNUM) {
          if (get_room_index(i)->y < 1000023 || get_room_index(i)->x < 1000023)
          continue;
        }
        if (validlinkroom(get_room_index(i)))
        return get_room_index(i);
      }
    }
    return NULL;
  }

  _DOFUN(do_offworldlinks) {
    AREA_DATA *pArea = ch->in_room->area;
    ROOM_INDEX_DATA *desti;
    int dir, rev;
    for (int i = pArea->min_vnum; i < pArea->max_vnum; i++) {
      ROOM_INDEX_DATA *room = get_room_index(i);
      if (room == NULL)
      continue;

      if ((room->x + room->y) % 49 != 0)
      continue;

      if (room->z != 0)
      continue;

      if (linkupzone(room, WORLD_HELL)) {
        if (validlinkroom(room)) {
          desti = fetchlinkroom(HELL_FOREST_VNUM);
          if (desti == NULL)
          continue;
          if (number_percent() % 2 == 0)
          dir = DIR_SOUTH;
          else
          dir = DIR_SOUTHWEST;
          rev = rev_dir[dir];
          if (room->exit[dir] != NULL) {
            free_exit(room->exit[dir]);
            room->exit[dir] = NULL;
          }
          if (desti->exit[rev] != NULL) {
            free_exit(desti->exit[rev]);
            desti->exit[rev] = NULL;
          }
          room->exit[dir] = new_exit();
          room->exit[dir]->u1.to_room = desti;
          room->exit[dir]->orig_door = dir;
          desti->exit[rev] = new_exit();
          desti->exit[rev]->u1.to_room = room;
          desti->exit[rev]->orig_door = rev;
          room->exit[dir]->wall = WALL_BRICK;
          desti->exit[rev]->wall = WALL_BRICK;
          printf_to_char(ch, "Hell Link: %d - %d\n\r", room->vnum, desti->vnum);
        }
      }
      if (linkupzone(room, WORLD_GODREALM)) {
        if (validlinkroom(room)) {
          desti = fetchlinkroom(GODREALM_FOREST_VNUM);
          if (desti == NULL)
          continue;
          if (number_percent() % 2 == 0)
          dir = DIR_NORTH;
          else
          dir = DIR_NORTHWEST;
          rev = rev_dir[dir];
          if (room->exit[dir] != NULL) {
            free_exit(room->exit[dir]);
            room->exit[dir] = NULL;
          }
          if (desti->exit[rev] != NULL) {
            free_exit(desti->exit[rev]);
            desti->exit[rev] = NULL;
          }
          room->exit[dir] = new_exit();
          room->exit[dir]->u1.to_room = desti;
          room->exit[dir]->orig_door = dir;
          desti->exit[rev] = new_exit();
          desti->exit[rev]->u1.to_room = room;
          desti->exit[rev]->orig_door = rev;
          printf_to_char(ch, "Godrealm Link: %d - %d\n\r", room->vnum, desti->vnum);
        }
      }
      if (linkupzone(room, WORLD_WILDS)) {
        if (validlinkroom(room)) {
          desti = fetchlinkroom(WILDS_FOREST_VNUM);
          if (desti == NULL)
          continue;
          if (number_percent() % 2 == 0)
          dir = DIR_WEST;
          else
          dir = DIR_NORTHWEST;
          rev = rev_dir[dir];
          if (room->exit[dir] != NULL) {
            free_exit(room->exit[dir]);
            room->exit[dir] = NULL;
          }
          if (desti->exit[rev] != NULL) {
            free_exit(desti->exit[rev]);
            desti->exit[rev] = NULL;
          }
          room->exit[dir] = new_exit();
          room->exit[dir]->u1.to_room = desti;
          room->exit[dir]->orig_door = dir;
          desti->exit[rev] = new_exit();
          desti->exit[rev]->u1.to_room = room;
          desti->exit[rev]->orig_door = rev;
          printf_to_char(ch, "Wilds Link: %d - %d\n\r", room->vnum, desti->vnum);
        }
      }
      if (linkupzone(room, WORLD_OTHER)) {
        if (validlinkroom(room)) {
          desti = fetchlinkroom(OTHER_FOREST_VNUM);
          if (desti == NULL)
          continue;
          if (number_percent() % 2 == 0)
          dir = DIR_WEST;
          else
          dir = DIR_SOUTHWEST;
          rev = rev_dir[dir];
          if (room->exit[dir] != NULL) {
            free_exit(room->exit[dir]);
            room->exit[dir] = NULL;
          }
          if (desti->exit[rev] != NULL) {
            free_exit(desti->exit[rev]);
            desti->exit[rev] = NULL;
          }
          room->exit[dir] = new_exit();
          room->exit[dir]->u1.to_room = desti;
          room->exit[dir]->orig_door = dir;
          desti->exit[rev] = new_exit();
          desti->exit[rev]->u1.to_room = room;
          desti->exit[rev]->orig_door = rev;
          printf_to_char(ch, "Other Link: %d - %d\n\r", room->vnum, desti->vnum);
        }
      }
    }
  }

  _DOFUN(do_fixocean) {
    AREA_DATA *pArea = ch->in_room->area;
    for (int i = pArea->min_vnum; i < pArea->max_vnum; i++) {
      if (get_room_index(i) != NULL) {
        if (get_room_index(i)->x > pArea->maxx)
        kill_room(get_room_index(i));
        if (get_room_index(i)->y > pArea->maxy)
        kill_room(get_room_index(i));
      }
    }
  }

  _DOFUN(do_hoursset) {
    AREA_DATA *pArea = ch->in_room->area;
    for (int i = pArea->min_vnum; i < pArea->max_vnum; i++) {
      if (get_room_index(i) != NULL) {
        get_room_index(i)->timezone = atoi(argument);
      }
    }
  }

  void offworld_weather_update(void) {
    AREA_DATA *pArea = get_area_data(HELL_FOREST_VNUM);
    ROOM_INDEX_DATA *room = get_room_index(pArea->min_vnum);
    if (room == NULL)
    return;
    LOCATION_TYPE *loc = get_loc("District 82");
    assign_weather(room, loc, FALSE);
    hell_temperature = room->temperature;
    hell_cloud_cover = room->cloud_cover;
    hell_cloud_density = room->cloud_density;
    hell_raining = room->raining;
    hell_hailing = room->hailing;
    hell_snowing = room->snowing;

    pArea = get_area_data(OTHER_FOREST_VNUM);
    room = get_room_index(pArea->min_vnum);
    if (room == NULL)
    return;
    loc = get_loc("Lauriea");
    assign_weather(room, loc, FALSE);
    other_temperature = room->temperature;
    other_cloud_cover = room->cloud_cover;
    other_cloud_density = room->cloud_density;
    other_raining = room->raining;
    other_hailing = room->hailing;
    other_snowing = room->snowing;

    pArea = get_area_data(GODREALM_FOREST_VNUM);
    room = get_room_index(pArea->min_vnum);
    if (room == NULL)
    return;
    loc = get_loc("Rhagost");
    assign_weather(room, loc, FALSE);
    godrealm_temperature = room->temperature;
    godrealm_cloud_cover = room->cloud_cover;
    godrealm_cloud_density = room->cloud_density;
    godrealm_raining = room->raining;
    godrealm_hailing = room->hailing;
    godrealm_snowing = room->snowing;

    pArea = get_area_data(WILDS_FOREST_VNUM);
    room = get_room_index(pArea->min_vnum);
    if (room == NULL)
    return;
    loc = get_loc("Navorost");
    assign_weather(room, loc, FALSE);
    wilds_temperature = room->temperature;
    wilds_cloud_cover = room->cloud_cover;
    wilds_cloud_density = room->cloud_density;
    wilds_raining = room->raining;
    wilds_hailing = room->hailing;
    wilds_snowing = room->snowing;
  }

  bool valid_antag_target(int faction, LOCATION_TYPE *terr) {
    if (terr->lockout > current_time)
    return FALSE;

    return TRUE;

  }

  int conflict_score(LOCATION_TYPE *loc)
  {
    int bscore = 50;
    for(int i=0;i<20;i++)
    bscore += loc->phil_amount[i];

    if(loc->status == STATUS_CONTEST)
    bscore *= 2;
    if(loc->status == STATUS_WAR)
    bscore *= 4;

    return bscore;
  }

  bool territory_has_op(LOCATION_TYPE *loc)
  {

    int num = number_from_territory(loc);
    for (vector<OPERATION_TYPE *>::iterator it = OpVect.begin();
    it != OpVect.end(); ++it) {
      if ((*it)->hour == 0)
      continue;

      if((*it)->territoryvnum == num)
      return TRUE;
    }
    return FALSE;
  }

  LOCATION_TYPE *antag_target(int faction) {
    int maxscore = 0;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if(!valid_antag_target(faction, (*it)))
      continue;

      if(territory_has_op((*it)))
      continue;

      if(conflict_score((*it)) > maxscore)
      maxscore = conflict_score(*it);
    }

    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;
      if(!valid_antag_target(faction, (*it)))
      continue;

      if(territory_has_op((*it)))
      continue;

      if(conflict_score((*it)) >= maxscore*4/5 && number_percent() % 3 == 0)
      return (*it);
    }

    return NULL;
  }

  _DOFUN(do_gengateways) {
    for (int ax = 22; ax < 26; ax++) {
      AREA_DATA *area = get_area_data(ax);
      for (int i = 0; i < 20; i++) {
        for (int x = 0; x < 100; x++) {
          int dir = number_range(DIR_NORTH, DIR_SOUTHWEST);
          if (dir != DIR_UP && dir != DIR_DOWN) {
            ROOM_INDEX_DATA *havenroom = get_random_outerforest();
            ROOM_INDEX_DATA *worldroom = get_lair_room(area->world);
            if (havenroom != NULL && worldroom != NULL && valid_gatedir(havenroom, dir)) {
              x = 200;
              GATEWAY_TYPE *gateway = new_gateway();
              gateway->havenroom = havenroom->vnum;
              gateway->worldroom = worldroom->vnum;
              gateway->direction = dir;
              gateway->world = area->world;
              gatewayVect.push_back(gateway);
              havenroom->exit[dir]->u1.to_room = worldroom;
              worldroom->exit[rev_dir[dir]]->u1.to_room = havenroom;
            }
          }
        }
      }
    }
  }

  bool offworld_gate_exists(ROOM_INDEX_DATA *desti, int dir) {
    if (desti == NULL)
    return TRUE;
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->worldroom == desti->vnum && (*it)->direction == dir)
      return TRUE;
    }
    return FALSE;
  }

  ROOM_INDEX_DATA *directional_outerforest(int dir) {
    ROOM_INDEX_DATA *havenroom = NULL;
    for (int x = 0; x < 100; x++) {
      havenroom = get_random_outerforest();
      if (havenroom != NULL && valid_gatedir(havenroom, dir))
      return havenroom;
    }
    return havenroom;
  }

  void postpass_gate(CHAR_DATA *ch, ROOM_INDEX_DATA *fromroom, ROOM_INDEX_DATA *toroom, int dir) {
    if (IS_NPC(ch))
    return;
    if (toroom->area->world != fromroom->area->world) {
      for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
      it != gatewayVect.end(); ++it) {
        if ((*it)->valid == FALSE)
        continue;
        if ((*it)->havenroom == fromroom->vnum && dir == (*it)->direction) {
          if (toroom->sector_type == SECT_STREET && fromroom->sector_type == SECT_STREET) {
            bool found = FALSE;
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->known_gateways[i] == (*it)->havenroom)
              found = TRUE;
            }
            if (found == FALSE) {
              for (int i = 0; i < 50; i++) {
                if (ch->pcdata->known_gateways[i] == 0) {
                  ch->pcdata->known_gateways[i] = (*it)->havenroom;
                  i = 100;
                }
              }
            }
          }
          else {
            if(number_percent() % 11 == 0) {
              ROOM_INDEX_DATA *newdoor = get_lair_room(toroom->area->world);
              if (newdoor != NULL) {
                toroom->exit[rev_dir[dir]]->u1.to_room =
                cardinal_room(toroom, rev_dir[dir]);
                newdoor->exit[rev_dir[dir]]->u1.to_room = fromroom;
                (*it)->worldroom = newdoor->vnum;
                toroom = newdoor;
              }
            }
            if (number_percent() % 11 == 0) {
              ROOM_INDEX_DATA *newearth = directional_outerforest(dir);
              if (newearth != NULL) {
                fromroom->exit[dir]->u1.to_room = cardinal_room(fromroom, dir);
                newearth->exit[dir]->u1.to_room = toroom;
                (*it)->havenroom = newearth->vnum;
              }
            }
            if (!IS_SET(toroom->area->area_flags, AREA_CHANGED))
            SET_BIT(toroom->area->area_flags, AREA_CHANGED);
            if (!IS_SET(fromroom->area->area_flags, AREA_CHANGED))
            SET_BIT(fromroom->area->area_flags, AREA_CHANGED);
          }
        }
        else if ((*it)->worldroom == fromroom->vnum && rev_dir[dir] == (*it)->direction) {
          if (toroom->sector_type == SECT_STREET && fromroom->sector_type == SECT_STREET) {
            bool found = FALSE;
            for (int i = 0; i < 50; i++) {
              if (ch->pcdata->known_gateways[i] == (*it)->havenroom)
              found = TRUE;
            }
            if (found == FALSE) {
              for (int i = 0; i < 50; i++) {
                if (ch->pcdata->known_gateways[i] == 0) {
                  ch->pcdata->known_gateways[i] = (*it)->havenroom;
                  i = 100;
                }
              }
            }
          }
          else {
            if(number_percent() % 11 == 0) {
              ROOM_INDEX_DATA *newdoor = directional_outerforest(rev_dir[dir]);
              if (newdoor != NULL) {
                toroom->exit[rev_dir[dir]]->u1.to_room =
                cardinal_room(toroom, rev_dir[dir]);
                newdoor->exit[rev_dir[dir]]->u1.to_room = fromroom;
                (*it)->havenroom = newdoor->vnum;
                toroom = newdoor;
              }
            }
            if (number_percent() % 11 == 0) {
              ROOM_INDEX_DATA *newearth = get_lair_room(fromroom->area->world);
              if (newearth != NULL) {
                fromroom->exit[dir]->u1.to_room = cardinal_room(fromroom, dir);
                newearth->exit[dir]->u1.to_room = toroom;
                (*it)->worldroom = newearth->vnum;
              }
            }
            if (!IS_SET(toroom->area->area_flags, AREA_CHANGED))
            SET_BIT(toroom->area->area_flags, AREA_CHANGED);
            if (!IS_SET(fromroom->area->area_flags, AREA_CHANGED))
            SET_BIT(fromroom->area->area_flags, AREA_CHANGED);
          }
        }
      }
    }
  }

  void prepass_gate(CHAR_DATA *ch, ROOM_INDEX_DATA *fromroom, ROOM_INDEX_DATA *toroom, int dir) {
    if (IS_NPC(ch))
    return;

    if (fromroom->sector_type != SECT_STREET)
    return;
    if (fromroom->area->world != WORLD_EARTH)
    return;
    if (toroom->sector_type == SECT_STREET)
    return;

    if (dir == DIR_NORTHWEST || dir == DIR_SOUTHWEST || dir == DIR_NORTHEAST || dir == DIR_SOUTHEAST) {
      if (toroom->area->world != fromroom->area->world) {
        for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
        it != gatewayVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if ((*it)->havenroom == fromroom->vnum && dir == (*it)->direction) {
            GATEWAY_TYPE *gate = (*it);
            gate->lastused = current_time;
            ROOM_INDEX_DATA *desti;
            if (dir == DIR_NORTHWEST) {
              desti = room_by_coordinates(toroom->area->minx + 25, toroom->area->miny + 24, 0);
              if (offworld_gate_exists(desti, dir))
              desti = room_by_coordinates(toroom->area->minx + 26, toroom->area->miny + 25, 0);
            }
            if (dir == DIR_NORTHEAST) {
              desti = room_by_coordinates(toroom->area->minx + 25, toroom->area->miny + 24, 0);
              if (offworld_gate_exists(desti, dir))
              desti = room_by_coordinates(toroom->area->minx + 24, toroom->area->miny + 25, 0);
            }
            if (dir == DIR_SOUTHEAST) {
              desti = room_by_coordinates(toroom->area->minx + 25, toroom->area->miny + 26, 0);
              if (offworld_gate_exists(desti, dir))
              desti = room_by_coordinates(toroom->area->minx + 24, toroom->area->miny + 25, 0);
            }
            if (dir == DIR_SOUTHWEST) {
              desti = room_by_coordinates(toroom->area->minx + 25, toroom->area->miny + 26, 0);
              if (offworld_gate_exists(desti, dir))
              desti = room_by_coordinates(toroom->area->minx + 26, toroom->area->miny + 25, 0);
            }

            if (offworld_gate_exists(desti, dir))
            return;
            toroom->exit[rev_dir[dir]]->u1.to_room =
            cardinal_room(toroom, rev_dir[dir]);
            desti->exit[rev_dir[dir]]->u1.to_room = fromroom;
            fromroom->exit[dir]->u1.to_room = desti;
            gate->worldroom = desti->vnum;
            if (!IS_SET(toroom->area->area_flags, AREA_CHANGED))
            SET_BIT(toroom->area->area_flags, AREA_CHANGED);
            if (!IS_SET(fromroom->area->area_flags, AREA_CHANGED))
            SET_BIT(fromroom->area->area_flags, AREA_CHANGED);
          }
        }
      }
    }
  }
  void clean_room(ROOM_INDEX_DATA *room) {
    EXTRA_DESCR_DATA *ed;
    EXTRA_DESCR_DATA *ped = NULL;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    for (obj = room->contents; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      if (IS_SET(obj->extra_flags, ITEM_RELIC)) {
        obj_from_room(obj);
        obj_to_room(obj, get_room_index(1));
      }
      else
      extract_obj(obj);
    }
    for (int i = 0; i < 10; i++) {
      if (room->exit[i] == NULL)
      continue;
      if (room->exit[i]->u1.to_room == NULL)
      continue;
      int rev = rev_dir[i];
      ROOM_INDEX_DATA *pToRoom = room->exit[i]->u1.to_room;

      if (pToRoom->exit[rev]) {
        free_exit(pToRoom->exit[rev]);
        pToRoom->exit[rev] = NULL;
      }
      free_exit(room->exit[i]);
      room->exit[i] = NULL;
    }

    free_string(room->name);
    room->name = str_dup("");
    free_string(room->description);
    room->description = str_dup("");
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

    RESET_DATA *pReset;

    for (int i = 30; i > 0; i--) {
      if (!room->reset_first) {
      }
      else if (i == 1) {
        pReset = room->reset_first;
        room->reset_first = room->reset_first->next;
        if (!room->reset_first)
        room->reset_last = NULL;
      }
      else if (i > 1) {
        int iReset = 0;
        RESET_DATA *prev = NULL;

        for (pReset = room->reset_first; pReset; pReset = pReset->next) {
          if (++iReset == i)
          break;
          prev = pReset;
        }
        if (!pReset) {
        }
        else {
          if (prev)
          prev->next = prev->next->next;
          else
          room->reset_first = room->reset_first->next;
          for (room->reset_last = room->reset_first; room->reset_last->next;
          room->reset_last = room->reset_last->next)
          ;
        }
      }
    }
  }
  bool border_territory(LOCATION_TYPE *loc) {
    if (loc == NULL)
    return FALSE;
    if (!str_cmp(loc->name, "District 82"))
    return TRUE;
    if (!str_cmp(loc->name, "Lauriea"))
    return TRUE;
    if (!str_cmp(loc->name, "Rhagost"))
    return TRUE;
    if (!str_cmp(loc->name, "Navorost"))
    return TRUE;
    return FALSE;
  }

  _DOFUN(do_fixgates) {
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      ROOM_INDEX_DATA *fromroom = get_room_index((*it)->havenroom);
      ROOM_INDEX_DATA *toroom = get_room_index((*it)->worldroom);
      if (fromroom->area->vnum >= 16 && fromroom->area->vnum <= 18) {
        ROOM_INDEX_DATA *newearth = directional_outerforest((*it)->direction);
        if (newearth != NULL) {
          fromroom->exit[(*it)->direction]->u1.to_room =
          cardinal_room(fromroom, (*it)->direction);
          newearth->exit[(*it)->direction]->u1.to_room = toroom;
          (*it)->havenroom = newearth->vnum;
        }
      }
      if (!IS_SET(toroom->area->area_flags, AREA_CHANGED))
      SET_BIT(toroom->area->area_flags, AREA_CHANGED);
      if (!IS_SET(fromroom->area->area_flags, AREA_CHANGED))
      SET_BIT(fromroom->area->area_flags, AREA_CHANGED);
    }
  }

  void fix_room_dirs(ROOM_INDEX_DATA *room) {
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if (room->vnum == (*it)->havenroom || room->vnum == (*it)->worldroom)
      return;
    }
    for (int i = 0; i < MAX_DIR; i++) {

      bool found = FALSE;
      if (room->exit[i] != NULL && room->exit[i]->u1.to_room != NULL) {
        if (IS_SET(room->exit[i]->exit_info, EX_HIDDEN))
        REMOVE_BIT(room->exit[i]->exit_info, EX_HIDDEN);

        if (is_cardinal_room(room, room->exit[i]->u1.to_room, i))
        found = TRUE;

        if (found == FALSE) {
          ROOM_INDEX_DATA *toroom = cardinal_room(room, i);
          if (toroom != NULL)
          room->exit[i]->u1.to_room = toroom;
        }
      }
    }
  }

  void fix_area_dirs(AREA_DATA *pArea) {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area == pArea)
        fix_room_dirs(pRoomIndex);
      }
    }
  }

  _DOFUN(do_areadirs) {
    int value = atoi(argument);
    AREA_DATA *pArea;
    if (!(pArea = get_area_data(value))) {
      if (ch)
      send_to_char("That area does not exist.\n\r", ch);
      return;
    }
    fix_area_dirs(pArea);
    send_to_char("Done.\n\r", ch);
  }

  void fix_forest_dirs(AREA_DATA *pArea) {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area == pArea)
        reset_forest_room(pRoomIndex);
      }
    }
  }

  _DOFUN(do_forestrest) {
    int value = atoi(argument);
    AREA_DATA *pArea;
    if (!(pArea = get_area_data(value))) {
      if (ch)
      send_to_char("That area does not exist.\n\r", ch);
      return;
    }
    fix_forest_dirs(pArea);
    send_to_char("Done.\n\r", ch);
  }

  void clean_forest_room(ROOM_INDEX_DATA *room) {}

  void clean_forest_dirs(AREA_DATA *pArea) {
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
      for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
      pRoomIndex = pRoomIndex->next) {
        if (pRoomIndex->area == pArea)
        clean_forest_room(pRoomIndex);
      }
    }
  }

  _DOFUN(do_forestcleanout) {
    int value = atoi(argument);
    AREA_DATA *pArea;
    if (!(pArea = get_area_data(value))) {
      if (ch)
      send_to_char("That area does not exist.\n\r", ch);
      return;
    }
    clean_forest_dirs(pArea);
    send_to_char("Done.\n\r", ch);
  }

  _DOFUN(do_rangedirs) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    for (int i = atoi(arg1); i <= atoi(argument); i++) {
      ROOM_INDEX_DATA *troom = get_room_index(i);
      if (troom != NULL)
      fix_room_dirs(troom);
    }
    send_to_char("Done.\n\r", ch);
  }

  bool territory_leader(CHAR_DATA *ch, LOCATION_TYPE *loc)
  {
    if(ch->fcore == loc->base_faction_core && is_leader(ch, ch->fcore))
    return TRUE;
    if(ch->fcult == loc->base_faction_cult && is_leader(ch, ch->fcult))
    return TRUE;
    if(ch->fsect == loc->base_faction_sect && is_leader(ch, ch->fsect))
    return TRUE;

    return FALSE;

  }


  const char *decree_names[6] = {"None", "Lockdown", "Border Control", "Construction Grant", "Sponsorship", "Overtax"};

  _DOFUN(do_decree)
  {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);
    int count = 1;
    if(!str_cmp(arg1, "list"))
    {
      for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
      it != DecreeVect.end(); ++it) {
        if((*it)->valid == FALSE)
        continue;
        if((*it)->start_time > 0 && (*it)->end_time > current_time)
        printf_to_char(ch, "`g%02d) %s in %s`x\n\r", count, decree_names[(*it)->btype], territory_by_number((*it)->territory_vnum));
        else if((*it)->start_time > 0 && (*it)->end_time < current_time)
        printf_to_char(ch, "`D%02d) %s in %s`x\n\r", count, decree_names[(*it)->btype], territory_by_number((*it)->territory_vnum));
        else
        printf_to_char(ch, "`r%02d) %s in %s`x\n\r", count, decree_names[(*it)->btype], territory_by_number((*it)->territory_vnum));
        count++;
      }
      return;
    }
    else if(!str_cmp(arg1, "info"))
    {
      DECREE_TYPE *decree = NULL;
      if(is_number(argument))
      {
        int num = atoi(argument);
        count = 1;
        for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
        it != DecreeVect.end(); ++it) {
          if((*it)->valid == FALSE)
          continue;
          if(count == num)
          {
            decree = (*it);
            break;
          }
          count++;
        }
      }
      else
      {
        for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
        it != DecreeVect.end(); ++it) {
          if((*it)->valid == FALSE)
          continue;
          if(!str_cmp(argument, territory_by_number((*it)->territory_vnum)->name))
          {
            decree = (*it);
            break;
          }
        }
      }
      if(decree == NULL)
      {
        send_to_char("No such decree.\n\r", ch);
        return;
      }
      printf_to_char(ch, "%s decree in %s\n\n%s\n\nVotes: %s, %s, %s\n\r", decree_names[decree->btype], territory_by_number(decree->territory_vnum), decree->desc, decree->vote_1, decree->vote_2, decree->vote_3);
      return;
    }
    else if(!str_cmp(arg1, "withdraw"))
    {
      DECREE_TYPE *decree = NULL;
      if(is_number(argument))
      {
        int num = atoi(argument);
        count = 1;
        for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
        it != DecreeVect.end(); ++it) {
          if((*it)->valid == FALSE)
          continue;
          if(count == num)
          {
            decree = (*it);
            break;
          }
          count++;
        }
      }
      else
      {
        for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
        it != DecreeVect.end(); ++it) {
          if((*it)->valid == FALSE)
          continue;
          if(!str_cmp(argument, territory_by_number((*it)->territory_vnum)->name))
          {
            decree = (*it);
            break;
          }
        }
      }
      if(decree == NULL)
      {
        send_to_char("No such decree.\n\r", ch);
        return;
      }
      if(str_cmp(decree->vote_1, ch->name))
      {
        send_to_char("You are not the sponsor of this decree.\n\r", ch);
        return;
      }
      if(strlen(decree->vote_2) > 2)
      {
        send_to_char("You cannot withdraw a decree that has already been voted on.\n\r", ch);
        return;
      }
      decree->valid = FALSE;
      send_to_char("Decree withdrawn.\n\r", ch);
      return;
    }
    else if(!str_cmp(arg1, "support"))
    {
      DECREE_TYPE *decree = NULL;
      if(is_number(argument))
      {
        int num = atoi(argument);
        count = 1;
        for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
        it != DecreeVect.end(); ++it) {
          if((*it)->valid == FALSE)
          continue;
          if(count == num)
          {
            decree = (*it);
            break;
          }
          count++;
        }
      }
      else
      {
        for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
        it != DecreeVect.end(); ++it) {
          if((*it)->valid == FALSE)
          continue;
          if(!str_cmp(argument, territory_by_number((*it)->territory_vnum)->name))
          {
            decree = (*it);
            break;
          }
        }
      }
      if(decree == NULL)
      {
        send_to_char("No such decree.\n\r", ch);
        return;
      }
      LOCATION_TYPE *loc = territory_by_number(decree->territory_vnum);
      if(!territory_leader(ch, loc))
      {
        send_to_char("You are not a leader of this territory.\n\r", ch);
        return;
      }
      if(!str_cmp(decree->vote_1, ch->name))
      {
        send_to_char("You are the sponsor of this decree.\n\r", ch);
        return;
      }
      if(!str_cmp(decree->vote_2, ch->name))
      {
        send_to_char("You have already voted on this decree.\n\r", ch);
        return;
      }
      if(!str_cmp(decree->vote_3, ch->name))
      {
        send_to_char("You have already voted on this decree.\n\r", ch);
        return;
      }
      if(strlen(decree->vote_2) < 2)
      {
        free_string(decree->vote_2);
        decree->vote_2 = str_dup(ch->name);
        send_to_char("You have voted to support this decree.\n\r", ch);
        decree->start_time = current_time;
        decree->end_time = current_time + (3600*24*7);
        return;
      }
      else
      {
        free_string(decree->vote_3);
        decree->vote_3 = str_dup(ch->name);
        send_to_char("You have voted to support this decree.\n\r", ch);
        decree->end_time = decree->end_time + (3600*24*4);
        return;
      }
    }
    else
    {
      send_to_char("Syntax: decree <list/info/support/withdraw> <number>\n\r", ch);
      return;
    }


  }

  bool world_blocked(CHAR_DATA *ch, int world)
  {
    if(ch == NULL || IS_NPC(ch))
    return FALSE;
    if(world != WORLD_OTHER && world != WORLD_HELL && world != WORLD_WILDS && world != WORLD_GODREALM)
    return FALSE;

    for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
    it != DecreeVect.end(); ++it) {
      if((*it)->valid == FALSE)
      continue;
      if((*it)->start_time < current_time || (*it)->end_time > current_time)
      continue;
      if(world == WORLD_OTHER && (*it)->btype == DECREE_LOCKDOWN && (*it)->territory_vnum ==    number_from_territory(get_loc("Lauriea")))
      return TRUE;
      if(world == WORLD_OTHER && ch->pcdata->resident != world && (*it)->btype == DECREE_BORDER && (*it)->territory_vnum == number_from_territory(get_loc("Lauriea")))
      return TRUE;

      if(world == WORLD_HELL && (*it)->btype == DECREE_LOCKDOWN && (*it)->territory_vnum ==    number_from_territory(get_loc("District 82")))
      return TRUE;
      if(world == WORLD_HELL && ch->pcdata->resident != world && (*it)->btype == DECREE_BORDER && (*it)->territory_vnum == number_from_territory(get_loc("District 82")))
      return TRUE;


      if(world == WORLD_WILDS && (*it)->btype == DECREE_LOCKDOWN && (*it)->territory_vnum ==    number_from_territory(get_loc("Navorost")))
      return TRUE;
      if(world == WORLD_WILDS && ch->pcdata->resident != world && (*it)->btype == DECREE_BORDER && (*it)->territory_vnum == number_from_territory(get_loc("Navorost")))
      return TRUE;


      if(world == WORLD_GODREALM && (*it)->btype == DECREE_LOCKDOWN && (*it)->territory_vnum ==    number_from_territory(get_loc("Rhagost")))
      return TRUE;
      if(world == WORLD_GODREALM && ch->pcdata->resident != world && (*it)->btype == DECREE_BORDER && (*it)->territory_vnum == number_from_territory(get_loc("Rhagost")))
      return TRUE;

    }

    return FALSE;
  }

  bool decree_target(int world, int decree_type, char * name)
  {

    if(world != WORLD_OTHER && world != WORLD_HELL && world != WORLD_WILDS && world != WORLD_GODREALM)
    return FALSE;

    for (vector<DECREE_TYPE *>::iterator it = DecreeVect.begin();
    it != DecreeVect.end(); ++it) {
      if((*it)->valid == FALSE)
      continue;
      if((*it)->start_time < current_time || (*it)->end_time > current_time)
      continue;
      if(world == WORLD_OTHER && (*it)->btype == decree_type && (*it)->territory_vnum ==    number_from_territory(get_loc("Lauriea")) && !str_cmp((*it)->target, name))
      return TRUE;
      if(world == WORLD_HELL && (*it)->btype == decree_type && (*it)->territory_vnum ==    number_from_territory(get_loc("District 82")) && !str_cmp((*it)->target, name))
      return TRUE;
      if(world == WORLD_WILDS && (*it)->btype == decree_type && (*it)->territory_vnum ==    number_from_territory(get_loc("Navorost")) && !str_cmp((*it)->target, name))
      return TRUE;
      if(world == WORLD_GODREALM && (*it)->btype == decree_type && (*it)->territory_vnum ==    number_from_territory(get_loc("Rhagost")) && !str_cmp((*it)->target, name))
      return TRUE;
    }
    return FALSE;
  }

#if defined(__cplusplus)
}
#endif
