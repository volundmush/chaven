#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif

#include <iterator>
#include "MudIo.hpp"
#include "Note.h"
#include "Util.h"
#include "merc.h"

#if defined(__cplusplus)
extern "C" {
#endif

  void noteAttach(CHAR_DATA *ch) {
    if (ch->pcdata->pNote == NULL) {
      ch->pcdata->pNote = new Note();
      if (IS_IMMORTAL(ch))
      ch->pcdata->pNote->setSender(ch->short_descr);
      else
      ch->pcdata->pNote->setSender(ch->name);

      ch->pcdata->pNote->setBoard(ch->pcdata->pNoteBoard);
    }
  }

  void notifyNoteRecipients(CHAR_DATA *ch, Note *pNote) {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      CHAR_DATA *victim = (*it)->character;

      if (victim != NULL && (*it)->connected == CON_PLAYING && pNote->isTo(victim->name)) {
        send_to_char("All online recipients have been notified.\n\r", ch);

        printf_to_char(
        victim, "[Note] %s has left a new note in the %s area to you.\n\r", pNote->getSender().c_str(), pNote->getBoard()->getDisplay().c_str());
        break;
      }
    }
  }

  void updateReadTime(CHAR_DATA *ch, NoteBoard *pNoteBoard, long value) {
    (*ch->pcdata->lastReadTimes)[pNoteBoard->getName()] = value;
  }

  bool isNewNote(CHAR_DATA *ch, Note *pNote) {
    if (IS_NPC(ch) || pNote == NULL) {
      bugf("[%s::isNewNote] NULL parameter received.", __FILE__);
      return FALSE;
    }

    return (*ch->pcdata->lastReadTimes)[pNote->getBoard()->getName()] <
    pNote->getDateStamp();
  }

  bool isNoteToGuild(CHAR_DATA *ch, Note *pNote) { return FALSE; }

  bool isNoteToGroup(CHAR_DATA *ch, Note *pNote) {
    if (IS_NPC(ch) || pNote == NULL || ch->pcdata->pNoteBoard == NULL) {
      bugf("[%s::isNoteToGroup] NULL parameter received.", __FILE__);
      return FALSE;
    }
    if (IS_IMMORTAL(ch) && (pNote->isTo("immortal") || pNote->isTo("immortals") || pNote->isTo("headquarters") || pNote->isTo("imm") || pNote->isTo("imms")))
    return TRUE;

    if (college_staff(ch, FALSE) && pNote->isTo("CollegeStaff"))
    return TRUE;
    if (clinic_staff(ch, FALSE) && pNote->isTo("ClinicStaff"))
    return TRUE;
    if ((college_student(ch, FALSE) || college_staff(ch, FALSE) || IS_IMMORTAL(ch)) && pNote->isTo("College"))
    return TRUE;
    if ((clinic_patient(ch) || clinic_staff(ch, FALSE) || IS_IMMORTAL(ch)) && pNote->isTo("Clinic"))
    return TRUE;
    if ((clinic_patient(ch) || clinic_staff(ch, FALSE) || college_student(ch, FALSE)
          || college_staff(ch, FALSE) || IS_IMMORTAL(ch)) && pNote->isTo("Institute"))
    return TRUE;
    if ((ch->race == RACE_DEPUTY || IS_IMMORTAL(ch)) && pNote->isTo("HSD"))
    return TRUE;

    if (IS_RESIDENT(ch) && (pNote->isTo("admin") || pNote->isTo("admins")))
    return TRUE;
    if (ch->level == MAX_LEVEL && (pNote->isTo("imp") || pNote->isTo("imps")))
    return TRUE;

    if (is_gm(ch) && (pNote->isTo("storyrunner") || pNote->isTo("storyrunners")))
    return TRUE;

    if (pNote->isTo("market") && ch->pcdata->market_visit > pNote->getDateStamp())
    return TRUE;

    if (ch->pcdata->association > 0 && pNote->isTo(nospaces(get_subfac_name(ch->pcdata->association))))
    return TRUE;

    if (safe_strlen(ch->pcdata->home_territory) > 2 && pNote->isTo(nospaces(ch->pcdata->home_territory)))
    return TRUE;

    if (ch->faction != 0 && clan_lookup(ch->faction) != NULL && pNote->isTo(clan_lookup(ch->faction)->name))
    return TRUE;

    if (ch->faction != 0 && clan_lookup(ch->faction) != NULL && pNote->isTo(nospaces(clan_lookup(ch->faction)->name)))
    return TRUE;

    if (get_cabal(ch) != NULL && pNote->isTo(nospaces(get_cabal(ch)->name)))
    return TRUE;

    if (ch->fcore != 0 && clan_lookup(ch->fcore) != NULL && clan_lookup(ch->fcore)->parent == FACTION_HAND && pNote->isTo("hand"))
    return TRUE;

    if (ch->fcore != 0 && clan_lookup(ch->fcore) != NULL && clan_lookup(ch->fcore)->parent == FACTION_ORDER && pNote->isTo("order"))
    return TRUE;

    if (ch->fcore != 0 && clan_lookup(ch->fcore) != NULL && clan_lookup(ch->fcore)->parent == FACTION_TEMPLE && pNote->isTo("temple"))
    return TRUE;

    if (ch->fcult != 0 && clan_lookup(ch->fcult) != NULL && pNote->isTo(clan_lookup(ch->fcult)->name))
    return TRUE;

    if (ch->fcult != 0 && clan_lookup(ch->fcult) != NULL && pNote->isTo(nospaces(clan_lookup(ch->fcult)->name)))
    return TRUE;


    if (ch->fsect != 0 && clan_lookup(ch->fsect) != NULL && pNote->isTo(clan_lookup(ch->fsect)->name))
    return TRUE;

    if (ch->fsect != 0 && clan_lookup(ch->fsect) != NULL && pNote->isTo(nospaces(clan_lookup(ch->fsect)->name)))
    return TRUE;


    /*
* Check to see if the note is addressed to any groups
* defined and if the character is part of that group
* then return TRUE
*/
    map<string, list<string>> m = NoteBoard::getGroups();
    map<string, list<string>>::iterator it;
    for (it = m.begin(); it != m.end(); ++it) {
      if (pNote->isTo(it->first) && NoteBoard::isInGroup(it->first, string(ch->name)))
      return TRUE;
    }

    return FALSE;
  }

  bool isNoteToChar(CHAR_DATA *ch, Note *pNote) {
    if (IS_NPC(ch) || pNote == NULL || ch->pcdata->pNoteBoard == NULL) {
      bugf("[%s::isNoteToChar] NULL parameter received.", __FILE__);
      return FALSE;
    }

    if (ch->level < pNote->getBoard()->getReadLevel())
    return FALSE;

    if (!is_gm(ch) || IS_IMMORTAL(ch)) {
      if (pNote->isTo("all")) // && pNote->getBoard()!=NoteBoard::getBoard(string("OOC")))
      return TRUE;
    }

    if (pNote->isTo(ch->name))
    return TRUE;

    if (pNote->getSender() == string(ch->name) || pNote->getSender() == string(ch->short_descr))
    return TRUE;

    if (is_srname(ch->name) && pNote->isTo(nosr_name(ch->name)))
    return TRUE;
    if (pNote->isTo(sr_name(ch->name)))
    return TRUE;

    if (isNoteToGuild(ch, pNote))
    return TRUE;

    if (isNoteToGroup(ch, pNote))
    return TRUE;

    return FALSE;
  }

  void appendNote(Note *pNote) {
    OutputFile outNotes(NOTES_FILE, ioAppend);

    if (outNotes) {
      pNote->writeTo(outNotes);
    }
  }

  void saveNotes() {
    OutputFile outBoards(NOTEBOARD_FILE), outNotes(NOTES_FILE);

    if (outBoards && outNotes) {
      //	outNotes << "NumNotes  " << Note::counter << endl;
      //	outNotes << "NumThread " << Note::threadCounter << endl << endl;

      vector<NoteBoard *> nb = NoteBoard::getBoards();
      vector<NoteBoard *>::iterator it;
      for (it = nb.begin(); it != nb.end(); ++it) {
        (*it)->writeTo(outBoards);
        list<Note *>::iterator lit;
        for (lit = (*it)->begin(); lit != (*it)->end(); ++lit)
        (*lit)->writeTo(outNotes);
      }
    }
  }

  void displayNote(Note *pNote, CHAR_DATA *ch) {
    char buf[MIL];
    time_t stamp = pNote->getDateStamp();
    /*
if(pNote->getBoard() == NoteBoard::getBoard( string("email")) && !IS_SET(ch->in_room->room_flags, ROOM_COMPUTER))
{
send_to_char("But there's no computer here.\n\r", ch);
return;
}
*/
    sprintf(buf, "---\n\rArea: %s - Note #%3d\n\rFrom: %s\n\r  To: %s\n\r  Cc: %s\n\rSubj: %s\n\rTime: %s\n\r---\n\r", pNote->getBoard()->getDisplay().c_str(), pNote->getNumber(), pNote->getSender().c_str(), pNote->getToList().toString().c_str(), pNote->getCcList().toString().c_str(), pNote->getSubject().c_str(), ctime(&stamp));

    send_to_char(buf, ch);
    page_to_char(pNote->getText().c_str(), ch);
    return;
  }

  void noteToChar(CHAR_DATA *ch, char *info, char *note_to) {
    NoteBoard *nb = NoteBoard::getBoard(string("OOC"));

    if (nb != NULL) {
      Note *pNote = new Note();
      pNote->setBoard(nb);
      pNote->setToList(string(note_to));
      pNote->setDateStamp(time(NULL));

      nb->add(pNote);

      appendNote(pNote);
      printf_to_char(ch, "Message posted to the %s area.\n\r", nb->getDisplay().c_str());
      notifyNoteRecipients(ch, pNote);
    }
  }

  // Checking ignore list prior to displaying notes - Discordance
  bool is_noteIgnored(CHAR_DATA *ch, Note *pNote) {
    bool ignored = FALSE;
    int i;

    for (i = 0; i < 50; i++) {
      if (!str_cmp(pNote->getSender().c_str(), ch->pcdata->ignored_characters[i])) {
        ignored = TRUE;
        i = 50;
      }
    }
    return ignored;
  }

  _DOFUN(do_note) {
    int anum;
    bool del_all = FALSE;
    char arg[MIL];
    char arg2[MIL];
    char buf[MIL];

    vector<NoteBoard *> nbl = NoteBoard::getBoards();
    vector<NoteBoard *>::iterator nbit;
    list<Note *>::iterator nit;

    if (IS_NPC(ch))
    return;

    ch->pcdata->secondary_timer = 0;
    ch->timer = 0;

    argument = one_argument(argument, arg);
    smash_tilde(argument);
    smash_vector(argument);

    if (arg[0] == '\0') {
      do_note(ch, "read");
      return;
    }

    /* note reset */
    if (!str_prefix(arg, "reset")) {
      if (!str_cmp(argument, "all")) {
        for (nbit = nbl.begin(); nbit != nbl.end(); ++nbit)
        updateReadTime(ch, *nbit, 0);

        printf_to_char(ch, "Assuming all notes are 'new'.\n\r");
      }
      else {
        updateReadTime(ch, ch->pcdata->pNoteBoard, 0);
        printf_to_char(ch, "Assuming all notes are 'new' in this area.\n\r");
      }
      return;
    }

    /* note catchup */
    else if (!str_prefix(arg, "catchup")) {
      if (!str_cmp(argument, "all")) {
        for (nbit = nbl.begin(); nbit != nbl.end(); ++nbit)
        updateReadTime(ch, *nbit, current_time);

        printf_to_char(ch, "Assuming all notes are read.\n\r");
      }
      else {
        updateReadTime(ch, ch->pcdata->pNoteBoard, current_time);
        printf_to_char(ch, "Assuming all notes are read in this area.\n\r");
      }
      return;
    }

    /* Change which note board to read from */
    else if (!str_prefix(arg, "area")) {
      NoteBoard *nb = NoteBoard::getBoard(argument);

      if (nb == NULL || ch->level < nb->getReadLevel())
      send_to_char("That is not a valid area name.\n\r", ch);
      else {
        ch->pcdata->pNoteBoard = nb;
        sprintf(buf, "You will now read/scan/post to the %s area.\n\r", nb->getDisplay().c_str());
        send_to_char(buf, ch);
        if (!str_cmp(argument, "bugs") || !str_cmp(argument, "ideas")) {
          send_to_char("If you'd like staff to see your issue, you should make a petition, see help petition.\n\r", ch);
        }
      }
      return;
    }

    /* Scan the note baords */
    else if (!str_prefix(arg, "scan")) {
      send_to_char("\nUnread Notes Per Area:\n\r", ch);

      for (nbit = nbl.begin(); nbit != nbl.end(); ++nbit) {
        int count = 0, pcount = 0, gcount = 0;

        if ((*nbit) != NULL && (*nbit)->getReadLevel() <= ch->level) {
          for (nit = (*nbit)->begin(); nit != (*nbit)->end(); ++nit) {
            if (isNewNote(ch, *nit)) {
              if (isNoteToChar(ch, *nit))
              count++;
              if ((*nit)->isTo(ch->name))
              pcount++;
              if (isNoteToGuild(ch, *nit))
              gcount++;
            }
          }

          sprintf(buf, "%*s: %10d \n\r", 13 + int(((*nbit)->getDisplay().length() -
          safe_strlen_color((*nbit)->getDisplay().c_str()))), (*nbit)->getDisplay().c_str(), count);
          send_to_char(buf, ch);
        }
      }

      printf_to_char(ch, "You are currently using area: %s\n\rType 'note area <name>' to change areas.\n\r", ch->pcdata->pNoteBoard->getDisplay().c_str());
      return;
    }

    /* List notes in current board */
    else if (!str_prefix(arg, "list")) {
      int count = 0;
      bool new_only = FALSE;
      bool find_to = FALSE;
      bool find_from = FALSE;
      char from_str[MIL], to_str[MIL];
      Buffer outbuf;

      buf[0] = '\0';

      while (argument[0] != '\0') {
        argument = one_argument(argument, arg2);
        if (str_cmp(arg2, "new") == 0)
        new_only = TRUE;

        if (str_cmp(arg2, "from") == 0) {
          find_from = TRUE;
          argument = one_argument(argument, from_str);
        }

        if (str_cmp(arg2, "to") == 0) {
          find_to = TRUE;
          argument = one_argument(argument, to_str);
        }
      }

      sprintf(buf, "Listing notes for area: %s\n\r", ch->pcdata->pNoteBoard->getDisplay().c_str());
      outbuf.strcat(buf);

      outbuf.strcat("  Number  Thread  From       Subject\n\r-------------------------------------------------------------------\n\r");

      for (nit = ch->pcdata->pNoteBoard->begin();
      nit != ch->pcdata->pNoteBoard->end(); ++nit) {
        if (!isNoteToChar(ch, *nit))
        continue;
        // Checking ignore list - Discordance
        if (is_noteIgnored(ch, *nit))
        continue;

        /* new only check */
        if (new_only && !isNewNote(ch, *nit))
        continue;

        /* from check */
        if (find_from && !(*nit)->isFrom(string(from_str)))
        continue;

        if (find_to && !(*nit)->isTo(string(to_str)))
        continue;

        /* Determine width of fields, taking color codes into consideration */
        int off = 11 + (safe_strlen((*nit)->getSender().c_str()) -
        safe_strlen_color((*nit)->getSender().c_str()));

        /* display note */
        sprintf(buf, "%s%*d %7d %*.*s %s\n\r", isNewNote(ch, *nit) ? "`R(New)`x" : "", isNewNote(ch, *nit) ? 3 : 8, (*nit)->getNumber(), (*nit)->getThread(), off * (-1), off - 1, (*nit)->getSender().c_str(), (*nit)->getSubject().c_str());

        outbuf.strcat(buf);
        count++;
      }

      if (count == 0)
      send_to_char("No notes found.\n\r", ch);
      else
      page_to_char(outbuf, ch);
    }
    else if (!str_prefix(arg, "thread")) {
      int count = 0;
      Buffer outbuf;

      if (argument[0] != '\0' && is_number(argument)) {
        sprintf(buf, "Listing notes for thread: %d\n\r", atoi(argument));
        outbuf.strcat(buf);

        outbuf.strcat("  Number  Thread  Subject\n\r-------------------------------------------------------------------\n\r");

        list<Note *> thread = ch->pcdata->pNoteBoard->getThread(atoi(argument));
        for (nit = thread.begin(); nit != thread.end(); ++nit) {
          if (!*nit || !isNoteToChar(ch, *nit))
          continue;

          if (is_noteIgnored(ch, *nit))
          continue;

          /* display note */
          sprintf(buf, "%s%*d %7d  %s\n\r", isNewNote(ch, *nit) ? "`R(New)`x" : "", isNewNote(ch, *nit) ? 3 : 8, (*nit)->getNumber(), (*nit)->getThread(), (*nit)->getSubject().c_str());
          outbuf.strcat(buf);
          count++;
        }

        if (count)
        page_to_char(outbuf, ch);
        else
        send_to_char("No notes found.", ch);
      }
      else
      send_to_char("Which note thread do you wish to see?", ch);
    }
    else if (!str_prefix(arg, "read")) {
      Note *next = NULL;

      /* read next unread note */
      if (argument[0] == '\0') {
        for (nit = ch->pcdata->pNoteBoard->begin();
        nit != ch->pcdata->pNoteBoard->end(); ++nit) {
          if (!isNoteToChar(ch, *nit))
          continue;
          // Checking ignore list - Discordance
          if (is_noteIgnored(ch, *nit))
          continue;
          if (!isNewNote(ch, *nit))
          break;
          next = *nit;
        }

        if (next != NULL) {
          displayNote(next, ch);
          long val = UMAX(
          (*ch->pcdata->lastReadTimes)[ch->pcdata->pNoteBoard->getName()], next->getDateStamp());
          updateReadTime(ch, ch->pcdata->pNoteBoard, val);
          return;
        }

        send_to_char("You have no unread notes.\n\r", ch);

        nbit = find(nbl.begin(), nbl.end(), ch->pcdata->pNoteBoard);
        nbit = (++nbit != nbl.end()) ? nbit : nbl.begin();
        ch->pcdata->pNoteBoard = NULL;
        while (ch->pcdata->pNoteBoard == NULL) {
          if (nbit == nbl.end())
          nbit = nbl.begin();

          if ((*nbit)->getReadLevel() <= ch->level)
          ch->pcdata->pNoteBoard = *nbit;
          else
          ++nbit;
        }

        sprintf(buf, "You will now read/scan/post to the %s area.\n\r", ch->pcdata->pNoteBoard->getDisplay().c_str());
        send_to_char(buf, ch);
        return;
      }
      else if (is_number(argument)) {
        anum = atoi(argument);
      }
      else {
        send_to_char("Note read which number?\n\r", ch);
        return;
      }

      for (nbit = nbl.begin(); nbit != nbl.end(); ++nbit) {
        for (nit = (*nbit)->begin(); nit != (*nbit)->end(); ++nit) {
          if ((*nit)->getNumber() == anum && isNoteToChar(ch, *nit)) {
            // Checking ignore list - Discordance
            if (!is_noteIgnored(ch, *nit)) {
              displayNote(*nit, ch);
              long val = UMAX(
              (*ch->pcdata->lastReadTimes)[ch->pcdata->pNoteBoard->getName()], (*nit)->getDateStamp());
              updateReadTime(ch, ch->pcdata->pNoteBoard, val);
              return;
            }
          }
        }
      }

      send_to_char("No such note.\n\r", ch);
    }
    else if (!str_cmp(arg, "reply")) {
      if (argument[0] == '\0' || !is_number(argument)) {
        send_to_char("You must specify a note number to reply to.\n\r", ch);
      }
      else if (ch->pcdata->pNote != NULL) {
        send_to_char("You already have a note in progress.\n\r", ch);
      }
      else {
        Note *pNote;
        if ((pNote = getPtrByFunc<Note, int>(&Note::getNumber, atoi(argument), ch->pcdata->pNoteBoard->begin(), ch->pcdata->pNoteBoard->end())) !=
            NULL && isNoteToChar(ch, pNote) && !is_noteIgnored(ch, pNote)) {
          ch->pcdata->pNote = Note::createReply(pNote, string(ch->name));
          ch->pcdata->noteText = str_dup(ch->pcdata->pNote->getText().c_str());
        }
        else {
          send_to_char("No such note!.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg, "forward")) {
      if (argument[0] == '\0' || !is_number(argument)) {
        send_to_char("You must specify a note number to forward.\n\r", ch);
      }
      else {
        Note *pNote;
        if ((pNote = getPtrByFunc<Note, int>(&Note::getNumber, atoi(argument), ch->pcdata->pNoteBoard->begin(), ch->pcdata->pNoteBoard->end())) !=
            NULL && isNoteToChar(ch, pNote) && !is_noteIgnored(ch, pNote)) {
          ch->pcdata->pNote = Note::createForward(pNote, string(ch->name));
          ch->pcdata->noteText = str_dup(ch->pcdata->pNote->getText().c_str());
        }
        else {
          send_to_char("No such note!.\n\r", ch);
          return;
        }
      }
    }
    else if (!str_cmp(arg, "edit")) {
      noteAttach(ch);
      string_append(ch, &ch->pcdata->noteText);
    }
    else if (!str_prefix(arg, "subject")) {
      noteAttach(ch);
      ch->pcdata->pNote->setSubject(argument);
      send_to_char("Ok.\n\r", ch);
    }
    else if (!str_prefix(arg, "to")) {
      noteAttach(ch);
      ch->pcdata->pNote->setToList(argument);
      send_to_char("Ok.\n\r", ch);
    }
    else if (!str_prefix(arg, "cc")) {
      noteAttach(ch);
      ch->pcdata->pNote->setCcList(argument);
      send_to_char("Ok.\n\r", ch);
    }
    else if (!str_prefix(arg, "bcc")) {
      noteAttach(ch);
      ch->pcdata->pNote->setBccList(argument);
      send_to_char("Ok.\n\r", ch);
    }
    else if (!str_prefix(arg, "move")) {
      Note *pNote;
      char num[MIL];
      char area[MIL];

      argument = one_argument(argument, num);
      argument = one_argument(argument, area);

      if (num[0] == '\0' || !is_number(num)) {
        send_to_char("You must specify a note number to move.\n\r", ch);
        return;
      }

      if (area[0] == '\0') {
        send_to_char("You must specify the area to move it to.\n\r", ch);
        return;
      }

      NoteBoard *nb = NoteBoard::getBoard(area);

      if (nb == NULL || ch->level < nb->getReadLevel()) {
        send_to_char("That is not a valid area name.\n\r", ch);
        return;
      }

      if ((pNote = getPtrByFunc<Note, int>(
              &Note::getNumber, atoi(num), ch->pcdata->pNoteBoard->begin(), ch->pcdata->pNoteBoard->end())) == NULL) {
        send_to_char("No such note!.\n\r", ch);
        return;
      }
      else {
        char name[256];

        remove_color(name, ch->name);
        if (!pNote->isFrom(string(name))) {
          send_to_char("That is not your note to move!\n\r", ch);
          return;
        }

        pNote->getBoard()->remove(pNote);
        nb->add(pNote);
        nb->sort();
      }
    }
    else if (!str_prefix(arg, "clear")) {
      if (ch->pcdata->pNote != NULL) {
        delete ch->pcdata->pNote;
        ch->pcdata->pNote = NULL;
      }

      send_to_char("Ok.\n\r", ch);
    }
    else if (!str_prefix(arg, "show")) {
      if (ch->pcdata->pNote == NULL) {
        send_to_char("You have no note in progress.\n\r", ch);
        return;
      }

      Note *pNote = ch->pcdata->pNote;
      char buf[MSL];
      time_t stamp = pNote->getDateStamp();

      sprintf(buf, "---\n\rArea: %s - Note #%3d\n\rFrom: %s\n\r  To: %s\n\r  Cc: %s\n\rSubj: %s\n\rTime: %s\n\r---\n\r", ch->pcdata->pNoteBoard->getDisplay().c_str(), pNote->getNumber(), pNote->getSender().c_str(), pNote->getToList().toString().c_str(), pNote->getCcList().toString().c_str(), pNote->getSubject().c_str(), ctime(&stamp));

      send_to_char(buf, ch);

      page_to_char(ch->pcdata->noteText, ch);
    }
    else if (!str_prefix(arg, "post")) {
      if (ch->pcdata->pNote == NULL) {
        send_to_char("You have no note in progress.\n\r", ch);
        return;
      }
      if ((is_helpless(ch) || in_fight(ch)) && ch->pcdata->pNote->getBoard() == NoteBoard::getBoard(string("ic"))) {
        send_to_char("Not now.\n\r", ch);
        return;
      }
      else if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NONOTE)) {
        send_to_char("Your note posting priviledges have been revoked.\n\r", ch);
      }
      else if (ch->pcdata->pNote->getToList().toString() == "") {
        send_to_char("You need to provide a recipient.\n\r", ch);
      }
      else if (ch->pcdata->pNote->getSubject() == "") {
        send_to_char("You need to provide a subject.\n\r", ch);
      }
      else if (ch->pcdata->pNoteBoard->getWriteLevel() > ch->level) {
        printf_to_char(ch, "Only %s or higher can post to that board.\n\r", ch->pcdata->pNoteBoard->getFailedMsg().c_str());
      }
      else {
        smash_vector(ch->pcdata->noteText);

        if (ch->pcdata->pNote->getBoard() == NoteBoard::getBoard(string("ic"))) {
          sprintf(buf, "%s", ch->pcdata->pNote->getToList().toString().c_str());
          note_reward(ch, str_dup(buf), ch->pcdata->noteText);
        }
        ch->pcdata->pNote->setDateStamp(time(NULL));
        if (ch->pcdata->noteText[0] != '\0') {
          ch->pcdata->pNote->setText(ch->pcdata->noteText);
          free_string(ch->pcdata->noteText);
          ch->pcdata->noteText = str_dup("");
        }
        sprintf(buf, "Message posted to %s area.\n\r", ch->pcdata->pNoteBoard->getDisplay().c_str());
        send_to_char(buf, ch);
        if (!spammer(ch) || !str_cmp(ch->name, ch->pcdata->pNote->getToList().toString().c_str())) {
          ch->pcdata->pNoteBoard->add(ch->pcdata->pNote);
          appendNote(ch->pcdata->pNote);
          notifyNoteRecipients(ch, ch->pcdata->pNote);
        }
        ch->pcdata->pNote = NULL;
      }
    }
    else if (!str_prefix(arg, "pastcharacter")) {
      if (ch->pcdata->pNote == NULL) {
        send_to_char("You have no note in progress.\n\r", ch);
        return;
      }
      if (ch->pcdata->pNoteBoard != NoteBoard::getBoard(string("ic"))) {
        ch->pcdata->pNoteBoard = NoteBoard::getBoard("ic");
      }
      if (ch->pcdata->pNote == NULL) {
        send_to_char("You have no note in progress.\n\r", ch);
      }
      else if (IS_FLAG(ch->act, PLR_NONOTE)) {
        send_to_char("Your note posting priviledges have been revoked.\n\r", ch);
      }
      else if (ch->pcdata->pNote->getToList().toString() == "") {
        send_to_char("You need to provide a recipient.\n\r", ch);
      }
      else if (ch->pcdata->pNote->getSubject() == "") {
        send_to_char("You need to provide a subject.\n\r", ch);
      }
      else if (ch->pcdata->pNoteBoard->getWriteLevel() > ch->level) {
        printf_to_char(ch, "Only %s or higher can post to that board.\n\r", ch->pcdata->pNoteBoard->getFailedMsg().c_str());
      }
      else {
        if (safe_strlen(argument) < 2) {
          send_to_char("Syntax: Note pastcharacter (past character full name)\n\r", ch);
          return;
        }
        if (strcasestr(ch->pcdata->account->pastcharacters, argument) == NULL) {
          send_to_char("That isn't a past character of yours.\n\r", ch);
          return;
        }
        if (ch->pcdata->pNote->getBoard() == NoteBoard::getBoard(string("ic"))) {
          sprintf(buf, "%s", ch->pcdata->pNote->getToList().toString().c_str());
          note_reward(ch, str_dup(buf), ch->pcdata->noteText);
        }
        sprintf(buf, "%s, %s\n\r", argument, ch->pcdata->account->name);
        ch->pcdata->pNote->setSender(buf);
        ch->pcdata->pNote->setDateStamp(time(NULL));
        if (ch->pcdata->noteText[0] != '\0') {
          ch->pcdata->pNote->setText(ch->pcdata->noteText);
          free_string(ch->pcdata->noteText);
          ch->pcdata->noteText = str_dup("");
        }
        if (!spammer(ch)) {
          ch->pcdata->pNoteBoard->add(ch->pcdata->pNote);
          appendNote(ch->pcdata->pNote);
        }
        sprintf(buf, "Message posted to %s area.\n\r", ch->pcdata->pNoteBoard->getDisplay().c_str());
        send_to_char(buf, ch);

        if (!spammer(ch))
        notifyNoteRecipients(ch, ch->pcdata->pNote);
        ch->pcdata->pNote = NULL;
      }
    }
    else if (!str_prefix(arg, "delete") && get_trust(ch) >= MAX_LEVEL - 10) {
      if (!is_number(argument)) {
        list<Note *>::iterator it;

        if (!str_cmp(argument, "all"))
        del_all = TRUE;

        for (nit = ch->pcdata->pNoteBoard->begin();
        nit != ch->pcdata->pNoteBoard->end(); nit = it) {
          it = ++nit;
          --nit;
          if ((del_all || (*nit)->isFrom(string(argument))) && isNoteToChar(ch, *nit)) {
            Note *n = *nit;
            ch->pcdata->pNoteBoard->remove(n);
            delete n;
          }
        }

        send_to_char("Area getting deleted.\n\r", ch);
        saveNotes();
      }
      else {
        Note *pNote = getPtrByFunc<Note, int>(&Note::getNumber, atoi(argument), ch->pcdata->pNoteBoard->begin(), ch->pcdata->pNoteBoard->end());

        if (pNote) {
          pNote->getBoard()->remove(pNote);
          delete pNote;
          saveNotes();
          send_to_char("Ok.\n\r", ch);
        }
        else
        send_to_char("No such note.\n\r", ch);
      }
    }
    else
    send_to_char("Huh?  Type 'help note' for usage.\n\r", ch);

    return;
  }

  char *note_news(CHAR_DATA *ch) {
    char string[MSL];
    char buf[MSL];
    vector<NoteBoard *> nbl = NoteBoard::getBoards();
    vector<NoteBoard *>::iterator nbit;
    list<Note *>::iterator nit;

    string[0] = '\0';

    strcat(string, "\nUnread Notes Per Area:\n\n\r");

    for (nbit = nbl.begin(); nbit != nbl.end(); ++nbit) {
      int count = 0, pcount = 0, gcount = 0;

      if ((*nbit) != NULL && (*nbit)->getReadLevel() <= ch->level) {
        for (nit = (*nbit)->begin(); nit != (*nbit)->end(); ++nit) {
          if (isNewNote(ch, *nit)) {
            if (isNoteToChar(ch, *nit))
            count++;
            if ((*nit)->isTo(ch->name))
            pcount++;
            if (isNoteToGuild(ch, *nit))
            gcount++;
          }
        }

        if (nbit == nbl.begin())
        sprintf(buf, "%*s:\t %10d \n\r", 13 + int(((*nbit)->getDisplay().length() -
        safe_strlen_color((*nbit)->getDisplay().c_str()))), (*nbit)->getDisplay().c_str(), count);
        else
        sprintf(buf, "%*s:\t\t %10d \n\r", 13 + int(((*nbit)->getDisplay().length() -
        safe_strlen_color((*nbit)->getDisplay().c_str()))), (*nbit)->getDisplay().c_str(), count);
        strcat(string, buf);
      }
    }
    return str_dup(string);
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif
