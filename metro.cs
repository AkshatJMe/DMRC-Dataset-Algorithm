// Metro.cs
using System;
using System.Collections.Generic;
using Newtonsoft.Json.Linq;
using System.IO;

class Metro {
    static Dictionary<string, List<string>> adj = new();
    static Dictionary<string, List<string>> stationLines = new();

    static string Key(string s,string l)=> $"{s}|{l}";

    static void AddEdge(string u,string v){
        if(!adj.ContainsKey(u)) adj[u]=new();
        adj[u].Add(v);
    }

    static void BuildGraph(JObject data){
        foreach(var station in data){
            var lines = (JObject)station.Value["lines"];

            foreach(var line in lines){
                stationLines.TryAdd(station.Key,new());
                stationLines[station.Key].Add(line.Key);

                var conn = (JObject)line.Value;

                if(conn["prev"]!=null)
                    AddEdge(Key(station.Key,line.Key),
                            Key(conn["prev"].ToString(),line.Key));

                if(conn["next"]!=null)
                    AddEdge(Key(station.Key,line.Key),
                            Key(conn["next"].ToString(),line.Key));
            }
        }
    }

    static void Main(){
        var json = File.ReadAllText("dmrc.json");
        var data = JObject.Parse(json);
        BuildGraph(data);
        Console.WriteLine("Graph built");
    }
}
