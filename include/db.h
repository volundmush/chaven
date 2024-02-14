
#if defined(__cplusplus)
extern "C" {
#endif

#define                 MAX_PERM_BLOCK  131072 //This used to be 131072 - Discordance

#define KEYS( literal, field, value ) \

/* vals from db.c */
extern bool fBootDb;
extern int		newmobs;
extern int		newobjs;
extern MOB_INDEX_DATA 	* mob_index_hash          [MAX_KEY_HASH];
extern OBJ_INDEX_DATA 	* obj_index_hash          [MAX_KEY_HASH];
extern int		top_mob_index;
extern int		top_obj_index;
extern int  		top_affect;
extern int		top_ed; 
extern AreaList		area_list;
extern int		top_group_index;

/* from db2.c */
//extern int	social_count;

/* conversion from db.h */
void	convert_mob(MOB_INDEX_DATA *mob);
void	convert_obj(OBJ_INDEX_DATA *obj);

/* macro for flag swapping */
#define GET_UNSET(flag1,flag2)	(~(flag1)&((flag1)|(flag2)))

/* Magic number for memory allocation */
#define MAGIC_NUM 52571214

/* func from db.c */
extern void assign_area_vnum( int vnum );                    /* OLC */

/* from db2.c */
 
void convert_mobile( MOB_INDEX_DATA *pMobIndex );            /* OLC ROM */
void convert_objects( void );                                /* OLC ROM */
void convert_object( OBJ_INDEX_DATA *pObjIndex );            /* OLC ROM */


#define DATPATH ""
extern unsigned strspace_size;
char* strspace_alloc();
void strspace_size_increment();
extern int	nAllocString;
extern int	sAllocString;
//extern int	social_count;

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif
