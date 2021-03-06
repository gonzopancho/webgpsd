// Copyright 2008, Tom Zerucha, tz@execpc.com, 
// released under GNU General Public License version 3.

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/termios.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/un.h>

#define BUFLEN (16384)
#define MAXCONN (32)

// data billboard
struct satinfo {
  // satellite number, elevation, azmuith, and signal
  // satellite number is NEGATIVE if used
    short num, el, az, sn;
};

struct gpsstate {
  // fd - connection -1 if device not connected, last ms seen
    int gpsfd, lastseen;
  // latitude, longitude in micro-degrees.  Altitude in feet * 1000
    int llat, llon, alt;
  // dilution of precision * 1000
    int pdop, hdop, vdop;
  // speed, mph * 1000, track, degrees * 1000
    int gspd, gtrk;
  // year (in century, 08 not 2008), month, day, hour, minute, second, thousanths
    int yr, mo, dy, hr, mn, sc, scth;
  // lock, 0-1.  fix from GPGSA
    int lock, fix;
  // number of sats visible, number being used in fix
    int nsats, nused;
  // satellite table
    struct satinfo sats[20];
};

struct gpssats {
    int satset, nsats, nused;
    int sats[50], view[100]; // list of used, inview
    int el[100], az[100], sn[100];
};
#if 0
struct harley {
    int rpm, vspd, full, gear, clutch, neutral, engtemp, turnsig, odoaccum, fuelaccum;
    int odolastms, fuellastms;
    int odolastval, fuellastval;
};
#endif

#define MAXSRC (16)
extern int bestgps;
extern struct gpsstate gpst[MAXSRC];
extern struct gpssats gpsat[MAXSRC];

// current ms
extern int thisms;

//commons
extern FILE *errfd;
extern FILE *logfd;
extern char *xbuf;

//configs
extern int kmlinterval;
extern char *zipkml;
extern char *rtname;

//FUNCTIONS
// process data from device
int getgpsinfo(int fd, char *, int);
void addnmeacksum(char *c);
void findbestgps(void);

//web subfunctions
void dowebget(void);

//misc
void rotatekml(void);
