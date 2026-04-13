#include "httplib.h"
#include "quality_service.h"
#include <iostream>
#include <sstream>
#include <vector>

static std::vector<int> parseCsvInts(const std::string& csv) {
    std::vector<int> values;
    std::stringstream ss(csv);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) values.push_back(std::stoi(item));
    }
    return values;
}

int main() {
    httplib::Server svr;
    QualityService service;

    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });

    svr.Get("/api/grade", [&](const httplib::Request& req, httplib::Response& res) {
        int score = std::stoi(req.get_param_value("score"));
        res.set_content(std::string("{\"result\":\"") + service.calculateGrade(score) + "\"}", "application/json");
    });

    svr.Get("/api/discount", [&](const httplib::Request& req, httplib::Response& res) {
        DiscountRequest request;
        request.amount = std::stoi(req.get_param_value("amount"));
        request.loyalCustomer = req.has_param("loyal") && req.get_param_value("loyal") == "1";
        request.couponCode = req.has_param("coupon") ? req.get_param_value("coupon") : "";
        request.productionMode = req.has_param("production") && req.get_param_value("production") == "1";
        request.hourOfDay = req.has_param("hour") ? std::stoi(req.get_param_value("hour")) : 12;
        res.set_content(std::string("{\"result\":") + std::to_string(service.calculateDiscount(request)) + "}", "application/json");
    });

    svr.Get("/api/booking", [&](const httplib::Request& req, httplib::Response& res) {
        BookingRequest request;
        request.requestedSeats = std::stoi(req.get_param_value("seats"));
        request.hasSafetyOverride = req.has_param("override") && req.get_param_value("override") == "1";
        request.currentReservations = req.has_param("reserved") ? std::stoi(req.get_param_value("reserved")) : 0;
        request.maintenanceMode = req.has_param("maintenance") && req.get_param_value("maintenance") == "1";
        std::string ok = service.canBookSeats(request) ? "true" : "false";
        res.set_content(std::string("{\"result\":") + ok + "}", "application/json");
    });

    svr.Get("/api/username", [&](const httplib::Request& req, httplib::Response& res) {
        std::string name = req.get_param_value("name");
        res.set_content(std::string("{\"result\":\"") + service.formatUsername(name) + "\"}", "application/json");
    });

    svr.Get("/api/sensor-average", [&](const httplib::Request& req, httplib::Response& res) {
        std::vector<int> values = parseCsvInts(req.get_param_value("values"));
        res.set_content(std::string("{\"result\":") + std::to_string(service.calculateSensorAverage(values)) + "}", "application/json");
    });

    svr.Get("/api/sensor-health", [&](const httplib::Request& req, httplib::Response& res) {
        std::vector<int> values = parseCsvInts(req.get_param_value("values"));
        res.set_content(std::string("{\"result\":\"") + service.evaluateSensorHealth(values) + "\"}", "application/json");
    });

    std::cout << "Server listening on http://0.0.0.0:8080";
    svr.listen("0.0.0.0", 8080);
}
