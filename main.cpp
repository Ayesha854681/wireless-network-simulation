/*
 * WIRELESS NETWORK SIMULATOR
 * Demonstrates:   Arrays, Quick Sort, and BFS Graph Traversal
 */

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

// ============================================================================
// CONFIGURATION
// ============================================================================

namespace Config {
    constexpr int MAX_NODES = 250;
    constexpr int MAX_NEIGHBORS = 6;
    constexpr double TRANSMISSION_RANGE = 1.5;
    constexpr int PATH_DISPLAY_LIMIT = 12;
    constexpr int SCRIPT_LAUNCH_DELAY_MS = 300;
}

// ============================================================================
// CONSOLE UI
// ============================================================================

namespace Console {
    enum Color {
        BLACK = 0, DARK_BLUE = 1, DARK_GREEN = 2, DARK_CYAN = 3,
        DARK_RED = 4, DARK_MAGENTA = 5, DARK_YELLOW = 6, GRAY = 7,
        DARK_GRAY = 8, BLUE = 9, GREEN = 10, CYAN = 11,
        RED = 12, MAGENTA = 13, YELLOW = 14, WHITE = 15
    };

    void setColor(int textColor, int bgColor = BLACK) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor + bgColor * 16);
    }

    void resetColor() { setColor(WHITE); }
    void clearScreen() { system("cls"); }

    void printLine(char c = '-', int length = 50) {
        cout << string(length, c) << "\n";
    }

    void printCentered(const string& text, int width = 50) {
        int padding = (width - static_cast<int>(text.length())) / 2;
        if (padding > 0) cout << string(padding, ' ');
        cout << text << "\n";
    }

    void printTitle(const string& title) {
        cout << "\n";
        setColor(CYAN);
        printLine('=', 60);
        printCentered(title, 60);
        printLine('=', 60);
        resetColor();
        cout << "\n";
    }

    void printSubTitle(const string& subtitle) {
        cout << "\n";
        setColor(YELLOW);
        cout << "  >> " << subtitle << "\n";
        resetColor();
        setColor(DARK_GRAY);
        cout << "  ";
        printLine('-', 56);
        resetColor();
    }

    void printSuccess(const string& msg) {
        setColor(GREEN);
        cout << "  [OK] " << msg << "\n";
        resetColor();
    }

    void printError(const string& msg) {
        setColor(RED);
        cout << "  [!  ] " << msg << "\n";
        resetColor();
    }

    void printInfo(const string& msg) {
        setColor(CYAN);
        cout << "  [i] " << msg << "\n";
        resetColor();
    }

    void printWarning(const string& msg) {
        setColor(YELLOW);
        cout << "  [!  ] " << msg << "\n";
        resetColor();
    }

    void printStep(int step, const string& msg) {
        setColor(DARK_GRAY);
        cout << "  [";
        setColor(CYAN);
        cout << step;
        setColor(DARK_GRAY);
        cout << "] ";
        setColor(WHITE);
        cout << msg << "\n";
        resetColor();
    }

    void printDetail(const string& label, const string& value, int indent = 6) {
        cout << string(indent, ' ');
        setColor(DARK_GRAY);
        cout << label << ": ";
        setColor(WHITE);
        cout << value << "\n";
        resetColor();
    }

    void printDetailHighlight(const string& label, const string& value, int color, int indent = 6) {
        cout << string(indent, ' ');
        setColor(DARK_GRAY);
        cout << label << ": ";
        setColor(color);
        cout << value << "\n";
        resetColor();
    }

    void printMenuItem(int num, const string& text, const string& desc = "") {
        setColor(CYAN);
        cout << "      [";
        setColor(WHITE);
        cout << num;
        setColor(CYAN);
        cout << "] ";
        setColor(WHITE);
        cout << text;
        if (!desc.empty()) {
            setColor(DARK_GRAY);
            cout << "  -  " << desc;
        }
        resetColor();
        cout << "\n";
    }

    void printMenuItemExit() {
        cout << "\n";
        setColor(DARK_GRAY);
        cout << "      [";
        setColor(GRAY);
        cout << "0";
        setColor(DARK_GRAY);
        cout << "] ";
        setColor(GRAY);
        cout << "Exit\n";
        resetColor();
    }

    void printPrompt(const string& prompt = "Choice") {
        cout << "\n";
        setColor(CYAN);
        cout << "  " << prompt << ": ";
        setColor(WHITE);
    }

    bool askYesNo(const string& question) {
        while (true) {
            setColor(YELLOW);
            cout << "\n  " << question << " ";
            setColor(DARK_GRAY);
            cout << "(y/n): ";
            setColor(WHITE);

            string resp;
            if (!getline(cin, resp) || resp.empty()) continue;

            char c = static_cast<char>(tolower(resp[0]));
            resetColor();
            if (c == 'y') return true;
            if (c == 'n') return false;
        }
    }

    bool getInt(int& val, int minVal, int maxVal) {
        string input;
        if (!getline(cin, input)) return false;
        try {
            val = stoi(input);
            return val >= minVal && val <= maxVal;
        }
        catch (...) {
            return false;
        }
    }

    void waitForEnter() {
        cout << "\n";
        setColor(DARK_GRAY);
        cout << "  Press ENTER to continue... ";
        resetColor();
        string dummy;
        getline(cin, dummy);
    }

    void printTableHeader(const vector<pair<string, int>>& columns) {
        setColor(CYAN);
        cout << "  ";
        for (const auto& col : columns) {
            cout << left << setw(col.second) << col.first;
        }
        cout << "\n  ";
        int totalWidth = 0;
        for (const auto& col : columns) totalWidth += col.second;
        printLine('-', totalWidth);
        resetColor();
    }

    void printTableRow(const vector<string>& values, const vector<int>& widths, bool highlight = false) {
        if (highlight) setColor(GREEN);
        else setColor(WHITE);
        cout << "  ";
        for (size_t i = 0; i < values.size() && i < widths.size(); i++) {
            cout << left << setw(widths[i]) << values[i];
        }
        cout << "\n";
        resetColor();
    }

    void printProgress(const string& task) {
        setColor(DARK_GRAY);
        cout << "      " << task << "...  ";
        resetColor();
    }

    void printDone() {
        setColor(GREEN);
        cout << "OK\n";
        resetColor();
    }

    void printFailed() {
        setColor(RED);
        cout << "FAILED\n";
        resetColor();
    }

    void printValue(const string& value) {
        setColor(WHITE);
        cout << value << "\n";
        resetColor();
    }

    void printSeparator() {
        cout << "\n";
        setColor(DARK_GRAY);
        cout << "  ";
        printLine('. ', 56);
        resetColor();
    }

    void printBoxStart(const string& title) {
        cout << "\n";
        setColor(DARK_GRAY);
        cout << "  +" << string(56, '-') << "+\n";
        cout << "  | ";
        setColor(WHITE);
        cout << left << setw(55) << title;
        setColor(DARK_GRAY);
        cout << "|\n";
        cout << "  +" << string(56, '-') << "+\n";
        resetColor();
    }

    void printBoxRow(const string& label, const string& value) {
        setColor(DARK_GRAY);
        cout << "  | ";
        setColor(GRAY);
        cout << left << setw(20) << label;
        setColor(WHITE);
        cout << left << setw(35) << value;
        setColor(DARK_GRAY);
        cout << "|\n";
        resetColor();
    }

    void printBoxEnd() {
        setColor(DARK_GRAY);
        cout << "  +" << string(56, '-') << "+\n";
        resetColor();
    }
}

