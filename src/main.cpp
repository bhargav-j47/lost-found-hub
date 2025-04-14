#include <cstdint>
#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <ctime>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "lib/json.hpp"

using nlohmann::json_abi_v3_12_0::json;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;
using bsoncxx::stdx::string_view;
using bsoncxx::oid;


//using namespace std;
using std::cin;
using std::cout;
using std::vector;
using std::string;
using std::pair;
using std::system;
using std::getline;
using std::stoi;

using std::this_thread::sleep_for;
using std::chrono::seconds;


#if defined(_WIN32)
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif


struct lItem{
    string itemid;
    string name;
    string des;
    string loc;
    string lostBy;
    int day;
    int  month;
    int year;
};

struct fItem{
    string itemid;
    string name;
    string des;
    string loc;
    string foundBy;
    int day;
    int month;
    int year;
    vector<pair<string,string>> claimed_by;
    int claimCount=0;
};


struct cItem{
    string itemid;
    string fid;
    string lid;
    string claimed_by;
    string claimed_for;
    string status="pending";
};

mongocxx::instance instance;
mongocxx::uri uri("<url>");
mongocxx::client client(uri);
auto db = client["lost-found"];

class LostFound{

	private:

        time_t timestamp = time(NULL);
        struct tm datetime = *localtime(&timestamp);
        int currmonth=datetime.tm_mon +1;
        int curryear=datetime.tm_year +1900;

	protected:

        void makeArchive(){

            auto foundc=db["found-items"];
            auto archivec=db["archived"];
            auto claimsc=db["claims"];  

            auto find=foundc.find(make_document(kvp("$or" ,make_array(
                make_document(kvp("month",make_document(kvp("$lt",currmonth-3)))),
                make_document(kvp("year",make_document(kvp("$lt",curryear-1))))
                ) )));


            
            
            vector<bsoncxx::document::view> findvec;
            for(auto&& r:find){
                findvec.push_back(r);

                auto x=make_document(kvp("found id",r["_id"].get_oid().value.to_string()));
            
                auto update_doc = make_document(kvp("$set", make_document(kvp("status", "item archived"))));

                auto result = claimsc.update_many(x.view(), update_doc.view());
            }

            if(findvec.size()!=0){
                auto done=archivec.insert_many(findvec);
            
                auto result=foundc.delete_many(make_document(kvp("$or" ,make_array(
                    make_document(kvp("month",make_document(kvp("$lt",currmonth-3)))),
                    make_document(kvp("year",make_document(kvp("$lt",curryear-1))))
                    ) )));

                
            }


            return;
    
        } //done

        void addNUser(){int a=2;}//all remains

        void resolveClaim(){int a=2;} //all remains

	public: 

        bool authenticate(string username,string password, bool is_admin){

            string collection="users";

            if(is_admin) collection="admins";

            json joson;
            auto result = db[collection].find_one(make_document(kvp("username", username)));
            //auto id=result->view()["_id"].get_oid().value.to_string();
            //cout<<id;
            if(result){
                joson =json::parse(bsoncxx::to_json(result->view()));
            }
            else{
                // cout<<"agaye idhar toh atleast"; 10hrs waisted successfully

                joson=json::parse(R"({"username": "null", "password": "null"})");
            }

            if(joson["username"]==username && joson["password"]==password){
                //cout<<joson["username"]<<"\n"<<joson["_id"]<<"\n";
                return true;
            }
            else{
                return false;
            }
        } //done


        vector<lItem> getAllLost(){

            auto coll=db["lost-items"];

            auto result=coll.find({});

            vector<lItem> items;

            for(auto&& doc:result){

                //json Json=json::parse(bsoncxx::to_json(doc));

                lItem item;
                item.itemid=doc["_id"].get_oid().value.to_string();;
                item.name=doc["name"].get_string().value;
                item.des=doc["description"].get_string().value;
                item.loc=doc["location"].get_string().value;
                item.lostBy=doc["lostBy"].get_string().value;
                item.day=doc["day"].get_int32().value;
                item.month=doc["month"].get_int32().value;
                item.year=doc["year"].get_int32().value;
                

                items.push_back(item);
            }


            return items;

        } //done


        vector<fItem> getAllFound(){
            auto coll=db["found-items"];

            auto result=coll.find({});

            vector<fItem> items;

            for(auto&& doc:result){

                //json Json=json::parse(bsoncxx::to_json(doc));

                fItem item;
                item.itemid=doc["_id"].get_oid().value.to_string();;
                item.name=doc["name"].get_string().value;
                item.des=doc["description"].get_string().value;
                item.loc=doc["location"].get_string().value;
                item.foundBy=doc["foundBy"].get_string().value;
                item.day=doc["day"].get_int32().value;
                item.month=doc["month"].get_int32().value;
                item.year=doc["year"].get_int32().value;
                
                vector<pair<string,string>> cby;

                for(auto p:doc["claimed by"].get_array().value){
                    
                    auto s=p["user"].get_string().value;
                    //auto s="abc";
                    auto k=p["lost-id"].get_string().value;
                    cout<<s<<" "<<k;
                    cby.push_back(std::make_pair((string)s,(string)k));

                }

                item.claimed_by=cby;
                item.claimCount=cby.size();

                items.push_back(item);
            }


            return items;
        } //done

        vector<cItem> getAllClaims(){
            
            auto coll=db["claims"];

            auto result=coll.find({});

            vector<cItem> items;

            for(auto&& doc:result){

                cItem item;
                item.itemid=doc["_id"].get_oid().value.to_string();;
                item.fid=doc["found id"].get_string().value;
                item.lid=doc["lost id"].get_string().value;
                item.claimed_by=doc["claimed by"].get_string().value;
                item.claimed_for=doc["claimed for"].get_string().value;
                item.status=doc["status"].get_string().value;
                
                items.push_back(item);
            }


            return items;
        } //done

        string addLost(lItem item){

            auto coll=db["lost-items"];
            
            
            auto result=coll.insert_one(make_document(
                kvp("name", item.name),
                kvp("description", item.des),
                kvp("day", (item.day)),
                kvp("month",(item.month)),
                kvp("year",(item.year)),
                kvp("location",item.loc),
                kvp("lostBy",item.lostBy)
                ));
            
            string id=result->inserted_id().get_oid().value.to_string();
            return id;
        } //done
        
        string addFound(fItem item){

            auto cby=bsoncxx::builder::basic::array{};

            for(auto v:item.claimed_by){
                cby.append(make_document(
                    kvp("user",v.first),
                    kvp("lost-id",v.second)
                ));
            }

            auto coll=db["found-items"];
            auto result = coll.insert_one(make_document(
                kvp("name", item.name),
                kvp("description", item.des),
                kvp("day", (item.day)),
                kvp("month",(item.month)),
                kvp("year",(item.year)),
                kvp("location",item.loc),
                kvp("foundBy",item.foundBy),
                kvp("claimed by",cby)
                ));

            string id=result->inserted_id().get_oid().value.to_string();
            return id;
        } //done

        void updateFound(fItem item){

            auto collection=db["found-items"];
            
            auto cby=bsoncxx::builder::basic::array{};

            for(auto v:item.claimed_by){
                cby.append(make_document(
                    kvp("user",v.first),
                    kvp("lost-id",v.second)
                ));
            }

            auto id=oid(string_view(item.itemid));

            auto query_filter = make_document(kvp("_id", id));
            auto update_doc = make_document(kvp("$set", make_document(kvp("claimed by", cby))));

            auto result = collection.update_many(query_filter.view(), update_doc.view());

            return;

        } //done

        string addtoClaim(cItem item){
            auto coll=db["claims"];
            
            auto result=coll.insert_one(make_document(
                kvp("found id", item.fid),
                kvp("lost id", item.lid),
                kvp("claimed by",item.claimed_by),
                kvp("claimed for",item.claimed_for),
                kvp("status",item.status)
                ));
            
            string id=result->inserted_id().get_oid().value.to_string();
            return id;
        } // done

