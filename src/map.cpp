//the template file "towntemplate.bmp" must be a 24 bit BMP with no color space information written.
//You can do this in GIMP by checking the advanced and capatibility sections respectively. - Discordance

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

#include <stdio.h>
#pragma pack(2)

  int mapnumber = 1;

  sh_int asciimap[150][189];

  typedef struct {
    char signature[2];
    unsigned int fileSize;
    unsigned int reserved;
    unsigned int offset;
  } BmpHeader;

  typedef struct {
    unsigned int headerSize;
    unsigned int width;
    unsigned int height;
    unsigned short planeCount;
    unsigned short bitDepth;
    unsigned int compression;
    unsigned int compressedImageSize;
    unsigned int horizontalResolution;
    unsigned int verticalResolution;
    unsigned int numColors;
    unsigned int importantColors;

  } BmpImageInfo;

  typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    // unsigned char reserved; Removed for convenience in fread; info.bitDepth/8
    // doesn't seem to work for some reason
  } Rgb;

#define COLOUR_WAR 1
#define COLOUR_WILD 2
#define COLOUR_CONTEST 3
#define COLOUR_HAND 4
#define COLOUR_ORDER 5
#define COLOUR_TEMPLE 6
#define COLOUR_OTHER 7

#define MAP_FOREST 1
#define MAP_PARK 2
#define MAP_SHALLOW 3
#define MAP_WATER 4
#define MAP_SHOP 5
#define MAP_HOUSE 6
#define MAP_TOWN 7
#define MAP_STREET 8
#define MAP_ROAD 9
#define MAP_BEACH 10
#define MAP_ROCKS 11
#define MAP_OTHER 12
#define MAP_ERROR 13
#define MAP_SHOPOWNED 14
#define MAP_HOUSEOWNED 15
#define MAP_HARMONYBASE 16
#define MAP_DOMBASE 17
#define MAP_RESISTBASE 18
#define MAP_OTHERBASE 19
#define MAP_ALLEY 20
#define MAP_LODGE 21
#define MAP_MEDICAL 22
#define MAP_DOCKS 23
#define MAP_PARKING 24
#define MAP_SIDEWALK 25
#define MAP_CEMETARY 26
#define MAP_DIRT 27
#define MAP_DARWINBASE 28
#define MAP_PATRONBASE 29

  void process_territories() {
    int othermax, val;
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      if ((*it)->continent == CONTINENT_WILDS || (*it)->continent == CONTINENT_OTHER || (*it)->continent == CONTINENT_GODREALM || (*it)->continent == CONTINENT_HELL)
      continue;

      if ((*it)->status == STATUS_CONTEST) {
        (*it)->radius = 25;
        (*it)->colour = COLOUR_CONTEST;
      }
      else if ((*it)->status == STATUS_WAR) {
        (*it)->radius = 25;
        (*it)->colour = COLOUR_WAR;
      }
      else {
        othermax = 0;
        for (int i = 0; i < 5; i++) {
          if ((*it)->other_amount[i] > othermax)
          othermax = (*it)->other_amount[i];
        }
        int philmax = 0;
        int philpoint = 0;
        for (int i = 0; i < 10; i++) {
          if ((*it)->phil_amount[i] > philmax) {
            philmax = (*it)->phil_amount[i];
            philpoint = i;
          }
        }
        int offphilcount = 0;
        for (int i = 0; i < 10; i++) {
          if (i != philpoint)
          offphilcount += (*it)->phil_amount[i];
        }

        if (philmax > othermax) {
          val = philmax + philmax - offphilcount - othermax;
          val = UMAX(val, philmax / 2);
          val = val * val;
          val = (int)(cbrt(val));
          val = UMAX(val, 10);
          (*it)->radius = val;
          (*it)->colour = philpoint + 10;
        }
        else {
          val = othermax + othermax - offphilcount - philmax;
          val = UMAX(val, othermax / 2);
          val = val * val;
          val = (int)(cbrt(val));
          val = UMAX(val, 10);

          (*it)->radius = val;

          (*it)->colour = COLOUR_OTHER;
        }
      }
      /*
if(!str_prefix("Belfast", (*it)->name))
(*it)->radius /= 3;
if(!str_prefix("Dublin", (*it)->name))
(*it)->radius /= 3;
if(!str_prefix("Jerusalem", (*it)->name))
(*it)->radius /= 3;
if(!str_prefix("Moscow", (*it)->name))
(*it)->radius *= 2;
if(!str_prefix("Helsinki", (*it)->name))
(*it)->radius = (*it)->radius/3;
if(!str_prefix("Bucharest", (*it)->name))
(*it)->radius /= 2;
if(!str_prefix("Ulaanbaatar", (*it)->name))
(*it)->radius *=2;
if(!str_prefix("Merida", (*it)->name))
(*it)->radius /= 2;
if(!str_prefix("Rome", (*it)->name))
(*it)->radius = (*it)->radius*2/3;
if(!str_prefix("Milan", (*it)->name))
(*it)->radius = (*it)->radius*2/3;
if(!str_prefix("Gaborone", (*it)->name))
(*it)->radius = (*it)->radius*2/3;
if(!str_prefix("Lusaka", (*it)->name))
(*it)->radius = (*it)->radius*17/10;
if(!str_prefix("Niamey", (*it)->name))
(*it)->radius = (*it)->radius*3/2;
if(!str_prefix("Cordoba", (*it)->name))
(*it)->radius = (*it)->radius*3/2;
if(!str_prefix("Hand", (*it)->name))
(*it)->radius = (*it)->radius*13/3;
if(!str_prefix("Dynasty", (*it)->name))
(*it)->radius = (*it)->radius*3;
if(!str_prefix("Novosibirsk", (*it)->name))
(*it)->radius = (*it)->radius*3/2;
if(!str_prefix("Shanghi", (*it)->name))
(*it)->radius = (*it)->radius/2;
if(!str_prefix("Beijing", (*it)->name))
(*it)->radius = (*it)->radius/2;
if(!str_prefix("Seoul", (*it)->name))
(*it)->radius = (*it)->radius/2;
*/
    }
  }

  _DOFUN(do_testter) {
    process_territories();
    for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
    it != locationVect.end(); ++it) {
      if (!(*it)->name || (*it)->name[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE)
      continue;

      printf_to_char(ch, "%s: %d\n\r", (*it)->name, (*it)->colour);
    }
  }

#define BMPONE "../data/worldmap.bmp"
#define BMPTWO "../../../../var/www/html/worldmap.bmp"

  _DOFUN(do_bmp) { makeworldmap(); }
  void makeworldmap() {
    FILE *inFile;
    FILE *outFile;
    BmpHeader header;
    BmpImageInfo info;
    Rgb *palette;
    int i = 0;

    process_territories();

    inFile = fopen(BMPONE, "rb");
    if (!inFile) {
      return;
    }
    outFile = fopen(BMPTWO, "w");
    if (!outFile) {
      return;
    }

    if (fread(&header, 1, sizeof(BmpHeader), inFile) != sizeof(BmpHeader)) {
      return;
    }
    fwrite(&header, 1, sizeof(BmpHeader), outFile);

    if (fread(&info, 1, sizeof(BmpImageInfo), inFile) != sizeof(BmpImageInfo)) {
      return;
    }
    fwrite(&info, 1, sizeof(BmpImageInfo), outFile);

    if (info.numColors > 0) {
      palette = (Rgb *)malloc(sizeof(Rgb) * info.numColors);
      if (fread(palette, sizeof(Rgb), info.numColors, inFile) !=
          (info.numColors * sizeof(Rgb))) {
        return; // error
      }
      fwrite(palette, sizeof(Rgb), info.numColors, outFile);
    }

    /*
FILE *outFile = fopen( BMPTWO, "wb" );
if( !outFile ) {
printf( "Error opening outputfile.\n" );
return -1;
}
*/
    Rgb *pixel = (Rgb *)malloc(sizeof(Rgb));
    int read, j;
    for (j = 0; j < (int)(info.height); j++) {
      //		printf( "------ Row %d\n", j+1 );
      read = 0;
      for (i = 0; i < (int)(info.width); i++) {
        if (fread(pixel, 1, sizeof(Rgb), inFile) != sizeof(Rgb)) {
          // printf( "Error reading pixel!\n" );
          return;
        }

        // Add 50 to the j/y coord from image to here.

        if (pixel->blue >= pixel->red && pixel->blue >= pixel->green) {

        }
        else if (pixel->blue <= 10 && pixel->red <= 10 && pixel->green <= 10) {
        }
        /*
else if(pixel->blue <= pixel->red + 50 && pixel->blue >= pixel->red - 50 && pixel->blue <= pixel->green + 50 && pixel->blue
>= pixel->green - 50 && pixel->red <= pixel->green + 50 && pixel->red >=
pixel->green -50)// && pixel->red < 120 && pixel->blue < 120 && pixel->green < 120)
{
}
*/
        else {
          bool keepgoing = TRUE;
          for (vector<LOCATION_TYPE *>::iterator it = locationVect.begin();
          it != locationVect.end(); ++it) {
            if (!(*it)->name || (*it)->name[0] == '\0') {
              continue;
            }
            if ((*it)->valid == FALSE)
            continue;
            if (keepgoing == FALSE)
            continue;

            if ((*it)->continent == CONTINENT_WILDS || (*it)->continent == CONTINENT_OTHER || (*it)->continent == CONTINENT_GODREALM || (*it)->continent == CONTINENT_HELL)
            continue;

            if (get_dist(i, j, (*it)->x, 953 - (*it)->y) <= (*it)->radius) {
              keepgoing = FALSE;
              switch ((*it)->colour) {
              case COLOUR_WAR:
                if (get_dist(i, j, (*it)->x, 953 - (*it)->y) % 5 == 0 || get_dist(i, j, (*it)->x, 953 - (*it)->y) <= 1) {
                  pixel->red = 0;
                  pixel->blue = 0;
                  pixel->green = 0;
                }
                else {
                  pixel->red = 255;
                  pixel->blue = 0;
                  pixel->green = 0;
                }
                break;
              case COLOUR_CONTEST:
                pixel->red = UMIN(250, pixel->red + (250 - pixel->red) / 2);
                pixel->red = UMIN(250, pixel->red + (250 - pixel->red) / 2);
                pixel->blue = UMAX(12, pixel->blue / 5);
                pixel->green = UMAX(12, pixel->green / 5);
                break;
              case (ALLIANCE_SIDELEFT + 10):
                pixel->blue = UMAX(6, pixel->blue / 4);
                pixel->red = UMIN(250, pixel->red + (250 - pixel->red) / 2);
                pixel->green = UMIN(250, pixel->green + (250 - pixel->green) / 4);
                pixel->red = UMIN(250, pixel->red + (250 - pixel->red) / 2);
                pixel->green = UMIN(250, pixel->green + (250 - pixel->green) / 4);
                break;
              case (ALLIANCE_SIDEMID + 10):
                pixel->green = UMAX(12, pixel->green / 2);
                pixel->red = UMIN(250, pixel->red + (250 - pixel->red) / 2);
                pixel->blue = UMIN(250, pixel->blue + (250 - pixel->blue) / 2);
                break;
              case (ALLIANCE_SIDERIGHT + 10):
                pixel->green = UMIN(250, pixel->green + (250 - pixel->green) / 2);
                pixel->green = UMIN(250, pixel->green + (250 - pixel->green) / 2);
                pixel->blue = UMAX(9, pixel->blue / 3);
                pixel->red = UMAX(9, pixel->red / 3);
                break;
              case COLOUR_OTHER:
                pixel->green = UMIN(250, pixel->green + (250 - pixel->green) / 2);
                pixel->blue = UMIN(250, pixel->blue + (250 - pixel->blue) / 2);
                pixel->red = UMAX(12, pixel->red * 2 / 3);
                break;
                /*
case COLOUR_HANDORDER:
if(i%6 == 0 || i%6 ==
1 || i%6 == 2)
{
pixel->red =
UMAX(12, pixel->red/2); pixel->blue = UMAX(12, pixel->blue/2);
pixel->green =
UMAX(12, pixel->green/2);
}
else
{
pixel->red =
UMIN(250, pixel->red + (250-pixel->red)/2); pixel->blue =
UMIN(250, pixel->blue + (250-pixel->blue)/2); pixel->green =
UMIN(250, pixel->green + (250-pixel->green)/2);
}
break;
case COLOUR_HANDTEMPLE:
if(i%6 == 0 || i%6 ==
1 || i%6 == 2)
{
pixel->red =
UMAX(12, pixel->red/2); pixel->blue = UMAX(12, pixel->blue/2);
pixel->green =
UMAX(12, pixel->green/2);
}
else
{
pixel->green =
UMIN(255, pixel->green + (255-pixel->green)/2); pixel->blue =
UMAX(12, pixel->blue/2); pixel->red = UMAX(12, pixel->red/2);
}
break;
case COLOUR_HANDOTHER:
if(i%6 == 0 || i%6 ==
1 || i%6 == 2)
{
pixel->red =
UMAX(12, pixel->red/2); pixel->blue = UMAX(12, pixel->blue/2);
pixel->green =
UMAX(12, pixel->green/2);
}
else
{
pixel->green =
UMIN(255, pixel->green + (255-pixel->green)/2); pixel->blue =
UMIN(255, pixel->blue + (255-pixel->blue)/2); pixel->red =
UMAX(12, pixel->red/2);
}
break;
case COLOUR_ORDERTEMPLE:
if(i%6 == 0 || i%6 ==
1 || i%6 == 2)
{
pixel->red =
UMIN(250, pixel->red + (250-pixel->red)/2); pixel->blue =
UMIN(250, pixel->blue + (250-pixel->blue)/2); pixel->green =
UMIN(250, pixel->green + (250-pixel->green)/2);
}
else
{
pixel->green =
UMIN(255, pixel->green + (255-pixel->green)/2); pixel->blue =
UMAX(12, pixel->blue/2); pixel->red = UMAX(12, pixel->red/2);
}
break;
case COLOUR_ORDEROTHER:
if(i%6 == 0 || i%6 ==
1 || i%6 == 2)
{
pixel->red =
UMIN(250, pixel->red + (250-pixel->red)/2); pixel->blue =
UMIN(250, pixel->blue + (250-pixel->blue)/2); pixel->green =
UMIN(250, pixel->green + (250-pixel->green)/2);
}
else
{
pixel->green =
UMIN(255, pixel->green + (255-pixel->green)/2); pixel->blue =
UMIN(255, pixel->blue + (255-pixel->blue)/2); pixel->red =
UMAX(12, pixel->red/2);
}
break;
case COLOUR_TEMPLEOTHER:
if(i%6 == 0 || i%6 ==
1 || i%6 == 2)
{
pixel->green =
UMIN(255, pixel->green + (255-pixel->green)/2); pixel->blue =
UMAX(12, pixel->blue/2); pixel->red = UMAX(12, pixel->red/2);
}
else
{
pixel->green =
UMIN(255, pixel->green + (255-pixel->green)/2); pixel->blue =
UMIN(255, pixel->blue + (255-pixel->blue)/2); pixel->red =
UMAX(12, pixel->red/2);
}
break;
*/
              }
            }
          }
        }

        fwrite(pixel, 1, sizeof(Rgb), outFile);
        read += sizeof(Rgb);
        // printf( "Pixel %d: %3d %3d %3d\n", i+1, pixel->red, pixel->green, // pixel->blue );
      }
      if (read % 4 != 0) {
        read = 4 - (read % 4);
        // printf( "Padding: %d bytes\n", read );
        fread(pixel, read, 1, inFile);
        fwrite(pixel, read, 1, outFile);
      }
    }

    fclose(inFile);
    fclose(outFile);

    return;
  }

