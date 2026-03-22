# SuperNOVAS C99 vs. astropy

Nowadays __astropy__ is widely used in the astronomy community and it is known for its simplicity and elegance, but it 
is rather slow ([putting it mildly](https://github.com/Sigmyne/SuperNOVAS#benchmarks)). In contrast, C is known to 
be fast, but it also has a bad reputation for being tedious and 'ugly'. However, below is a side-by-side comparison of 
equivalent program snippets for calculating CIRS apparent positions in __astropy__ vs. __SuperNOVAS__ for Antares for a 
given date and observer location:

<table>
<tr>
<th><b>astropy</b></th>
<th><b>supernovas (C99)</b></th>
</tr>
<tr>
<td>

```python
from astropy import units as u
from astropy.coordinates import SkyCoord,
   EarthLocation, Longitude, Latitude,
   CIRS
   
   







   
   
# Define ICRS coordinates
source = SkyCoord(
  '16h 29m 24.45970s', '−26d 25m 55.2094s',
  d = u.AU / 5.89 * u.mas,
  pmra = -12.11 * u.mas / u.yr,
  pmdec = -23.30 * u.mas / u.yr,
  rv = -3.4 * u.km / u.s)

# Observer location
loc = EarthLocation.from_geodetic(
  Longitude(50.7374), 
  Latitude(7.0982), 
  height=60.0)

# Set time of observation
time = astropy.time.Time(
  "2025-02-27T19:57:00.728+0200"
  scale='tai')

# Observer frame & system
frame = CIRS(obstime=time, location=loc)


# apparent coordinates
app = source.transform_to(frame); 
```

</td>
<td>

```c
#include <novas.h>

cat_entry star;
object source;
observer loc;
novas_timespec time;
novas_frame frame;
sky_pos app;

// IERS Earth Orientation Parameters...
int leap_seconds = 37;
double dt1 = 0.06256; // [s]
double dx = 103.4;    // [mas]
double dy = 396.2;    // [mas]

// Define ICRS coordinates
make_cat_entry("Antares", "HIP", 80763, 
  novas_hms_hours("16h 29m 24.45970s"), 
  novas_dms_degrees("−26d 25m 55.2094s"),
  -12.11, -23.30, 5.89, -3.4, &star);
  
make_cat_object(&star, &source);

// Observer location
make_gps_observer(50.7374, 7.0982, 60.0,
  &loc);



// Set time of observation
novas_set_str_time(NOVAS_TAI,
  "2025-02-27T19:57:00.728+0200", 
  leap_seconds, dut1, &time);

// Observer frame
novas_make_frame(NOVAS_FULL_ACCURACY, 
  &loc, &time, dx, dy, &frame);

// apparent coordinates in system
novas_sky_pos(&source, &frame, NOVAS_CIRS, &app);
```

</td>
</tr>
</table>

Yes, __SuperNOVAS__ is a bit more verbose, but not painfully so. While __astropy__  will automatically fetch IERS data 
(leap seconds, and Earth-orientation parameters), in __SuperNOVAS__ you will have to set these explicitly. And, of 
course, the above comparison ignores error checking also. In Python, you can simply surround the above code in a `try` 
block, and then catch errors using `except`. In C, there is no catch-all solution like that. Instead, you will have to 
check the return values for each line, and decide if you need to bail early, e.g.:

```c
  if(make_cat_object(&star, &source) != 0) {
    // oops, the above failed, bail if we must...
    return -1;
  }
``` 

Regardless of the slight omissions, the difference is not night and day. If you can handle __astropy__, chances are 
you can handle __SuperNOVAS__ too. And the reward for dealing with slightly 'uglier' code, is orders of magnitude gain 
in the speed of execution. It is a trade-off that worth considering, at the least.

-----------------------------------------------------------------------------
Copyright (C) 2025 Attila Kovács