        void exit(){
            cout<<"thank you for using the app";
            cout<<"\n exiting ....";
            sleep_for(seconds(2));
            return;
        } //done

};


class User:public LostFound{
	private:
        bool is_auth=false;

        string currUser;

	public:

        //using LostFound::LostFound;

        void start(){
            system(CLEAR);
            if(is_auth){
                
                cout<<"initiating...\n";
                sleep_for(seconds(2));
                cout<<"choose a option from below\n";
                cout<<" 1.report lost request\n 2.report found request\n 3.view all lost-items\n 4.view all found-items\n 5.my lost requests\n 6.view my claims\n 7.exit\n";
                cout<<"enter choice:";
                string ce;
                getline(cin,ce);
                int choice=stoi(ce);

                switch(choice){
                    case 1:
                        reportlost();
                        break;
                    case 2:
                        reportfound();
                        break;
                    case 3:
                        viewAllLost();
                        break;
                    case 4:
                        viewAllFound();
                        break;
                    case 5:
                        viewMyLost();
                        break;
                    case 6:
                        viewMyClaims();
                        break;
                    case 7:
                        exit();
                        break;
                    default:
                        cout<<"sorry you have entered wrong option please enter correct option";
                        sleep_for(seconds(1));
                        system(CLEAR);
                        start();
                }
            }
            else{
                login();
            }

            return;
        } //done


        void login(){
            cout<<"hello user please enter your username and password to login\n";
            cout<<"username: ";
            string username;
            string password;
            //cin>>username;
            getline(cin,username);
            cout<<"password: ";
            //cin>>password;
            getline(cin,password);

            
            cout<<"\n";

            if(authenticate(username,password,false)){
                cout<<"logged in successfully as "<<username<<"\n";
                sleep_for(seconds(2));
                system(CLEAR);
                is_auth=true;
                currUser=username;
                start();
            }
            else{
                cout<<"login failed try again\n";
                login();
            }

            return;
        } //done


        void reportlost(){
            system(CLEAR);
            cout<<"follow the procedure to report your lost item\n";
            
            lItem item;
            item.lostBy=currUser;

            cout<<"enter title of your lost item:";
            getline(cin,item.name); 
            cout<<"provide a description of your item \n (be sure to add exact description to get claim easily)\n";
            cout<<"enter description:";
            getline(cin,item.des); 
            cout<<"enter date of  lost item\n";
            cout<<"day:";
            string temp;
            getline(cin,temp);
            item.day=stoi(temp);
            cout<<"month:";
            getline(cin,temp);
            item.month=stoi(temp);
            cout<<"year:";
            getline(cin,temp);
            item.year=stoi(temp);
            cout<<"where you lost your item?";
            cout<<"location:";
            getline(cin,item.loc); 
            sleep_for(seconds(1));
            cout<<"confirm all the information\n";
            cout<<"name: "<<item.name<<"\ndescription: "<<item.des<<"\nlocation: "<<item.loc<<"\n date: "<<item.day<<"/"<<item.month<<"/"<<item.year<<"\n";
            sleep_for(seconds(1));
            redol:
            cout<<"\n Do you want to continue(Yes/No/Restart): ";
            string choice;
            getline(cin,choice);
            if(choice=="Yes"||choice=="y"||choice=="Y"){
                cout<<"adding your request please wait\n";
                addLost(item);
                cout<<"added successfully\n";
                sleep_for(seconds(2));
                start();
            }
            else if(choice=="No"||choice=="n"||choice=="N"){
                cout<<"going back to start menu...\n";
                sleep_for(seconds(1));
                start();
            }
            else if(choice=="Restart"||choice=="r"||choice=="R"){
                cout<<"restarting...";
                sleep_for(seconds(1));
                reportlost();
            }
            else{
                cout<<"you have entered wrong choice please enter correct choice\n ";
                sleep_for(seconds(1));
                goto redol;
            }

            return;
        } //done

