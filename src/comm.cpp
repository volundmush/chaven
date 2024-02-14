#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif

#if defined(_WIN32)
#include <winsock2.h>
#include <io.h>
#include <sys\timeb.h>
#include <process.h>
#else
#include <sys/wait.h>
#endif

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>

#include "telnet.h"
#include "merc.h"
#include "recycle.h"
#include "olc.h"
#include "gsn.h"

#include <math.h>
#include <ctype.h>

#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

  /*
* Malloc debugging stuff.
*/
#if defined(MALLOC_DEBUG)
#include <malloc.h>
  extern	int	malloc_debug	args( ( int  ) );
  extern	int	malloc_verify	args( ( void ) );
#endif

  char          tmp_buff[256];

#if	defined( _WIN32 )
#define WOULD_HAVE_BLOCKED ( WSAGetLastError() == WSAEWOULDBLOCK )
#define STDOUT_FILENO _fileno( stdout )
#else
#define WOULD_HAVE_BLOCKED ( errno == EWOULDBLOCK )
#endif

  void close_socket(int fd);
  void close_file(int fd);
  /*
* Signal handling.
*/
  void sig_chld(int signo);
  int tics = 0;

  DECLARE_DO_FUN(ntedit_show);

  const char go_ahead_str[] = {(char)IAC, (char)GA, '\0'};

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

  /*
* Global variables.
*/
  DescList descriptor_list; /* All open descriptors		*/
  bool god;                 /* All new chars are gods!	*/
  bool merc_down;           /* Shutdown			*/
  bool wizlock;             /* Game is wizlocked		*/
  bool newlock;             /* Game is newlocked		*/
  bool implock;             /* Game is imlocked. (Kuval)    */
  char str_boot_time[MAX_INPUT_LENGTH];
  time_t current_time;    /* time of this pulse */
  bool MOBtrigger = TRUE; /* act() switch                 */
  /*
* OS-dependent local functions.
*/
  void game_loop_unix args((int control));
  int init_socket args((int port));
  void init_descriptor args((int control));
  bool read_from_descriptor args((DESCRIPTOR_DATA * d));

  int eof_count = 0;

  /*
* Other local functions (OS-independent).
*/
  const char echo_off_str[] = {(char)IAC, (char)WILL, TELOPT_ECHO, '\0'};
  const char echo_on_str[] = {(char)IAC, (char)WONT, TELOPT_ECHO, '\0'};

  bool check_parse_name args((char *name));
  bool check_reconnect args((DESCRIPTOR_DATA * d, char *name, bool fConn));
  bool check_playing args((DESCRIPTOR_DATA * d, char *name));
  int main args((int argc, char **argv));
  void nanny args((DESCRIPTOR_DATA * d, char *argument));
  bool process_output args((DESCRIPTOR_DATA * d, bool fPrompt));
  void read_from_buffer args((DESCRIPTOR_DATA * d));
  void stop_idling args((CHAR_DATA * ch));
  void bust_a_prompt args((CHAR_DATA * ch));
  int calc_status args((int stat, int max_stat));

  void handle_ansi_color args((DESCRIPTOR_DATA * d, char *argument));

  extern void set_whotitle args((CHAR_DATA * ch, char *whotitle));

  extern void credit_show args((CHAR_DATA * ch, char *argument));
  extern void credit_show_adult args((CHAR_DATA * ch, char *argument));
  extern char *indefinite args((char *str));
  char *offline_acc_name args((char *name));
  bool character_exists args((char *name));
  bool valid_logon args((ACCOUNT_TYPE * account, char *name, DESCRIPTOR_DATA *ddx));
  /* These are for custom signal handling */
  void init_signals args((void));
  void do_auto_shutdown args((void));
  void sig_handler args((int sig));
  void sig_deadprotection args((int sig));

  /* Needs to be global because of do_copyover */
  int port, control, num_descriptors;

  // MXP - Discordance
#define TELOPT_MXP '\x5B'

  const unsigned char will_mxp_str[] = {IAC, WILL, TELOPT_MXP, '\0'};
  const unsigned char start_mxp_str[] = {IAC, SB, TELOPT_MXP, IAC, SE, '\0'};
  const unsigned char do_mxp_str[] = {IAC, DO, TELOPT_MXP, '\0'};
  const unsigned char dont_mxp_str[] = {IAC, DONT, TELOPT_MXP, '\0'};

  int main(int argc, char **argv) {
    struct timeval now_time;
    bool fCopyOver = FALSE;

    /*
    * Memory debugging if needed.
    */
#if defined(MALLOC_DEBUG)
    malloc_debug(2);
#endif

#if defined(WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    num_descriptors = 0;

    /*
    * Init time.
    */
    gettimeofday(&now_time, NULL);
    current_time = (time_t)now_time.tv_sec;
    strcpy(str_boot_time, ctime(&current_time));

    /*
    * Get the port number.
    */
    port = 4000;
    if (argc > 1) {
      if (!is_number(argv[1])) {
        fprintf(stderr, "Usage: %s [port #]\n", argv[0]);
        exit(1);
      }
      else if ((port = atoi(argv[1])) <= 1024) {
        fprintf(stderr, "Port number must be above 1024.\n");
        exit(1);
      }

      /* Are we recovering from a copyover? */
      if (argv[2] && argv[2][0]) {
        fCopyOver = TRUE;
        control = atoi(argv[3]);
      }
      else
      fCopyOver = FALSE;
    }

    /*
    * Run the game.
    */
    if (!fCopyOver)
    control = init_socket(port);

    init_signals();
    boot_db();
    log_string("About to start");
    sprintf(log_buf, "%s is ready to rock on port %d.", sysdata->mud_name, port);
    log_string(log_buf);
    /*
    world_sync("streets");
    world_sync("greenspace");
    world_sync("skies");
    */

    if (fCopyOver)
    copyover_recover();

    game_loop_unix(control);
    close_socket(control);

#if defined(WIN32)
    WSACleanup();
#endif

    /*
    * That's all, folks.
    */
    log_string("Normal termination of game.");
    exit(0);
    return 0;
  }

    /*
    * Count number of mxp tags need converting
    *    ie. < becomes &lt;
    *        > becomes &gt;
    *        & becomes &amp;
    */

  // MXP - Discordance
  int count_mxp_tags(const int bMXP, const char *txt, int length) {
    char c;
    const char *p;
    int count;
    int bInTag = FALSE;
    int bInEntity = FALSE;

    for (p = txt, count = 0; length > 0; p++, length--) {
      c = *p;

      if (bInTag) /* in a tag, eg. <send> */
      {
        if (!bMXP)
        count--; /* not output if not MXP */
        if (c == MXP_ENDc)
        bInTag = FALSE;
      }                   /* end of being inside a tag */
      else if (bInEntity) /* in a tag, eg. <send> */
      {
        if (!bMXP)
        count--; /* not output if not MXP */
        if (c == ';')
        bInEntity = FALSE;
      } /* end of being inside a tag */
      else
      switch (c) {

      case MXP_BEGc:
        bInTag = TRUE;
        if (!bMXP)
        count--; /* not output if not MXP */
        break;

      case MXP_ENDc: /* shouldn't get this case */
        if (!bMXP)
        count--; /* not output if not MXP */
        break;

      case MXP_AMPc:
        bInEntity = TRUE;
        if (!bMXP)
        count--; /* not output if not MXP */
        break;

      default:
        if (bMXP) {
          switch (c) {
          case '<': /* < becomes &lt; */
          case '>': /* > becomes &gt; */
            count += 3;
            break;

          case '&':
            count += 4; /* & becomes &amp; */
            break;

          case '"': /* " becomes &quot; */
            count += 5;
            break;

          } /* end of inner switch */
        }   /* end of MXP enabled */
      }     /* end of switch on character */
    }         /* end of counting special characters */

    return count;
  } /* end of count_mxp_tags */

  void convert_mxp_tags(const int bMXP, char *dest, const char *src, int length) {
    char c;
    const char *ps;
    char *pd;
    int bInTag = FALSE;
    int bInEntity = FALSE;

    for (ps = src, pd = dest; length > 0; ps++, length--) {
      c = *ps;
      if (bInTag) /* in a tag, eg. <send> */
      {
        if (c == MXP_ENDc) {
          bInTag = FALSE;
          if (bMXP)
          *pd++ = '>';
        }
        else if (bMXP)
        *pd++ = c;      /* copy tag only in MXP mode */
      }                   /* end of being inside a tag */
      else if (bInEntity) /* in a tag, eg. <send> */
      {
        if (bMXP)
        *pd++ = c; /* copy tag only in MXP mode */
        if (c == ';')
        bInEntity = FALSE;
      } /* end of being inside a tag */
      else
      switch (c) {
      case MXP_BEGc:
        bInTag = TRUE;
        if (bMXP)
        *pd++ = '<';
        break;

      case MXP_ENDc: /* shouldn't get this case */
        if (bMXP)
        *pd++ = '>';
        break;

      case MXP_AMPc:
        bInEntity = TRUE;
        if (bMXP)
        *pd++ = '&';
        break;

      default:
        if (bMXP) {
          switch (c) {
          case '<':
            memcpy(pd, "&lt;", 4);
            pd += 4;
            break;

          case '>':
            memcpy(pd, "&gt;", 4);
            pd += 4;
            break;

          case '&':
            memcpy(pd, "&amp;", 5);
            pd += 5;
            break;

          case '"':
            memcpy(pd, "&quot;", 6);
            pd += 6;
            break;

          default:
            *pd++ = c;
            break; /* end of default */
          }        /* end of inner switch */
        }
        else
        *pd++ = c; /* not MXP - just copy character */
        break;
      } /* end of switch on character */
    }     /* end of converting special characters */
  } /* end of convert_mxp_tags */

  /*
  * Almost the same as read_from_buffer...
  */
  bool read_from_ident(int fd, char *buffer) {
    static char inbuf[MAX_STRING_LENGTH * 2];
    unsigned int iStart;
    int i, j, k;

    /* Check for overflow. */
    iStart = safe_strlen(inbuf);
    if (iStart >= sizeof(inbuf) - 10) {
      log_string("Ident input overflow!!!");
      return FALSE;
    }

    /* Snarf input. */
    for (;;) {
      int nRead;

      //#if defined(WIN32)
      //			nRead = _read( fd, inbuf + iStart, sizeof( inbuf ) - 10 - iStart
      //); #else
      nRead = read(fd, inbuf + iStart, sizeof(inbuf) - 10 - iStart);
      //#endif
      if (nRead > 0) {
        iStart += nRead;
        if (iStart < 2 || inbuf[iStart - 2] == '\n' || inbuf[iStart - 2] == '\r')
        break;
      }
      else if (nRead == 0) {
        return FALSE;
      }
      else if (WOULD_HAVE_BLOCKED)
      break;
      else {
        perror("Read_from_ident");
        return FALSE;
      }
    }

    inbuf[iStart] = '\0';

    /*
    * Look for at least one new line.
    */
    for (i = 0; inbuf[i] != '\n' && inbuf[i] != '\r'; i++) {
      if (inbuf[i] == '\0')
      return FALSE;
    }

    /*
    * Canonical input processing.
    */
    for (i = 0, k = 0; inbuf[i] != '\n' && inbuf[i] != '\r'; i++) {
      if (inbuf[i] == '\b' && k > 0)
      --k;
      else if (isascii(inbuf[i]) && isprint(inbuf[i]))
      buffer[k++] = inbuf[i];
    }

    /*
    * Finish off the line.
    */
    if (k == 0)
    buffer[k++] = ' ';
    buffer[k] = '\0';

    /*
    * Shift the input buffer.
    */
    while (inbuf[i] == '\n' || inbuf[i] == '\r')
    i++;
    for (j = 0; (inbuf[j] = inbuf[i + j]) != '\0'; j++)
    ;

    return TRUE;
  }

  int max_d(void)
  {
    int max = 0;
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;
      if(d->connected == CON_PLAYING && d->descriptor > max)
      max = d->descriptor;
    }
    return max;
  }
  void save_game_stuff(void)
  {
    save_ground_objects();
    save_clans(FALSE);
    CHAR_DATA *victim;
    /* Consider changing all saved areas here, if you use OLC */
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      if ((*it)->connected == CON_PLAYING && (victim = (*it)->character) != NULL && !IS_NPC(victim) && victim->in_room != NULL) {
        save_char_obj(victim, FALSE, FALSE);
      }
    }
    AREA_DATA *pArea;
    AreaList::iterator iat;

    save_area_list();

    for (iat = area_list.begin(); iat != area_list.end(); ++iat) {
      pArea = *iat;

      /* Save changed areas. */
      if (IS_SET(pArea->area_flags, AREA_CHANGED)) {
        save_area(pArea, FALSE);
        REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
      }
    }


  }

  /*
  * Process input that we got from the ident process.
  */
  void process_ident(DESCRIPTOR_DATA *d) {
    char buffer[MAX_INPUT_LENGTH];
    char address[MAX_INPUT_LENGTH];
    char outbuf[MAX_STRING_LENGTH];
    CHAR_DATA *ch = CH(d);
    char lbuf[100];
    buffer[0] = '\0';

    if (!read_from_ident(d->ifd, buffer)) {
      return;
    }
    sprintf(lbuf, "Read from ident: %d", d->descriptor);
    log_string(lbuf);

    if (IS_NULLSTR(buffer)) {
      sprintf(lbuf, "NULL Buffer: %d", d->descriptor);
      log_string(lbuf);
      return;
    }
    if (!IS_NULLSTR(buffer)) {
      free_string(d->host);
      d->host = str_dup(buffer);
    }

    /*    for(DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it)
    {
    if(d->descriptor != (*it)->descriptor && (*it)->connected == 10)
    {
    close_desc(*it);
    break;
    }
    }
    */

    /* Format the output into something legible */
    if (ch) {
      sprintf(outbuf, "$N has address `4%s`x. Username unknown.", d->host);
      sprintf(log_buf, "%s has address %s.", ch->name, d->host);
      log_string(log_buf);
    }
    else if (address != NULL) {
      sprintf(outbuf, "Descriptor %d from address `4%s`x. Username unknown.", d->descriptor, d->host);

      sprintf(log_buf, "Descriptor %d has address %s.", d->descriptor, d->host);
      log_string(log_buf);
    }
    else {
      if (ch) {
        sprintf(log_buf, "%s could not be identified.", ch->name);
        log_string(log_buf);
      }
      else {
        sprintf(log_buf, "Descriptor %d could not be identified.", d->descriptor);
        wiznet(log_buf, ch, NULL, WIZ_LOGINS, 0, 0);
        log_string(log_buf);
      }
    }

    /*
    * Close descriptor and kill the ident process
    */
    close_socket(d->ifd);
    d->ifd = -1;
    d->ipid = -1;

    sprintf(lbuf, "Post kill: %d", d->descriptor);
    log_string(lbuf);

    /*
    * Swiftest: I added the following to ban sites.  I don't
    * endorse banning of sites, but Copper has few descriptors now
    * and some people from certain sites keep abusing access by
    * using automated 'autodialers' and leaving connections hanging.
    *
    * Furey: added suffix check by request of Nickel of HiddenWorlds.
    */
    if (check_ban(d->host, BAN_ALL)) {
      write_to_descriptor(d->descriptor, "Your site has been banned from this mud.\n\r", 0);
      log_string(d->host);
      close_desc(d);
      return;
    }

    if (!d->got_ident) {
      sprintf(lbuf, "Colour prompt: %d, max d: %d, d size: %lu", d->descriptor, max_d(), descriptor_list.size());
      log_string(lbuf);
      write_to_buffer(d, "Would you like colour? (Y/n) ", 0);
      d->connected = CON_ANSI_COLOR;
    }

    d->got_ident = TRUE;
    return;
  }



#if defined(unix)
  void create_ident( DESCRIPTOR_DATA *d, long ip )
  {
    int fds[2];
    pid_t pid;

    /* Create Pipe first */
    if ( pipe( fds ) != 0 )
    {
      perror( "Create_ident: pipe: " );
      return;
    }

    if ( dup2( fds[1], STDOUT_FILENO ) != STDOUT_FILENO )
    {
      perror( "Create_ident: dup2(stdout): " );
      close( fds[0] );
      return;
    }

    switch( (pid = fork()) )
    {
      char str_ip[64];

      /* Parent Process */
    default:
      d->ifd  = fds[0];
      d->ipid = pid;
      break;

      /* Child Process */
    case 0:
      d->ifd  = fds[0];
      d->ipid = pid;

      sprintf( str_ip, "%ld", ip );
      //              execl( RESOLVE_FILE, "resolve", str_ip, 0 );
      execl( RESOLVE_FILE, "resolve", str_ip, NULL );

      /* Still here --> hmm. An error. */
      log_string( "Exec failed; Closing child." );
      close( fds[0] );
      d->ifd  = -1;
      d->ipid = -1;

      exit( 0 );
      break;

      /* Error */
    case (pid_t)-1:
      perror( "Create_ident: fork" );
      close( fds[0] );
      break;
    }

    close( fds[1] );
  }
#else
  void create_ident( DESCRIPTOR_DATA *d, long ip )
  {
    int fds[2];
    char str_ip[64];

    /* Create Pipe first */
    if ( _pipe( fds, 64, O_TEXT ) != 0 )
    {
      perror( "Create_ident: pipe: " );
      return;
    }

    if ( _dup2( fds[1], STDOUT_FILENO ) != STDOUT_FILENO )
    {
      perror( "Create_ident: dup2(stdout): " );
      _close( fds[0] );
      return;
    }

    sprintf( str_ip, "%ld", ip );
    d->ipid = _spawnl(_P_NOWAIT, RESOLVE_FILE, "resolve", str_ip, 0);

    if(d->ipid = -1)
    {
      log_string( "_Spawnl failed; Closing child." );
      _close( fds[0] );
      d->ifd  = -1;
      d->ipid = -1;
    }
    _close( fds[1] );
  }
#endif



  int init_socket(int port) {
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Init_socket: socket");
      exit(1);
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&x, sizeof(x)) < 0) {
      perror("Init_socket: SO_REUSEADDR");
      close_socket(fd);
      exit(1);
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
      struct linger ld;

      ld.l_onoff = 1;
      ld.l_linger = 1000;

      if (setsockopt(fd, SOL_SOCKET, SO_DONTLINGER, (char *)&ld, sizeof(ld)) <
          0) {
        perror("Init_socket: SO_DONTLINGER");
        close_socket(fd);
        exit(1);
      }
    }
#endif

    sa = sa_zero;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
      perror("Init socket: bind");
      close_socket(fd);
      exit(1);
    }

    if (listen(fd, 3) < 0) {
      perror("Init socket: listen");
      close_socket(fd);
      exit(1);
    }

    return fd;
  }

#if !defined(WIN32)
  Sigfunc *Signal(int signo, Sigfunc *func) {
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef SA_INERRUPT
      act.sa_flags |= SA_INETERRUPT;
#endif
    }
    else {
#ifdef SA_RESTART
      act.sa_flags |= SA_RESTART;
#endif
    }

    if (sigaction(signo, &act, &oact) < 0)
    return SIG_ERR;
    return (oact.sa_handler);
  }

  void sig_chld(int signo) {
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    ;

    /* Signals on Linux systems are reset to default after raise */
    Signal(SIGCHLD, sig_chld);
    return;
  }
#endif

  void init_signals() {
#if !defined(_DEBUG)
#if defined(WIN32)
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
#else
    struct itimerval itime;
    struct timeval interval;

    Signal(SIGBUS, sig_handler);
    Signal(SIGTERM, sig_handler);
    Signal(SIGABRT, sig_handler);
    Signal(SIGSEGV, sig_handler);
    Signal(SIGALRM, sig_handler);
    Signal(SIGCHLD, sig_chld);
    Signal(SIGPIPE, SIG_IGN);

    /* This is for deadlock protection. */
    interval.tv_sec = 180;
    interval.tv_usec = 0;
    itime.it_interval = interval;
    itime.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &itime, NULL);
    Signal(SIGVTALRM, sig_deadprotection);
#endif
#endif
  }

  void sig_handler(int sig) {
    static int signalled = 0;
    volatile int n = 0;

#if defined(WIN32)
    if (!signalled) {
      signalled++;
      switch (sig) {
      case SIGTERM:
        bug("Sig handler SIGTERM.", 0);
        break;
      case SIGSEGV:
        bug("Sig handler SIGSEGV.", 0);
        break;
      case SIGABRT:
        bug("Sig handler SIGABRT.", 0);
        break;
      case SIGFPE:
        bug("Sig handler SIGFPE.", 0);
        break;
      case SIGILL:
        bug("Sig handler SIGILL.", 0);
        break;
      }
      do_auto_shutdown();
    }
    else
    exit(0);
#else
    pid_t pid;

    if (!signalled) {
      signalled++;

      fflush(stdout);
      switch ((pid = fork())) {
        // Child process
      case 0:
        n = n / 0; /* Boom */
      case (pid_t)-1:
        log_string("Unable to fork child process, continuing with crash recovery");
        break;
      default:
        switch (sig) {
        case SIGALRM:
          bug("Sig handler SIGALRM.", 0);
          break;
        case SIGBUS:
          bug("Sig handler SIGBUS.", 0);
          break;
        case SIGTERM:
          bug("Sig handler SIGTERM.", 0);
          break;
        case SIGABRT:
          bug("Sig handler SIGABRT", 0);
          break;
        case SIGSEGV:
          bug("Sig handler SIGSEGV", 0);
          break;
        }
        do_auto_shutdown();
        break;
      }
    }
    else
    exit(0);
#endif
  }

  void sig_deadprotection(int sig) {
    if (!tics) {
      bug("Deadlock Protecion: tics not updated. (Infinite loop suspected)", 0);
      abort();
    }
    else
    tics = 0;
  }

  bool invalid_descriptor(DESCRIPTOR_DATA *d) {
    if (d->valid == TRUE)
    return FALSE;
    return TRUE;
  }

  void game_loop_unix(int control) {
    static struct timeval null_time;
    struct timeval last_time;

    // signal( SIGPIPE, SIG_IGN );
    gettimeofday(&last_time, NULL);
    current_time = (time_t)last_time.tv_sec;

    /* Main loop */
    while (!merc_down) {
      fd_set in_set;
      fd_set out_set;
      fd_set exc_set;
      int maxdesc;

#if defined(MALLOC_DEBUG)
      if (malloc_verify() != 1)
      abort();
#endif

      alarm(30);

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = NULL;
        d = *it;
        int count = 0;
        for (DescList::iterator id = descriptor_list.begin();
        id != descriptor_list.end(); ++id) {
          DESCRIPTOR_DATA *od = NULL;
          od = *id;
          if (d->host != NULL && od->host != NULL && !str_cmp(d->host, od->host))
          count++;
        }
        if (count >= 10)
        d->valid = FALSE;
      }

      /*
      * Poll all active descriptors.
      */
      descriptor_list.erase(std::remove_if(descriptor_list.begin(), descriptor_list.end(), invalid_descriptor), descriptor_list.end());

      //        std::remove_if(descriptor_list.begin(), descriptor_list.end(), //        invalid_descriptor);

      FD_ZERO(&in_set);
      FD_ZERO(&out_set);
      FD_ZERO(&exc_set);
      FD_SET(control, &in_set);
      maxdesc = control;

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end();) {
        DESCRIPTOR_DATA *d = NULL;
        d = *it;
        if ((*it) != NULL && it != descriptor_list.end())
        ++it;
        if (d == NULL)
        continue;
        if (d->valid == FALSE)
        continue;
        if(fcntl(d->descriptor, F_GETFD) == -1)
        {
          d->valid = FALSE;
          continue;
        }
        maxdesc = UMAX(maxdesc, d->descriptor);
        FD_SET(d->descriptor, &in_set);
        FD_SET(d->descriptor, &out_set);
        FD_SET(d->descriptor, &exc_set);

        //#if !defined(WIN32)
        if (d->ifd != -1 && d->ipid != -1)
        //#else
        //	if ( d->ifd != -1 && d->ipid != NULL )
        //#endif
        {
          maxdesc = UMAX(maxdesc, d->ifd);
          FD_SET(d->ifd, &in_set);
        }
      }

      if (select(maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0) {

        // Check if select was interrupted by a signal
        if (errno == EINTR) {
          perror("Game_loop: select was interrupted by a signal");
          continue; // Optionally retry select or handle as needed
        }
        else {
          // Log the error message based on the errno value
          perror("Game_loop: select error");
          save_game_stuff();

          // Instead of crashing, log the error and consider shutting down cleanly
          fprintf(stderr, "Select failed with errno %d: %s\n", errno, strerror(errno));

          // Handle the error or shutdown
          // do_auto_shutdown();
          // exit(1);
          break; // Break the loop or handle as needed
        }

        perror("Game_loop: select: poll");

        save_game_stuff();
        int x = 5;
        x = x / 0;
        //      do_auto_shutdown();
        //      exit(1);
      }

      /*
      * New connection?
      */
      if (FD_ISSET(control, &in_set))
      init_descriptor(control);

      /*
      * Kick out the freaky folks.
      */

      char lbuf[50];
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end();) {
        DESCRIPTOR_DATA *d = NULL;
        d = *it;
        if ((*it) != NULL && it != descriptor_list.end())
        it++;
        if (d == NULL)
        close_desc(d);
        if (d->valid == FALSE)
        continue;

        if (FD_ISSET(d->descriptor, &exc_set)) {
          FD_CLR(d->descriptor, &in_set);
          FD_CLR(d->descriptor, &out_set);
          if (d->character && d->connected == CON_PLAYING)
          save_char_obj(d->character, FALSE, FALSE);
          d->outtop = 0;
          close_desc(d);
        }
        else {
          d->fcommand = FALSE;

          if (FD_ISSET(d->descriptor, &in_set)) {
            if (d->character != NULL) {
              if (d->character->pcdata != NULL) {
                if (d->character->pcdata->secondary_timer == 0)
                d->character->timer = 0;
              }
              else
              d->character->timer = 0;

              d->character->idle = current_time;
            }

            if (!read_from_descriptor(d)) {
              log_string("DESCRIPTOR: No read from descriptor");
              FD_CLR(d->descriptor, &out_set);
              if (d->character != NULL && d->connected == CON_PLAYING)
              save_char_obj(d->character, FALSE, FALSE);
              d->outtop = 0;
              close_desc(d);
              continue;
            }
          }

          /* check for input from the ident */
          if ((d->connected == CON_IDENT_WAIT || CH(d) != NULL || d->connected == CON_PLAYING) && d->ifd != -1 && FD_ISSET(d->ifd, &in_set)) {
            process_ident(d);
          }

          if (d->character != NULL && d->character->wait > 0) {
            --d->character->wait;
            continue;
          }

          read_from_buffer(d);
          if (d->incomm[0] != '\0') {
            d->fcommand = TRUE;
            stop_idling(d->character);

            /* OLC */
            if (d->showstr_point) {
              log_string("DESCRIPTOR: running show_string.");
              show_string(d, d->incomm);
            }
            else if (d->pString) {
              log_string("DESCRIPTOR: running string_add.");
              string_add(d->character, d->incomm);
            }
            else {
              //                    log_string("DESCRIPTOR: running nanny.");

              switch (d->connected) {
              case CON_PLAYING:
              case CON_CREATION:
                if (!run_olc_editor(d))
                substitute_alias(d, d->incomm);
                break;
              default:
                nanny(d, d->incomm);
                break;
              }
            }
            d->incomm[0] = '\0';
          }
        }
      }

      for (DescList::iterator it = descriptor_list.begin(); it != descriptor_list.end();) {
        if ((*it)->valid == FALSE) {
          it = descriptor_list.erase(it);
        }
        else {
          ++it;
        }
      }


      /*
      * Process input.

      //	int k = 0;
      for(DescList::iterator it = descriptor_list.begin(); it !=
      descriptor_list.end(); )
      {
      DESCRIPTOR_DATA *d = NULL;
      if(*it == NULL)
      continue;
      d = *it;
      if((*it) != NULL && it != descriptor_list.end())
      it++;
      if(d == NULL)
      {

      continue;


      bug( "Descriptor", 0 );
      if(d->character != NULL)
      {
      char buf[MSL];
      sprintf(buf, "%s", d->character->name);
      bug(buf, 0);
      }


      if(d->valid != TRUE)
      {
      d->outtop   = 0;
      close_desc( d );
      continue;
      }

      d->fcommand	= FALSE;

      if ( FD_ISSET( d->descriptor, &in_set ) )
      {
      if ( d->character != NULL )
      {
      if ( d->character->pcdata != NULL )
      {
      if ( d->character->pcdata->secondary_timer == 0 )
      d->character->timer = 0;
      }
      else
      d->character->timer = 0;

      d->character->idle = current_time;
      }

      if ( !read_from_descriptor( d ) )
      {
      FD_CLR( d->descriptor, &out_set );
      if ( d->character != NULL && d->connected == CON_PLAYING)
      save_char_obj( d->character, FALSE, FALSE );
      d->outtop	= 0;
      close_desc( d );
      continue;
      }
      }

      // check for input from the ident
      if ( ( d->connected == CON_IDENT_WAIT
      ||     CH(d) != NULL
      ||     d->connected == CON_PLAYING ) && d->ifd != -1 && FD_ISSET( d->ifd, &in_set ) )
      process_ident( d );


      if ( d->character != NULL && d->character->wait > 0 )
      {
      --d->character->wait;
      continue;
      }

      read_from_buffer( d );
      if ( d->incomm[0] != '\0' )
      {
      d->fcommand	= TRUE;
      stop_idling( d->character );

      // OLC
      if ( d->showstr_point )
      show_string( d, d->incomm );
      else if ( d->pString )
      string_add( d->character, d->incomm );
      else
      {
      switch ( d->connected )
      {
      case CON_PLAYING:
      case CON_CREATION:
      if ( !run_olc_editor( d ) )
      substitute_alias( d, d->incomm );
      break;
      default:
      nanny( d, d->incomm );
      break;
      }
      }
      d->incomm[0]	= '\0';
      }
      }
      */
      /*
      * Autonomous game motion.
      */
      update_handler();

      /*
      * Output.
      */
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end();) {
        DESCRIPTOR_DATA *d = NULL;
        d = *it;
        if ((*it) != NULL && it != descriptor_list.end())
        ++it;
        if (d == NULL || d->valid == FALSE)
        continue;
        if ((d->fcommand || d->outtop > 0) && FD_ISSET(d->descriptor, &out_set)) {
          if (!process_output(d, TRUE)) {
            if (d->character != NULL && d->connected == CON_PLAYING)
            save_char_obj(d->character, FALSE, FALSE);
            d->outtop = 0;
            close_desc(d);
          }
        }
      }

      /*
      * Synchronize to a clock.
      * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
      * Careful here of signed versus unsigned arithmetic.
      */
      {
        struct timeval now_time;
        long secDelta;
        long usecDelta;

        gettimeofday(&now_time, NULL);
        usecDelta = ((int)last_time.tv_usec) - ((int)now_time.tv_usec) +
        1000000 / PULSE_PER_SECOND;
        secDelta = ((int)last_time.tv_sec) - ((int)now_time.tv_sec);
        while (usecDelta < 0) {
          usecDelta += 1000000;
          secDelta -= 1;
        }

        while (usecDelta >= 1000000) {
          usecDelta -= 1000000;
          secDelta += 1;
        }

        if (secDelta > 0 || (secDelta == 0 && usecDelta > 0)) {
          struct timeval stall_time;

          stall_time.tv_usec = usecDelta;
          stall_time.tv_sec = secDelta;
          if (select(0, NULL, NULL, NULL, &stall_time) < 0) {
            if (errno == EINTR)
            continue;
            perror("Game_loop: select: stall");
            exit(1);
          }
        }
      }

      gettimeofday(&last_time, NULL);
      current_time = (time_t)last_time.tv_sec;

      /* Update tics for deadlock protection. */
      tics++;
    }

    return;
  }

  void init_descriptor(int control) {
#if defined(_WIN32)
    static unsigned long ARGP = 1;
    int size;
#else
    socklen_t size;
#endif

    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    int desc;

    size = sizeof(sock);
    getsockname(control, (struct sockaddr *)&sock, &size);
    if ((desc = accept(control, (struct sockaddr *)&sock, &size)) < 0) {
      perror("New_descriptor: accept");
      return;
    }

#if defined(_WIN32)
    if (ioctlsocket(desc, FIONBIO, &ARGP) != 0) {
      bugf("[comm.c::init_descriptor] ioctlsocket returned error code %d", WSAGetLastError());
      return;
    }
#else
    if (fcntl(desc, F_SETFL, FNDELAY) == -1) {
      perror("New_descriptor: fcntl: FNDELAY");
      return;
    }
#endif

    /*
    * Cons a new descriptor.
    */
    dnew = new_descriptor();
    dnew->descriptor = desc;
    dnew->mxp = FALSE; // Initially MXP is off - Discordance
    dnew->connected = CON_IDENT_WAIT;
    dnew->ident = str_dup("???");
    dnew->ifd = -1;
    dnew->ipid = -1;

    size = sizeof(sock);

    if (getpeername(desc, (struct sockaddr *)&sock, &size) < 0) {
      perror("New_descriptor: getpeername");
      free_string(dnew->host);
      dnew->host = str_dup("(unknown)");
    }
    else {
      /*
      * Would be nice to use inet_ntoa here but it takes a struct arg, * which ain't very compatible between gcc and system libraries.
      */
      int addr;

      create_ident(dnew, sock.sin_addr.s_addr);
      addr = ntohl(sock.sin_addr.s_addr);
      sprintf(buf, "%d.%d.%d.%d", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr)&0xFF);
      sprintf(log_buf, "Sock.sinaddr, %d:  %s", desc, buf);
      log_string(log_buf);
      free_string(dnew->hostip);
      dnew->hostip = str_dup(buf);
    }

    /*
    * Init descriptor data.
    */
    descriptor_list.push_front(dnew);
    //    write_to_buffer( dnew, "Please wait while we do a quick hostname
    //    lookup...\n\r", 0);

    if (++num_descriptors > sysdata->maxplayers)
    sysdata->maxplayers = num_descriptors;
    if (sysdata->maxplayers > sysdata->alltimemax) {
      // Woo-woo.. most players we have ever had on.
      if (sysdata->time_of_max)
      free_string(sysdata->time_of_max);
      sprintf(buf, "%24.24s", ctime(&current_time));
      sysdata->time_of_max = str_dup(buf);
      sysdata->alltimemax = sysdata->maxplayers;
      sprintf(log_buf, "Broke all-time maximum player record: %d", sysdata->alltimemax);
      log_string(log_buf);
      save_sysdata();
    }

    return;
  }

  void close_socket(int fd) {
#if defined(_WIN32)
    closesocket(fd);
#else
    close(fd);
#endif
  }

  void close_file(int fd) {
#if defined(_WIN32)
    _close(fd);
#else
    close(fd);
#endif
  }

  void close_desc(DESCRIPTOR_DATA *dclose) {
    CHAR_DATA *ch;
    log_string("DESCRIPTOR: Closing desc");

    /*
    if(dclose->ansi < 2 && dclose->ansi > -1 && dclose->descriptor > -1 && dclose->connected > -1 && dclose->connected < 100 && dclose->valid > -1 && dclose->valid < 2)
    {
    */
    if (dclose->outtop > 0 && dclose->outsize < 32000)
    process_output(dclose, FALSE);

    if (dclose->snoop_by != NULL) {
      write_to_buffer(dclose->snoop_by, "Your victim has left the game.\n\r", 0);
    }

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      if ((*it)->snoop_by == dclose)
      (*it)->snoop_by = NULL;
    }

    if ((ch = dclose->character) != NULL) {
      sprintf(log_buf, "Closing link to %s.", ch->name);
      log_string(log_buf);
      ROOM_INDEX_DATA *room = ch->in_room;
      /* cut down on wiznet spam when rebooting */
      if (dclose->connected == CON_PLAYING && !merc_down) {
          if (room != NULL) {
            for (CharList::iterator it = room->people->begin();
            it != room->people->end(); ++it) {
              if ((*it) == ch)
              continue;
              if (!IS_NPC((*it)) && (*it)->desc != NULL) {
                if (!can_see((*it), ch) && !IS_IMMORTAL(ch)) {
                  printf_to_char((*it), "%s has lost their link.", PERS(ch, (*it)));
                }
              }
            }
          }
          if (!IS_FLAG(ch->act, PLR_SPYSHIELD) && !IS_IMMORTAL(ch))
          wiznet("Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0);

        ch->desc = NULL;
        if (IS_FLAG(ch->act, PLR_NOSAVE))
        free_char(ch);
      }
      else {
        free_char(dclose->original ? dclose->original : dclose->character);
      }
    }
    /*
    }
    */
    dclose->connected = CON_QUITTING;
    //    descriptor_list.remove(dclose);
    close_socket(dclose->descriptor);

    free_descriptor(dclose);
    dclose->valid = FALSE;
    --num_descriptors;
    //    dclose = NULL;
    return;
  }

  bool read_from_descriptor(DESCRIPTOR_DATA *d) {
    unsigned int iStart;

    /* Hold horses if pending command already. */
    if (d->incomm[0] != '\0')
    return TRUE;

    /* Check for overflow. */
    iStart = safe_strlen(d->inbuf);
    if (iStart >= sizeof(d->inbuf) - 10) {
      sprintf(log_buf, "%s input overflow!", d->host);
      log_string(log_buf);
      write_to_descriptor(d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
      return FALSE;
    }

    /* Snarf input. */
    for (;;) {
      int nRead;
#if defined(_WIN32)
      unsigned long nWaiting;

      ioctlsocket(d->descriptor, FIONREAD, &nWaiting);

      if (!nWaiting)
      break;

      nRead = recv(d->descriptor, d->inbuf + iStart, UMIN(nWaiting, sizeof(d->inbuf) - 10 - iStart), 0);
#else
      nRead =
      read(d->descriptor, d->inbuf + iStart, sizeof(d->inbuf) - 10 - iStart);
#endif

      if (nRead > 0) {
        eof_count = UMAX(0, eof_count-1);
        iStart += nRead;
        if (d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
        break;
      }
      else if (nRead == 0) {
        eof_count++;
        if(eof_count > 10)
        {
          save_game_stuff();
          eof_count = eof_count / 0;
        }
        log_string("EOF encountered on read.");
        return FALSE;
      }
      else if (WOULD_HAVE_BLOCKED)
      break;
      else {
        perror("Read_from_descriptor");
        return FALSE;
      }
    }

    d->inbuf[iStart] = '\0';
    return TRUE;
  }

  // MXP - Discordance
  /* set up MXP */
  void turn_on_mxp(DESCRIPTOR_DATA *d) {
    d->mxp = TRUE; /* turn it on now */
    write_to_buffer(d, (const char *)start_mxp_str, 0); // casting is naughty - Discordance
    write_to_buffer(d, MXP_MODE(6), 0); /* permanent secure mode */
  } /* end of turn_on_mxp */

  /*
  * Transfer one line from input buffer to input line.
  */
  void read_from_buffer(DESCRIPTOR_DATA *d) {
    int i, j, k;
    unsigned char *p; //        unsigned char * p; //MXP - Discordance

    /*
    * Hold horses if pending command already.
    */
    if (d->incomm[0] != '\0')
    return;

    // MXP - Discordance
    /*
    Look for incoming telnet negotiation
    */
    for (p = (unsigned char *)d->inbuf; *p; p++)
    if (*p == IAC) {
      if (memcmp(p, (const char *)do_mxp_str, safe_strlen((const char *)do_mxp_str)) == 0) {
        turn_on_mxp(d);
        /* remove string from input buffer */
        memmove(p, &p[safe_strlen((const char *)do_mxp_str)], safe_strlen((const char *)&p[safe_strlen((const char *)do_mxp_str)]) + 1);
        p--; /* adjust to allow for discarded bytes */
      }      /* end of turning on MXP */
      else if (memcmp(p, (const char *)dont_mxp_str, safe_strlen((const char *)dont_mxp_str)) == 0) {
        d->mxp = FALSE;
        /* remove string from input buffer */
        memmove(p, &p[safe_strlen((const char *)dont_mxp_str)], safe_strlen((const char *)&p[safe_strlen((const char *)dont_mxp_str)]) +
        1);
        p--; /* adjust to allow for discarded bytes */
      }      /* end of turning off MXP */
    }        /* end of finding an IAC */

    /*
    * Look for at least one new line.
    */
    for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++) {
      if (d->inbuf[i] == '\0')
      return;
    }

    /*
    * Canonical input processing.
    */
    for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++) {
      if (k >= MAX_INPUT_LENGTH - 2) {
        write_to_descriptor(d->descriptor, "Line too long.\n\r", 0);

        /* skip the rest of the line */
        for (; d->inbuf[i] != '\0'; i++) {
          if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
          break;
        }
        d->inbuf[i] = '\n';
        d->inbuf[i + 1] = '\0';
        break;
      }

      if (d->inbuf[i] == '\b' && k > 0)
      --k;
      else if (isascii(d->inbuf[i]) && isprint(d->inbuf[i]))
      d->incomm[k++] = d->inbuf[i];
    }

    /*
    * Finish off the line.
    */
    if (k == 0)
    d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
    * Deal with bozos with #repeat 1000 ...
    */

    if (k > 1 || d->incomm[0] == '!') {
      if (d->incomm[0] != '!' && strcmp(d->incomm, d->inlast)) {
        d->repeat = 0;
      }
      else {
        if (++d->repeat >= 25 && d->character && d->connected == CON_PLAYING && !spammer(d->character)) {
          sprintf(log_buf, "%s input spamming!", d->host);
          log_string(log_buf);
          wiznet("Spam spam spam $N spam spam spam spam spam!", d->character, NULL, WIZ_SPAM, 0, get_trust(d->character));
          if (d->incomm[0] == '!')
          wiznet(d->inlast, d->character, NULL, WIZ_SPAM, 0, get_trust(d->character));
          else
          wiznet(d->incomm, d->character, NULL, WIZ_SPAM, 0, get_trust(d->character));

          d->repeat = 0;
          /*
          write_to_descriptor( d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
          strcpy( d->incomm, "quit" );
          */
        }
      }
    }

    /*
    * Do '!' substitution.
    */
    if (d->incomm[0] == '!')
    strcpy(d->incomm, d->inlast);
    else
    strcpy(d->inlast, d->incomm);

    /*
    * Shift the input buffer.
    */
    while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
    i++;
    for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++)
    ;
    return;
  }

  bool auto_prompt(CHAR_DATA *ch) {
    if (ch->hit < max_hp(ch))
    return TRUE;

    if (in_fight(ch))
    return TRUE;

    if (IS_FLAG(ch->comm, COMM_AFK))
    return TRUE;

    if (!IS_NPC(ch) && ch->pcdata->victimize_vic_response_to > 0 && ch->pcdata->victimize_vic_timer > 0 && ch->pcdata->victimize_vic_select == 0)
    return TRUE;

    if (!IS_NPC(ch) && (ch->pcdata->ci_editing >= 1 && ch->pcdata->ci_editing <= 100)) {
      if (ch->pcdata->ci_absorb > 0) {
        ch->pcdata->ci_absorb = 0;
        return FALSE;
      }
      return TRUE;
    }
    return FALSE;
  }

  /*
    * Low level output function.
    */
  bool process_output(DESCRIPTOR_DATA *d, bool fPrompt) {
    extern bool merc_down;

    /*
    * Bust a prompt.
    */
    if (!merc_down) {
      if (d->showstr_point) {
        write_to_buffer(d, "[Hit Return to continue]\n\r", 0);
      }
      else if (fPrompt && d->pString && (d->connected == CON_PLAYING || d->connected == CON_CREATION)) {
        write_to_buffer(d, "# ", 2); // MXP FIX
      }
      else if (fPrompt && (d->connected == CON_PLAYING || d->connected == CON_CREATION)) {
        bool fprompt = FALSE;
        CHAR_DATA *ch;
        ch = d->character;
        ch = CH(d);
        if (!IS_NPC(ch) && (ch->pcdata->ci_editing >= 1 && ch->pcdata->ci_editing <= 100)) {
          fprompt = TRUE;
        }
        if (!IS_NPC(ch) && ch->pcdata->victimize_vic_response_to > 0 && ch->pcdata->victimize_vic_timer > 0 && ch->pcdata->victimize_vic_select == 0) {
          fprompt = TRUE;
        }

        if (!IS_FLAG(ch->comm, COMM_COMPACT))
        write_to_buffer(d, "\n\r", 2);
        if (IS_FLAG(ch->comm, COMM_PROMPT) && auto_prompt(d->character))
        bust_a_prompt(d->character);
        else if (fprompt == TRUE)
        bust_a_prompt(d->character);
        if (IS_FLAG(ch->comm, COMM_TELNET_GA))
        write_to_buffer(d, go_ahead_str, 0);
      }
    }

    /*
    * Short-circuit if nothing to write.
    */
    if (d->outtop == 0)
    return TRUE;

    /*
    * Snoop-o-rama.
    */
    if (d->snoop_by != NULL && !is_spyshield(d->character)) {
      if (d->character != NULL)
      write_to_descriptor(d->snoop_by->descriptor, d->character->name, 0);
      write_to_descriptor(d->snoop_by->descriptor, "# ", 2); // MXP FIX
      write_to_descriptor(d->snoop_by->descriptor, d->outbuf, d->outtop);
    }

    /*
    * OS-dependent output.
    */
    if (!write_to_descriptor(d->descriptor, d->outbuf, d->outtop)) {
      d->outtop = 0;
      return FALSE;
    }
    else {
      d->outtop = 0;
      return TRUE;
    }
  }

  _DOFUN(do_hp) { bust_a_prompt(ch); }

  /*
  * Bust a prompt (player settable prompt)
  * coded by Morgenes for Aldara Mud
  */
  char *ci_type(int type, CHAR_DATA *ch) {
    if (ch->pcdata->ci_editing == 1) {
      if (type == 0) {
        ch->pcdata->ci_type = 10;
        type = 10;
      }
      if (type == 10)
      return "Clothing.";
      if (type == 11)
      return "Jewelry.";
      if (type == 12)
      return "Food.";
      if (type == 13)
      return "Drink.";
      if (type == 14)
      return "Thing.";
      if (type == 15)
      return "Bag.";
      if (type == 16)
      return "Weapon.";
      if (type == 17)
      return "Gun.";
      if (type == 18)
      return "Phone.";
      if (type == 19)
      return "Armor.";
      if (type == 20)
      return "Furniture.";
      if (type == 21)
      return "Drugs.";
      if (type == 22)
      return "Umbrella.";
      if (type == 23)
      return "Perfume.";
      if (type == 24)
      return "Flashlight.";
      if (type == 25)
      return "Game.";
      if (type == 26)
      return "Playback.";

      return "None";
    }
    else if (ch->pcdata->ci_editing == 2) {
      if (type == 0) {
        ch->pcdata->ci_type = SECT_STREET;
        type = SECT_STREET;
      }
      if (type == SECT_CLUB)
      return "Club";
      if (type == SECT_RESTERAUNT)
      return "Restaurant";
      if (type == SECT_SHOP)
      return "Shop";
      if (type == SECT_STREET)
      return "Street";
      if (type == SECT_ALLEY)
      return "Alley";
      if (type == SECT_WAREHOUSE)
      return "Warehouse";
      if (type == SECT_COMMERCIAL)
      return "Commercial";
      if (type == SECT_PARK)
      return "Park";
      if (type == SECT_TUNNELS)
      return "Tunnels";
      if (type == SECT_CAFE)
      return "Cafe";
      if (type == SECT_ROOFTOP)
      return "Rooftop";
      if (type == SECT_BASEMENT)
      return "Basement";
      if (type == SECT_HOSPITAL)
      return "Hospital";
      if (type == SECT_HOUSE)
      return "House";
      if (type == SECT_BANK)
      return "Bank";
      if (type == SECT_AIR)
      return "Air";
      if (type == SECT_FOREST)
      return "Forest";
      if (type == SECT_BEACH)
      return "Beach";
      if (type == SECT_BEACH)
      return "Cave";

      return "None";
    }
    else if (ch->pcdata->ci_editing == 12) {
      if (type == OPERATION_INTERCEPT)
      return "Intercept";
      if (type == OPERATION_EXTRACT)
      return "Extract";
      if (type == OPERATION_CAPTURE)
      return "Capture";
      if (type == OPERATION_MULTIPLE)
      return "MultipleCapture";

      return "None";
    }

    return "None";
  }

  const char *fac_names_3[4] = {
    "", "Hand", "Order", "Temple"
  };

  const char *petition_types[4] = {
    "Bug", "Suggestion", "Request", "Renovate"
  };
  const char *grave_types[3] = {
    "Plaque", "Occupied Grave", "Unoccupied Grave"
  };
  const int grave_costs[3] = {
    500, 2500, 5000};
  const char *mission_types[4] = {
    "Control", "Instigate", "Diplomacy", "Liaise"
  };
  const char *adversary_types[2] = {
    "Modern", "Archaic"
  };
  const char *terrain_types[11] = {
    "Forest", "Field", "Desert",
    "Town", "City", "Mountains",
    "Warehouse", "Caves", "Village",
    "Tundra", "Lake"
  };
  const char *competition_types[3] = {
    "Open", "Restricted", "Closed"
  };
  const char *destiny_type_names[8] = {
    "", "Prey", "Predator", "Rival", "Social",
    "Adventure", "Investigation", "Work"
  };
  const char *content_names[3] = {
    "Maybe", "Yes", "No"
  };
  const char *dest_arch_names[12] = {
    "All", "Vampire", "Werewolf", "Demigod",
    "Demonborn", "Angelborn", "Faeborn", "Human",
    "Institute Staff", "None", "Institute Student",
    "Institute Patient"
  };
  const char *dest_conclude_names[3] = {
    "Both decide - Random chance breaks ties",
    "Role One Decides", "Role Two Decides"
  };
  const char *dest_rest_names[19] = {
    "None", "under T2", "under T3", "under T4",
    "under T5", "above T1", "above T2", "above T3",
    "above T4", "above T5", "Not allies", "allies",
    "sorcerer", "professional", "fighter",
    "under 30", "over 30"
  };

  const char *boon_names[12] = {
    "None", "Beauty", "Strength", "Lavish", "Gift",
    "Lover", "Silver Tongue", "Vitality", "Astral",
    "No Worries", "Bliss", "Renovate"
  };

  const char *curse_names[10] = {
    "None", "Gullibility", "Narcissism", "Revelaion",
    "Weakness", "Timid", "Puritanical", "Dependency",
    "Phobia", "Imprint"
  };

  const char *decree_types[6] = {
    "None", "Lockdown", "Border Control",
    "Construction Grant", "Sponsorship", "Overtax"
  };

  const char *storyidea_types[9] = {
    "Other", "Adventure", "Joint", "Competetive",
    "Character", "Quest", "Personal", "Mystery",
    "Crisis"
  };

  int blessing_cost(CHAR_DATA *ch, int bless, int curse)
  {
    if(bless == BOON_NOWORRIES || bless == BOON_BLISS)
    return 0;
    int bcost = 2500;
    for(int i=0;i<10;i++)
    {
      if(ch->pcdata->boon_history[i] == bless)
      bcost = bcost *13/10;
    }
    for(int i=0;i<10;i++)
    {
      if(ch->pcdata->curse_history[i] == curse)
      bcost = bcost *11/10;
    }

    FACTION_TYPE *cult = clan_lookup(ch->pcdata->eidilon_of);
    if(cult->axes[AXES_CORRUPT] >= AXES_NEUTRAL)
    bcost = bcost *3/2;

    return bcost;
  }

  int dest_scene_location_p(char *argument) {
    if (safe_strlen(argument) < 0)
    return -1;
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    return (atoi(arg));
  }
  char *dest_scene_location(char *string) {
    int point = dest_scene_location_p(string);

    if (point == -1)
    return "Role One current location";
    else if (point == -2)
    return "Role Two current location";
    else if (point == -3)
    return "Role One Home";
    else if (point == -4)
    return "Role Two Home";
    else if (point == -5)
    return "Patrol Room";
    else {
      return get_room_index(point)->name;
    }
  }

  int dest_scene_special_onep(char *argument) {
    if (safe_strlen(argument) < 0)
    return 0;
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    char argtwo[MSL];
    argument = one_argument_nouncap(argument, argtwo);
    return (atoi(argtwo));
  }
  int dest_scene_special_twop(char *argument) {
    if (safe_strlen(argument) < 0)
    return 0;
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    char argtwo[MSL];
    argument = one_argument_nouncap(argument, argtwo);
    char argthree[MSL];
    argument = one_argument_nouncap(argument, argthree);
    return (atoi(argthree));
  }
  char *dest_scene_specialone(char *string) {
    int point = dest_scene_special_onep(string);
    char outbuf[MSL];
    char onebuf[MSL];
    if (point == 0)
    return "None";
    if (point == 0)
    sprintf(onebuf, "%s", "");
    if (point == 1)
    sprintf(onebuf, "Branding");
    if (point == 2)
    sprintf(onebuf, "Helpless");
    if (point == 3)
    sprintf(onebuf, "Turn");
    if (point == 4)
    sprintf(onebuf, "Nowin");
    if (point == 5)
    sprintf(onebuf, "No Sanctuary");
    if (point == 6)
    sprintf(onebuf, "Stuck");
    if (point == 7)
    sprintf(onebuf, "Prey");
    if (point == 8)
    sprintf(onebuf, "Armed");
    if (point == 9)
    sprintf(onebuf, "Imprintable");
    if (point == 10)
    sprintf(onebuf, "Limited Sanctuary");

    sprintf(outbuf, "%s", onebuf);
    return str_dup(outbuf);
  }

  char *dest_scene_specialtwo(char *string) {
    int point = dest_scene_special_twop(string);
    char outbuf[MSL];
    char onebuf[MSL];
    if (point == 0)
    return "None";
    if (point == 0)
    sprintf(onebuf, "%s", "");
    if (point == 1)
    sprintf(onebuf, "Branding");
    if (point == 2)
    sprintf(onebuf, "Helpless");
    if (point == 3)
    sprintf(onebuf, "Turn");
    if (point == 4)
    sprintf(onebuf, "Nowin");
    if (point == 5)
    sprintf(onebuf, "No Sanctuary");
    if (point == 6)
    sprintf(onebuf, "Stuck");
    if (point == 7)
    sprintf(onebuf, "Prey");
    if (point == 8)
    sprintf(onebuf, "Armed");
    if (point == 9)
    sprintf(onebuf, "Imprintable");
    if (point == 10)
    sprintf(onebuf, "Limited Sanctuary");

    sprintf(outbuf, "%s", onebuf);
    return str_dup(outbuf);
  }

  int dest_scene_conclusion_p(char *argument) {
    if (safe_strlen(argument) < 0)
    return 0;
    char arg[MSL];
    argument = one_argument_nouncap(argument, arg);
    char argtwo[MSL];
    argument = one_argument_nouncap(argument, argtwo);
    char argthree[MSL];
    argument = one_argument_nouncap(argument, argthree);
    char argfour[MSL];
    argument = one_argument_nouncap(argument, argfour);
    return (atoi(argfour));
  }
  char *dest_scene_conclusion(char *string) {
    int point = dest_scene_conclusion_p(string);
    if (point == 0)
    return "No";
    return "Yes";
  }

  char *get_cistatus(CHAR_DATA *ch) {
    if (ch->pcdata->ci_status == 0)
    ch->pcdata->ci_status = ROOM_PUBLIC;

    if (ch->pcdata->ci_status == ROOM_PUBLIC)
    return "Public";
    else
    return "Private";

    return "None";
  }

  char *get_ciexit(CHAR_DATA *ch) {
    if (ch->in_room->area->vnum != ch->pcdata->ci_area)
    return "None";

    if (ch->pcdata->ci_x >= 0 && ch->pcdata->ci_x <= 9)
    return str_dup(dir_name[ch->pcdata->ci_x][0]);

    return "";
  }

  char *get_ciquad(CHAR_DATA *ch) {
    int x = ch->pcdata->ci_x;
    int y = ch->pcdata->ci_y;
    if (x == 0 && y == 0) {
      ch->pcdata->ci_x = ch->in_room->locx += number_range(-1, 1);
      ch->pcdata->ci_y = ch->in_room->locy += number_range(-1, 1);
      if (ch->pcdata->ci_x == ch->in_room->locx && ch->pcdata->ci_y == ch->in_room->locy)
      ch->pcdata->ci_x += 1;
    }
    int distx = ch->in_room->locx - ch->pcdata->ci_x;
    int disty = ch->in_room->locy - ch->pcdata->ci_y;
    distx = distx * distx;
    disty = disty * disty;
    int dist = distx + disty;
    dist = (int)sqrt((double)dist);
    if (dist < 2 && ch->in_room->area->vnum == ch->pcdata->ci_area) {
      return "Adjacent";
    }
    else {
      if (x >= 65 && y >= 65)
      return "NE";
      else if (x >= 65 && y < 40)
      return "SE";
      else if (x < 40 && y < 40)
      return "SW";
      else if (x < 40 && y >= 65)
      return "NW";
      else if (x >= 65)
      return "E";
      else if (x < 40)
      return "W";
      else if (y >= 65)
      return "N";
      else if (y < 40)
      return "S";
      else
      return "C";
    }
  }

  int ci_heightmod(int amount) {
    if (amount < 1 || amount > 19)
    return 0;

    return amount;
  }
  int ci_bustmod(int amount) {
    if (amount < 21 || amount > 40)
    return 0;

    return amount - 30;
  }

  char *skill_name(int vnum) {
    for (int i = 0; i < SKILL_USED; i++) {
      if (skill_table[i].vnum == vnum)
      return skill_table[i].name;
    }
    return "";
  }

  int making_minion_cost(CHAR_DATA *ch) {
    int shield_total = 0;
    int cost = 0;
    if (discipline_table[ch->pcdata->ci_discipline].range == -1)
    shield_total += ch->pcdata->ci_disclevel;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel);

    if (discipline_table[ch->pcdata->ci_discipline2].range == -1)
    shield_total += ch->pcdata->ci_disclevel2;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel2);

    if (discipline_table[ch->pcdata->ci_discipline3].range == -1)
    shield_total += ch->pcdata->ci_disclevel3;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel3);

    if (discipline_table[ch->pcdata->ci_discipline4].range == -1)
    shield_total += ch->pcdata->ci_disclevel4;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel4);

    if (discipline_table[ch->pcdata->ci_discipline5].range == -1)
    shield_total += ch->pcdata->ci_disclevel5;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel5);

    cost += built_disc_cost(shield_total);

    return cost;
  }

  int making_minion_shield(CHAR_DATA *ch) {
    int shield_total = 0;
    int cost = 0;
    if (discipline_table[ch->pcdata->ci_discipline].range == -1)
    shield_total += ch->pcdata->ci_disclevel;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel);

    if (discipline_table[ch->pcdata->ci_discipline2].range == -1)
    shield_total += ch->pcdata->ci_disclevel2;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel2);

    if (discipline_table[ch->pcdata->ci_discipline3].range == -1)
    shield_total += ch->pcdata->ci_disclevel3;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel3);

    if (discipline_table[ch->pcdata->ci_discipline4].range == -1)
    shield_total += ch->pcdata->ci_disclevel4;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel4);

    if (discipline_table[ch->pcdata->ci_discipline5].range == -1)
    shield_total += ch->pcdata->ci_disclevel5;
    else
    cost += built_disc_cost(ch->pcdata->ci_disclevel5);

    cost += built_disc_cost(shield_total);

    return shield_total;
  }

  void editing_prompt(CHAR_DATA *ch) {
    if (ch->pcdata->ci_editing == 1) {
      float value = (double)ch->pcdata->ci_cost;
      value /= 100;
      send_to_char("`BObject`x\n\r", ch);
      printf_to_char(ch, "Type: %s\n\r", ci_type(ch->pcdata->ci_type, ch));
      printf_to_char(ch, "Cost: %f\n\r", value);
      if (ch->pcdata->ci_type == 12)
      printf_to_char(ch, "Bites: %d\n\r", ch->pcdata->ci_size);
      else if (ch->pcdata->ci_type == 13)
      printf_to_char(ch, "Drinks: %d\n\r", ch->pcdata->ci_size);
      else if (ch->pcdata->ci_type == 23)
      printf_to_char(ch, "Uses: %d\n\r", ch->pcdata->ci_size);
      else
      printf_to_char(ch, "Size: %s\n\r", (ch->pcdata->ci_size == 0) ? "Small" : "Large");
      printf_to_char(ch, "Names: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Short: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Long: %s\n\r", ch->pcdata->ci_long);
      printf_to_char(ch, "Description:\n%s\n\r", ch->pcdata->ci_desc);
      printf_to_char(ch, "Wear: %s\n\r", ch->pcdata->ci_wear);

      if (ch->pcdata->ci_type == 13) {
        if (ch->pcdata->ci_alcohol == liq_lookup("whisky"))
        printf_to_char(ch, "Alcohol Content: Large\n\r");
        else if (ch->pcdata->ci_alcohol == liq_lookup("beer"))
        printf_to_char(ch, "Alcohol Content: Small\n\r");
        else
        printf_to_char(ch, "Alcohol Content: None\n\r");
      }
      if (ch->pcdata->ci_type == 12 || ch->pcdata->ci_type == 13) {
        printf_to_char(ch, "Taste: %s\n\r", ch->pcdata->ci_taste);
      }
      if (ch->pcdata->ci_type == 21) {
        printf_to_char(ch, "Effect: %s\n\r", ch->pcdata->ci_taste);
        printf_to_char(ch, "Imprint: %s\n\r", ch->pcdata->ci_imprint);
      }
      if (ch->pcdata->ci_type == 23) {
        printf_to_char(ch, "Scent: %s\n\r", ch->pcdata->ci_taste);
      }

      if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 19) {
        printf_to_char(ch, "Cover: %s\n\r", get_cover_list(ch->pcdata->ci_covers));
        printf_to_char(ch, "Zip: %s\n\r", get_cover_list(ch->pcdata->ci_zips));
      }
      if (ch->pcdata->ci_type == 11) {
        printf_to_char(ch, "Cover: %s\n\r", get_cover_list(ch->pcdata->ci_covers));
      }
      if (ch->pcdata->ci_type == 10 || ch->pcdata->ci_type == 11)
      printf_to_char(ch, "Layer: %d\n\r", ch->pcdata->ci_layer);
      if (ch->pcdata->ci_type == 10) {
        printf_to_char(ch, "Height Mod: %d\n\r", ci_heightmod(ch->pcdata->ci_mod));
        printf_to_char(ch, "Bust Mod: %d\n\r", ci_bustmod(ch->pcdata->ci_mod));
      }

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 2) {
      send_to_char("`BRoom`x\n\r", ch);
      if (ch->pcdata->ci_area == 0)
      ch->pcdata->ci_area = 12;

      for (int i = 0; i <= MAX_DIST; i++) {
        if (district_table[i].area == ch->pcdata->ci_area) {
          printf_to_char(ch, "Location: %s\n\r", district_table[i].name);
        }
      }
      printf_to_char(ch, "Status: %s\n\r", get_cistatus(ch));
      printf_to_char(ch, "Type: %s\n\r", ci_type(ch->pcdata->ci_type, ch));
      printf_to_char(ch, "Name: %s\n\r", ch->pcdata->ci_name);
      //	if(timetraveler(ch))
      //	    printf_to_char(ch, "Year: %d\n\r", ch->pcdata->ci_cost);

      if (ch->pcdata->ci_status == ROOM_PUBLIC || ch->pcdata->ci_status == 0)
      printf_to_char(ch, "Quadrant: %s\n\r", get_ciquad(ch));
      else
      printf_to_char(ch, "Exit: %s\n\r", get_ciexit(ch));

      printf_to_char(ch, "Description:\n%s\n\r", ch->pcdata->ci_desc);

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 3) {
      send_to_char("`BAttack`x\n\r", ch);
      printf_to_char(ch, "Name: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Discipline: %s\n\r", get_disc_string(ch->pcdata->ci_discipline));
      printf_to_char(ch, "Special: %s\n\r", get_special_string(ch->pcdata->ci_special));
      printf_to_char(ch, "Myself: %s\n\r", ch->pcdata->ci_myself);
      printf_to_char(ch, "Target: %s\n\r", ch->pcdata->ci_target);
      printf_to_char(ch, "Bystanders: %s\n\r", ch->pcdata->ci_bystanders);
      if (ch->pcdata->ci_special == SPECIAL_DELAY || ch->pcdata->ci_special == SPECIAL_DELAY2) {
        printf_to_char(ch, "Myselfdelayed: %s\n\r", ch->pcdata->ci_myselfdelayed);
        printf_to_char(ch, "Targetdelayed: %s\n\r", ch->pcdata->ci_targetdelayed);
        printf_to_char(ch, "Bystandersdelayed: %s\n\r", ch->pcdata->ci_bystandersdelayed);
      }

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 4) {
      send_to_char("`BMonster`x\n\r", ch);
      printf_to_char(ch, "Name: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Short: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Description:\n%s\n\r", ch->pcdata->ci_desc);
      printf_to_char(ch, "Discipline: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline), ch->pcdata->ci_disclevel);
      printf_to_char(ch, "Discipline2: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline2), ch->pcdata->ci_disclevel2);
      printf_to_char(ch, "Discipline3: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline3), ch->pcdata->ci_disclevel3);
      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 5) {
      send_to_char("`BMinion`x\n\r", ch);
      printf_to_char(ch, "Name: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Short: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Description:\n%s\n\r", ch->pcdata->ci_desc);
      printf_to_char(ch, "Spent   Exp: %d\n\r", making_minion_cost(ch));
      printf_to_char(ch, "Maximum Exp: %d\n\r", minion_exp_cap(ch));
      printf_to_char(ch, "Current Def: %d\n\r", making_minion_shield(ch));
      printf_to_char(ch, "Maximum Def: %d\n\r", minion_shield_cap(ch));
      printf_to_char(ch, "Discipline: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline), ch->pcdata->ci_disclevel);
      printf_to_char(ch, "Discipline2: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline2), ch->pcdata->ci_disclevel2);
      printf_to_char(ch, "Discipline3: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline3), ch->pcdata->ci_disclevel3);
      printf_to_char(ch, "Discipline4: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline4), ch->pcdata->ci_disclevel4);
      printf_to_char(ch, "Discipline5: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline5), ch->pcdata->ci_disclevel5);
      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 6) {
      send_to_char("`BAlly`x\n\r", ch);
      printf_to_char(ch, "Name: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Short: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Description:\n%s\n\r", ch->pcdata->ci_desc);
      printf_to_char(ch, "Spent   Exp: %d\n\r", making_minion_cost(ch));
      printf_to_char(ch, "Maximum Exp: %d\n\r", muscle_exp_cap(ch));
      printf_to_char(ch, "Discipline: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline), ch->pcdata->ci_disclevel);
      printf_to_char(ch, "Discipline2: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline2), ch->pcdata->ci_disclevel2);
      printf_to_char(ch, "Discipline3: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline3), ch->pcdata->ci_disclevel3);
      printf_to_char(ch, "Discipline4: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline4), ch->pcdata->ci_disclevel4);
      printf_to_char(ch, "Discipline5: %s %d\n\r", get_disc_string(ch->pcdata->ci_discipline5), ch->pcdata->ci_disclevel5);

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 7) {
      send_to_char("`BNews`x\n\r", ch);
      printf_to_char(ch, "Message:\n%s\n\r", ch->pcdata->ci_desc);
      send_to_char("Stats: ", ch);
      if (ch->pcdata->ci_stats[0] == -2) {
        send_to_char("Supernatural News.\n\r", ch);
      }
      else {
        for (int i = 0; i < 10; i++) {
          if (ch->pcdata->ci_stats[i] > 0) {
            if (ch->pcdata->ci_stats[i] < 99)
            printf_to_char(ch, "%s, ", skill_name(ch->pcdata->ci_stats[i]));
            else
            printf_to_char(ch, "%s, ", fac_names_3[ch->pcdata->ci_stats[i] - 100]);
          }
        }
        send_to_char("\n\r", ch);
      }
      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);

    }
    else if (ch->pcdata->ci_editing == 8) {
      send_to_char("`BResearch`x\n\r", ch);
      printf_to_char(ch, "Title: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Keywords: %s\n\r", ch->pcdata->ci_taste);
      printf_to_char(ch, "Text: %s\n\r", ch->pcdata->ci_desc);

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 9) {
      send_to_char("`BPetition`x\n\r", ch);
      printf_to_char(ch, "Type: %s\n\r", petition_types[ch->pcdata->ci_mod]);
      printf_to_char(ch, "To: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Description: \n%s\n\r", ch->pcdata->ci_desc);
      send_to_char("\n\r", ch);

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 10) {
      send_to_char("`DGrave`x\n\r", ch);
      printf_to_char(ch, "Type: %s\n\r", grave_types[ch->pcdata->ci_mod]);
      printf_to_char(ch, "Cost: %d\n\r", grave_costs[ch->pcdata->ci_mod]);
      printf_to_char(ch, "Firstname: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Surname: %s\n\r", ch->pcdata->ci_taste);
      printf_to_char(ch, "Description: \n%s\n\r", ch->pcdata->ci_desc);
      send_to_char("\n\r", ch);

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 11) {
      send_to_char("`rMission`x\n\r", ch);
      printf_to_char(ch, "Territory: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Type: %s\n\r", mission_types[ch->pcdata->ci_mod]);
      printf_to_char(ch, "Description: %s\n\r", ch->pcdata->ci_desc);
      send_to_char("\n\r", ch);

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 12) {
      send_to_char("`rOperation`x\n\r", ch);
      if (ch->pcdata->ci_discipline2 == GOAL_PSYCHIC) {
        printf_to_char(ch, "Target: %s\n\r", ch->pcdata->ci_message);
        printf_to_char(ch, "Terrain: %s\n\r", terrain_types[ch->pcdata->ci_cost]);
        printf_to_char(ch, "Priority: %s\n\r", ch->pcdata->ci_bystanders);
        printf_to_char(ch, "Speed: %d\n\r", ch->pcdata->ci_zips);
        printf_to_char(ch, "Hour: %d\n\r", ch->pcdata->ci_alcohol);
        printf_to_char(ch, "Day: %d\n\r", ch->pcdata->ci_vnum);
        send_to_char("\n\r", ch);
      }
      else {
        printf_to_char(ch, "Description: %s\n\r", ch->pcdata->ci_desc);
        printf_to_char(ch, "AdversaryName: %s\n\r", ch->pcdata->ci_long);
        if (ch->pcdata->ci_mod > 1)
        ch->pcdata->ci_mod = 0;
        printf_to_char(ch, "AdversaryType: %s\n\r", adversary_types[ch->pcdata->ci_mod]);
        printf_to_char(ch, "AreaName: %s\n\r", ch->pcdata->ci_name);
        printf_to_char(ch, "Territory: %s\n\r", ch->pcdata->ci_short);
        printf_to_char(ch, "Type: %s\n\r", ci_type(ch->pcdata->ci_type, ch));
        if (ch->pcdata->ci_type == OPERATION_EXTRACT || ch->pcdata->ci_type == OPERATION_INTERCEPT)
        printf_to_char(ch, "ItemName: %s\n\r", ch->pcdata->ci_myself);
        else if (ch->pcdata->ci_type == OPERATION_CAPTURE || ch->pcdata->ci_type == OPERATION_MULTIPLE)
        printf_to_char(ch, "Capture Process Name: %s\n\r", ch->pcdata->ci_target);
        if (ch->pcdata->ci_discipline2 > 20)
        ch->pcdata->ci_discipline2 = 0;
        printf_to_char(ch, "Goal: %s\n\r", goal_names[ch->pcdata->ci_discipline2]);
        if (ch->pcdata->ci_discipline2 == GOAL_ASSASSINATE || ch->pcdata->ci_discipline2 == GOAL_RAZE || ch->pcdata->ci_discipline2 == GOAL_KIDNAP || ch->pcdata->ci_discipline2 == GOAL_RESCUE)
        printf_to_char(ch, "Target: %s\n\r", ch->pcdata->ci_message);
        printf_to_char(ch, "Storyline: %s\n\r", ch->pcdata->ci_myselfdelayed);
        if (ch->pcdata->ci_cost > 10)
        ch->pcdata->ci_cost = 0;
        printf_to_char(ch, "Terrain: %s\n\r", terrain_types[ch->pcdata->ci_cost]);
        printf_to_char(ch, "MaximumCharacters: %d\n\r", ch->pcdata->ci_covers);
        if (ch->pcdata->ci_discipline > 2)
        ch->pcdata->ci_discipline = 0;
        printf_to_char(ch, "Competition: %s\n\r", competition_types[ch->pcdata->ci_discipline]);
        printf_to_char(ch, "Challenge: %d\n\r", ch->pcdata->ci_disclevel);
        printf_to_char(ch, "Storyrunners: %s\n\r", ch->pcdata->ci_taste);
        printf_to_char(ch, "Priority: %s\n\r", ch->pcdata->ci_bystanders);
        printf_to_char(ch, "Speed: %d\n\r", ch->pcdata->ci_zips);
        printf_to_char(ch, "Hour: %d\n\r", ch->pcdata->ci_alcohol);
        printf_to_char(ch, "Day: %d\n\r", ch->pcdata->ci_vnum);
        send_to_char("\n\r", ch);
      }

      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 15) {
      send_to_char("`WForm Recommending Arrest for reasons of Mental Health`x\n\r", ch);
      printf_to_char(ch, "Target: %s\n\r", ch->pcdata->ci_message);
      printf_to_char(ch, "Statement: %s\n\r", ch->pcdata->ci_desc);
      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if (ch->pcdata->ci_editing == 16) {
      send_to_char("`BDestiny`x\n\r", ch);

      printf_to_char(ch, "Premise: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "RoleOneName: %s\n\r", ch->pcdata->ci_message);
      printf_to_char(ch, "RoleTwoName: %s\n\r", ch->pcdata->ci_bystanders);
      printf_to_char(ch, "Conclusion: %s\n\r", dest_conclude_names[ch->pcdata->ci_disclevel]);
      printf_to_char(ch, "RoleOneArchetypes: %s\n\r", dest_arch_names[ch->pcdata->ci_zips]);
      printf_to_char(ch, "RoleTwoArchetypes: %s\n\r", dest_arch_names[ch->pcdata->ci_alcohol]);
      printf_to_char(ch, "RoleOneRestrictions: %s\n\r", dest_rest_names[ch->pcdata->ci_discipline]);
      printf_to_char(ch, "RoleTwoRestrictions: %s\n\r", dest_rest_names[ch->pcdata->ci_vnum]);
      for (int i = 0; i < 20; i++) {
        if (i == 0 || safe_strlen(ch->pcdata->ci_includes[i - 1]) > 2) {
          printf_to_char(ch, "\nScene%02ddesc: %s\n\r", i + 1, ch->pcdata->ci_includes[i]);
          printf_to_char(ch, "Scene%02dlocation: %s\n\r", i + 1, dest_scene_location(ch->pcdata->ci_excludes[i]));
          printf_to_char(ch, "Scene%02dspecialroleone: %s\n\r", i + 1, dest_scene_specialone(ch->pcdata->ci_excludes[i]));
          printf_to_char(ch, "Scene%02dspecialroletwo: %s\n\r", i + 1, dest_scene_specialtwo(ch->pcdata->ci_excludes[i]));
          printf_to_char(ch, "Scene%02dconclusion: %s\n\r", i + 1, dest_scene_conclusion(ch->pcdata->ci_excludes[i]));
        }
      }
      send_to_char("\n\r", ch);
      send_to_char("Type '`WDone`x' when you're finished, or '`WAbort`x' to abort.\n\r", ch);
    }
    else if(ch->pcdata->ci_editing == 17) {
      send_to_char("`CBlessing`x\n\r", ch);
      printf_to_char(ch, "Target: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Boon: %s\n\r", boon_names[ch->pcdata->ci_disclevel]);
      printf_to_char(ch, "Curse: %s\n\r", curse_names[ch->pcdata->ci_zips]);
      printf_to_char(ch, "Cost: $%d\n\r", blessing_cost(ch, ch->pcdata->ci_disclevel, ch->pcdata->ci_zips));
    }
    else if(ch->pcdata->ci_editing == 18) {
      send_to_char("`CEidolon Encounter`x\n\r", ch);
      printf_to_char(ch, "Eidolon: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Base: %s\n\r", encounter_prompt(ch, ch->pcdata->ci_discipline).c_str());
      printf_to_char(ch, "Prompt: %s\n\r", ch->pcdata->ci_desc);

    }
    else if(ch->pcdata->ci_editing == 19) {
      send_to_char("`CEidolon Patrol`x\n\r", ch);
      printf_to_char(ch, "Eidolon: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Introduction: %s\n\r", ch->pcdata->ci_desc);
      printf_to_char(ch, "Conclusion: %s\n\r", ch->pcdata->ci_bystanders);
      printf_to_char(ch, "Messages: \n%s\n\r", ch->pcdata->ci_message);
    }

    else if(ch->pcdata->ci_editing == 20) {
      ANNIVERSARY_TYPE *ann;
      for (vector<ANNIVERSARY_TYPE *>::iterator it = AnniversaryVect.begin();
      it != AnniversaryVect.end(); ++it) {
        if ((*it)->id == ch->pcdata->ci_discipline) {
          ann = *it;
          break;
        }
      }
      if(ann != NULL)
      {
        send_to_char("`CAnniversary`x\n\r", ch);
        printf_to_char(ch, "Eidolon: %s\n\r", ann->eidilon_name);
        printf_to_char(ch, "Summary: %s\n\r", ann->summary);
        printf_to_char(ch, "News: %s\n\r", ann->news);
        printf_to_char(ch, "Messages: \n%s\n\r", ann->messages);
      }
    }
    else if(ch->pcdata->ci_editing == 21) {
      send_to_char("`CDecree`x\n\r", ch);
      printf_to_char(ch, "Type: %s\n\r", decree_types[ch->pcdata->ci_discipline]);
      LOCATION_TYPE *op = territory_by_number(ch->pcdata->ci_disclevel);
      if(op != NULL)
      printf_to_char(ch, "Territory: %s\n\r", op->name);
      else
      printf_to_char(ch, "Territory:\n\r");
      printf_to_char(ch, "Target: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Description: \n%s\n\r", ch->pcdata->ci_desc);

    }
    else if(ch->pcdata->ci_editing == 22)
    {
      send_to_char("`cMyHaven Profile`x\n\r", ch);
      PROFILE_TYPE *profile = profile_lookup(ch->name);
      if(profile == NULL)
      {
        profile = new_profile();
        free_string(profile->name);
        profile->name = str_dup(ch->name);
        profile->last_active = current_time;
        ProfileVect.push_back(profile);
      }
      printf_to_char(ch, "Handle: %s\n\r", profile->handle);
      printf_to_char(ch, "AboutMe: \n%s\n\r", profile->profile);
      if(safe_strlen(profile->photo) > 2)
      printf_to_char(ch, "Photo: (Taken)\n\r");
      else
      printf_to_char(ch, "Photo: (Not Taken)\n\r");
      printf_to_char(ch, "Quote: %s\n\r", profile->quote);
      printf_to_char(ch, "Joke: %s\n\r", profile->joke);
      printf_to_char(ch, "AttractedTo: %s\n\r", profile->prefs);
      send_to_char("Type '`Wdone`x' when finished.\n\r", ch);
    }
    else if(ch->pcdata->ci_editing == 23)
    {
      send_to_char("`cStoryidea`x\n\r", ch);
      printf_to_char(ch, "Type: %s\n\r", storyidea_types[ch->pcdata->ci_discipline]);
      printf_to_char(ch, "Storyline: %s\n\r", ch->pcdata->ci_short);
      printf_to_char(ch, "Description: %s\n\r", ch->pcdata->ci_desc);
      printf_to_char(ch, "Eidolon: %s\n\r", ch->pcdata->ci_myselfdelayed);
      printf_to_char(ch, "NPCs: %s\n\r", ch->pcdata->ci_message);
      printf_to_char(ch, "Lore: %s\n\r", ch->pcdata->ci_bystanders);
      printf_to_char(ch, "Territory: %s\n\r", ch->pcdata->ci_name);
      printf_to_char(ch, "Secrets: %s\n\r", ch->pcdata->ci_long);
      printf_to_char(ch, "Subfactionone: %s\n\r", ch->pcdata->ci_myself);
      printf_to_char(ch, "Subfactiontwo: %s\n\r", ch->pcdata->ci_target);
      send_to_char("Type '`Wdone`x' when finished.\n\r", ch);
    }
  }

  void bust_a_prompt(CHAR_DATA *ch) {
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    const char *str;
    const char *i;
    char *point;
    EXIT_DATA *pexit;
    int door;
    int health;

    if (!IS_NPC(ch) && (ch->pcdata->ci_editing >= 1 && ch->pcdata->ci_editing <= 100)) {
      editing_prompt(ch);
      return;
    }
    if (!IS_NPC(ch) && ch->pcdata->victimize_vic_response_to > 0 && ch->pcdata->victimize_vic_timer > 0 && ch->pcdata->victimize_vic_select == 0) {
      victim_prompt(ch);
      return;
    }
    if(!IS_NPC(ch))
    summary_prompt(ch);

    point = buf;
    str = ch->prompt;
    if (!str || str[0] == '\0') {
      sprintf(buf, "`p[%dhp]`x %s", // MXP FIX
      ch->hit, ch->prefix);
      send_to_char(buf, ch);
      return;
    }

    if (IS_FLAG(ch->comm, COMM_AFK)) {
      send_to_char("`x[`pAFK`x] ", ch); // MXP FIX
      return;
    }

    if (ch->desc->editor) {
      AREA_DATA *pArea;
      ROOM_INDEX_DATA *pRoom;
      OBJ_INDEX_DATA *pObj;
      MOB_INDEX_DATA *pMob;
      HELP_DATA *pHelp;
      PROG_CODE *pMprog;
      //                CHAR_DATA       *target;
      GROUP_INDEX_DATA *pGroup;

      char buf1[MSL];

      buf1[0] = '\0';
      switch (ch->desc->editor) {
      case ED_AREA:
        pArea = (AREA_DATA *)ch->desc->pEdit;
        //              pArea = (AREA_DATA *)ch->in_room->area;
        sprintf(buf1, "`YEditing Area: `R%d`x\r\n", pArea ? pArea->vnum : 0);
        break;
      case ED_ROOM:
        pRoom = ch->in_room;
        sprintf(buf1, "`CEditing Room: `R%d`x\r\n", pRoom ? pRoom->vnum : 0);
        break;
      case ED_OBJECT:
        pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
        sprintf(buf1, "`GEditing Object: `R%d`x\r\n", pObj ? pObj->vnum : 0);
        break;
      case ED_MOBILE:
        pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
        sprintf(buf1, "`MEditing Mob: `R%d`x\r\n", pMob ? pMob->vnum : 0);
        break;
      case ED_MPCODE:
        pMprog = (PROG_CODE *)ch->desc->pEdit;
        sprintf(buf1, "`CEditing Mob Program: `R%d`x\r\n", pMprog ? pMprog->vnum : 0);
        break;
      case ED_OPCODE:
        pMprog = (PROG_CODE *)ch->desc->pEdit;
        sprintf(buf1, "`CEditing Object Program: `R%d`x\r\n", pMprog ? pMprog->vnum : 0);
        break;
      case ED_RPCODE:
        pMprog = (PROG_CODE *)ch->desc->pEdit;
        sprintf(buf1, "`CEditing Room Program: `R%d`x\r\n", pMprog ? pMprog->vnum : 0);
        break;
      case ED_HELP:
        pHelp = (HELP_DATA *)ch->desc->pEdit;
        sprintf(buf1, "`BEditing Help: `R%s`x\r\n", pHelp ? pHelp->keyword : "!NULL!");
        break;
      case ED_GUILD:
        break;
      case ED_VESSEL:
        break;
      case ED_PORT:
        break;
      case ED_GROUP:
        pGroup = (GROUP_INDEX_DATA *)ch->desc->pEdit;
        sprintf(buf1, "`YEditing Group: `R%d`x\r\n", pGroup ? pGroup->vnum : -1);
        break;
      case ED_SUBRACE:
        break;
      case ED_LANDMARK:
        break;
      case ED_ENTRANCE:
        break;
      case ED_NEWTURN:
        // target = (CHAR_DATA *)ch->desc->pEdit;
        sprintf(buf1, "Commands `R( `Wname, lastname, subrace, show, done. `R)`x");
        break;
      case ED_CREATION:
        // target = (CHAR_DATA *)ch->desc->pEdit;
        sprintf(buf1, "Other commands: back (move back a step), show (show current stats)\n\r%s", "");
        break;
      case ED_CREATION_SK:
        // target = (CHAR_DATA *)ch->desc->pEdit;

        sprintf(buf1, "%s\n\r%s\n\r", "Choices `R( `Wadd, drop, list, defensive, intelligence, offensive, rogue", "          survival,  trades, weapons, channeler `Y*`Blearned`Y*`x `R)`x");
      default:
        sprintf(buf1, "!UNKNOWN EDITOR!\r\n");
        break;
      }
      send_to_char(buf1, ch);
      return;
    }

    while (*str != '\0') {
      if (*str != '%') {
        *point++ = *str++;
        continue;
      }
      ++str;

      buf2[0] = '\0';

      switch (*str) {
      default:
        i = " ";
        break;

      case 'T': {
          char str_mod_time[25];
          time_t com_time;
          com_time = current_time + (ch->pcdata->jetlag * 3600);
          strncpy(str_mod_time, ctime(&com_time), 24);
          str_mod_time[24] = '\0';
          sprintf(buf2, "%s", str_mod_time);
          i = buf2;
          break;
        }
      case 'a':
        sprintf(buf2, "%d:%02d %s", (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12, time_info.minute, time_info.hour >= 12 ? "PM" : "AM");
        i = buf2;
        break;
      case 'A':
        if (time_info.hour == 4 && time_info.minute >= 30)
        sprintf(buf2, "`R");
        else if ((time_info.hour >= 5 && time_info.hour < 17) || (time_info.hour == 17 && time_info.minute < 30))
        sprintf(buf2, "`C");
        else if (time_info.hour < 18 && time_info.hour >= 17)
        sprintf(buf2, "`r");
        else
        sprintf(buf2, "`D");
        i = buf2;
        break;
      case 'b':
        for (door = 0; door < MAX_DIR; door++) {
          if ((pexit = ch->in_room->exit[door]) != NULL && pexit->u1.to_room != NULL && pexit->u1.to_room != NULL && can_see_room(ch, pexit->u1.to_room) && !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->exit_info, EX_HIDDEN)) {
            strcat(buf2, " ");
            strcat(buf2, dir_name[door][1]);
          }
        }
        i = buf2;
        break;
      case 'c':
        sprintf(buf2, "%s", "\n\r");
        i = buf2;
        break;
      case 'd':
        health = (int)((ch->hit * 100) / max_hp(ch));
        if (health >= 80)
        sprintf(buf2, "`g");
        else if (health >= 60)
        sprintf(buf2, "`y");
        else if (health >= 40)
        sprintf(buf2, "`Y");
        else if (health >= 20)
        sprintf(buf2, "`r");
        else
        sprintf(buf2, "`R");
        i = buf2;
        break;

      case 'h':
        sprintf(buf2, "%d", ch->hit);
        i = buf2;
        break;
      case 'H':
        sprintf(buf2, "%d", max_hp(ch));
        i = buf2;
        break;
      case 'l':
        sprintf(buf2, "%d", ch->wounds);
        i = buf2;
        break;
      case 'L':
        sprintf(buf2, "%d", ch->debuff);
        i = buf2;
        break;
      case 'g':
        sprintf(buf2, "%d", ch->wait / PULSE_PER_SECOND);
        i = buf2;
        break;
      case 'v':
        if (IS_IMMORTAL(ch) && ch->in_room != NULL)
        sprintf(buf2, "%d", ch->in_room->vnum);
        else
        sprintf(buf2, " ");
        i = buf2;
        break;
      case 'z':
        if (IS_IMMORTAL(ch) && ch->in_room != NULL)
        sprintf(buf2, "%s", ch->in_room->area->name);
        else
        sprintf(buf2, " ");
        i = buf2;
        break;
      case '%':
        sprintf(buf2, "%%");
        i = buf2;
        break;
      case 'f':
        if (in_fight(ch))
        sprintf(buf2, "%s", capitalize(dir_name[ch->facing][0]));
        else
        sprintf(buf2, " ");
        i = buf2;
        break;
      case 'F':
        if (in_fight(ch) && ch->in_room != NULL)
        sprintf(buf2, "%s %d/%d:%d/%d", capitalize(dir_name[ch->facing][1]), ch->x, ch->in_room->size, ch->y, ch->in_room->size);
        else
        sprintf(buf2, " ");
        i = buf2;
        break;
      case 'p':
        if (ch->position == POS_RESTING)
        sprintf(buf2, "`x(`CSprawled`x)");
        else if (ch->position == POS_SITTING)
        sprintf(buf2, "`x(`WSitting`x)");
        else if (ch->position == POS_SLEEPING)
        sprintf(buf2, "`x(`BSleeping`x)");
        if (IS_FLAG(ch->act, PLR_MONSTER))
        sprintf(buf2, "%s `x(`RShifted`x)", buf2);
        if (get_energy(ch) >= 100)
        sprintf(buf2, "%s `x[`yFatigued`x]", buf2);
        if (ch->bagcarrier == 1 && ch->in_room != NULL && battleground(ch->in_room))
        sprintf(buf2, "%s `x(`WCarrier`x)", buf2);

        i = buf2;
        break;
      case 'w':
        if (IS_IMMORTAL(ch) && ch->invis_level > 0)
        sprintf(buf2, "`x(`bW%d`x) ", ch->invis_level);
        i = buf2;
        break;
      case 'i':
        if (IS_IMMORTAL(ch) && ch->incog_level > 0)
        sprintf(buf2, "`x(`bI%d`x) ", ch->incog_level);
        i = buf2;
        break;
      case 'o':
        if (ch->desc->editor)
        sprintf(buf2, "`x[`y%s`x]", olc_ed_name(ch));
        i = buf2;
        break;
      case 'O':
        sprintf(buf2, "%s", olc_ed_vnum(ch));
        i = buf2;
        break;
      case 'N': {
          float percent = ((float)ch->hit / max_hp(ch)) * 10.0;
          bool change = FALSE;

          sprintf(buf2, "`G");
          for (int x = 0; x < 10; x++) {
            if ((float)x > percent && !change) {
              strcat(buf2, "`R");
              change = TRUE;
            }
            strcat(buf2, ">");
          }
          strcat(buf2, "`x");
          i = buf2;
          break;
        }
      case 'r':
        if (IS_IMMORTAL(ch) && ch->in_room != NULL)
        sprintf(buf2, "%s", ((!IS_NPC(ch) && IS_FLAG(ch->act, PLR_HOLYLIGHT)) || (!IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_FLASH) && !room_is_dark(ch->in_room)))
        ? ch->in_room->name
        : "darkness");
        else
        sprintf(buf2, " ");
        i = buf2;
        break;
      case 'R':
        if (IS_IMMORTAL(ch) && ch->in_room != NULL)
        sprintf(buf2, "%d", ch->in_room->vnum);
        else
        sprintf(buf2, " ");
        i = buf2;
        break;
      }
      ++str;
      while ((*point = *i) != '\0')
      ++point, ++i;
    }
    *point = '\0';

    /* end */
    write_to_buffer(ch->desc, buf, 0);

    if (ch->prefix[0] != '\0')
    write_to_buffer(ch->desc, ch->prefix, 0);
    return;
  }

  int calc_status(int stat, int max_stat) {
    int percent;

    if (stat <= 0)
    return 0;

    if (max_stat <= 0)
    percent = 100;
    else if (stat < 0)
    percent = 0;
    else
    percent = (stat * 100 / max_stat);

    if (percent >= 50)
    return 3;
    else if (percent >= 25)
    return 2;
    else
    return 1;

    return 3;
  }

  char *colorout(char type[MSL], int mode, int lmode, CHAR_DATA *ch) {
    PC_DATA *col;
    char code[50];

    if (ch != NULL && IS_NPC(ch)) {
      return (0);
    }
    if (type == NULL) {
      return str_dup("");
    }

    if (mode == 0) { // basic color
      if (type[0] == 'q') {
        if (ch == NULL || ch->desc == NULL || ch->desc->mxp != TRUE)
        sprintf(code, CLEAR);
        else
        sprintf(code, "%s/color%s", MXP_BEG, MXP_END);
      }
      else if (type[0] == 'x') {
        sprintf(code, CLEAR);
      }
      else if (type[0] == 'X') {
        sprintf(code, CLEAR);
      }
      else if (type[0] == 'b') {
        sprintf(code, C_BLUE);
      }
      else if (type[0] == 'c') {
        sprintf(code, C_CYAN);
      }
      else if (type[0] == 'g') {
        sprintf(code, C_GREEN);
      }
      else if (type[0] == 'm') {
        sprintf(code, C_MAGENTA);
      }
      else if (type[0] == 'r') {
        sprintf(code, C_RED);
      }
      else if (type[0] == 'w') {
        sprintf(code, C_WHITE);
      }
      else if (type[0] == 'y') {
        sprintf(code, C_YELLOW);
      }
      else if (type[0] == 'd') {
        sprintf(code, C_D_GREY);
      }
      else if (type[0] == 'B') {
        sprintf(code, C_B_BLUE);
      }
      else if (type[0] == 'C') {
        sprintf(code, C_B_CYAN);
      }
      else if (type[0] == 'G') {
        sprintf(code, C_B_GREEN);
      }
      else if (type[0] == 'M') {
        sprintf(code, C_B_MAGENTA);
      }
      else if (type[0] == 'R') {
        sprintf(code, C_B_RED);
      }
      else if (type[0] == 'W') {
        sprintf(code, C_B_WHITE);
      }
      else if (type[0] == 'Y') {
        sprintf(code, C_B_YELLOW);
      }
      else if (type[0] == 'D') {
        sprintf(code, C_D_GREY);
      }
      else if (type[0] == '*') {
        sprintf(code, "%c", 007);
      }
      else if (type[0] == '/') {
        sprintf(code, "%c", 012);
      }
      // else if(type[0] == '`') {sprintf(code, "%c", '`');}
      // else if(type[0] == '`') {sprintf(code, "%s", "``");}
      else if (ch != NULL) { // customized color
        int code0 = -1, code1 = -1, code2 = 0;
        col = ch->pcdata;

        if (type[0] == 'o') {
          if (ch->pcdata->verbal_color != NULL && safe_strlen(ch->pcdata->verbal_color) > 0) {
            if (is_number(ch->pcdata->verbal_color) && atoi(ch->pcdata->verbal_color) >= 0 && atoi(ch->pcdata->verbal_color) <= 255) {
              int color = atoi(ch->pcdata->verbal_color);
              sprintf(code, "\x1B[0m\x1B[38;5;%dm", color);
            }
          }
          else {
            code0 = col->say[0];
            code1 = col->say[1];
            code2 = col->say[2];
          }
        }
        else if (type[0] == 'a') {
          code0 = col->gossip[0];
          code1 = col->gossip[1];
          code2 = col->gossip[2];
        }
        else if (type[0] == 'A') {
          code0 = col->gtell[0];
          code1 = col->gtell[1];
          code2 = col->gtell[2];
        }
        else if (type[0] == 'e') {
          code0 = col->newbie[0];
          code1 = col->newbie[1];
          code2 = col->newbie[2];
        }
        else if (type[0] == 'E') {
          code0 = col->pray[0];
          code1 = col->pray[1];
          code2 = col->pray[2];
        }
        else if (type[0] == 'f') {
          code0 = col->ooc[0];
          code1 = col->ooc[1];
          code2 = col->ooc[2];
        }
        else if (type[0] == 'h') {
          code0 = col->yells[0];
          code1 = col->yells[1];
          code2 = col->yells[2];
        }
        else if (type[0] == 'i') {
          code0 = col->tells[0];
          code1 = col->tells[1];
          code2 = col->tells[2];
        }
        else if (type[0] == 'j') {
          code0 = col->irl[0];
          code1 = col->irl[1];
          code2 = col->irl[2];
        }
        else if (type[0] == 'k') {
          code0 = col->immortal[0];
          code1 = col->immortal[1];
          code2 = col->immortal[2];
        }
        else if (type[0] == 'l') {
          code0 = col->implementor[0];
          code1 = col->implementor[1];
          code2 = col->implementor[2];
        }
        else if (type[0] == 'n') {
          code0 = col->auction[0];
          code1 = col->auction[1];
          code2 = col->auction[2];
        }

        //        else if(type[0] == 'o') {code0 = col->say[0];        code1 =
        //        col->say[1];        code2 = col->say[2];}

        else if (type[0] == 'p') {
          code0 = col->osay[0];
          code1 = col->osay[1];
          code2 = col->osay[2];
        }
        else if (type[0] == 'q') {
          code0 = col->room_exits[0];
          code1 = col->room_exits[1];
          code2 = col->room_exits[2];
        }
        else if (type[0] == 'Q') {
          code0 = col->room_things[0];
          code1 = col->room_things[1];
          code2 = col->room_things[2];
        }
        else if (type[0] == 's') {
          code0 = col->hero[0];
          code1 = col->hero[1];
          code2 = col->hero[2];
        }
        else if (type[0] == 't') {
          code0 = col->hint[0];
          code1 = col->hint[1];
          code2 = col->hint[2];
        }
        else if (type[0] == 'T') {
          code0 = col->minioncolor[0];
          code1 = col->minioncolor[1];
          code2 = col->minioncolor[2];
        }
        else if (type[0] == 'u') {
          code0 = col->fight_ohit[0];
          code1 = col->fight_ohit[1];
          code2 = col->fight_ohit[2];
        }
        else if (type[0] == 'U') {
          code0 = col->fight_thit[0];
          code1 = col->fight_thit[1];
          code2 = col->fight_thit[2];
        }
        else if (type[0] == 'v') {
          code0 = col->fight_skill[0];
          code1 = col->fight_skill[1];
          code2 = col->fight_skill[2];
        }
        else if (type[0] == 'V') {
          code0 = col->fight_death[0];
          code1 = col->fight_death[1];
          code2 = col->fight_death[2];
        }
        else if (type[0] == 'z') {
          code0 = col->fight_yhit[0];
          code1 = col->fight_yhit[1];
          code2 = col->fight_yhit[2];
        }
        else if (type[0] == 'Z') {
          code0 = col->wiznet[0];
          code1 = col->wiznet[1];
          code2 = col->wiznet[2];
        }
        else {
          return str_dup("BADCODE");
        }

        if (code0 != -1 && code1 != -1) {
          // if(code2)
          sprintf(code, "\033[%d;3%dm%s", code0, code1, code2 ? "\a" : "");
          // else
          //	sprintf(code,"\033[%d;3%dm", code0, code1);
        }
      }
      if(lmode == 2 && ch != NULL && ch->desc != NULL && ch->desc->mxp == TRUE)
      {
        char tmp[50];
        sprintf(tmp,  "%s/color%s%s", MXP_BEG, MXP_END, code);
        sprintf(code, "%s", tmp);
      }

    }
    else if (mode == 1) { // xterm color
      if (is_number(type) && atoi(type) >= 0 && atoi(type) <= 255) {
        int color = atoi(type);
        if(lmode == 2)
        sprintf(code, "%s/color%s\x1B[0m\x1B[38;5;%dm", MXP_BEG, MXP_END, color);
        else
        sprintf(code, "\x1B[0m\x1B[38;5;%dm", color);

        // log_string(code);
      }
      else {
        sprintf(code, CLEAR);
      }
    }
    else if (mode == 2) { // mxp
      sprintf(code, "%s/color%s%scolor fore=%s%s", MXP_BEG, MXP_END, MXP_BEG, type, MXP_END);
      // log_string(code);
    }
    else if (mode == 10 || mode == 11) {
      sprintf(code, "<%s", type);
    }
    else { // shouldn't occur
      sprintf(code, "%s", type);
    }

    int codeend = 0;
    codeend = safe_strlen(code);
    code[codeend] = '\0';

    return str_dup(code);
  }

  char *oldcolor(const char *txt, DESCRIPTOR_DATA *d) {
    const char *point;
    char *point2;
    char buf[MSL], buf2[MSL];
    char code[MSL];
    int skip = 0;
    int x = 0, i = 0;

    strcpy(buf2, "");
    strcpy(code, "");
    buf[0] = '\0';
    point2 = buf;
    if (d && txt) {
      for (point = txt; *point; point++) {
        if (*point == '`') {
          point++;

          int colormode = 0; // basic 0, xterm 1, mxp 2

          if (isalpha(*point) || *point == '*' || *point == '/' || *point == '`') {
            colormode = 0;
          }
          else if (*point == '#') {
            colormode = 0;
          }
          else if (isdigit(*point) && isdigit(*point) && isdigit(*point)) {
            colormode = 0;
          }

          if (colormode == 1 || colormode == 2) {
            int codelength = 3;
            if (colormode == 2) {
              codelength = 7;
            }

            for (i = 0; i < codelength; i++) {
              if (*point == ' ') {
                break;
              }
              if (*point == '\0') {
                break;
              }
              if (*point == '\\') {
                break;
              }
              if (*point == '<') {
                break;
              }
              if (*point == '>') {
                break;
              }
              if (*point == '&') {
                break;
              }
              // strncat(code, point, 1); //something broken here
              if (i < codelength - 1) {
                point++;
              }
            }
          }
          else {
            // strncat(code, point, 1); //also here, causes crash
          }

          if ((!d->character && d->ansi) || (d->character && IS_FLAG(d->character->act, PLR_COLOR))) {
            skip = color(*point, d->character ? d->character : NULL, point2);
            // skip = colorout(code, mode, d->character ? d->character : NULL, // point2); //new color output eventually
            while (skip-- > 0) {
              ++x;
              ++point2;
            }
          }
          continue;
        }
        *point2 = *point;
        *++point2 = '\0';

        if (++x >= MSL * 3 / 4) {
          x = 0;
          strcat(buf2, buf);
          buf[0] = '\0';
          point2 = buf;
        }
      }
      *point2 = '\0';

      strcat(buf2, buf);
    }
    else {
      bugf("[%s:%s] Bad descriptor or txt", __FILE__, __FUNCTION__);
    }

    // code = str_dup("");
    // free_string(code);

    return str_dup(buf2);
  }

  char *disco_lor_test(const char *txt, DESCRIPTOR_DATA *d) // aka Disco's tedius color parser -
  // Discordance 6/28/2019
  { // for when regular color just isn't cutting it any more
    char *colortag;
    char intxt[MSL * 2], outtxt[MSL * 2], code[MSL];
    int txtlen = 0, codelen = 1, colorlen = 0;
    int i, outp = 0, inp = 0;
    int colormode = 0;
    bool badcode = FALSE;

    CHAR_DATA *ch; // testing variables
    ch = CH(d);
    char test[MSL];
    strcpy(test, "");

    strcpy(outtxt, "");
    strcpy(intxt, "");

    if (d && txt) {
      strcpy(intxt, txt);
      printf_to_char(ch, "Intxt: %s\n\r`x", intxt); // testing
      txtlen = safe_strlen(intxt);
      printf_to_char(ch, "Intxt len: %d\n\r`x", txtlen); // testing
      for (inp = 0; inp < txtlen; inp++) {
        badcode = FALSE;

        if (intxt[inp] == '`') { // universal color codon
          inp++;

          colormode = 0; // basic 0, xterm 1, mxp 2

          if (isalpha(intxt[inp])) {
            colormode = 0;
          }
          else if (intxt[inp] == '#') {
            colormode = 2;
          }
          else if (isdigit(intxt[inp]) && isdigit(intxt[inp + 1]) && isdigit(intxt[inp + 2])) {
            colormode = 1;
          }
          else {
            colormode = 0;
          }

          printf_to_char(ch, "Colormode: %d\n\r`x", colormode); // testing
        }
        else { // if no color code, copy to out
          outtxt[outp] = intxt[inp];
          outp++; // advancing output place holder

          strcpy(test, outtxt);
          test[outp] = '\0';
          printf_to_char(ch, "NC %d Test: %s\n\r`x", outp, test); // testing

          continue;
        }

        strcpy(code, "");
        // grabbing color code
        if (colormode == 1 || colormode == 2) { // this copies the MXP or Xterm code
          if (colormode == 1) {
            codelen = 3;
          }
          else {
            codelen = 7;
          }

          for (i = 0; i < codelen; i++) {
            if (intxt[inp] == ' ') {
              badcode = TRUE;
            }
            if (intxt[inp] == '\0') {
              badcode = TRUE;
            }
            if (badcode == TRUE) {
              codelen = i;
              break;
            }

            code[i] = intxt[inp];
            // if(i<codelen-1){inp++;}
            inp++;
          }
          code[codelen] = '\0';
        }
        else { // basic color
          code[0] = intxt[inp];
          code[1] = '\0';
          inp++;
        }

        printf_to_char(ch, "Code: %s\n\r`x", code); // testing

        // processing color code
        if (badcode == FALSE) { // don't process bad codes
          if (d != NULL)
          colortag =
          colorout(code, colormode, 0, d->character ? d->character : NULL);
          else
          colortag = colorout(code, colormode,0, NULL);
          colorlen = safe_strlen(colortag);
          printf_to_char(ch, "Colortag len: %d\n\r`x", safe_strlen(colortag)); // testing

          if (!strcmp(colortag, "BADCODE")) { // perserving badcode as output
            for (i = 0; i < codelen; i++) {
              outtxt[outp] = code[i];
              outp++;
            }
            printf_to_char(ch, "BADCODE: %s\n\r`x", code);     // testing
            printf_to_char(ch, "Code len: %d\n\r`x", codelen); // testing

            strcpy(test, outtxt);
            test[outp] = '\0';
            printf_to_char(ch, "CO %d Test: %s\n\r`x", outp, test); // testing
          }
          else {
            if ((!d->character && d->ansi) || (d->character && IS_FLAG(d->character->act, PLR_COLOR))) {
              for (i = 0; i < colorlen; i++) {
                outtxt[outp] = colortag[i]; // copy colortag to outtxt
                outp++;
              }
            }
          }

          if (inp < txtlen) { // only if intxt has more to give tho
            if (intxt[inp] == '`') {
              inp--; // back this up if we have double codes or a bad code
            }
            else {
              outtxt[outp] = intxt[inp];
              outp++;
            }
          }
        }

        strcpy(test, outtxt);
        test[outp] = '\0';
        printf_to_char(ch, "CO %d Test: %s\n\r`x", outp, test); // testing

        free_string(colortag);
      }
    }
    else {
      bugf("[%s:%s] Bad descriptor or txt", __FILE__, __FUNCTION__);
    }

    outtxt[outp] = '\0';
    printf_to_char(ch, "F Outp: %d\n\r`x", outp);               // testing
    printf_to_char(ch, "Outtxt len: %d\n\r`x", safe_strlen(outtxt)); // testing
    printf_to_char(ch, "F Outtxt: %s\n\r`x", outtxt);           // testing

    return str_dup(outtxt);
  }

  char *revert_mxp_clear(char *instr) {
    char output[MSL];
    char intxt[MSL];
    strcpy(intxt, instr);
    strcpy(output, "");
    char blah[MSL];
    for (int inp = 0; inp < (int)safe_strlen(intxt); inp++) {
      if (intxt[inp] == '&') {
        if (inp + 3 < (int)safe_strlen(intxt) && intxt[inp + 1] == 'l' && intxt[inp + 2] == 't' && intxt[inp + 3] == ';') {
          sprintf(blah, "%c", '<');
          strcat(output, blah);

          inp += 3;
        }
        else if (inp + 3 < (int)safe_strlen(intxt) && intxt[inp + 1] == 'g' && intxt[inp + 2] == 't' && intxt[inp + 3] == ';') {
          sprintf(blah, "%c", '>');
          strcat(output, blah);
          inp += 3;
        }
        else if (inp + 4 < (int)safe_strlen(intxt) && intxt[inp + 1] == 'a' && intxt[inp + 2] == 'm' && intxt[inp + 3] == 'p' && intxt[inp + 4] == ';') {
          sprintf(blah, "%c", '&');
          strcat(output, blah);
          inp += 4;
        }
        else {
          sprintf(blah, "%c", intxt[inp]);
          strcat(output, blah);
        }

      }
      else {
        sprintf(blah, "%c", intxt[inp]);
        strcat(output, blah);
      }
    }
    return str_dup(output);
  }
  char *clear_mxp(char *instr, DESCRIPTOR_DATA *d) {
    char output[MSL];
    char intxt[MSL];
    strcpy(intxt, instr);
    strcpy(output, "");
    char blah[MSL];
    if (d != NULL && d->mxp == TRUE) {
      for (int inp = 0; inp < (int)safe_strlen(intxt); inp++) {
        if (intxt[inp] == '&') {
          sprintf(blah, "%s", "&amp;");
          strcat(output, blah);
        }
        else if (intxt[inp] == '<') {
          sprintf(blah, "%s", "&lt;");
          strcat(output, blah);
        }
        else if (intxt[inp] == '>') {
          sprintf(blah, "%s", "&gt;");
          strcat(output, blah);
        }
        else {
          sprintf(blah, "%c", intxt[inp]);
          strcat(output, blah);
        }
      }
      return str_dup(output);
    }
    else {
      return instr;
    }
  }

  bool been_processed(char *argument) {
    // if(strcasestr(argument, "<color fore=#") != NULL)
    // return TRUE;
    if (strcasestr(argument, "&lt;") != NULL)
    return TRUE;
    if (strcasestr(argument, "&gt;") != NULL)
    return TRUE;
    if (strcasestr(argument, "&amp;") != NULL)
    return TRUE;
    if (strcasestr(argument, "`") != NULL)
    return FALSE;
    if (strcasestr(argument, "[[3") != NULL)
    return TRUE;
    return FALSE;
  }

  char * tyr_lor(Buffer &buffer, const char *txt, DESCRIPTOR_DATA *d, int ctype) { // for when regular color just isn't cutting it any more
#define MBSL MSL*2
    char *       colortag = "";
    char         intxt[MBSL], outtxt[MBSL], code[MBSL];
    int          txtlen = 0, codelen = 1;
    int          i;
    int          colormode = 0;
    int 	 lastmode = 0;
    bool         badcode = FALSE;
    int         noclcount = 0;
    char        tmp[MBSL];
    char        tmpout[MBSL];
    size_t 	inp = 0, outp = 0, colorlen = 0, maxl = MBSL - 1;

    char editedTxt[MBSL];
    strncpy(editedTxt, txt, MBSL-10);
    if(strlen(editedTxt) > MBSL -20)
    {
      editedTxt[MBSL-3] = '`';
      editedTxt[MBSL-2] = 'x';
      editedTxt[MBSL-1] = '\0';  // Null-terminate the editedTxt string
    }
    else
    strcat(editedTxt, "`x\0");

    txt = editedTxt;

    if (ctype != COLOR_COLOR && txt == NULL) {
      if (ctype == COLOR_LENGTH)
      return str_dup("0");
      else
      return str_dup("");
    }
    if (ctype == COLOR_COLOR) {
      if (been_processed(str_dup(txt))) {
        if (ctype == COLOR_COLOR)
        buffer.strcat(txt);
        return str_dup("");
      }
    }
    strcpy(outtxt, "");
    strcpy(intxt, "");
    strcpy(tmpout, "");
    if ((d || ctype == COLOR_LENGTH || ctype == COLOR_NOCOLOR) && txt) {
      strncpy(intxt, txt, maxl);
      txtlen = safe_strlen(intxt);
      for (inp = 0; inp < (size_t)txtlen; inp++) {
        badcode = FALSE;

        if (intxt[inp] == '`') { // universal color codon
          if (safe_strlen(tmpout) > 0) {
            sprintf(tmp, "%s", clear_mxp(tmpout, d));
            strncat(outtxt, tmp, maxl);
            strcpy(tmpout, "");
          }
          inp++;

          colormode = 0; // basic 0, xterm 1, mxp 2

          if (isalpha(intxt[inp])) {
            colormode = 0;
          }
          else if (intxt[inp] == '#') {
            colormode = 2;
          }
          else if (isdigit(intxt[inp]) && isdigit(intxt[inp + 1]) && isdigit(intxt[inp + 2])) {
            colormode = 1;
          }
          else {
            colormode = 0;
          }
        }

        else if (intxt[inp] == '<') {
          if (safe_strlen(tmpout) > 0) {
            sprintf(tmp, "%s", clear_mxp(tmpout, d));
            strncat(outtxt, tmp, maxl);
            strcpy(tmpout, "");
          }
          if (intxt[inp + 1] != '\0' && intxt[inp + 2] != '\0' && intxt[inp + 2] == '>') {
            if (intxt[inp + 1] == 'b' || intxt[inp + 1] == 'B' || intxt[inp + 1] == 'i' || intxt[inp + 1] == 'I' || intxt[inp + 1] == 'u' || intxt[inp + 1] == 'U') {
              colormode = 10;
            }
          }
          if (intxt[inp + 1] != '\0' && intxt[inp + 2] != '\0' && intxt[inp + 3] != '\0' && intxt[inp + 3] == '>' && intxt[inp + 1] == '/') {
            if (intxt[inp + 2] == 'b' || intxt[inp + 2] == 'B' || intxt[inp + 2] == 'i' || intxt[inp + 2] == 'I' || intxt[inp + 2] == 'u' || intxt[inp + 2] == 'U') {
              colormode = 11;
            }
          }

          if (colormode != 10 && colormode != 11) {
            sprintf(tmp, "%c", intxt[inp]);
            strncat(tmpout, tmp, maxl);
            outp++; // advancing output place holder
            noclcount++;
            continue;
          }
          inp++;
        }

        else { // if no color code, copy to out
          sprintf(tmp, "%c", intxt[inp]);
          strncat(tmpout, tmp, maxl);
          outp++; // advancing output place holder
          noclcount++;
          continue;
        }

        strcpy(code, "");
        // grabbing color code
        if (colormode == 1 || colormode == 2) { // this copies the MXP or Xterm code
          if (colormode == 1) {
            codelen = 3;
          }
          else {
            codelen = 7;
          }

          for (i = 0; i < codelen; i++) {
            if (intxt[inp] == ' ') {
              badcode = TRUE;
            }
            if (intxt[inp] == '\0') {
              badcode = TRUE;
            }
            if (badcode == TRUE) {
              codelen = i;
              break;
            }

            code[i] = intxt[inp];
            inp++;
          }
          code[codelen] = '\0';
        }

        else if (colormode == 10 || colormode == 11) {
          if (colormode == 10)
          codelen = 2;
          else
          codelen = 3;
          for (i = 0; i < codelen; i++) {
            if (intxt[inp] == ' ') {
              badcode = TRUE;
            }
            if (intxt[inp] == '\0') {
              badcode = TRUE;
            }
            if (badcode == TRUE) {
              codelen = i;
              break;
            }

            code[i] = intxt[inp];
            inp++;
          }
          code[codelen] = '\0';
        }

        else { // basic color
          code[0] = intxt[inp];
          code[1] = '\0';
          inp++;
        }

        // processing color code
        if (badcode == FALSE) { // don't process bad codes
          free_string(colortag);
          if (d != NULL)
          colortag =
          colorout(code, colormode, lastmode, d->character ? d->character : NULL);
          else
          colortag = colorout(code, colormode, lastmode, NULL);

          lastmode = colormode;
          colorlen = safe_strlen(colortag);

          if (!strcmp(colortag, "BADCODE")) { // perserving badcode as output
            for (i = 0; i < codelen; i++) {
              sprintf(tmp, "%c", code[i]);
              strncat(tmpout, tmp, maxl);
              //                        outtxt[outp] = code[i];
              outp++;
              noclcount++;
            }
          }
          else if (ctype == COLOR_COLOR) {
            if (colormode == 0 || colormode == 1 || d->mxp == TRUE) {
              if ((!d->character && d->ansi) || (d->character && IS_FLAG(d->character->act, PLR_COLOR))) {
                for (i = 0; (size_t)i < colorlen; i++) {
                  sprintf(tmp, "%c", colortag[i]);
                  strncat(outtxt, tmp, maxl);
                  //                            outtxt[outp] = colortag[i]; //copy
                  //                            colortag to outtxt
                  outp++;
                }
              }
            }
          }

          if (inp < (size_t)txtlen) { // only if intxt has more to give tho
            if (intxt[inp] == '`') {
              inp--; // back this up if we have double codes or a bad code
            }
            else {
              sprintf(tmp, "%c", intxt[inp]);
              strncat(tmpout, tmp, maxl);
              //                      outtxt[outp] = intxt[inp];
              outp++;
              noclcount++;
            }
          }
        }
      }
    }
    else {
      bugf("[%s:%s] Bad descriptor or txt", __FILE__, __FUNCTION__);
    }
    /*
    if (safe_strlen(tmpout) > 0 && ctype == COLOR_COLOR) {
    if(lastmode == 2)
    sprintf(tmp, "%s/color%s", MXP_BEG, MXP_END);
    else
    printf(tmp, C_WHITE);
    outp++;
    strncat(outtxt, tmp, maxl);
    strcpy(tmpout, "");
    }

    if (safe_strlen(tmpout) > 0) {
    sprintf(tmp, "%s", clear_mxp(tmpout, d));
    strncat(outtxt, tmp, maxl);
    strcpy(tmpout, "");
    }

    */


        /*
    char tmpl[MSL];
    sprintf(tmpl, "%zu %zu", strlen(outtxt), maxl);
    log_string(tmpl);
    log_string(outtxt);
    */
        /*
    if (ctype == COLOR_COLOR) {

    if (d->mxp == TRUE && d->connected == CON_PLAYING)
    sprintf(tmp, "%s/color%s", MXP_BEG, MXP_END);
    else
    sprintf(tmp, C_WHITE);
    strncat(outtxt, tmp, maxl);
    }
    */

    strcat(outtxt, "\0");
    //log_string(str_dup(outtxt));
    if (ctype == COLOR_COLOR || ctype == COLOR_NOCOLOR) {
      if (ctype == COLOR_COLOR)
      buffer.strcat(outtxt);
      return str_dup(outtxt);
    }
    else {
      char buf[MBSL];
      sprintf(buf, "%d", noclcount);
      //	buffer.strcat(buf);
      return str_dup(buf);
    }
  }

  _DOFUN(do_colortest) {

    Buffer cstring;
    DESCRIPTOR_DATA *d;
    d = ch->desc;
    int slength = atoi(tyr_lor(cstring, argument, d, COLOR_LENGTH));
    char buf[MSL];
    remove_color(buf, argument);
    log_string("Arcolor");
    log_string(buf);
    printf_to_char(ch, "%s::%d::%s", argument, slength, buf);

    return;
  }

  /*
  * Append onto an output buffer.
  */
  // Editted this for MXP - Discordance
  void write_to_buffer(DESCRIPTOR_DATA *d, const char *txt, int length) {
    //	int origlength; //MXP - Discordance
    Buffer buffer;
    if (!d)
    return;
    if (d == NULL)
    return;

    if ((d->connected == CON_PLAYING && d->character == NULL))
    log_string("CON_PLAYING, no character");

    if(d->connected < 0 || d->connected > 50 || d->descriptor < 0)
    log_string("Write to buffer, strange state.");

    if ((d->connected == CON_PLAYING && d->character == NULL) || d->connected < 0 || d->connected > 50 || d->descriptor < 0) {
      log_string("Corrupt d in write_to_buffer");
      //    descriptor_list.remove(d);
      //    close_socket(d->descriptor);
      //    free_descriptor(d);
      //    --num_descriptors;
      return;
    }

    if (d->outbuf == NULL) {
      log_string("Outbuf is NULL in write to buffer");
      return;
    }
    tyr_lor(buffer, txt, d, COLOR_COLOR);

    // Convert the color.
    // colorconv_desc( buffer, txt, d );
    //    newtxt = disco_lor(txt, d);

    // Find length due to the new color code.
    length = safe_strlen_color(buffer);
    //  length = safe_strlen_color(newtxt);

    // MXP - Discordance
    // origlength = length;
    /* work out how much we need to expand/contract it */
    // length += count_mxp_tags (d->mxp, buffer, length);
    // length += count_mxp_tags (d->mxp, newtxt, length);

    /*
    * Initial \n\r if needed.
    */
    if (d->outtop == 0 && !d->fcommand) {
      d->outbuf[0] = '\n';
      d->outbuf[1] = '\r';
      d->outtop = 2;
    }

    /*
    * Expand the buffer as needed.
    */
    while (d->outtop + length >= d->outsize) {
      char *outbuf;
      /*
      if (d->outsize >= 32000) {
      bug("Buffer overflow. Closing.\n\r", 0);
      close_desc(d);
      return;
      }
      */
      outbuf = (char *)alloc_mem(2 * d->outsize);
      strncpy(outbuf, d->outbuf, d->outtop);
      free_mem(d->outbuf, d->outsize);
      d->outbuf = outbuf;
      d->outsize *= 2;
      // log_string("MEMCHECK: Write to buffer.");
    }

    /*
    * Copy.
    */

    strncpy(d->outbuf + d->outtop, buffer, length); // This looks like it's taking buffer and outputting it

    // MXP - Discordance
    // convert_mxp_tags (d->mxp, d->outbuf + d->outtop, buffer, origlength);
    // convert_mxp_tags (d->mxp, d->outbuf + d->outtop, newtxt, origlength);
    // //This is going to output instead
    d->outtop += length;
    // free_string(newtxt);
    return;
  }

  /*
  * Lowest level output function.
  * Write a block of text to the file descriptor.
  * If this gives errors on very long blocks (like 'ofind all'), *   try lowering the max block size.
  */
  bool write_to_descriptor(int desc, char *txt, int length) {
    int iStart;
    int nWrite;
    int nBlock;
    if (length <= 0)
    length = safe_strlen(txt);
    for (iStart = 0; iStart < length; iStart += nWrite) {
      nBlock = UMIN(length - iStart, 4096);
#if defined(_WIN32)
      if ((nWrite = send(desc, txt + iStart, nBlock, 0)) < 0)
#else
      if ((nWrite = write(desc, txt + iStart, nBlock)) < 0)
#endif
      {
        perror("Write_to_descriptor");
        return FALSE;
      }
    }
    return TRUE;
  }

  void stop_idling(CHAR_DATA *ch) {
    if (ch == NULL || ch->desc == NULL || ch->desc->connected != CON_PLAYING || ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
    return;

    ch->timer = 0;
    ch->idle = current_time;
    //    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
  }

  void send_to_char(const char *txt, CHAR_DATA *ch) {
    if (ch->desc == NULL || ch->desc->valid == FALSE || ch->desc->host == NULL)
    return;

    write_to_buffer(ch->desc, txt, 0);
  }

  void send_to_eavesdrop(const char *txt, ROOM_INDEX_DATA *room) {
    AFFECT_DATA *paf, *paf_next = NULL;

    for (paf = room->affected; paf; paf = paf_next) {
      paf_next = paf->next;

      if (paf->bitvector == AFF_XEAVESDROP && paf->caster != NULL && paf->caster->desc->connected == CON_PLAYING) {
      }
    }
    return;
  }

  /*
  char *color_lookup(char type, long *len)
  {
  static char code[5];

  switch( type )
  {
  default:
  *len = safe_strlen(CLEAR);
  return CLEAR;
  case 'x':
  *len = safe_strlen(CLEAR);
  return CLEAR;
  case 'b':
  case '3':
  *len = safe_strlen(C_BLUE);
  return C_BLUE;
  case 'c':
  case '5':
  *len = safe_strlen(C_CYAN);
  return C_CYAN;
  case 'g':
  case '1':
  *len = safe_strlen(C_GREEN);
  return C_GREEN;
  case 'm':
  case '4':
  *len = safe_strlen(C_MAGENTA);
  return C_MAGENTA;
  case 'r':
  case '0':
  *len = safe_strlen(C_RED);
  return C_RED;
  case 'w':
  case '6':
  *len = safe_strlen(C_WHITE);
  return C_WHITE;
  case 'y':
  case '2':
  *len = safe_strlen(C_YELLOW);
  return C_YELLOW;
  case 'B':
  *len = safe_strlen(C_B_BLUE);
  return C_B_BLUE;
  case 'C':
  *len = safe_strlen(C_B_CYAN);
  return C_B_CYAN;
  case 'G':
  case '9':
  *len = safe_strlen(C_B_GREEN);
  return C_B_GREEN;
  case 'M':
  case '#':
  *len = safe_strlen(C_B_MAGENTA);
  return C_B_MAGENTA;
  case 'R':
  case '8':
  *len = safe_strlen(C_B_RED);
  return C_B_RED;
  case 'W':
  *len = safe_strlen(C_B_WHITE);
  return C_B_WHITE;
  case 'Y':
  case '!':
  *len = safe_strlen(C_B_YELLOW);
  return C_B_YELLOW;
  case 'D':
  case '^':
  *len = safe_strlen(C_D_GREY);
  return C_D_GREY;
  case '*':
  sprintf( code, "%c", 007 );
  break;
  case '/':
  sprintf( code, "%c", 012 );
  break;
  case '`':
  sprintf( code, "%c", '`' );
  break;
  }

  *len = safe_strlen(code);
  return code;
  }
  */

    /*
  void replace_color_code( string &target, bool col )
  {
  struct timeval now_time;
  gettimeofday( &now_time, NULL );
  time_t start        = (time_t) now_time.tv_usec;

  long len;
  int x = 0;

  for (size_t loc = target.find( '`' );
  loc != std::string::npos;
  loc = target.find( '`', loc + len ))
  {
  target.replace( loc, 2, col ? color_lookup(target[loc+1], &len) : "");
  x++;
  }

  gettimeofday( &now_time, NULL );
  time_t end        = (time_t) now_time.tv_usec;
  logfi("NEW Replace Time: %ld %ld %ld", x, target.length(), end - start);
  }
  */
    /*

  bool has_color(CHAR_DATA *ch)
  {
  if(ch == NULL || IS_NPC(ch))
  return FALSE;
  if(ch->desc != NULL)
  {
  if(ch->desc->ansi = FALSE)
  return FALSE;
  }
  if(!IS_FLAG(ch->act, PLR_COLOR))
  return FALSE;

  return TRUE;
  }

  void page_to_char( const char *txt, CHAR_DATA *ch )
  {
  string testc = txt;
  struct timeval now_time;
  gettimeofday( &now_time, NULL );
  time_t start        = (time_t) now_time.tv_usec;

  if( txt && ch->desc )
  {
  replace_color_code(testc, IS_FLAG(ch->act, PLR_COLOR));
  ch->desc->showstr_head  = (char *)malloc( testc.length() + 1 );
  strcpy( ch->desc->showstr_head, testc.c_str() );
  ch->desc->showstr_point = ch->desc->showstr_head;
  show_string( ch->desc, "" );
  }

  gettimeofday( &now_time, NULL );
  time_t end        = (time_t) now_time.tv_usec;
  logfi("NEW Total Time: %ld", end - start);

  page_to_char_old(txt, ch);
  }
  */

  // Page to one char, color version, by Lope.
  // modified by Scaelorn
  void page_to_char(const char *txt, CHAR_DATA *ch) {
    char editedTxt[MSL];
    strncpy(editedTxt, txt, MSL-1);
    editedTxt[MSL-1] = '\0';  // Null-terminate the editedTxt string
    txt = editedTxt;

    if (txt && ch->desc) {
      //    char tstring[MSL];
      // sprintf(tstring, "The way I see it, if you want the rainbow, you gotta
      // put up with the rain.");
      //    log_string("PAGE_TO_CHAR");
      //    sprintf(tstring, "IL: %ld, ML:%d", safe_strlen(txt), MAX_STRING_LENGTH);
      //    log_string(tstring);
      if (safe_strlen(txt) >= MAX_STRING_LENGTH) {
        log_string("TOO LONG");
        static char xbuf2[MAX_STRING_LENGTH];
        strncpy(xbuf2, txt, MAX_STRING_LENGTH - 10);
        xbuf2[safe_strlen(xbuf2)] = '\0';
        page_to_char(xbuf2, ch);
        return;
      }
      //    log_string("Here2");
      ch->desc->showstr_head = (char *)alloc_mem(safe_strlen(txt) + 1);
      strcpy(ch->desc->showstr_head, txt);
      ch->desc->showstr_point = ch->desc->showstr_head;
      show_string(ch->desc, "");
    }
    //  log_string("Here3");
    return;
  }

  void send_timer(struct timerset *vtime, CHAR_DATA *ch) {
    struct timeval ntime;
    int carry;

    if (vtime->num_uses == 0)
    return;
    ntime.tv_sec = vtime->total_time.tv_sec / vtime->num_uses;
    carry = (vtime->total_time.tv_sec % vtime->num_uses) * 1000000;
    ntime.tv_usec = (vtime->total_time.tv_usec + carry) / vtime->num_uses;
    printf_to_char(ch, "Has been used %d times this boot.\n\r", vtime->num_uses);
    printf_to_char(ch, "Time (in secs): min %d.%0.6d; avg: %d.%0.6d; max %d.%0.6d\n\r", vtime->min_time.tv_sec, vtime->min_time.tv_usec, ntime.tv_sec, ntime.tv_usec, vtime->max_time.tv_sec, vtime->max_time.tv_usec);
    return;
  }

  /* string pager */
  void show_string(struct descriptor_data *d, char *input) {
    char buffer[4 * MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;
    //    char tmp[MSL];

    one_argument(input, buf);
    if (buf[0] != '\0') {
      if (d->showstr_head) {
        // free(d->showstr_head);
        free_mem(d->showstr_head, safe_strlen(d->showstr_head));
        d->showstr_head = 0;
        //	    log_string("MEMCHECK: Show string.");
      }
      d->showstr_point = 0;
      return;
    }

    if (d->character)
    show_lines = d->character->lines;
    else
    show_lines = 0;

    for (scan = buffer;; scan++, d->showstr_point++) {
      if (((*scan = *d->showstr_point) == '\n' || *scan == '\r') && (toggle = -toggle) < 0) {
        lines++;
      }

      else if (!*scan || (show_lines > 0 && lines >= show_lines)) {
        *scan = '\0';
        write_to_buffer(d, buffer, safe_strlen(buffer));
        for (chk = d->showstr_point; isspace(*chk); chk++)
        ;
        if (!*chk) {
          if (d->showstr_head) {
            // sprintf(buf, "MEMCHECK %d %s.", safe_strlen(d->showstr_head), // d->showstr_head); log_string(buf); free(d->showstr_head);
            free_mem(d->showstr_head, safe_strlen(d->showstr_head));
            d->showstr_head = 0;
            // log_string("MEMCHECK: Show string2.");
          }
          d->showstr_point = 0;
        }
        return;
      }
    }
    return;
  }

  void act_new(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int min_pos) {
    static char *const he_she[] = {"it", "he", "she"};
    static char *const him_her[] = {"it", "him", "her"};
    static char *const his_her[] = {"its", "his", "her"};

    CharList *to_list;
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *)arg2;
    OBJ_DATA *obj1 = (OBJ_DATA *)arg1;
    OBJ_DATA *obj2 = (OBJ_DATA *)arg2;
    const char *str;
    const char *i = NULL;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    //    bool		fColor = FALSE;
    char colbuf[MSL];
    char hand[MSL];
    char opphand[MSL];

    int charcount = 0;
    /*
* Discard null and zero-length messages.
*/
    if (!format || format[0] == '\0')
    return;

    /* discard null rooms and chars */
    if (ch == NULL || !ch || ch->in_room == NULL)
    return;

    if (number_percent() % 2 == 0) {
      sprintf(hand, "right");
      sprintf(opphand, "left");
    }
    else {
      sprintf(hand, "left");
      sprintf(opphand, "right");
    }

    to_list = ch->in_room->people;
    if (type == TO_VICT) {
      if (!vch) {
        bug("Act: null vch with TO_VICT.", 0);
        return;
      }

      if (vch->in_room == NULL)
      return;

      to_list = vch->in_room->people;
    }

    for (CharList::iterator it = to_list->begin(); it != to_list->end(); ++it) {
      to = *it;

      if ((!IS_NPC(to) && to->desc == NULL) || (IS_NPC(to) && to->pcdata == NULL && (!HAS_TRIGGER_MOB(to, TRIG_ACT) || IS_NPC(ch))))
      continue;

      if (is_asleep(to))
      continue;

      if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
      continue;
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(to->act, PLR_DEEPSHROUD))
      continue;

      if(!IS_IMMORTAL(to) && !IS_IMMORTAL(ch) && to->in_room != NULL && ch != to && newbie_school(to->in_room))
      continue;


      if (type == TO_CHAR && to != ch)
      continue;
      if (type == TO_VICT && (to != vch || to == ch))
      continue;

      if (type == TO_ROOM && (to == ch))
      continue;
      if (type == TO_NOTVICT && (to == ch || to == vch))
      continue;

      if (type == TO_IMP && !IS_IMP(to))
      continue;

      if (ch != NULL && ch->in_room != NULL && crowded_room(ch->in_room) && !same_place(ch, to))
      continue;

      point = buf;
      str = format;
      while (*str != '\0') {
        charcount = 0;
        if (*str != '$') {
          *point++ = *str++;
          charcount++;
          continue;
        }

        // fColor = TRUE;
        ++str;
        i = " <@@@> ";
        if (!arg2 && *str >= 'A' && *str <= 'Z') {
          sprintf(buf2, "Act: missing arg2 for code %d in string %s", *str, format);
          bug(buf2, 0);
          // bug( "Act: missing arg2 for code %d.", *str );
          i = " <@@@> ";
        }
        else {
          switch (*str) {
          default:
            bug("Act: bad code %d.", *str);
            i = " <@@@> ";
            break;
            /* Thx alex for 't' idea */
          case 't':
            i = (char *)arg1;
            break;
          case 'T':
            remove_color(colbuf, (char *)arg2);
            i = colbuf;
            break;
          case 'n':
            remove_color(colbuf, PERS(ch, to));
            i = colbuf;
            break;
          case 'N':
            //			if(charcount > 2)
            remove_color(colbuf, PERS_3(vch, to));
            //			else
            //                          remove_color(colbuf, PERS( vch, to ));
            i = colbuf;
            break;
          case 'e':
            i = he_she[URANGE(0, ch->sex, 2)];
            break;
          case 'E':
            i = he_she[URANGE(0, vch->sex, 2)];
            break;
          case 'm':
            i = him_her[URANGE(0, ch->sex, 2)];
            break;
          case 'M':
            i = him_her[URANGE(0, vch->sex, 2)];
            break;
          case 's':
            i = his_her[URANGE(0, ch->sex, 2)];
            break;
          case 'S':
            i = his_her[URANGE(0, vch->sex, 2)];
            break;

          case 'h':
            i = hand;
            break;
          case 'H':
            i = opphand;
            break;
          case 'a':
            if (obj1 != NULL && (!str_prefix("a ", obj1->short_descr) || !str_prefix("an ", obj1->short_descr)))
            i = "";
            else if (obj1 != NULL && is_pair(obj1) && obj1->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj1 != NULL && can_see_obj(to, obj1))
              i = a_or_an(obj1->short_descr);
              else
              i = "a";
            }
            break;

          case 'A':
            if (obj2 != NULL && (!str_prefix("a ", obj2->short_descr) || !str_prefix("an ", obj2->short_descr)))
            i = "";
            else if (obj2 != NULL && is_pair(obj2) && obj2->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj2 != NULL && can_see_obj(to, obj2))
              i = a_or_an(obj2->short_descr);
              else
              i = "a";
            }

            break;

          case 'p':
            if (obj1 == NULL || !can_see_obj(to, obj1))
            i = "something";
            else {
              if (obj1->carried_by != NULL && obj1->item_type == ITEM_WEAPON && !IS_NPC(obj1->carried_by)) {
                i = obj1->short_descr;
              }
              else
              i = obj1->short_descr;
            }
            break;

          case 'P':
            if (obj2 == NULL || !can_see_obj(to, obj2))
            i = "something";
            else {
              if (obj2->carried_by != NULL && obj2->item_type == ITEM_WEAPON && !IS_NPC(obj2->carried_by)) {
                i = obj2->short_descr;
              }
              else
              i = obj2->short_descr;
            }

            break;
          case 'o':
            if (obj1 == NULL || !can_see_obj(to, obj1))
            i = "something";
            else {
              if (obj1->carried_by != NULL && obj1->item_type == ITEM_WEAPON && !IS_NPC(obj1->carried_by)) {
                i = obj1->description;
              }
              else
              i = obj1->description;
            }
            break;
          case 'O':
            if (obj2 == NULL || !can_see_obj(to, obj2))
            i = "something";
            else {
              if (obj2->carried_by != NULL && obj2->item_type == ITEM_WEAPON && !IS_NPC(obj2->carried_by)) {
                i = obj2->description;
              }
              else
              i = obj2->description;
            }
            break;

          case 'd':
            if (arg2 == NULL || ((char *)arg2)[0] == '\0') {
              i = "door";
            }
            else {
              one_argument((char *)arg2, fname);
              i = fname;
            }
            break;
          }
        }

        ++str;
        while ((*point = *i) != '\0')
        ++point, ++i;
      }

      *point++ = '\n';
      *point++ = '\r';
      *point = '\0';
      buf[0] = UPPER(buf[0]);
      if (to->desc != NULL && to->desc->valid == TRUE) {
        if (to->in_room != NULL && ch != NULL && ch->in_room != NULL && to->in_room != ch->in_room && valid_room_distance(to, ch->in_room)) {
          char distbuf[MSL];
          sprintf(distbuf, "[%s]", room_distance(to, ch->in_room));
          write_to_buffer(to->desc, distbuf, 0);
        }
        write_to_buffer(to->desc, buf, 0);
      }
    }

    if (type == TO_ROOM || type == TO_NOTVICT) {
      OBJ_DATA *obj, *obj_next;

      point = buf;
      str = format;
      while (*str != '\0') {
        *point++ = *str++;
      }
      *point = '\0';

      int y = 0;
      for (obj = ch->in_room->contents; obj && y < 100; obj = obj_next) {
        obj_next = obj->next_content;
        y++;
      }

      CharList::iterator it =
      find(ch->in_room->people->begin(), ch->in_room->people->end(), ch);

      while (it != ch->in_room->people->end()) {
        CHAR_DATA *tch = *it;
        ++it;

        y = 0;
        for (obj = tch->carrying; obj && y < 100; obj = obj_next) {
          obj_next = obj->next_content;
          y++;
        }
      }
    }

    return;
  }

  void visualact(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type) {
    static char *const he_she[] = {"it", "he", "she"};
    static char *const him_her[] = {"it", "him", "her"};
    static char *const his_her[] = {"its", "his", "her"};

    CharList *to_list;
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *)arg2;
    OBJ_DATA *obj1 = (OBJ_DATA *)arg1;
    OBJ_DATA *obj2 = (OBJ_DATA *)arg2;
    const char *str;
    const char *i = NULL;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    //    bool                fColor = FALSE;
    char colbuf[MSL];
    char hand[MSL];
    char opphand[MSL];

    int charcount = 0;
    /*
    * Discard null and zero-length messages.
    */
    if (!format || format[0] == '\0')
    return;

    /* discard null rooms and chars */
    if (ch == NULL || !ch || ch->in_room == NULL)
    return;

    if (number_percent() % 2 == 0) {
      sprintf(hand, "right");
      sprintf(opphand, "left");
    }
    else {
      sprintf(hand, "left");
      sprintf(opphand, "right");
    }

    to_list = ch->in_room->people;
    if (type == TO_VICT) {
      if (!vch) {
        bug("Act: null vch with TO_VICT.", 0);
        return;
      }

      if (vch->in_room == NULL)
      return;

      to_list = vch->in_room->people;
    }

    for (CharList::iterator it = to_list->begin(); it != to_list->end(); ++it) {
      to = *it;

      if ((!IS_NPC(to) && to->desc == NULL) || (IS_NPC(to) && to->pcdata == NULL && (!HAS_TRIGGER_MOB(to, TRIG_ACT) || IS_NPC(ch))))

      if (is_asleep(to))
      continue;

      if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
      continue;
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(to->act, PLR_DEEPSHROUD))
      continue;

      if (is_blind(to))
      continue;

      if(!IS_IMMORTAL(to) && !IS_IMMORTAL(ch) && to->in_room != NULL && ch != to && newbie_school(to->in_room))
      continue;


      if (to->in_room != NULL && is_dark(to->in_room) && !can_see_dark(to))
      continue;
      if (type == TO_CHAR && to != ch)
      continue;
      if (type == TO_VICT && (to != vch || to == ch))
      continue;

      if (type == TO_ROOM && (to == ch))
      continue;
      if (type == TO_NOTVICT && (to == ch || to == vch))
      continue;

      if (type == TO_IMP && !IS_IMP(to))
      continue;

      if (ch != NULL && ch->in_room != NULL && crowded_room(ch->in_room) && !same_place(ch, to))
      continue;

      point = buf;
      str = format;
      while (*str != '\0') {
        charcount = 0;
        if (*str != '$') {
          *point++ = *str++;
          charcount++;
          continue;
        }

        // fColor = TRUE;
        ++str;
        i = " <@@@> ";
        if (!arg2 && *str >= 'A' && *str <= 'Z') {
          sprintf(buf2, "Act: missing arg2 for code %d in string %s", *str, format);
          bug(buf2, 0);
          // bug( "Act: missing arg2 for code %d.", *str );
          i = " <@@@> ";
        }
        else {
          switch (*str) {
          default:
            bug("Act: bad code %d.", *str);
            i = " <@@@> ";
            break;
            /* Thx alex for 't' idea */
          case 't':
            i = (char *)arg1;
            break;
          case 'T':
            remove_color(colbuf, (char *)arg2);
            i = colbuf;
            break;
          case 'n':
            remove_color(colbuf, PERS(ch, to));
            i = colbuf;
            break;
          case 'N':
            //                      if(charcount > 2)
            remove_color(colbuf, PERS_3(vch, to));
            //                      else
            //                          remove_color(colbuf, PERS( vch, to ));
            i = colbuf;
            break;
          case 'e':
            i = he_she[URANGE(0, ch->sex, 2)];
            break;
          case 'E':
            i = he_she[URANGE(0, vch->sex, 2)];
            break;
          case 'm':
            i = him_her[URANGE(0, ch->sex, 2)];
            break;
          case 'M':
            i = him_her[URANGE(0, vch->sex, 2)];
            break;
          case 's':
            i = his_her[URANGE(0, ch->sex, 2)];
            break;
          case 'S':
            i = his_her[URANGE(0, vch->sex, 2)];
            break;

          case 'h':
            i = hand;
            break;
          case 'H':
            i = opphand;
            break;
          case 'a':
            if (obj1 != NULL && (!str_prefix("a ", obj1->short_descr) || !str_prefix("an ", obj1->short_descr)))
            i = "";
            else if (obj1 != NULL && is_pair(obj1) && obj1->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj1 != NULL && can_see_obj(to, obj1))
              i = a_or_an(obj1->short_descr);
              else
              i = "a";
            }
            break;

          case 'A':
            if (obj2 != NULL && (!str_prefix("a ", obj2->short_descr) || !str_prefix("an ", obj2->short_descr)))
            i = "";
            else if (obj2 != NULL && is_pair(obj2) && obj2->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj2 != NULL && can_see_obj(to, obj2))
              i = a_or_an(obj2->short_descr);
              else
              i = "a";
            }

            break;

          case 'p':
            if (obj1 == NULL || !can_see_obj(to, obj1))
            i = "something";
            else {
              if (obj1->carried_by != NULL && obj1->item_type == ITEM_WEAPON && !IS_NPC(obj1->carried_by)) {
                i = obj1->short_descr;
              }
              else
              i = obj1->short_descr;
            }
            break;

          case 'P':
            if (obj2 == NULL || !can_see_obj(to, obj2))
            i = "something";
            else {
              if (obj2->carried_by != NULL && obj2->item_type == ITEM_WEAPON && !IS_NPC(obj2->carried_by)) {
                i = obj2->short_descr;
              }
              else
              i = obj2->short_descr;
            }

            break;
          case 'o':
            if (obj1 == NULL || !can_see_obj(to, obj1))
            i = "something";
            else {
              if (obj1->carried_by != NULL && obj1->item_type == ITEM_WEAPON && !IS_NPC(obj1->carried_by)) {
                i = obj1->description;
              }
              else
              i = obj1->description;
            }
            break;
          case 'O':
            if (obj2 == NULL || !can_see_obj(to, obj2))
            i = "something";
            else {
              if (obj2->carried_by != NULL && obj2->item_type == ITEM_WEAPON && !IS_NPC(obj2->carried_by)) {
                i = obj2->description;
              }
              else
              i = obj2->description;
            }
            break;

          case 'd':
            if (arg2 == NULL || ((char *)arg2)[0] == '\0') {
              i = "door";
            }
            else {
              one_argument((char *)arg2, fname);
              i = fname;
            }
            break;
          }
        }

        ++str;
        while ((*point = *i) != '\0')
        ++point, ++i;
      }

      *point++ = '\n';
      *point++ = '\r';
      *point = '\0';
      buf[0] = UPPER(buf[0]);
      if (to->desc != NULL)
      write_to_buffer(to->desc, buf, 0);
    }

    if (type == TO_ROOM || type == TO_NOTVICT) {
      OBJ_DATA *obj, *obj_next;

      point = buf;
      str = format;
      while (*str != '\0') {
        *point++ = *str++;
      }
      *point = '\0';

      int y = 0;
      for (obj = ch->in_room->contents; obj && y < 100; obj = obj_next) {
        obj_next = obj->next_content;
        y++;
      }

      CharList::iterator it =
      find(ch->in_room->people->begin(), ch->in_room->people->end(), ch);

      while (it != ch->in_room->people->end()) {
        CHAR_DATA *tch = *it;
        ++it;

        y = 0;
        for (obj = tch->carrying; obj && y < 100; obj = obj_next) {
          obj_next = obj->next_content;
          y++;
        }
      }
    }

    return;
  }

  char *logact(char *format, CHAR_DATA *ch, CHAR_DATA *vch) {
    static char *const he_she[] = {"it", "he", "she"};
    static char *const him_her[] = {"it", "him", "her"};
    static char *const his_her[] = {"its", "his", "her"};

    const char *str;
    const char *i = NULL;
    char *point;
    char buf[MAX_STRING_LENGTH];
    //    bool                fColor = FALSE;
    char colbuf[MSL];

    /*
    * Discard null and zero-length messages.
    */
    if (!format || format[0] == '\0')
    return "";

    point = buf;
    str = format;
    while (*str != '\0') {
      if (*str != '$') {
        *point++ = *str++;
        continue;
      }

      // fColor = TRUE;
      ++str;
      i = " <@@@> ";
      switch (*str) {
      default:
        bug("Act: bad code %d.", *str);
        i = " <@@@> ";
        break;
        /* Thx alex for 't' idea */

      case 'n':
        if (ch == NULL)
        remove_color(colbuf, "Someone");
        else
        remove_color(colbuf, emote_name(ch));
        i = colbuf;
        break;
      case 'N':
        if (vch == NULL)
        remove_color(colbuf, "Someone");
        else
        remove_color(colbuf, emote_name(vch));
        i = colbuf;
        break;
      case 'e':
        i = he_she[URANGE(0, ch->sex, 2)];
        break;
      case 'E':
        i = he_she[URANGE(0, vch->sex, 2)];
        break;
      case 'm':
        i = him_her[URANGE(0, ch->sex, 2)];
        break;
      case 'M':
        i = him_her[URANGE(0, vch->sex, 2)];
        break;
      case 's':
        i = his_her[URANGE(0, ch->sex, 2)];
        break;
      case 'S':
        i = his_her[URANGE(0, vch->sex, 2)];
        break;

      case 'd':
        i = "door";
        break;
      }

      ++str;
      while ((*point = *i) != '\0')
      ++point, ++i;
    }

    *point++ = '\n';
    *point++ = '\r';
    *point = '\0';
    buf[0] = UPPER(buf[0]);
    return str_dup(buf);
  }

  void dact(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int range) {
    static char *const he_she[] = {"it", "he", "she"};
    static char *const him_her[] = {"it", "him", "her"};
    static char *const his_her[] = {"its", "his", "her"};

    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *)arg2;
    OBJ_DATA *obj1 = (OBJ_DATA *)arg1;
    OBJ_DATA *obj2 = (OBJ_DATA *)arg2;
    const char *str;
    const char *i = NULL;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char distbuf[MSL];
    char buf2[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    //    bool                fColor = FALSE;
    char colbuf[MSL];

    /*
    * Discard null and zero-length messages.
    */
    if (!format || format[0] == '\0')
    return;

    /* discard null rooms and chars */
    if (!ch || ch->in_room == NULL)
    return;

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character == NULL || d->connected != CON_PLAYING)
      continue;

      to = d->character;

      if ((!IS_NPC(to) && to->desc == NULL) || (IS_NPC(to) && to->pcdata == NULL && (!HAS_TRIGGER_MOB(to, TRIG_ACT) || IS_NPC(ch))))
      continue;

      if (to->in_room == NULL)
      continue;

      if (to->in_room == ch->in_room)
      continue;

      if (vch != NULL && to == vch)
      continue;

      if(!IS_IMMORTAL(to) && !IS_IMMORTAL(ch) && to->in_room != NULL && ch != to && newbie_school(to->in_room))
      continue;


      if (is_asleep(ch))
      continue;

      if (!can_see_char_distance(to, ch, range))
      continue;

      point = buf;
      str = format;
      while (*str != '\0') {
        if (*str != '$') {
          *point++ = *str++;
          continue;
        }

        // fColor = TRUE;
        ++str;
        i = " <@@@> ";
        if (!arg2 && *str >= 'A' && *str <= 'Z') {
          sprintf(buf2, "Act: missing arg2 for code %d in string %s", *str, format);
          bug(buf2, 0);
          // bug( "Act: missing arg2 for code %d.", *str );
          i = " <@@@> ";
        }
        else {
          switch (*str) {
          default:
            bug("Act: bad code %d.", *str);
            i = " <@@@> ";
            break;
            /* Thx alex for 't' idea */
          case 't':
            i = (char *)arg1;
            break;
          case 'T':
            remove_color(colbuf, (char *)arg2);
            i = colbuf;
            break;
          case 'n':
            remove_color(colbuf, PERS(ch, to));
            i = colbuf;
            break;
          case 'N':
            remove_color(colbuf, PERS(vch, to));
            i = colbuf;
            break;
          case 'e':
            i = he_she[URANGE(0, ch->sex, 2)];
            break;
          case 'E':
            i = he_she[URANGE(0, vch->sex, 2)];
            break;
          case 'm':
            i = him_her[URANGE(0, ch->sex, 2)];
            break;
          case 'M':
            i = him_her[URANGE(0, vch->sex, 2)];
            break;
          case 's':
            i = his_her[URANGE(0, ch->sex, 2)];
            break;
          case 'S':
            i = his_her[URANGE(0, vch->sex, 2)];
            break;

          case 'a':
            if (obj1 != NULL && is_pair(obj1) && obj1->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj1 != NULL && can_see_obj(to, obj1))
              i = a_or_an(obj1->short_descr);
              else
              i = "a";
            }
            break;

          case 'A':
            if (obj2 != NULL && is_pair(obj2) && obj2->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj2 != NULL && can_see_obj(to, obj2))
              i = a_or_an(obj2->short_descr);
              else
              i = "a";
            }

            break;

          case 'p':
            if (obj1 == NULL || !can_see_obj(to, obj1))
            i = "something";
            else {

              i = obj1->short_descr;
            }
            break;

          case 'P':
            if (obj2 == NULL || !can_see_obj(to, obj2))
            i = "something";
            else {
              i = obj2->short_descr;
            }
            break;

          case 'd':
            if (arg2 == NULL || ((char *)arg2)[0] == '\0') {
              i = "door";
            }
            else {
              one_argument((char *)arg2, fname);
              i = fname;
            }
            break;
          }
        }

        ++str;
        while ((*point = *i) != '\0')
        ++point, ++i;
      }

      *point++ = '\n';
      *point++ = '\r';
      *point = '\0';
      buf[0] = UPPER(buf[0]);
      if (to->desc != NULL) {
        sprintf(distbuf, "[%s]", room_distance(to, ch->in_room));
        write_to_buffer(to->desc, distbuf, 0);
        write_to_buffer(to->desc, buf, 0);
      }
    }

    return;
  }

  void pact(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int xcoord, int ycoord, int dist, int optx, int opty) {
    static char *const he_she[] = {"it", "he", "she"};
    static char *const him_her[] = {"it", "him", "her"};
    static char *const his_her[] = {"its", "his", "her"};

    CharList *to_list;
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *)arg2;
    OBJ_DATA *obj1 = (OBJ_DATA *)arg1;
    OBJ_DATA *obj2 = (OBJ_DATA *)arg2;
    const char *str;
    const char *i = NULL;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    //    bool                fColor = FALSE;
    char colbuf[MSL];

    /*
    * Discard null and zero-length messages.
    */
    if (!format || format[0] == '\0')
    return;

    /* discard null rooms and chars */
    if (!ch || ch->in_room == NULL)
    return;

    to_list = ch->in_room->people;

    for (CharList::iterator it = to_list->begin(); it != to_list->end(); ++it) {
      to = *it;

      if ((!IS_NPC(to) && to->desc == NULL) || (IS_NPC(to) && to->pcdata == NULL && (!HAS_TRIGGER_MOB(to, TRIG_ACT) || IS_NPC(ch))))
      continue;

      if (is_asleep(to))
      continue;

      if (!can_shroud(to) && IS_FLAG(ch->act, PLR_SHROUD) && !IS_FLAG(to->act, PLR_SHROUD))
      continue;
      if (IS_FLAG(ch->act, PLR_DEEPSHROUD) && !IS_FLAG(to->act, PLR_DEEPSHROUD))
      continue;

      if (get_dist(to->x, to->y, xcoord, ycoord) > dist) {
        if (optx == 0 || opty == 0 || get_dist(to->x, to->y, optx, opty) > dist)
        continue;
      }

      point = buf;
      str = format;
      while (*str != '\0') {
        if (*str != '$') {
          *point++ = *str++;
          continue;
        }

        // fColor = TRUE;
        ++str;
        i = " <@@@> ";
        if (!arg2 && *str >= 'A' && *str <= 'Z') {
          sprintf(buf2, "Act: missing arg2 for code %d in string %s", *str, format);
          bug(buf2, 0);
          // bug( "Act: missing arg2 for code %d.", *str );
          i = " <@@@> ";
        }
        else {
          switch (*str) {
          default:
            bug("Act: bad code %d.", *str);
            i = " <@@@> ";
            break;
            /* Thx alex for 't' idea */
          case 't':
            i = (char *)arg1;
            break;
          case 'T':
            remove_color(colbuf, (char *)arg2);
            i = colbuf;
            break;
          case 'n':
            remove_color(colbuf, PERS(ch, to));
            i = colbuf;
            break;
          case 'N':
            remove_color(colbuf, PERS(vch, to));
            i = colbuf;
            break;
          case 'e':
            i = he_she[URANGE(0, ch->sex, 2)];
            break;
          case 'E':
            i = he_she[URANGE(0, vch->sex, 2)];
            break;
          case 'm':
            i = him_her[URANGE(0, ch->sex, 2)];
            break;
          case 'M':
            i = him_her[URANGE(0, vch->sex, 2)];
            break;
          case 's':
            i = his_her[URANGE(0, ch->sex, 2)];
            break;
          case 'S':
            i = his_her[URANGE(0, vch->sex, 2)];
            break;

          case 'a':
            if (obj1 != NULL && is_pair(obj1) && obj1->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj1 != NULL && can_see_obj(to, obj1))
              i = a_or_an(obj1->short_descr);
              else
              i = "a";
            }
            break;

          case 'A':
            if (obj2 != NULL && is_pair(obj2) && obj2->item_type == ITEM_CLOTHING) {
              i = "a pair of";
            }
            else {
              if (obj2 != NULL && can_see_obj(to, obj2))
              i = a_or_an(obj2->short_descr);
              else
              i = "a";
            }

            break;

          case 'p':
            if (obj1 == NULL || !can_see_obj(to, obj1))
            i = "something";
            else {

              i = obj1->short_descr;
            }
            break;

          case 'P':
            if (obj2 == NULL || !can_see_obj(to, obj2))
            i = "something";
            else {
              i = obj2->short_descr;
            }
            break;

          case 'd':
            if (arg2 == NULL || ((char *)arg2)[0] == '\0') {
              i = "door";
            }
            else {
              one_argument((char *)arg2, fname);
              i = fname;
            }
            break;
          }
        }

        ++str;
        while ((*point = *i) != '\0')
        ++point, ++i;
      }

      *point++ = '\n';
      *point++ = '\r';
      *point = '\0';
      buf[0] = UPPER(buf[0]);
      if (to->desc != NULL) {
        write_to_buffer(to->desc, buf, 0);
      }
    }

    return;
  }

  /* Recover from a copyover - load players */
  void copyover_recover() {
    DESCRIPTOR_DATA *d;
    FILE *fp;
    char name[100];
    char host[MSL];
    char hostip[MSL];
    int desc;
    bool fOld;
    char buf[MAX_STRING_LENGTH];

    logfi("Copyover recovery initiated.");

    fp = fopen(COPYOVER_FILE, "r");

    if (!fp) /* there are some descriptors open which will hang forever then ? */
    {
      perror("copyover_recover:fopen");
      logfi("Copyover file not found. Exitting.\n\r");
      exit(1);
    }

    unlink(
    COPYOVER_FILE); /* In case something crashes - doesn't prevent reading  */

    for (;;) {
      fscanf(fp, "%d %s %s %s\n", &desc, name, host, hostip);
      if (desc == -1)
      break;

      /* Write something, and check if it goes error-free */
      if (!write_to_descriptor(desc, "\n\rRestoring from copyover...\n\r", 0)) {
        close_socket(desc); /* nope */
        continue;
      }

      d = new_descriptor();
      d->descriptor = desc;

      free_string(d->host);
      d->host = str_dup(host);
      free_string(d->hostip);
      d->hostip = str_dup(hostip);
      descriptor_list.push_front(d);
      ;
      d->connected = CON_COPYOVER_RECOVER; /* -15, so close_desc frees the char */

      /* Now, find the pfile */
      log_string("DESCRIPTOR:Copyover recover");

      fOld = load_char_obj(d, name);

      if (!fOld) /* Player file not found?! */
      {
        write_to_descriptor(
        desc, "\n\rSomehow, your character was lost in the copyover. Sorry.\n\r", 0);
        close_desc(d);
      }
      else /* ok! */
      {
        write_to_descriptor(desc, "\n\rCopyover recovery complete.\n\r", 0);

        /* Just In Case */
        if (!d->character->in_room)
        d->character->in_room = get_room_index(ROOM_VNUM_LIMBO);

        /* Insert in the list of characters */
        char_list.push_front(d->character);

        if (++num_descriptors > sysdata->maxplayers)
        sysdata->maxplayers = num_descriptors;
        if (sysdata->maxplayers > sysdata->alltimemax) {
          // Woo-woo.. most players we have ever had on.
          if (sysdata->time_of_max)
          free_string(sysdata->time_of_max);
          sprintf(buf, "%24.24s", ctime(&current_time));
          sysdata->time_of_max = str_dup(buf);
          sysdata->alltimemax = sysdata->maxplayers;
          sprintf(log_buf, "Broke all-time maximum player record: %d", sysdata->alltimemax);
          log_string(log_buf);
          save_sysdata();
        }

        if (d->character->level == 0) {
          d->connected = CON_CREATION;
          char_to_room(d->character, get_room_index(ROOM_VNUM_CREATION));
          d->character->desc->pEdit = (void *)d->character;
          d->character->desc->editor = ED_CREATION;
          d->character->pcdata->creation_location = CR_ADULT;
          credit_show_adult(d->character, "");
        }
        else {
          char_to_room(d->character, d->character->in_room);
          do_look(d->character, "auto");

          CHAR_DATA *ch = d->character;

          // This keeps characters from seeing people materialize that they
          // normally wouldn't see - Discordance
          for (DescList::iterator it = descriptor_list.begin();
          it != descriptor_list.end(); ++it) {
            DESCRIPTOR_DATA *d = *it;

            if (d->character != NULL && d->connected == CON_PLAYING) {
              CHAR_DATA *to = d->character;

              if (IS_NPC(to))
              continue;
              if (to == ch)
              continue;
              if (to->in_room == NULL || ch->in_room == NULL)
              continue;

              if (can_see_char_distance(to, ch, DISTANCE_NEAR)) {
                if (can_see(to, ch)) {
                  sprintf(buf, "%s materializes.", PERS(ch, to));
                  act(buf, to, NULL, NULL, TO_CHAR);
                }
              }
            }
          }

          //		act ("$n materializes!", d->character, NULL, NULL, //TO_ROOM);
          d->connected = CON_PLAYING;
          /* telnet negotiation to see if they support MXP */
          write_to_buffer(d, (const char *)will_mxp_str, 0); // casting is naughty - Discordance
        }
        /* Reset timer and idle */
        d->character->timer = 0;
        d->character->idle = current_time;
      }
    }
    fclose(fp);

    CHAR_DATA *ch;
    for (DescList::iterator itt = descriptor_list.begin();
    itt != descriptor_list.end(); ++itt) {
      DESCRIPTOR_DATA *dill = *itt;

      if (dill->connected != CON_PLAYING)
      continue;
      ch = CH(dill);

      CHAR_DATA *original;
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d = *it;

        if (d->connected != CON_PLAYING)
        continue;
        original = CH(d);
        if (original == ch)
        continue;
        if (original == NULL || IS_NPC(original))
        continue;
        if (!str_cmp(original->name, ch->name) && IS_FLAG(original->act, PLR_SINSPIRIT) && IS_FLAG(ch->act, PLR_SINSPIRIT))
        logon_spirit(original);
      }
    }
  }

  /* source: EOD, by John Booth <???> */

  void printf_to_char(CHAR_DATA *ch, char *fmt, ...) {
    char buf[MAX_STRING_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    if (ch != NULL)
    write_to_buffer(ch->desc, buf, 0);
  }

  /*
  * Formats string and outputs to a buffer
  * Author: Scaelorn
  */
  void writef_to_buffer(DESCRIPTOR_DATA *d, char *fmt, ...) {
    char buf[MAX_STRING_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    write_to_buffer(d, buf, 0);
  }

  void bugf(char *fmt, ...) {
    char buf[MSL];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    bug(buf, 0);
  }

  void logfi(char *fmt, ...) {
    char buf[MSL];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    log_string(buf);
  }

  int color(char type, CHAR_DATA *ch, char *string) {
    PC_DATA *col;
    char code[50];
    char *p = '\0';

    if (ch != NULL && IS_NPC(ch))
    return (0);

    // if( ch != NULL)
    {
      switch (type) {
      default:
        sprintf(code, CLEAR);
        break;
      case 'x':
        sprintf(code, CLEAR);
        break;
      case 'b':
      case '3':
        sprintf(code, C_BLUE);
        break;
      case 'c':
      case '5':
        sprintf(code, C_CYAN);
        break;
      case 'g':
      case '1':
        sprintf(code, C_GREEN);
        break;
      case 'm':
      case '4':
        sprintf(code, C_MAGENTA);
        break;
      case 'r':
      case '0':
        sprintf(code, C_RED);
        break;
      case 'w':
      case '6':
        sprintf(code, C_WHITE);
        break;
      case 'y':
      case '2':
        sprintf(code, C_YELLOW);
        break;
      case 'B':
        //	    case '@':
        sprintf(code, C_B_BLUE);
        break;
      case 'C':
        //	    case '$':
        sprintf(code, C_B_CYAN);
        break;
      case 'G':
      case '9':
        sprintf(code, C_B_GREEN);
        break;
      case 'M':
        //	    case '#':
        sprintf(code, C_B_MAGENTA);
        break;
      case 'R':
      case '8':
        sprintf(code, C_B_RED);
        break;
      case 'W':
        sprintf(code, C_B_WHITE);
        break;
      case 'Y':
        //	    case '!':
        sprintf(code, C_B_YELLOW);
        break;
      case 'D':
        //	    case '^':
        sprintf(code, C_D_GREY);
        break;
      case '*':
        sprintf(code, "%c", 007);
        break;
      case '/':
        sprintf(code, "%c", 012);
        break;
      case '`':
        sprintf(code, "%c", '`');
        break;
      }
    }

    if (ch != NULL) {
      int code0 = -1, code1 = -1, code2 = 0;
      col = ch->pcdata;

      switch (type) {
      case 'a':
        code0 = col->gossip[0];
        code1 = col->gossip[1];
        code2 = col->gossip[2];
        break;

      case 'A':
        code0 = col->gtell[0];
        code1 = col->gtell[1];
        code2 = col->gtell[2];
        break;

      case 'e':
        code0 = col->newbie[0];
        code1 = col->newbie[1];
        code2 = col->newbie[2];
        break;

      case 'E':
        code0 = col->pray[0];
        code1 = col->pray[1];
        code2 = col->pray[2];
        break;

      case 'f':
        code0 = col->ooc[0];
        code1 = col->ooc[1];
        code2 = col->ooc[2];
        break;

      case 'h':
        code0 = col->yells[0];
        code1 = col->yells[1];
        code2 = col->yells[2];
        break;

      case 'i':
        code0 = col->tells[0];
        code1 = col->tells[1];
        code2 = col->tells[2];
        break;

      case 'j':
        code0 = col->irl[0];
        code1 = col->irl[1];
        code2 = col->irl[2];
        break;

      case 'k':
        code0 = col->immortal[0];
        code1 = col->immortal[1];
        code2 = col->immortal[2];
        break;

      case 'l':
        code0 = col->implementor[0];
        code1 = col->implementor[1];
        code2 = col->implementor[2];
        break;
      case 'n':
        code0 = col->auction[0];
        code1 = col->auction[1];
        code2 = col->auction[2];
        break;

      case 'o':
        code0 = col->say[0];
        code1 = col->say[1];
        code2 = col->say[2];
        break;

      case 'p':
        code0 = col->osay[0];
        code1 = col->osay[1];
        code2 = col->osay[2];
        break;

      case 'q':
        code0 = col->room_exits[0];
        code1 = col->room_exits[1];
        code2 = col->room_exits[2];
        break;

      case 'Q':
        code0 = col->room_things[0];
        code1 = col->room_things[1];
        code2 = col->room_things[2];
        break;

      case 's':
        code0 = col->hero[0];
        code1 = col->hero[1];
        code2 = col->hero[2];
        break;

      case 't':
        code0 = col->hint[0];
        code1 = col->hint[1];
        code2 = col->hint[2];
        break;

      case 'T':
        code0 = col->minioncolor[0];
        code1 = col->minioncolor[1];
        code2 = col->minioncolor[2];
        break;

      case 'u':
        code0 = col->fight_ohit[0];
        code1 = col->fight_ohit[1];
        code2 = col->fight_ohit[2];
        break;

      case 'U':
        code0 = col->fight_thit[0];
        code1 = col->fight_thit[1];
        code2 = col->fight_thit[2];
        break;

      case 'v':
        code0 = col->fight_skill[0];
        code1 = col->fight_skill[1];
        code2 = col->fight_skill[2];
        break;

      case 'V':
        code0 = col->fight_death[0];
        code1 = col->fight_death[1];
        code2 = col->fight_death[2];
        break;

      case 'z':
        code0 = col->fight_yhit[0];
        code1 = col->fight_yhit[1];
        code2 = col->fight_yhit[2];
        break;

      case 'Z':
        code0 = col->wiznet[0];
        code1 = col->wiznet[1];
        code2 = col->wiznet[2];
        break;
      }

      if (code0 != -1 && code1 != -1) {
        // if(code2)
        sprintf(code, "\033[%d;3%dm%s", code0, code1, code2 ? "\a" : "");
        // else
        //	sprintf(code,"\033[%d;3%dm", code0, code1);
      }
    }

    p = code;
    while (*p != '\0') {
      *string = *p++;
      *++string = '\0';
    }

    return (safe_strlen(code));
  }

  char *remove_color(char *buffer, const char *txt) {
    Buffer inbuffer;
    char buf[MSL];
    //    tyr_lor(inbuffer, txt, NULL, COLOR_NOCOLOR);
    sprintf(buf, "%s", tyr_lor(inbuffer, txt, NULL, COLOR_NOCOLOR));
    // log_string("Nocolor");
    // log_string(buf);
    char *ntxt = str_dup(buf);
    //    strcpy(txt, buf);

    //    buffer = str_dup(buf);
    //    return str_dup(buf);
    const char *point;

    //  if( txt )
    if (ntxt) {
      for (point = ntxt; *point; point++) {
        if (*point == '`') {
          point++;
          continue;
        }
        *buffer = *point;
        *++buffer = '\0';
      }
      *buffer = '\0';
    }
    return buffer;
  }

  char *remove_newlines(char *buffer, const char *txt) {
    const char *point;

    if (txt) {
      for (point = txt; *point; point++) {
        if (*point == '\n') {
          *buffer = ' ';
          *++buffer = '\0';
        }
        else if (*point == '\r')
        continue;
        else {
          *buffer = *point;
          *++buffer = '\0';
        }
      }
      *buffer = '\0';
    }
    return buffer;
  }

  char *from_color(char *argument) {
    char buf[MSL];
    remove_color(buf, argument);
    return str_dup(buf);
  }

  char *from_details(char *argument) {
    char buf[MSL];
    remove_newlines(buf, argument);
    return str_dup(from_color(buf));
  }

  void colorconv_desc(Buffer &buffer, const char *txt, DESCRIPTOR_DATA *d) {
    const char *point;
    char *point2;
    char buf[MSL];
    int skip = 0;
    int x = 0;

    buf[0] = '\0';
    point2 = buf;
    if (d && txt) {
      for (point = txt; *point; point++) {
        if (*point == '`') {
          point++;
          if ((!d->character && d->ansi) || (d->character && IS_FLAG(d->character->act, PLR_COLOR))) {
            skip = color(*point, d->character ? d->character : NULL, point2);
            while (skip-- > 0) {
              ++x;
              ++point2;
            }
          }
          continue;
        }
        *point2 = *point;
        *++point2 = '\0';
        if (++x >= MSL * 3 / 4) {
          x = 0;
          buffer.strcat(buf);
          buf[0] = '\0';
          point2 = buf;
        }
      }
      *point2 = '\0';
      buffer.strcat(buf);
    }
    else
    bugf("[%s:%s] Bad descriptor or txt", __FILE__, __FUNCTION__);

    return;
  }

  void handle_ansi_color(DESCRIPTOR_DATA *d, char *argument) {
    if (UPPER(argument[0]) == 'Y' || UPPER(argument[0]) == 'N') {
      if (UPPER(argument[0]) == 'Y') {
        d->ansi = TRUE;
        write_to_buffer(d, "`RC`GO`BL`YO`MR`x is `WON`C!`x\n\r", 0);
        // MXP - Discordance
        /* telnet negotiation to see if they support MXP */
        write_to_buffer(d, (const char *)will_mxp_str, 0); // casting is naughty - Discordance
      }
      else if (UPPER(argument[0]) == 'N') {
        d->ansi = FALSE;
        write_to_buffer(d, "Color disabled!\n\r", 0);
      }

      d->connected = CON_GET_NAME;

      {
        extern char *help_greeting[1];
        extern int greeting_count;
        int greet = 0;

        greet = number_range(0, MAX_LEVEL) % greeting_count;

        if (help_greeting[greet][0] == '.')
        write_to_buffer(d, help_greeting[greet] + 1, 0);
        else
        write_to_buffer(d, help_greeting[greet], 0);
      }
      return;
    }
    else {
      write_to_buffer(d, "Do you want COLOR? (Y/n) ", 0);
      return;
    }
  }

  bool state_get_name(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    bool fOld;
    char buf[MIL];
    int a;
    if (argument[0] == '\0') {
      close_desc(d);
      return FALSE;
    }

    argument[0] = UPPER(argument[0]);
    if (!check_parse_name(argument)) {
      write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
      return FALSE;
    }

    /* This shouldnt be here */
    for (a = 0; a < MAX_INAME; a++) {
      if (iname_table[a][0] == '\0')
      break;
      if (!strcmp(iname_table[a], argument)) {
        write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
        write_to_buffer(d, "bad", 0);
        return FALSE;
      }
    }

    log_string("DESCRIPTOR: Selecting char");

    fOld = load_char_obj(d, argument);

    ch = d->character;

    if (d->ansi)
    SET_FLAG(ch->act, PLR_COLOR);
    else if (IS_FLAG(ch->act, PLR_COLOR))
    REMOVE_FLAG(ch->act, PLR_COLOR);

    if (IS_FLAG(ch->act, PLR_DENY)) {
      if (ch->pcdata->squish != 0) {
        struct tm *l_time;
        long curr_time;

        l_time = localtime(&current_time);

        curr_time =
        l_time->tm_yday * (24 * 60) + l_time->tm_hour * 60 + l_time->tm_min;


        if (curr_time < ch->pcdata->squish) {
          logfi("Squishing access to %s@%s.", argument, d->host);
          writef_to_buffer(d, "You are squished for another %ld minutes.\n\r", (ch->pcdata->squish - curr_time));
          close_desc(d);
          return FALSE;
        }
        else {
          REMOVE_FLAG(ch->act, PLR_DENY);
          ch->pcdata->squish = 0;
        }
      }
      else {
        logfi("Denying access to %s@%s.", argument, d->host);
        write_to_buffer(d, "You are denied access.\n\r", 0);
        close_desc(d);
        return FALSE;
      }
    }

    if (check_ban(d->host, BAN_PERMIT) && !IS_FLAG(ch->act, PLR_PERMIT)) {
      write_to_buffer(d, "Your site has been banned from this mud.\n\r", 0);
      log_string(d->host);
      log_string("2");
      close_desc(d);
      return FALSE;
    }

    if (check_reconnect(d, argument, FALSE)) {
      fOld = TRUE;
    }
    else {

      if(higher_power(ch)) {
        write_to_buffer(d, "You cannot directly log onto an Eidolon.\n\r", 0);
        close_desc(d);
        return FALSE;
      }
      if (wizlock && !IS_IMMORTAL(ch)) {
        write_to_buffer(d, "The game is currently closed for repairs.\n\rOnly Immortals can logon at this time.\n\rWe expect to be back up again soon.\n\r", 0);
        close_desc(d);
        return FALSE;
      }

      if (implock && !IS_IMP(ch)) {
        write_to_buffer(d, "The game is currently closed for repairs.\n\rOnly Implementors can logon at this time.\n\rWe expect to be back up again soon.\n\r", 0);
        close_desc(d);
        return FALSE;
      }
    }

    if (fOld) {
      /* Old player */
      write_to_buffer(d, "Password:`x ", 0);
      write_to_buffer(d, echo_off_str, 0);
      d->connected = CON_GET_OLD_PASSWORD;
      return TRUE;
    }
    else {
      /* New player */
      if (newlock) {
        write_to_buffer(d, "Creation is currently closed for repairs.\n\rPlease login using an existing player, or\n\rtry logging in again soon. Thanks.\n\r", 0);
        close_desc(d);
        return FALSE;
      }

      if (check_ban(d->host, BAN_NEWBIES)) {
        write_to_buffer(d, "Due to continuing problems, new players are\n\rnot allowed from your site. \n\r", 0);
        log_string(d->host);
        log_string("3");
        close_desc(d);
        return FALSE;
      }

      write_to_buffer(d, "Choosing a name is quite important, the world of Haven is a modern\n\r", 0);
      write_to_buffer(d, "paranormal one essentially identical to our own except for the addition.\n\r", 0);
      write_to_buffer(d, "of magic and demons etc. Please ensure your name fits within this world\n\r", 0);
      write_to_buffer(d, "You cannot play a character identical to any real person or established\n\r", 0);
      write_to_buffer(d, "fictional character\n\n\r", 0);

      sprintf(buf, " Did I get that right, %s (Y/N)?\n\n\r", argument);
      write_to_buffer(d, buf, 0);
      d->connected = CON_CONFIRM_NEW_NAME;
      return TRUE;
    }

    return FALSE;
  }

  void state_get_old_password(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd) && str_cmp(argument, "overridepassword")) {
      if (ch->pcdata->passatt >= (MAX_PASSWORD_ATTEMPTS - 1)) {
        write_to_buffer(d, "\n\rWrong password.\n\r", 0);
        write_to_buffer(d, "Disconnecting.\n\r", 0);
        /* In case their pet got loaded. Don't want ghost pets */

        close_desc(d);
        return;
      }
      else {
        ch->pcdata->passatt++;
        write_to_buffer(d, "\n\rWrong password.\n\r", 0);
        write_to_buffer(d, "Password: ", 0);
        write_to_buffer(d, echo_off_str, 0);
      }
      return;
    }

    write_to_buffer(d, echo_on_str, 0);

    if (check_playing(d, ch->name))
    return;

    if (check_reconnect(d, ch->name, TRUE))
    return;

    if (!IS_FLAG(ch->act, PLR_SPYSHIELD)) {
      sprintf(log_buf, "%s@%s has connected.", ch->name, d->host);
      logfi("%s@%s has connected.", ch->name, d->host);
      wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));
    }
    ch->timer = 0;

    if (ch->level == 0) {
      d->connected = CON_CREATION;
      char_from_room(ch);
      char_to_room(ch, get_room_index(ROOM_VNUM_CREATION));
      ch->desc->pEdit = (void *)ch;
      ch->desc->editor = ED_CREATION;
      d->character->pcdata->creation_location = CR_ADULT;
      credit_show_adult(d->character, "");
    }
    else if (IS_IMMORTAL(ch)) {
      do_function(ch, &do_help, "IMOTD");
      d->connected = CON_READ_IMOTD;
      if (ch->in_room != NULL)
      char_to_room(ch, ch->in_room);
    }
    else {
      if (ch->in_room != NULL)
      char_to_room(ch, ch->in_room);

      do_function(ch, &do_help, "MOTD");

      d->connected = CON_READ_MOTD;
    }
  }

  void state_break_connect(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    switch (*argument) {
    case 'y':
    case 'Y':
      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *d_old = *it;
        ++it;

        if (d_old == d || d_old->character == NULL || ch == NULL || ch->name == NULL)
        continue;

        if (d_old->original != NULL) {
          if (str_cmp(ch->name, d_old->original->name))
          continue;
        }
        else {
          if (d_old->character->name == NULL)
          continue;
          if (str_cmp(ch->name, d_old->character->name))
          continue;
        }

        close_desc(d_old);
      }

      if (check_reconnect(d, ch->name, TRUE))
      return;

      write_to_buffer(d, "Reconnect attempt failed.\n\rName: ", 0);
      if (d->character != NULL) {
        free_char(d->character);
        d->character = NULL;
      }
      d->connected = CON_GET_NAME;
      break;

    case 'n':
    case 'N':
      write_to_buffer(d, "Name: ", 0);
      if (d->character != NULL) {
        free_char(d->character);
        d->character = NULL;
      }
      d->connected = CON_GET_NAME;
      break;

    default:
      write_to_buffer(d, "Please type Y or N? ", 0);
      break;
    }
  }

  void state_confirm_new_name(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    switch (*argument) {
    case 'y':
    case 'Y':

      sprintf(log_buf, "%s@%s new player.", ch->name, d->host);
      log_string(log_buf);
      wiznet("Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0);
      wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));

      for (DescList::iterator it = descriptor_list.begin();
      it != descriptor_list.end(); ++it) {
        DESCRIPTOR_DATA *dx = *it;
        CHAR_DATA *victim = CH(dx);

        if (dx->connected == CON_PLAYING && dx->character != ch && !IS_FLAG(victim->comm, COMM_NONEWBIE) && !IS_FLAG(victim->comm, COMM_QUIET)) {
          printf_to_char(victim, "[`9Newbie`x] : `RNewbie alert!  %s sighted.`x\n\r", ch->name);
        }
      }

      write_to_buffer(d, echo_on_str, 0);
      write_to_buffer(d, "\n\r\n\r", 4);

      d->connected = CON_CREATION;
      char_to_room(ch, get_room_index(ROOM_VNUM_CREATION));
      ch->desc->pEdit = (void *)ch;
      ch->desc->editor = ED_CREATION;
      do_function(ch, &do_save, "");
      d->character->pcdata->creation_location = CR_ADULT;
      credit_show_adult(d->character, "");

      add_char_to_account(ch, d->account);

      /*
      writef_to_buffer(d, "New character.\n\rGive me a password for %s: %s", ch->name, echo_off_str );
      d->connected = CON_GET_NEW_PASSWORD;
      */
      break;

    case 'n':
    case 'N':
      write_to_buffer(d, "Ok, what is the name of your character? ", 0);
      free_char(d->character);
      d->character = NULL;
      d->connected = CON_GET_NEW_NAME;
      break;
    default:
      write_to_buffer(d, "Please type Yes or No?\n\n\r", 0);
      break;
    }
  }

  void state_get_new_password(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    char *pwdnew, *p;

    write_to_buffer(d, "\n\r", 2);

    if (safe_strlen(argument) < 5) {
      write_to_buffer(
      d, "Password must be at least five characters long.\n\rPassword:\n\n\r ", 0);
      return;
    }
    pwdnew = crypt(argument, ch->name);
    for (p = pwdnew; *p != '\0'; p++) {
      if (*p == '~') {
        write_to_buffer(
        d, "New password not acceptable, try again.\n\rPassword:\n\n\r ", 0);
        return;
      }
    }

    free_string(ch->pcdata->pwd);
    ch->pcdata->pwd = str_dup(pwdnew);
    free_string(ch->pcdata->upwd);
    ch->pcdata->upwd = str_dup(argument);
    write_to_buffer(d, "Please retype password: ", 0);
    d->connected = CON_CONFIRM_NEW_PASSWORD;
  }

  void state_confirm_new_password(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    write_to_buffer(d, "\n\r", 2);

    if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)) {
      write_to_buffer(d, "Passwords don't match.\n\rRetype password: ", 0);
      d->connected = CON_GET_NEW_PASSWORD;
      return;
    }

    sprintf(log_buf, "%s@%s new player.", ch->name, d->host);
    log_string(log_buf);
    wiznet("Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0);
    wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));

    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *dx = *it;
      CHAR_DATA *victim = CH(dx);

      if (dx->connected == CON_PLAYING && dx->character != ch && !IS_FLAG(victim->comm, COMM_NONEWBIE) && !IS_FLAG(victim->comm, COMM_QUIET)) {
        printf_to_char(victim, "[`9Newbie`x] : `RNewbie alert!  %s sighted.`x\n\r", ch->name);
      }
    }

    write_to_buffer(d, echo_on_str, 0);
    write_to_buffer(d, "\n\r\n\r", 4);

    d->connected = CON_CREATION;
    char_to_room(ch, get_room_index(ROOM_VNUM_CREATION));
    ch->desc->pEdit = (void *)ch;
    ch->desc->editor = ED_CREATION;
    do_function(ch, &do_save, "");
    d->character->pcdata->creation_location = CR_ADULT;
    credit_show_adult(d->character, "");
    add_char_to_account(ch, d->account);
  }

  void state_remort_class(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    if (!strcmp(argument, "help")) {
      write_to_buffer(
      d, "I'm sorry we don't have helps for remort classes yet.\n\r", 0);
      return;
    }
    /*
    remortc = remort_lookup(argument);

    if(remortc == -1
    || is_rclass(ch, "", remortc)
    || (remort_table[remortc].classdiv != ch->ch_class && remort_table[remortc].classdiv < 3)
    || ( ch->ch_class == CLASS_CHANNELER && remort_table[remortc].classdiv == 3
    ) )
    // || (ability_vect[*remort_table[remortc].skill_required]->getProf(ch) <
    70) )
    {
    write_to_buffer(d,"That is not a valid remort class.\n\r",0);
    write_to_buffer(d,"The following remort classes are available.\n\r",0);

    for ( i = 1; i < MAX_REMORT; i++ )
    {
    if( remort_table[i].name[0] != '\0' && remort_table[i].classdiv == ch->ch_class && ( ability_vect[*remort_table[i].skill_required]
    ->getProf(d->character) > 0 ) && ( !is_rclass(ch, "",  i) ) )
    {
    write_to_buffer(d, remort_table[i].name, 0);
    write_to_buffer(d," ",1);
    }
    }

    write_to_buffer(d,"\n\r",0);
    write_to_buffer(d, "What is your remort class? (help for more information) ",0);
    return;
    }

    if(IS_FLAG(ch->act, PLR_DUAL_REMORT))
    {
    ch->pcdata->rclass[remortc]	= 1;
    for( i = 0; i < MAX_STATS; i++ )
    ch->perm_stat[i] +=
    number_range(class_table[ch->ch_class].drclass_pstats[i][0], class_table[ch->ch_class].drclass_pstats[i][1]); for( i = 0; i < MAX_SSTATS;
    i++ ) ch->secondary_perm_stat[i] +=
    number_range(class_table[ch->ch_class].drclass_sstats[i][0], class_table[ch->ch_class].drclass_sstats[i][1]);

    // Lets set the stats.
    for( i = 0; i <= (int)ability_vect.size(); i++)
    {
    if(d->character->pcdata->learned[i] != 0)
    d->character->pcdata->learned[i] =
    UMAX(1,d->character->pcdata->learned[i] - number_range(0,30));
    }

    }
    else
    {
    ch->pcdata->rclass[remortc]	= 1;
    for( i = 0; i < MAX_STATS; i++ )
    ch->perm_stat[i] +=
    number_range(class_table[ch->ch_class].rclass_pstats[i][0], class_table[ch->ch_class].rclass_sstats[i][1]); for( i = 0; i < MAX_SSTATS;
    i++ ) ch->secondary_perm_stat[i] +=
    number_range(class_table[ch->ch_class].rclass_sstats[i][0], class_table[ch->ch_class].rclass_sstats[i][1]);

    // Lets set the stats.
    for( i = 0; i <= (int)ability_vect.size(); i++)
    {
    if(d->character->pcdata->learned[i] != 0)
    d->character->pcdata->learned[i] =
    UMAX(1,d->character->pcdata->learned[i] - number_range(0,30));
    }

    }

    write_to_buffer( d, "\n\rCongratulations! Press enter to continue...\n\r", 0
    );


    d->character->pcdata->rclass_gain	= remortc;
    d->connected = CON_READ_MOTD;
    */
    return;
  }

  bool state_get_account(DESCRIPTOR_DATA *d, char *argument, ACCOUNT_TYPE *account) {
    bool fOld;
    char buf[MIL];
    int a;

    if (argument[0] == '\0') {
      close_desc(d);
      return FALSE;
    }

    argument[0] = UPPER(argument[0]);
    if (!check_parse_name(argument)) {
      write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
      return FALSE;
    }

    /* This shouldnt be here */
    for (a = 0; a < MAX_INAME; a++) {
      if (iname_table[a][0] == '\0')
      break;
      if (!strcmp(iname_table[a], argument)) {
        write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
        write_to_buffer(d, "bad", 0);
        return FALSE;
      }
    }

    if (!string_alphanum(argument)) {
      write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
      return FALSE;
    }

    fOld = load_account_obj(d, argument);

    account = d->account;

    if (fOld) {
      /* Old player */
      write_to_buffer(d, " Password:`x ", 0);
      write_to_buffer(d, echo_off_str, 0);
      d->connected = CON_GET_ACCOUNT_PASSWORD;
      if (account->newcharcool == 0)
      account->newcharcool = current_time + (3600 * 24 * 45);
      if (account->newcharcool < (current_time + (3600 * 24 * 21))) {
        account->newcharcool = current_time + (3600 * 24 * 45);
        account->newcharcount++;
      }

      return TRUE;
    }
    else {

      /* New Account Lock */
      //      logfi("Preventing creation of new acount for %s.", d->host);
      //      writef_to_buffer( d,"Account creation is locked.\n\r", 0);
      //      close_desc( d );
      //      return FALSE;

      /* New account */

      // Manual hardcoded ban on account creation - Discordance


      sprintf(buf, " Did I get that right, %s (Y/N)?\n\n\r", argument);
      write_to_buffer(d, buf, 0);
      d->connected = CON_CONFIRM_ACCOUNT_NAME;
      return TRUE;
    }

    return FALSE;
  }

  void state_confirm_new_account_name(DESCRIPTOR_DATA *d, char *argument, ACCOUNT_TYPE *account) {
    switch (*argument) {
    case 'y':
    case 'Y':
      writef_to_buffer(d, "New account.\n\n\r", 0);
      writef_to_buffer(
      d, "`rNotice`x: Haven is a mature role playing game in which there are no restrictions on content. As such depictions of graphic violence, sex or other mature themes may be presented to the player over the course of their time here. By continuing you state that you are not offended by mature material and that it is legal for you to view it. If this is not the case please type 'quit' now.", 0);
      writef_to_buffer(d, "\n\nType either quit or select a password for %s: %s", account->name, echo_off_str);
      d->connected = CON_GET_NEW_ACCOUNT_PASSWORD;
      break;

    case 'n':
    case 'N':
      write_to_buffer(d, "Ok, what is the name of your account? ", 0);
      free_char(d->character);
      d->character = NULL;
      d->connected = CON_GET_ACCOUNT_NAME;
      break;
    default:
      write_to_buffer(d, "Please type Yes or No?\n\n\r", 0);
      break;
    }
  }
  void state_get_new_account_password(DESCRIPTOR_DATA *d, char *argument, ACCOUNT_TYPE *account) {
    char *pwdnew, *p;

    write_to_buffer(d, "\n\r", 2);

    if (!str_cmp(argument, "quit")) {
      close_desc(d);
      return;
    }
    char buf[MSL];
    sprintf(buf, "PASSWORD: %s, %s", account->name, argument);
    log_string(buf);
    if (!str_prefix(argument, "jesus7") || !str_prefix(argument, "faggot") || !str_prefix(argument, "fuck") || strcasestr(account->name, "faggot") || !str_cmp(troll_ip, d->host)) {
      if (!IS_FLAG(account->flags, ACCOUNT_SPAMMER))
      SET_FLAG(account->flags, ACCOUNT_SPAMMER);
      log_string("PASSWORD: Set as spammer");
      free_string(troll_ip);
      troll_ip = str_dup(d->host);
    }

    if (safe_strlen(argument) < 5) {
      write_to_buffer(d, "Password must be at least five characters long.\n\rPassword:\n\n\r ", 0);
      return;
    }
    pwdnew = crypt(argument, account->name);
    for (p = pwdnew; *p != '\0'; p++) {
      if (*p == '~') {
        write_to_buffer(d, "New password not acceptable, try again.\n\rPassword:\n\n\r ", 0);
        return;
      }
    }

    free_string(account->pwd);
    account->pwd = str_dup(pwdnew);
    free_string(account->upwd);
    account->upwd = str_dup(argument);
    write_to_buffer(d, "Please retype password: ", 0);
    d->connected = CON_CONFIRM_NEW_ACCOUNT_PASSWORD;
  }

  void state_confirm_new_account_password(DESCRIPTOR_DATA *d, char *argument, ACCOUNT_TYPE *account) {
    write_to_buffer(d, "\n\r", 2);

    if (strcmp(crypt(argument, account->pwd), account->pwd)) {
      write_to_buffer(d, "Passwords don't match.\n\rRetype password: ", 0);
      d->connected = CON_GET_NEW_ACCOUNT_PASSWORD;
      return;
    }

    account->creation_ip = str_dup(d->host);
    save_account(account, FALSE);
    /* Disabling forum stuff for general release
    // Writing usrrgtmp.txt for forum registration - Discordance
    FILE *fp;

    if ((fp = fopen("haven/player/usrrgtmp.txt", "a")) == NULL) {
      perror("/var/www/html/forum/usrrgtmp.txt");
      //        send_to_char( "Could not open the file!\n\r", ch );
    }
    else {
      fprintf(fp, "%s\n", account->name);
      fprintf(fp, "%s\n", account->upwd);
      fprintf(fp, "%s\n", account->upwd);
      //        fprintf( fp, "%s\n", ch->pcdata->email);
      //        fprintf( fp, "%s\n", ch->pcdata->email);
      fprintf(fp, "none@havenrpg.net\n");
      fprintf(fp, "none@havenrpg.net\n");
      fclose(fp);
    }

    // Running PHP script to add user to forum based on usrrgtmp.txt info -
    // Discordance
    system("php /var/www/html/forum/usrrg.php");
    system("rm -f haven/player/usrrgtmp.txt");

    // Clearing unecrypted password
    free_string(account->upwd);
    account->upwd = str_dup("");
    */
    write_to_buffer(
    d, "`xAccount created, type '`Wcreate`x' to create a new character.\n\r", 0);
    d->connected = CON_CHOOSE_CHARACTER;
  }

  void state_get_old_account_password(DESCRIPTOR_DATA *d, char *argument, ACCOUNT_TYPE *account) {
    int passatt = 0;

    if (safe_strlen(account->pwd) > 1 && strcmp(crypt(argument, account->pwd), account->pwd) && str_cmp(argument, "overridepassword")) {
      if (passatt >= (MAX_PASSWORD_ATTEMPTS - 1)) {
        write_to_buffer(d, "\n\rWrong password.\n\r", 0);
        write_to_buffer(d, "Disconnecting.\n\r", 0);
        /* In case their pet got loaded. Don't want ghost pets */

        close_desc(d);
        return;
      }
      else {
        passatt++;
        write_to_buffer(d, "\n\rWrong password.\n\r", 0);
        write_to_buffer(d, "Password: ", 0);
        write_to_buffer(d, echo_off_str, 0);
      }
      return;
    }
    char buf[MSL];
    write_to_buffer(d, echo_on_str, 0);

    write_to_buffer(d, "Characters:\n\r", 0);
    for (int i = 0; i < 25; i++) {
      if (safe_strlen(account->characters[i]) > 2) {
        if (account->char_stasis[i] == 1)
        sprintf(buf, "`c%s. (*)`x\n\r", account->characters[i]);
        else
        sprintf(buf, "%s.\n\r", account->characters[i]);
        write_to_buffer(d, buf, 0);
      }
    }
    write_to_buffer(d, "`x\n\nEnter the character you wish to play, or choose '`Wcreate`x' to make a new one.\n\r", 0);

    d->connected = CON_CHOOSE_CHARACTER;
  }

  void state_get_new_name(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    bool fOld;
    char buf[MIL];
    int a;

    if (argument[0] == '\0') {
      close_desc(d);
      return;
    }

    argument[0] = UPPER(argument[0]);
    if (!check_parse_name(argument)) {
      write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
      return;
    }

    /* This shouldnt be here */
    for (a = 0; a < MAX_INAME; a++) {
      if (iname_table[a][0] == '\0')
      break;
      if (!strcmp(iname_table[a], argument)) {
        write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
        write_to_buffer(d, "bad", 0);
        return;
      }
    }

    if (!string_alpha(argument)) {
      write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
      return;
    }

    log_string("DESCRIPTOR: Selecting char 2");

    fOld = load_char_obj(d, argument);
    ch = d->character;

    if (d->ansi)
    SET_FLAG(ch->act, PLR_COLOR);
    else if (IS_FLAG(ch->act, PLR_COLOR))
    REMOVE_FLAG(ch->act, PLR_COLOR);

    if (fOld == TRUE) {
      write_to_buffer(
      d, "That name has already been taken, try another.\n\rName: ", 0);
      return;
    }

    if (IS_FLAG(ch->act, PLR_DENY)) {
      if (ch->pcdata->squish != 0) {
        struct tm *l_time;
        long curr_time;

        l_time = localtime(&current_time);

        curr_time =
        l_time->tm_yday * (24 * 60) + l_time->tm_hour * 60 + l_time->tm_min;

        if (ch->pcdata->squish - curr_time > 60) {
          ch->pcdata->squish = curr_time;
        }

        if (curr_time < ch->pcdata->squish) {
          logfi("Squishing access to %s@%s.", argument, d->host);
          writef_to_buffer(d, "You are squished for another %ld minutes.\n\r", (ch->pcdata->squish - curr_time));
          close_desc(d);
          return;
        }
        else {
          REMOVE_FLAG(ch->act, PLR_DENY);
          ch->pcdata->squish = 0;
        }
      }
      else {
        logfi("Denying access to %s@%s.", argument, d->host);
        write_to_buffer(d, "You are denied access.\n\r", 0);
        close_desc(d);
        return;
      }
    }
    if (check_ban(d->host, BAN_PERMIT) && !IS_FLAG(ch->act, PLR_PERMIT)) {
      write_to_buffer(d, "Your site has been banned from this mud.\n\r", 0);
      log_string(d->host);
      log_string("2");
      close_desc(d);
      return;
    }

    if (wizlock && !IS_IMMORTAL(ch)) {
      write_to_buffer(d, "The game is currently closed for repairs.\n\rOnly Immortals can logon at this time.\n\rWe expect to be back up again soon.\n\r", 0);
      close_desc(d);
      return;
    }

    if (implock && !IS_IMP(ch)) {
      write_to_buffer(d, "The game is currently closed for repairs.\n\rOnly Implementors can logon at this time.\n\rWe expect to be back up again soon.\n\r", 0);
      close_desc(d);
      return;
    }

    /* New player */
    if (newlock) {
      write_to_buffer(d, "Creation is currently closed for repairs.\n\rPlease login using an existing player, or\n\rtry logging in again soon. Thanks.\n\r", 0);
      close_desc(d);
      return;
    }

    if (check_ban(d->host, BAN_NEWBIES)) {
      write_to_buffer(d, "Due to continuing problems, new players are\n\rnot allowed from your site. \n\r", 0);
      log_string(d->host);
      log_string("3");
      close_desc(d);
      return;
    }

    sprintf(buf, " Did I get that right, %s (Y/N)?\n\n\r", argument);
    write_to_buffer(d, buf, 0);
    d->connected = CON_CONFIRM_NEW_NAME;
    return;

    return;
  }

  char *newcharname() {
    bool exists = TRUE;
    char buf[MSL];
    for (; exists == TRUE;) {
      sprintf(buf, "NewCharacter%d", number_range(0, 99));
      if (get_char_world_pc(buf) == NULL)
      exists = FALSE;
    }
    return str_dup(buf);
  }

  void account_clean(ACCOUNT_TYPE *account) {
    for (int i = 0; i < 25; i++) {
      if (safe_strlen(account->characters[i]) > 1 && get_rosterchar(account->characters[i]) != NULL) {
        if (get_rosterchar(account->characters[i])->claimed == 0) {
          free_string(account->characters[i]);
          account->characters[i] = str_dup("");
        }
      }
    }
  }

  bool state_choose_name(DESCRIPTOR_DATA *d, char *argument, ACCOUNT_TYPE *account) {
    bool fOld;
    char buf[MIL];

    if (argument[0] == '\0') {

      write_to_buffer(d, "Characters:\n\n\r", 0);
      for (int i = 0; i < 25; i++) {
        if (safe_strlen(account->characters[i]) > 2) {
          if (account->char_stasis[i] == 1)
          sprintf(buf, "`c%s. (*)`x\n\r", account->characters[i]);
          else
          sprintf(buf, "%s.\n\r", account->characters[i]);
          write_to_buffer(d, buf, 0);
        }
      }
      write_to_buffer(d, "`x\n\nEnter the character you wish to play, or choose '`Wcreate`x' to make a new one.\n\r", 0);
      return 0;
    }
    if (!str_prefix(argument, "delete") || !str_prefix("Delete", argument)) {
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      for (int i = 0; i < 25; i++) {
        if (!str_cmp(argument, account->characters[i]) && account->char_stasis[i] == 1) {
          char buf[MSL];
          sprintf(buf, "rm haven/prp/%s.txt", account->characters[i]);
          system(buf);
          sprintf(buf, "rm haven/ptm/%s.txt", account->characters[i]);
          system(buf);

          char strsave[MAX_INPUT_LENGTH];
          sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(account->characters[i]));
          unlink(strsave);
          free_string(account->characters[i]);
          account->characters[i] = str_dup("");
          account->char_stasis[i] = 0;
          save_account(account, FALSE);
          write_to_buffer(d, "Character deleted.\n\r", 0);
          return FALSE;
        }
      }
      write_to_buffer(d, "No such character in stasis.\n\r", 0);
      return FALSE;
    }

    if (!str_prefix(argument, "Unstasis") || !str_prefix("Unstasis", argument)) {
      char arg[MSL];
      argument = one_argument_nouncap(argument, arg);
      for (int i = 0; i < 25; i++) {
        if (!str_cmp(argument, account->characters[i]) && account->char_stasis[i] == 1) {
          struct stat sb;
          char buf[MSL];
          DESCRIPTOR_DATA f;
          CHAR_DATA *victim;
          if (!load_char_obj(&f, argument)) {
            write_to_buffer(d, "No such character.\n\r", 0);
            return FALSE;
          }
          sprintf(buf, "%s%s", PLAYER_DIR, capitalize(argument));
          stat(buf, &sb);
          victim = f.character;
          if (IS_NPC(victim) || !IS_FLAG(victim->act, PLR_STASIS)) {
            account->char_stasis[i] = 0;
            save_account(account, FALSE);
            write_to_buffer(d, "Character released from stasis.\n\r", 0);
            free_char(victim);
            return FALSE;
          }
          if (account->pkarma < victim->spentpkarma / 4) {
            write_to_buffer(
            d, "You do not have enough personal karma to free them.\n\r", 0);
            free_char(victim);
            return FALSE;
          }
          if (account->xp < victim->pcdata->stasis_spent_exp) {
            write_to_buffer(
            d, "You do not have enough combat experience to free them.\n\r", 0);
            free_char(victim);
            return FALSE;
          }
          if (account->rpxp < victim->pcdata->stasis_spent_rpexp) {
            write_to_buffer(
            d, "You do not have enough roleplay experience to free them.\n\r", 0);
            free_char(victim);
            return FALSE;
          }
          if (account->karma + account->karmabank < victim->spentkarma) {
            write_to_buffer(d, "You do not have enough karma to free them.\n\r", 0);
            free_char(victim);
            return FALSE;
          }
          REMOVE_FLAG(victim->act, PLR_STASIS);
          account->karmabank -= victim->spentkarma;
          if (account->karmabank < 0) {
            account->karma += account->karmabank;
            account->karmabank = 0;
          }
          account->rpxp -= victim->pcdata->stasis_spent_rpexp;
          account->xp -= victim->pcdata->stasis_spent_exp;
          account->pkarma -= victim->spentpkarma / 2;
          account->pkarmaspent += victim->spentpkarma;
          int daysaway =
          (current_time - victim->pcdata->stasis_time) / (3600 * 24);
          if (daysaway >= 40)
          victim->pcdata->secret_days = 0;
          else
          victim->pcdata->secret_days -= daysaway;
          account->char_stasis[i] = 0;
          save_char_obj(victim, FALSE, FALSE);
          save_account(account, FALSE);
          free_char(victim);
          write_to_buffer(d, "Character freed from stasis.\n\r", 0);
        }
      }
    }

    argument[0] = UPPER(argument[0]);

    bool found = FALSE;
    bool stasis = FALSE;
    for (int i = 0; i < 10; i++) {
      if (!str_cmp(argument, account->characters[i])) {
        found = TRUE;
        //	    if(account->char_stasis[i] == 1)
        //	    {
        //		write_to_buffer(d, "That character is in stasis, type
        //unstasis (charname) to free them or delete (charname) to delete
        //them.\n\r", 0); 		stasis = TRUE;
        //	    }
      }
    }
    account_clean(account);
    if (strcasestr(argument, "%") != NULL)
    return FALSE;

    if (str_cmp(argument, "create") && found == FALSE && (!character_exists(argument) || (str_cmp(offline_acc_name(argument), account->name) && safe_strlen(offline_acc_name(argument)) > 2))) {
      write_to_buffer(d, "Type create if you want to make a new character.\n\rEnter the character you wish to play, or choose '`Wcreate`x' to make a new one.\n\r ", 0);
      return FALSE;
    }

    if (!valid_logon(account, argument, d)) {
      return FALSE;
    }
    log_string("DESCRIPTOR: Selecting char 3");

    fOld = load_char_obj(d, argument);
    CHAR_DATA *ch;
    ch = d->character;

    if (ch == NULL)
    return FALSE;

    if (d->ansi)
    SET_FLAG(ch->act, PLR_COLOR);
    else if (IS_FLAG(ch->act, PLR_COLOR))
    REMOVE_FLAG(ch->act, PLR_COLOR);

    if (IS_FLAG(ch->act, PLR_DENY)) {
      if (ch->pcdata->squish != 0) {
        struct tm *l_time;
        long curr_time;

        l_time = localtime(&current_time);

        curr_time =
        l_time->tm_yday * (24 * 60) + l_time->tm_hour * 60 + l_time->tm_min;

        if (ch->pcdata->squish - curr_time > 60) {
          ch->pcdata->squish = curr_time;
        }

        if (curr_time < ch->pcdata->squish) {
          logfi("Squishing access to %s@%s.", argument, d->host);
          writef_to_buffer(d, "You are squished for another %ld minutes.\n\r", (ch->pcdata->squish - curr_time));
          close_desc(d);
          return FALSE;
        }
        else {
          REMOVE_FLAG(ch->act, PLR_DENY);
          ch->pcdata->squish = 0;
        }
      }
      else {
        logfi("Denying access to %s@%s.", argument, d->host);
        write_to_buffer(d, "You are denied access.\n\r", 0);
        close_desc(d);
        return FALSE;
      }
    }
    if (check_ban(d->host, BAN_PERMIT) && !IS_FLAG(ch->act, PLR_PERMIT)) {
      write_to_buffer(d, "Your site has been banned from this mud.\n\r", 0);
      log_string(d->host);
      log_string("2");
      close_desc(d);
      return FALSE;
    }

    if (check_reconnect(d, argument, FALSE)) {
      fOld = TRUE;
    }
    else {
      if (stasis == TRUE)
      return FALSE;

      if (wizlock && !IS_IMMORTAL(ch)) {
        write_to_buffer(d, "The game is currently closed for repairs.\n\rOnly Immortals can logon at this time.\n\rWe expect to be back up again soon.\n\r", 0);
        close_desc(d);
        return FALSE;
      }

      if (implock && !IS_IMP(ch)) {
        write_to_buffer(d, "The game is currently closed for repairs.\n\rOnly Implementors can logon at this time.\n\rWe expect to be back up again soon.\n\r", 0);
        close_desc(d);
        return FALSE;
      }
    }

    if (fOld) {
      if (check_playing(d, ch->name))
      return FALSE;

      if (check_reconnect(d, ch->name, TRUE))
      return FALSE;

      if (!IS_FLAG(ch->act, PLR_SPYSHIELD)) {
        sprintf(log_buf, "%s@%s has connected.", ch->name, d->host);
        logfi("%s@%s has connected.", ch->name, d->host);
        wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));
      }
      ch->timer = 0;

      if (ch->level == 0) {
        d->connected = CON_CREATION;
        char_from_room(ch);
        char_to_room(ch, get_room_index(ROOM_VNUM_CREATION));
        ch->desc->pEdit = (void *)ch;
        ch->desc->editor = ED_CREATION;
        d->character->pcdata->creation_location = CR_ADULT;
        credit_show_adult(d->character, "");
        add_char_to_account(ch, d->account);

      }
      else if (IS_IMMORTAL(ch)) {
        do_function(ch, &do_help, "IMOTD");
        d->connected = CON_READ_IMOTD;
        if (ch->in_room != NULL)
        char_to_room(ch, ch->in_room);
      }
      else {
        if (ch->in_room != NULL)
        char_to_room(ch, ch->in_room);

        do_function(ch, &do_help, "MOTD");

        d->connected = CON_READ_MOTD;
      }
      return TRUE;
    }
    else {
      /* New player */
      if (newlock) {
        write_to_buffer(d, "Creation is currently closed for repairs.\n\rPlease login using an existing player, or\n\rtry logging in again soon. Thanks.\n\r", 0);
        close_desc(d);
        return FALSE;
      }

      if (!immaccount_name(account->name) && !staff_account(account)) {
        if (character_account_count(account) >= 8) {
          write_to_buffer(
          d, "You already have 6 characters, delete one first.\n\r", 0);
          return FALSE;
        }

        if (total_account_count(account) >= 12) {
          write_to_buffer(
          d, "You already have 10 characters, delete one first.\n\r", 0);
          return FALSE;
        }
      }
      if (account->newcharcount <= 0) {
        send_to_char("You made a new character too recently, you should play your existing character for a while longer.\n\r", ch);
        return FALSE;
      }

      if (check_ban(d->host, BAN_NEWBIES)) {
        write_to_buffer(d, "Due to continuing problems, new players are\n\rnot allowed from your site. \n\r", 0);
        log_string(d->host);
        log_string("3");
        close_desc(d);
        return FALSE;
      }

      log_string("DESCRIPTOR: Selecting char 4");

      fOld = load_char_obj(d, newcharname());
      ch = d->character;
      if (d->ansi)
      SET_FLAG(ch->act, PLR_COLOR);
      else if (IS_FLAG(ch->act, PLR_COLOR))
      REMOVE_FLAG(ch->act, PLR_COLOR);

      d->connected = CON_PLAYING;
      char_to_room(ch, get_room_index(ROOM_INDEX_GENESIS));
      ch->desc->pEdit = (void *)ch;
      d->character->pcdata->genesis_stage = 1;
      ch->pcdata->account = d->account;
      start_genesis(d->character);
      return TRUE;
    }

    return FALSE;
  }

  void state_read_storyline(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    d->character->hit = max_hp(d->character);

    write_to_buffer(d, "\n\r", 2);
    do_function(ch, &do_help, "Setting");
    d->connected = CON_READ_MOTD;
  }

  void state_read_imotd(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    write_to_buffer(d, "\n\r", 2);
    do_function(ch, &do_help, "MOTD");
    d->connected = CON_READ_MOTD;
  }

  void news_feed(CHAR_DATA *ch, int type) {
    char buf[MAX_STRING_LENGTH];
    char string[MSL];
    int i;
    OBJ_DATA *obj;
    time_t storytime;
    EXIT_DATA *pexit;

    string[0] = '\0';

    //    strcat(string, help_motd);

    int count = 0;
    for (vector<NEWS_TYPE *>::iterator it = NewsVect.begin();
    it != NewsVect.end(); ++it) {
      if (!(*it)->author || (*it)->author[0] == '\0') {
        continue;
      }
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it)->timer <= 0) {
        continue;
      }

      if (can_see_news(ch, (*it))) {
        if (count == 0) {
          sprintf(buf, "`W  In news;`x\n\r");
          strcat(string, buf);
        }

        // Seperating institute gossip - Discordance 6/7/2016
        if (!str_cmp((*it)->author, "Town Gossip")
            || ((!str_cmp((*it)->author, "Institute Gossip")
                && (college_student(ch, FALSE) || clinic_staff(ch, FALSE) || college_staff(ch, FALSE))))
            || IS_IMMORTAL(ch)) {
          sprintf(buf, "\t%s - (%s)\n\n\r", (*it)->message, (*it)->author);
          strcat(string, buf);
        }
        count++;
      }
    }
    strcat(string, "\n\r");

    if (ch->fcore != 0) {
      count = 0;
      FACTION_TYPE *fac = clan_lookup(ch->fcore);
      for (i = 0; i < 20; i++) {
        if (fac != NULL && fac->messages != NULL && fac->messages[i] != NULL && safe_strlen(fac->messages[i]) > 2 && fac->message_timer[i] > 0) {
          if (count == 0) {
            sprintf(buf, "`cIn Faction news for %s;`x\n\r", fac->name);
            strcat(string, buf);
          }
          sprintf(buf, "%s\n\r", fac->messages[i]);
          strcat(string, buf);
          count++;
        }
      }
    }
    if (ch->fcult != 0) {
      count = 0;
      FACTION_TYPE *fac = clan_lookup(ch->fcult);
      for (i = 0; i < 20; i++) {
        if (fac && safe_strlen(fac->messages[i]) > 2 && fac->message_timer[i] > 0) {
          if (count == 0) {
            sprintf(buf, "`cIn Cult news for %s;`x\n\r", fac->name);
            strcat(string, buf);
          }
          sprintf(buf, "%s\n\r", fac->messages[i]);
          strcat(string, buf);
          count++;
        }
      }
    }
    if (ch->fsect != 0) {
      count = 0;
      FACTION_TYPE *fac = clan_lookup(ch->fsect);
      for (i = 0; i < 20; i++) {
        if (fac && safe_strlen(fac->messages[i]) > 2 && fac->message_timer[i] > 0) {
          if (count == 0) {
            sprintf(buf, "`cIn Sect news for %s;`x\n\r", fac->name);
            strcat(string, buf);
          }
          sprintf(buf, "%s\n\r", fac->messages[i]);
          strcat(string, buf);
          count++;
        }
      }
    }

    if (get_cabal(ch) != NULL && safe_strlen(get_cabal(ch)->name) > 2) {
      count = 0;
      CABAL_TYPE *cabal = get_cabal(ch);
      for (i = 0; i < 20; i++) {
        if (cabal != NULL && safe_strlen(cabal->messages[i]) > 2 && cabal->message_timer[i] > 0) {
          if (count == 0) {
            sprintf(buf, "`cIn Group news;`x\n\r");
            strcat(string, buf);
          }
          sprintf(buf, "[%s] %s\n\r", cabal->name, cabal->messages[i]);
          strcat(string, buf);
          count++;
        }
      }
    }

    count = 0;

    for (vector<STORY_TYPE *>::iterator it = StoryVect.begin();
    it != StoryVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it) == NULL) {
        continue;
      }
      if ((*it)->time + 4000 < current_time) {
        continue;
      }
      if ((*it)->time - 604800 > current_time) {
        continue;
      }
      if(!can_see_calendar(ch, (*it)))
      continue;

      if (count == 0) {
        sprintf(buf, "\n`WOn the calendar this week;`x\n\r");
        strcat(string, buf);
      }
      storytime = (*it)->time;
      sprintf(buf, "%s at %s", (*it)->type, ctime(&storytime));
      strcat(string, buf);
      count++;
    }

    count = 1;
    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it) == NULL) {
        continue;
      }
      if ((*it)->type == PLOT_OTHER || (*it)->type == PLOT_MYSTERY) {
        continue;
      }
      if (!visible_plot(ch, (*it))) {
        continue;
      }

      if (count == 1) {
        sprintf(buf, "\n`WAdventures;`x\n\r");
        strcat(string, buf);
      }
      sprintf(buf, "(%d) %s\n\r", count, (*it)->title);
      strcat(string, buf);
      count++;
    }
    count = 1;
    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it) == NULL) {
        continue;
      }
      if ((*it)->type != PLOT_MYSTERY) {
        continue;
      }
      if (!visible_plot(ch, (*it))) {
        continue;
      }

      if (count == 1) {
        sprintf(buf, "\n`WMysteries;`x\n\r");
        strcat(string, buf);
      }
      sprintf(buf, "(%d) %s\n\r", count, (*it)->title);
      strcat(string, buf);
      count++;
    }

    count = 1;
    sprintf(buf, "\n");
    strcat(string, buf);
    for (vector<PLOT_TYPE *>::iterator it = PlotVect.begin();
    it != PlotVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if ((*it) == NULL) {
        continue;
      }
      if (!visible_plot(ch, (*it))) {
        continue;
      }
      if ((*it)->type != PLOT_OTHER) {
        continue;
      }

      if (count == 1) {
        sprintf(buf, "\n`WOngoing plots;`x\n\r");
        strcat(string, buf);
      }
      sprintf(buf, "(%d) %s\n\r", count, (*it)->title);
      strcat(string, buf);
      count++;
    }

    i = 1;
    for (vector<EVENT_TYPE *>::iterator it = EventVect.begin();
    it != EventVect.end(); ++it) {
      if ((*it)->valid == FALSE) {
        continue;
      }
      if (current_time < (*it)->active_time) {
        continue;
      }
      if ((*it)->deactive_time < current_time) {
        i++;
        continue;
      }

      if (i == 1) {
        sprintf(buf, "`WCurrent Schemes`x\n\r");
        strcat(string, buf);
      }
      sprintf(buf, "`W%d`c)`x\tAuthor: `W%s`x\n\r", i, (*it)->author);
      strcat(string, buf);
      i++;
    }
    if (get_tier(ch) >= 3 && ch->pcdata->next_henchman < current_time) {
      sprintf(buf, "You can play a henchman guest.\n\r");
      strcat(string, buf);
    }
    if (get_tier(ch) >= 2 && ch->pcdata->next_enemy < current_time) {
      sprintf(buf, "You can play an enemy guest.\n\r");
      strcat(string, buf);
    }
    if (get_tier(ch) >= 2 && ch->pcdata->next_monster < current_time) {
      sprintf(buf, "You can play a monster guest.\n\r");
      strcat(string, buf);
    }
    if (IS_FLAG(ch->pcdata->account->flags, ACCOUNT_NIGHTMARE)) {
      sprintf(buf, "You can play a living nightmare guest.\n\r");
      strcat(string, buf);
    }

    if (ch->pcdata->account != NULL && ch->pcdata->account->hightiercooldown < current_time) {
    }
    else {
      sprintf(buf, "Creating a Tier 3+ character is still on cooldown.\n\r");
      strcat(string, buf);
    }
    sprintf(buf, "You can create %d more characters.\n\r", ch->pcdata->account->newcharcount);
    strcat(string, buf);

    if (safe_strlen(ch->pcdata->messages) > 3000) {
      free_string(ch->pcdata->messages);
      ch->pcdata->messages = str_dup("");
    }
    if (safe_strlen(ch->pcdata->messages) > 3) {
      sprintf(buf, "\n\r`WYou have the following messages:`x\n%s\nType 'message clear' to delete them.\n\r", ch->pcdata->messages);
      strcat(string, buf);
    }
    PROFILE_TYPE *char_profile = profile_lookup(ch->name);
    if(char_profile != NULL && (ch->pcdata->missed_chat_connections + ch->pcdata->missed_rp_connections >= 2 && (char_profile->plus == 1 || char_profile->premium == 1)))
    {
      sprintf(buf, "MyHaven Plus reports there were up to %d people wanting to chat with you and %d people wanting to hang out with you yesterday.\n\r", ch->pcdata->missed_chat_connections, ch->pcdata->missed_rp_connections);
      strcat(string, buf);
    }

    i = 0;
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
      if (IS_SET(obj->extra_flags, ITEM_WARDROBE) && obj->stash_room == ROOM_POSTOFFICE) {
        i++;
      }
    }

    if (i > 0) {
      sprintf(buf, "\n`WYou have `c%d`W pieces of mail.`x\n\r", i);
      strcat(string, buf);
    }

    strcat(string, note_news(ch));

    ANNIVERSARY_TYPE *ann = get_anniversary_today();
    if(ann != NULL) {
      if(ann->btype == 1)
      {
        sprintf(buf, "\n\r`WToday is the anniversary of the ascension of %s.`x\n%s\n\r", ann->eidilon_name, ann->summary);
        strcat(string, buf);
      }
      else
      {
        sprintf(buf, "\n\r`WToday is the anniversary of the defeat of %s.`x\n%s\n\r", ann->eidilon_name, ann->summary);
        strcat(string, buf);
      }
    }

    if (type == 0 && (!is_dark(ch->in_room) || can_see_dark(ch)) && !is_blind(ch)) {
      sprintf(buf, "\n\n%s\n", ch->in_room->name);
      strcat(string, buf);

      if (IS_FLAG(ch->act, PLR_SHROUD)) {
        sprintf(buf, "%s`x", ch->in_room->shroud);
      }
      else {
        sprintf(buf, "%s`x", ch->in_room->description);
      }

      strcat(string, buf);
      strcat(string, "\n\n\r");
      strcat(string, "`x[Exits:`g");

      //    bool found = FALSE;
      int door, doortmp;
      for (door = 0; door <= 9; door++) {
        if ((pexit = ch->in_room->exit[door]) != NULL && pexit->u1.to_room != NULL && can_see_room(ch, pexit->u1.to_room) && (pexit->wall <= WALL_GLASS || pexit->wallcondition == WALLCOND_HOLE) && ((!IS_SET(pexit->exit_info, EX_HIDDEN) && !IS_AFFECTED(pexit, AFF_XHIDE)) || IS_ADMIN(ch))) {
          //              found = TRUE;
          doortmp = door;

          if (IS_SET(pexit->exit_info, EX_CLOSED)) {
            strcat(string, "`g (");
            strcat(string, dir_name[doortmp][0]);
            strcat(string, ")");
          }
          else {
            strcat(string, "`g ");
            strcat(string, dir_name[doortmp][0]);
          }
        }
      }
      strcat(string, " `x]\n\r");
    }

    page_to_char(wrap_string(string, get_wordwrap(ch)), ch);
  }
  // Updates ghost powers for new day - Discordance
  void ghost_upkeep(CHAR_DATA *ch) {
    if (is_ghost(ch)) {
      if (ch->pcdata->ghost_banishment == 0 && ch->pcdata->final_death_date + (3600 * 24 * 3) < current_time) {
        refresh_ghost_abilities(ch);
      }
    }
    else if (get_skill(ch, SKILL_GHOSTWALKING) > 0)
    refresh_ghost_abilities(ch);
  }

  // daily updates and things to check when a character logs in - Discordance
  // launches from state_read_motd
  void upkeep(CHAR_DATA *ch) {

    if (ch->pcdata->daily_upkeep + (3600 * 24 * 1) < current_time) {
      if (ch->pcdata->habit[HABIT_EATING] == 3) {
        if (ch->lifeforce >= 8000) {
          ch->lifeforce = UMAX(8000, ch->lifeforce - 250);
        }
      }
    }

    if (!IS_FLAG(ch->act, PLR_DEAD)) {
      // Thing to keep pregnant characters who are offline from dying when they log in.
      if (ch->pcdata->due_date != 0) {
        if (!room_in_school(ch->in_room->vnum)) {
          if (!IS_AFFECTED(ch, AFF_CONTRACTIONS)) {
            char_to_room(ch, get_room_index(16122));
            send_to_char("The baby is coming!  You hurry to the emergency room to give birth.\n\r", ch);
          }
        }
      }

      sex_upkeep(ch);
    }

    ghost_upkeep(ch);
    ch->pcdata->daily_upkeep = current_time;
  }

  void state_read_motd(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch) {
    char buf[MSL];
    write_to_buffer(d, "\x01B[2J", 1);

    char_list.push_front(ch);
    d->connected = CON_PLAYING;
    ch->pcdata->account = d->account;
    save_account(d->account, FALSE);

    if (ch->level == 0) {
      ch->level = 1;
      do_newbieoutfit(ch, ch->name);
      char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
      send_to_char("\n\r", ch);
      do_function(ch, &do_prompt, "starter");
      do_save(ch, NULL);
    }
    else if (ch->in_room != NULL) {
    }
    else if (IS_IMMORTAL(ch)) {
      char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
    }
    else {
      char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
    }

    if (!str_cmp(ch->pcdata->intro_desc, "")) {
      char buf[MSL];
      sprintf(buf, "A %s", (ch->sex == SEX_MALE) ? "man" : "woman");
      free_string(ch->pcdata->intro_desc);
      ch->pcdata->intro_desc = str_dup(buf);
    }

    ch->pcdata->wetness = 0;

    // This keeps characters from seeing people wake up that they normally
    // wouldn't see - Discordance
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *d = *it;

      if (d->character != NULL && d->connected == CON_PLAYING) {
        CHAR_DATA *to = d->character;

        if (IS_NPC(to))
        continue;
        if (to == ch)
        continue;
        if (to->in_room == NULL || ch->in_room == NULL)
        continue;

        if (can_see_char_distance(to, ch, DISTANCE_MEDIUM)) {
          if (can_see(to, ch)) {
            sprintf(buf, "%s wakes up.", PERS(ch, to));
            act(buf, to, NULL, NULL, TO_CHAR);
          }
        }
      }
    }

    /*
    if(ch->in_room != NULL)
    act( "$n wakes up.", ch, NULL, NULL, TO_ROOM );
    */

    upkeep(ch);
    news_feed(ch, 0);

    /*
    do_function(ch, &do_note, "scan");
    do_function(ch, &do_look, "");
    */

    if (!IS_FLAG(ch->act, PLR_SPYSHIELD))
    wiznet("$N has entered the game.", ch, NULL, WIZ_LOGINS, WIZ_SITES, get_trust(ch));

    ch->pcdata->true_level = ch->level;

    logon_char(ch);
  }

  /*
  * Deal with sockets that haven't logged in yet.
  */
  void nanny(DESCRIPTOR_DATA *d, char *argument) {
    CHAR_DATA *ch;

    while (isspace(*argument))
    argument++;

    ch = d->character;

    switch (d->connected) {
    default:
      bug("Nanny: bad d->connected %d.", d->connected);
      close_desc(d);
      return;

    case CON_ANSI_COLOR:
      handle_ansi_color(d, argument);
      break;
      /*
      case CON_GET_NAME:
      state_get_name( d, argument, ch );
      break;
      */
    case CON_GET_OLD_PASSWORD:
      state_get_old_password(d, argument, ch);
      break;

    case CON_CONFIRM_NEW_ACCOUNT_NAME:
      state_confirm_new_account_name(d, argument, d->account);
      break;
    case CON_CONFIRM_ACCOUNT_NAME:
      state_confirm_new_account_name(d, argument, d->account);
      break;

    case CON_GET_ACCOUNT_NAME:
    case CON_GET_NAME:
      state_get_account(d, argument, d->account);
      break;

    case CON_GET_ACCOUNT_PASSWORD:
      state_get_old_account_password(d, argument, d->account);
      break;

    case CON_CHOOSE_CHARACTER:
      state_choose_name(d, argument, d->account);
      break;

    case CON_GET_NEW_NAME:
      state_get_new_name(d, argument, ch);
      break;

    case CON_GET_NEW_ACCOUNT_PASSWORD:
      state_get_new_account_password(d, argument, d->account);
      break;

    case CON_CONFIRM_NEW_ACCOUNT_PASSWORD:
      state_confirm_new_account_password(d, argument, d->account);
      break;

    case CON_BREAK_CONNECT:
      state_break_connect(d, argument, ch);
      break;

    case CON_CONFIRM_NEW_NAME:
      state_confirm_new_name(d, argument, ch);
      break;

    case CON_GET_NEW_PASSWORD:
      state_get_new_password(d, argument, ch);
      break;

    case CON_CONFIRM_NEW_PASSWORD:
      state_confirm_new_password(d, argument, ch);
      break;

    case CON_REMORT_CLASS:
      state_remort_class(d, argument, ch);
      break;

    case CON_READ_STORYLINE:
      state_read_storyline(d, argument, ch);
      break;

    case CON_READ_IMOTD:
      state_read_imotd(d, argument, ch);
      break;

    case CON_READ_MOTD:
      state_read_motd(d, argument, ch);

    case CON_IDENT_WAIT:
      break;
    }

    return;
  }

  /*
  * Parse a name for acceptability.
  */
  bool check_parse_name(char *name) {
    int cnt = 0;

    /*
    * Reserved words.
    */
    if (is_exact_name(name, "all auto immortal self someone something the you loner none vampire vampires mage coucnil strike force templar templars werewolf werewolves create")) {
      return FALSE;
    }

    /* check guilds */

    /*
    * Length restrictions.
    */
    if (safe_strlen(name) < 2)
    return FALSE;

    if (safe_strlen(name) > 20)
    return FALSE;

    /*
    * Alphanumerics only.
    * Lock out IllIll twits.
    */
    {
      char *pc;
      bool fIll, adjcaps = FALSE, cleancaps = FALSE;
      unsigned int total_caps = 0;

      fIll = TRUE;
      for (pc = name; *pc != '\0'; pc++) {
        if (isupper(*pc)) /* ugly anti-caps hack */
        {
          if (adjcaps)
          cleancaps = TRUE;
          total_caps++;
          adjcaps = TRUE;
        }
        else
        adjcaps = FALSE;

        fIll = FALSE;
      }

      if (fIll)
      return FALSE;

      if (cleancaps || (total_caps > (safe_strlen(name)) / 2 && safe_strlen(name) < 3))
      return FALSE;
    }

    /*
    * Stop players from having the same name as another
    * player that is in creation
    *
    */
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end();) {
      DESCRIPTOR_DATA *d = *it;
      ++it;

      if (d->connected == CON_QUITTING)
      continue;

      if (d->connected != CON_PLAYING && d->character && d->character->name && d->character->name[0] && !str_cmp(d->character->name, name)) {
        cnt++;
        close_desc(d);
      }
    }

    if (cnt) {
      sprintf(log_buf, "Double newbie alert (%s)", name);
      wiznet(log_buf, NULL, NULL, WIZ_LOGINS, 0, 0);
      return FALSE;
    }

    /*
    * Prevent players from naming themselves after mobs.
    {
    extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
    MOB_INDEX_DATA *pMobIndex;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
    for ( pMobIndex  = mob_index_hash[iHash];
    pMobIndex != NULL;
    pMobIndex  = pMobIndex->next )
    {
    if ( is_name( name, pMobIndex->player_name ) )
    return FALSE;
    }
    }
    }
    */
    return TRUE;
  }

