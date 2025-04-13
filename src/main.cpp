#include <cstdint>
#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "lib/json.hpp"

using nlohmann::json_abi_v3_12_0::json;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;


//using namespace std;
using std::cin;
using std::cout;
using std::vector;
using std::string;
using std::pair;
using std::system;

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
    int month;
    int year;
    vector<string> possMatch;
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
};

mongocxx::instance instance;
mongocxx::uri uri("mongodb+srv://bhargavj16:S8JUoO8L6a7B4kEG@clster0.dj1sh.mongodb.net/?retryWrites=true&w=majority&appName=clster0");
mongocxx::client client(uri);
auto db = client["lost-found"];

class LostFound{

	private:

	protected:

        void addArchive();
        void addNUser();
        void resolveClaim();

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
        }


        void getAllLost();
        void getAllFound();



        void exit(){
            cout<<"thank you for using the app";
            cout<<"\n exiting ....";
            sleep_for(seconds(2));
            return;
        }

};


class User:public LostFound{
	private:
        bool is_auth=false;

	public:

        //using LostFound::LostFound;

        void start(){
            if(is_auth){
                cout<<"initiating...\n";
                sleep_for(seconds(2));
                cout<<"choose a option from below\n";
                cout<<" 1.report lost request\n 2.report found request\n 3.view all lost-items\n 4.view all found-items\n 5.my lost requests\n 6.view my claims\n 7.exit\n";
                int choice;
                cin>>choice;

                switch(choice){
                    case 1:
                        reportlost();
                        break;
                    case 2:
                        reportfound();
                        break;
                    case 3:
                        viewAllLost();
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
        }


        void login(){
            cout<<"hello user please enter your username and password to login\n";
            cout<<"username: ";
            string username;
            string password;
            cin>>username;
            cout<<"password: ";
            cin>>password;
            cout<<"\n";

            if(authenticate(username,password,false)){
                cout<<"logged in successfully as "<<username<<"\n";
                sleep_for(seconds(2));
                system(CLEAR);
                is_auth=true;
                start();
            }
            else{
                cout<<"login failed try again\n";
                login();
            }


        }


        void reportlost(){
             
            cout<<"follow the procedure to report your lost item\n";
            
            lItem item;

            


        }

        void reportfound();
        void viewAllLost();
        void viewAllFound();
        void viewMyLost();
        void viewMyClaims();



};

class Admin:protected LostFound{

	private:
        bool is_auth=false;

	public:
        //using LostFound::LostFound;

        void start(){
            if(is_auth){
                cout<<"initiating...\n";
                sleep_for(seconds(2));
                cout<<"choose a option from below\n";
                case 5:
                archiveItems();
                break;
                cout<<" 1.view all lost items\n 2.view all found items\n 3.add new user or admin \n 4.view claims \n 6.exit";
                int choice;
                cin>>choice;

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
                        exit()
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
        }


        void login(){
            cout<<"hello admin please enter your username and password to login\n";
            cout<<"username: ";
            string username;
            string password;
            cin>>username;
            cout<<"password: ";
            cin>>password;
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


        }


        void AviewLost();
        void AviewFound();
        void addUser();
        void viewClaims();
        void verifyClaim();
        void archiveItems();


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

    creatSession();

	return 0;
}



void creatSession(){
    /*
        creating objects and session 
    */

    cout<<"Are you a admin or user\n 1.user\n 2.admin\n";
    
    int* d=new int;
    cin>>*d;
    
    if(*d==1){
        free(d);
        cout<<"creating a user session please wait\n";
        sleep_for(seconds(2));
        system(CLEAR);
        User usr;
        usr.start();
    }
    else if(*d==2){
        free(d);
        cout<<"creating a user session please wait\n";
        sleep_for(seconds(2));
        system(CLEAR);
        Admin usr;
        usr.start();
    }
    else{
        sleep_for(seconds(2));
        cout<<"please provide valid option\n";
        sleep_for(seconds(2));
        free(d);
        creatSession();
    }

    
}





