import json
import heapq
from collections import defaultdict, deque

# -------------------- GRAPH --------------------

adj = defaultdict(list)
station_lines = defaultdict(list)

def key(station, line):
    return f"{station}|{line}"

def split(k):
    return k.split("|")

# -------------------- BUILD GRAPH --------------------

def build_graph(data):
    for station, info in data.items():
        for line, conn in info["lines"].items():
            station_lines[station].append(line)

            if conn["prev"] is not None:
                adj[key(station, line)].append(key(conn["prev"], line))
            if conn["next"] is not None:
                adj[key(station, line)].append(key(conn["next"], line))

    # Interchanges
    for station, lines in station_lines.items():
        for l1 in lines:
            for l2 in lines:
                if l1 != l2:
                    adj[key(station, l1)].append(key(station, l2))

# -------------------- ALGO 1 --------------------
# Min interchanges → min stations (Dijkstra)

def min_interchange_route(src, dst):
    INF = (10**9, 10**9)
    dist = defaultdict(lambda: INF)
    parent = {}

    pq = []
    for line in station_lines[src]:
        dist[key(src, line)] = (0, 0)
        heapq.heappush(pq, (0, 0, key(src, line)))

    while pq:
        ic, st, u = heapq.heappop(pq)
        if (ic, st) > dist[u]:
            continue

        us, ul = split(u)

        for v in adj[u]:
            vs, vl = split(v)
            nic = ic + (ul != vl)
            nst = st + 1

            if (nic, nst) < dist[v]:
                dist[v] = (nic, nst)
                parent[v] = u
                heapq.heappush(pq, (nic, nst, v))

    best = None
    ans = INF
    for line in station_lines[dst]:
        k = key(dst, line)
        if dist[k] < ans:
            ans = dist[k]
            best = k

    if not best:
        return None

    path = []
    while best:
        path.append(best)
        best = parent.get(best)

    return {
        "path": path[::-1],
        "interchanges": ans[0],
        "stations": ans[1]
    }

# -------------------- ALGO 2 --------------------
# Min stations only (BFS)

def min_station_route(src, dst):
    q = deque()
    visited = {}

    for line in station_lines[src]:
        q.append((src, line, 0, 0))
        visited[key(src, line)] = (0, 0)

    while q:
        station, line, stations, interchanges = q.popleft()
        if station == dst:
            return {
                "stations": stations,
                "interchanges": interchanges
            }

        for nxt in adj[key(station, line)]:
            ns, nl = split(nxt)
            ni = interchanges + (line != nl)
            nst = stations + 1

            if nxt not in visited or nst < visited[nxt][1]:
                visited[nxt] = (ni, nst)
                q.append((ns, nl, nst, ni))

    return None

# -------------------- PRINT HELPERS --------------------

def print_route(title, result):
    print(f"\n=== {title} ===")
    for p in result["path"]:
        print(p)
    print("Interchanges:", result["interchanges"])
    print("Stations:", result["stations"])

# -------------------- MAIN --------------------

def main():
    with open("dmrc.json", "r", encoding="utf-8") as f:
        data = json.load(f)

    build_graph(data)

    # Show all stations
    stations = sorted(station_lines.keys())
    print("\n===== DELHI METRO STATIONS =====")
    for s in stations:
        print(s)

    src = input("\nEnter Source Station: ").strip()
    dst = input("Enter Destination Station: ").strip()

    r1 = min_interchange_route(src, dst)
    r2 = min_station_route(src, dst)

    if not r1:
        print("No route found.")
        return

    print_route("MIN INTERCHANGE ROUTE", r1)

    if r2:
        print("\n=== MIN STATION ROUTE ===")
        print("Stations:", r2["stations"])
        print("Interchanges:", r2["interchanges"])

    # Decide BEST route
    print("\n===== BEST ROUTE SELECTED =====")
    print_route("BEST ROUTE", r1)

if __name__ == "__main__":
    main()