        void reportfound(){
            system(CLEAR);
            cout<<"follow the procedure to report a found item\n";
            
            fItem item;
            item.foundBy=currUser;

            cout<<"enter title of your found item:";
            getline(cin,item.name); 
            cout<<"provide a description of found item \n (be sure to add exact description to get claim easily)\n";
            cout<<"enter description:";
            getline(cin,item.des); 
            cout<<"enter date of found item\n";
            cout<<"day:";
            string temp;
            getline(cin,temp);
            item.day=stoi(temp);
            cout<<"month:";
            getline(cin,temp);
            item.month=stoi(temp);
            cout<<"year:";
            getline(cin,temp);
            item.year=stoi(temp);
            cout<<"where you found this your item?";
            cout<<"location:";
            getline(cin,item.loc); 
            sleep_for(seconds(1));
            cout<<"confirm all the information\n";
            cout<<"name: "<<item.name<<"\ndescription: "<<item.des<<"\nlocation: "<<item.loc<<"\n date: "<<item.day<<"/"<<item.month<<"/"<<item.year<<"\n";
            sleep_for(seconds(1));
            redof:
            cout<<"\n Do you want to continue(Yes/No/Restart): ";
            string choice;
            getline(cin,choice);
            if(choice=="Yes"||choice=="y"||choice=="Y"){
                cout<<"adding your request please wait\n";
                string resid=addFound(item);
                cout<<"added successfully\n";
                sleep_for(seconds(2));
                start();
            }
            else if(choice=="No"||choice=="n"||choice=="N"){
                cout<<"going back to start menu...\n";
                sleep_for(seconds(1));
                start();
            }
            else if(choice=="Restart"||choice=="r"||choice=="R"){
                cout<<"restarting...";
                sleep_for(seconds(1));
                reportfound();
            }
            else{
                cout<<"you have entered wrong choice please enter correct choice\n ";
                sleep_for(seconds(1));
                goto redof;
            }

            return;
        } //done

        void viewAllLost(){
            system(CLEAR);
            cout<<"fetching all lost request please wait\n";
            vector<lItem> items=getAllLost();
            int idx=1;
            for(auto a:items){
                cout<<"index:"<<idx<<"\n";
                cout<<"name: "<<a.name<<"\n";
                cout<<"description: "<<a.des<<"\n";
                cout<<"location: "<<a.loc<<"\n";
                cout<<"lost by: "<<a.lostBy<<"\n";
                cout<<"date lost: "<<a.day<<"/"<<a.month<<"/"<<a.year<<"\n"<<std::endl;
                idx++;
            }

            rechoice:

            sleep_for(seconds(1));

            cout<<"choose one option from below\n";
            cout<<" 1.found a item from above\n 2.filter my lost items\n 3.return to main menu\n";
            cout<<"enter a choice: ";
            string choice;
            getline(cin,choice);
            if(choice=="1"){
                foundres:
                cout<<"paste correct index of lost item that you found\n";
                cout<<"index: ";
                string id;
                getline(cin,id);
                idx=stoi(id);
                idx--;
                cout<<"enter date you found this item\n";
                string fday;string fmonth;string fyear;
                cout<<"day: ";
                getline(cin,fday);
                cout<<"month: ";
                getline(cin,fmonth);
                cout<<"year: ";
                getline(cin,fyear);

                fItem f;
                f.name=items[idx].name;
                f.des=items[idx].des;
                f.loc=items[idx].loc;
                f.foundBy=currUser;
                f.day=stoi(fday);
                f.month=stoi(fmonth);
                f.year=stoi(fyear);
                f.claimed_by.push_back(make_pair(items[idx].lostBy,items[idx].itemid));
                string fid=addFound(f);

                cItem ix;
                ix.fid=fid;
                ix.lid=items[idx].itemid;
                ix.claimed_by=currUser;
                ix.claimed_for=items[idx].lostBy;
                addtoClaim(ix);
                
                cout<<"added successfully \n";
                sleep_for(seconds(1));
                cout<<"returning to main menu\n";
                sleep_for(seconds(1));
                start();

            }
            else if(choice=="2"){
                viewMyLost();
            }
            else if(choice=="3"){
                cout<<"going to main menu please wait...";
                sleep_for(seconds(1));
                start();
            }
            else{
                cout<<"you have entered wrong option \n";
                goto rechoice;
            }

            return;
        } //done

