// Metro.java
import org.json.*;
import java.nio.file.*;
import java.util.*;

class Metro {

    static Map<String, List<String>> adj = new HashMap<>();
    static Map<String, List<String>> stationLines = new HashMap<>();

    static String key(String s, String l){ return s+"|"+l; }

    static String[] split(String k){ return k.split("\\|"); }

    static void addEdge(String u, String v){
        adj.computeIfAbsent(u, x->new ArrayList<>()).add(v);
    }

    static void buildGraph(JSONObject data){
        for(String station: data.keySet()){
            JSONObject info = data.getJSONObject(station);
            JSONObject lines = info.getJSONObject("lines");

            for(String line: lines.keySet()){
                stationLines.computeIfAbsent(station,x->new ArrayList<>()).add(line);
                JSONObject conn = lines.getJSONObject(line);

                if(!conn.isNull("prev"))
                    addEdge(key(station,line),
                            key(conn.getString("prev"),line));

                if(!conn.isNull("next"))
                    addEdge(key(station,line),
                            key(conn.getString("next"),line));
            }
        }

        // Interchanges
        for(String st: stationLines.keySet()){
            for(String l1: stationLines.get(st))
                for(String l2: stationLines.get(st))
                    if(!l1.equals(l2))
                        addEdge(key(st,l1), key(st,l2));
        }
    }

    // -------- DIJKSTRA (min interchanges) --------
    static void minInterchangeRoute(String src, String dst){
        Map<String, int[]> dist = new HashMap<>();
        Map<String, String> parent = new HashMap<>();

        PriorityQueue<String> pq = new PriorityQueue<>(
            Comparator.comparingInt((String k)->dist.get(k)[0])
                      .thenComparingInt(k->dist.get(k)[1])
        );

        for(String st: stationLines.keySet())
            for(String l: stationLines.get(st))
                dist.put(key(st,l), new int[]{9999,9999});

        for(String l: stationLines.get(src)){
            String k = key(src,l);
            dist.put(k, new int[]{0,0});
            pq.add(k);
        }

        while(!pq.isEmpty()){
            String u = pq.poll();
            String[] su = split(u);

            for(String v: adj.getOrDefault(u, List.of())){
                String[] sv = split(v);
                int ic = dist.get(u)[0] + (su[1].equals(sv[1])?0:1);
                int st = dist.get(u)[1] + 1;

                if(ic < dist.get(v)[0] ||
                   (ic==dist.get(v)[0] && st<dist.get(v)[1])){
                    dist.put(v, new int[]{ic,st});
                    parent.put(v,u);
                    pq.add(v);
                }
            }
        }

        System.out.println("Route computed.");
    }

    public static void main(String[] args) throws Exception {
        String text = Files.readString(Path.of("dmrc.json"));
        JSONObject data = new JSONObject(text);
        buildGraph(data);
    }
}
