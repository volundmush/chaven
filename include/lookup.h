
#ifndef _LOOKUP_H_
#define _LOOKUP_H_

#if defined(__cplusplus)
extern "C" {
#endif

int		rpclass_lookup	args( (const char *name) );
int		climate_lookup	args( (const char *name) );
int		poison_lookup	args( (const char *name) );
int		song_lookup	args( (const char *name) );
int		bonus_lookup	args( (const char *name) );
int		stat_lookup	args( (const char *name) );
int		sstat_lookup	args( (const char *name) );
int		position_lookup	args( (const char *name) );
int 		sex_lookup	args( (const char *name) );
int 		size_lookup	args( (const char *name) );
int		flag_lookup	args( (const char *, const struct flag_type *) );
int 		material_lookup args( (const char *name) );
int		trap_lookup	args( (const char *name) );
int		dstatus_lookup	args( (const char *name) );
int 		weapon_lookup 	args( (const char *name) );
int 		attack_lookup  	args( (const char *name) );
long 		wiznet_lookup 	args( (const char *name) );
int 		class_lookup 	args( (const char *name) );
int		bposition_lookup args( (const char *name) );
int		coin_lookup	args( (const char *name) );
char 		*flow_lookup 	args( ( int flow ) );
int 		flow_lookup_int	args( ( const char *name) );
HELP_DATA 	*help_lookup	args( (char *) );
HELP_AREA 	*had_lookup	args( (char *) );
int		vessel_brand_lookup	args( ( char *keyword ) );
int		hfile_type_lookup	args( ( char *keyword ) );
bool		did_quest	args( ( CHAR_DATA *ch, int quest_value ) );


// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif
