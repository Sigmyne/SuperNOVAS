/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"



int main() {
  TestUtil test = TestUtil("Weather");

  int n = 0;

  if(!test.check("is_valid(T=NAN)", !Weather(NAN, 1e5, 50.0).is_valid())) n++;
  if(!test.check("is_valid(p=NAN)", !Weather(0.0, NAN, 50.0).is_valid())) n++;
  if(!test.check("is_valid(h=NAN)", !Weather(0.0, 1e5, NAN).is_valid())) n++;

  Weather a = Weather(45.0, 1.0 * Unit::atm, 30.0);
  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("temperature()", a.temperature().celsius(), 45.0, 1e-15)) n++;
  if(!test.equals("pressure()", a.pressure().atm(), 1.0, 1e-15)) n++;
  if(!test.equals("humidity()", a.humidity(), 30.0, 1e-15)) n++;
  if(!test.equals("humidity()", a.humidity_fraction(), 0.3, 1e-15)) n++;
  if(!test.equals("to_string()", a.to_string(), "Weather (T = " + a.temperature().to_string() +
          ", p = " + a.pressure().to_string() + ", h = 30.0 %)")) n++;

  Weather b = Weather(Temperature::celsius(45.0), Pressure::atm(1.0), 30.0);
  if(!test.equals("temperature() ==", b.temperature().celsius(), a.temperature().celsius(), 1e-15)) n++;
  if(!test.equals("pressure() ==", b.pressure().atm(), a.pressure().atm(), 1e-15)) n++;
  if(!test.equals("humidity() ==", b.humidity(), a.humidity(), 1e-15)) n++;
  if(!test.equals("humidity_fraction() ==", b.humidity_fraction(), a.humidity_fraction(), 1e-15)) n++;

  Site site = Site(45.0 * Unit::deg, 30.0 * Unit::deg, 1500.0 * Unit::m);
  Weather c = Weather::guess(site);

  novas::on_surface s = {};
  novas::make_itrf_site(30.0, 45.0, 1500.0, &s);
  novas::novas_set_default_weather(&s);

  if(!test.equals("temperature(site)", c.temperature().celsius(), s.temperature, 1e-15)) n++;
  if(!test.equals("pressure(site)", c.pressure().mbar(), s.pressure, 1e-15)) n++;
  if(!test.equals("humidity(site)", c.humidity(), s.humidity, 1e-15)) n++;

  std::cout << "Weather.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