using namespace Console;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct Node {
    int id;
    double x, y;
    Node() : id(0), x(0.0), y(0.0) {}
    Node(int id, double x, double y) : id(id), x(x), y(y) {}
};

struct Neighbor {
    int id;
    double distance;
    Neighbor(int id, double dist) : id(id), distance(dist) {}
    bool operator<(const Neighbor& other) const { return distance < other.distance; }
};

struct PathResult {
    bool found;
    vector<int> path;
    int hops;
    float distance;
    PathResult() : found(false), hops(0), distance(0.0f) {}
};

struct RoutingQuery {
    int source, destination;
    PathResult before, after;
    RoutingQuery(int src = 0, int dest = 0) : source(src), destination(dest) {}
};

struct NetworkStats {
    int nodeCount;
    int edgeCount;
    int maxDegree;
    float avgDegree;
    int isolatedNodes;
    int connectedPairs;
};

// ============================================================================
// NETWORK CLASS
// ============================================================================

class Network {
private:
    int nodeCount;
    double gridSize;
    vector<Node> nodes;
    vector<vector<bool>> adjMatrix;
    vector<vector<int>> adjList;
    int maxDegree;
    float avgDegree;
    int edgeCount;

    double calcDistance(int i, int j) const {
        double dx = nodes[i].x - nodes[j].x;
        double dy = nodes[i].y - nodes[j].y;
        return sqrt(dx * dx + dy * dy);
    }

    void rebuildAdjList() {
        for (int i = 0; i < nodeCount; i++) {
            adjList[i].clear();
            for (int j = 0; j < nodeCount; j++) {
                if (adjMatrix[i][j]) {
                    adjList[i].push_back(j);
                }
            }
        }
    }

public:
    Network(int numNodes, double size)
        : nodeCount(numNodes), gridSize(size), maxDegree(0), avgDegree(0.0f), edgeCount(0) {
        nodes.resize(nodeCount);
        adjMatrix.assign(nodeCount, vector<bool>(nodeCount, false));
        adjList.resize(nodeCount);

        for (int i = 0; i < nodeCount; i++) {
            double x = static_cast<double>(rand()) / RAND_MAX * gridSize;
            double y = static_cast<double>(rand()) / RAND_MAX * gridSize;
            nodes[i] = Node(i, x, y);
        }
    }

    int getNodeCount() const { return nodeCount; }
    double getGridSize() const { return gridSize; }
    int getMaxDegree() const { return maxDegree; }
    float getAvgDegree() const { return avgDegree; }
    int getEdgeCount() const { return edgeCount; }
    const vector<Node>& getNodes() const { return nodes; }
    const vector<vector<bool>>& getAdjMatrix() const { return adjMatrix; }

    NetworkStats getStats() const {
        NetworkStats stats;
        stats.nodeCount = nodeCount;
        stats.edgeCount = edgeCount;
        stats.maxDegree = maxDegree;
        stats.avgDegree = avgDegree;
        stats.isolatedNodes = 0;
        stats.connectedPairs = 0;

        for (int i = 0; i < nodeCount; i++) {
            if (adjList[i].empty()) stats.isolatedNodes++;
        }
        stats.connectedPairs = edgeCount;
        return stats;
    }

    int connect(double range) {
        edgeCount = 0;
        for (int i = 0; i < nodeCount; i++) {
            adjList[i].clear();
            fill(adjMatrix[i].begin(), adjMatrix[i].end(), false);
        }

        for (int i = 0; i < nodeCount; i++) {
            for (int j = i + 1; j < nodeCount; j++) {
                if (calcDistance(i, j) <= range) {
                    adjMatrix[i][j] = adjMatrix[j][i] = true;
                    adjList[i].push_back(j);
                    adjList[j].push_back(i);
                    edgeCount++;
                }
            }
        }
        return edgeCount;
    }

    void calcDegreeStats() {
        maxDegree = 0;
        int total = 0;
        for (int i = 0; i < nodeCount; i++) {
            int deg = static_cast<int>(adjList[i].size());
            total += deg;
            maxDegree = max(maxDegree, deg);
        }
        avgDegree = static_cast<float>(total) / nodeCount;
    }

    int optimize(int maxNeighbors) {
        vector<vector<bool>> newMatrix(nodeCount, vector<bool>(nodeCount, false));
        int removedEdges = 0;
        int keptEdges = 0;

        for (int i = 0; i < nodeCount; i++) {
            vector<Neighbor> neighbors;
            for (int j = 0; j < nodeCount; j++) {
                if (i != j && adjMatrix[i][j]) {
                    neighbors.emplace_back(j, calcDistance(i, j));
                }
            }
            sort(neighbors.begin(), neighbors.end());

            int keep = min(static_cast<int>(neighbors.size()), maxNeighbors);
            for (int k = 0; k < keep; k++) {
                int nid = neighbors[k].id;
                newMatrix[i][nid] = newMatrix[nid][i] = true;
            }
        }

        // Count changes
        for (int i = 0; i < nodeCount; i++) {
            for (int j = i + 1; j < nodeCount; j++) {
                if (adjMatrix[i][j] && !newMatrix[i][j]) removedEdges++;
                if (newMatrix[i][j]) keptEdges++;
            }
        }

        adjMatrix = move(newMatrix);
        edgeCount = keptEdges;
        rebuildAdjList();
        return removedEdges;
    }

    PathResult findPath(int start, int end) {
        PathResult result;
        if (start < 0 || start >= nodeCount || end < 0 || end >= nodeCount) return result;

        if (start == end) {
            result.found = true;
            result.path.push_back(start);
            return result;
        }

        vector<bool> visited(nodeCount, false);
        vector<int> parent(nodeCount, -1);
        vector<int> queue;
        queue.reserve(nodeCount);
        queue.push_back(start);
        visited[start] = true;
        size_t front = 0;

        while (front < queue.size()) {
            int curr = queue[front++];
            for (int next : adjList[curr]) {
                if (!visited[next]) {
                    visited[next] = true;
                    parent[next] = curr;
                    if (next == end) {
                        for (int n = end; n != -1; n = parent[n]) {
                            result.path.push_back(n);
                        }
                        reverse(result.path.begin(), result.path.end());
                        result.found = true;
                        result.hops = static_cast<int>(result.path.size()) - 1;
                        result.distance = calcPathDist(result.path);
                        return result;
                    }
                    queue.push_back(next);
                }
            }
        }
        return result;
    }

    float calcPathDist(const vector<int>& path) const {
        if (path.size() < 2) return 0.0f;
        float total = 0.0f;
        for (size_t i = 0; i < path.size() - 1; i++) {
            total += static_cast<float>(calcDistance(path[i], path[i + 1]));
        }
        return total;
    }

    void copyMatrixTo(vector<vector<bool>>& dest) const { dest = adjMatrix; }

    void getEdgeChanges(const vector<vector<bool>>& before,
        vector<pair<int, int>>& removed,
        vector<pair<int, int>>& added,
        vector<pair<int, int>>& kept) const {
        vector<vector<bool>> after(nodeCount, vector<bool>(nodeCount, false));

        for (int i = 0; i < nodeCount; i++) {
            vector<Neighbor> neighbors;
            for (int j = 0; j < nodeCount; j++) {
                if (i != j && before[i][j]) {
                    neighbors.emplace_back(j, calcDistance(i, j));
                }
            }
            sort(neighbors.begin(), neighbors.end());
            int keep = min(static_cast<int>(neighbors.size()), Config::MAX_NEIGHBORS);
            for (int k = 0; k < keep; k++) {
                int nid = neighbors[k].id;
                after[i][nid] = after[nid][i] = true;
            }
        }

        for (int i = 0; i < nodeCount; i++) {
            for (int j = i + 1; j < nodeCount; j++) {
                bool b = before[i][j], a = after[i][j];
                if (b && !a) removed.emplace_back(i, j);
                else if (!b && a) added.emplace_back(i, j);
                else if (b && a) kept.emplace_back(i, j);
            }
        }
    }
};

// ============================================================================
// PYTHON VISUALIZER
// ============================================================================

#include "PythonVisualizer.h"

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

namespace Utils {
    string formatPath(const vector<int>& path, int maxShow = Config::PATH_DISPLAY_LIMIT) {
        if (path.empty()) return "-";
        stringstream ss;
        int limit = min(static_cast<int>(path.size()), maxShow);
        for (int i = 0; i < limit; i++) {
            if (i > 0) ss << "->";
            ss << path[i];
        }
        if (static_cast<int>(path.size()) > maxShow) ss << "->...";
        return ss.str();
    }

    string formatNum(double val, int dec) {
        ostringstream ss;
        ss << fixed << setprecision(dec) << val;
        return ss.str();
    }

    string formatPercent(double val) {
        ostringstream ss;
        ss << fixed << setprecision(1) << (val * 100) << "%";
        return ss.str();
    }
}

BOOL WINAPI consoleHandler(DWORD event) {
    if (event == CTRL_C_EVENT || event == CTRL_CLOSE_EVENT) {
        resetColor();
        printInfo("Shutting down...");
    }
    return FALSE;
}

// ============================================================================
// EXPERIMENTS
// ============================================================================

void runExperiment1() {
    clearScreen();
    printTitle("EXPERIMENT 1:  TOPOLOGY CONTROL");

    // Configuration display
    printBoxStart("Configuration");
    printBoxRow("Algorithm", "Quick Sort");
    printBoxRow("Max Neighbors", to_string(Config::MAX_NEIGHBORS));
    printBoxRow("Transmission Range", Utils::formatNum(Config::TRANSMISSION_RANGE, 1));
    printBoxRow("Network Count", "10");
    printBoxRow("Node Range", "100 - 190");
    printBoxEnd();

    printSubTitle("Phase 1: Network Generation");

    vector<unique_ptr<Network>> networks;

    printStep(1, "Allocating networks");
    for (int i = 0; i < 10; i++) {
        networks.push_back(make_unique<Network>(100 + i * 10, 5.0));
    }
    printDetail("Networks created", "10");
    printDetail("Grid size", "5. 0 x 5.0");

    printStep(2, "Establishing connections");
    int totalEdges = 0;
    for (int i = 0; i < 10; i++) {
        int edges = networks[i]->connect(Config::TRANSMISSION_RANGE);
        totalEdges += edges;
    }
    printDetail("Total edges created", to_string(totalEdges));

    printStep(3, "Calculating statistics");
    for (int i = 0; i < 10; i++) {
        networks[i]->calcDegreeStats();
    }

    printSubTitle("Initial Network Statistics");

    printTableHeader({ {"Nodes", 10}, {"Edges", 10}, {"Max Deg", 12}, {"Avg Deg", 12} });
    vector<int> widths = { 10, 10, 12, 12 };

    for (int i = 0; i < 10; i++) {
        NetworkStats stats = networks[i]->getStats();
        printTableRow({
            to_string(stats.nodeCount),
            to_string(stats.edgeCount),
            to_string(stats.maxDegree),
            Utils::formatNum(stats.avgDegree, 2)
            }, widths);
    }

    if (askYesNo("Generate connection animation?")) {
        printProgress("Creating visualization network");
        Network viz(100, 5.0);
        int edges = viz.connect(Config::TRANSMISSION_RANGE);
        printDone();

        printDetail("Nodes", "100");
        printDetail("Edges", to_string(edges));

        printProgress("Generating Python script");
        PythonVisualizer::generateConnectionAnimation(viz, "viz_initial.py", "Initial Network");
        printDone();

        printSuccess("Opened viz_initial.py");
    }

    if (askYesNo("Apply topology optimization?")) {
        printSubTitle("Phase 2: Topology Optimization");

        printStep(1, "Sorting neighbors by distance");
        printDetail("Algorithm", "Quick Sort (std::sort)");
        printDetail("Criteria", "Euclidean distance");

        printStep(2, "Pruning excess connections");
        printDetail("Keep per node", to_string(Config::MAX_NEIGHBORS) + " closest");

        int totalRemoved = 0;
        for (int i = 0; i < 10; i++) {
            int removed = networks[i]->optimize(Config::MAX_NEIGHBORS);
            totalRemoved += removed;
            networks[i]->calcDegreeStats();
        }

        printStep(3, "Recalculating statistics");
        printDetailHighlight("Edges removed", to_string(totalRemoved), YELLOW);

        printSubTitle("Optimized Network Statistics");

        printTableHeader({ {"Nodes", 10}, {"Edges", 10}, {"Max Deg", 12}, {"Avg Deg", 12} });

        for (int i = 0; i < 10; i++) {
            NetworkStats stats = networks[i]->getStats();
            bool good = stats.maxDegree <= Config::MAX_NEIGHBORS;
            printTableRow({
                to_string(stats.nodeCount),
                to_string(stats.edgeCount),
                to_string(stats.maxDegree),
                Utils::formatNum(stats.avgDegree, 2)
                }, widths, good);
        }

        printSuccess("All networks optimized");

        if (askYesNo("Generate optimization animation?")) {
            printProgress("Creating visualization network");
            Network viz(100, 5.0);
            int edgesBefore = viz.connect(Config::TRANSMISSION_RANGE);
            printDone();

            vector<vector<bool>> before;
            viz.copyMatrixTo(before);

            printProgress("Calculating edge changes");
            vector<pair<int, int>> removed, added, kept;
            viz.getEdgeChanges(before, removed, added, kept);
            printDone();

            printDetail("Edges before", to_string(edgesBefore));
            printDetail("Edges removed", to_string(removed.size()));
            printDetail("Edges kept", to_string(kept.size()));

            printProgress("Generating Python script");
            PythonVisualizer::generateOptimizationAnimation(viz, "viz_optimize.py", "Optimization", before);
            printDone();

            printSuccess("Opened viz_optimize.py");
        }
    }
}

void runExperiment2() {
    clearScreen();
    printTitle("EXPERIMENT 2: ROUTING (BEFORE)");

    printBoxStart("Configuration");
    printBoxRow("Algorithm", "Breadth-First Search (BFS)");
    printBoxRow("Network Size", "200 nodes");
    printBoxRow("Grid Size", "7. 0 x 7.0");
    printBoxRow("Query Count", "10");
    printBoxRow("Optimization", "None (original network)");
    printBoxEnd();

    printSubTitle("Phase 1: Network Setup");

    printStep(1, "Creating network");
    Network net(200, 7.0);
    printDetail("Nodes allocated", "200");

    printStep(2, "Establishing connections");
    int edges = net.connect(Config::TRANSMISSION_RANGE);
    net.calcDegreeStats();
    printDetail("Edges created", to_string(edges));
    printDetail("Max degree", to_string(net.getMaxDegree()));
    printDetail("Avg degree", Utils::formatNum(net.getAvgDegree(), 2));

    printSubTitle("Phase 2: Path Queries (BFS)");

    printStep(1, "Generating random queries");
    printDetail("Source range", "0 - 199");
    printDetail("Destination range", "0 - 199");

    printStep(2, "Executing BFS searches");

    printTableHeader({
        {"#", 4}, {"Src", 6}, {"Dst", 6}, {"Found", 6},
        {"Hops", 6}, {"Dist", 8}, {"Path", 30}
        });

    vector<int> widths = { 4, 6, 6, 6, 6, 8, 30 };
    int success = 0, totalHops = 0;
    double totalDist = 0;

    for (int q = 0; q < 10; q++) {
        int from = rand() % 200;
        int to = rand() % 200;
        if (from == to) to = (to + 1) % 200;

        PathResult r = net.findPath(from, to);

        if (r.found) {
            success++;
            totalHops += r.hops;
            totalDist += r.distance;
        }

        printTableRow({
            to_string(q),
            to_string(from),
            to_string(to),
            r.found ? "YES" : "NO",
            r.found ? to_string(r.hops) : "-",
            r.found ? Utils::formatNum(r.distance, 2) : "-",
            Utils::formatPath(r.path)
            }, widths, r.found);
    }

    printSubTitle("Results Summary");

    printBoxStart("Query Statistics");
    printBoxRow("Total queries", "10");
    printBoxRow("Successful", to_string(success));
    printBoxRow("Failed", to_string(10 - success));
    printBoxRow("Success rate", Utils::formatPercent(success / 10.0));
    if (success > 0) {
        printBoxRow("Avg hops", Utils::formatNum(static_cast<double>(totalHops) / success, 2));
        printBoxRow("Avg distance", Utils::formatNum(totalDist / success, 2));
    }
    printBoxEnd();
}

