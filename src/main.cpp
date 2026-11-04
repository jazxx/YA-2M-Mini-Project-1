#include "Airport.h"
#include "AirportBST.h"
#include <algorithm>
#include <chrono>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace {

AirportBST tree;
std::vector<Airport> sourceAirports;
std::vector<Airport> catalog;

struct ExperimentResult {
    std::string name;
    int height;
    double averageDepth;
    double searchMilliseconds;
};

std::string trim(const std::string& text) {
    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string uppercase(std::string text) {
    const int n = static_cast<int>(text.size());
    for (int i = 0; i < n; i++) {
        text[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(text[i])));
    }
    return text;
}

std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool insideQuotes = false;

    const int n = static_cast<int>(line.size());
    for (int i = 0; i < n; i++) {
        const char character = line[i];
        if (character == '"') {
            insideQuotes = !insideQuotes;
        } else if (character == ',' && !insideQuotes) {
            fields.push_back(field);
            field.clear();
        } else if (character != '\r') {
            field.push_back(character);
        }
    }
    fields.push_back(field);
    return fields;
}

bool looksLikeAirportData(const std::string& fileName) {
    std::ifstream input(fileName);
    std::string firstLine;
    return input && std::getline(input, firstLine) && firstLine.find(',') != std::string::npos;
}

const Airport* findInCatalog(const std::string& code) {
    const int n = static_cast<int>(catalog.size());
    for (int i = 0; i < n; i++) {
        if (catalog[i].getIataCode() == code) {
            return &catalog[i];
        }
    }
    return nullptr;
}

void printStats() {
    std::cout << "Nodes: " << tree.size()
              << "  Height: " << tree.height()
              << "  Average depth: " << std::fixed << std::setprecision(3)
              << tree.averageDepth() << std::defaultfloat << '\n';
}

void printPath(const SearchResult& result) {
    std::cout << "Path: ";
    const int n = static_cast<int>(result.path.size());
    for (int i = 0; i < n; i++) {
        if (i > 0) {
            std::cout << " -> ";
        }
        std::cout << result.path[i];
    }
    if (!result.found()) {
        if (!result.path.empty()) {
            std::cout << " -> ";
        }
        std::cout << "(NULL)";
    }
    std::cout << "  (comparisons: " << result.comparisons() << ")\n";
}

void printLevels(int count) {
    const std::vector<std::vector<std::string>> levels = tree.levelOrder(count);
    if (levels.empty()) {
        std::cout << "Tree is empty.\n";
        return;
    }

    const int n = static_cast<int>(levels.size());
    for (int i = 0; i < n; i++) {
        std::cout << "Level " << i << ": ";
        const int levelSize = static_cast<int>(levels[i].size());
        for (int j = 0; j < levelSize; j++) {
            if (j > 0) {
                std::cout << ' ';
            }
            std::cout << levels[i][j];
        }
        std::cout << '\n';
    }
}

bool loadData(const std::string& fileName) {
    std::ifstream input(fileName);
    if (!input) {
        std::cout << "Unable to open " << fileName << '\n';
        return false;
    }

    tree.clear();
    sourceAirports.clear();
    catalog.clear();

    int totalLines = 0;
    int inserted = 0;
    int skipped = 0;
    int malformed = 0;
    int duplicates = 0;
    std::string line;

    while (std::getline(input, line)) {
        totalLines++;
        const std::vector<std::string> fields = parseCsvLine(line);
        if (fields.size() != 14) {
            malformed++;
            continue;
        }

        const std::string code = uppercase(trim(fields[4]));
        if (code == "\\N" || code.empty()) {
            skipped++;
            continue;
        }

        try {
            Airport airport(code,
                            fields[1],
                            fields[2],
                            fields[3],
                            fields[5],
                            std::stod(fields[6]),
                            std::stod(fields[7]));

            if (!tree.insert(airport)) {
                duplicates++;
                std::cout << "Duplicate code on line " << totalLines << ": " << code << '\n';
                continue;
            }

            sourceAirports.push_back(airport);
            catalog.push_back(airport);
            inserted++;
        } catch (const std::exception&) {
            malformed++;
        }
    }

    std::cout << "Loaded " << inserted << " airports ("
              << skipped << " lines skipped: no IATA code). "
              << "Duplicates: " << duplicates;
    if (malformed > 0) {
        std::cout << "  Malformed: " << malformed;
    }
    std::cout << '\n';
    printStats();
    std::cout << "Validate: " << (tree.validate() ? "OK" : "FAILED") << '\n';
    return inserted > 0;
}

bool loadTree(const std::string& fileName) {
    if (catalog.empty()) {
        std::cout << "Load airports.dat first so complete records are available.\n";
        return false;
    }

    std::ifstream input(fileName);
    if (!input) {
        std::cout << "Unable to open " << fileName << '\n';
        return false;
    }

    std::vector<std::string> codes;
    std::string line;
    while (std::getline(input, line)) {
        const std::string code = uppercase(trim(line));
        if (code.empty()) {
            continue;
        }
        if (findInCatalog(code) == nullptr) {
            std::cout << "Cannot reload unknown code: " << code << '\n';
            return false;
        }
        codes.push_back(code);
    }

    tree.clear();
    int duplicates = 0;
    const int n = static_cast<int>(codes.size());
    for (int i = 0; i < n; i++) {
        const Airport* airport = findInCatalog(codes[i]);
        if (airport != nullptr && !tree.insert(*airport)) {
            duplicates++;
        }
    }

    std::cout << "Reloaded " << tree.size() << " airports from preorder. "
              << "Duplicates: " << duplicates << '\n';
    printStats();
    std::cout << "Top three levels after reload:\n";
    printLevels(3);
    std::cout << "Validate: " << (tree.validate() ? "OK" : "FAILED") << '\n';
    return tree.size() > 0;
}

void findAirport(const std::string& code) {
    if (code.empty()) {
        std::cout << "Usage: find CODE\n";
        return;
    }

    const SearchResult result = tree.find(code);
    printPath(result);
    if (result.found()) {
        std::cout << result.airport->toString() << '\n';
    } else {
        std::cout << code << " not found\n";
    }
}

void addAirport(std::istringstream& input) {
    std::string code;
    std::string name;
    std::string city;
    std::string country;
    std::string icao;
    double latitude = 0.0;
    double longitude = 0.0;

    if (!(input >> code >> std::quoted(name) >> std::quoted(city) >>
          std::quoted(country) >> icao >> latitude >> longitude)) {
        std::cout << "Usage: add CODE \"NAME\" \"CITY\" \"COUNTRY\" ICAO LAT LON\n";
        return;
    }

    code = uppercase(code);
    if (code.size() != 3) {
        std::cout << "IATA code must contain exactly three characters.\n";
        return;
    }

    Airport airport(code, name, city, country, uppercase(icao), latitude, longitude);
    if (!tree.insert(airport)) {
        std::cout << "Duplicate code: " << code << '\n';
        return;
    }

    catalog.push_back(airport);
    std::cout << "Added " << code
              << "  validate: " << (tree.validate() ? "OK" : "FAILED") << '\n';
}

void removeAirport(const std::string& code) {
    if (code.empty()) {
        std::cout << "Usage: remove CODE\n";
        return;
    }

    std::cout << "Top three levels before delete:\n";
    printLevels(3);
    const DeleteResult result = tree.remove(code);
    if (!result.deleted) {
        std::cout << code << " not found; nothing deleted.\n";
        return;
    }

    std::cout << "Deleted " << code << " (case " << result.caseDescription << ")"
              << "  validate: " << (tree.validate() ? "OK" : "FAILED") << '\n';
    std::cout << "Top three levels after delete:\n";
    printLevels(3);
}

void listAirports() {
    const std::vector<Airport> airports = tree.inorder();
    const int n = static_cast<int>(airports.size());
    for (int i = 0; i < n; i++) {
        std::cout << airports[i].toString() << '\n';
    }

    const bool sorted = std::is_sorted(
        airports.begin(), airports.end(),
        [](const Airport& first, const Airport& second) {
            return first.getIataCode() < second.getIataCode();
        });
    std::cout << airports.size() << " airports. Inorder sorted: "
              << (sorted ? "YES" : "NO") << '\n';
}

void showRange(const std::string& from, const std::string& to) {
    if (from.empty() || to.empty() || from > to) {
        std::cout << "Usage: range FROM TO (FROM must not exceed TO)\n";
        return;
    }

    const RangeResult result = tree.range(from, to);
    const int n = static_cast<int>(result.airports.size());
    for (int i = 0; i < n; i++) {
        std::cout << result.airports[i].toString() << '\n';
    }
    std::cout << result.airports.size() << " airports, "
              << result.nodesVisited << " nodes visited\n";
}

void printExtreme(const Airport* airport, const std::string& label) {
    if (airport == nullptr) {
        std::cout << "Tree is empty.\n";
    } else {
        std::cout << "The " << label << " airport is " << airport->toString() << '\n';
    }
}

ExperimentResult measureTree(const std::string& name,
                             const std::vector<Airport>& insertionOrder,
                             const std::vector<std::string>& searchCodes,
                             bool shouldRebalance) {
    AirportBST experimentTree;
    const int airportCount = static_cast<int>(insertionOrder.size());
    for (int i = 0; i < airportCount; i++) {
        experimentTree.insert(insertionOrder[i]);
    }
    if (shouldRebalance) {
        experimentTree.rebalance();
    }

    const auto start = std::chrono::steady_clock::now();
    const int searchCount = static_cast<int>(searchCodes.size());
    for (int i = 0; i < searchCount; i++) {
        experimentTree.find(searchCodes[i]);
    }
    const auto finish = std::chrono::steady_clock::now();

    return {
        name,
        experimentTree.height(),
        experimentTree.averageDepth(),
        std::chrono::duration<double, std::milli>(finish - start).count()
    };
}

