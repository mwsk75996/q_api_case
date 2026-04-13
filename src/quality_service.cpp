#include "quality_service.h"
#include <algorithm>
#include <cctype>
#include <numeric>

static std::string trim(const std::string& input) {
    size_t start = input.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = input.find_last_not_of(" \t\r\n");
    return input.substr(start, end - start + 1);
}

std::string QualityService::calculateGrade(int score) const {
    if (score < 0 || score > 100) return "Ugyldig";
    if (score >= 90) return "A";
    if (score >= 80) return "B";
    if (score >= 70) return "C";
    if (score > 60) return "D";
    return "F";
}

int QualityService::calculateDiscount(const DiscountRequest& request) const {
    if (request.amount < 0) return -1;
    if (request.hourOfDay < 0 || request.hourOfDay > 23) return -1;

    int discount = 0;
    if (request.amount > 100) discount = 10;
    if (request.amount >= 500) discount = 20;
    if (request.amount >= 1000) discount = 30;

    if (request.loyalCustomer) discount += 5;
    if (request.couponCode == "SAVE10") discount += 10;
    if (request.productionMode && request.hourOfDay >= 22) discount += 5;

    if (discount > 35) discount = 35;
    return discount;
}

bool QualityService::canBookSeats(const BookingRequest& request) const {
    if (request.maintenanceMode && !request.hasSafetyOverride) return false;
    if (request.requestedSeats < 1) return false;
    if (request.requestedSeats <= 6) return true;
    if (request.hasSafetyOverride && request.currentReservations < 100) return true;
    return false;
}

std::string QualityService::formatUsername(const std::string& name) const {
    if (name.empty()) return "anonymous";
    std::string value = trim(name);
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return value;
}

double QualityService::calculateSensorAverage(const std::vector<int>& values) const {
    if (values.empty()) return 0.0;
    int sum = std::accumulate(values.begin(), values.end(), 0);
    return static_cast<double>(sum / values.size());
}

std::string QualityService::evaluateSensorHealth(const std::vector<int>& values) const {
    if (values.empty()) return "NO_DATA";

    int minValue = *std::min_element(values.begin(), values.end());
    int maxValue = *std::max_element(values.begin(), values.end());
    double avg = calculateSensorAverage(values);

    if (minValue < 0 || maxValue > 100) return "ERROR";
    if ((maxValue - minValue) > 40) return "UNSTABLE";
    if (avg < 20) return "WARNING";
    return "OK";
}
