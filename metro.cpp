#include <bits/stdc++.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

/* -------------------- DATA STRUCTURES -------------------- */

struct State {
    string station;
    string line;
    int stations;
    int interchanges;
};

unordered_map<string, vector<string>> adj;          // adjacency list
unordered_map<string, vector<string>> stationLines; // station → lines

/* -------------------- HELPERS -------------------- */

string makeKey(const string &station, const string &line) {
    return station + "|" + line;
}

pair<string,string> splitKey(const string &k) {
    int pos = k.find('|');
    return {k.substr(0,pos), k.substr(pos+1)};
}

/* -------------------- GRAPH BUILDING -------------------- */

void buildGraph(const json &data) {
    // Line connections
    for (auto &[station, info] : data.items()) {
        for (auto &[line, conn] : info["lines"].items()) {
            stationLines[station].push_back(line);

            if (!conn["prev"].is_null()) {
                adj[makeKey(station,line)]
                    .push_back(makeKey(conn["prev"], line));
            }
            if (!conn["next"].is_null()) {
                adj[makeKey(station,line)]
                    .push_back(makeKey(conn["next"], line));
            }
        }
    }

    // Interchanges (same station, different lines)
    for (auto &[station, lines] : stationLines) {
        for (string &l1 : lines)
            for (string &l2 : lines)
                if (l1 != l2)
                    adj[makeKey(station,l1)]
                        .push_back(makeKey(station,l2));
    }
}

/* -------------------- ALGO 1 --------------------
   MIN INTERCHANGES (PRIMARY), MIN STATIONS (SECONDARY)
-------------------------------------------------- */

void minInterchangeRoute(const string &src, const string &dst) {

    map<string, pair<int,int>> dist;  // {interchanges, stations}
    map<string, string> parent;

    priority_queue<
        tuple<int,int,string>,
        vector<tuple<int,int,string>>,
        greater<>
    > pq;

    // Initialize distances
    for (auto &[station, lines] : stationLines)
        for (auto &l : lines)
            dist[makeKey(station,l)] = {INT_MAX, INT_MAX};

    // Start states
    for (auto &l : stationLines[src]) {
        dist[makeKey(src,l)] = {0,0};
        pq.push({0,0,makeKey(src,l)});
    }

    while (!pq.empty()) {
        auto [ic, st, u] = pq.top();
        pq.pop();

        if (dist[u] < make_pair(ic,st)) continue;

        auto [uStation, uLine] = splitKey(u);

        for (auto &v : adj[u]) {
            auto [vStation, vLine] = splitKey(v);

            int nic = ic + (uLine != vLine);
            int nst = st + 1;

            if (make_pair(nic,nst) < dist[v]) {
                dist[v] = {nic,nst};
                parent[v] = u;
                pq.push({nic,nst,v});
            }
        }
    }

    // Best destination state
    string best;
    pair<int,int> ans = {INT_MAX, INT_MAX};

    for (auto &l : stationLines[dst]) {
        if (dist[makeKey(dst,l)] < ans) {
            ans = dist[makeKey(dst,l)];
            best = makeKey(dst,l);
        }
    }

    if (best.empty()) {
        cout << "No route found.\n";
        return;
    }

    // Reconstruct path
    vector<string> path;
    while (best != "") {
        path.push_back(best);
        best = parent[best];
    }
    reverse(path.begin(), path.end());

    cout << "\n=== MIN INTERCHANGE ROUTE ===\n";
    for (auto &p : path) cout << p << "\n";
    cout << "Interchanges: " << ans.first << "\n";
    cout << "Stations: " << ans.second << "\n";
}

/* -------------------- ALGO 2 --------------------
   MIN STATIONS WITH MAX INTERCHANGE LIMIT
-------------------------------------------------- */

void minStationsWithLimit(const string &src,
                          const string &dst,
                          int maxIC) {

    queue<State> q;
    map<string, pair<int,int>> best;

    for (auto &l : stationLines[src]) {
        q.push({src,l,0,0});
        best[makeKey(src,l)] = {0,0};
    }

    while (!q.empty()) {
        State cur = q.front(); q.pop();

        if (cur.station == dst) {
            cout << "\n=== MIN STATION ROUTE (LIMITED IC) ===\n";
            cout << "Stations: " << cur.stations << "\n";
            cout << "Interchanges: " << cur.interchanges << "\n";
            return;
        }

        for (auto &nx : adj[makeKey(cur.station,cur.line)]) {
            auto [ns, nl] = splitKey(nx);

            int ni = cur.interchanges + (cur.line != nl);
            if (ni > maxIC) continue;

            int nst = cur.stations + 1;

            if (!best.count(nx) || nst < best[nx].second) {
                best[nx] = {ni,nst};
                q.push({ns,nl,nst,ni});
            }
        }
    }

    cout << "No route within interchange limit.\n";
}

/* -------------------- MAIN -------------------- */

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream file("dmrc.json");
    if (!file) {
        cout << "Error: dmrc.json not found\n";
        return 1;
    }

    json data;
    file >> data;

    buildGraph(data);

    string src, dst;
    cout << "Enter Source Station: ";
    getline(cin, src);
    cout << "Enter Destination Station: ";
    getline(cin, dst);

    minInterchangeRoute(src, dst);

    int k;
    cout << "\nEnter max interchanges allowed: ";
    cin >> k;

    minStationsWithLimit(src, dst, k);

    return 0;
}