void runExperiment3() {
    clearScreen();
    printTitle("EXPERIMENT 3: ROUTING COMPARISON");

    printBoxStart("Configuration");
    printBoxRow("Algorithm", "BFS (before & after)");
    printBoxRow("Optimization", "Quick Sort topology control");
    printBoxRow("Network Size", "200 nodes");
    printBoxRow("Query Count", "10");
    printBoxEnd();

    printSubTitle("Phase 1: Initial Network");

    printStep(1, "Creating network");
    Network net(200, 7.0);
    printDetail("Nodes", "200");

    printStep(2, "Establishing connections");
    int edgesBefore = net.connect(Config::TRANSMISSION_RANGE);
    net.calcDegreeStats();
    printDetail("Edges", to_string(edgesBefore));
    printDetail("Max degree", to_string(net.getMaxDegree()));

    printStep(3, "Generating queries");
    vector<RoutingQuery> queries;
    for (int i = 0; i < 10; i++) {
        int from = rand() % 200;
        int to = rand() % 200;
        if (from == to) to = (to + 1) % 200;
        queries.emplace_back(from, to);
    }
    printDetail("Queries generated", "10");

    printSubTitle("Phase 2: Pre-Optimization Routing");

    printStep(1, "Saving network state");
    vector<vector<bool>> matrixBefore;
    net.copyMatrixTo(matrixBefore);
    printDetail("Matrix saved", to_string(edgesBefore) + " edges");

    printStep(2, "Running BFS queries");
    int successBefore = 0;
    for (auto& q : queries) {
        q.before = net.findPath(q.source, q.destination);
        if (q.before.found) successBefore++;
    }
    printDetail("Paths found", to_string(successBefore) + "/10");

    printSubTitle("Phase 3: Topology Optimization");

    printStep(1, "Sorting neighbors");
    printDetail("Algorithm", "Quick Sort");

    printStep(2, "Pruning connections");
    int removed = net.optimize(Config::MAX_NEIGHBORS);
    net.calcDegreeStats();
    printDetailHighlight("Edges removed", to_string(removed), YELLOW);
    printDetail("Edges remaining", to_string(net.getEdgeCount()));
    printDetail("New max degree", to_string(net.getMaxDegree()));

    printStep(3, "Saving optimized state");
    vector<vector<bool>> matrixAfter;
    net.copyMatrixTo(matrixAfter);

    printSubTitle("Phase 4: Post-Optimization Routing");

    printStep(1, "Running BFS queries");
    int successAfter = 0;
    for (auto& q : queries) {
        q.after = net.findPath(q.source, q.destination);
        if (q.after.found) successAfter++;
    }
    printDetail("Paths found", to_string(successAfter) + "/10");

    printSubTitle("Comparison Results");

    printTableHeader({
        {"#", 4}, {"Src", 5}, {"Dst", 5},
        {"B: OK", 5}, {"B:H", 4}, {"B:D", 7},
        {"A:OK", 5}, {"A:H", 4}, {"A:D", 7}, {"Delta", 6}
        });

    vector<int> widths = { 4, 5, 5, 5, 4, 7, 5, 4, 7, 6 };
    double hopsB = 0, hopsA = 0;

    for (int i = 0; i < 10; i++) {
        const auto& b = queries[i].before;
        const auto& a = queries[i].after;

        string delta = "-";
        if (b.found && a.found) {
            int diff = a.hops - b.hops;
            delta = (diff > 0 ? "+" : "") + to_string(diff);
        }

        if (b.found) hopsB += b.hops;
        if (a.found) hopsA += a.hops;

        printTableRow({
            to_string(i),
            to_string(queries[i].source),
            to_string(queries[i].destination),
            b.found ? "Y" : "N",
            b.found ? to_string(b.hops) : "-",
            b.found ? Utils::formatNum(b.distance, 1) : "-",
            a.found ? "Y" : "N",
            a.found ? to_string(a.hops) : "-",
            a.found ? Utils::formatNum(a.distance, 1) : "-",
            delta
            }, widths);
    }

    printSubTitle("Summary");

    printBoxStart("Before Optimization");
    printBoxRow("Edges", to_string(edgesBefore));
    printBoxRow("Paths found", to_string(successBefore) + "/10");
    printBoxRow("Success rate", Utils::formatPercent(successBefore / 10.0));
    if (successBefore > 0) {
        printBoxRow("Avg hops", Utils::formatNum(hopsB / successBefore, 2));
    }
    printBoxEnd();

    printBoxStart("After Optimization");
    printBoxRow("Edges", to_string(net.getEdgeCount()));
    printBoxRow("Edges removed", to_string(removed));
    printBoxRow("Paths found", to_string(successAfter) + "/10");
    printBoxRow("Success rate", Utils::formatPercent(successAfter / 10.0));
    if (successAfter > 0) {
        printBoxRow("Avg hops", Utils::formatNum(hopsA / successAfter, 2));
    }
    printBoxEnd();

    if (askYesNo("Generate comparison visualization?")) {
        printPrompt("Enter query index (0-9)");
        int idx;
        if (getInt(idx, 0, 9)) {
            printProgress("Generating Python script");
            string file = "viz_compare_" + to_string(idx) + ".py";
            PythonVisualizer::generateComparisonVisualization(
                net, file, "Query " + to_string(idx),
                matrixBefore, matrixAfter,
                queries[idx].before.path, queries[idx].after.path
            );
            printDone();

            printDetail("Query", to_string(queries[idx].source) + " -> " + to_string(queries[idx].destination));
            printDetail("Before hops", queries[idx].before.found ? to_string(queries[idx].before.hops) : "No path");
            printDetail("After hops", queries[idx].after.found ? to_string(queries[idx].after.hops) : "No path");

            printSuccess("Opened " + file);
        }
        else {
            printError("Invalid index");
        }
    }
}