#define BMPTHREE "../data/miniwhite.bmp"
#define BMPFOUR "../../../../var/www/html/maps/town.bmp"
#define BMPFIVE "../data/towntemplate.bmp"

  int get_pixelx(int x) {
    x = x * 32 / 7;
    x -= 1998;
    x -= 48;
    x = UMAX(x, 0);
    x = UMIN(x, 481);
    return x;
  }
  int get_pixely(int y) {
    int orig = y;
    y = y * 32 / 7;
    y -= 2006;
    y -= 82;
    if (orig > 510)
    y--;
    if (orig > 528)
    y--;
    if (orig > 540)
    y--;
    y = UMAX(y, 0);
    y = UMIN(y, 405);
    return y;
  }

  int bluemap(sh_int value) {
    if (value == MAP_FOREST)
    return 20;
    if (value == MAP_PARK)
    return 20 * 5 / 4;
    if (value == MAP_CEMETARY)
    return 20 * 5 / 4;
    if (value == MAP_SHALLOW)
    return 255;
    if (value == MAP_WATER)
    return 255;
    if (value == MAP_SHOP)
    return 87;
    if (value == MAP_HOUSE)
    return 217;
    if (value == MAP_SHOPOWNED)
    return 87 * 3 / 4;
    if (value == MAP_HOUSEOWNED)
    return 217 * 3 / 4;
    if (value == MAP_TOWN)
    return 128;
    if (value == MAP_LODGE)
    return 128;
    if (value == MAP_SIDEWALK)
    return 180;
    if (value == MAP_DOCKS)
    return 0;
    if (value == MAP_MEDICAL)
    return 36;
    if (value == MAP_OTHERBASE)
    return 0;
    if (value == MAP_STREET)
    return 130;
    if (value == MAP_ALLEY)
    return 130 * 4 / 5;
    if (value == MAP_PARKING)
    return 130 * 4 / 5;
    if (value == MAP_ROAD)
    return 0;
    if (value == MAP_DIRT)
    return 0;
    if (value == MAP_BEACH)
    return 128;
    if (value == MAP_ROCKS)
    return 88;
    if (value == MAP_OTHER)
    return 200;
    if (value == MAP_ERROR)
    return 100;
    if (value == MAP_DOMBASE)
    return 0;
    if (value == MAP_HARMONYBASE)
    return 255;
    if (value == MAP_RESISTBASE)
    return 0;
    if (value == MAP_PATRONBASE)
    return 128;
    if (value == MAP_DARWINBASE)
    return 0;

    return 255;
  }
  int redmap(sh_int value) {
    if (value == MAP_FOREST)
    return 23;
    if (value == MAP_PARK)
    return 23 * 5 / 4;
    if (value == MAP_CEMETARY)
    return 23 * 5 / 4;
    if (value == MAP_SHALLOW)
    return 30;
    if (value == MAP_WATER)
    return 0;
    if (value == MAP_SHOP)
    return 255;
    if (value == MAP_HOUSE)
    return 255;
    if (value == MAP_TOWN)
    return 64;
    if (value == MAP_SIDEWALK)
    return 180;
    if (value == MAP_LODGE)
    return 64;
    if (value == MAP_DOCKS)
    return 128;
    if (value == MAP_MEDICAL)
    return 255;
    if (value == MAP_OTHERBASE)
    return 128;
    if (value == MAP_HOUSEOWNED)
    return 255 * 3 / 4;
    if (value == MAP_SHOPOWNED)
    return 255 * 3 / 4;
    if (value == MAP_STREET)
    return 130;
    if (value == MAP_ALLEY)
    return 130 * 4 / 5;
    if (value == MAP_PARKING)
    return 130 * 4 / 5;
    if (value == MAP_ROAD)
    return 157;
    if (value == MAP_DIRT)
    return 157;
    if (value == MAP_BEACH)
    return 255;
    if (value == MAP_ROCKS)
    return 88;
    if (value == MAP_OTHER)
    return 200;
    if (value == MAP_ERROR)
    return 255;
    if (value == MAP_RESISTBASE)
    return 128;
    if (value == MAP_DOMBASE)
    return 0;
    if (value == MAP_HARMONYBASE)
    return 255;
    if (value == MAP_DARWINBASE)
    return 189;
    if (value == MAP_PATRONBASE)
    return 128;

    return 255;
  }
  int greenmap(sh_int value) {
    if (value == MAP_FOREST)
    return 90;
    if (value == MAP_PARK)
    return 90 * 5 / 4;
    if (value == MAP_CEMETARY)
    return 90 * 5 / 4;
    if (value == MAP_SHALLOW)
    return 58; // 30;
    if (value == MAP_WATER)
    return 0;
    if (value == MAP_SHOP)
    return 171;
    if (value == MAP_HOUSE)
    return 179;
    if (value == MAP_SHOPOWNED)
    return 171 * 3 / 4;
    if (value == MAP_HOUSEOWNED)
    return 179 * 3 / 4;
    if (value == MAP_TOWN)
    return 128;
    if (value == MAP_LODGE)
    return 128;
    if (value == MAP_SIDEWALK)
    return 180;
    if (value == MAP_DOCKS)
    return 64;
    if (value == MAP_MEDICAL)
    return 36;
    if (value == MAP_OTHERBASE)
    return 128;
    if (value == MAP_STREET)
    return 130;
    if (value == MAP_ALLEY)
    return 130 * 4 / 5;
    if (value == MAP_PARKING)
    return 130 * 4 / 5;
    if (value == MAP_ROAD)
    return 79;
    if (value == MAP_DIRT)
    return 79;
    if (value == MAP_BEACH)
    return 255;
    if (value == MAP_ROCKS)
    return 88;
    if (value == MAP_OTHER)
    return 200;
    if (value == MAP_ERROR)
    return 100;
    if (value == MAP_RESISTBASE)
    return 64;
    if (value == MAP_DOMBASE)
    return 0;
    if (value == MAP_HARMONYBASE)
    return 255;
    if (value == MAP_PATRONBASE)
    return 0;
    if (value == MAP_DARWINBASE)
    return 0;

    return 255;
  }

#define ASCII_HORIZONTAL 1
#define ASCII_VERTICAL 2

  bool ascii_deliniate(int typeone, int typetwo, int xcoord, int ycoord, int direction) {
    if (typeone == typetwo)
    return FALSE;

    if (typetwo == MAP_STREET || typetwo == MAP_ALLEY) {
      if (direction == ASCII_VERTICAL && ycoord == 131)
      return FALSE;
      if (direction == ASCII_HORIZONTAL && xcoord == 58)
      return FALSE;
    }

    if (typeone == MAP_ALLEY) {
      if (typetwo == MAP_STREET)
      return FALSE;
      if (typetwo == MAP_ROAD)
      return FALSE;
      if (typetwo == MAP_SIDEWALK)
      return FALSE;
      if (typetwo == MAP_PARKING)
      return FALSE;
    }

    if (typeone == MAP_STREET) {
      if (typetwo == MAP_ALLEY)
      return FALSE;
      if (typetwo == MAP_ROAD)
      return FALSE;
      if (typetwo == MAP_SIDEWALK)
      return FALSE;
      if (typetwo == MAP_PARKING)
      return FALSE;
    }

    if (typeone == MAP_ROAD) {
      if (typetwo == MAP_STREET)
      return FALSE;
      if (typetwo == MAP_ALLEY)
      return FALSE;
      if (typetwo == MAP_SIDEWALK)
      return FALSE;
      if (typetwo == MAP_PARKING)
      return FALSE;
    }

    if (typeone == MAP_PARKING) {
      if (typetwo == MAP_STREET)
      return FALSE;
      if (typetwo == MAP_ALLEY)
      return FALSE;
      if (typetwo == MAP_SIDEWALK)
      return FALSE;
      if (typetwo == MAP_ROAD)
      return FALSE;
    }

    if (typeone == MAP_SIDEWALK) {
      if (typetwo == MAP_STREET)
      return FALSE;
      if (typetwo == MAP_ALLEY)
      return FALSE;
      if (typetwo == MAP_PARKING)
      return FALSE;
      if (typetwo == MAP_ROAD)
      return FALSE;
    }

    if (typeone == MAP_SHALLOW) {
      if (typetwo == MAP_WATER)
      return FALSE;
      if (typetwo == MAP_BEACH)
      return FALSE;
      if (typetwo == MAP_PARK)
      return FALSE;
      if (typetwo == MAP_ROCKS)
      return FALSE;
      if (typetwo == MAP_DIRT)
      return FALSE;
      if (typetwo == MAP_FOREST)
      return FALSE;
    }

    if (typeone == MAP_WATER) {
      if (typetwo == MAP_SHALLOW)
      return FALSE;
      if (typetwo == MAP_DIRT)
      return FALSE;
      if (typetwo == MAP_PARK)
      return FALSE;
    }

    if (typeone == MAP_BEACH) {
      if (typetwo == MAP_SHALLOW)
      return FALSE;
      if (typetwo == MAP_FOREST)
      return FALSE;
      if (typetwo == MAP_PARK)
      return FALSE;
      if (typetwo == MAP_DIRT)
      return FALSE;
      if (typetwo == MAP_PARK)
      return FALSE;
    }

    if (typeone == MAP_FOREST) {
      if (typetwo == MAP_BEACH)
      return FALSE;
      if (typetwo == MAP_SHALLOW)
      return FALSE;
      if (typetwo == MAP_PARK)
      return FALSE;
      if (typetwo == MAP_ROCKS)
      return FALSE;
      if (typetwo == MAP_DIRT)
      return FALSE;
    }

    if (typeone == MAP_PARK) {
      if (typetwo == MAP_FOREST)
      return FALSE;
      if (typetwo == MAP_ROCKS)
      return FALSE;
      if (typetwo == MAP_WATER)
      return FALSE;
      if (typetwo == MAP_SHALLOW)
      return FALSE;
      if (typetwo == MAP_DIRT)
      return FALSE;
    }

    if (typeone == MAP_DIRT) {
      if (typetwo == MAP_PARK)
      return FALSE;
      if (typetwo == MAP_FOREST)
      return FALSE;
      if (typetwo == MAP_SHALLOW)
      return FALSE;
      if (typetwo == MAP_BEACH)
      return FALSE;
      if (typetwo == MAP_ROCKS)
      return FALSE;
      if (typetwo == MAP_WATER)
      return FALSE;
    }

    if (typeone == MAP_ROCKS) {
      if (typetwo == MAP_PARK)
      return FALSE;
      if (typetwo == MAP_FOREST)
      return FALSE;
      if (typetwo == MAP_SHALLOW)
      return FALSE;
      if (typetwo == MAP_DIRT)
      return FALSE;
    }

    return TRUE;
  }

#define ASCII_BLACKLINE 1
#define ASCII_BLURREDX 2
#define ASCII_BLURREDY 3
#define ASCII_NORMALCOLOR 4

  int ascii_treatment_mini(int i, int j, int xcoord, int ycoord) {
    if (i % 3 == 0 || j % 3 == 0) {
      if ((i % 3 == 0) && (ascii_deliniate(asciimap[xcoord][ycoord], asciimap[xcoord - 1][ycoord], xcoord, ycoord, ASCII_HORIZONTAL)) == TRUE)
      return ASCII_BLACKLINE;
      if ((j % 3 == 0) && (ascii_deliniate(asciimap[xcoord][ycoord], asciimap[xcoord][ycoord - 1], xcoord, ycoord, ASCII_VERTICAL)) == TRUE)
      return ASCII_BLACKLINE;
      if ((i % 3 == 0) && asciimap[xcoord][ycoord] != asciimap[xcoord - 1][ycoord])
      return ASCII_BLURREDX;
      if ((j % 3 == 0) && asciimap[xcoord][ycoord] != asciimap[xcoord][ycoord - 1])
      return ASCII_BLURREDY;
    }
    return ASCII_NORMALCOLOR;
  }

  int ascii_treatment(int i, int j, int xcoord, int ycoord) {
    if (i % 15 == 0 || j % 15 == 0 || i % 15 == 1 || j % 15 == 1) {
      if ((i % 15 == 0 || i % 15 == 1) && (ascii_deliniate(asciimap[xcoord][ycoord], asciimap[xcoord - 1][ycoord], xcoord, ycoord, ASCII_HORIZONTAL)) == TRUE)
      return ASCII_BLACKLINE;
      if ((j % 15 == 0 || j % 15 == 1) && (ascii_deliniate(asciimap[xcoord][ycoord], asciimap[xcoord][ycoord - 1], xcoord, ycoord, ASCII_VERTICAL)) == TRUE)
      return ASCII_BLACKLINE;
      if ((i % 15 == 0) && asciimap[xcoord][ycoord] != asciimap[xcoord - 1][ycoord])
      return ASCII_BLURREDX;
      if ((j % 15 == 0) && asciimap[xcoord][ycoord] != asciimap[xcoord][ycoord - 1])
      return ASCII_BLURREDY;
    }
    return ASCII_NORMALCOLOR;
  }

  int ascii_treatment_forest(int i, int j, int xcoord, int ycoord) {
    if (i % 10 == 0 || j % 10 == 0 || i % 10 == 1 || j % 10 == 1) {
      if ((i % 10 == 0 || i % 10 == 1) && (ascii_deliniate(asciimap[xcoord][ycoord], asciimap[xcoord - 1][ycoord], xcoord, ycoord, ASCII_HORIZONTAL)) == TRUE)
      return ASCII_BLACKLINE;
      if ((j % 10 == 0 || j % 10 == 1) && (ascii_deliniate(asciimap[xcoord][ycoord], asciimap[xcoord][ycoord - 1], xcoord, ycoord, ASCII_VERTICAL)) == TRUE)
      return ASCII_BLACKLINE;
      if ((i % 10 == 0) && asciimap[xcoord][ycoord] != asciimap[xcoord - 1][ycoord])
      return ASCII_BLURREDX;
      if ((j % 10 == 0) && asciimap[xcoord][ycoord] != asciimap[xcoord][ycoord - 1])
      return ASCII_BLURREDY;
    }
    return ASCII_NORMALCOLOR;
  }

  void trytownmap(void) {
    FILE *inFile;
    FILE *outFile;
    BmpHeader header;
    BmpImageInfo info;
    Rgb *palette;
    int i = 0;

    if (asciimap[0][0] == 0) {
      buildasciimap();
    }

    inFile = fopen(BMPFIVE, "rb");
    if (!inFile) {
      return;
    }

    outFile = fopen("../../../../var/www/html/maps/town.bmp", "w");
    if (!outFile) {
      return;
    }

    if (fread(&header, 1, sizeof(BmpHeader), inFile) != sizeof(BmpHeader)) {
      return;
    }
    fwrite(&header, 1, sizeof(BmpHeader), outFile);

    if (fread(&info, 1, sizeof(BmpImageInfo), inFile) != sizeof(BmpImageInfo)) {
      return;
    }
    fwrite(&info, 1, sizeof(BmpImageInfo), outFile);
    if (info.numColors > 0) {
      palette = (Rgb *)malloc(sizeof(Rgb) * info.numColors);
      if (fread(palette, sizeof(Rgb), info.numColors, inFile) !=
          (info.numColors * sizeof(Rgb))) {
        return; // error
      }
      fwrite(palette, sizeof(Rgb), info.numColors, outFile);
    }
    Rgb *pixel = (Rgb *)malloc(sizeof(Rgb));
    int read, j;
    for (j = 0; j < (int)(info.height); j++) {
      read = 0;
      for (i = 0; i < (int)(info.width); i++) {
        if (fread(pixel, 1, sizeof(Rgb), inFile) != sizeof(Rgb)) {
          // printf( "Error reading pixel!\n" );
          return;
        }

        int xcoord = i / 15;
        int ycoord = j / 15;
        xcoord += 57;
        ycoord += 58;

        if (xcoord >= 0 && xcoord < 150 && ycoord >= 0 && ycoord < 189) {
          if (pixel->blue + pixel->red + pixel->green < 500) {
            if (bluemap(asciimap[xcoord][ycoord]) +
                redmap(asciimap[xcoord][ycoord]) +
                greenmap(asciimap[xcoord][ycoord]) <
                10 && ascii_treatment(i, j, xcoord, ycoord) != ASCII_BLACKLINE) {
              pixel->blue = 255;
              pixel->red = 255;
              pixel->green = 255;
            }
            else {
              pixel->blue = 0;
              pixel->red = 0;
              pixel->green = 0;
            }
          }
          else if (ascii_treatment(i, j, xcoord, ycoord) == ASCII_BLACKLINE) {
            pixel->blue = 0;
            pixel->red = 0;
            pixel->green = 0;
          }
          else if (ascii_treatment(i, j, xcoord, ycoord) == ASCII_BLURREDX) {
            pixel->blue = (bluemap(asciimap[xcoord][ycoord]) +
            bluemap(asciimap[xcoord - 1][ycoord])) /
            2;
            pixel->red = (redmap(asciimap[xcoord][ycoord]) +
            redmap(asciimap[xcoord - 1][ycoord])) /
            2;
            pixel->green = (greenmap(asciimap[xcoord][ycoord]) +
            greenmap(asciimap[xcoord - 1][ycoord])) /
            2;
          }
          else if (ascii_treatment(i, j, xcoord, ycoord) == ASCII_BLURREDY) {
            pixel->blue = (bluemap(asciimap[xcoord][ycoord]) +
            bluemap(asciimap[xcoord][ycoord - 1])) /
            2;
            pixel->red = (redmap(asciimap[xcoord][ycoord]) +
            redmap(asciimap[xcoord][ycoord - 1])) /
            2;
            pixel->green = (greenmap(asciimap[xcoord][ycoord]) +
            greenmap(asciimap[xcoord][ycoord - 1])) /
            2;
          }
          else {
            pixel->blue = bluemap(asciimap[xcoord][ycoord]);
            pixel->red = redmap(asciimap[xcoord][ycoord]);
            pixel->green = greenmap(asciimap[xcoord][ycoord]);
          }
        }
        fwrite(pixel, 1, sizeof(Rgb), outFile);
        read += sizeof(Rgb);
      }
      if (read % 4 != 0) {
        read = 4 - (read % 4);
        fread(pixel, read, 1, inFile);
        fwrite(pixel, read, 1, outFile);
      }
    }

    fclose(inFile);
    fclose(outFile);
  }

  void tryforestmap(int type) {
    FILE *inFile;
    FILE *outFile;
    BmpHeader header;
    BmpImageInfo info;
    Rgb *palette;
    int i = 0;

    if (asciimap[0][0] == 0)
    buildasciimap();

    if (type == DIR_NORTH || type == DIR_SOUTH) {
      inFile = fopen("../data/whitehorizontal.bmp", "rb");
      if (!inFile) {
        return;
      }
    }
    else {
      inFile = fopen("../data/whitevertical.bmp", "rb");
      if (!inFile) {
        return;
      }
    }

    if (type == DIR_NORTH) {
      outFile = fopen("../../../../var/www/html/maps/northforest.bmp", "w");
      if (!outFile) {
        return;
      }
    }
    else if (type == DIR_SOUTH) {
      outFile = fopen("../../../../var/www/html/maps/southforest.bmp", "w");
      if (!outFile) {
        return;
      }
    }
    else {
      outFile = fopen("../../../../var/www/html/maps/westforest.bmp", "w");
      if (!outFile) {
        return;
      }
    }

    if (fread(&header, 1, sizeof(BmpHeader), inFile) != sizeof(BmpHeader)) {
      return;
    }
    fwrite(&header, 1, sizeof(BmpHeader), outFile);

    if (fread(&info, 1, sizeof(BmpImageInfo), inFile) != sizeof(BmpImageInfo)) {
      return;
    }
    fwrite(&info, 1, sizeof(BmpImageInfo), outFile);
    if (info.numColors > 0) {
      palette = (Rgb *)malloc(sizeof(Rgb) * info.numColors);
      if (fread(palette, sizeof(Rgb), info.numColors, inFile) !=
          (info.numColors * sizeof(Rgb))) {
        return; // error
      }
      fwrite(palette, sizeof(Rgb), info.numColors, outFile);
    }
    Rgb *pixel = (Rgb *)malloc(sizeof(Rgb));
    int read, j;
    for (j = 0; j < (int)(info.height); j++) {

      read = 0;
      for (i = 0; i < (int)(info.width); i++) {

        if (fread(pixel, 1, sizeof(Rgb), inFile) != sizeof(Rgb)) {
          // printf( "Error reading pixel!\n" );
          return;
        }

        int xcoord = i / 10;
        int ycoord = j / 10;

        if (type == DIR_NORTH)
        ycoord += 127;
        else if (type == DIR_WEST)
        ycoord += 58;

        if (xcoord >= 0 && xcoord < 150 && ycoord >= 0 && ycoord < 189) {
          if (pixel->blue + pixel->red + pixel->green < 500) {
            if (bluemap(asciimap[xcoord][ycoord]) +
                redmap(asciimap[xcoord][ycoord]) +
                greenmap(asciimap[xcoord][ycoord]) <
                10 && ascii_treatment_forest(i, j, xcoord, ycoord) != ASCII_BLACKLINE) {
              pixel->blue = 255;
              pixel->red = 255;
              pixel->green = 255;
            }
            else {
              pixel->blue = 0;
              pixel->red = 0;
              pixel->green = 0;
            }
          }
          else if (ascii_treatment_forest(i, j, xcoord, ycoord) ==
              ASCII_BLACKLINE) {
            pixel->blue = 0;
            pixel->red = 0;
            pixel->green = 0;
          }
          else if (ascii_treatment_forest(i, j, xcoord, ycoord) ==
              ASCII_BLURREDX) {
            pixel->blue = (bluemap(asciimap[xcoord][ycoord]) +
            bluemap(asciimap[xcoord - 1][ycoord])) /
            2;
            pixel->red = (redmap(asciimap[xcoord][ycoord]) +
            redmap(asciimap[xcoord - 1][ycoord])) /
            2;
            pixel->green = (greenmap(asciimap[xcoord][ycoord]) +
            greenmap(asciimap[xcoord - 1][ycoord])) /
            2;
          }
          else if (ascii_treatment_forest(i, j, xcoord, ycoord) ==
              ASCII_BLURREDY) {
            pixel->blue = (bluemap(asciimap[xcoord][ycoord]) +
            bluemap(asciimap[xcoord][ycoord - 1])) /
            2;
            pixel->red = (redmap(asciimap[xcoord][ycoord]) +
            redmap(asciimap[xcoord][ycoord - 1])) /
            2;
            pixel->green = (greenmap(asciimap[xcoord][ycoord]) +
            greenmap(asciimap[xcoord][ycoord - 1])) /
            2;
          }
          else {
            pixel->blue = bluemap(asciimap[xcoord][ycoord]);
            pixel->red = redmap(asciimap[xcoord][ycoord]);
            pixel->green = greenmap(asciimap[xcoord][ycoord]);
          }
        }
        fwrite(pixel, 1, sizeof(Rgb), outFile);
        read += sizeof(Rgb);
      }
      if (read % 4 != 0) {
        read = 4 - (read % 4);
        fread(pixel, read, 1, inFile);
        fwrite(pixel, read, 1, outFile);
      }
    }

    fclose(inFile);
    fclose(outFile);
  }

  void makemonstermap() {
    FILE *inFile;
    FILE *outFile;
    BmpHeader header;
    BmpImageInfo info;
    Rgb *palette;
    int i = 0;

    if (asciimap[0][0] == 0)
    buildasciimap();

    int monster_x[5];
    int monster_y[5];
    for (int i = 0; i < 5; i++) {
      monster_x[i] = -1000;
      monster_y[i] = -1000;
    }
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      CHAR_DATA *to;
      if (d->character != NULL && d->connected == CON_PLAYING) {
        to = d->character;
        if (IS_NPC(to) || to->in_room == NULL)
        continue;

        if (guestmonster(to)) {
          for (int i = 0; i < 5; i++) {
            if (monster_x[i] == -1000) {
              monster_x[i] = to->in_room->x;
              monster_y[i] = to->in_room->y;
              i = 6;
            }
          }
        }
      }
    }

    inFile = fopen(BMPTHREE, "rb");
    if (!inFile) {
      return;
    }

    outFile = fopen("../../../../var/www/html/report.bmp", "w");
    if (!outFile) {
      return;
    }

    if (fread(&header, 1, sizeof(BmpHeader), inFile) != sizeof(BmpHeader)) {
      return;
    }
    fwrite(&header, 1, sizeof(BmpHeader), outFile);

    if (fread(&info, 1, sizeof(BmpImageInfo), inFile) != sizeof(BmpImageInfo)) {
      return;
    }
    fwrite(&info, 1, sizeof(BmpImageInfo), outFile);
    if (info.numColors > 0) {
      palette = (Rgb *)malloc(sizeof(Rgb) * info.numColors);
      if (fread(palette, sizeof(Rgb), info.numColors, inFile) !=
          (info.numColors * sizeof(Rgb))) {
        return; // error
      }
      fwrite(palette, sizeof(Rgb), info.numColors, outFile);
    }
    Rgb *pixel = (Rgb *)malloc(sizeof(Rgb));
    int read, j;
    for (j = 0; j < (int)(info.height); j++) {
      read = 0;
      for (i = 0; i < (int)(info.width); i++) {

        if (fread(pixel, 1, sizeof(Rgb), inFile) != sizeof(Rgb)) {
          // printf( "Error reading pixel!\n" );
          return;
        }

        int xcoord = i / 3;
        int ycoord = j / 3;
        xcoord += 27;
        ycoord += 28;

        if (xcoord >= 0 && xcoord < 150 && ycoord >= 0 && ycoord < 189) {
          for (int z = 0; z < 5; z++) {
            if (monster_x[z] == -1000)
            continue;
            if (monster_x[z] + 58 == xcoord && monster_y[z] + 59 == ycoord) {
              pixel->blue = 0;
              pixel->red = 255;
              pixel->green = 0;
            }
            else if (ascii_treatment_mini(i, j, xcoord, ycoord) ==
                ASCII_BLACKLINE) {
              pixel->blue = 0;
              pixel->red = 0;
              pixel->green = 0;
            }
            else if (asciimap[xcoord][ycoord] == MAP_STREET || asciimap[xcoord][ycoord] == MAP_ALLEY) {
              pixel->blue = 0;
              pixel->red = 0;
              pixel->green = 0;
            }
            else if (ascii_treatment_mini(i, j, xcoord, ycoord) ==
                ASCII_BLURREDX || ascii_treatment_mini(i, j, xcoord, ycoord) ==
                ASCII_BLURREDY) {
              pixel->blue = 255;
              pixel->red = 255;
              pixel->green = 255;
            }
            else {
              pixel->blue = 255;
              pixel->red = 255;
              pixel->green = 255;
            }
          }
          for (int z = 0; z < 5; z++) {
            if (monster_x[z] == -1000)
            continue;

            if (monster_x[z] + 58 != xcoord || monster_y[z] + 59 != ycoord) {
              if (get_dist(monster_x[z] + 58, monster_y[z] + 59, xcoord, ycoord) <
                  2) {
                pixel->blue /= 2;
                pixel->red = UMIN(255, pixel->red * 2);
                pixel->green /= 2;
              }
            }
          }
        }
        fwrite(pixel, 1, sizeof(Rgb), outFile);
        read += sizeof(Rgb);
      }
      if (read % 4 != 0) {
        read = 4 - (read % 4);
        fread(pixel, read, 1, inFile);
        fwrite(pixel, read, 1, outFile);
      }
    }

    fclose(inFile);
    fclose(outFile);
  }

  void maketownmap(CHAR_DATA *ch) {
    FILE *inFile;
    FILE *outFile;
    BmpHeader header;
    BmpImageInfo info;
    Rgb *palette;
    int i = 0;

    if (asciimap[0][0] == 0)
    buildasciimap();

    inFile = fopen(BMPTHREE, "rb");
    if (!inFile) {
      return;
    }

    if (mapnumber == 1) {
      outFile = fopen("../../../../var/www/html/townone.bmp", "w");
      if (!outFile) {
        return;
      }
    }
    else if (mapnumber == 2) {
      outFile = fopen("../../../../var/www/html/towntwo.bmp", "w");
      if (!outFile) {
        return;
      }
    }
    else if (mapnumber == 3) {
      outFile = fopen("../../../../var/www/html/townthree.bmp", "w");
      if (!outFile) {
        return;
      }
    }
    else if (mapnumber == 4) {
      outFile = fopen("../../../../var/www/html/townfour.bmp", "w");
      if (!outFile) {
        return;
      }
    }
    else {
      outFile = fopen("../../../../var/www/html/townfive.bmp", "w");
      if (!outFile) {
        return;
      }
    }

    if (fread(&header, 1, sizeof(BmpHeader), inFile) != sizeof(BmpHeader)) {
      return;
    }
    fwrite(&header, 1, sizeof(BmpHeader), outFile);

    if (fread(&info, 1, sizeof(BmpImageInfo), inFile) != sizeof(BmpImageInfo)) {
      return;
    }
    fwrite(&info, 1, sizeof(BmpImageInfo), outFile);
    if (info.numColors > 0) {
      palette = (Rgb *)malloc(sizeof(Rgb) * info.numColors);
      if (fread(palette, sizeof(Rgb), info.numColors, inFile) !=
          (info.numColors * sizeof(Rgb))) {
        return; // error
      }
      fwrite(palette, sizeof(Rgb), info.numColors, outFile);
    }
    Rgb *pixel = (Rgb *)malloc(sizeof(Rgb));
    int read, j;
    for (j = 0; j < (int)(info.height); j++) {

      read = 0;
      for (i = 0; i < (int)(info.width); i++) {

        if (fread(pixel, 1, sizeof(Rgb), inFile) != sizeof(Rgb)) {
          // printf( "Error reading pixel!\n" );
          return;
        }

        int xcoord = i / 3;
        int ycoord = j / 3;
        xcoord += 27;
        ycoord += 28;

        if (xcoord >= 0 && xcoord < 150 && ycoord >= 0 && ycoord < 189) {
          if (ch->in_room->x + 58 == xcoord && ch->in_room->y + 59 == ycoord) {
            pixel->blue = 0;
            pixel->red = 255;
            pixel->green = 0;
          }
          else if (ascii_treatment_mini(i, j, xcoord, ycoord) ==
              ASCII_BLACKLINE) {
            pixel->blue = 0;
            pixel->red = 0;
            pixel->green = 0;
          }
          else if (asciimap[xcoord][ycoord] == MAP_STREET || asciimap[xcoord][ycoord] == MAP_ALLEY) {
            pixel->blue = 0;
            pixel->red = 0;
            pixel->green = 0;
          }
          else if (ascii_treatment_mini(i, j, xcoord, ycoord) ==
              ASCII_BLURREDX || ascii_treatment_mini(i, j, xcoord, ycoord) ==
              ASCII_BLURREDY) {
            pixel->blue = 255;
            pixel->red = 255;
            pixel->green = 255;
          }
          else {
            pixel->blue = 255;
            pixel->red = 255;
            pixel->green = 255;
          }

          if (ch->in_room->x + 58 != xcoord || ch->in_room->y + 59 != ycoord) {
            if (get_dist(ch->in_room->x + 58, ch->in_room->y + 59, xcoord, ycoord) < 2) {
              pixel->blue /= 2;
              pixel->red = UMIN(255, pixel->red * 2);
              pixel->green /= 2;
            }
          }
        }
        fwrite(pixel, 1, sizeof(Rgb), outFile);
        read += sizeof(Rgb);
      }
      if (read % 4 != 0) {
        read = 4 - (read % 4);
        fread(pixel, read, 1, inFile);
        fwrite(pixel, read, 1, outFile);
      }
    }

    fclose(inFile);
    fclose(outFile);

    if (mapnumber == 1)
    send_to_char("http://havenrpg.net/townone.html\n\r", ch);
    else if (mapnumber == 2)
    send_to_char("http://havenrpg.net/towntwo.html\n\r", ch);
    else if (mapnumber == 3)
    send_to_char("http://havenrpg.net/townthree.html\n\r", ch);
    else if (mapnumber == 4)
    send_to_char("http://havenrpg.net/townfour.html\n\r", ch);
    else
    send_to_char("http://havenrpg.net/townfive.html\n\r", ch);

    mapnumber++;
    if (mapnumber > 5)
    mapnumber = 1;
  }

  bool street_exists(ROOM_INDEX_DATA *room, int direction, ROOM_INDEX_DATA *destination) {
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    if ((pexit = room->exit[direction]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (in_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && (in_room->sector_type == SECT_STREET && in_room->y <= 65 && in_room->x >= 3 && in_room->y >= 0)) {
      if (destination != NULL && in_room->area->world != destination->area->world)
      return FALSE;
      if(institute_room(in_room) && !institute_room(destination))
      return FALSE;

      if(room->vnum == 2194 && destination->vnum != 9636 && (direction == DIR_NORTH || direction == DIR_NORTHWEST || direction == DIR_NORTHEAST))
      return FALSE;

      if(room->vnum == 17665 && destination->vnum != 17575 && (direction == DIR_NORTH || direction == DIR_NORTHWEST || direction == DIR_NORTHEAST))
      return FALSE;

      return TRUE;
    }

    return FALSE;
  }

  bool alley_exists(ROOM_INDEX_DATA *room, int direction, ROOM_INDEX_DATA *destination) {
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    if ((pexit = room->exit[direction]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (in_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU)) && (in_room->sector_type == SECT_STREET || in_room->sector_type == SECT_ALLEY)) {
      if (destination != NULL && in_room->area->world != destination->area->world)
      return FALSE;
      return TRUE;
    }

    return FALSE;
  }

  bool moving_backwards(int dir, int orig) {
    if (orig == -1 || dir == -1)
    return FALSE;
    if (dir == rev_dir[orig])
    return TRUE;
    if (dir == turn_dir[rev_dir[orig]])
    return TRUE;
    if (dir == aturn_dir[rev_dir[orig]])
    return TRUE;
    return FALSE;
  }

  int pref_direction(int dir1, int dir2, ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination) {
    int xdiff = get_roomx(destination) - get_roomx(in_room);
    int ydiff = get_roomy(in_room) - get_roomy(destination);
    if (xdiff > 0) {
      if (dir1 == DIR_SOUTHEAST || dir2 == DIR_SOUTHEAST)
      return DIR_SOUTHEAST;
      if (dir1 == DIR_NORTHEAST || dir2 == DIR_NORTHEAST)
      return DIR_NORTHEAST;
      if (dir1 == DIR_EAST || dir2 == DIR_EAST)
      return DIR_EAST;
    }
    if (xdiff < 0) {
      if (dir1 == DIR_SOUTHWEST || dir2 == DIR_SOUTHWEST)
      return DIR_SOUTHWEST;
      if (dir1 == DIR_NORTHWEST || dir2 == DIR_NORTHWEST)
      return DIR_NORTHWEST;
      if (dir1 == DIR_WEST || dir2 == DIR_WEST)
      return DIR_WEST;
    }
    if (ydiff > 0) {
      if (dir1 == DIR_SOUTHEAST || dir2 == DIR_SOUTHEAST)
      return DIR_SOUTHEAST;
      if (dir1 == DIR_SOUTHWEST || dir2 == DIR_SOUTHWEST)
      return DIR_SOUTHWEST;
      if (dir1 == DIR_SOUTH || dir2 == DIR_SOUTH)
      return DIR_SOUTH;
    }
    if (ydiff < 0) {
      if (dir1 == DIR_NORTHEAST || dir2 == DIR_NORTHEAST)
      return DIR_NORTHEAST;
      if (dir1 == DIR_NORTHWEST || dir2 == DIR_NORTHWEST)
      return DIR_NORTHWEST;
      if (dir1 == DIR_NORTH || dir2 == DIR_NORTH)
      return DIR_NORTH;
    }
    return dir1;
  }

  bool knows_gateway(CHAR_DATA *ch, int gateway) {
    for (int i = 0; i < 50; i++) {
      if (ch->pcdata->known_gateways[i] == gateway)
      return TRUE;
    }
    return FALSE;
  }

  bool bluffs_room(ROOM_INDEX_DATA *room)
  {
    if(room->x >= 32 && room->y >= 26 && room->y <=45)
    return TRUE;
    return FALSE;
  }

  bool bluffs_or_downtown_room(ROOM_INDEX_DATA *room)
  {
    if(room->x >= 32 && room->y >= 0 && room->y <=45)
    return TRUE;
    return FALSE;
  }



  int path_dir(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination, int moving, CHAR_DATA *ch) {
    if (in_room == destination)
    return -1;
    if (in_room->sector_type != SECT_STREET && in_room->sector_type != SECT_ALLEY && in_room->sector_type != SECT_PARKING)
    return -1;

    if (in_room->area->world != destination->area->world && ch != NULL && !IS_NPC(ch)) {
      if (in_room->area->world > WORLD_EARTH && in_room->area->world <= WORLD_HELL) {
        int mindist = 200;
        int minvnum = -1;
        for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
        it != gatewayVect.end(); ++it) {
          if ((*it)->valid == FALSE)
          continue;
          if ((*it)->world != in_room->area->world)
          continue;
          if ((*it)->worldroom == in_room->vnum && knows_gateway(ch, (*it)->havenroom))
          return rev_dir[(*it)->direction];

          if (!knows_gateway(ch, (*it)->havenroom))
          continue;
          ROOM_INDEX_DATA *oroom = get_room_index((*it)->worldroom);
          if (get_dist(in_room->x, in_room->y, oroom->x, oroom->y) < mindist && oroom->sector_type == SECT_STREET) {
            minvnum = oroom->vnum;
            mindist = get_dist(in_room->x, in_room->y, oroom->x, oroom->y);
          }
        }
        if (minvnum != -1)
        destination = get_room_index(minvnum);
      }
      else if (in_room->area->world == WORLD_EARTH) {
        if (destination->area->world > WORLD_EARTH && destination->area->world <= WORLD_HELL) {
          int mindist = 200;
          int minvnum = -1;
          for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
          it != gatewayVect.end(); ++it) {
            if ((*it)->valid == FALSE)
            continue;

            if ((*it)->world != destination->area->world)
            continue;
            if ((*it)->havenroom == in_room->vnum && knows_gateway(ch, (*it)->havenroom))
            return (*it)->direction;

            if (!knows_gateway(ch, (*it)->havenroom))
            continue;
            ROOM_INDEX_DATA *oroom = get_room_index((*it)->havenroom);
            if (get_dist(in_room->x, in_room->y, oroom->x, oroom->y) < mindist && oroom->sector_type == SECT_STREET) {
              minvnum = oroom->vnum;
              mindist = get_dist(in_room->x, in_room->y, oroom->x, oroom->y);
            }
          }
          if (minvnum != -1)
          destination = get_room_index(minvnum);
        }
      }
    }
    else {

      if(bluffs_room(in_room) == TRUE && bluffs_or_downtown_room(destination) == FALSE && in_room->vnum != 14570 && destination->vnum != 14570)
      destination = get_room_index(14570);
      if(bluffs_room(destination) == TRUE && bluffs_or_downtown_room(in_room) == FALSE && in_room->vnum != 14570 && destination->vnum != 14570)
      destination = get_room_index(14570);


      if (in_room->x >= 31 && destination->x < 31 && destination->x > 20 && destination->y < 26 && destination->y > 0)
      destination = get_room_index(3673);
      else if (in_room->x < 31 && destination->x >= 31 && in_room->x > 20 && in_room->y < 26 && in_room->y > 0)
      destination = get_room_index(3673);
      if (destination->x < 0 && in_room->x > 0 && destination->y < 30)
      destination = get_room_index(2281);
      if (destination->x < 0 && in_room->x > 0 && destination->y >= 30)
      destination = get_room_index(15282);
      if (destination->x < 20 && in_room->y <= 26 && in_room->y > 0 && destination->y <= 26)
      destination = get_room_index(1061);
      if (in_room->y < 37 && destination->y > 37 && in_room->x >= 0 && in_room->x <= 72 && destination->x >= 0 && destination->x <= 72)
      destination = get_room_index(1109);
      else if (in_room->y > 27 && destination->y < 27 && in_room->x >= 0 && in_room->x <= 72 && destination->x >= 0 && destination->x <= 72)
      destination = get_room_index(5854);
      if(!bluffs_room(in_room) && !bluffs_room(destination))
      {
        if (destination->y >= 0 && destination->y <= 26 && destination->x > 31 && in_room->x <= 31 && in_room->vnum != 3609)
        destination = get_room_index(3609);
        else if (in_room->y >= 0 && in_room->y <= 26 && in_room->x > 31 && (destination->x <= 31 || destination->y > 26 || destination->y < 0))
        destination = get_room_index(3609);

      }


      /*
if(in_room->y < 30 && in_room->y > 0 && in_room->x < 26 && (destination->y > 30 || destination->x >= 26)) destination =
get_room_index(1021); else if(in_room->y == 0 && in_room->x < 26 && destination->x >= 26) destination = get_room_index(1043); else
if(in_room->y > 43 && destination->y < 40) destination =
get_room_index(5931); else if(in_room->y < 40 && destination->y > 43)
destination = get_room_index(5931);
*/
    }
    int bdir =
    roomdirection(in_room->x, in_room->y, destination->x, destination->y);

    if (!str_cmp(ch->name, "Tyr"))
    {
      printf_to_char(ch, "Going To: %s(%d) ", destination->name, destination->vnum);
      printf_to_char(ch, "Heading: %s ", dir_name[bdir][0]);
    }
    if (street_exists(in_room, bdir, destination) && (moving == -1 || bdir != rev_dir[moving]))
    return bdir;
    if (alley_exists(in_room, bdir, destination) && (moving == -1 || bdir != rev_dir[moving]))
    return bdir;

    int cdir = turn_dir[bdir];
    int acdir = aturn_dir[bdir];
    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Clock: %s AntiClock: %s", dir_name[cdir][0], dir_name[acdir][0]);

    if (cdir == pref_direction(cdir, acdir, in_room, destination)) {
      if (street_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
      if (street_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
    }
    else {
      if (street_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
      if (street_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
    }

    cdir = turn_dir[cdir];
    acdir = aturn_dir[acdir];
    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Clock: %s AntiClock: %s", dir_name[cdir][0], dir_name[acdir][0]);

    if (cdir == pref_direction(cdir, acdir, in_room, destination)) {
      if (street_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
      if (street_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
    }
    else {
      if (street_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
      if (street_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
    }

    int alcdir = turn_dir[bdir];
    int alacdir = aturn_dir[bdir];

    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Alley Clock: %s AntiClock: %s", dir_name[cdir][0], dir_name[acdir][0]);

    if (alcdir == pref_direction(alcdir, alacdir, in_room, destination)) {
      if (alley_exists(in_room, alcdir, destination) && !moving_backwards(alcdir, moving))
      return alcdir;
      if (alley_exists(in_room, alacdir, destination) && !moving_backwards(alacdir, moving))
      return alacdir;
    }
    else {
      if (alley_exists(in_room, alacdir, destination) && !moving_backwards(alacdir, moving))
      return alacdir;
      if (alley_exists(in_room, alcdir, destination) && !moving_backwards(alcdir, moving))
      return alcdir;
    }

    cdir = turn_dir[cdir];
    acdir = aturn_dir[acdir];
    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Clock: %s AntiClock: %s", dir_name[cdir][0], dir_name[acdir][0]);

    if (cdir == pref_direction(cdir, acdir, in_room, destination)) {
      if (street_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
      if (street_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
    }
    else {
      if (street_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
      if (street_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
    }

    cdir = turn_dir[alcdir];
    acdir = aturn_dir[alacdir];

    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Alley Clock: %s AntiClock: %s", dir_name[cdir][0], dir_name[acdir][0]);

    if (cdir == pref_direction(cdir, acdir, in_room, destination)) {
      if (alley_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
      if (alley_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
    }
    else {
      if (alley_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
      if (alley_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
    }

    cdir = turn_dir[cdir];
    acdir = aturn_dir[acdir];

    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Alley Clock: %s AntiClock: %s", dir_name[cdir][0], dir_name[acdir][0]);

    if (cdir == pref_direction(cdir, acdir, in_room, destination)) {
      if (alley_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
      if (alley_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
    }
    else {
      if (alley_exists(in_room, acdir, destination) && !moving_backwards(acdir, moving))
      return acdir;
      if (alley_exists(in_room, cdir, destination) && !moving_backwards(cdir, moving))
      return cdir;
    }

    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Backwards: %s", dir_name[rev_dir[bdir]][0]);

    if (street_exists(in_room, rev_dir[bdir], destination) && !moving_backwards(rev_dir[bdir], moving))
    return rev_dir[bdir];
    if (alley_exists(in_room, rev_dir[bdir], destination) && !moving_backwards(rev_dir[bdir], moving))
    return rev_dir[bdir];

    bool altfound = FALSE;
    for (int i = 0; i < 10; i++) {
      if (i == DIR_UP || i == DIR_DOWN)
      continue;
      if (i == rev_dir[moving])
      continue;
      if (i == turn_dir[rev_dir[moving]])
      continue;
      if (i == aturn_dir[rev_dir[moving]])
      continue;
      if (alley_exists(in_room, i, destination))
      altfound = TRUE;
      if (street_exists(in_room, i, destination))
      altfound = TRUE;
    }
    if (!str_cmp(ch->name, "Tyr"))
    printf_to_char(ch, "Backwards: %s", dir_name[rev_dir[moving]][0]);

    if (altfound == FALSE && (street_exists(in_room, rev_dir[moving], destination) || alley_exists(in_room, rev_dir[moving], destination)))
    return rev_dir[moving];

    return -1;
  }

  /*
int path_dir_alley(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination)
{
if(in_room == destination)
return -1;
if(in_room->sector_type != SECT_STREET && in_room->sector_type != SECT_ALLEY
&& in_room->sector_type != SECT_PARKING) return -1;

if(in_room->y < 30 && in_room->y > 0 && in_room->x < 26 && (destination->y >
30 || destination->x >= 26)) destination = get_room_index(1021); else
if(in_room->y == 0 && in_room->x < 26 && destination->x >= 26) destination =
get_room_index(1051); else if(in_room->y > 43 && destination->y < 40)
destination = get_room_index(5931);
else if(in_room->y < 40 && destination->y > 43)
destination = get_room_index(5931);


int xdiff = get_roomx(destination) - get_roomx(in_room);
int ydiff = get_roomy(in_room) - get_roomy(destination);


if(UMAX(xdiff, xdiff*-1) > UMAX(ydiff, ydiff*-1))
{
if(xdiff > 0)
{
if(ydiff > 0 && alley_exists(in_room, DIR_SOUTHEAST))
return DIR_SOUTHEAST;
else if(ydiff < 0 && alley_exists(in_room, DIR_NORTHEAST))
return DIR_NORTHEAST;
else if(alley_exists(in_room, DIR_EAST))
return DIR_EAST;
}
if(xdiff < 0)
{
if(ydiff > 0 && alley_exists(in_room, DIR_SOUTHWEST))
return DIR_SOUTHWEST;
else if(ydiff < 0 && alley_exists(in_room, DIR_NORTHWEST))
return DIR_NORTHWEST;
else if(alley_exists(in_room, DIR_WEST))
return DIR_WEST;
}
}

if(ydiff > 0)
{
if(xdiff > 0 && alley_exists(in_room, DIR_SOUTHEAST))
return DIR_SOUTHEAST;
else if( xdiff < 0 && alley_exists(in_room, DIR_SOUTHWEST))
return DIR_SOUTHWEST;
else if(alley_exists(in_room, DIR_SOUTH))
return DIR_SOUTH;
else if(alley_exists(in_room, DIR_SOUTHEAST))
return DIR_SOUTHEAST;
else if(alley_exists(in_room, DIR_SOUTHWEST))
return DIR_SOUTHWEST;
}
if(ydiff < 0)
{
if(xdiff > 0 && alley_exists(in_room, DIR_NORTHEAST))
return DIR_NORTHEAST;
else if( xdiff < 0 && alley_exists(in_room, DIR_NORTHWEST))
return DIR_NORTHWEST;
else if(alley_exists(in_room, DIR_NORTH))
return DIR_NORTH;
else if(alley_exists(in_room, DIR_NORTHEAST))
return DIR_NORTHEAST;
else if(alley_exists(in_room, DIR_NORTHWEST))
return DIR_NORTHWEST;
}

if(xdiff > 0)
{
if(ydiff > 0 && alley_exists(in_room, DIR_SOUTHEAST))
return DIR_SOUTHEAST;
else if(ydiff < 0 && alley_exists(in_room, DIR_NORTHEAST))
return DIR_NORTHEAST;
else if(alley_exists(in_room, DIR_EAST))
return DIR_EAST;
}
if(xdiff < 0)
{
if(ydiff > 0 && alley_exists(in_room, DIR_SOUTHWEST))
return DIR_SOUTHWEST;
else if(ydiff < 0 && alley_exists(in_room, DIR_NORTHWEST))
return DIR_NORTHWEST;
else if(alley_exists(in_room, DIR_WEST))
return DIR_WEST;
}
return -1;
}

int path_dir(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination)
{
int val = path_dir_street(in_room, destination);
if(val == -1)
return path_dir_alley(in_room, destination);
else
return val;
}
*/

  int street_distance(ROOM_INDEX_DATA *orig, ROOM_INDEX_DATA *desti, CHAR_DATA *ch) {
    int direction = -1;
    EXIT_DATA *pexit;
    if (orig == desti)
    return 0;

    if (orig == NULL || desti == NULL)
    return 0;

    if (path_dir(orig, desti, -1, ch) == -1)
    return 0;

    int distance = 0;
    for (; orig->vnum != desti->vnum; distance++) {
      direction = path_dir(orig, desti, direction, ch);
      if (direction == -1)
      return distance;
      pexit = orig->exit[direction];
      orig = pexit->u1.to_room;
      if (distance > 200)
      return 200;
    }
    return distance;
  }

  _DOFUN(do_sdist) {
    char arg1[MSL];
    int desti;
    ROOM_INDEX_DATA *room;

    argument = one_argument_nouncap(argument, arg1);

    desti = atoi(arg1);

    if (desti < 0 || desti > 100000) {
      send_to_char("walk (number)\n\r", ch);
      return;
    }

    if (desti <= MAX_TAXIS) {
      room = get_room_index(taxi_table[desti - 1].vnum);
    }
    else {
      return;
    }
    printf_to_char(ch, "%d\n\r", street_distance(ch->in_room, room, ch));
  }

  int roomdirection(int startx, int starty, int finx, int finy) {
    int diffx = finx - startx;
    int diffy = starty - finy;

    if (UMAX(diffx, diffx * -1) > (UMAX(diffy, diffy * -1) * 2)) {
      if (diffx > 0)
      return DIR_EAST;
      else
      return DIR_WEST;
    }
    if (UMAX(diffy, diffy * -1) > (UMAX(diffx, diffx * -1) * 2)) {
      if (diffy > 0)
      return DIR_SOUTH;
      else
      return DIR_NORTH;
    }

    if (diffx > 0) {
      if (diffy > 0)
      return DIR_SOUTHEAST;
      else
      return DIR_NORTHEAST;
    }
    else {
      if (diffy > 0)
      return DIR_SOUTHWEST;
      else
      return DIR_NORTHWEST;
    }
  }

  int roomdirection_3d(int startx, int starty, int startz, int finx, int finy, int finz) {
    int diffx = finx - startx;
    int diffy = starty - finy;
    int diffz = startz - finz;
    if (UMAX(diffz, diffz * -1) >
        UMAX(UMAX(diffy, diffy * -1), UMAX(diffx, diffx * -1))) {
      if (diffz > 0)
      return DIR_DOWN;
      else
      return DIR_UP;
    }
    if (UMAX(diffx, diffx * -1) > (UMAX(diffy, diffy * -1) * 2)) {
      if (diffx > 0)
      return DIR_EAST;
      else
      return DIR_WEST;
    }
    if (UMAX(diffy, diffy * -1) > (UMAX(diffx, diffx * -1) * 2)) {
      if (diffy > 0)
      return DIR_SOUTH;
      else
      return DIR_NORTH;
    }

    if (diffx > 0) {
      if (diffy > 0)
      return DIR_SOUTHEAST;
      else
      return DIR_NORTHEAST;
    }
    else {
      if (diffy > 0)
      return DIR_SOUTHWEST;
      else
      return DIR_NORTHWEST;
    }
  }

  bool vision_exists(ROOM_INDEX_DATA *room, int direction) {
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    if ((pexit = room->exit[direction]) != NULL && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (in_room = pexit->u1.to_room) != NULL && (pexit->wall != WALL_GLASS || !IS_SET(pexit->exit_info, EX_CURTAINS)) && (!IS_SET(pexit->exit_info, EX_CLOSED) || IS_SET(pexit->exit_info, EX_SEETHRU))) {
      return TRUE;
    }
    return FALSE;
  }

  int vision_path(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination) {

    if (in_room == destination)
    return -1;

    int xdiff = get_roomx(destination) - get_roomx(in_room);
    int ydiff = get_roomy(in_room) - get_roomy(destination);
    int zdiff = get_roomz(in_room) - get_roomz(destination);

    if (zdiff > 0 && vision_exists(in_room, DIR_DOWN) && zdiff * 5 > UMAX(UMAX(xdiff, xdiff * -1), UMAX(ydiff, ydiff * -1)))
    return DIR_DOWN;
    if (zdiff < 0 && vision_exists(in_room, DIR_UP) && zdiff * -5 > UMAX(UMAX(xdiff, xdiff * -1), UMAX(ydiff, ydiff * -1)))
    return DIR_UP;

    int dir =
    roomdirection(in_room->x, in_room->y, destination->x, destination->y);
    {
      if (vision_exists(in_room, dir))
      return dir;
    }
    /*

if(UMAX(xdiff, xdiff*-1) > UMAX(ydiff, ydiff*-1))
{
if(xdiff > 0)
{
if(ydiff > 0 && vision_exists(in_room, DIR_SOUTHEAST))
return DIR_SOUTHEAST;
else if(ydiff < 0 && vision_exists(in_room, DIR_NORTHEAST))
return DIR_NORTHEAST;
else if(vision_exists(in_room, DIR_EAST))
return DIR_EAST;
return -1;
}
if(xdiff < 0)
{
if(ydiff > 0 && vision_exists(in_room, DIR_SOUTHWEST))
return DIR_SOUTHWEST;
else if(ydiff < 0 && vision_exists(in_room, DIR_NORTHWEST))
return DIR_NORTHWEST;
else if(vision_exists(in_room, DIR_WEST))
return DIR_WEST;
return -1;
}
}

if(ydiff > 0)
{
if(xdiff > 0 && vision_exists(in_room, DIR_SOUTHEAST))
return DIR_SOUTHEAST;
else if( xdiff < 0 && vision_exists(in_room, DIR_SOUTHWEST))
return DIR_SOUTHWEST;
else if(vision_exists(in_room, DIR_SOUTH))
return DIR_SOUTH;
return -1;
}
if(ydiff < 0)
{
if(xdiff > 0 && vision_exists(in_room, DIR_NORTHEAST))
return DIR_NORTHEAST;
else if( xdiff < 0 && vision_exists(in_room, DIR_NORTHWEST))
return DIR_NORTHWEST;
else if(vision_exists(in_room, DIR_NORTH))
return DIR_NORTH;
return -1;
}
*/
    if (zdiff > 0 && vision_exists(in_room, DIR_DOWN))
    return DIR_DOWN;
    if (zdiff < 0 && vision_exists(in_room, DIR_UP))
    return DIR_UP;

    return -1;
  }

  bool exit_exists(ROOM_INDEX_DATA *room, int direction) {
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    if ((pexit = room->exit[direction]) != NULL && (in_room = pexit->u1.to_room) != NULL) {
      return TRUE;
    }
    return FALSE;
  }

  int sound_path_closed(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination) {
    if (in_room == destination)
    return -1;

    int xdiff = get_roomx(destination) - get_roomx(in_room);
    int ydiff = get_roomy(in_room) - get_roomy(destination);
    int zdiff = get_roomz(in_room) - get_roomz(destination);

    if (zdiff > 0 && exit_exists(in_room, DIR_DOWN) && zdiff * 5 > UMAX(UMAX(xdiff, xdiff * -1), UMAX(ydiff, ydiff * -1)))
    return DIR_DOWN;
    if (zdiff < 0 && exit_exists(in_room, DIR_UP) && zdiff * -15 > UMAX(UMAX(xdiff, xdiff * -1), UMAX(ydiff, ydiff * -1)))
    return DIR_UP;

    int dir =
    roomdirection(in_room->x, in_room->y, destination->x, destination->y);
    {
      if (exit_exists(in_room, dir))
      return dir;
    }
    if (zdiff > 0 && exit_exists(in_room, DIR_DOWN))
    return DIR_DOWN;
    if (zdiff < 0 && exit_exists(in_room, DIR_UP))
    return DIR_UP;
    return -1;
  }

  int sound_resistance(EXIT_DATA *pexit, ROOM_INDEX_DATA *orig, ROOM_INDEX_DATA *desti) {
    int roomone = 10;
    int roomtwo = 10;
    if (orig->vnum == 16289 || desti->vnum == 16289)
    return 0;
    if (!IS_SET(orig->room_flags, ROOM_INDOORS) && (is_raining(orig) || is_hailing(orig) || is_snowing(orig)))
    roomone += 10;
    if (public_room(orig))
    roomone += 5;
    else if (IS_SET(orig->room_flags, ROOM_INDOORS))
    roomone -= 5;

    if (!IS_SET(desti->room_flags, ROOM_INDOORS) && (is_raining(desti) || is_hailing(desti) || is_snowing(desti)))
    roomtwo += 10;
    if (public_room(desti))
    roomtwo += 5;
    else if (IS_SET(desti->room_flags, ROOM_INDOORS))
    roomtwo -= 5;

    int resist = roomone + roomtwo;
    resist /= 2;
    int wallresist = 0;
    if (pexit->wallcondition != WALLCOND_HOLE) {
      if (pexit->wall == WALL_GLASS)
      wallresist = 1;
      else if (pexit->wall == WALL_BRICK)
      wallresist = 15;
    }
    if (wallresist <= 0 && IS_SET(pexit->exit_info, EX_CLOSED))
    wallresist = 1;

    wallresist =
    wallresist * (4 + get_toughness(orig) + get_toughness(desti)) / 4;

    resist += wallresist;
    return resist;
  }

  int closed_sound_distance(CHAR_DATA *ch, ROOM_INDEX_DATA *orig, ROOM_INDEX_DATA *desti) {
    int direction = 0;
    EXIT_DATA *pexit;
    if (orig == desti)
    return TRUE;

    if (orig == NULL || desti == NULL)
    return FALSE;

    int distance = 0;

    for (; orig->vnum != desti->vnum;) {
      direction = sound_path_closed(orig, desti);
      if (direction == -1)
      return -1;
      pexit = orig->exit[direction];
      distance += sound_resistance(pexit, orig, pexit->u1.to_room);
      orig = pexit->u1.to_room;

      if (distance > 1000)
      return -1;
    }
    return distance;
  }

  bool open_sound(ROOM_INDEX_DATA *room, int direction) {
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    if ((pexit = room->exit[direction]) != NULL && (pexit->wall < WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && (in_room = pexit->u1.to_room) != NULL && (!IS_SET(pexit->exit_info, EX_CLOSED))) {
      return TRUE;
    }
    return FALSE;
  }

  int sound_path_open(ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *destination) {

    if (in_room == destination)
    return -1;

    int xdiff = get_roomx(destination) - get_roomx(in_room);
    int ydiff = get_roomy(in_room) - get_roomy(destination);
    int zdiff = get_roomz(in_room) - get_roomz(destination);

    if (zdiff > 0 && open_sound(in_room, DIR_DOWN) && zdiff * 5 > UMAX(UMAX(xdiff, xdiff * -1), UMAX(ydiff, ydiff * -1)))
    return DIR_DOWN;
    if (zdiff < 0 && open_sound(in_room, DIR_UP) && zdiff * -15 > UMAX(UMAX(xdiff, xdiff * -1), UMAX(ydiff, ydiff * -1)))
    return DIR_UP;

    if (UMAX(xdiff, xdiff * -1) > UMAX(ydiff, ydiff * -1)) {
      if (xdiff > 0) {
        if (ydiff > 0 && open_sound(in_room, DIR_SOUTHEAST))
        return DIR_SOUTHEAST;
        else if (ydiff < 0 && open_sound(in_room, DIR_NORTHEAST))
        return DIR_NORTHEAST;
        else if (open_sound(in_room, DIR_EAST))
        return DIR_EAST;
      }
      if (xdiff < 0) {
        if (ydiff > 0 && open_sound(in_room, DIR_SOUTHWEST))
        return DIR_SOUTHWEST;
        else if (ydiff < 0 && open_sound(in_room, DIR_NORTHWEST))
        return DIR_NORTHWEST;
        else if (open_sound(in_room, DIR_WEST))
        return DIR_WEST;
      }
    }

    if (ydiff > 0) {
      if (xdiff > 0 && open_sound(in_room, DIR_SOUTHEAST))
      return DIR_SOUTHEAST;
      else if (xdiff < 0 && open_sound(in_room, DIR_SOUTHWEST))
      return DIR_SOUTHWEST;
      else if (open_sound(in_room, DIR_SOUTH))
      return DIR_SOUTH;
    }
    if (ydiff < 0) {
      if (xdiff > 0 && open_sound(in_room, DIR_NORTHEAST))
      return DIR_NORTHEAST;
      else if (xdiff < 0 && open_sound(in_room, DIR_NORTHWEST))
      return DIR_NORTHWEST;
      else if (open_sound(in_room, DIR_NORTH))
      return DIR_NORTH;
    }

    if (zdiff > 0 && open_sound(in_room, DIR_DOWN))
    return DIR_DOWN;
    if (zdiff < 0 && open_sound(in_room, DIR_UP))
    return DIR_UP;

    return -1;
  }

  int open_sound_distance(CHAR_DATA *ch, ROOM_INDEX_DATA *orig, ROOM_INDEX_DATA *desti) {
    int direction = 0;
    EXIT_DATA *pexit;
    if (orig == desti)
    return TRUE;

    if (orig == NULL || desti == NULL)
    return FALSE;

    int distance = 0;
    if (!IS_SET(orig->room_flags, ROOM_INDOORS) && (is_raining(orig) || is_hailing(orig) || is_snowing(orig)))
    distance += 10;
    if (public_room(orig))
    distance += 5;
    else if (IS_SET(orig->room_flags, ROOM_INDOORS))
    distance -= 5;

    for (; orig->vnum != desti->vnum;) {
      direction = sound_path_open(orig, desti);
      if (direction == -1)
      return -1;
      pexit = orig->exit[direction];
      orig = pexit->u1.to_room;
      distance += 10;
      if (!IS_SET(orig->room_flags, ROOM_INDOORS) && (is_raining(orig) || is_hailing(orig) || is_snowing(orig)))
      distance += 10;
      if (public_room(orig))
      distance += 5;
      else if (IS_SET(orig->room_flags, ROOM_INDOORS))
      distance -= 5;

      if (distance > 1000)
      return -1;
    }
    return distance;
  }

  bool can_hear(CHAR_DATA *ch, CHAR_DATA *victim, int volume) {
    //    bool value = TRUE;

    if(!IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) && victim->in_room != NULL && ch != victim && newbie_school(victim->in_room))
    return FALSE;


    if ((is_gm(ch) || higher_power(ch)) && IS_FLAG(ch->act, PLR_SPYING) && can_spy(ch, victim))
    return TRUE;

    if (is_gm(ch) && ch->in_room == victim->in_room)
    return TRUE;

    if (is_gm(victim) && ch->in_room == victim->in_room)
    return TRUE;

    if(ch->possessing == victim || victim->possessing == ch)
    return TRUE;

    if ((is_gm(ch) || is_gm(victim)) && battleground(ch->in_room) && battleground(victim->in_room))
    return TRUE;

    if (!same_player(ch, victim) && victim->pcdata->account != NULL && IS_FLAG(victim->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    return FALSE;

    if (!same_player(ch, victim) && ch->pcdata->account != NULL && IS_FLAG(ch->pcdata->account->flags, ACCOUNT_SHADOWBAN))
    return FALSE;

    if (volume != VOLUME_QUIET) {
      if (ch->in_room->vnum == 16290 && victim->in_room->vnum == 16289)
      return TRUE;
      if (ch->in_room->vnum == 16289 && victim->in_room->vnum == 16290)
      return TRUE;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim)) {
      if (ch->pcdata->patrol_timer > 0 && (ch->pcdata->patrol_status == PATROL_LEADING_ASSAULT || ch->pcdata->patrol_status == PATROL_UNDER_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_ASSAULT || ch->pcdata->patrol_status == PATROL_ASSISTING_UNDER_ASSAULT)) {
        if (ch->pcdata->patrol_target != NULL && ch->pcdata->patrol_target->in_room != NULL && ch->pcdata->patrol_target->in_room == victim->in_room)
        return TRUE;
      }
      if (!str_cmp(ch->name, victim->pcdata->haunter) && victim->pcdata->haunt_timer > 0 && ch->pcdata->watching > 0)
      return TRUE;
    }

    if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
    return FALSE;

    //    if(is_ghost(victim) && (!is_possessing(victim) || victim->possessing !=
    //    ch) && !is_manifesting(victim) && ch != victim && get_skill(ch, //    SKILL_CLAIRAUDIENCE) < 1)
    //	value= FALSE;

    if (is_ghost(victim))
    volume = VOLUME_QUIET;

    if (victim->pcdata->patrol_status >= PATROL_ATTACKSEARCHING && victim->pcdata->patrol_status <= PATROL_WAGINGWAR) {
      if (get_dist(ch->in_room->x, ch->in_room->y, victim->in_room->x, victim->in_room->y) < 6 && ch->pcdata->patrol_status >= PATROL_ATTACKSEARCHING && ch->pcdata->patrol_status <= PATROL_WAGINGWAR)
      return TRUE;
    }
    if (is_dreaming(ch)) {
      if (ch == victim)
      return TRUE;
      if (is_dreaming(victim)) {
        if (ch->pcdata->dream_room > 0 && ch->pcdata->dream_room == victim->pcdata->dream_room)
        return TRUE;
        if (ch->pcdata->dream_room == 0 && ch->pcdata->dream_link == victim)
        return TRUE;
      }
      return FALSE;
    }

    if (is_deaf(ch))
    return FALSE;
    //    if(IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(victim->act, PLR_SHROUD))
    //	value = FALSE;
    //    if(IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(victim->act, //    PLR_DEEPSHROUD))
    //       value = FALSE;
    //    if(is_mute(victim))
    //	value = FALSE;

    if (ch == victim)
    return TRUE;

    if (in_world(ch) != in_world(victim))
    return FALSE;

    if (in_haven(ch->in_room) != in_haven(victim->in_room))
    return FALSE;

    if (IS_FLAG(victim->act, PLR_SHROUD) && !IS_FLAG(ch->act, PLR_SHROUD))
    return FALSE;

    if (volume == VOLUME_QUIET) {
      if (get_skill(victim, SKILL_ACUTEHEAR) > 0) {
        if (ch->in_room == victim->in_room && !crowded_room(victim->in_room))
        return TRUE;
      }
      if (ch->in_room == victim->in_room && same_place(ch, victim))
      return TRUE;
      //	value= FALSE;
    }

    if (volume == VOLUME_NORMAL) {
      if (ch->in_room == victim->in_room)
      return TRUE;

      if (!crowded_room(victim->in_room)) {
        int sdist = UMIN(open_sound_distance(ch, ch->in_room, victim->in_room), closed_sound_distance(ch, ch->in_room, victim->in_room));

        if (sdist < 20 && sdist != -1)
        return TRUE;

        if (get_skill(ch, SKILL_ACUTEHEAR) > 0 && sdist < 30 && sdist != -1)
        return TRUE;
      }
    }
    if (volume == VOLUME_LOUD) {
      if (ch->in_room == victim->in_room)
      return TRUE;

      int sdist = UMIN(open_sound_distance(ch, ch->in_room, victim->in_room), closed_sound_distance(ch, ch->in_room, victim->in_room));

      if (sdist < 30 && sdist != -1)
      return TRUE;

      if (get_skill(ch, SKILL_ACUTEHEAR) > 0 && sdist < 45 && sdist != -1)
      return TRUE;
    }
    if (volume == VOLUME_YELLING) {
      if (ch->in_room == victim->in_room)
      return TRUE;

      int sdist = UMIN(open_sound_distance(ch, ch->in_room, victim->in_room), closed_sound_distance(ch, ch->in_room, victim->in_room));

      if (sdist < 60 && sdist != -1)
      return TRUE;

      if (get_skill(ch, SKILL_ACUTEHEAR) > 0 && sdist < 90 && sdist != -1)
      return TRUE;
    }
    if (volume == VOLUME_SCREAMING) {
      if (ch->in_room == victim->in_room)
      return TRUE;

      int sdist = UMIN(open_sound_distance(ch, ch->in_room, victim->in_room), closed_sound_distance(ch, ch->in_room, victim->in_room));

      if (sdist < 80 && sdist != -1)
      return TRUE;

      if (get_skill(ch, SKILL_ACUTEHEAR) > 0 && sdist < 110 && sdist != -1)
      return TRUE;
    }
    if (volume == VOLUME_HISCREAMING) {
      if (ch->in_room == victim->in_room)
      return TRUE;

      int sdist = UMIN(open_sound_distance(ch, ch->in_room, victim->in_room), closed_sound_distance(ch, ch->in_room, victim->in_room));

      if (sdist < 100 && sdist != -1)
      return TRUE;

      if (get_skill(ch, SKILL_ACUTEHEAR) > 0 && sdist < 130 && sdist != -1)
      return TRUE;
    }

    if (ch->pcdata->brandstatus >= 1) {
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;
        CHAR_DATA *to;
        if (d->character != NULL && d->connected == CON_PLAYING) {
          to = d->character;
          if (IS_NPC(to) || to->in_room == NULL)
          continue;
          if (str_cmp(to->pcdata->brander, ch->name))
          continue;
          if (!str_cmp(ch->pcdata->brander, to->name))
          continue;
          if (is_helpless(to))
          continue;
          if (is_prisoner(to))
          continue;
          if (prop_from_room(to->in_room) != NULL && prop_from_room(to->in_room)->warded > 50)
          continue;
          if (in_fight(to))
          continue;

          if (can_hear(to, victim, volume) == TRUE)
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  int visibility_impair(CHAR_DATA *ch, ROOM_INDEX_DATA *room) {
    int impair = 100;
    if (room->sector_type == SECT_STREET)
    impair = 200;
    if (room->sector_type == SECT_ALLEY)
    impair = 250;
    if (room->sector_type == SECT_BEACH || room->sector_type == SECT_PARK || room->sector_type == SECT_SHALLOW)
    impair = 150;
    if (room->sector_type == SECT_WATER)
    impair = 250;
    if (room->sector_type == SECT_AIR)
    impair = 50;
    if (room->sector_type == SECT_ATMOSPHERE)
    impair = 50;
    if (room->sector_type == SECT_FOREST) {
      if (room->area->vnum == HAVEN_TOWN_VNUM)
      impair = 1000;
      else
      impair = 450;
    }
    if (room->sector_type == SECT_UNDERWATER) {
      if (IS_NPC(ch) || is_swimmer(ch))
      impair = 500;
      else
      impair = 1500;
    }
    if (mist_level(room) == 1)
    impair = impair * 11 / 10;
    if (mist_level(room) == 2 && room->sector_type == SECT_FOREST)
    impair = impair * 12 / 10;
    else if (mist_level(room) == 2)
    impair = impair * 18 / 10;
    if (mist_level(room) == 3)
    impair *= 3;

    if (in_fight(ch))
    impair = UMIN(impair, 300);

    if (!IS_SET(room->room_flags, ROOM_INDOORS) && room->sector_type != SECT_UNDERWATER && get_skill(ch, SKILL_STORMCALLING) < 1) {
      if (is_raining(room))
      impair = UMAX(300, impair + 50);
      else if (is_hailing(room) || is_snowing(room))
      impair = UMAX(350, impair + 100);
    }

    return impair;
  }

  int lineofsight(CHAR_DATA *ch, ROOM_INDEX_DATA *orig, ROOM_INDEX_DATA *desti, int range) {
    int direction = 0;
    EXIT_DATA *pexit;
    if (orig == desti)
    return DISTANCE_NEAR;

    if (orig == NULL || desti == NULL)
    return -1;

    bool directions[10] = {FALSE};
    for (int i = 0; i < 10; i++)
    directions[i] = FALSE;

    int distance = visibility_impair(ch, orig);
    for (; orig->vnum != desti->vnum;) {
      direction = vision_path(orig, desti);
      if (direction == -1)
      return -1;
      directions[direction] = TRUE;
      if (directions[rev_dir[direction]] == TRUE)
      return -1;
      pexit = orig->exit[direction];
      orig = pexit->u1.to_room;
      distance += visibility_impair(ch, orig);
      if (distance > range * 100)
      return -1;
    }
    if (distance <= range * 100 / 3)
    return DISTANCE_NEAR;
    else if (distance <= range * 100 * 2 / 3)
    return DISTANCE_MEDIUM;

    return DISTANCE_FAR;
  }

  bool invisioncone(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (IS_NPC(ch))
    return TRUE;

    if (desti == NULL) // quick catch to prevent nasty crash related to
    // lighthouse(?) - Discordance
    return FALSE;

    if (ch->in_room->x == desti->x && ch->in_room->y == desti->y) {
      if (ch->in_room->z > desti->z && ch->facing != DIR_DOWN)
      return FALSE;
      if (ch->in_room->z < desti->z && ch->facing != DIR_UP)
      return FALSE;
      return TRUE;
    }
    int dir = roomdirection(ch->in_room->x, ch->in_room->y, desti->x, desti->y);

    if (ch->facing == dir)
    return TRUE;
    int facing = ch->facing;
    facing = turn_dir[facing]; // NE
    if (facing == dir)
    return TRUE;
    facing = turn_dir[facing]; // E
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // SE
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // S
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // SW
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // W
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // NW
    if (facing == dir)
    return TRUE;

    return FALSE;
  }

  bool invisioncone_character(CHAR_DATA *ch, CHAR_DATA *victim) {
    if (IS_NPC(ch))
    return TRUE;
    if (ch->in_room->x == victim->in_room->x && ch->in_room->y == victim->in_room->y) {
      if (ch->in_room->z > victim->in_room->z && ch->facing != DIR_DOWN)
      return FALSE;
      if (ch->in_room->z < victim->in_room->z && ch->facing != DIR_UP)
      return FALSE;
      return TRUE;
    }

    int dir = roomdirection(0, 0, relative_x(ch, victim->in_room, victim->x), relative_y(ch, victim->in_room, victim->y));

    if (ch->facing == dir)
    return TRUE;
    int facing = ch->facing;
    facing = turn_dir[facing]; // NE
    if (facing == dir)
    return TRUE;
    facing = turn_dir[facing]; // E
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // SE
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // S
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // SW
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // W
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // NW
    if (facing == dir)
    return TRUE;

    return FALSE;
  }

  bool invisioncone_coordinates(CHAR_DATA *ch, int x, int y) {
    if (IS_NPC(ch))
    return TRUE;

    int dir = roomdirection(0, 0, x, y);

    if (ch->facing == dir)
    return TRUE;
    int facing = ch->facing;
    facing = turn_dir[facing]; // NE
    if (facing == dir)
    return TRUE;
    facing = turn_dir[facing]; // E
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // SE
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // S
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // SW
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // W
    if (facing == dir)
    return FALSE;
    facing = turn_dir[facing]; // NW
    if (facing == dir)
    return TRUE;

    return FALSE;
  }

  int charlineofsight_character(CHAR_DATA *ch, CHAR_DATA *victim) {
    ROOM_INDEX_DATA *desti = victim->in_room;

    if (in_world(ch) != in_world(victim))
    return -1;
    if (in_haven(ch->in_room) && !in_haven(desti))
    return -1;
    if (!in_haven(ch->in_room) && in_haven(desti))
    return -1;
    if (battleground(ch->in_room) && !battleground(desti))
    return -1;
    if (ch->in_room->x - desti->x > 100)
    return -1;
    if (ch->in_room->y - desti->y > 100)
    return -1;
    if (ch->in_room->x - desti->x < -100)
    return -1;
    if (ch->in_room->y - desti->y < -100)
    return -1;

    if (!invisioncone(ch, desti))
    return -1;

    int visionrange = vision_range_character(ch, victim);
    int dir;
    if (in_fight(ch)) {
      dir = vision_path(ch->in_room, victim->in_room);
    }
    else {
      dir = roomdirection_3d(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomz(ch->in_room), get_roomx(desti), get_roomy(desti), get_roomz(desti));
    }
    if (vision_exists(ch->in_room, dir)) {
      return lineofsight(ch, ch->in_room, desti, visionrange);
    }
    return -1;
  }

  int charlineofsight_room(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (in_haven(ch->in_room) && !in_haven(desti))
    return -1;
    if (!in_haven(ch->in_room) && in_haven(desti))
    return -1;
    if (battleground(ch->in_room) && !battleground(desti))
    return -1;
    if (ch->in_room->x - desti->x > 100)
    return -1;
    if (ch->in_room->y - desti->y > 100)
    return -1;
    if (ch->in_room->x - desti->x < -100)
    return -1;
    if (ch->in_room->y - desti->y < -100)
    return -1;

    if (!invisioncone(ch, desti))
    return -1;

    int visionrange = vision_range_room(ch, desti);
    int dir;
    if (in_fight(ch))
    dir = roomdirection_3d(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomz(ch->in_room), get_roomx(desti), get_roomy(desti), get_roomz(desti));
    else
    dir = vision_path(ch->in_room, desti);
    if (vision_exists(ch->in_room, dir)) {
      return lineofsight(ch, ch->in_room, desti, visionrange);
    }
    return -1;
  }

  bool charlineofsight_landmark(CHAR_DATA *ch, ROOM_INDEX_DATA *desti) {
    if (ch == NULL || ch->in_room == NULL || desti == NULL)
    return FALSE;
    if (in_haven(ch->in_room) && !in_haven(desti))
    return FALSE;
    if (!in_haven(ch->in_room) && in_haven(desti))
    return FALSE;
    if (battleground(ch->in_room) && !battleground(desti))
    return FALSE;
    if (ch->in_room->x - desti->x > 300)
    return FALSE;
    if (ch->in_room->y - desti->y > 300)
    return FALSE;
    if (ch->in_room->x - desti->x < -300)
    return FALSE;
    if (ch->in_room->y - desti->y < -300)
    return FALSE;

    if (!invisioncone(ch, desti))
    return FALSE;

    //    int dir = vision_path(ch->in_room, desti);
    int dir = roomdirection_3d(get_roomx(ch->in_room), get_roomy(ch->in_room), get_roomz(ch->in_room), get_roomx(desti), get_roomy(desti), get_roomz(desti));
    EXIT_DATA *pexit;
    if (vision_exists(ch->in_room, dir)) {
      pexit = ch->in_room->exit[dir];
      if (can_see_room_distance(ch, pexit->u1.to_room, DISTANCE_FAR)) {
        if (!IS_SET(pexit->u1.to_room->room_flags, ROOM_INDOORS))
        return TRUE;
        else
        return FALSE;
      }
    }
    return FALSE;
  }

  _DOFUN(do_lostest) {
    ROOM_INDEX_DATA *orig = ch->in_room;
    ROOM_INDEX_DATA *desti = get_room_index(atoi(argument));
    int direction = 0;
    EXIT_DATA *pexit;
    if (orig == desti)
    return;

    if (orig == NULL || desti == NULL)
    return;

    send_to_char("Starting.\n\r", ch);
    int distance = 0;
    for (; orig->vnum != desti->vnum; distance++) {
      direction = vision_path(orig, desti);
      if (direction == -1)
      return;
      pexit = orig->exit[direction];
      orig = pexit->u1.to_room;
      printf_to_char(ch, "%d, ", orig->vnum);
      if (distance > 200)
      return;
    }
    send_to_char("Finished.\n\r", ch);
    return;
  }

  int coord_path(ROOM_INDEX_DATA *in_room, int desx, int desy, int desz, bool xfirst) {
    if (in_room->x == desx && in_room->y == desy && in_room->z == desz)
    return -1;

    int xdiff = desx - get_roomx(in_room);
    int ydiff = get_roomy(in_room) - desy;
    int zdiff = get_roomz(in_room) - desz;

    if (zdiff > 0 && exit_exists(in_room, DIR_DOWN) && xfirst == FALSE)
    return DIR_DOWN;
    if (zdiff < 0 && exit_exists(in_room, DIR_UP) && xfirst == FALSE)
    return DIR_UP;

    if (UMAX(xdiff, xdiff * -1) > UMAX(ydiff, ydiff * -1)) {
      if (xdiff > 0) {
        if (ydiff > 0 && exit_exists(in_room, DIR_SOUTHEAST))
        return DIR_SOUTHEAST;
        else if (ydiff < 0 && exit_exists(in_room, DIR_NORTHEAST))
        return DIR_NORTHEAST;
        else if (exit_exists(in_room, DIR_EAST))
        return DIR_EAST;
      }
      if (xdiff < 0) {
        if (ydiff > 0 && exit_exists(in_room, DIR_SOUTHWEST))
        return DIR_SOUTHWEST;
        else if (ydiff < 0 && exit_exists(in_room, DIR_NORTHWEST))
        return DIR_NORTHWEST;
        else if (exit_exists(in_room, DIR_WEST))
        return DIR_WEST;
      }
    }

    if (ydiff > 0) {
      if (xdiff > 0 && exit_exists(in_room, DIR_SOUTHEAST))
      return DIR_SOUTHEAST;
      else if (xdiff < 0 && exit_exists(in_room, DIR_SOUTHWEST))
      return DIR_SOUTHWEST;
      else if (exit_exists(in_room, DIR_SOUTH))
      return DIR_SOUTH;
    }
    if (ydiff < 0) {
      if (xdiff > 0 && exit_exists(in_room, DIR_NORTHEAST))
      return DIR_NORTHEAST;
      else if (xdiff < 0 && exit_exists(in_room, DIR_NORTHWEST))
      return DIR_NORTHWEST;
      else if (exit_exists(in_room, DIR_NORTH))
      return DIR_NORTH;
    }

    if (zdiff > 0 && exit_exists(in_room, DIR_DOWN))
    return DIR_DOWN;
    if (zdiff < 0 && exit_exists(in_room, DIR_UP))
    return DIR_UP;
    return -1;
  }

  ROOM_INDEX_DATA *sourced_room_by_coordinates(ROOM_INDEX_DATA *orig, int desx, int desy, int desz, bool xfirst) {
    int direction = 0;
    EXIT_DATA *pexit;

    if (orig == NULL) {
      ROOM_INDEX_DATA *room = room_by_coordinates(desx, desy, desz);
      if (room != NULL)
      return room;
      return NULL;
    }

    if (orig->x == desx && orig->y == desy && orig->z == desz)
    return orig;

    for (int i = 0; i < 50; i++) {
      direction = coord_path(orig, desx, desy, desz, xfirst);
      if (direction == -1)
      return NULL;
      pexit = orig->exit[direction];
      orig = pexit->u1.to_room;
      if (orig->x == desx && orig->y == desy && orig->z == desz)
      return orig;
    }
    return NULL;
  }

  char *ascii_map_code(sh_int code) {
    if (code == MAP_FOREST)
    return "`gF";
    if (code == MAP_PARK)
    return "`GP";
    if (code == MAP_SHALLOW)
    return "`CW";
    if (code == MAP_WATER)
    return "`cW";
    if (code == MAP_SHOP)
    return "`yS";
    if (code == MAP_HOUSE)
    return "`mH";
    if (code == MAP_TOWN)
    return "`xT";
    if (code == MAP_STREET)
    return "`W=";
    if (code == MAP_ROAD)
    return "`y=";
    if (code == MAP_BEACH)
    return "`YB";
    if (code == MAP_ROCKS)
    return "`wR";
    if (code == MAP_ERROR)
    return "`RX";

    return "`x#";
  }

  void build_a_map(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    if (asciimap[0][0] == 0)
    buildasciimap();

    obj = create_object(get_obj_index(14), 0);
    free_string(obj->name);
    obj->name = str_dup("map haven paper");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("map of Haven");
    free_string(obj->description);
    obj->description = str_dup("A paper map of Haven");
    obj_to_char(obj, ch);

    static char string[MSL];
    string[0] = '\0';
    EXTRA_DESCR_DATA *ed;
    /*
for(int y=188;y>= 0;y--)
{
for(int x=0;x<150;x++)
{
*/
    for (int y = 110; y >= 68; y--) {
      for (int x = 78; x < 120; x++) {
        strcat(string, ascii_map_code(asciimap[x][y]));
      }
      strcat(string, "`x\n");
    }
    for (ed = obj->extra_descr; ed; ed = ed->next) {
      if (is_name("all", ed->keyword))
      break;
    }
    if (!ed) {
      ed = new_extra_descr();
      ed->keyword = "all";
      ed->next = obj->extra_descr;
      obj->extra_descr = ed;
      free_string(obj->extra_descr->description);
      obj->extra_descr->description = str_dup(string);
    }
    else {
      free_string(ed->description);
      ed->description = str_dup(string);
    }
    page_to_char(string, ch);
  }

  bool in_dockyard(ROOM_INDEX_DATA *room) {
    if (room->x > 4 && room->x < 15 && room->y > 44 && room->y < 51)
    return TRUE;
    return FALSE;
  }

  sh_int room_map_color(ROOM_INDEX_DATA *room) {
    PROP_TYPE *prop;
    if (room == NULL) {
      return MAP_ERROR;
    }
    if ((prop = prop_from_room(room)) != NULL) {
      if (prop->type == PROP_SHOP && IS_SET(room->room_flags, ROOM_INDOORS)) {
        if (str_cmp(prop->owner, "NPC"))
        return MAP_SHOPOWNED;
        else
        return MAP_SHOP;
      }
      else if (prop->type == PROP_HOUSE && IS_SET(room->room_flags, ROOM_INDOORS)) {
        if (str_cmp(prop->owner, "NPC"))
        return MAP_HOUSEOWNED;
        else
        return MAP_HOUSE;
      }
    }
    if (room->sector_type == SECT_FOREST)
    return MAP_FOREST;
    if (room->sector_type == SECT_PARK)
    return MAP_PARK;
    if (room->sector_type == SECT_SHALLOW)
    return MAP_SHALLOW;
    if (room->sector_type == SECT_WATER)
    return MAP_WATER;
    if (room->sector_type == SECT_SIDEWALK)
    return MAP_SIDEWALK;
    if (room->sector_type == SECT_PARKING)
    return MAP_PARKING;
    if (room->sector_type == SECT_DIRT)
    return MAP_DIRT;
    if (room->sector_type == SECT_CEMETARY)
    return MAP_CEMETARY;
    if (in_lodge(room))
    return MAP_LODGE;
    if (in_dockyard(room))
    return MAP_DOCKS;

    if (room->sector_type == SECT_STREET) {
      if (IS_SET(room->room_flags, ROOM_DIRTROAD))
      return MAP_ROAD;
      else
      return MAP_STREET;
    }
    if (room->sector_type == SECT_ALLEY) {
      return MAP_ALLEY;
    }
    if (room->sector_type == SECT_BEACH) {
      return MAP_BEACH;
    }
    if (room->sector_type == SECT_ROCKY) {
      return MAP_ROCKS;
    }
    if (in_medcent(room)) {
      return MAP_MEDICAL;
    }

    if (room->sector_type == SECT_HOUSE || room->sector_type == SECT_SHOP || room->sector_type == SECT_COMMERCIAL) {
      if (room->sector_type == SECT_HOUSE)
      return MAP_HOUSEOWNED;
      return MAP_TOWN;
    }
    if (room->sector_type == SECT_STREET) {
      if (IS_SET(room->room_flags, ROOM_DIRTROAD)) {
        return MAP_ROAD;
      }
      else {
        return MAP_STREET;
      }
    }

    return MAP_OTHER;
  }

  void buildasciimap(void) {
    int yshift = -59;
    int xshift = -58;
    ROOM_INDEX_DATA *row_room = room_by_coordinates(xshift, yshift, 0);
    for (int y = 0; y < 189; y++) {
      row_room =
      sourced_room_by_coordinates(row_room, xshift, y + yshift, 0, TRUE);
      ROOM_INDEX_DATA *in_room = row_room;
      for (int x = 0; x < 150; x++) {
        int newx = x + xshift;
        int newy = y + yshift;
        in_room = sourced_room_by_coordinates(in_room, newx, newy, 0, TRUE);
        if (in_room == NULL)
        in_room = room_by_coordinates(newx, newy, 0);

        asciimap[x][y] = room_map_color(in_room);
      }
    }
  }

  void buildmaps(void) {
    buildasciimap();
    trytownmap();
    tryforestmap(DIR_NORTH);
    tryforestmap(DIR_SOUTH);
    tryforestmap(DIR_WEST);
  }

  int offworld_direction(ROOM_INDEX_DATA *room) {
    for (int i = 0; i < 10; i++) {
      if (room->exit[i] != NULL && room->exit[i]->u1.to_room != NULL && room->area->world != room->exit[i]->u1.to_room->area->world)
      return i;
    }
    return -1;
  }

  int compass_direction(ROOM_INDEX_DATA *room) {
    if (room == NULL)
    return -1;
    if (room->area->vnum < OUTER_NORTH_FOREST || room->area->vnum > HELL_FOREST_VNUM)
    return -1;

    ROOM_INDEX_DATA *oroom = NULL;

    int mindist = 20;
    int minvnum = -1;
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;

      if (room->vnum == (*it)->havenroom)
      return (*it)->direction;
      if (room->vnum == (*it)->worldroom)
      return rev_dir[(*it)->direction];

      if (room->area->world == WORLD_EARTH)
      oroom = get_room_index((*it)->havenroom);
      else {
        if ((*it)->world == room->area->world)
        oroom = get_room_index((*it)->worldroom);
      }
      if(oroom == NULL)
      continue;

      if (get_dist(room->x, room->y, oroom->x, oroom->y) < mindist) {
        minvnum = oroom->vnum;
        mindist = get_dist(room->x, room->y, oroom->x, oroom->y);
      }
    }
    char buf[MSL];
    sprintf(buf, "CDIR: mroom: %d, mdist: %d\n\r", minvnum, mindist);
    log_string(buf);
    if (minvnum > 0 && get_room_index(minvnum) != NULL)
    return roomdirection(room->x, room->y, get_room_index(minvnum)->x, get_room_index(minvnum)->y);
    return -1;
  }

  bool world_access(CHAR_DATA *ch, int world) {
    for (vector<GATEWAY_TYPE *>::iterator it = gatewayVect.begin();
    it != gatewayVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if ((*it)->world != world)
      continue;

      if (knows_gateway(ch, (*it)->havenroom))
      return TRUE;
    }
    return FALSE;
  }

#if defined(__cplusplus)
}
#endif
