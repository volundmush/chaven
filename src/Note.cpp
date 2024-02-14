#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#pragma warning(disable : 4800)
#endif
#endif

#include "Note.h"
#include "merc.h"
#include <algorithm>  // for std::replace
#include <iostream>

vector<NoteBoard *> NoteBoard::s_nbList;
map<string, list<string>> NoteBoard::s_grpList;

NoteBoard::NoteBoard(string _name, string _display, int _rlevel, int _wlevel, string _fail, bool _enabled)
: m_strName(_name), m_strDisplay(_display), m_iReadLevel(_rlevel), m_iWriteLevel(_wlevel), m_strFailedWriteMsg(_fail), m_bEnabled(_enabled), m_Notes(0) {}

NoteBoard::~NoteBoard() {
  list<Note *>::iterator it;
  for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
  delete *it;
}

list<Note *> NoteBoard::getThread(int iTNum) {
  list<Note *> retList;
  list<Note *>::iterator start = m_Notes.begin();
  list<Note *>::iterator it;

  while (start != m_Notes.end()) {
    it = find_if(start, m_Notes.end(), func_equal<Note, int>(&Note::getThread, iTNum));

    if (it != m_Notes.end()) {
      retList.push_back(*it);
      ++it;
    }
    start = it;
  }

  return retList;
}

bool NoteBoard::writeTo(Output &os) {
  os << "Name    " << m_strName << "~" << endl;
  os << "Display " << m_strDisplay << "~" << endl;
  os << "RLevel  " << m_iReadLevel << endl;
  os << "WLevel  " << m_iWriteLevel << endl;
  os << "FailMsg " << m_strFailedWriteMsg << "~" << endl;
  os << "Enabled " << (int)m_bEnabled << endl;
  os << "End" << endl << endl;

  return true;
}

bool NoteBoard::readFrom(StaticInput &is) {
  char key[256], temp[1024];
  bool bMatch, bDone = false;

  while (*is.getWord(key)) {
    bMatch = false;
    if (!str_cmp(key, "End")) {
      bDone = true;
      break;
    }

    switch (toupper(key[0])) {
    case 'D':
      if (!str_cmp(key, "Display")) {
        m_strDisplay = is.getString(temp);
        bMatch = true;
      }
      break;
    case 'E':
      if (!str_cmp(key, "Enabled")) {
        m_bEnabled = is.getInt();
        bMatch = true;
      }
      break;
    case 'F':
      if (!str_cmp(key, "FailMsg")) {
        m_strFailedWriteMsg = is.getString(temp);
        bMatch = true;
      }
      break;
    case 'N':
      if (!str_cmp(key, "Name")) {
        m_strName = is.getString(temp);
        bMatch = true;
      }
      break;
    case 'R':
      if (!str_cmp(key, "RLevel")) {
        m_iReadLevel = is.getInt();
        bMatch = true;
      }
      break;
    case 'W':
      if (!str_cmp(key, "WLevel")) {
        m_iWriteLevel = is.getInt();
        bMatch = true;
      }
      break;
    }

    if (!bMatch) {
      bugf("[%s::%s] Key not found: %s", __FILE__, __FUNCTION__, key);
      is.getLine(temp);
    }
  }

  return bDone;
}

vector<NoteBoard *> NoteBoard::getBoards() { return s_nbList; }

NoteBoard *NoteBoard::getBoard(const string &name) {
  return getPtrByFuncCIPre<NoteBoard>(&NoteBoard::getName, name, s_nbList.begin(), s_nbList.end());
}

NoteBoard *NoteBoard::getBoard(const int num) {
  return getPtrByFunc<NoteBoard, int>(&NoteBoard::getNumber, num, s_nbList.begin(), s_nbList.end());
}

void NoteBoard::addBoard(NoteBoard *nb) {
  if (nb)
  s_nbList.push_back(nb);
}

bool NoteBoard::isGroup(string grpName) { return !s_grpList[grpName].empty(); }