        void viewAllFound(){
            system(CLEAR);
            cout<<"fetching all found request please wait\n";
            vector<fItem> items=getAllFound();
            int idx=1;
            for(auto a:items){
                cout<<"\nindex: "<<idx<<"\n";
                cout<<"name: "<<a.name<<"\n";
                cout<<"description: "<<a.des<<"\n";
                cout<<"location: "<<a.loc<<"\n";
                cout<<"found by: "<<a.foundBy<<"\n";
                cout<<"date lost: "<<a.day<<"/"<<a.month<<"/"<<a.year<<"\n";
                for(auto c:a.claimed_by){
                    cout<<"claimed_by: "<<c.first<<"\n"<<"claimed under :"<<c.second<<"\n";
                }
                idx++;
            }

            re:
            cout<<"\nchoose one option from below\n";
            cout<<" 1.make claim to one of the item\n 2.return to main menu\n";
            cout<<"enter a choice: ";
            string choice;
            getline(cin,choice);
            
            if(choice=="1"){
                
                cout<<"select index of found item you want to make claim to\n";
                cout<<"enter index: ";
                string indx;
                getline(cin,indx);
                idx=stoi(indx);
                
                makeClaim(items[--idx]);

            }
            else if(choice=="2"){
                cout<<"going to main menu please wait...";
                sleep_for(seconds(1));
                start();   
            }
            else{
                cout<<"you have entered wrong option \n";
                goto re;
            }




            return;
        } //done

        void viewMyLost(){
            system(CLEAR);
            cout<<"fetching all your lost item reports\n";
            vector<lItem> items=getAllLost();
            int idx=1;
            //vector<lItem> finalL;
            for(auto a:items){
                if(a.lostBy==currUser){
                cout<<"index:"<<idx<<"\n";
                cout<<"name: "<<a.name<<"\n";
                cout<<"description: "<<a.des<<"\n";
                cout<<"location: "<<a.loc<<"\n";
                cout<<"lost by: "<<a.lostBy<<"\n";
                cout<<"date lost: "<<a.day<<"/"<<a.month<<"/"<<a.year<<" \n"<<std::endl;
                idx++;
                //finalL.push_back(a);
            }
            }

            cout<<"\nhit any key to return to main menu\n";
            string any;
            getline(cin,any);
            start();
            
            return ;

        } //done

        void viewMyClaims(){
            system(CLEAR);
            cout<<"showing all your claims";
            sleep_for(seconds(1));
            vector<cItem> items=getAllClaims();
            int idx=1;
            //vector<lItem> finalL;
            for(auto a:items){
                if(a.claimed_by==currUser || a.claimed_for==currUser){
                cout<<"index:"<<idx<<"\n";
                cout<<"found id: "<<a.fid<<"\n";
                cout<<"lost id: "<<a.lid<<"\n";
                cout<<"claimed by: "<<a.claimed_by<<"\n";
                cout<<"claimed for: "<<a.claimed_for<<"\n";
                cout<<"status: "<<a.status<<" \n"<<std::endl;
                idx++;
                //finalL.push_back(a);
            }
            }

            cout<<"\nhit any key to return to main menu\n";
            string any;
            getline(cin,any);
            start();
            
            return ;
        }