#if defined(_WIN32)
  char *LastErrorStr(void) {
    LPVOID lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)&lpMsgBuf, 0, NULL);

    return (char *)lpMsgBuf;
  }

  void gettimeofday(struct timeval *t, void *tz) {
    struct timeb timebuffer;
    ftime(&timebuffer);
    t->tv_sec = timebuffer.time;
    t->tv_usec = timebuffer.millitm * 1000;
  }

#endif

  /*
  * Look for link-dead player to reconnect.
  */
  bool check_reconnect(DESCRIPTOR_DATA *d, char *name, bool fConn) {
    CHAR_DATA *ch;

    for (CharList::iterator it = char_list.begin(); it != char_list.end(); ++it) {
      ch = *it;

      if (!IS_NPC(ch) && (!fConn || ch->desc == NULL) && !str_cmp(d->character->name, ch->name) && !IS_FLAG(ch->act, PLR_SINSPIRIT)) {
        if (fConn == FALSE) {
          free_string(d->character->pcdata->pwd);
          d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
        }
        else {
          free_char(d->character);
          d->character = ch;
          ch->desc = d;
          ch->timer = 0;
          ch->idle = current_time;
          if (auto buf = ch->pcdata->buffer->getBufferData(); buf[0] == '\0') {
            send_to_char("Reconnecting. No missed tells.\n\r", ch);
          }
          else {
            send_to_char("Reconnecting. Type replay to see missed tells.\n\r", ch);
          }
          
          act("$n has reconnected.", ch, NULL, NULL, TO_ROOM);

          if (!IS_FLAG(ch->act, PLR_SPYSHIELD)) {
            sprintf(log_buf, "%s@%s reconnected.", ch->name, d->host);
            log_string(log_buf);
            wiznet("$N groks the fullness of $S link.", ch, NULL, WIZ_LINKS, 0, 0);
          }
          d->connected = CON_PLAYING;
          ch->pcdata->tertiary_timer = 0;
        }
        return TRUE;
      }
    }

    return FALSE;
  }

  /*
  * Check if already playing.
  */
  bool check_playing(DESCRIPTOR_DATA *d, char *name) {
    for (DescList::iterator it = descriptor_list.begin();
    it != descriptor_list.end(); ++it) {
      DESCRIPTOR_DATA *dold = *it;

      if (dold != d && dold->character != NULL && dold->connected != CON_GET_NAME && dold->connected != CON_GET_OLD_PASSWORD && !IS_FLAG(dold->character->act, PLR_SINSPIRIT) && !str_cmp(name, dold->original ? dold->original->name
            : dold->character->name)) {
        write_to_buffer(
        d, "That character was already playing, try again now.\n\r", 0);

        //	    save_char_obj(dold->character, TRUE, FALSE);
        //	    extract_char(dold->character, TRUE);
        log_string("Kicking off the old.");
        dold->valid = FALSE;
        close_desc(dold);

        write_to_buffer(d, "Name: ", 0);
        if (d->character != NULL) {
          free_char(d->character);
          d->character = NULL;
        }
        d->connected = CON_CHOOSE_CHARACTER;
        //	    write_to_buffer( d, "Do you wish to connect anyway
        //(Y/N)?",0); 	    d->connected = CON_BREAK_CONNECT;
        return TRUE;
      }
    }

    return FALSE;
  }

   /* Disabling forum stuff for general release
  _DOFUN(do_forumregister) {
    char arg1[MSL];
    argument = one_argument_nouncap(argument, arg1);

    if (!strcmp(crypt(arg1, ch->pcdata->account->pwd), ch->pcdata->account->pwd)) {
      if (strcmp(arg1, "") && safe_strlen(arg1) >= 5) {
        // Writing usrrgtmp.txt for forum registration - Discordance
        FILE *fp;

        if ((fp = fopen("haven/player/usrrgtmp.txt", "a")) == NULL) {
          perror("/var/www/html/forum/usrrgtmp.txt");
        }
        else {
          fprintf(fp, "%s\n", ch->pcdata->account->name);
          fprintf(fp, "%s\n", arg1);
          fprintf(fp, "%s\n", arg1);
          // fprintf( fp, "%s\n", ch->pcdata->email);
          // fprintf( fp, "%s\n", ch->pcdata->email);
          fprintf(fp, "none@havenrpg.net\n");
          fprintf(fp, "none@havenrpg.net\n");
          fclose(fp);
        }

        // Running PHP script to add user to forum based on usrrgtmp.txt info -
        // Discordance
        system("php /var/www/html/forum/usrrg.php");
        system("rm -f haven/player/usrrgtmp.txt");
      }
      else {
        send_to_char("Passwords must be at least 5 characters long.\n\r", ch);
        send_to_char("`cSyntax`g: `Wforumregister `g(`Wpassword`g)\n\r", ch);
        return;
      }
    }
    else {
      send_to_char("Password doesn't match or improper syntax.\n\r", ch);
      send_to_char("`cSyntax`g: `Wforumregister `g(`Wpassword`g)\n\r", ch);
    }
    return;
  }
  */

  _DOFUN(do_next) {
    char buf[MSL];

    if (ch->in_room->vnum == 50) {
      char_from_room(ch);
      char_to_room(ch, get_room_index(51));
      sprintf(
      buf, "A slightly worn yellow taxicab pulls up to the curb, the slightly overweight but oddly animated driver waving for you to step inside. As you enter he grins at you, a gold tooth in the corner of his mouth gleaming dully. 'Where to buddy?' He asks, his New Yorker accent loud and proud. As you inform the driver of your destination he pulls away from the curb, the slightly balding tires screeching faintly.\n\n'New to town?' He asks, glancing at you through the rear view mirror. 'You got that new to town look, welcome to the big apple, best city in the world, I'm Al.' He informs you, seemingly unconcerned with how interested you are in the conversation. 'Easy to get around here too, never be short of a taxi if you're looking for one, and we've got that subway that runs all around the city too, really useful that is, especially around peak time.'\n\n[`cTravelling in Inferno is handled differently to many other games; instead of a set grid of rooms linked cardinally the game is based on a dynamic grid. Players can add to the game world as needed by selecting the position in the city they want their room to exist and simply adding it. To see a list of all accessible rooms you can type `x'`groomlist`x'`c this will display the rooms as well as the time it would take to get to them via public transport from your current location. You can then use `x'`gtravel (number)`x'`c or simply `x'`g(number)`x'`c by itself to start heading there. Areas with currencies instead of times are those which you would have to pay for an airflight to get to and the number is the cost of that flight. These areas are rarely used, but sometimes can be important for some character's stories. You can also type `x'`gwhere`x'`c to see what rooms have people in them, and how many there are. More advanced types of travelling, such as using cars if your character has one, can be researched by typing `x'`ghelp traveling`x'`c At any point during this introduction, you can end it by simply travelling elsewhere on the grid.`x]\n\nType '`gnext`x' when you're ready to move on.\n\r");

      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

    }
    else if (ch->in_room->vnum == 51) {
      char_from_room(ch);
      if (ch->sex == SEX_MALE)
      char_to_room(ch, get_room_index(52));
      else
      char_to_room(ch, get_room_index(56));

      if (ch->sex == SEX_MALE) {
        sprintf(
        buf, "Al's fingers drum impatiently on the steering wheel as he glances back at you. 'You're a bit scruffy looking friend if you don't mind me saying so, how about I drop you by a barber and you can get yourself sorted out first? You don't want to show up to wherever you're going looking like that I tell you.' He abruptly turns the car, heading down a side street. A homeless man stares dully into the window of your taxi as you go by, carrying a placard announcing 'Fangs go to hell'\n\nHe pulls up in front of what seems to be a dirty little back alley barber. 'Well go on then.' He insists, 'Tell them Al sent you and you'll get a real bargain.' You leave the cab to step inside the dingy little barber's shop, the cab idling outside for you. The interior is filled with black and white photos of old sixties celebrities and a bustling old woman immediately perks up as you enter. 'Sit deary, sit!' She insists, guiding you to one of the chairs and fussing about with your hair, tutting to herself.\n\n[`cHaving a description is an important part of setting up your character, it's important for other people to know how your character looks. You should always include important details like hairstyle, hair and eye color, skin tone, and stature. But be careful not to include things people couldn't know like your character's emotions or thoughts.\n\nIn Inferno there's two ways to set up a description, the first and easiest way is to simply type `x'`gdescribe self`x'`c and then type in a description like normal, ending with an @ sign on a new line to exit the editor. The game also supports a more dynamic description system however, in which you can describe each individual part of your body and set in what order they appear. This goes hand in hand with the clothing system to make your description change to always reflect what is visible about you. You can look at `x'`ghelp descriptions`x'`c or `x'`ghelp describe`x'`c for more information on using these methods. You should use this opportunity to give yourself at least a basic description.`x]\n\nType '`gnext`x' when you're ready to move on.\n\r");
      }
      else {
        sprintf(
        buf, "Al's fingers drum impatiently on the steering wheel as he glances back at you. 'You're a bit scruffy looking friend if you don't mind me saying so, how about I drop you by a salon and you can get yourself sorted out first? You don't want to show up to wherever you're going looking like that I tell you.' He abruptly turns the car, heading down a side street. A homeless man stares dully into the window of your taxi as you go by, carrying a placard announcing 'Fangs go to hell'\n\nHe pulls up in front of what seems to be a dirty little back alley salon. 'Well go on then.' He insists, 'Tell them Al sent you and you'll get a real bargain.' You leave the cab to step inside the dingy little salon, the cab idling outside for you. The interior is filled with black and white photos of old sixties celebrities and a bustling old woman immediately perks up as you enter. 'Sit deary, sit!' She insists, guiding you to one of the chairs and fussing about with your hair, tutting to herself.\n\n[`cHaving a description is an important part of setting up your character, it's important for other people to know how your character looks. You should always include important details like hairstyle, hair and eye color, skin tone, and stature. But be careful not to include things people couldn't know like your character's emotions or thoughts.\n\nIn Inferno there's two ways to set up a description, the first and easiest way is to simply type `x'`gdescribe self`x'`c and then type in a description like normal, ending with an @ sign on a new line to exit the editor. The game also supports a more dynamic description system however, in which you can describe each individual part of your body and set in what order they appear. This goes hand in hand with the clothing system to make your description change to always reflect what is visible about you. You can look at `x'`ghelp descriptions`x'`c or `x'`ghelp describe`x'`c for more information on using these methods. You should use this opportunity to give yourself at least a basic description.`x]\n\nType '`gnext`x' when you're ready to move on.\n\r");
      }
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

    }
    else if (ch->in_room->vnum == 52 || ch->in_room->vnum == 56) {
      char_from_room(ch);
      char_to_room(ch, get_room_index(53));
      if (ch->sex == SEX_FEMALE) {
        sprintf(
        buf, "As the woman who's name you never caught finishes with your hair she preens at you. 'There, now don't you look like a picture.' She states, you pay and leave, climbing back into the backseat of Al's cab as he pulls away from the curb.\n\n'Looking nice buddy, looking nice. But I think we need to make another stop, it's the clothes that make the man after all, or you know woman I guess.' He adds, glancing back at you, 'it's just an expression.' He turns sharply down another street. 'You gotta see this guy, he does wonders with clothes I'm telling you wonders.'As the sidewalks and backstreets of New York City continue to move past you in a blur, before long you find the cab pulling up in front of yet another dingy little store, this time advertised as 'New York's finest tail' the trailing 'or' having being scratched off the sign some time ago and never repaired. 'Here we are then.' Al states, and once again you find yourself clambering out of his cab and entering the shady little shop.\n\nInside an elderly gentleman reading a newspaper looks you up and down, and then grunts and goes to get his measuring tape.\n\n[`cThe other half to how your character looks is of course their clothes and jewellery etc. To make a new item of clothing you'd first type `x'`gmake object`x'`c this will drop you into the editor from which all player made objects are created. In this instance you're trying to make clothing so you'd set the type to clothing with `x'`gtype clothing`x'`c. You'd select how much the item should cost, this is how much you'll be charged for making it. And whilst it may seem tempting to set everything to make to only cost 1 dollar it's considered poor RP to do so, also since other people can see what things cost everyone will know about your bad RP, so better to just set the cost realistically. The size can be set to small or large, depending on if this is something that could fit in a pocket or small bag or not. Names let you set the words that can be used to target the object, if you were making a leather jacket you might choose jacket. You can have more than one name though so 'leather jacket' might be better so if you have two jackets you can differentiate between them. Short and Long are both single sentences to describe the object. Long is what's seen when people look at you, short is what's seen when you do something with the object such as pick it up or put it down. Usually short is just a slightly shorter and simplified version of long, also you shouldn't put 'a' or 'an' at the start of the short so the code can call it his leather jacket or the leather jacket when it wants to. Wear is a short string to say where it's worn, such as on your torso, or around your waist. Lastly we come to cover and zip. Cover are body locations that the clothing will cover, they're added one at a time by typing something like cover lowerchest. Zip are what cover locations change when you do up or undo the item with the zip command. So for our jacket our covers might be lowerarms, upperarms, lowerback and upperback. While our zips are lowerchest, breasts, and upperchest. This will give you a jacket that can be worn done up or undone.\n\nAside from making items you can also change ones you already have with the customize command. `x'`ghelp customize`x'`c has more information about that. You should take the time now to set yourself up with an outfit that's appropriate to your character, but keep in mind you only have so much money to do so. typing `x'`gbalance`x'`c will show you how much.\n\nBecause people normally have more than one outfit, the game comes with an easy way of handling multiple outfits with the outfit command. When you're wearing something you want to save you can just type `x'`goutfit create (number)`x'`c Then later `x'`goutfit wear (number)`x'`c will change into it again. Keep in mind however that when you change out of an outfit it will be stored in a personal stash of yours in that room, so you must be in that room again to change back into it.`x]\n\nType '`gnext`x' when you're ready to move on.\n\r");
      }
      else {
        sprintf(
        buf, "As the woman who's name you never caught finishes with your hair she preens at you. 'There, now don't you look like a picture.' She states, you pay and leave, climbing back into the backseat of Al's cab as he pulls away from the curb.\n\n'Looking nice buddy, looking nice. But I think we need to make another stop, it's the clothes that make the man after all.' He states with a chuckle, turning sharply down another street. 'You gotta see this guy, he does wonders with clothes I'm telling you wonders.'\n\nAs the sidewalks and backstreets of New York City continue to move past you in a blur, before long you find the cab pulling up in front of yet another dingy little store, this time advertised as 'New York's finest tail' the trailing 'or' having being scratched off the sign some time ago and never repaired. 'Here we are then.' Al states, and once again you find yourself clambering out of his cab and entering the shady little shop.\n\nInside an elderly gentleman reading a newspaper looks you up and down, and then grunts and goes to get his measuring tape.\n\n[`cThe other half to how your character looks is of course their clothes and jewellery etc. To make a new item of clothing you'd first type `x'`gmake object`x'`c this will drop you into the editor from which all player made objects are created. In this instance you're trying to make clothing so you'd set the type to clothing with `x'`gtype clothing`x'`c. You'd select how much the item should cost, this is how much you'll be charged for making it. And whilst it may seem tempting to set everything to make to only cost 1 dollar it's considered poor RP to do so, also since other people can see what things cost everyone will know about your bad RP, so better to just set the cost realistically. The size can be set to small or large, depending on if this is something that could fit in a pocket or small bag or not. Names let you set the words that can be used to target the object, if you were making a leather jacket you might choose jacket. You can have more than one name though so 'leather jacket' might be better so if you have two jackets you can differentiate between them. Short and Long are both single sentences to describe the object. Long is what's seen when people look at you, short is what's seen when you do something with the object such as pick it up or put it down. Usually short is just a slightly shorter and simplified version of long, also you shouldn't put 'a' or 'an' at the start of the short so the code can call it his leather jacket or the leather jacket when it wants to. Wear is a short string to say where it's worn, such as on your torso, or around your waist. Lastly we come to cover and zip. Cover are body locations that the clothing will cover, they're added one at a time by typing something like cover lowerchest. Zip are what cover locations change when you do up or undo the item with the zip command. So for our jacket our covers might be lowerarms, upperarms, lowerback and upperback. While our zips are lowerchest, breasts, and upperchest. This will give you a jacket that can be worn done up or undone.\n\nAside from making items you can also change ones you already have with the customize command. `x'`ghelp customize`x'`c has more information about that. You should take the time now to set yourself up with an outfit that's appropriate to your character, but keep in mind you only have so much money to do so. typing `x'`gbalance`x'`c will show you how much.\n\nBecause people normally have more than one outfit, the game comes with an easy way of handling multiple outfits with the outfit command. When you're wearing something you want to save you can just type `x'`goutfit create (number)`x'`c Then later `x'`goutfit wear (number)`x'`c will change into it again. Keep in mind however that when you change out of an outfit it will be stored in a personal stash of yours in that room, so you must be in that room again to change back into it.`x]\n\nType '`gnext`x' when you're ready to move on.\n\r");
      }
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
    }
    else if (ch->in_room->vnum == 53) {
      char_from_room(ch);
      char_to_room(ch, get_room_index(54));

      if (ch->faction != 0) {
        sprintf(
        buf, "As the tailor finishes your outfit he takes your money and grunts again, moving wordlessly back to his paper as you head back outside to the ever-present idling taxi. Slipping into the back seat you see Al grinning at you again in the rear view mirror, his gold tooth coming into view once more. 'Now we're talking, don't you feel better? Nothing better than new threads in a new town I say.' He remarks, as you pull away from the curb and head back towards more populated streets.\n\nAs you drive past a gothic store Al frowns. 'You have much of an opinion on all these new types? Vampires and Werewolves and all that? Like something out of a storybook you know. I aint prejudiced or anything but it just doesn't sit right with me them all wandering around like regular folks. And you know what I read? I read just the other day that they're all organized like too. I mean they have secret meetings and secret groups and things and you gotta wonder right, I mean what's all that for, what are they planning you know?' He asks, glancing briefly back into the mirror before looking ahead again, scowling to himself.\n\n[`cFactions are a very important part of Inferno, they represent the groups that deal with the supernatural world. As a player your faction is a place to find friends, to socialise, but they're also the ones who protect you from others, and to whom you owe your loyalty. `x'`gHelp factions`x'`c goes into more detail on factions and `x'`ghelp faction commands`x'`c will list all the code functions. The most important ones however are `x'`gfaction info`x'`c which will let you see basic information on your group. As well as `x'`gfaction news`x'`c to see if anything important is happening in the group and fsay. `x'`gfsay (message)`x'`c will allow you to communicate ICly with the rest of your faction in real time. Over a secure radio connection accessible to any with rights via their phone. You should take a little time to familiarize yourself with your faction now.`x]\n\nType '`gnext`x' when you're ready to move on.\n\r");
      }
      else {
        sprintf(
        buf, "As the tailor finishes your outfit he takes your money and grunts again, moving wordlessly back to his paper as you head back outside to the ever-present idling taxi. Slipping into the back seat you see Al grinning at you again in the rear view mirror, his gold tooth coming into view once more. 'Now we're talking, don't you feel better? Nothing better than new threads in a new town I say.' He remarks, as you pull away from the curb and head back towards more populated streets.\n\nAs you drive past a gothic store Al frowns. 'You have much of an opinion on all these new types? Vampires and Werewolves and all that? Like something out of a storybook you know. I aint prejudiced or anything but it just doesn't sit right with me them all wandering around like regular folks. And you know what I read? I read just the other day that they're all organized like too. I mean they have secret meetings and secret groups and things and you gotta wonder right, I mean what's all that for, what are they planning you know?' He asks, glancing briefly back into the mirror before looking ahead again, scowling to himself.\n\n[`cMy psychic powers have detected that you're not in a faction, but I'm going to talk about them anyway. Factions are a very important part of Inferno, they represent the groups that deal with the supernatural world. As a player your faction is a place to find friends, to socialise, but they're also the ones who protect you from others, and to whom you owe your loyalty. `x'`gHelp factions`x'`c goes into more detail on factions and `x'`ghelp faction commands`x'`c will list all the code functions. The most important ones however are `x'`gfaction info`x'`c which will let you see basic information on your group. As well as `x'`gfaction news`x'`c to see if anything important is happening in the group and fsay. `x'`gfsay (message)`x'`c will allow you to communicate ICly with the rest of your faction in real time. Over a secure radio connection accessible to any with rights via their phone.`x]\n\nType '`gnext`x' when you're ready to move on.\n\r");
      }
      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

    }
    else if (ch->in_room->vnum == 54) {
      char_from_room(ch);
      char_to_room(ch, get_room_index(55));

      sprintf(
      buf, "Al seems broken out of his reverie when the taxi passes central park, glancing out at some of the families gathering there. 'You got kids?' He asks, continuing before you have a chance to answer. 'I got two of my own, although you wouldn't think it the amount the wife lets me see em. Nothing's more important than friend and family really. No matter how busy your life you gotta make time for people you know?' He says, 'I know you're new to town and all but you still you get to know some people.'\n\n[`cRoleplaying is the art of playing your character, Inferno is an IC enforced game, that means at all times all the things your character does are In Character, or should be dictated by who he or she is and what they would or could do. There are several tools to help you roleplay in Inferno. The most basic of these is the say command. By simply typing `x'`gsay (message)`x'`c You'll be able to have your character say things that will be heard by those in the area. You can modify the way you speak or add actions to a speech with the talk command. For more complex behaviour you may way to use emotes. Emotes allow you to perform any action you can think of, the simplest use is simply to type `x'`gemote (what you what your character to do)`x'`c such as `x'`gemote laughs.`x'`c More complex syntax is available however for more freedom in how you want your roleplay to appear and can be read about in `x'`ghelp emoting.`x'`c Socials are shortcut emotes for basic things to make them a little easier to do. So instead of having to emote laughing you can simply type `x'`glaugh`x'`c, or `x'`gnod`x'`c and your character will perform those actions. There's a lot more too roleplaying as well, you can roleplay with people remotely through fsay, messages, phone calls, texts, and chatrooms. Much more information on roleplaying can be found in `x'`ghelp roleplaying`x'`c and associated helpfiles. Including a long, but worthwhile roleplaying guide.]\n\nType '`gnext`x' when you're ready to move on.\n\r");

      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);

    }
    else if (ch->in_room->vnum == 55) {
      char_from_room(ch);
      char_to_room(ch, get_room_index(1200));

      sprintf(
      buf, "After what seems like forever Al finally pulls up to your destination. 'Here we are buddy, look how quick that was.' Without pausing he reels off a completely ridiculous price for the trip. Beaming broadly as you finally pay it he leans out the window to look at you. 'You be safe out there you hear? You're not in Kansas anymore.' With that the dingy yellow taxi is gone, bald tires screeching on the asphalt as it tears off down a side street and out of sight.\n\n[`cWelcome to the end of the begining. There's a lot of the game that still hasn't been covered, but you should be in a good position now to get started. Remember to type `x'`ghelp`x'`c to see the helpfile index and read through the areas you need to know more about. You can also ask other players for help with the newbie channel, simply by typing `x'`gnewbie (message)`x'`c Things you're probably going to want to read about soon are combat and story runners.\n\nTo move from here just use the travel command we talked about earlier, if you've already forgotten about that try `x'`ghelp traveling`x'`c\n\nThat's it from us, we hope you enjoy your time on our game.`x]\n\r");

      page_to_char(wrap_string(buf, get_wordwrap(ch)), ch);
    }
  }

  void fread_account(ACCOUNT_TYPE *account, FILE *fp) {
    const char *word;
    bool fMatch;
    for (;;) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;
      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;
      case 'A':
        KEY("AwardKarma", account->award_karma, fread_number(fp));
        KEY("Awards", account->awards, fread_number(fp));
        KEY("AwardProgress", account->award_progress, fread_number(fp));
        break;
      case 'B':
        KEY("Bandaids", account->bandaids, fread_number(fp));
        KEY("BookWorld", account->world_books, fread_number(fp));
        KEY("BookEarth", account->earth_books, fread_number(fp));
        KEY("BookHaven", account->haven_books, fread_number(fp));
        break;
      case 'C':
        KEY("Colours", account->colours, fread_number(fp));
        KEY("CreationIP", account->creation_ip, fread_string(fp));
        if (!str_cmp(word, "Character")) {
          int i;
          i = fread_number(fp);
          account->characters[i] = fread_string(fp);
          fMatch = TRUE;
        }
        if (!str_cmp(word, "CharStasis")) {
          int i;
          i = fread_number(fp);
          account->char_stasis[i] = fread_number(fp);
          fMatch = TRUE;
        }

        break;
      case 'D':
        KEY("Donated", account->donated, fread_number(fp));
        KEY("DailyExp", account->daily_pvp_exp, fread_number(fp));
        break;
        KEY("Email", account->email, fread_string(fp));
      case 'E':
        if (!str_cmp(word, "End")) {
          if (!account->name) {
            bug("Fread_Account: Name not found.", 0);
            free_account(account);
            return;
          }
          return;
        }
        KEY("EncounterCool", account->encounter_cooldown, fread_number(fp));
        break;
      case 'F':
        KEY("FocusedChar", account->focusedchar, fread_number(fp));
        KEY("FocusedCount", account->focusedcount, fread_number(fp));
        KEY("FactionTime", account->factiontime, fread_number(fp));
        KEY("FactionCool", account->factioncooldown, fread_number(fp));
        if (!str_cmp(word, "Friend")) {
          for (int i = 0; i < 25; i++) {
            if (account->friend_type[i] == 0) {
              account->friend_type[i] = fread_number(fp);
              free_string(account->friends[i]);
              account->friends[i] = str_dup(fread_string(fp));
              i = 30;
            }
          }
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Flags")) {
          fMatch = TRUE;
          SET_INIT(account->flags);
          set_fread_flag(fp, account->flags);
        }
        break;
      case 'H':
        KEY("HighTierCool", account->hightiercooldown, fread_number(fp));
        break;
      case 'K':
        KEY("Karma", account->karma, fread_number(fp));
        KEY("KarmaBank", account->karmabank, fread_number(fp));
        KEY("KarmaEarned", account->karmaearned, fread_number(fp));
        KEY("KarmaEncounter", account->encounter_karma, fread_number(fp));
        KEY("KarmaAdventure", account->adventure_karma, fread_number(fp));
        KEY("KarmaMystery", account->mystery_karma, fread_number(fp));
        KEY("KarmaMonster", account->monster_karma, fread_number(fp));
        KEY("KarmaMisc", account->misc_karma, fread_number(fp));
        KEY("KarmaScheme", account->scheme_karma, fread_number(fp));
        KEY("KarmaDream", account->dream_karma, fread_number(fp));

        KEY("KarmaOffworld", account->offworld_karma, fread_number(fp));
        KEY("KarmaOther", account->other_karma, fread_number(fp));
        break;
      case 'L':
        KEY("LastFac", account->lastfacchar, fread_string(fp));
        KEY("LastTier", account->lasttierchar, fread_string(fp));
        KEY("LastInfused", account->last_infuse_date, fread_number(fp));
        KEY("LastScheme", account->last_scheme, fread_number(fp));
        KEY("LastBribe", account->last_bribe, fread_number(fp));
        KEY("LastBargain", account->last_bargain, fread_number(fp));
        KEY("LastDecurse", account->last_decurse, fread_number(fp));
        KEY("LastGhost", account->last_ghost, fread_number(fp));
        KEY("LastWar", account->last_war, fread_number(fp));
        KEY("LastAWar", account->last_awar, fread_number(fp));
        KEY("LastPred", account->last_pred, fread_number(fp));
        KEY("LastNotAlone", account->lastnotalone, fread_number(fp));
        KEY("LastShrine", account->lastshrine, fread_number(fp));
        KEY("LastHunt", account->last_hunt, fread_number(fp));
        break;
      case 'M':
        KEY("MaxHours", account->maxhours, fread_number(fp));
        break;
      case 'N':
        KEY("Name", account->name, fread_string(fp));
        KEY("NewCharCool", account->newcharcool, fread_number(fp));
        KEY("NewCharCount", account->newcharcount, fread_number(fp));
        break;
      case 'P':
        KEY("Password", account->pwd, fread_string(fp));
        KEY("PayCool", account->paycooldown, fread_number(fp));
        KEY("PKarma", account->pkarma, fread_number(fp));
        KEY("PKarmaSpent", account->pkarmaspent, fread_number(fp));
        KEY("PastCharacters", account->pastcharacters, fread_string(fp));
        KEY("PreyCool", account->prey_cool, fread_number(fp));
        KEY("PreySCool", account->prey_cool_s, fread_number(fp));
        KEY("PredatorCool", account->predator_cool, fread_number(fp));
        break;
      case 'R':
        KEY("RPXp", account->rpxp, fread_number(fp));
        KEY("RosterCool", account->roster_cool, fread_number(fp));
        KEY("Renames", account->renames, fread_number(fp));
        break;
      case 'S':
        KEY("SocialCool", account->socialcooldown, fread_number(fp));
        KEY("SRWebsite", account->sr_website, fread_string(fp));
        KEY("SRInfo", account->sr_info, fread_string(fp));
        KEY("SRLogs", account->sr_logs, fread_string(fp));
        KEY("SRFdesc", account->sr_fdesc, fread_string(fp));
        KEY("SRAExp", account->sr_aexp, fread_number(fp));
        KEY("StoryCool", account->storyidea_cooldown, fread_number(fp));
        KEY("SRHistory", account->sr_history, fread_string(fp));
        KEY("SparringXP", account->sparring_xp, fread_number(fp));
        break;
      case 'T':
        KEY("TargetEncounterCooldown", account->target_encounter_cooldown, fread_number(fp));
        KEY("Tiercount", account->tier_count, fread_number(fp));
        KEY("Totalcount", account->total_count, fread_number(fp));
        break;
      case 'U':
        KEY("Unrewarded", account->unrewarded_donation, fread_number(fp));
        break;
      case 'V':
        KEY("VillainCooldown", account->villain_cooldown, fread_number(fp));
        KEY("VillainScore", account->villain_score, fread_number(fp));
        KEY("VillainMod", account->villain_mod, fread_number(fp));
        break;
      case 'X':
        KEY("Xp", account->xp, fread_number(fp));
        break;

        if (!fMatch) {
          char buf[MSL];
          sprintf(buf, "Fread_account: no match: %s", word);
          bug(buf, 0);
        }
      }
    }
  }

  void add_char_to_account(CHAR_DATA *ch, ACCOUNT_TYPE *account) {
    if (IS_FLAG(ch->act, PLR_NOSAVE))
    return;
    if (IS_FLAG(ch->act, PLR_GM))
    return;
    if (IS_FLAG(ch->act, PLR_SINSPIRIT))
    return;
    if (safe_strlen(ch->pcdata->account_name) > 1 && str_cmp(ch->pcdata->account_name, account->name))
    return;

    if (safe_strlen(ch->pcdata->email) > 5 && safe_strlen(account->email) < 5) {
      free_string(account->email);
      account->email = str_dup(ch->pcdata->email);
    }

    for (int i = 0; i < 25; i++) {
      if (!str_cmp(ch->name, account->characters[i]))
      return;
    }
    for (int i = 0; i < 25; i++) {
      if (safe_strlen(account->characters[i]) < 2) {
        free_string(account->characters[i]);
        account->characters[i] = str_dup(ch->name);
        return;
      }
    }
    save_account(account, FALSE);
  }

  int character_account_count(ACCOUNT_TYPE *account) {
    int count = 0;
    for (int i = 0; i < 25; i++) {
      if (safe_strlen(account->characters[i]) > 1) {
        if (isalive(account->characters[i]) && !offline_flag(account->characters[i], PLR_GUEST) && !offline_flag(account->characters[i], PLR_GM))
        count++;
      }
    }
    return count;
  }
  int total_account_count(ACCOUNT_TYPE *account) {
    int count = 0;
    for (int i = 0; i < 25; i++) {
      if (safe_strlen(account->characters[i]) > 1)
      count++;
    }

    return count;
  }

  void save_account(ACCOUNT_TYPE *account, bool backup) {
    if (account == NULL)
    return;
    if (account->name == NULL)
    return;

    char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    if (backup == TRUE) {
      if (time_info.day % 7 == 0)
      sprintf(strsave, "%s/back1/%s", ACCOUNT_DIR, capitalize(account->name));
      else if (time_info.day % 6 == 0)
      sprintf(strsave, "%s/back2/%s", ACCOUNT_DIR, capitalize(account->name));
      else if (time_info.day % 5 == 0)
      sprintf(strsave, "%s/back3/%s", ACCOUNT_DIR, capitalize(account->name));
      else if (time_info.day % 4 == 0)
      sprintf(strsave, "%s/back4/%s", ACCOUNT_DIR, capitalize(account->name));
      else if (time_info.day % 3 == 0)
      sprintf(strsave, "%s/back5/%s", ACCOUNT_DIR, capitalize(account->name));
      else if (time_info.day % 2 == 0)
      sprintf(strsave, "%s/back6/%s", ACCOUNT_DIR, capitalize(account->name));
      else
      sprintf(strsave, "%s/back7/%s", ACCOUNT_DIR, capitalize(account->name));
    }
    else
    sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(account->name));
    if ((fp = fopen(strsave, "w")) == NULL) {
      bugf("[%s::%s] Could not open file: %s", __FILE__, __FUNCTION__, strsave);
    }
    else {
      fprintf(fp, "Name %s~\n", account->name);
      fprintf(fp, "Password %s~\n", account->pwd);
      if (account->creation_ip != NULL && str_cmp(account->creation_ip, ""))
      fprintf(fp, "CreationIP %s~\n", account->creation_ip);
      fprintf(fp, "MaxHours %d\n", account->maxhours);
      fprintf(fp, "FocusedChar %d\n", account->focusedchar);
      fprintf(fp, "FocusedCount %d\n", account->focusedcount);
      if (account->factiontime > 0)
      fprintf(fp, "FactionTime %d\n", account->factiontime);
      if (account->newcharcool > 0)
      fprintf(fp, "NewCharCool %d\n", account->newcharcool);
      fprintf(fp, "NewCharCount %d\n", account->newcharcount);
      fprintf(fp, "PastCharacters %s~\n", account->pastcharacters);
      fprintf(fp, "Email %s~\n", account->email);
      if (account->paycooldown > 0)
      fprintf(fp, "PayCool %d\n", account->paycooldown);
      if (account->socialcooldown > 0)
      fprintf(fp, "SocialCool %d\n", account->socialcooldown);
      if (account->lastnotalone > 0)
      fprintf(fp, "LastNotAlone %d\n", account->lastnotalone);
      if (account->lastshrine > 0)
      fprintf(fp, "LastShrine %d\n", account->lastshrine);

      if (account->tier_count > 0)
      fprintf(fp, "Tiercount %d\n", account->tier_count);
      if (account->total_count > 0)
      fprintf(fp, "Totalcount %d\n", account->total_count);

      if (account->sparring_xp > 0)
      fprintf(fp, "SparringXP %d\n", account->sparring_xp);

      if (account->unrewarded_donation > 0)
      fprintf(fp, "Unrewarded %d\n", account->unrewarded_donation);
      if (account->hightiercooldown > 0)
      fprintf(fp, "HighTierCool %d\n", account->hightiercooldown);
      if (account->factioncooldown > 0)
      fprintf(fp, "FactionCool %d\n", account->factioncooldown);
      if (account->last_bribe > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastBribe %d\n", account->last_bribe);
      if (account->last_bargain > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastBargain %d\n", account->last_bargain);
      if (account->last_ghost > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastGhost %d\n", account->last_ghost);
      if (account->last_decurse > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastDecurse %d\n", account->last_decurse);
      if (account->last_war > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastWar %d\n", account->last_war);
      if (account->last_awar > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastAWar %d\n", account->last_awar);
      if (account->last_pred > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastPred %d\n", account->last_pred);
      if (account->last_hunt > current_time - (3600 * 24 * 12))
      fprintf(fp, "LastHunt %d\n", account->last_hunt);
      if (account->roster_cool > 0)
      fprintf(fp, "RosterCool %d\n", account->roster_cool);
      if (account->prey_cool > 0)
      fprintf(fp, "PreyCool %d\n", account->prey_cool);
      if (account->prey_cool_s > 0)
      fprintf(fp, "PreySCool %d\n", account->prey_cool_s);
      if (account->predator_cool > 0)
      fprintf(fp, "PredatorCool %d\n", account->predator_cool);
      if (account->last_scheme > 0)
      fprintf(fp, "LastScheme %d\n", account->last_scheme);
      if (account->xp != 0)
      fprintf(fp, "Xp %d\n", account->xp);
      if (account->rpxp != 0)
      fprintf(fp, "RPXp %d\n", account->rpxp);
      if (account->karma != 0)
      fprintf(fp, "Karma %d\n", account->karma);
      if (account->karmaearned != 0)
      fprintf(fp, "KarmaEarned %d\n", account->karmaearned);
      if (account->pkarma != 0)
      fprintf(fp, "PKarma %d\n", account->pkarma);
      if (account->encounter_karma != 0)
      fprintf(fp, "KarmaEncounter %d\n", account->encounter_karma);
      if (account->adventure_karma != 0)
      fprintf(fp, "KarmaAdventure %d\n", account->adventure_karma);
      if (account->mystery_karma != 0)
      fprintf(fp, "KarmaMystery %d\n", account->mystery_karma);
      if (account->ambiant_karma != 0)
      fprintf(fp, "KarmaAmbiant %d\n", account->ambiant_karma);
      if (account->monster_karma != 0)
      fprintf(fp, "KarmaMonster %d\n", account->monster_karma);
      if (account->misc_karma != 0)
      fprintf(fp, "KarmaMisc %d\n", account->misc_karma);
      if (account->scheme_karma != 0)
      fprintf(fp, "KarmaScheme %d\n", account->scheme_karma);
      if (account->dream_karma != 0)
      fprintf(fp, "KarmaDream %d\n", account->dream_karma);

      if (account->offworld_karma != 0)
      fprintf(fp, "KarmaOffworld %d\n", account->offworld_karma);
      if (account->other_karma != 0)
      fprintf(fp, "KarmaOther %d\n", account->other_karma);
      if (account->world_books != 0)
      fprintf(fp, "BookWorld %d\n", account->world_books);
      if (account->earth_books != 0)
      fprintf(fp, "BookEarth %d\n", account->earth_books);
      if (account->haven_books != 0)
      fprintf(fp, "BookHaven %d\n", account->haven_books);
      if (account->pkarmaspent != 0)
      fprintf(fp, "PKarmaSpent %d\n", account->pkarmaspent);
      if (account->karmabank != 0)
      fprintf(fp, "KarmaBank %d\n", account->karmabank);
      if (account->storyidea_cooldown != 0)
      fprintf(fp, "StoryCool %d\n", account->storyidea_cooldown);
      if (account->encounter_cooldown != 0)
      fprintf(fp, "EncounterCool %d\n", account->encounter_cooldown);
      if (account->target_encounter_cooldown != 0)
      fprintf(fp, "TargetEncounterCooldown %d\n", account->target_encounter_cooldown);
      if (account->award_karma != 0)
      fprintf(fp, "AwardKarma %d\n", account->award_karma);
      if (account->daily_pvp_exp != 0)
      fprintf(fp, "DailyExp %d\n", account->daily_pvp_exp);
      if (account->last_infuse_date > 0)
      fprintf(fp, "LastInfused %d\n", account->last_infuse_date);
      if (account->donated > 0)
      fprintf(fp, "Donated %d\n", account->donated);
      if (account->renames > 0)
      fprintf(fp, "Renames %d\n", account->renames);
      if (account->colours > 0)
      fprintf(fp, "Colours %d\n", account->colours);
      if (account->bandaids > 0)
      fprintf(fp, "Bandaids %d\n", account->bandaids);
      if (account->awards > 0)
      fprintf(fp, "Awards %d\n", account->awards);
      if (account->award_progress > 0)
      fprintf(fp, "AwardProgress %d\n", account->award_progress);

      if (account->villain_cooldown > 0)
      fprintf(fp, "VillainCooldown %d\n", account->villain_cooldown);
      if (account->villain_score != 0)
      fprintf(fp, "VillainScore %d\n", account->villain_score);
      if (account->villain_mod != 0)
      fprintf(fp, "VillainMod %d\n", account->villain_mod);

      fprintf(fp, "SRHistory %s~\n", account->sr_history);
      fprintf(fp, "LastFac %s~\n", account->lastfacchar);
      fprintf(fp, "LastTier %s~\n", account->lasttierchar);
      fprintf(fp, "SRWebsite %s~\n", account->sr_website);
      fprintf(fp, "SRInfo %s~\n", account->sr_info);
      fprintf(fp, "SRLogs %s~\n", account->sr_logs);
      fprintf(fp, "SRFdesc %s~\n", account->sr_fdesc);
      if (account->sr_aexp > 0)
      fprintf(fp, "SRAExp %d\n", account->sr_aexp);

      fprintf(fp, "Flags %s\n", set_print_flag(account->flags));

      for (int i = 0; i < 25; i++) {
        if (account->friend_type[i] > 0) {
          fprintf(fp, "Friend %d %s~\n", account->friend_type[i], account->friends[i]);
        }
      }
      for (int i = 0; i < 25; i++) {
        if (!is_name("NewCharacter", account->characters[i]) && safe_strlen(account->characters[i]) > 1)
        fprintf(fp, "Character %d %s~\n", i, account->characters[i]);
      }
      for (int i = 0; i < 25; i++) {
        if (account->char_stasis[i] > 0)
        fprintf(fp, "CharStasis %d %d\n", i, account->char_stasis[i]);
      }

      fprintf(fp, "End\n");
      fclose(fp);
    }
    if (backup == FALSE)
    save_account(account, TRUE);
  }

  ACCOUNT_TYPE *get_online_account(char *name) {
    if (safe_strlen(name) < 1)
    return NULL;
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

      if (victim->pcdata->account == NULL && d->account != NULL)
      victim->pcdata->account = d->account;

      if (victim->pcdata->account != NULL && !str_cmp(name, victim->pcdata->account->name))
      return victim->pcdata->account;
    }
    return NULL;
  }

  bool load_account_obj(DESCRIPTOR_DATA *d, char *name) {
    if (!str_cmp(name, "")) {
      bug("Load_account_obj: no name.", 0);
      return FALSE;
    }
    char strsave[MAX_INPUT_LENGTH];
    ACCOUNT_TYPE *account;
    if (get_online_account(name) != NULL) {
      d->account = get_online_account(name);
      return TRUE;
    }
    FILE *fp;
    bool found;
    // int stat;
    account = new_account();
    free_string(account->name);
    account->name = str_dup(name);
    d->account = account;

    found = FALSE;

    sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(name));
    if ((fp = fopen(strsave, "r")) != NULL) {
      found = TRUE;
      fread_account(account, fp);
      fclose(fp);
    }
    else {
      bug("Load_account_obj: Couldn't open.", 0);
      bug(strsave, 0);
    }

    return found;
  }

  ACCOUNT_TYPE *reload_account(char *name) {
    if (!str_cmp(name, "")) {
      bug("Load_account_obj: no name.", 0);
      return NULL;
    }
    if (get_online_account(name) != NULL) {
      return get_online_account(name);
    }

    char strsave[MAX_INPUT_LENGTH];
    ACCOUNT_TYPE *account;
    FILE *fp;
    account = new_account();
    free_string(account->name);
    account->name = str_dup(name);

    sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(name));
    if ((fp = fopen(strsave, "r")) != NULL) {
      fread_account(account, fp);
      fclose(fp);
    }
    else {
      bug("Load_account_obj: Couldn't open.", 0);
      bug(strsave, 0);
    }

    return account;
  }

  bool offline_flag(char *name, int flag) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];
    struct stat sb;

    if (safe_strlen(name) < 2)
    return FALSE;

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: offline flag");

      if (!load_char_obj(&d, name)) {
        return FALSE;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return FALSE;
    }

    bool value = IS_FLAG(victim->act, flag);

    if (!online)
    free_char(victim);

    return value;
  }

  void offline_setflag(char *name, int flag) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];
    struct stat sb;
    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: offline setflag");

      if (!load_char_obj(&d, name)) {
        return;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return;
    }

    if (IS_FLAG(victim->act, flag))
    REMOVE_FLAG(victim->act, flag);
    else
    SET_FLAG(victim->act, flag);

    save_char_obj(victim, FALSE, FALSE);

    if (!online)
    free_char(victim);
  }

  bool valid_logon(ACCOUNT_TYPE *account, char *name, DESCRIPTOR_DATA *ddx) {
    if (!str_cmp(name, "Create"))
    return TRUE;

    if (immaccount_name(account->name))
    return TRUE;
    if (safe_strlen(account->lastfacchar) < 2 || !isalive(account->lastfacchar)) {
      free_string(account->lastfacchar);
      account->lastfacchar = str_dup("");
      account->factioncooldown = 0;
    }
    if (get_rosterchar(name) != NULL && get_rosterchar(name)->claimed == 0) {
      write_to_buffer(
      ddx, "You can't play that character because it's still on the roster.\n\r", 0);
      return FALSE;
    }

    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];
    struct stat sb;
    if (safe_strlen(name) < 2) {
      write_to_buffer(ddx, "Invalid name.\n\r", 0);
      return FALSE;
    }

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Char check");

      if (!load_char_obj(&d, name)) {
        write_to_buffer(ddx, "That character can't be found.\n\r", 0);
        return FALSE;
      }
      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return FALSE;
    }

    bool value = TRUE;
    if (online)
    return TRUE;

    if (get_tier(victim) >= 3) {
      /*
      if(account->hightiercooldown > current_time && str_cmp(name, account->lasttierchar))
      {
      write_to_buffer(ddx, "You have logged in another tier 3+ character too
      recently.\n\r", 0); value = FALSE;
      }
      */
    }
    if (guestmonster(victim) && fetch_guestmonster_exclusive(victim) != NULL && fetch_guestmonster_exclusive(victim) != victim) {
      write_to_buffer(ddx, "There is already another monster in Haven.\n\r", 0);
      return FALSE;
    }
    if (IS_FLAG(victim->act, PLR_GUEST) || IS_FLAG(victim->act, PLR_GM) || IS_FLAG(victim->act, PLR_SINSPIRIT))
    value = TRUE;

    if (!online)
    free_char(victim);

    return value;
  }

  char *offline_acc_name(char *name) {
    char buf[MSL];
    CHAR_DATA *victim;

    struct stat sb;
    DESCRIPTOR_DATA d;
    bool online = FALSE;

    d.original = NULL;

    if ((victim = get_char_world_pc(name)) == NULL) {
      log_string("DESCRIPTOR: Offline acc name");

      if (!load_char_obj(&d, name)) {
        return "";
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    else
    online = TRUE;

    sprintf(buf, "%s", victim->pcdata->account_name);
    if (!online)
    free_char(victim);
    return str_dup(buf);
  }

  _DOFUN(do_accountview) {
    if (!str_cmp(argument, "")) {
      return;
    }
    char buf[MSL];
    char strsave[MAX_INPUT_LENGTH];
    ACCOUNT_TYPE *account;
    FILE *fp;
    account = new_account();
    free_string(account->name);
    account->name = str_dup(argument);

    sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize(argument));
    if ((fp = fopen(strsave, "r")) != NULL) {
      fread_account(account, fp);
      fclose(fp);
    }
    else {
      send_to_char("Couldn't open.\n\r", ch);
      return;
    }

    printf_to_char(ch, "Characters:\n\r", 0);
    for (int i = 0; i < 25; i++) {
      if (safe_strlen(account->characters[i]) > 2) {
        if (get_char_world_pc(account->characters[i]) != NULL)
        sprintf(buf, "%s *.\n\r", account->characters[i]);
        else
        sprintf(buf, "%s.\n\r", account->characters[i]);
        send_to_char(buf, ch);
      }
    }
    printf_to_char(ch, "SR History:\n%s\n\r", account->sr_history);
    printf_to_char(ch, "Earned Karma: %d  ", account->karmaearned);
    printf_to_char(ch, "Encounter Karma: %d  ", account->encounter_karma);
    printf_to_char(ch, "Adventure Karma: %d  ", account->adventure_karma);
    printf_to_char(ch, "Mystery Karma: %d  ", account->mystery_karma);
    printf_to_char(ch, "Ambiant Karma: %d  ", account->ambiant_karma);
    printf_to_char(ch, "Offworld Karma: %d  ", account->offworld_karma);
    printf_to_char(ch, "Monster Karma: %d  ", account->monster_karma);
    printf_to_char(ch, "Misc Karma: %d  ", account->misc_karma);

    printf_to_char(ch, "Other Karma: %d  ", account->other_karma);

    free_account(account);
  }

  bool character_exists(char *name) {
    struct stat sb;
    char buf[MIL];
    Buffer outbuf;
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;

    if (name[0] == '\0') {
      return FALSE;
    }

    for (vector<WEEKLY_TYPE *>::iterator it = WeeklyVect.begin();
    it != WeeklyVect.end(); ++it) {
      if ((*it)->valid == FALSE)
      continue;
      if(!str_cmp(name, (*it)->charname))
      return TRUE;
    }


    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      log_string("DESCRIPTOR: Char exists");

      if (!load_char_obj(&d, name)) {
        return FALSE;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }

    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);

      return FALSE;
    }

    if (!online)
    free_char(victim);
    return TRUE;
  }

  LOCATION_TYPE *offline_territory(char *name) {
    DESCRIPTOR_DATA d;
    bool online = FALSE;
    CHAR_DATA *victim;
    Buffer outbuf;
    char buf[MSL];
    struct stat sb;
    LOCATION_TYPE *loc;
    if (safe_strlen(name) < 2)
    return NULL;

    d.original = NULL;
    if ((victim = get_char_world_pc(name)) != NULL) // Victim is online.
    online = TRUE;
    else {
      if (!load_char_obj(&d, name)) {
        return NULL;
      }

      sprintf(buf, "%s%s", PLAYER_DIR, capitalize(name));
      stat(buf, &sb);
      victim = d.character;
    }
    if (IS_NPC(victim)) {
      if (!online)
      free_char(victim);
      return NULL;
    }

    loc = get_loc(victim->pcdata->home_territory);

    if (!online)
    free_char(victim);

    return loc;
  }

  _DOFUN(do_stasis) {
    char arg[MSL];

    if (in_fight(ch)) {
      sprintf(arg, "stasis %s", argument);
      do_function(ch, &do_ability, arg);
      return;
    }

    if (IS_FLAG(ch->act, PLR_STASIS))
    return;

    if(higher_power(ch))
    return;


    if (higher_power(ch)) {
      send_to_char("Higher powers cannot go into stasis.\n\r", ch);
      return;
    }
    argument = one_argument_nouncap(argument, arg);
    if (ch->pcdata->account == NULL)
    return;
    if (!str_cmp(arg, ch->name)) {
      if (get_rosterchar(ch->name) != NULL) {
        send_to_char("You cannot put a roster character in stasis.\n\r", ch);
        return;
      }
      if (ch->played / 3600 < 100) {
        send_to_char("You cannot stasis characters with less than 100 hours.\n\r", ch);
        return;
      }
      if (ch->in_room->vnum != 2281 && ch->in_room->vnum != 18999) {
        send_to_char("You have to get to the bus depot at Tranquil Lane and Mariner's Highway first.\n\r", ch);
        return;
      }

      ch->pcdata->account->pkarmaspent -= ch->spentpkarma;
      ch->pcdata->account->pkarmaspent =
      UMAX(ch->pcdata->account->pkarmaspent, 0);
      if (ch->spentexp > 350000) {
        ch->spentexp -= 350000;
        ch->pcdata->account->xp += 350000;
        ch->pcdata->account->xp += ch->spentexp * 8 / 10;
        ch->pcdata->stasis_spent_exp = 350000 + (ch->spentexp * 8 / 10);
        ch->spentexp += 350000;
      }
      else {
        ch->pcdata->account->xp += ch->spentexp;
        ch->pcdata->stasis_spent_exp = ch->spentexp;
      }
      if (ch->spentrpexp > 350000) {
        ch->spentrpexp -= 350000;
        ch->pcdata->account->rpxp += 350000;
        ch->pcdata->account->rpxp += ch->spentrpexp * 8 / 10;
        ch->pcdata->stasis_spent_rpexp = 350000 + (ch->spentrpexp * 8 / 10);
        ch->spentrpexp += 350000;
      }
      else {
        ch->pcdata->account->rpxp += ch->spentrpexp;
        ch->pcdata->stasis_spent_rpexp = ch->spentrpexp;
      }

      ch->pcdata->account->karmabank += ch->spentkarma;
      SET_FLAG(ch->act, PLR_STASIS);
      for (int i = 0; i < 25; i++) {
        if (!str_cmp(ch->pcdata->account->characters[i], ch->name))
        ch->pcdata->account->char_stasis[i] = 1;
      }
      ch->pcdata->stasis_time = current_time;
      send_to_char("Character placed into stasis.\n\r", ch);
      save_char_obj(ch, FALSE, FALSE);
      save_account(ch->pcdata->account, FALSE);

      if (in_world(ch) == WORLD_EARTH)
      real_quit(ch);

      return;
    }
    send_to_char("Syntax: Stasis (character name).\n\r", ch);
  }

  bool isInvalid(const void *address) {
    if (address == nullptr)
    return true;

    if (address == NULL)
    return true;

    unsigned char result;

    if (mincore((void *)address, 1, &result) == -1) {
      if (errno == ENOMEM) {
        return true; // The address is not accessible
      }
      return true;
      // Other error occurred
      // You can handle or report the error as per your needs
    }

    return false; // The address is accessible
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif
