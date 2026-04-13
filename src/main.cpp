#include "httplib.h"
#include "quality_service.h"
#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

static std::string trim(const std::string& value) {
    const size_t start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }

    const size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

static std::string escapeJson(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());

    for (const char ch : value) {
        switch (ch) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += ch; break;
        }
    }

    return escaped;
}

static void setJsonError(httplib::Response& res, int status, const std::string& message) {
    res.status = status;
    res.set_content(std::string("{\"error\":\"") + escapeJson(message) + "\"}", "application/json");
}

static std::string requireParam(const httplib::Request& req, const char* name) {
    if (!req.has_param(name)) {
        throw std::invalid_argument(std::string("Missing required parameter: ") + name);
    }

    const std::string value = trim(req.get_param_value(name));
    if (value.empty()) {
        throw std::invalid_argument(std::string("Parameter must not be empty: ") + name);
    }

    return value;
}

static int parseIntStrict(const std::string& rawValue, const char* name) {
    const std::string value = trim(rawValue);
    size_t parsedLength = 0;

    try {
        const int parsed = std::stoi(value, &parsedLength);
        if (parsedLength != value.size()) {
            throw std::invalid_argument("trailing characters");
        }
        return parsed;
    } catch (const std::exception&) {
        throw std::invalid_argument(std::string("Invalid integer parameter: ") + name);
    }
}

static int requireIntParam(const httplib::Request& req, const char* name) {
    return parseIntStrict(requireParam(req, name), name);
}

static int optionalIntParam(const httplib::Request& req, const char* name, int defaultValue) {
    if (!req.has_param(name)) {
        return defaultValue;
    }

    return parseIntStrict(req.get_param_value(name), name);
}

static bool boolParam(const httplib::Request& req, const char* name) {
    return req.has_param(name) && req.get_param_value(name) == "1";
}

static std::vector<int> parseCsvInts(const std::string& csv, const char* name) {
    const std::string trimmedCsv = trim(csv);
    if (trimmedCsv.empty()) {
        throw std::invalid_argument(std::string("Parameter must not be empty: ") + name);
    }

    std::vector<int> values;
    std::stringstream ss(trimmedCsv);
    std::string item;

    while (std::getline(ss, item, ',')) {
        values.push_back(parseIntStrict(item, name));
    }

    return values;
}

template <typename Handler>
static auto jsonRoute(Handler handler) {
    return [handler](const httplib::Request& req, httplib::Response& res) {
        try {
            handler(req, res);
        } catch (const std::invalid_argument& ex) {
            setJsonError(res, 400, ex.what());
        } catch (const std::exception& ex) {
            setJsonError(res, 500, ex.what());
        } catch (...) {
            setJsonError(res, 500, "Unknown server error");
        }
    };
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

    svr.Get("/api/grade", jsonRoute([&](const httplib::Request& req, httplib::Response& res) {
        const int score = requireIntParam(req, "score");
        res.set_content(
            std::string("{\"result\":\"") + escapeJson(service.calculateGrade(score)) + "\"}",
            "application/json"
        );
    }));

    svr.Get("/api/discount", jsonRoute([&](const httplib::Request& req, httplib::Response& res) {
        DiscountRequest request;
        request.amount = requireIntParam(req, "amount");
        request.loyalCustomer = boolParam(req, "loyal");
        request.couponCode = req.has_param("coupon") ? req.get_param_value("coupon") : "";
        request.productionMode = boolParam(req, "production");
        request.hourOfDay = optionalIntParam(req, "hour", 12);
        res.set_content(std::string("{\"result\":") + std::to_string(service.calculateDiscount(request)) + "}", "application/json");
    }));

    svr.Get("/api/booking", jsonRoute([&](const httplib::Request& req, httplib::Response& res) {
        BookingRequest request;
        request.requestedSeats = requireIntParam(req, "seats");
        request.hasSafetyOverride = boolParam(req, "override");
        request.currentReservations = optionalIntParam(req, "reserved", 0);
        request.maintenanceMode = boolParam(req, "maintenance");
        std::string ok = service.canBookSeats(request) ? "true" : "false";
        res.set_content(std::string("{\"result\":") + ok + "}", "application/json");
    }));

    svr.Get("/api/username", jsonRoute([&](const httplib::Request& req, httplib::Response& res) {
        const std::string name = req.has_param("name") ? req.get_param_value("name") : "";
        res.set_content(
            std::string("{\"result\":\"") + escapeJson(service.formatUsername(name)) + "\"}",
            "application/json"
        );
    }));

    svr.Get("/api/sensor-average", jsonRoute([&](const httplib::Request& req, httplib::Response& res) {
        const std::vector<int> values = parseCsvInts(requireParam(req, "values"), "values");
        std::ostringstream result;
        result << "{\"result\":" << service.calculateSensorAverage(values) << "}";
        res.set_content(result.str(), "application/json");
    }));

    svr.Get("/api/sensor-health", jsonRoute([&](const httplib::Request& req, httplib::Response& res) {
        const std::vector<int> values = parseCsvInts(requireParam(req, "values"), "values");
        res.set_content(
            std::string("{\"result\":\"") + escapeJson(service.evaluateSensorHealth(values)) + "\"}",
            "application/json"
        );
    }));

    std::cout << "Server listening on http://0.0.0.0:8080";
    svr.listen("0.0.0.0", 8080);
}
