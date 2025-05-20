#include <cstdint>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using std::string;

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

int main() {
    mongocxx::instance instance;
    mongocxx::uri uri("mongodb+srv://bhargavj16:S8JUoO8L6a7B4kEG@clster0.dj1sh.mongodb.net/?retryWrites=true&w=majority&appName=clster0");
    
    ///*
    mongocxx::client client(uri);
    auto db = client["lost-found"];
    auto collection = db["admins"];

    mongocxx::options::find opts{};
    opts.projection(make_document(kvp("username", 1), kvp("password", 1),kvp("_id",1)));

    auto result = collection.find_one(make_document(kvp("username", "admin")),opts);

    //std::cout<<result->view()["_id"].get_oid().value.to_string();
    std::cout<<result->view()["_id"].key();

    std::cout << bsoncxx::to_json(*result) << std::endl;
    string x="76";
    std::cout<<std::stoi(x);
    //*/
    
    /*
    mongocxx::pool pool(uri);
    auto conn = pool.acquire();

    for(auto&& c : conn->database("lost-found")["users"].find({}))
	{   
        auto id=c["username"]["bhargav"].type();
        //std::cout << bsoncxx::to_json(id) << std::endl;
		std::cout<<id<<std::endl;
	}
        */
    

    
}