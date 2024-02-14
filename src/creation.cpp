#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#pragma warning (disable : 4800)
#endif
#endif

#include "merc.h"
#include "olc.h"
#include "gsn.h"

#if defined(__cplusplus)
extern "C" {
#endif


  void set_whotitle args((CHAR_DATA * ch, char *whotitle));
  void state_read_storyline(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);

#define CREATION(fun) void fun(CHAR_DATA *ch, char *argument)

  DECLARE_DO_FUN(credit_show_email);
  DECLARE_DO_FUN(credit_show_lastname);
  DECLARE_DO_FUN(credit_show_sex);
  DECLARE_DO_FUN(credit_show_class);
  DECLARE_DO_FUN(credit_show_year);
  DECLARE_DO_FUN(credit_show_month);
  DECLARE_DO_FUN(credit_show_day);
  DECLARE_DO_FUN(credit_show_height_feet);
  DECLARE_DO_FUN(credit_show_height_inches);
  DECLARE_DO_FUN(credit_show_bust);
  DECLARE_DO_FUN(credit_show_intro);
  DECLARE_DO_FUN(credit_show_desc);
  DECLARE_DO_FUN(credit_show_sireyear);
  DECLARE_DO_FUN(credit_show_siremonth);
  DECLARE_DO_FUN(credit_show_sireday);
  DECLARE_DO_FUN(credit_show_customise);
  DECLARE_DO_FUN(credit_show_disciplines);
  DECLARE_DO_FUN(credit_show_skills);
  DECLARE_DO_FUN(credit_show_factions);
  DECLARE_DO_FUN(credit_show_adult);
  DECLARE_DO_FUN(credit_show_human);
  DECLARE_DO_FUN(credit_show_monster);
  DECLARE_DO_FUN(credit_show_special);
  DECLARE_DO_FUN(credit_show_demonborn);
  DECLARE_DO_FUN(credit_show_angelborn);
  DECLARE_DO_FUN(credit_show_faeborn);
  DECLARE_DO_FUN(credit_show_demigod);
  DECLARE_DO_FUN(credit_show_shifter);
  DECLARE_DO_FUN(credit_show_eyes);
  DECLARE_DO_FUN(credit_show_hair);
  DECLARE_DO_FUN(credit_show_skin);

  DECLARE_DO_FUN(credit_help);

  DECLARE_DO_FUN(credit_email);
  DECLARE_DO_FUN(credit_lastname);
  DECLARE_DO_FUN(credit_sex);
  DECLARE_DO_FUN(credit_class);
  DECLARE_DO_FUN(credit_year);
  DECLARE_DO_FUN(credit_month);
  DECLARE_DO_FUN(credit_day);
  DECLARE_DO_FUN(credit_height_feet);
  DECLARE_DO_FUN(credit_height_inches);
  DECLARE_DO_FUN(credit_bust);
  DECLARE_DO_FUN(credit_intro);
  DECLARE_DO_FUN(credit_desc);
  DECLARE_DO_FUN(credit_sireyear);
  DECLARE_DO_FUN(credit_siremonth);
  DECLARE_DO_FUN(credit_sireday);
  DECLARE_DO_FUN(credit_customise);
  DECLARE_DO_FUN(credit_disciplines);
  DECLARE_DO_FUN(credit_skills);
  DECLARE_DO_FUN(credit_factions);
  DECLARE_DO_FUN(credit_adult);
  DECLARE_DO_FUN(credit_show);
  DECLARE_DO_FUN(credit_human);
  DECLARE_DO_FUN(credit_monster);
  DECLARE_DO_FUN(credit_special);
  DECLARE_DO_FUN(credit_demonborn);
  DECLARE_DO_FUN(credit_angelborn);
  DECLARE_DO_FUN(credit_faeborn);
  DECLARE_DO_FUN(credit_demigod);
  DECLARE_DO_FUN(credit_shifter);
  DECLARE_DO_FUN(credit_eyes);
  DECLARE_DO_FUN(credit_hair);
  DECLARE_DO_FUN(credit_skin);

  /*
  * Structure for the creation location shows.
  */
  struct creation_show_type {
    int step;
    DO_FUN *show_fun;
  };

  void finish_creation(CHAR_DATA *ch) {
    char buf[MSL];
    ch->version = CURR_VERSION;
    SET_FLAG(ch->comm, COMM_NOGUIDE);

    /* Now lets set up the autoflags.. */

    ch->hit = UMAX(10000, max_hp(ch));

    ch->pcdata->version_player = 2;
    int years = get_real_age(ch) - 18;
    years = UMIN(years, 25);
    years = UMAX(1, years);

    if (IS_FLAG(ch->act, PLR_GM))
    ch->money = 0;
    else {
      ch->pcdata->total_credit = years * 50000;
      ch->pcdata->total_credit += 100000;
      ch->money = years * 5000;
      ch->pcdata->total_money = years * 5000;
    }
    if (IS_FLAG(ch->act, PLR_GUEST)) {
      ch->pcdata->total_credit *= 3;
      ch->money = 0;
      ch->pcdata->total_money = 0;
    }
    ch->pcdata->total_money += ch->money;
    ch->money = 2500;
    ch->pcdata->create_date = current_time;
    ch->pcdata->last_paid = current_time;
    if (ch->sex == SEX_MALE)
    ch->pcdata->strains += 3;

    sprintf(buf, "%s %s", ch->pcdata->last_name, "the Newbie");
    set_whotitle(ch, buf);
    ch->pcdata->difficulty = 3;
    // char_to_room(ch, get_room_index(50));
    if (ch->in_room != NULL) {
      char_from_room(ch);
      char_to_room(ch, get_room_index(50));
    }
    do_function(ch, &do_prompt, "starter");

    if (ch->faction > 0) {
      ch->pcdata->class_type = 1;
      ch->pcdata->class_faction = ch->faction;
    }
    else {
      if (is_super(ch))
      ch->pcdata->class_type = 1;
    }

    if (!IS_FLAG(ch->comm, COMM_PROMPT))
    SET_FLAG(ch->comm, COMM_PROMPT);

    if (!IS_FLAG(ch->comm, COMM_STORY))
    SET_FLAG(ch->comm, COMM_STORY);

    if (IS_FLAG(ch->comm, COMM_CONSENT))
    REMOVE_FLAG(ch->comm, COMM_CONSENT);

    if (!IS_FLAG(ch->comm, COMM_SUBDUE))
    SET_FLAG(ch->comm, COMM_SUBDUE);

    if (ch->faction > 0)
    join_to_clan(ch, ch->faction);
    /* Disabling forum stuff for general release
    //Writing usrrgtmp.txt for forum registration - Discordance
    FILE *fp;

    if ( ( fp = fopen( "../player/usrrgtmp.txt", "a" ) ) == NULL ) {
      perror( "../../public_html/forum/usrrgtmp.txt" );
      send_to_char( "Could not open the file!\n\r", ch );
    }
    else {
      fprintf( fp, "%s\n", ch->name);
      fprintf( fp, "%s\n", ch->pcdata->upwd);
      fprintf( fp, "%s\n", ch->pcdata->upwd);
      fprintf( fp, "%s\n", ch->pcdata->email);
      fprintf( fp, "%s\n", ch->pcdata->email);
      fclose( fp );
    }
    //Running PHP script to add user to forum based on usrrgtmp.txt info - Discordance
    system ("php -f ../../public_html/forum/usrrg.php");
    system ("rm ../player/usrrgtmp.txt");

    //Clearing unecrypted password - Discordance
    free_string(ch->pcdata->upwd);
    ch->pcdata->upwd=str_dup("");
    */
    send_to_char(buf, ch);
    send_to_char("[Hit RETURN to continue]\n\r", ch);
    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    ch->desc->connected = CON_READ_STORYLINE;
    send_to_char("Hit return to continue!\n\r", ch);
    state_read_storyline(ch->desc, "", ch);
  }

  const struct creation_show_type advedit_table[] = {
    {CR_ALL, credit_show}, {CR_EMAIL, credit_show_email}, {CR_LASTNAME, credit_show_lastname}, {CR_SEX, credit_show_sex}, {CR_CLASS, credit_show_class}, {CR_HUMAN, credit_show_human}, {CR_MONSTER, credit_show_monster}, {CR_DEMONBORN, credit_show_demonborn}, {CR_DEMIGOD, credit_show_demigod}, {CR_SHIFTER, credit_show_shifter}, {CR_FAEBORN, credit_show_faeborn}, {CR_ANGELBORN, credit_show_angelborn}, {CR_YEAR, credit_show_year}, {CR_MONTH, credit_show_month}, {CR_DAY, credit_show_day}, {CR_SIREYEAR, credit_show_sireyear}, {CR_SIREMONTH, credit_show_siremonth}, {CR_SIREDAY, credit_show_sireday}, {CR_HEIGHTFEET, credit_show_height_feet}, {CR_HEIGHTINCH, credit_show_height_inches}, {CR_EYES, credit_show_eyes}, {CR_HAIR, credit_show_hair}, {CR_SKIN, credit_show_skin}, {CR_BUST, credit_show_bust}, {CR_INTRO, credit_show_intro}, {CR_FACTIONS, credit_show_factions}, {CR_CUSTOMISE, credit_show_customise}, {CR_DISCIPLINES, credit_show_disciplines}, {CR_SKILLS, credit_show_skills}, {CR_ADULT, credit_show_adult}, {CR_SPECIAL, credit_show_special}, 
  };

/*
* Structure for an Creation editor command.
*/
  struct creation_cmd_type {
    char *comm;
    DO_FUN *fun;
    char *help;
    int step;
  };

/*
* List of commands available during creation and their
* associated functions and helpfiles
*/
  const struct creation_cmd_type credit_table[] = {
/*  {command        function        	  helpfile                  Creation Step}, */
    {"year",        credit_year,          "creation_birthday",      CR_YEAR},
    {"month",       credit_month,         "creation_birthday",      CR_MONTH},
    {"sireyear",    credit_sireyear,      "creation_sireyear",      CR_SIREYEAR},
    {"siremonth",   credit_siremonth,     "creation_siremonth",     CR_SIREMONTH},
    {"sireday",     credit_sireday,       "creation_sireday",       CR_SIREDAY},
    {"customise",   credit_customise,     "creation_customise",     CR_CUSTOMISE},
    {"disciplines", credit_disciplines,   "creation_disciplines",   CR_DISCIPLINES},
    {"stats",       credit_skills,        "creation_skills",        CR_SKILLS},
    {"factions",    credit_factions,      "creation_factions",      CR_FACTIONS},
    {"monster",     credit_monster,       "creation_monster",       CR_MONSTER},
    {"demonborn",   credit_demonborn,     "creation_demonborn",     CR_DEMONBORN},
    {"demigod",     credit_demigod,       "creation_demigod",       CR_DEMIGOD},
    {"shifter",     credit_shifter,       "creation_shifter",       CR_SHIFTER},
    {"faeborn",     credit_faeborn,       "creation_faeborn",       CR_FAEBORN},
    {"angelborn",   credit_angelborn,     "creation_angelborn",     CR_ANGELBORN},
    {"special",     credit_special,       "creation_special",       CR_SPECIAL},
    {"human",       credit_human,         "creation_human",         CR_HUMAN},
    {"hair",        credit_hair,          "creation_hair",          CR_HAIR},
    {"eyes",        credit_eyes,          "creation_eyes",          CR_EYES},
    {"skin",        credit_skin,          "creation_skin",          CR_SKIN},
    {"adult",       credit_adult,         "creation_adult",         CR_ADULT},
    {"day",         credit_day,           "creation_birthday",      CR_DAY},
    {"feet",        credit_height_feet,   "creation_height_feet",   CR_HEIGHTFEET},
    {"inches",      credit_height_inches, "creation_height_inches", CR_HEIGHTINCH},
    {"bust",        credit_bust,          "creation_bust",          CR_BUST},
    {"intro",       credit_intro,         "creation_intro",         CR_INTRO},
    {"email",       credit_email,         "creation_email",         CR_EMAIL},
    {"help",        credit_help,          NULL,                     CR_ALL},
    {"lastname",    credit_lastname,      NULL,                     CR_LASTNAME},
    {"newbie",      do_newbie,            NULL,                     CR_ALL},
    {"ooc",         do_ooc,               NULL,                     CR_ALL},
    {"pray",        do_pray,              NULL,                     CR_ALL},
    {"quit",        do_quit,              NULL,                     CR_ALL},
    {"class",       credit_class,         "creation_class",         CR_CLASS},
    {"save",        do_save,              NULL,                     CR_ALL},
    {"sex",         credit_sex,           NULL,                     CR_SEX},
    {"who",         do_who,               NULL,                     CR_ALL},
    {NULL,          NULL,                 NULL,                     0}
  };

  /*
  * Structure for the new turn table.
  */
  struct newturn_cmd_type {
    char *comm;
    DO_FUN *fun;
    char *help;
  };

  /******************************************************************
  * Name: creation
  * Parameters:
  *	CHAR_DATA *ch - character currently in creation
  *      char *argument - the command being issued
  * Returns:
  *	void
  *
  * Purpose:  Controls the flow in creation.  When a command is
  *	issued this function checks the list for that command and
  *	calls the associated function of the command is found.
  * Author: Cameron Matthews-Dickson (Scaelorn)
  *****************************************************************/
    void creation(CHAR_DATA *ch, char *argument) {
      char arg[MAX_INPUT_LENGTH];
      char command[MAX_INPUT_LENGTH];
      int cmd;

      smash_tilde(argument);
      strcpy(arg, argument);

      return;
      argument = one_argument(argument, command);

      if (command[0] == '\0' || !str_cmp(command, "")) {
        (*advedit_table[ch->pcdata->creation_location].show_fun)(ch, "");
        return;
      }

      if (!str_cmp(command, "back")) {
        return;
        if (ch->pcdata->creation_location == CR_EMAIL) {
          send_to_char("You cannot go back any further than this.\n\r", ch);
          return;
        }

        ch->pcdata->creation_location--;

        // fixing some badness in chargen that results in character accruing free
        // trains - Discordance
        if (ch->pcdata->creation_location == CR_ANGELBORN) {
          while (ch->pcdata->creation_location != CR_CLASS) {
            ch->pcdata->creation_location--;
          }
        }

        // this addresses asking about cup size for guys - Discordance
        if (ch->pcdata->creation_location == CR_BUST && ch->pcdata->penis > 0) {
          ch->pcdata->creation_location--;
        }

        // this addresses vamp stuff for non vamps - Discordance
        if (ch->pcdata->creation_location == CR_SIREDAY && !is_vampire(ch)) {
          while (ch->pcdata->creation_location != CR_DAY) {
            ch->pcdata->creation_location--;
          }
        }

        creation(ch, "");
        return;
      }

      // Removed buggy channels from chargen - Discordance
      if (ch->pcdata->creation_location != CR_ALL && str_prefix(command, "help"))
      //&& str_prefix( command, "gossip") && //str_prefix( command, "newbie")
      //&& str_prefix( command, "ooc") && str_prefix( command, "pray"))
      {
        if (argument[0] != '\0' && str_cmp(argument, ""))
        strcat(command, " ");
        strcat(command, argument);
        strcpy(argument, command);
      }

      /* Search Creation Table and Execute Command */
      for (cmd = 0; credit_table[cmd].comm != NULL; cmd++) {
        if ((!str_prefix(command, credit_table[cmd].comm) && ((ch->pcdata->creation_location == CR_ALL) || (credit_table[cmd].step == CR_ALL)))) {
          (*credit_table[cmd].fun)(ch, argument);
          return;
        }
      }

      for (cmd = 0; credit_table[cmd].comm != NULL; cmd++) {
        if (ch->pcdata->creation_location != CR_ALL && ch->pcdata->creation_location == credit_table[cmd].step) {
          (*credit_table[cmd].fun)(ch, argument);
          return;
        }
      }

      /* Command not found */
      send_to_char("There is no such command in creation.\n\rFor a list of commands use `Bhelp list`x.\n\r", ch);
      return;
    }

    // This median heights according to the world health organization - Discordance
    char *median_height_for_sex(CHAR_DATA *ch) {
      if (ch->pcdata->penis == 0) {
        return str_dup("5'4\"");
      }
      else {
        return str_dup("5'9\"");
      }
      return str_dup("");
    }

    CREATION(credit_show) { return; }

    CREATION(credit_show_email) {
      send_to_char("It's often useful for us to have player emails on file, if you'd rather we\n\r", ch);
      send_to_char("didn't have this information just type in 'none'\n\r", ch);
      send_to_char("\n\n\r`WPlease Type Your Email: `x\n\n\r", ch);
      return;
    }
    CREATION(credit_show_lastname) {
      send_to_char("You should now select your character's lastname, \n\r", ch);
      send_to_char("choose something suitable to to a modern world.\n\r", ch);
      send_to_char("`WLastname:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_sex) {
      Buffer outbuf;
      outbuf.strcat("Now its time to choose your gender.  Simply pick either\n\r`BMale`x or `MFemale`x.\n\r");
      outbuf.strcat("Male characters currently receive a small number of bonus stats on exiting creation.\n\r");
      outbuf.strcat("\n\r`WPlease select your gender:`x\n\n\r");
      send_to_char(outbuf, ch);
      return;
    }
    CREATION(credit_show_class) {
      send_to_char("You should now pick your character's race\n\r", ch);
      send_to_char("The categories are: Innocent, Human, Monster, Angelborn, Demonborn, Faeborn, Demigod\n\r", ch);
      send_to_char("Type help 'category' for more information on each.\n\r", ch);

      send_to_char("\n\r`WPlease select your racial category:`x\n\n\r", ch);

      return;
    }
    CREATION(credit_show_year) {
      send_to_char("Please choose the year in which you wish your character to be born as\n\r", ch);
      send_to_char("a number. Alternately you can just pick an age and have your birthday be\n\r", ch);
      send_to_char("randomly generated.\n `WYear/Age:`x\n\r", ch);

      return;
    }
    CREATION(credit_show_month) {
      send_to_char("Please choose the month in which you wish your character to be born as a number \n\r", ch);
      send_to_char("between 1 and 12.\n`WMonth:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_day) {
      send_to_char("Please choose the day on which you wish your character to be born as a number\n\r", ch);
      send_to_char("`WDay:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_height_feet) {
      printf_to_char(ch, "The median height for your age and sex is %s.\n\r\n\r", median_height_for_sex(ch));
      send_to_char("You should now select how tall your character is in feet\n\r", ch);
      send_to_char("as a number between 3 and 6 depending on your age and sex.\n\r", ch);
      send_to_char("You'll be able to select the inch portion afterward.\n\r", ch);
      send_to_char("Please enter your height in feet now.\n\r", ch);
      send_to_char("`WFeet:`x\n\r", ch);

      return;
    }

    CREATION(credit_show_height_inches) {
      send_to_char("Please enter the inch part of your height now.\n\r", ch);
      send_to_char("`WInches:\n\r`x", ch);
      return;
    }
    CREATION(credit_show_bust) {
      send_to_char("You should now select the size of your character's bust as a number\n\r", ch);
      send_to_char("Between 1 and 12.\n\r", ch);

      send_to_char("1 - A cup\n\r", ch);
      send_to_char("2 /\n\r", ch);
      send_to_char("3 \\ \n\r", ch);
      send_to_char("4  ) B cup\n\r", ch);
      send_to_char("5 / \n\r", ch);
      send_to_char("6 \\ \n\r", ch);
      send_to_char("7  ) C cup\n\r", ch);
      send_to_char("8 / \n\r", ch);
      send_to_char("9  \\ \n\r", ch);
      send_to_char("10  ) D cup\n\r", ch);
      send_to_char("11 / \n\r", ch);
      send_to_char("12 DD cup\n\n\r", ch);
      send_to_char("`WBust:`x\n\r", ch);
    }
    CREATION(credit_show_intro) {
      send_to_char("You should now choose your character's one line intro. This is a\n\r", ch);
      send_to_char("string that people would see when they don't know your name, something\n\r", ch);
      send_to_char("like 'A tall, dark skinned man in his thirties' would be\n\r", ch);
      send_to_char("appropriate.\n\r", ch);
      send_to_char("Please enter your intro as a string now.\n\r", ch);
      send_to_char("`WIntro:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_hair) {
      send_to_char("You should now choose your hair description, this will appear as\n\r", ch);
      send_to_char("He/She has (string). So for instance if you enter long black hair\n\r", ch);
      send_to_char("It will appear like he has long black hair. Do not end the string\n\r", ch);
      send_to_char("with punctuation.\n\r", ch);
      send_to_char("Please enter your hair as a string now.\n\r", ch);
      send_to_char("`WHair:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_eyes) {
      send_to_char("You should now choose your eyes description, this will appear as\n\r", ch);
      send_to_char("He/She has (string). So for instance if you enter deep blues eyes\n\r", ch);
      send_to_char("It will appear like he has deep blue eyes. Do not end the string\n\r", ch);
      send_to_char("with punctuation.\n\r", ch);
      send_to_char("Please enter your eyes as a string now.\n\r", ch);
      send_to_char("`WEyes:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_skin) {
      send_to_char("You should now choose your skin description, this will appear as\n\r", ch);
      send_to_char("He/She has (string). So for instance if you enter tanned Caucasian skin\n\r", ch);
      send_to_char("It will appear like he has tanned Caucasian skin. Do not end the string\n\r", ch);
      send_to_char("with punctuation.\n\r", ch);
      send_to_char("Please enter your skin as a string now.\n\r", ch);
      send_to_char("`WSkin:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_human) {
      send_to_char("You should now choose if you want to be supernatural or not.\n\r", ch);
      send_to_char("Available options are: Normal, Touched.\n\r", ch);
      send_to_char("See help human for more information.\n\r", ch);
      send_to_char("Please enter your subtype now.\n\r", ch);
      send_to_char("`Wsubtype:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_monster) {
      send_to_char("You should now choose your species.\n\r", ch);
      send_to_char("Available options are: Vampire, Werewolf.\n\r", ch);
      send_to_char("See help (species name) for more information.\n\r", ch);
      send_to_char("Please enter your species now.\n\r", ch);
      send_to_char("`WSpecies:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_special) {
      send_to_char("You should now choose your archetype.\n\r", ch);
      send_to_char("Available options are: None.\n\r", ch);
      send_to_char("See help (archetype name) for more information.\n\r", ch);
      send_to_char("Please enter your archetype now.\n\r", ch);
      send_to_char("`WArchetype:`x\n\r", ch);
      return;
    }

    CREATION(credit_show_demonborn) {
      send_to_char("You should now choose your racial ability.\n\r", ch);
      send_to_char("Available options are: Seduce, enrage, ragefueled, nightmares, wailing, none.\n\r", ch);
      send_to_char("See help demonborn for more information.\n\r", ch);
      send_to_char("Please enter your ability now.\n\r", ch);
      send_to_char("`WAbility:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_angelborn) {
      send_to_char("You should now choose your racial ability.\n\r", ch);
      send_to_char("Available options are: Protection, fleetfooted, healingtouch, none.\n\r", ch);
      send_to_char("See help angelborn for more information.\n\r", ch);
      send_to_char("Please enter your ability now.\n\r", ch);
      send_to_char("`WAbility:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_faeborn) {
      send_to_char("You should now choose your racial ability.\n\r", ch);
      send_to_char("Available options are: Luckchanging, mershifting, blooming, sirensong, senseweakness, none.\n\r", ch);
      send_to_char("See help faeborn for more information.\n\r", ch);
      send_to_char("Please enter your ability now.\n\r", ch);
      send_to_char("`WAbility:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_shifter) {
      send_to_char("You should now choose your archetype. This will determine the type of animal you will turn into\n\r", ch);
      send_to_char("Available options are: Flier, Runner, Fighter, Swimmer.\n\r", ch);
      send_to_char("See help shifter for more information.\n\r", ch);
      send_to_char("Please enter your archetype now.\n\r", ch);
      send_to_char("`WArchetype:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_demigod) {
      send_to_char("You should now choose your racial ability.\n\r", ch);
      send_to_char("Available options are: Stormcalling, oceanicaffinity, solarblessing, lunarblessing, deathtouch, warblessing, attraction, none.\n\r", ch);
      send_to_char("See help demigod for more information.\n\r", ch);
      send_to_char("Please enter your ability now.\n\r", ch);
      send_to_char("`WAbility:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_desc) {

      send_to_char("You should now enter a description, it's strongly\n\r", ch);
      send_to_char("encouraged for you to edit this later, and newbie school will\n\r", ch);
      send_to_char("teach you how, remember to include details such as build, hair and eyes\n\r", ch);
      send_to_char("Type @ on a new line to exit the editor. \n\r", ch);
      send_to_char("Once done type edit to re-enter the editor, or done to move on.\n\r", ch);
      send_to_char("`WDesc:`x\n\r", ch);
      string_append(ch, &ch->description);
      return;
    }
    CREATION(credit_show_sireyear) {
      send_to_char("Please choose the year in which you wish your character to have been turned into\n\r", ch);
      send_to_char("a vampire as a number. Alternately you can just choose how old you wish to be\n\r", ch);
      send_to_char("when sired and have the date be randomly generated.\n `WYear/Age:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_siremonth) {
      send_to_char("Please choose the month in which you wish your character to have been turned into a\n\r", ch);
      send_to_char("vampre as a number between 1 and 12.\n`WMonth:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_sireday) {
      send_to_char("Please choose the day on which you wish your character to have been turned into a\nvampire as a number.\n`WDay:`x\n\r", ch);
      return;
    }
    CREATION(credit_show_customise) {
      send_to_char("Now please take a few minutes to pick your starting stats.\n\r", ch);
      send_to_char("Type continue to proceed.\n\r", ch);
      return;
    }
    CREATION(credit_show_disciplines) {
      send_to_char("Disciplines are the attributes with which your character will fight, you will\n\r", ch);
      send_to_char("effectively use three most of the time; one ranged, one melee and one defensive.\n\r", ch);
      send_to_char("Although some characters may wish for more variation. If you are not highly\n\r", ch);
      send_to_char("familiar with combat here it is very strongly recommended to not take more than\n\r", ch);
      send_to_char("one ranged discipline or more than one melee disipline.\n\r", ch);
      send_to_char("Your current disciplines have been selected by your faction choice, you can\n\r", ch);
      send_to_char("change them by entering 'Discipline name' 'new number' for instance 'brawling 5'\n\r", ch);
      send_to_char("'would change your brawling to 5, you have 60 points overall to distribute.\n\r", ch);
      send_to_char("You can type reset to wipe all the disciplines.\n\r", ch);
      send_to_char("Type 'done' once you a ready to move on.\n\r", ch);
      do_function(ch, &do_disciplines, "creation");
      printf_to_char(ch, "\n\rYou  have %d points left to distribute.\n\r", ch->pcdata->dtrains);
      return;
    }
    CREATION(credit_show_skills) {
      send_to_char("RP stats, or simply stats are all the attributes and abilities your character\n\r", ch);
      send_to_char("has that aren't direct combat abilities, some of these have mechanical\n\r", ch);
      send_to_char("functions in the game, others are RP tools to flesh out your character, each\n\r", ch);
      send_to_char("has a helpfile you can read for more information. Ro raise a skill to 1 costs\n\r", ch);
      send_to_char("1 point, to raise a skill from 1 to 2 costs 2 points, to raise from 0 to 2, 3.\n\r", ch);
      send_to_char("This carries on, to raise a skill from 3 to 4 is 4 points, to raise one from 0\n\r", ch);
      send_to_char("to 5 is 15 total etc. Just type in the name of the stat and then what you'd like\n\r", ch);
      send_to_char("it's new value to be, you can type reset to wipe all the stats.\nYou can only have one supernatural power.\nType 'done' once you are ready to move on.\n\r", ch);
      send_to_char("You begin with 25 points to distribute over these, you must spend\n\r", ch);
      send_to_char("At least 5 points on a combination of physical and contacts.\n\r", ch);
      send_to_char("At least 5 points on a combination of skills and intellectual.\n\r", ch);
      send_to_char("At least 3 points on abilities.\n\r", ch);
      send_to_char("Be sure to use these pools first.\n\r", ch);

      do_function(ch, &do_skills, "creation");
      printf_to_char(ch, "\n\rYou  have %d points left to distribute.\n\r", ch->pcdata->strains);
      return;
    }

    CREATION(credit_show_factions) {
      send_to_char("You should now pick the faction you want your character to belong to.\n\r", ch);
      send_to_char("You can see a list of the factions with list, or more information on\n\r", ch);
      send_to_char("one of them with info (faction name), to find out more about the parent\n\r", ch);
      send_to_char("factions see help hand/order/temple. You can also choose none. Factions\n\r", ch);
      send_to_char("are for 18+ characters.\n\r", ch);
      send_to_char("Choose your faction now:\n\r", ch);
      return;
    }

    CREATION(credit_show_adult) {
      send_to_char("Haven is a mature role playing game in which there are no restrictions on\n\r", ch);
      send_to_char("content, as such depictions of graphic violence, sex or other mature themes may\n\r", ch);
      send_to_char("be presented to the player over the course of their time here. Although we don't\n\r", ch);
      send_to_char("believe these things are common that is ultimately dependant on the players and\n\r", ch);
      send_to_char("their RP, we don't in any way limit what people can or will RP.\n\r", ch);
      send_to_char("By typing continue you state that it is legal for you to view mature material\n\r", ch);
      send_to_char("and that you are not offended by such.\n\n\r", ch);
      send_to_char("Please type 'continue' now, or type 'quit' to leave the game.\n\r", ch);
      return;
    }

    CREATION(credit_email) {
      char email[MSL];
      bool valEmail = FALSE;
      char *p;

      if (argument[0] == '\0') {
        do_function(ch, &do_help, "creation_email");
        strcpy(email, "");
        valEmail = TRUE;
        free_string(ch->pcdata->email);
        ch->pcdata->email = str_dup(email);
        return;
      }
      else {
        strcpy(email, argument);

        if (!str_cmp(email, "none"))
        valEmail = TRUE;

        for (p = email; *p != '\0'; p++) {
          if (*p == '@')
          valEmail = TRUE;
        }
      }
      /* A valid address? */
      if (!valEmail) {
        send_to_char("Invalid email address.  Try again.\n\r", ch);
        return;
      }

      /* set the address */
      free_string(ch->pcdata->email);
      ch->pcdata->email = str_dup(email);

      printf_to_char(ch, "Email address now set to: %s\n\n\r", ch->pcdata->email);

      if (ch->pcdata->creation_location != CR_ALL) {
        ch->pcdata->creation_location = CR_LASTNAME;
        creation(ch, "");
      }

      return;
    }
    
    CREATION(credit_lastname) {
      char lastname[MSL];
      char arg1[MSL];

      argument[0] = UPPER(argument[0]);

      /* Set the last name */
      remove_color(lastname, argument);
      free_string(ch->pcdata->last_name);

      argument = one_argument_nouncap(argument, arg1);
      ch->pcdata->last_name = str_dup(arg1);

      printf_to_char(ch, "Last name now set to: %s\n\n\r", ch->pcdata->last_name);

      if (ch->pcdata->creation_location != CR_ALL) {
        ch->pcdata->creation_location++;
        creation(ch, "");
      }

      return;
    }
    
    CREATION(credit_sex) {
      /* Can only choose male or female ! */
      if (argument[0] == '\0') {
        send_to_char("You may choose `BMale`x or `MFemale`x.\n\r", ch);
        return;
      }
      switch (argument[0]) {
      case 'm':
      case 'M':
        ch->sex = SEX_MALE;
        break;
      case 'f':
      case 'F':

        ch->sex = SEX_FEMALE;
        break;
      default:
        send_to_char("You may choose `BMale`x or `MFemale`x.\n\r", ch);
        return;
      }

      printf_to_char(ch, "Sex now set to: %s\n\n\r", (ch->sex == 0 ? "sexless" : ch->sex == 1 ? "Male" : "Female"));

      if (ch->sex == SEX_MALE)
      ch->pcdata->penis = 10;
      else
      ch->pcdata->penis = 0;

      if (IS_FLAG(ch->act, PLR_GM)) {
        ch->race = RACE_HUMAN;
        ch->pcdata->birth_year = 1990;
        ch->pcdata->birth_day = 1;
        ch->pcdata->birth_month = 1;
        ch->pcdata->height_feet = 6;
        finish_creation(ch);
        return;
      }

      if (ch->pcdata->creation_location != CR_ALL) {
        ch->pcdata->creation_location++;
        creation(ch, "");
      }

      return;
    }

    CREATION(credit_class) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      if (!IS_FLAG(ch->act, PLR_GUEST)) {
        ch->pcdata->dtrains = 0;
        ch->pcdata->strains = 0;
      }
      else {
        ch->pcdata->dtrains = 140;
        ch->pcdata->strains = 60;
      }

      if (!str_cmp(argument, "human")) {
        ch->pcdata->creation_location = CR_HUMAN;
        creation(ch, "");
      }
      else if (!str_cmp(argument, "monster")) {
        ch->pcdata->creation_location = CR_MONSTER;
        creation(ch, "");
      }
      else if (!str_cmp(argument, "demonborn")) {
        ch->pcdata->creation_location = CR_DEMONBORN;
        creation(ch, "");
      }
      else if (!str_cmp(argument, "demigod")) {
        ch->pcdata->creation_location = CR_DEMIGOD;
        creation(ch, "");
      }
      else if (!str_cmp(argument, "faeborn")) {
        ch->pcdata->creation_location = CR_FAEBORN;
        creation(ch, "");
      }
      else if (!str_cmp(argument, "angelborn")) {
        ch->pcdata->creation_location = CR_ANGELBORN;
        creation(ch, "");
      }
      else if (!str_cmp(argument, "innocent")) {
        ch->race = RACE_CIVILIAN;
        ch->pcdata->strains = 25;
        ch->pcdata->creation_location = CR_YEAR;
        creation(ch, "");
      }
      else {
        send_to_char("Valid choices are: innocent, human, monster, demonborn, faeborn, angelborn\n\r", ch);
        return;
      }
    }
    CREATION(credit_monster) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      send_to_char("Valid choices are: vampire, werewolf\n\r", ch);
      return;

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }
    
    CREATION(credit_special) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }
      return;
      if (!str_cmp(argument, "devourer")) {
        /*
        ch->race = RACE_DEVOURER;
        ch->skills[SKILL_REGEN] = 1;
        ch->skills[SKILL_STRENGTH] = 4;
        ch->skills[SKILL_DEXTERITY] = 1;
        ch->skills[SKILL_STAMINA] = 3;
        ch->skills[SKILL_RUNNING] = 1;
        ch->disciplines[DIS_THROWN] = 15;
        ch->disciplines[DIS_BRUTE] = 25;
        ch->disciplines[DIS_TOUGHNESS] = 20;
        */
      }
      else {
        send_to_char("Valid choices are: devourer\n\r", ch);
        return;
      }

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }
    CREATION(credit_human) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      if (!str_cmp(argument, "normal")) {
        ch->race = RACE_NORMAL;
        ch->skills[SKILL_DEXTERITY] = 3;
        ch->skills[SKILL_STAMINA] = 1;
        ch->skills[SKILL_RUNNING] = 1;
        ch->skills[SKILL_MARTIALART] = 3;
        ch->skills[SKILL_COMBATTRAINING] = 3;
        ch->disciplines[DIS_PISTOLS] = 25;
        ch->disciplines[DIS_KNIFE] = 15;
        ch->disciplines[DIS_BARMOR] = 20;
      }
      else {
        send_to_char("Valid choices are: normal, touched\n\r", ch);
        return;
      }

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }

    CREATION(credit_demonborn) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      send_to_char("Valid choices are: seduce, enrage, ragefueled, nightmares, wailing, none.\n\r", ch);
      return;

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }

    CREATION(credit_demigod) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      send_to_char("Valid choices are: stormcalling, oceanicaffinity, solarblessing, lunarblessing, deathtouch, warblessing, attraction, none.\n\r", ch);
      return;

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }

    CREATION(credit_shifter) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      send_to_char("Valid choices are: flier, runner, swimmer, fighter\n\r", ch);
      return;

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }

    CREATION(credit_faeborn) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      send_to_char("Valid choices are: luckchanging, mershifting, blooming, sirensong, senseweakness, none\n\r", ch);
      return;

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }

    CREATION(credit_angelborn) {
      for (int i = 0; i <= DIS_USED; i++) {
        ch->disciplines[i] = 0;
      }
      
      for (int i = 0; i <= SKILL_USED; i++) {
        ch->skills[i] = 0;
      }

      send_to_char("Valid choices are: protection, fleetfooted, healingtouch, none\n\r", ch);
      return;

      ch->pcdata->creation_location = CR_YEAR;
      creation(ch, "");
    }

    CREATION(credit_year) {
      tm *ptm;
      ptm = gmtime(&current_time);
      int val = atoi(argument);
      if (val < 100) {
        if (val <= 18 && ch->race != RACE_CIVILIAN) {
          send_to_char("Non innocent characters must be at least 18.\n\r", ch);
          return;
        }
        
        if (val <= 18 && ch->race != RACE_NORMAL && !is_vampire(ch) && ch->race != RACE_CIVILIAN) {
          send_to_char("Supernatural characters must be at least 18.\n\r", ch);
          return;
        }

        int year = ptm->tm_year + 1900;

        ch->pcdata->birth_year = year - val;
        ch->pcdata->birth_month = number_range(1, 12);
        ch->pcdata->birth_day = number_range(1, 28);

        int age = year - ch->pcdata->birth_year;
        if (ptm->tm_mon < ch->pcdata->birth_month)
        age--;
        if (ptm->tm_mon == ch->pcdata->birth_month && ptm->tm_mday < ch->pcdata->birth_day)
        age--;

        if (age > val)
        ch->pcdata->birth_year++;
        else if (age < val)
        ch->pcdata->birth_year--;

        if (get_age(ch) > val)
        ch->pcdata->birth_year++;
        else if (get_age(ch) < val)
        ch->pcdata->birth_year--;

        if (get_age(ch) > val)
        ch->pcdata->birth_year++;
        else if (get_age(ch) < val)
        ch->pcdata->birth_year--;

        send_to_char("Age set.\n\r", ch);
        if (is_vampire(ch))
        ch->pcdata->creation_location += 3;
        else
        ch->pcdata->creation_location += 6;
        creation(ch, "");
        return;
      }

      if (val > ptm->tm_year - 18 + 1900 && ch->race != RACE_CIVILIAN) {
        send_to_char("Non innocent characters must be at least 18.\n\r", ch);
        return;
      }
      if (val <= ptm->tm_year - 18 + 1900 && ch->race == RACE_CIVILIAN) {
        send_to_char("Innocents must be younger than 18.\n\r", ch);
        return;
      }
      if (val < 1900 - ch->exp / 1000) {
        send_to_char("You can't be that old.\n\r", ch);
        return;
      }
      ch->pcdata->birth_year = val;
      printf_to_char(ch, "Birth year now set to: %d\n\n\r", ch->pcdata->birth_year);
      ch->pcdata->creation_location++;
      creation(ch, "");
      return;
    }

    CREATION(credit_month) {
      if (atoi(argument) < 1 || atoi(argument) > 12) {
        printf_to_char(ch, "Month must be between 1 and 12\n\r");
        return;
      }
      ch->pcdata->birth_month = atoi(argument);
      printf_to_char(ch, "Birth month now set to: %d\n\n\r", ch->pcdata->birth_month);
      ch->pcdata->creation_location++;
      creation(ch, "");
      return;
    }

    CREATION(credit_day) {
      if (atoi(argument) < 1 || atoi(argument) > 31) {
        printf_to_char(ch, "day must be between 1 and 31\n\r");
        return;
      }
      ch->pcdata->birth_day = atoi(argument);
      printf_to_char(ch, "Birth day now set to: %d\n\n\r", ch->pcdata->birth_day);
      if (is_vampire(ch))
      ch->pcdata->creation_location++;
      else
      ch->pcdata->creation_location = CR_HEIGHTFEET;
      creation(ch, "");
      return;
    }

    bool change_feet(CHAR_DATA *ch, char argument[MSL]) {
      int minheight, maxheight;

      minheight = 4;
      maxheight = 6;

      if (atoi(argument) < minheight || atoi(argument) > maxheight) {
        printf_to_char(ch, "Choose a number between %d and %d.\n\r", minheight, maxheight);
        return FALSE;
      }

      return TRUE;
    }

    CREATION(credit_height_feet) {
      char arg1[MSL];
      argument = one_argument_nouncap(argument, arg1);

      if (change_feet(ch, arg1) == TRUE) {
        ch->pcdata->height_feet = atoi(arg1);
        printf_to_char(ch, "You are %d feet tall.\n\n\r", ch->pcdata->height_feet);
        ch->pcdata->creation_location++;
        creation(ch, "");
      }

      return;
    }

    bool change_inches(CHAR_DATA *ch, char argument[MSL]) {
      int minheight, maxheight;

      if (ch->pcdata->height_feet == 4) {minheight = 8;}
      else                              {minheight = 0;}

      if (ch->pcdata->height_feet == 6) {maxheight =  6;}
      else                              {maxheight = 11;}

      if (atoi(argument) < minheight || atoi(argument) > maxheight) {
        printf_to_char(ch, "Enter a number between %d and %d for inches.\n\r", minheight, maxheight);
        return FALSE;
      }

      return TRUE;
    }

    CREATION(credit_height_inches) {
      char arg1[MSL];
      argument = one_argument_nouncap(argument, arg1);

      if (change_inches(ch, arg1) == TRUE) {
        ch->pcdata->height_inches = atoi(arg1);
        printf_to_char(ch, "You are %d'%d tall.\n\n\r", ch->pcdata->height_feet, ch->pcdata->height_inches);
        ch->pcdata->creation_location++;
        creation(ch, "");
      }

      return;
    }
    CREATION(credit_bust) {

      if (atoi(argument) < 1 || atoi(argument) > 12) {
        send_to_char("Please make a selection between 1 and 12.\n\r", ch);
        return;
      }
      ch->pcdata->bust = atoi(argument);
      send_to_char("Bust size set.\n\r", ch);
      ch->pcdata->creation_location++;
      creation(ch, "");

      return;
    }
    CREATION(credit_intro) {
      char lastname[MSL];

      /* Set the last name */
      remove_color(lastname, argument);
      lastname[0] += 'A' - 'a';
      free_string(ch->pcdata->intro_desc);
      ch->pcdata->intro_desc = str_dup(lastname);
      printf_to_char(ch, "You are %s.\n\n\r", ch->pcdata->intro_desc);

      if (ch->race == RACE_CIVILIAN || IS_FLAG(ch->act, PLR_GUEST))
      ch->pcdata->creation_location += 2;
      else
      ch->pcdata->creation_location += 1;
      creation(ch, "");

      return;
    }

    CREATION(credit_hair) {
      char lastname[MSL];

      /* Set the last name */
      remove_color(lastname, argument);
      //    lastname[0] += 'A' - 'a';
      free_string(ch->pcdata->hair);
      ch->pcdata->hair = str_dup(lastname);
      printf_to_char(ch, "You have %s.\n\n\r", ch->pcdata->hair);

      ch->pcdata->creation_location += 1;
      creation(ch, "");

      return;
    }

    CREATION(credit_eyes) {
      char lastname[MSL];

      /* Set the last name */
      remove_color(lastname, argument);
      //    lastname[0] += 'A' - 'a';
      free_string(ch->pcdata->eyes);
      ch->pcdata->eyes = str_dup(lastname);
      printf_to_char(ch, "You have %s.\n\n\r", ch->pcdata->eyes);

      ch->pcdata->creation_location += 1;
      creation(ch, "");

      return;
    }

    CREATION(credit_skin) {
      char lastname[MSL];

      /* Set the last name */
      remove_color(lastname, argument);
      //    lastname[0] += 'A' - 'a';
      free_string(ch->pcdata->skin);
      ch->pcdata->skin = str_dup(lastname);
      printf_to_char(ch, "You have %s.\n\n\r", ch->pcdata->skin);

      if (ch->sex == SEX_FEMALE)
      ch->pcdata->creation_location += 1;
      else
      ch->pcdata->creation_location += 2;

      creation(ch, "");

      return;
    }

    CREATION(credit_desc) {
      /* Set the last name */
      if (!str_cmp(argument, "done")) {
        if (is_vampire(ch))
        ch->pcdata->creation_location++;
        else
        ch->pcdata->creation_location = CR_CUSTOMISE;
        creation(ch, "");
      }
      else if (!str_cmp(argument, "edit")) {
        string_append(ch, &ch->description);
      }
      else
      send_to_char("Please enter either 'done' or 'edit'\n\r", ch);

      return;
    }

    CREATION(credit_sireyear) {
      tm *ptm;
      ptm = gmtime(&current_time);
      int val = atoi(argument);

      if (val < 100) {
        if (val < 10) {
          send_to_char("Your character must appear to be at least 10.\n\r", ch);
          return;
        }

        ch->pcdata->sire_year = ch->pcdata->birth_year + val;
        ch->pcdata->sire_month = number_range(1, 12);
        ch->pcdata->sire_day = number_range(1, 28);

        int age = ch->pcdata->sire_year - ch->pcdata->birth_year;
        if (ch->pcdata->sire_month > ch->pcdata->birth_month)
        age--;
        if (ch->pcdata->sire_month == ch->pcdata->birth_month && ch->pcdata->sire_day > ch->pcdata->birth_day)
        age--;

        if (age > val)
        ch->pcdata->sire_year++;
        else if (age < val)
        ch->pcdata->sire_year--;

        send_to_char("Sired age set.\n\r", ch);
        ch->pcdata->creation_location += 3;
        creation(ch, "");
        return;
      }
      if (val > ptm->tm_year + 1900) {
        send_to_char("You can't be sired in the future.\n\r", ch);
        return;
      }
      if (val < ch->pcdata->birth_year + 5) {
        send_to_char("You can't be sired that young.\n\r", ch);
        return;
      }
      ch->pcdata->sire_year = val;
      printf_to_char(ch, "Sire year now set to: %d\n\n\r", ch->pcdata->sire_year);
      ch->pcdata->creation_location++;
      creation(ch, "");
      return;
    }

    CREATION(credit_siremonth) {
      if (atoi(argument) < 1 || atoi(argument) > 12) {
        printf_to_char(ch, "Month must be between 1 and 12\n\r");
        return;
      }
      ch->pcdata->sire_month = atoi(argument);
      printf_to_char(ch, "Sire month now set to: %d\n\n\r", ch->pcdata->sire_month);
      ch->pcdata->creation_location++;
      creation(ch, "");
      return;
    }

    CREATION(credit_sireday) {
      if (atoi(argument) < 1 || atoi(argument) > 31) {
        printf_to_char(ch, "day must be between 1 and 31\n\r");
        return;
      }
      ch->pcdata->sire_day = atoi(argument);
      printf_to_char(ch, "Sire day now set to: %d\n\n\r", ch->pcdata->sire_day);
      ch->pcdata->creation_location++;
      creation(ch, "");
      return;
    }

    CREATION(credit_customise) {
      if (!str_cmp(argument, "continue")) {
        int i;
        for (i = 0; i <= DIS_USED; i++) {
          ch->disciplines[i] = 0;
        }
        ch->pcdata->dtrains = 60;

        if (IS_FLAG(ch->act, PLR_GUEST))
        ch->pcdata->dtrains += 140;

        for (i = 0; i <= SKILL_USED; i++) {
          ch->skills[i] = 0;
        }
        ch->pcdata->strains = 25;

        if (IS_FLAG(ch->act, PLR_GUEST))
        ch->pcdata->strains += 60;

        for (int x = 0; x < 5; x++) {
          ch->pcdata->ability_trees[x] = 0;
        }

        ch->pcdata->creation_location++;
        creation(ch, "");
      }

      return;
    }

    CREATION(credit_disciplines) {
      char arg1[MSL];
      char arg2[MSL];
      char arg3[MSL];
      char buf[MSL];
      int point = -1;
      int val = 0, i;
      if (!str_cmp(argument, "all") || !str_cmp(argument, "disciplines all")) {
        do_function(ch, &do_disciplines, "creation");
        return;
      }
      argument = one_argument_nouncap(argument, arg1);
      if (!str_cmp(arg1, "done")) {
        ch->pcdata->creation_location++;
        creation(ch, "");
        return;
      }
      if (!str_cmp(arg1, "reset")) {
        for (i = 0; i <= DIS_USED; i++) {
          ch->disciplines[i] = 0;
        }
        ch->pcdata->dtrains = 60;
        if (IS_FLAG(ch->act, PLR_GUEST))
        ch->pcdata->dtrains += 140;
        send_to_char("Reset.\n\r", ch);
        return;
      }
      for (i = 0; i < DIS_USED; i++) {
        if (!str_cmp(arg1, discipline_table[i].name)) {
          point = i;
          val = atoi(argument);
          if (argument[0] == '\0') {
            do_function(ch, &do_help, discipline_table[point].name);
            return;
          }
        }
      }
      argument = one_argument_nouncap(argument, arg2);
      sprintf(buf, "%s %s", arg1, arg2);
      for (i = 0; i < DIS_USED; i++) {
        if (!str_cmp(buf, discipline_table[i].name)) {
          point = i;
          val = atoi(argument);
          if (argument[0] == '\0') {
            do_function(ch, &do_help, discipline_table[point].name);
            return;
          }
        }
      }
      argument = one_argument_nouncap(argument, arg3);
      sprintf(buf, "%s %s", buf, arg3);
      for (i = 0; i < DIS_USED; i++) {
        if (!str_cmp(buf, discipline_table[i].name)) {
          point = i;
          val = atoi(argument);
          if (argument[0] == '\0') {
            do_function(ch, &do_help, discipline_table[point].name);
            return;
          }
        }
      }
      if (point == -1) {
        send_to_char("No such discipline.\n\r", ch);
        do_function(ch, &do_disciplines, "all");
        send_to_char("Type (discipline name) (new value) to set your disciplines and 'done' when you are finished.\n\r", ch);
        return;
      }
      if (discipline_table[point].pc < 1) {
        send_to_char("PCs can't train that discipline.\n\r", ch);
        return;
      }
      if (val >
          ch->disciplines[discipline_table[point].vnum] + ch->pcdata->dtrains) {
        printf_to_char(ch, "You only have %d points left to distribute.\n\r", ch->pcdata->dtrains);
        return;
      }
      if (val < 0) {
        send_to_char("Positive numbers only.\n\r", ch);
        return;
      }
      if (!IS_FLAG(ch->act, PLR_GUEST) && val > 30) {
        send_to_char("Disciplines can only be set to a max of thirty in chargen.\n\r", ch);
        return;
      }
      if (discipline_table[point].range == -1 && val + shield_total_creation(ch) > 30 && !IS_FLAG(ch->act, PLR_GUEST)) {
        send_to_char("Disciplines can only be set to a max of thirty in chargen.\n\r", ch);
        return;
      }
      if ((discipline_table[point].vnum == DIS_STRIKING || discipline_table[point].vnum == DIS_GRAPPLE) && val > 20 && !is_vampire(ch)) {
        send_to_char("20 is the maximum for that discipline.\n\r", ch);
        return;
      }
      if ((discipline_table[point].vnum == DIS_STRIKING || discipline_table[point].vnum == DIS_GRAPPLE) && val > 10 && !is_super(ch)) {
        send_to_char("You'd need to be supernatural for that.\n\r", ch);
        return;
      }
      if (discipline_table[point].vnum == DIS_TOUGHNESS && !is_super(ch)) {
        send_to_char("You'd need to be supernatural for that.\n\r", ch);
        return;
      }
      if ((discipline_table[point].vnum == DIS_KNIFE || discipline_table[point].vnum == DIS_LONGBLADE || discipline_table[point].vnum == DIS_BLUNT) && val > 25 && !is_super(ch)) {
        send_to_char("You'd need to be supernatural for that.\n\r", ch);
        return;
      }
      if (discipline_table[point].vnum == DIS_BARMOR && ch->disciplines[DIS_MARMOR] + val > 30) {
        send_to_char("You can't raise your armor that high.\n\r", ch);
        return;
      }
      if (discipline_table[point].vnum == DIS_MARMOR && ch->disciplines[DIS_BARMOR] + val > 30) {
        send_to_char("You can't raise your armor that high.\n\r", ch);
        return;
      }

      if (discipline_table[point].vnum == DIS_TOUGHNESS && !is_vampire(ch) && val > UMAX(ch->disciplines[DIS_BARMOR], ch->disciplines[DIS_MARMOR])) {
        send_to_char("You can't raise your toughness higher than your highest armor discipline.\n\r", ch);
        return;
      }

      ch->pcdata->dtrains =
      ch->pcdata->dtrains - val + ch->disciplines[discipline_table[point].vnum];
      ch->disciplines[discipline_table[point].vnum] = val;
      send_to_char("Discipline set.\n\r", ch);
      printf_to_char(ch, "You have %d points left to distribute.\n\r", ch->pcdata->dtrains);

      return;
    }

    int skillpoint(int val) {
      switch (val) {
      case -1:
        return -1;
        break;
      case 0:
        return 0;
        break;
      case 1:
        return 1;
        break;
      case 2:
        return 3;
        break;
      case 3:
        return 6;
        break;
      case 4:
        return 10;
        break;
      case 5:
        return 15;
        break;
      case 6:
        return 21;
        break;
      }
      return 0;
    }

    CREATION(credit_skills) {
      char arg1[MSL];
      char arg2[MSL];
      char arg3[MSL];
      char buf[MSL];
      int point = -1;
      int val = 0, i;
      argument = one_argument_nouncap(argument, arg1);
      if (!str_cmp(arg1, "done")) {
        if (ch->race == RACE_CIVILIAN) {
          finish_creation(ch);
          return;
        }
        finish_creation(ch);

        return;
      }
      if (!str_cmp(arg1, "reset")) {
        for (i = 0; i <= SKILL_USED; i++) {
          ch->skills[i] = 0;
        }
        ch->pcdata->strains = 25;
        if (IS_FLAG(ch->act, PLR_GUEST))
        ch->pcdata->strains += 60;
        for (int x = 0; x < 5; x++) {
          ch->pcdata->ability_trees[x] = 0;
        }

        send_to_char("Reset.\n\r", ch);
        return;
      }
      for (i = 0; i < SKILL_USED; i++) {
        if (!str_cmp(arg1, skill_table[i].name)) {
          point = i;
          val = atoi(argument);
          if (argument[0] == '\0') {
            do_function(ch, &do_help, skill_table[point].name);
            return;
          }
        }
      }
      argument = one_argument_nouncap(argument, arg2);
      sprintf(buf, "%s %s", arg1, arg2);
      for (i = 0; i < SKILL_USED; i++) {
        if (!str_cmp(buf, skill_table[i].name)) {
          point = i;
          val = atoi(argument);
          if (argument[0] == '\0') {
            do_function(ch, &do_help, skill_table[point].name);
            return;
          }
        }
      }
      argument = one_argument_nouncap(argument, arg3);
      sprintf(buf, "%s %s", buf, arg3);
      for (i = 0; i < SKILL_USED; i++) {
        if (!str_cmp(buf, skill_table[i].name)) {
          point = i;
          val = atoi(argument);
          if (argument[0] == '\0') {
            do_function(ch, &do_help, skill_table[point].name);
            return;
          }
        }
      }
      if (skill_table[point].vnum == SKILL_AGELESS && is_vampire(ch)) {
        send_to_char("You're already immortal.\n\r", ch);
        return;
      }
      if (skilltype(skill_table[point].vnum) == STYPE_ORIGIN && skillcount(ch, STYPE_ORIGIN) > 0) {
        send_to_char("You already have an origin.\n\r", ch);
        return;
      }
      if (skilltype(skill_table[point].vnum) == STYPE_SUPERNATURAL && ch->race == RACE_CIVILIAN) {
        send_to_char("Innocents don't have supernatural powers.\n\r", ch);
        return;
      }
      if (skilltype(skill_table[point].vnum) == STYPE_SABILITIES && ch->race == RACE_CIVILIAN) {
        send_to_char("Innocents don't have supernatural powers.\n\r", ch);
        return;
      }

      if (point == -1) {
        send_to_char("No such skill.\n\r", ch);
        do_function(ch, &do_skills, "creation");
        send_to_char("Type (stat name) (new value) to set your disciplines and 'done' when you are finished.\n\r", ch);
        return;
      }
      bool found = FALSE;
      for (i = 0; i < 6; i++) {
        if (skill_table[point].levels[i] == val || val == 0)
        found = TRUE;
      }
      if (found == FALSE) {
        printf_to_char(
        ch, "That isn't a valid value for that stat, valid values are: 0\n\r");
        for (i = 0; i < 6; i++) {
          if (skill_table[point].levels[i] != 0)
          printf_to_char(ch, ", %d", skill_table[point].levels[i]);
        }
        printf_to_char(ch, "\n\r");
        return;
      }
      int tpoints =
      skillpoint(val) - skillpoint(ch->skills[skill_table[point].vnum]);

      int x;
      if (skilltype(skill_table[point].vnum) == STYPE_ABILITIES || skilltype(skill_table[point].vnum) == STYPE_SABILITIES) {
        for (x = 0; x < 5 && ch->pcdata->ability_trees[x] != 0 && ch->pcdata->ability_trees[x] != skill_table[point].vnum;
        x++) {
        }
        tpoints = (x + 1) * (val - ch->skills[skill_table[point].vnum]);
      }

      if (!has_requirements(ch, skill_table[point].vnum, tpoints, FALSE)) {
        send_to_char("You don't meet the requirements for that.\n\r", ch);
        return;
      }
      if (tpoints >
          neweffective_trains(ch, skilltype(skill_table[point].vnum), tpoints)) {
        printf_to_char(ch, "You only have %d points left to distribute.\n\r", ch->pcdata->strains);
        return;
      }

      ch->pcdata->strains = ch->pcdata->strains - tpoints;
      ch->skills[skill_table[point].vnum] = val;

      if (skilltype(skill_table[point].vnum) == STYPE_ABILITIES || skilltype(skill_table[point].vnum) == STYPE_SABILITIES) {
        bool found = FALSE;
        for (x = 0; x < 5; x++) {
          if (ch->pcdata->ability_trees[x] == skill_table[point].vnum)
          found = TRUE;
        }
        if (found == FALSE) {
          for (x = 0; x < 5 && ch->pcdata->ability_trees[x] != 0 && ch->pcdata->ability_trees[x] != skill_table[point].vnum;
          x++) {
          }
          ch->pcdata->ability_trees[x] = skill_table[point].vnum;
        }
      }
      send_to_char("Stat set.\n\r", ch);
      printf_to_char(ch, "You have %d points left to distribute.\n\r", ch->pcdata->strains);

      return;
    }

    CREATION(credit_factions) {
      char arg1[MSL];

      if (!str_cmp(argument, "list")) {
        send_to_char("`g  Player Factions`x\n\r", ch);
        for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
        it != FacVect.end(); ++it) {
          if (safe_strlen((*it)->name) > 3 && (*it)->closed == 0 && (*it)->stasis == 0) {
            printf_to_char(ch, "%s.\n\r", (*it)->name);
          }
        }
        send_to_char("Use info (name) for more information.\n\r", ch);
        return;
      }
      if (!str_prefix("info", argument)) {
        argument = one_argument_nouncap(argument, arg1);
        FACTION_TYPE *disp = clan_lookup_name(argument);
        if (disp == NULL) {
          send_to_char("No such faction, use list to see all options.\n\r", ch);
          return;
        }
        printf_to_char(
        ch, "`gName`W:`x %s\n`gSymbol`W:`x %s\n`gLeader`W:`x %s\n\n%s\n\r", disp->name, disp->symbol, disp->leader, disp->description);

        return;
      }
      if (!str_cmp(argument, "none")) {
        ch->faction = 0;
        ch->pcdata->creation_location++;
        creation(ch, "");
        return;
      }

      FACTION_TYPE *fac = clan_lookup_name(argument);
      if (fac == NULL) {
        send_to_char("No such faction.\n\r", ch);
        return;
      }
      if (fac->closed == 1) {
        send_to_char("This faction has been set as unjoinable from creation.\n\r", ch);
        return;
      }
      if (fac->stasis == 1) {
        send_to_char("This faction is not currently active.\n\r", ch);
        return;
      }

      if (ch->desc->account != NULL && ch->desc->account->factiontime != 0 && ch->desc->account->factiontime > current_time) {
        send_to_char("You rolled a faction character too recently, you will have to join up ICly in game.\n\r", ch);
        return;
      }
      if (ch->pcdata->account != NULL && ch->pcdata->account->factiontime != 0 && ch->pcdata->account->factiontime > current_time) {
        send_to_char("You rolled a faction character too recently, you will have to join up ICly in game...\n\r", ch);
        return;
      }

      if (get_true_age(ch) < 18) {
        send_to_char("You're not old enough to join a faction.\n\r", ch);
        return;
      }
      ch->faction = fac->vnum;
      // 	join_to_clan(ch, fac->vnum);
      if (ch->desc->account != NULL)
      ch->desc->account->factiontime = (current_time + (3600 * 24 * 30));
      if (ch->pcdata->account != NULL)
      ch->pcdata->account->factiontime = (current_time + (3600 * 24 * 30));
      ch->pcdata->creation_location++;
      creation(ch, "");

      return;
    }

    CREATION(credit_adult) {
      if (!str_cmp(argument, "continue")) {
        ch->pcdata->creation_location = CR_EMAIL;
        creation(ch, "");

      }
      else if (!str_cmp(argument, "quit")) {
        char strsave[MSL];

        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
        wiznet("$N turns $Mself into line noise.", ch, NULL, 0, 0, 0);
        save_char_obj(ch, FALSE, FALSE);
        real_quit(ch);
        unlink(strsave);
      }
      else {
        send_to_char("Please select 'continue' if you assent, or 'quit' if you do not.\n\r", ch);
        return;
      }
    }

    /******************************************************************
    * Name: credit_choice_show
    * Parameters:
    *      CHAR_DATA *ch - character currently in creation
    *      char *argument - class being selected
    * Returns:
    *      void
    *
    * Purpose:  Allows the character in creation to decide if
    *	     they want long or short creation.
    * Author: Cameron Matthews-Dickson (Scaelorn)
    *****************************************************************/
    CREATION(credit_choice_show) {
      send_to_char("Welcome to BuffyMud Creation, from here you will build the character\n\r", ch);
      send_to_char("in which you will experience the buffyverse, remember that you cannot\n\r", ch);
      send_to_char("create show characters, or other characters from existing fictional sources\n\r", ch);
      send_to_char("beyond that the only limit is your imagination! You can experience anything\n\r", ch);
      send_to_char("the buffyverse has to offer, from normal humans, to witches, vampires, demons\n\r", ch);
      send_to_char("and even slayers!\n\r", ch);
      send_to_char("Just type in `Ccontinue`x when you are ready to start.\n\r", ch);
      return;
    }

    /******************************************************************
    * Name: credit_help
    * Parameters:
    *      CHAR_DATA *ch - character currently in creation
    *      char *argument - subject for which help is required
    * Returns:
    *      void
    *
    * Purpose:  Allows the character in creation to get help on the
    *	commands and choices available to them in creation
    * Author: Cameron Matthews-Dickson (Scaelorn)
    *****************************************************************/
    CREATION(credit_help) {
      do_function(ch, &do_help, argument);
      return;

      int x, col = 0;
      bool found = FALSE;
      char name[MIL], buf[MSL], buf1[50];

      one_argument(argument, name);

      /* If no subject specified ask for a subject */
      if (name[0] == '\0') {
        send_to_char("What would you like to know more about?\n\rFor a list of creation-related topics use `Rhelp list`x\n\rSyntax: help <topic>\n\r", ch);
        return;
      }

      /* If they ask for a list, list creation commands */
      if (!str_cmp(name, "list")) {
        buf[0] = '\0';

        /* Go through all the creation commands */
        for (x = 0; credit_table[x].comm != NULL; x++) {
          sprintf(buf1, "%-19.18s", credit_table[x].comm);
          strcat(buf, buf1);
          if (++col % 3 == 0)
          strcat(buf, "\n\r");
        }
        send_to_char(buf, ch);
        send_to_char("\n\n\r", ch);
        return;
      }

      /* Check for the subject in the list of creation commands */
      for (x = 0; credit_table[x].comm != NULL; x++) {
        /*
        * If there is a help associated with the subject then
        * display it.
        */
        if (credit_table[x].help != NULL) {
          if (LOWER(name[0]) == LOWER(credit_table[x].comm[0]) && !str_prefix(name, credit_table[x].comm)) {
            do_function(ch, &do_help, credit_table[x].help);
            found = TRUE;
            break;
          }
        }
      }

      /* if not found in creation commands then check the normal helps */
      if (!found)
      do_function(ch, &do_help, name);

      send_to_char("\n\r\n\r", ch);
      return;
    }

    void start_genesis(CHAR_DATA *ch) {
      char buf[MSL];
      ch->race = RACE_VISITOR;
      ch->version = CURR_VERSION;
      SET_FLAG(ch->comm, COMM_NOGUIDE);
      ch->pcdata->version_player = 2;
      sprintf(buf, "%s %s", ch->pcdata->last_name, "the Newbie");
      set_whotitle(ch, buf);
      do_function(ch, &do_prompt, "starter");
      if (IS_FLAG(ch->comm, COMM_NONEWBIE))
      REMOVE_FLAG(ch->comm, COMM_NONEWBIE);
      if (IS_FLAG(ch->comm, COMM_NOGUIDE))
      REMOVE_FLAG(ch->comm, COMM_NOGUIDE);
      ch->pcdata->height_inches = 7;
      ch->pcdata->height_feet = 5;
      ch->pcdata->birth_year = 1980;
      ch->pcdata->birth_month = 1;
      ch->pcdata->birth_day = 1;
      ch->pcdata->lfcount = 1;
      ch->pcdata->lftotal = 100;

      if (!IS_FLAG(ch->comm, COMM_PROMPT))
      SET_FLAG(ch->comm, COMM_PROMPT);

      if (!IS_FLAG(ch->comm, COMM_STORY))
      SET_FLAG(ch->comm, COMM_STORY);

      if (IS_FLAG(ch->comm, COMM_CONSENT))
      REMOVE_FLAG(ch->comm, COMM_CONSENT);

      if (!IS_FLAG(ch->comm, COMM_SUBDUE))
      SET_FLAG(ch->comm, COMM_SUBDUE);
      ch->desc->pEdit = NULL;
      ch->desc->editor = 0;
      ch->pcdata->create_date = current_time;
      ch->pcdata->last_paid = current_time;

      ch->level = 1;
      char_list.push_front(ch);
      ch->desc->connected = CON_PLAYING;
      ch->sex = SEX_MALE;
      free_string(ch->pcdata->intro_desc);
      ch->pcdata->intro_desc = str_dup("A man");

      ch->pcdata->true_level = ch->level;

      page_to_char(help_story, ch);
    }

    int max_pc_tier(void) {
      int max = 0;
      for (vector<FACTION_TYPE *>::iterator it = FacVect.begin();
      it != FacVect.end(); ++it) {
        if ((*it)->vnum == 0)
        continue;
        for (int i = 0; i < 100; i++) {
          if ((*it)->member_tier[i] > max)
          max = (*it)->member_tier[i];
        }
      }
      return max;
    }

    int charcost(CHAR_DATA *ch) {
      int tier = get_tier(ch);
      int val = 0;
      if (tier == 2)
      val = 25000;
      if (tier == 3)
      val = 50000;
      if (tier == 4)
      val = 100000;
      if (tier == 5)
      val = 150000;

      if(is_ffamily(ch))
      {
        val += 5000;
      }

      return val;
    }

    bool readychar(CHAR_DATA *ch) {
      if (!IS_FLAG(ch->act, PLR_GUEST) && !IS_FLAG(ch->act, PLR_GM)) {
        if (available_karma(ch) > 0) {
          if (charcost(ch) > available_karma(ch) + available_pkarma(ch)) {
            return FALSE;
          }
        }
        else {
          if (charcost(ch) > 0) {
            return FALSE;
          }
        }
        if (get_tier(ch) >= 3 && get_tier(ch) <= 5 && safe_strlen(ch->pcdata->account->lasttierchar) > 1 && str_cmp(ch->name, ch->pcdata->account->lasttierchar) && ch->pcdata->account->hightiercooldown > current_time)
        return FALSE;
      }
      else {
        if (get_tier(ch) > ch->pcdata->guest_tier)
        return FALSE;
      }
      if (guestmonster(ch) && ch->race != RACE_LANDMONSTER && ch->race != RACE_SEAMONSTER)
      return FALSE;

      if (!can_hair(ch) || !can_eyes(ch))
      return FALSE;

      if (is_name("NewCharacter", ch->name))
      return FALSE;
      char buf[MSL];
      sprintf(buf, "%d", ch->pcdata->height_inches);
      if (!guestmonster(ch) && !change_inches(ch, buf)) {
        return FALSE;
      }

      //Extra check, also set in min_aage and min_age functions, and race_table - Disco
      if (get_age(ch)      < 18) {return FALSE;}
      if (get_true_age(ch) < 18) {return FALSE;}
      
      //regular age checks
      if (get_true_age(ch) < min_age(ch))  {return FALSE;}
      if (get_true_age(ch) > max_age(ch))  {return FALSE;}
      if (get_age(ch)      < min_aage(ch)) {return FALSE;}
      if (get_age(ch)      > max_aage(ch)) {return FALSE;}

      if (ch->pcdata->eyes_genetic == 0) {return FALSE;}
      if (ch->pcdata->hair_genetic == 0) {return FALSE;}

      return TRUE;
    }

    void genesis_display(CHAR_DATA *ch) {
      char *const month_names[] = {"January",   "February", "March",    "April", "May",       "June",     "July",     "August", "September", "October",  "November", "December"};
      if (ch->pcdata->gen_stage == 0) {
        send_to_char("`gRoster list`x or `gcreate`x to proceed.\n\r", ch);
      }
      if (ch->pcdata->gen_stage == 1) {
        if (IS_FLAG(ch->act, PLR_GUEST))
        printf_to_char(ch, "\n`WGuest Tier 1-%d`x\n\r", ch->pcdata->guest_tier);
        if (is_name("NewCharacter", ch->name))
        printf_to_char(ch, "\n`rName:`x %s\n\r", ch->name);
        else
        printf_to_char(ch, "\nName: %s\n\r", ch->name);
        if (safe_strlen(ch->pcdata->last_name) < 2)
        send_to_char("`r", ch);
        printf_to_char(ch, "Lastname:`x %s\n\r", ch->pcdata->last_name);
        printf_to_char(ch, "Email:`x %s\n\r", ch->pcdata->email);
        printf_to_char(ch, "Gender: %s\n\r", (ch->sex == SEX_MALE) ? "Male" : "Female");
        if (get_tier(ch) >= 3 && get_tier(ch) <= 5 && safe_strlen(ch->pcdata->account->lasttierchar) > 1 && str_cmp(ch->name, ch->pcdata->account->lasttierchar) && ch->pcdata->account->hightiercooldown > current_time) {
          printf_to_char(ch, "`RArchetype`x: %s\n\r", race_table[ch->race].who_name);
        }
        else {
          printf_to_char(ch, "Archetype: %s\n\r", race_table[ch->race].who_name);
        }
        printf_to_char(ch, "Modifier: %s\n\r", modifier_table[ch->modifier].name);
        printf_to_char(
        ch, "`xBirthday:`x %d %d %d (%d%s of %s %d)[yyyy mm dd]\n\r", ch->pcdata->birth_year, ch->pcdata->birth_month, ch->pcdata->birth_day, ch->pcdata->birth_day, datesuffix(ch->pcdata->birth_day), month_names[ch->pcdata->birth_month - 1], ch->pcdata->birth_year);

        if (get_true_age(ch) < min_age(ch) || get_true_age(ch) > max_age(ch) || get_age(ch) < min_aage(ch) || get_age(ch) > max_aage(ch))
        send_to_char("`r", ch);
        else
        send_to_char("`g", ch);
        printf_to_char(ch, "Resulting Age`x:`x %d\n\r", get_real_age(ch));
        if (is_vampire(ch)) {
          printf_to_char(
          ch, "Deathday: %d %d %d (%d%s of %s %d)[yyyy mm dd]\n\r", ch->pcdata->sire_year, ch->pcdata->sire_month, ch->pcdata->sire_day, ch->pcdata->sire_day, datesuffix(ch->pcdata->sire_day), month_names[ch->pcdata->sire_month - 1], ch->pcdata->sire_year);
        }
        if (get_skill(ch, SKILL_AGELESS) > 0) {
          if (get_age(ch) < min_aage(ch) || get_age(ch) > max_aage(ch))
          send_to_char("`r", ch);
          printf_to_char(ch, "Ageless`x: %d\n\r", get_age(ch));
        }
        if (ch->pcdata->height_feet > 6 || ch->pcdata->height_feet < 3)
        send_to_char("`r", ch);
        printf_to_char(ch, "Feet:`x %d\n\r", ch->pcdata->height_feet);
        if (ch->pcdata->height_inches < 0 || ch->pcdata->height_inches > 11)
        send_to_char("`r", ch);
        printf_to_char(ch, "Inches:`x %d\n\r", ch->pcdata->height_inches);

        if (ch->pcdata->hair_genetic == 0)
        send_to_char("`r", ch);
        if (can_hair(ch))
        printf_to_char(ch, "Haircolor:`x %s\n\r", get_natural_hair(ch));
        else
        printf_to_char(ch, "`rHaircolor:`x %s\n\r", get_natural_hair(ch));
        if (safe_strlen(ch->pcdata->hair_dyed) > 3)
        printf_to_char(ch, "Hairdye: %s\n\r", ch->pcdata->hair_dyed);
        else
        printf_to_char(ch, "Hairdye: %s\n\r", ch->pcdata->hair_dyed);
        if (ch->pcdata->eyes_genetic == 0)
        send_to_char("`r", ch);
        if (can_eyes(ch))
        printf_to_char(ch, "Eyecolor:`x %s\n\r", get_natural_eyes(ch));
        else
        printf_to_char(ch, "`rEyecolor:`x %s\n\r", get_natural_eyes(ch));
        if (safe_strlen(ch->pcdata->skin) < 2)
        send_to_char("`r", ch);
        printf_to_char(ch, "Skin:`x %s\n\r", ch->pcdata->skin);

        if (ch->sex == SEX_FEMALE)
        printf_to_char(ch, "Bust: %s\n\r", cup_size[get_bust(ch)]);
        printf_to_char(ch, "Intro: %s\n\r", ch->pcdata->intro_desc);
        printf_to_char(ch, "Width(of buffer): %d\n\r", ch->linewidth);
        printf_to_char(ch, "Length(of buffer): %d\n\r", ch->lines);

        if (!IS_FLAG(ch->act, PLR_GUEST)) {
          if (charcost(ch) > available_karma(ch) + available_pkarma(ch))
          printf_to_char(ch, "This character would cost `R%d`x karma to create, you have %d karma.\n\r", charcost(ch), available_karma(ch) + available_pkarma(ch));
          else
          printf_to_char(ch, "\nThis character would cost %d karma to create, you have %d karma.\n\r", charcost(ch), available_karma(ch) + available_pkarma(ch));
        }
        if (readychar(ch))
        printf_to_char(ch, "This character is `Gready`x to be created. Type `WDone`x to proceed.\n\r");
        else
        printf_to_char(ch, "This character is `rnot yet ready`x to be created.\n\r");
        send_to_char("Change (field) (thing to change it to) or help (field)\n\r", ch);
      }
    }

    int extract_feet(CHAR_DATA *ch) {
      char *arg;
      arg = str_dup(median_height_for_sex(ch));
      char buf[10];
      sprintf(buf, "%c", arg[0]);
      return atoi(buf);
    }

    int extract_inches(CHAR_DATA *ch) {
      char *arg;
      arg = str_dup(median_height_for_sex(ch));
      char buf[10];
      sprintf(buf, "%c%c", arg[2], arg[3]);
      if (is_number(buf))
      return atoi(buf);
      sprintf(buf, "%c", arg[2]);
      return atoi(buf);
    }

    _DOFUN(do_enter) {

      if (ch->in_room->vnum != 56) {
        send_to_char("You must be in the last room of chargen to use this.\n\r", ch);
        return;
      }
      /*
      if(ch->desc->account != NULL && IS_FLAG(ch->desc->account->flags, ACCOUNT_SHADOWBAN))
      {
        send_to_char("You must be in the last room of chargen to use this.\n\r", ch);
        return;
      }
      */

      if(ch->desc->account != NULL && IS_FLAG(ch->desc->account->flags, ACCOUNT_GRIEFER))
      {
        send_to_char("You must be in the last room of chargen to use this.\n\r", ch);
        return;
      }

      if (largeweapons(ch)) {
        send_to_char("You can't enter with visible weapons.\n\r", ch);
        return;
      }
      if(is_ffamily(ch))
      {
        if(strlen(ch->pcdata->ff_knowledge) < 2 || strlen(ch->pcdata->ff_secret) < 2)
        {
          send_to_char("You must have a family knowledge and secret to enter the game. Use family knowledge or family secrets to set these.\n\r", ch);
          return;
        }

      }
      act("$n enters the game.", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);

      char_to_room(ch, get_room_index(3609));
      act("$n arrives.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You arrive in Haven.\n\r", ch);
      if(crisis_prologue == 1)
      {
        char_from_room(ch);
        char_to_room(ch, get_room_index(15050));
        send_to_char("You rush into the lodge, the sound of piercing screams echoing behind you. You struggle to remember what you were just doing.\n\r", ch);
        act("$n arrives.", ch, NULL, NULL, TO_ROOM);
      }


      if(is_ffamily(ch))
      {
        for (DescList::iterator it = descriptor_list.begin();
        it != descriptor_list.end(); ++it) {
          DESCRIPTOR_DATA *d = *it;
          CHAR_DATA *to;
          if (d->character != NULL && d->connected == CON_PLAYING) {
            to = d->character;
            if (IS_NPC(to))
            continue;
            if (to->in_room == NULL || ch->in_room == NULL)
            continue;
            if(to != ch && is_ffamily(to) && number_percent() % 5 == 0)
            printf_to_char(to, "You recollect a secret about %s %s.\n%s\n\r", ch->name, ch->pcdata->last_name, ch->pcdata->ff_secret);
          }
        }
      }

      return;
    }

    _DOFUN(do_rcm) {
      act("A man in a `Yraincoat`x, unobtrusive 'til now, touches $n's forehead and $n falls asleep.", ch, NULL, NULL, TO_ROOM);
      send_to_char("A man in a `Yraincoat`x, unobtrusive 'til now, touches your forehead and you fall asleep.\n\r", ch);
      char_from_room(ch);

      if (!IS_FLAG(ch->comm, COMM_STORY)) {
        send_to_char("You need to have story on to participate.\n\r", ch);
        return;
      }

      char_to_room(ch, get_room_index(41070));
      act("$n arrives on Sanctuary.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You arrive on Sanctuary.\n\r", ch);
    }

    _DOFUN(do_create) {
      if (ch->in_room != NULL && ch->in_room->vnum == ROOM_INDEX_GENESIS) {
        ch->pcdata->gen_stage = 1;
        do_function(ch, &do_look, "");
      }
    }


    bool newbie_school(ROOM_INDEX_DATA *room)
    {
      if(room == NULL)
      return FALSE;
      if(room->vnum == 50 || room->vnum == 51 || room->vnum == 62 || room->vnum == 59 || room->vnum == 57 || room->vnum == 58 || room->vnum == 52 || room->vnum == 53 || room->vnum == 54 || room->vnum == 61 || room->vnum == 55 || room->vnum == 56)
      return TRUE;
      return FALSE;
    }

    // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
  }
#endif
