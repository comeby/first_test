SHELL = /bin/sh
SYSTEM = $(shell uname)
BINDIR = .
C++ = g++
DEF_FLAGS =
OPT_FLAGS = -O2 -w

ifeq ($(SYSTEM),Darwin)
DEF_FLAGS += -D__APPLE__
OPT_FLAGS += -flat_namespace
LFLAGS = -lpthread -Lzlib/ -lz
CFLAGS =
else
LFLAGS = -pthread -Lzlib/ -lz
CFLAGS = -pthread
endif

ifeq ($(SYSTEM),FreeBSD)
DEF_FLAGS += -DFreeBSD
endif

ifeq ($(SYSTEM),SunOS)
DEF_FLAGS += -DSolaris
LFLAGS = -lresolv -lsocket -lnsl -lpthread -Lzlib/ -lz
CFLAGS =
endif

CFLAGS += $(OPT_FLAGS) -I. $(DEF_FLAGS)
OBJS = atom.o base64.o bencode.o bnbt.o client.o config.o link.o md5.o server.o sha1.o sort.o tracker.o tracker_admin.o tracker_announce.o tracker_comments.o tracker_file.o tracker_index.o tracker_info.o tracker_login.o tracker_scrape.o tracker_signup.o tracker_stats.o tracker_torrent.o tracker_upload.o tracker_users.o util.o
PROG = $(BINDIR)/bnbt
PROGS = $(PROG)

all: $(OBJS) $(PROG)

$(PROG): $(OBJS)
	$(C++) -o $(PROG) $(OBJS) $(LFLAGS)

clean:
	rm -f $(OBJS) $(PROGS)

%.o: %.cpp
	$(C++) -o $@ $(CFLAGS) -c $<

$(PROG): $(OBJS)

all: $(PROGS)

atom.o: bnbt.h atom.h util.h
base64.o: base64.h
bencode.o: bnbt.h atom.h bencode.h util.h
bnbt.o: bnbt.h config.h server.h util.h
client.o: bnbt.h atom.h base64.h client.h md5.h server.h tracker.h util.h
config.o: bnbt.h atom.h config.h tracker.h util.h
link.o: bnbt.h atom.h bencode.h config.h link.h md5.h server.h tracker.h util.h
md5.o: md5.h
server.o: bnbt.h client.h config.h server.h tracker.h util.h
sha1.o: sha1.h
sort.o: bnbt.h sort.h tracker.h util.h
tracker.o: bnbt.h atom.h bencode.h config.h md5.h server.h sort.h tracker.h util.h
tracker_admin.o: bnbt.h atom.h tracker.h server.h util.h
tracker_announce.o: bnbt.h atom.h bencode.h tracker.h util.h
tracker_comments.o: bnbt.h atom.h tracker.h util.h
tracker_file.o: bnbt.h atom.h tracker.h util.h
tracker_index.o: bnbt.h atom.h sort.h tracker.h util.h
tracker_info.o: bnbt.h atom.h tracker.h util.h
tracker_login.o: bnbt.h atom.h tracker.h util.h
tracker_scrape.o: bnbt.h atom.h bencode.h tracker.h util.h
tracker_signup.o: bnbt.h atom.h tracker.h util.h
tracker_stats.o: bnbt.h atom.h bencode.h sort.h tracker.h util.h
tracker_torrent.o: bnbt.h atom.h bencode.h tracker.h util.h
tracker_upload.o: bnbt.h atom.h bencode.h client.h tracker.h util.h
tracker_users.o: bnbt.h atom.h md5.h sort.h tracker.h util.h
util.o: bnbt.h atom.h bencode.h sha1.h tracker.h util.h
