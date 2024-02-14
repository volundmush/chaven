
#ifndef _RECYCLE_H_
#define _RECYCLE_H_

#if defined(__cplusplus)
extern "C" {
#endif

/* externs */
extern char str_empty[1];
extern int mobile_count;

/* stuff for providing a crash-proof buffer */

#define MAX_BUF		16384
#define MAX_BUF_LIST 	10
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2

/* ban data recycling */
#define BD BAN_DATA
BD	*new_ban args( (void) );
void	free_ban args( (BAN_DATA *ban) );
#undef BD

#define BD BET_DATA
BD	*new_bet args( (void) );
void	free_bet args( (BET_DATA *ban) );
#undef BD

/* descriptor recycling */
#define DD DESCRIPTOR_DATA
DD	*new_descriptor args( (void) );
void	free_descriptor args( (DESCRIPTOR_DATA *d) );
#undef DD

/* extra descr recycling */
#define ED EXTRA_DESCR_DATA
ED	*new_extra_descr args( (void) );
void	free_extra_descr args( (EXTRA_DESCR_DATA *ed) );
#undef ED

/* affect recycling */
#define AD AFFECT_DATA
AD	*new_affect args( (void) );
void	free_affect args( (AFFECT_DATA *af) );
#undef AD

/* object recycling */
#define OD OBJ_DATA
OD	*new_obj args( (void) );
void	free_obj args( (OBJ_DATA *obj) );
#undef OD

/* character recyling */
#define CD CHAR_DATA
#define PD PC_DATA
CD	*new_char args( (void) );
void	free_char args( (CHAR_DATA *ch) );
PD	*new_pcdata args( (void) );
void	free_pcdata args( (PC_DATA *pcdata) );
#undef PD
#undef CD

/* mob meory recycling */
#define MD MEM_DATA
MD      *new_memory args( ( void ) );
void    free_memory args( (MEM_DATA *mem) );
#undef MD

/* memorize data recycling */
#define MD MEMORIZE_DATA
MD	*new_memorize args( ( void ) );
void 	free_memorize args( ( MEMORIZE_DATA *mem ) );
#undef MD

/* Command recycling */
CMD_TYPE *new_command args( (void) );
void free_command args( (CMD_TYPE *cmd) );


/* mob id  procedures */
long 	get_pc_id args( (void) );
long	get_mob_id args( (void) );

HELP_AREA *	new_had		args( ( void ) );
HELP_DATA *	new_help	args( ( void ) );
void		free_help	args( ( HELP_DATA * ) );

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif
