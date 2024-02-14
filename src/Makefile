CC      = g++-6
PROF    = -O0 -g
NOCRYPT =
C_FLAGS =  -Wall -Wno-deprecated -Wno-write-strings -fno-stack-protector $(PROF) $(NOCRYPT) -I$(CURDIR)/rapidjson
L_FLAGS =  $(PROF) -Wl,--no-as-needed -ldl -rdynamic -fno-stack-protector
LIBS	= -lcrypt -lcurl

O_FILES = abominations.o act_comm.o act_info.o act_move.o act_obj.o act_wiz.o \
	  alias.o attract.o ban.o bit.o bitarray.o \
	  Buffer.o clans.o comm.o creation.o db.o db2.o events.o\
	  flags.o fight.o ghosts.o handler.o \
	  hedit.o houses.o institute.o interp.o lookup.o  map.o mem.o\
	  music.o note_new.o olc.o olc_act.o olc_mpcode.o olc_save.o \
	  patrols.o recycle.o save.o sex.o  skills.o\
	  social-edit.o social.o stories.o shifters.o string.o sys.o system.o tables.o travel.o\
	  update.o world.o\
	  Note.o MudIo.o Util.o\

haven: $(O_FILES)
	rm -f haven
	$(CC) $(L_FLAGS) -o haven $(O_FILES) $(LIBS)
resolve: 
	rm -f resolve
	$(CC) $(L_FLAGS) -o resolve resolve.c $(LIBS)
web:
	rm -f webserver
	rm -f webclient
	$(CC) $(L_FLAGS) -o webserver webserver.c $(LIBS)
	$(CC) $(L_FLAGS) -o webclient webclient.c $(LIBS)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<
.cc.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -rf *.o haven
