# SuperNOVAS C++ vs. C99



<table>
<tr>
<th><b>C99</b></th>
<th><b>C++11</b></th>
</tr>
<tr>
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
<td>

```c++
#include <supernovas.h>

using namespace supernovas;








// IERS Earth Orientation Parameters...
EOP eop = EOP(37, 0.06256, 
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
auto app = source.apparent_in(frame).to_cirs();
```

</td>
</tr>
</table>



-----------------------------------------------------------------------------
Copyright (C) 2026 Attila Kovács