bool NoteBoard::isInGroup(string grpName, string name) {
  return find_if(s_grpList[grpName].begin(), s_grpList[grpName].end(), ci_string_cmp(name)) != s_grpList[grpName].end();
}

map<string, list<string>> &NoteBoard::getGroups() { return s_grpList; }

void NoteBoard::addGroup(string grpName, string members) {
  list<string> values;
  split(members, " ", back_inserter(values));
  s_grpList[grpName] = values;
}

int Note::counter = 0;
int Note::threadCounter = 0;

Note::Note(StaticInput &is) { readFrom(is); }

Note::~Note() {
  if (m_nbBoard)
  m_nbBoard->remove(this);
}

bool Note::isTo(const string to) {
  return m_toList.hasValue(to) || m_ccList.hasValue(to) || m_bccList.hasValue(to);
}

bool Note::isFrom(const string from) {
  string str = str_rem_color(m_strSender);

  logfi("%s == %s", str.c_str(), from.c_str());
  /*
char buf[35];

remove_color(buf, from.c_str());
log_string(buf);
return string_cmp(m_strSender, buf);
*/
  return string_cmp(str_rem_color(m_strSender), from);
}

Note *Note::createReply(Note *pNote, string strSender) {
  NoteField to; //(pNote->getToList());
  char buf[256];

  remove_color(buf, pNote->getSender().c_str());
  to.add(buf);

  list<string> l = pNote->getToList().getValues();
  list<string>::iterator i;
  for (i = l.begin(); i != l.end(); ++i) {
    char buf[256];
    remove_color(buf, i->c_str());
    to.add(buf);
  }

  Note *newNote = new Note(
  strSender, to, pNote->getCcList(), pNote->getBccList(), pNote->getBoard(), "Re: " + pNote->getSubject(), string(""), pNote->getThread());

  newNote->setText(pNote->redirectFormat("#"));
  return newNote;
}

Note *Note::createForward(Note *pNote, string strSender) {
  Note *newNote =
  new Note(strSender, string(""), string(""), string(""), pNote->getBoard(), "Fwd: " + pNote->getSubject(), string(""), pNote->getThread());

  newNote->setText(pNote->redirectFormat("#"));
  return newNote;
}

string Note::redirectFormat(string pre) {
  strstream ss;
  string str;
  time_t stamp = m_lDateStamp;

  ss << "Original Note:"
  << "\n\r";
  ss << "Area: " << m_nbBoard->getDisplay();
  ss << " Note #: " << m_iNumber << "\n\r";
  ss << "   From: " << m_strSender << "\n\r";
  ss << "     To: " << m_toList << "\n\r";
  ss << "     CC: " << m_ccList << "\n\r";
  ss << "Subject: " << m_strSubject << "\n\r";
  ss << "   Time: " << string(ctime(&stamp)) << "\r";
  ss << "---------\n\r" << m_strText;
  ss << ends;

  str = ss.str();
  for (size_t loc = str.find("\n\r"); loc != string::npos;
  loc = str.find("\n\r", loc + 3)) {
    str.insert(loc + 2, pre);
  }
  return str;
}

void Note::removeRecipient(string str) {
  m_toList.remove(str);
  m_ccList.remove(str);
  m_bccList.remove(str);
}