// ============================================================================
// MAIN MENU
// ============================================================================

void showMainMenu() {
    clearScreen();

    cout << "\n";
    setColor(DARK_GRAY);
    cout << "      +-------------------------------------------+\n";
    cout << "      |                                           |\n";
    setColor(CYAN);
    cout << "      |       WIRELESS NETWORK SIMULATOR          ";
    setColor(DARK_GRAY);
    cout << "|\n";
    cout << "      |                                           |\n";
    cout << "      +-------------------------------------------+\n";
    resetColor();

    setColor(DARK_GRAY);
    cout << "\n      Data Structures & Algorithms Demonstration\n";
    resetColor();

    cout << "\n\n";
    printMenuItem(1, "Topology Control", "Arrays & Quick Sort");
    cout << "\n";
    printMenuItem(2, "Routing (Before)", "BFS Graph Traversal");
    cout << "\n";
    printMenuItem(3, "Routing Comparison", "Before vs After");
    cout << "\n";
    printMenuItem(4, "Run All Experiments", "Complete Demo");
    printMenuItemExit();
}

void showGoodbye() {
    clearScreen();

    cout << "\n\n";
    setColor(DARK_GRAY);
    cout << "      +-------------------------------------------+\n";
    cout << "      |                                           |\n";
    setColor(CYAN);
    cout << "      |                 GOODBYE!                    ";
    setColor(DARK_GRAY);
    cout << "|\n";
    cout << "      |                                           |\n";
    cout << "      +-------------------------------------------+\n";
    resetColor();

    cout << "\n";
    printInfo("Thank you for using the simulator");
    cout << "\n\n";
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    while (true) {
        showMainMenu();
        printPrompt("Select option");

        int choice;
        if (!getInt(choice, 0, 4)) {
            printError("Invalid input.  Enter 0-4.");
            Sleep(1000);
            continue;
        }

        if (choice == 0) {
            showGoodbye();
            break;
        }

        if (choice == 1 || choice == 4) {
            runExperiment1();
            waitForEnter();
        }
        if (choice == 2 || choice == 4) {
            runExperiment2();
            waitForEnter();
        }
        if (choice == 3 || choice == 4) {
            runExperiment3();
            waitForEnter();
        }
    }

    resetColor();
    return 0;
}