#include "analyzer.h"
#include "fstream"
#include <sstream>
#include <algorithm>
#include <string_view>
#include <string>

using std::string_view;


// Students may use ANY data structure internally

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    // TODO:
    // - open file
    std::ifstream file(csvPath);
    if (!file.is_open()) return; //if file is opened use else open csv file


    // - skip header
    //There is no header in csv sample file but documentation says that;"Header row is always present" so we discard the first line
    std::string line;
    if (!std::getline(file, line)) return; // Skip Header line

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty()) continue;

        //Firstly find comma points ;
        size_t p1 = line.find(','); // 0 --> p1 = Id
        if (p1 == std::string::npos) continue; // skip malformed rows

        size_t p2 = line.find(',', p1 + 1); //p1+1 --> p2-1 = pickupZone
        if (p2 == std::string::npos) continue;// skip malformed rows

        size_t p3 = line.find(',', p2 + 1); //p2 + 1 --> p3 + 1 = dropoffZone
        if (p3 == std::string::npos) continue;// skip malformed rows

        size_t p4 = line.find(',', p3 + 1); //p3 + 1 --> p4 -1 = datetimeStr
        if (p4 == std::string::npos) continue; // skip malformed rows

        size_t p5 = line.find(',', p4 + 1); //p4 + 1 --> p5 -1 = distance 
        if (p5 == std::string::npos) continue; // skip malformed rows


        string_view pickupZone(line.data() + p1 + 1, p2 - p1 - 1);
        string_view dropoffZone(line.data() + p2 + 1, p3 - p2 - 1);
        string_view datetimeStr(line.data() + p3 + 1, p4 - p3 - 1);

        // Validasyon
        if (pickupZone.empty() || dropoffZone.empty() ||
            datetimeStr.length() < 16) continue;

        char h1 = datetimeStr[11];
        char h2 = datetimeStr[12];

        if (h1 < '0' || h1 > '9' || h2 < '0' || h2 > '9')
            continue;

        int hour = (h1 - '0') * 10 + (h2 - '0');
        if (hour > 23) continue;

        zoneCounts[std::string(pickupZone)]++;
        slotCounts[std::string(pickupZone)][hour]++;
    }
    file.close();
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    // TODO:
    std::vector<ZoneCount> result;
    for (const auto& pair : zoneCounts) {
        result.push_back({ pair.first, pair.second });
    }

    // - sort by count desc, zone asc
    std::sort(result.begin(), result.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count; // Count DESC
        return a.zone < b.zone; // Zone ASC
        });

    // - return first k
    if (result.size() > (size_t)k) {
        result.resize(k);
    }
    return result;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    // TODO:
    std::vector<SlotCount> result;

    for (const auto& zonePair : slotCounts) {
        for (const auto& hourPair : zonePair.second) {
            result.push_back({ zonePair.first, hourPair.first, hourPair.second });
        }
    }

    // - sort by count desc, zone asc, hour asc
    std::sort(result.begin(), result.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
        });

    // - return first k
    if (result.size() > (size_t)k) {
        result.resize(k);
    }
    return result;
}