/*
bool Note::writeTo(Output &os) {
// Helper function to replace '~' with an empty string
auto removeTilde = [](const std::string &str) {
std::string result = str;  // Make a copy
std::replace(result.begin(), result.end(), '~', ' ');
return result;
};

os << "Number  " << m_iNumber << std::endl;
os << "Thread  " << m_iThread << std::endl;

// Remove '~' from m_strSender
os << "Sender  " << removeTilde(m_strSender) << std::endl;

os << "Date    " << m_lDateStamp << std::endl;

// Remove '~' from m_toList.toString()
os << "To      " << removeTilde(m_toList.toString()) << std::endl;

if (!m_ccList.empty()) {
// Remove '~' from m_ccList.toString()
os << "Cc      " << removeTilde(m_ccList.toString()) << std::endl;
}

if (!m_bccList.empty()) {
// Remove '~' from m_bccList.toString()
os << "Bcc     " << removeTilde(m_bccList.toString()) << std::endl;
}

// Remove '~' from m_strSubject
os << "Subject " << removeTilde(m_strSubject) << std::endl;

os << "BoardN  " << m_nbBoard->getName() << std::endl;

// Remove '~' from m_strText
os << "Text" << std::endl << removeTilde(m_strText) << std::endl;

os << "End" << std::endl << std::endl;

return true;
}
*/

bool Note::writeTo(Output &os) {

  auto removeTilde = [](const std::string &str) {
    std::string result = str;  // Make a copy
    std::replace(result.begin(), result.end(), '~', ' ');
    return result;
  };


  os << "Number  " << m_iNumber << endl;
  os << "Thread  " << m_iThread << endl;
  os << "Sender  " << m_strSender << "~" << endl;
  os << "Date    " << m_lDateStamp << endl;
  os << "To      " << m_toList.toString() << "~" << endl;

  if (!m_ccList.empty())
  os << "Cc      " << m_ccList.toString() << "~" << endl;

  if (!m_bccList.empty())
  os << "Bcc     " << m_bccList.toString() << "~" << endl;

  os << "Subject " << m_strSubject << "~" << endl;
  os << "BoardN  " << m_nbBoard->getName() << endl;
  os << "Text" << endl << removeTilde(m_strText) << "~" << endl;
  os << "End" << endl << endl;

  return true;
}


bool Note::readFrom(StaticInput &is) {
  char key[256], temp[MAX_STRING_LENGTH];
  bool bMatch, bDone = false;

  while (*is.getWord(key)) {
    bMatch = false;
    // cout << key << endl;
    if (!str_cmp(key, "End")) {
      bDone = true;
      break;
    }

    switch (toupper(key[0])) {
    case 'B':
      if (!str_cmp(key, "Bcc")) {
        m_bccList.setValue(is.getString(temp));
        bMatch = true;
      }
      if (!str_cmp(key, "BoardN")) {
        string str(is.getWord(temp));
        m_nbBoard = NoteBoard::getBoard(str);
        bMatch = true;
      }
      break;
    case 'C':
      if (!str_cmp(key, "Cc")) {
        m_ccList.setValue(is.getString(temp));
        bMatch = true;
      }
      break;
    case 'D':
      if (!str_cmp(key, "Date")) {
        m_lDateStamp = is.getLong();
        bMatch = true;
      }
      break;
    case 'N':
      if (!str_cmp(key, "Number")) {
        m_iNumber = is.getInt();
        bMatch = true;
      }
      break;
    case 'S':
      if (!str_cmp(key, "Sender")) {
        m_strSender = is.getString(temp);
        bMatch = true;
      }
      else if (!str_cmp(key, "Subject")) {
        m_strSubject = is.getString(temp);
        bMatch = true;
      }
      break;
    case 'T':
      if (!str_cmp(key, "Text")) {
        m_strText = is.getString(temp);
        bMatch = true;
      }
      else if (!str_cmp(key, "Thread")) {
        m_iThread = is.getInt();
        bMatch = true;
      }
      else if (!str_cmp(key, "To")) {
        m_toList.setValue(is.getString(temp));
        bMatch = true;
      }
      break;
    }
    if (!bMatch) {
      bugf("[%s::%s] Key not found: %s", __FILE__, __FUNCTION__, key);
      is.getLine(temp);
    }
  }

  return bDone;
}

string Note::toString() {
  strstream ss;

  ss << "[" << m_iNumber << "][" << m_iThread << "] ";
  ss << m_strSender << ": " << m_strSubject << ends;

  return ss.str();
}
