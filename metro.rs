// metro.rs
use std::collections::{HashMap, VecDeque};
use serde_json::Value;
use std::fs;

type Adj = HashMap<String, Vec<String>>;

fn key(s:&str,l:&str)->String{
    format!("{}|{}",s,l)
}

fn main(){
    let text = fs::read_to_string("dmrc.json").unwrap();
    let data: Value = serde_json::from_str(&text).unwrap();

    let mut adj: Adj = HashMap::new();
    let mut station_lines: HashMap<String,Vec<String>> = HashMap::new();

    for (station, info) in data.as_object().unwrap(){
        let lines = &info["lines"];

        for (line, conn) in lines.as_object().unwrap(){
            station_lines.entry(station.clone())
                .or_default().push(line.clone());

            if !conn["prev"].is_null(){
                adj.entry(key(station,line))
                    .or_default()
                    .push(key(conn["prev"].as_str().unwrap(), line));
            }
            if !conn["next"].is_null(){
                adj.entry(key(station,line))
                    .or_default()
                    .push(key(conn["next"].as_str().unwrap(), line));
            }
        }
    }

    println!("Graph built in Rust");
}