void runExperiment() {
    if (sourceAirports.empty()) {
        std::cout << "Load airports.dat before running the experiment.\n";
        return;
    }

    std::vector<Airport> sortedAirports = sourceAirports;
    std::sort(sortedAirports.begin(), sortedAirports.end(),
              [](const Airport& first, const Airport& second) {
                  return first.getIataCode() < second.getIataCode();
              });

    std::mt19937 generator(2560);
    std::uniform_int_distribution<int> pick(0, static_cast<int>(sourceAirports.size()) - 1);
    std::vector<std::string> searchCodes;
    for (int i = 0; i < 10000; i++) {
        searchCodes.push_back(sourceAirports[pick(generator)].getIataCode());
    }

    std::vector<ExperimentResult> results;
    results.push_back(measureTree("File order", sourceAirports, searchCodes, false));
    results.push_back(measureTree("Sorted order", sortedAirports, searchCodes, false));
    results.push_back(measureTree("Balanced", sortedAirports, searchCodes, true));

    std::cout << std::left << std::setw(17) << "Build order"
              << std::right << std::setw(10) << "Height"
              << std::setw(18) << "Average depth"
              << std::setw(20) << "10,000 searches ms" << '\n';

    const int n = static_cast<int>(results.size());
    for (int i = 0; i < n; i++) {
        std::cout << std::left << std::setw(17) << results[i].name
                  << std::right << std::setw(10) << results[i].height
                  << std::setw(18) << std::fixed << std::setprecision(3)
                  << results[i].averageDepth
                  << std::setw(20) << results[i].searchMilliseconds << '\n';
    }
    std::cout << std::defaultfloat;
}

void printHelp() {
    std::cout
        << "Commands:\n"
        << "  load FILE\n"
        << "  find CODE\n"
        << "  add CODE \"NAME\" \"CITY\" \"COUNTRY\" ICAO LAT LON\n"
        << "  remove CODE\n"
        << "  list\n"
        << "  range FROM TO\n"
        << "  min\n"
        << "  max\n"
        << "  save FILE\n"
        << "  validate\n"
        << "  rebalance\n"
        << "  experiment\n"
        << "  help\n"
        << "  quit\n";
}

void runProgram() {
    std::cout << "Airport Code Lookup Service\nType help for commands.\n";
    std::string line;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        std::istringstream input(line);
        std::string command;
        input >> command;
        command = uppercase(command);

        if (command == "QUIT") {
            break;
        } else if (command == "HELP") {
            printHelp();
        } else if (command == "LOAD") {
            std::string fileName;
            input >> fileName;
            if (fileName.empty()) {
                std::cout << "Usage: load FILE\n";
            } else if (looksLikeAirportData(fileName)) {
                loadData(fileName);
            } else {
                loadTree(fileName);
            }
        } else if (command == "FIND") {
            std::string code;
            input >> code;
            findAirport(uppercase(code));
        } else if (command == "ADD") {
            addAirport(input);
        } else if (command == "REMOVE") {
            std::string code;
            input >> code;
            removeAirport(uppercase(code));
        } else if (command == "LIST") {
            listAirports();
        } else if (command == "RANGE") {
            std::string from;
            std::string to;
            input >> from >> to;
            showRange(uppercase(from), uppercase(to));
        } else if (command == "MIN") {
            printExtreme(tree.minimum(), "minimum");
        } else if (command == "MAX") {
            printExtreme(tree.maximum(), "maximum");
        } else if (command == "SAVE") {
            std::string fileName;
            input >> fileName;
            if (fileName.empty()) {
                std::cout << "Usage: save FILE\n";
            } else {
                std::cout << (tree.savePreorder(fileName) ? "Saved " : "Unable to save ")
                          << fileName << '\n';
            }
        } else if (command == "VALIDATE") {
            std::cout << "Validate: " << (tree.validate() ? "OK" : "FAILED") << '\n';
        } else if (command == "REBALANCE") {
            tree.rebalance();
            std::cout << "Tree rebalanced.\n";
            printStats();
            std::cout << "Validate: " << (tree.validate() ? "OK" : "FAILED") << '\n';
        } else if (command == "EXPERIMENT") {
            runExperiment();
        } else if (!command.empty()) {
            std::cout << "Unknown command. Type help.\n";
        }
    }
}

}  // namespace

int main() {
    runProgram();
    return 0;
}
