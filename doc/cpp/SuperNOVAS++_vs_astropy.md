# SuperNOVAS C++ vs. astropy



<table>
<tr>
<th><b>astropy</b></th>
<th><b>supernovas++</b></th>
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

```c++
#include <supernovas.h>

using namespace supernovas;

// IERS Earth Orientation Parameters...
EOP eop = EOP(37, 62.56 * Unit::ms, 
     103.4 * Unit::mas, 396.2 * Unit::mas);

// Define ICRS coordinates
auto source = CatalogEntry("Antares", 
    "16h 29m 24.45970s", "−26d 25m 55.2094s")
  .parallax(5.89 * Unit::mas)
  .proper_motion(-12.11 * Unit::mas / Unit::yr, 
    -23.30 * Unit::mas / Unit::yr)
  .radial_velocity(-3.4 * Unit::km / Unit::s)
  .to_source();

// Observer location
auto obs = Site::from_GPS(50.7374, 7.0982, 60.0)
     .to_observer(eop);


  
// Set time of observation
Time t("2026-03-14T13:43:00.728+0200", eop, 
     NOVAS_TAI);
 

// Observer frame
auto frame = obs.frame_at(t);

// apparent coordinates in system
auto app = source.apparent_in(frame).cirs();
```

</td>
</tr>
</table>



-----------------------------------------------------------------------------
Copyright (C) 2026 Attila Kovács


