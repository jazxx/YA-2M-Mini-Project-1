#ifndef AIRPORT_H
#define AIRPORT_H
#include <string>

class Airport {
public:
    Airport() = default;
    Airport(std::string iataCode,
            std::string name,
            std::string city,
            std::string country,
            std::string icaoCode,
            double latitude,
            double longitude);

    const std::string& getIataCode() const;
    const std::string& getName() const;
    const std::string& getCity() const;
    const std::string& getCountry() const;
    const std::string& getIcaoCode() const;
    double getLatitude() const;
    double getLongitude() const;

    std::string toString() const;

private:
    std::string iataCode_;
    std::string name_;
    std::string city_;
    std::string country_;
    std::string icaoCode_;
    double latitude_ = 0.0;
    double longitude_ = 0.0;
};

#endif
