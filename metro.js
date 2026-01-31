// metro.js
const fs = require('fs');

let adj = {};
let stationLines = {};

const key = (s,l)=>`${s}|${l}`;

function addEdge(u,v){
    if(!adj[u]) adj[u]=[];
    adj[u].push(v);
}

function buildGraph(data){
    for(const station in data){
        const lines = data[station].lines;

        for(const line in lines){
            if(!stationLines[station]) stationLines[station]=[];
            stationLines[station].push(line);

            const conn = lines[line];

            if(conn.prev)
                addEdge(key(station,line),
                        key(conn.prev,line));
            if(conn.next)
                addEdge(key(station,line),
                        key(conn.next,line));
        }
    }

    for(const st in stationLines){
        for(const l1 of stationLines[st])
            for(const l2 of stationLines[st])
                if(l1!==l2)
                    addEdge(key(st,l1), key(st,l2));
    }
}

const data = JSON.parse(fs.readFileSync("dmrc.json"));
buildGraph(data);
console.log("Graph ready");
