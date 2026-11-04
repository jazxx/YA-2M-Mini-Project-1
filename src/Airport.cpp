#include "Airport.h"

#include <iomanip>
#include <sstream>
#include <utility>

Airport::Airport(std::string iataCode,
                 std::string name,
                 std::string city,
                 std::string country,
                 std::string icaoCode,
                 double latitude,
                 double longitude)
    : iataCode_(std::move(iataCode)),
      name_(std::move(name)),
      city_(std::move(city)),
      country_(std::move(country)),
      icaoCode_(std::move(icaoCode)),
      latitude_(latitude),
      longitude_(longitude) {}

const std::string& Airport::getIataCode() const { return iataCode_; }
const std::string& Airport::getName() const { return name_; }
const std::string& Airport::getCity() const { return city_; }
const std::string& Airport::getCountry() const { return country_; }
const std::string& Airport::getIcaoCode() const { return icaoCode_; }
double Airport::getLatitude() const { return latitude_; }
double Airport::getLongitude() const { return longitude_; }

std::string Airport::toString() const {
    std::ostringstream output;
    output << iataCode_ << "  " << name_ << ", " << city_ << ", " << country_
           << "  [ICAO: " << icaoCode_ << ", location: "
           << std::fixed << std::setprecision(6) << latitude_ << ", " << longitude_ << "]";
    return output.str();
}