        void makeClaim(fItem item){
            rec:
            cout<<"do you want to choose old lost id against request or create new\n";
            cout<<" 1.choose old\n 2. create new\n";
            cout<<"enter choice: ";
            string x;
            getline(cin,x);
            if(x=="1"){

                cout<<"choose index of lost item from below\n";
                vector<lItem> items=getAllLost();
                int idx=1;
                vector<lItem> finalL;
                for(auto a:items){
                    if(a.lostBy==currUser){
                    cout<<"index:"<<idx<<"\n";
                    cout<<"name: "<<a.name<<"\n";
                    idx++;
                    finalL.push_back(a);
                }
                }                

                cout<<"enter index:";
                string y;
                getline(cin,y);
                idx=stoi(y);
                idx--;

                item.claimed_by.push_back(make_pair(currUser,finalL[idx].itemid));
                updateFound(item);
                
                cItem ix;
                ix.fid=item.itemid;
                ix.lid=finalL[idx].itemid;
                ix.claimed_by=currUser;
                ix.claimed_for=currUser;
                addtoClaim(ix);
                
                cout<<"claim added successfully\n \n return to main menu\n";
                sleep_for(seconds(2));
                start();
            }   
            else if(x=="2"){
                lItem l;
                l.name=item.name;
                l.des=item.des;
                l.loc=item.loc;
                l.day=item.day;
                l.month=item.month;
                l.year=item.year;

                string id=addLost(l);
                item.claimed_by.push_back(make_pair(currUser,id));

                updateFound(item);

                cItem ix;
                ix.fid=item.itemid;
                ix.lid=id;
                ix.claimed_by=currUser;
                ix.claimed_for=currUser;
                addtoClaim(ix);
                
                cout<<"claim added successfully\n \n return to main menu\n";
                sleep_for(seconds(2));
                start();

            }
            else{
                cout<<"oh no you entered wrong choice ";
                goto rec;
            }

            return;
        } //done 
        
        



};

class Admin:protected LostFound{

	private:
        bool is_auth=false;

	public:
        //using LostFound::LostFound;

        void start(){
            if(is_auth){
                system(CLEAR);
                cout<<"initiating...\n";
                sleep_for(seconds(2));
                cout<<"choose a option from below\n";
                cout<<" 1.view all lost items\n 2.view all found items\n 3.add new user or admin \n 4.view claims \n 5.archive old items\n 6.exit\n";
                
                string ce;
                getline(cin,ce);
                int choice=stoi(ce);

                switch(choice){

                    case 1:
                        AviewLost();
                        break;
                    case 2:
                        AviewFound();
                        break;
                    case 3:
                        addUser();
                        break;
                    case 4:
                        viewClaims();
                        break;
                    case 5:
                        archiveItems();
                    case 6:
                        exit();
                        break; 
                    default:
                        cout<<"sorry you have entered wrong option please enter correct option";
                        sleep_for(seconds(1));
                        system(CLEAR);
                        start();
                }

                
            }
            else{
                login();
            }

            return;
        } //done


        void login(){
            system(CLEAR);
            cout<<"hello admin please enter your username and password to login\n";
            cout<<"username: ";
            string username;
            string password;
            //cin>>username;
            getline(cin,username);
            cout<<"password: ";
            //cin>>password;
            getline(cin,password);
            cout<<"\n";

            if(authenticate(username,password,true)){
                cout<<"logged in successfully as "<<username<<"\n";
                sleep_for(seconds(3));
                system(CLEAR);
                is_auth=true;
                start();
            }
            else{
                cout<<"login failed try again\n";
                login();
            }

            return;
        } //done


        void AviewLost(){
            system(CLEAR);
            cout<<"fetching all lost request please wait\n";
            vector<lItem> items=getAllLost();
            for(auto a:items){
                cout<<"name: "<<a.name<<"\n";
                cout<<"description: "<<a.des<<"\n";
                cout<<"location: "<<a.loc<<"\n";
                cout<<"lost by: "<<a.lostBy<<"\n";
                cout<<"date lost: "<<a.day<<"/"<<a.month<<"/"<<a.year<<"\n";
            }
            cout<<"hit any key to go back to main menu\n";
            string any;
            getline(cin,any);
            start();

            return;
        } //done

