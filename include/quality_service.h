#pragma once
#include <string>
#include <vector>

struct DiscountRequest {
    int amount;
    bool loyalCustomer;
    std::string couponCode;
    bool productionMode;
    int hourOfDay;
};

struct BookingRequest {
    int requestedSeats;
    bool hasSafetyOverride;
    int currentReservations;
    bool maintenanceMode;
};

class QualityService {
public:
    std::string calculateGrade(int score) const;
    int calculateDiscount(const DiscountRequest& request) const;
    bool canBookSeats(const BookingRequest& request) const;
    std::string formatUsername(const std::string& name) const;
    double calculateSensorAverage(const std::vector<int>& values) const;
    std::string evaluateSensorHealth(const std::vector<int>& values) const;
};