        void AviewFound(){
            system(CLEAR);
            cout<<"fetching all found request please wait\n";
            vector<fItem> items=getAllFound();
            for(auto a:items){
                cout<<"name: "<<a.name<<"\n";
                cout<<"description: "<<a.des<<"\n";
                cout<<"location: "<<a.loc<<"\n";
                cout<<"found by: "<<a.foundBy<<"\n";
                cout<<"date lost: "<<a.day<<"/"<<a.month<<"/"<<a.year<<"\n";
                for(auto c:a.claimed_by){
                    cout<<"claimed_by: "<<c.first<<"\n"<<"claimed under :"<<c.second;
                }
                cout<<"\n";
            }

            cout<<"hit any key to go back to main menu\n";
            string any;
            getline(cin,any);
            start();

            return;
        }//done


        void viewClaims(){
            system(CLEAR);
            cout<<"showing all claims\n";
            sleep_for(seconds(1));
            vector<cItem> items=getAllClaims();
            int idx=1;
            for(auto a:items){
                cout<<"index:"<<idx<<"\n";
                cout<<"found id: "<<a.fid<<"\n";
                cout<<"lost id: "<<a.lid<<"\n";
                cout<<"claimed by: "<<a.claimed_by<<"\n";
                cout<<"claimed for: "<<a.claimed_for<<"\n";
                cout<<"status: "<<a.status<<" \n"<<std::endl;
                idx++;
            }

            reopn:
            cout<<"choose option from below\n";
            cout<<" 1.verify a claim\n 2.return to main menu\n";
            string any;
            getline(cin,any);
            if(any=="1"){
                cout<<"enter index which you want to verify\n";
                cout<<"index: ";
                getline(cin,any);
                idx=stoi(any)-1;

                verifyClaim(items[idx]);
            }
            else if(any=="2"){
                cout<<"returning to main menu please wait..\n";
                sleep_for(seconds(1));
                start();
            }
            else{
                cout<<"you have entered wrong option please enter correct option\n";
                goto reopn;
            }
            
            return ;
        } //done


        void verifyClaim(cItem item){int a=2;} //fully remaining

        void archiveItems(){
            system(CLEAR);
            reop:
            cout<<"do you really want to archive all items older than 6 months? \n( 1.Yes 2.No)\n";
            string c;
            getline(cin,c);
            if(c=="Yes"||c=="Y"||c=="y"||c=="yes"){
                cout<<"archiving old items.....\n";
                sleep_for(seconds(2));
                cout<<"removing these items from found...\n";
                sleep_for(seconds(1));
                makeArchive();
                cout<<"succefully added to archive \n";
                cout<<"returning to main menu\n";
                sleep_for(seconds(1));
                start();
            }
            else if(c=="No"||c=="N"||c=="n"||c=="no"){
                cout<<"returning to start menu\n";
            }
            else{
                cout<<"you have entered wrong option please enter correct option\n";
                goto reop;
            }

            return;
        }   //done


        void addUser(){int a=2;} //all remaining
        

};

void creatSession();

int main(){
	
    /*
        setting up mongodb connection
    */

    //mongocxx::instance instance;
    //mongocxx::uri uri("mongodb+srv://bhargavj16:S8JUoO8L6a7B4kEG@clster0.dj1sh.mongodb.net/?retryWrites=true&w=majority&appName=clster0");
    //mongocxx::client clnt(uri);
    //auto db = clnt["lost-found"];
    //auto collection = db["admins"];
    //auto result = collection.find_one(make_document(kvp("username", "admin")));
    //cout << bsoncxx::to_json(*result) << std::endl;

    system(CLEAR);
    cout<<"welcome to wce lost and found \n";
    creatSession();
	return 0;
}



void creatSession(){
    /*
        creating objects and session 
    */

    cout<<"Are you a admin or user\n 1.user\n 2.admin\n";
    string d;
    getline(cin,d);
    if(d=="1"){
        cout<<"creating a user session please wait\n";
        sleep_for(seconds(2));
        //system(CLEAR);
        User usr;
        usr.start();
    }
    else if(d=="2"){
        cout<<"creating a user session please wait\n";
        sleep_for(seconds(2));
        //system(CLEAR);
        Admin usr;
        usr.start();
    }
    else{
        sleep_for(seconds(2));
        cout<<"please provide valid option\n";
        sleep_for(seconds(2));
        //free(d);
        creatSession();
    }

    return;
    
}
