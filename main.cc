#define CROW_USE_BOOST 1
#include <crow_all.h>
#include <stdio.h>

#include <boost/program_options.hpp>
#include <cctype>
#include <ctime>
#include <iostream>
#include <memory>
#include <optional>
#include <pqxx/pqxx>
#include <stdexcept>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "functions.h"
namespace po = boost::program_options;
class InvalidArguments : std::exception {};
using namespace std;
auto get_args(int argc, char** argv) -> po::variables_map {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "show help")("pgdsn", po::value<std::string>(),
                                          "PostgreSQL DSN");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  if (!vm.count("pgdsn")) {
    throw InvalidArguments();
  }
  return vm;
}

auto establish_pgconnection(std::string_view dsn) -> pqxx::connection {
  return pqxx::connection(dsn.data());
}

int main(int argc, char** argv) {
  auto vm = get_args(argc, argv);
  auto dsn = vm["pgdsn"].as<std::string>();

  DB postgres("postgres", dsn);

  crow::App<crow::CORSHandler> app;
  auto& cors = app.get_middleware<crow::CORSHandler>();

  // Добавить курьера
  CROW_ROUTE(app, "/api/couriers")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        // парсинг json
        int age;
        string gender;
        string phone;
        string active;
          try {
          auto json = crow::json::load(req.body);

          age = json["age"].i();
          gender = json["gender"].s();
          phone = json["phone"].s();
          active = json["active"].s();
          if(age==0 || age<10 || gender.empty() || gender.size()>3 || phone.size()>12 || phone.size()<11 || active.empty() || active.size()>3){
            throw crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
          }
        catch (const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
          db::courier courier1(1, age, gender, phone, active);

          auto succes=postgres.add_courier(courier1);
          if(!succes){
             return crow::response(
              crow::status::BAD_REQUEST,
              crow::json::wvalue(
                  {{"message", "Courier already exists"}})); 
          }
          return crow::response(
              crow::status::CREATED,
              crow::json::wvalue(
                  {{"message", "Courier added"}}));  // Возвращаем JSON ответ
        
        }
         catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
      
      });

  // обновить статус курьера
  CROW_ROUTE(app, "/api/v1/couriers:id")
      .methods(crow::HTTPMethod::DELETE)([&postgres](const crow::request& req) {
        // парсинг json
        int courier_id;
         string active;
        try {
          auto json = crow::json::load(req.body);

          courier_id = json["courier_id"].i();
          active = json["active"].s();
           if(courier_id<=0 || active.empty() || active.size()>3){
            throw crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
           catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
          db::courier courier1(courier_id, active);
          postgres.update_courier_status(courier1);
          return crow::response(
              crow::status::OK, crow::json::wvalue(
                       {{"message",
                         "Courier status updated"}})); 
        }
         catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
       
      });

  // зарегать пользователя(добавить)
  CROW_ROUTE(app, "/api/v1/register")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        // парсинг json
         int age ;
         string gender;
         string phone;
         string password;
         bool add;
        try {
          auto json = crow::json::load(req.body);
         
          age = json["age"].i();
          gender = json["gender"].s();
          phone = json["phone"].s();
          password = json["password"].s();
            if(age<=10 || phone.size()>12 || phone.size()<11 || gender.empty() || gender.size()>3 || password.empty() || password.size()>40){
            throw crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
        catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
          db::user_password pass1(phone,password);
          db::user user1(1, age, gender, phone);
         
          add = postgres.add_password(pass1);
          if (!add){
              return crow::response(
              crow::status::BAD_REQUEST, crow::json::wvalue({{"message", "User already exists"}}));
          }
          auto succes = postgres.add_user(user1);
          if(!succes){
             return crow::response(
              crow::status::BAD_REQUEST,
              crow::json::wvalue(
                  {{"message", "User already exists"}})); 
          }
          return crow::response(
              crow::status::CREATED, crow::json::wvalue({{"message", "User added"}}));
        } 
        catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
       
      });
//логин пользователя
CROW_ROUTE(app, "/api/v1/login")
.methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
         string phone;
         string password;
         bool login;
         try {
          auto json = crow::json::load(req.body);
         
          phone = json["phone"].s();
          password = json["password"].s();
            if( phone.size()<11 || phone.size()>12 || password.empty() || password.size()>40){
            throw crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
        catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
        db::user_password pass(phone,password);
        login = postgres.login(pass);
        if (login==false){
              return crow::response(
              crow::status::BAD_REQUEST, crow::json::wvalue({{"message", "Incorrect number or password"}}));
        }



         if(login==true){
         return crow::response(
              crow::status::OK, crow::json::wvalue({{"message", "Hello, user!"}}));
         }
         }catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
  
 });

  // Отобразить инфо про пользователя
  CROW_ROUTE(app, "/api/v1/profile")
      .methods(crow::HTTPMethod::GET)([&postgres](const crow::request& req) {
        // парсинг json
        try {

          std::string phone;
          auto phone_param = req.url_params.get("phone");

          phone = phone_param;
          if(phone.size()>12 || phone.size()<11){
            throw crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
          db::user user1(1,34,"", phone);
          auto user_info = postgres.get_info_user(user1);
          if (user_info.has_value()) {
            crow::json::wvalue responce_data{};
            responce_data["user_id"] = user_info.value().get_user_id();
            responce_data["age"] = user_info.value().get_age();
            responce_data["gender"] = user_info.value().get_gender();
            responce_data["phone"] = user_info.value().get_phone();
            return crow::response(crow::status::OK, responce_data);
          } else {
            return crow::response(crow::status::NOT_FOUND, "User not found");
          }
        }
         catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
         catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
      });

  // проверить

  // изменить инфо о курьере
  CROW_ROUTE(app, "/api/v1/couriers:id")
      .methods(crow::HTTPMethod::PATCH)([&postgres](const crow::request& req) {
        int courier_id;
        int age;
        string gender;
        string phone;
        string active;
        try {
          auto json = crow::json::load(req.body);
    
          courier_id = json["courier_id"].i();
          age = json["age"].i();
          gender = json["gender"].s();
          phone = json["phone"].s();
          active = json["active"].s();

          if(courier_id <=0 || age ==0 || age <10 || gender.empty() || gender.size()>3 || phone.size()>12 || phone.size()<11 || active.empty() || active.size()>3){
              return crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
           catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try {
          db::courier courier1(courier_id, age, gender, phone, active);
          postgres.update_courier(courier1);
          return crow::response(
              crow::status::OK, crow::json::wvalue({{"message", "Courier updated"}}));
        }
         catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
        
      });

  // посмотреть инфо о продукте
  CROW_ROUTE(app, "/api/v1/product:id")
      .methods(crow::HTTPMethod::GET)([&postgres](const crow::request& req) {
         int product_id;
        try {
         
          auto id_param = req.url_params.get("product_id");

          product_id = std::stoi(id_param);
            if(product_id<=0){
            return crow::response(crow::status::BAD_REQUEST, "Bad request");
            }
        }
        catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
          db::product product1(product_id);
          auto product_info = postgres.get_info_product(product1);
          if (product_info.has_value()) {
            crow::json::wvalue responce_data{};
            responce_data["product_id"] = product_info.value().get_product_id();
            responce_data["name"] = product_info.value().get_name();
            responce_data["price"] = product_info.value().get_price();
            responce_data["count"] = product_info.value().get_count();
            return crow::response(crow::status::FOUND, responce_data);
          } else {
            return crow::response(crow::status::NOT_FOUND, "Product not found");
          }
        } 
        catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
        
      });

  // изменить данные о продукте
  CROW_ROUTE(app, "/api/v1/product:id")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        int product_id;
        string name;
        int price ;
        int count;
        try {
          auto json = crow::json::load(req.body);
         
          product_id = json["product_id"].i();
          name = json["name"].s();
          price = json["price"].i();
          count = json["count"].i();
          if(product_id<=0 || name.size()>30 || name.empty() || price<0 || count<0)
          {
                      return crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
          catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
          db::product product1(product_id, name, price, count);
          postgres.update_product(product1);
          return crow::response(
              crow::status::OK, crow::json::wvalue({{"message", "Product updated"}}));
        }
         catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
       
      });
 // удалить продукт
  CROW_ROUTE(app, "/api/v1/product:id")
      .methods(crow::HTTPMethod::PATCH)([&postgres](const crow::request& req) {
        int product_id;
        try {
          auto json = crow::json::load(req.body);
         
          product_id = json["product id"].i();
          if(product_id<=0){
                      return crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
        catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
          db::product product1(product_id);
          postgres.update_product_status(product1);

          return crow::response(
              crow::status::OK, crow::json::wvalue({{"message", "Product deleted"}}));
        } 
        catch (const exception& e) {
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
       
      });

  // Изменить информацию в профиле о пользователе  // мб добавить еще старый номер, по которому будет происходить обновление
  CROW_ROUTE(app, "/api/v1/profile")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        int age;
        string phone;
        string old_phone;
        try {
          auto json = crow::json::load(req.body);
         

          age = json["age"].i();
          phone = json["phone"].s();
          old_phone = json["old_phone"].s();
       
          if(age<10  || phone.size()>12 || phone.size()<11 || old_phone.size()>12 || old_phone.size()<11){
            return crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
        catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        try{
          db::user user1(1,age,"", phone);
          postgres.update_user(user1,old_phone);

          return crow::response(
              crow::status::OK, crow::json::wvalue({{"message", "User data updated"}}));
        }
         catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
         
      });

  // Создать заказ  // доделать
  CROW_ROUTE(app, "/api/v1/order")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
       string delivery_address;
      vector<int> products_ids;
        try {
          auto json = crow::json::load(req.body);
          
          // во фронте добавить проверку, что у нас будет не пустой заказ
          delivery_address = json["delivery_address"].s();
          const auto& products_json = json["products"];
        
            for (const auto& product_data : products_json) {
                products_ids.push_back(product_data.i());
            }
          if(products_ids.empty() || delivery_address.size()>80 || delivery_address.empty()){
                      return crow::response(crow::status::BAD_REQUEST, "Bad request");
          }
        }
        catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
        }
        int order_id;
        try{
          db::order order1(0, delivery_address);
          auto maybe_order_id =postgres.add_order(order1);
          if(!maybe_order_id.has_value()){
              return crow::response(crow::status::INTERNAL_SERVER_ERROR,"Error retrieving last order.");
          }
          order_id=*maybe_order_id;
          if(order_id <=0){
            return crow::response(crow::status::INTERNAL_SERVER_ERROR,"Error retrieving last order.");
          }

          //вставить логику user_action отправляем номер телефона и по нему ищем user_id телефона в users, и создаем действие.
          for(size_t i=0;i<products_ids.size();i++){
          db::content content1(order_id,products_ids[i]);
          postgres.add_content_item(content1);
          }
          return crow::response(
              crow::status::CREATED,
              crow::json::wvalue({{"message", "Order succesfully added"}}));

        }
         catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
        }
        
      });


  // посмотреть инфо о заказе
 CROW_ROUTE(app, "/api/v1/order")
      .methods(crow::HTTPMethod::GET)([&postgres](const crow::request& req) {
        int order_id;
    try {
      auto id_param = req.url_params.get("order_id");
    
      order_id = std::stoi(id_param);
      if(order_id<=0){
        return crow::response(crow::status::BAD_REQUEST, "Bad request");
      }
    }
    catch ( const std::runtime_error& e){
          return crow::response(crow::status::BAD_REQUEST, "Bad request");
      }
      try{
      db::order order1(order_id);
      auto order_info = postgres.get_info_order(order1);
      if (order_info.has_value()) {
        crow::json::wvalue responce_data{};
        responce_data["order_id"] = order_info.value().get_order_id();
        responce_data["creation_time"] = order_info.value().get_creation_time();
        responce_data["time"] = order_info.value().get_time();
        responce_data["delivery address"] =
            order_info.value().get_delivery_address();
        responce_data["status"] = order_info.value().get_status();
         return crow::response(crow::status::FOUND, responce_data);
      }
       else{
        return crow::response(
            crow::status::NOT_FOUND, crow::json::wvalue({{"message", "Order not found"}}));
       }
      }
      catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return crow::response(crow::status::INTERNAL_SERVER_ERROR, "Internal Server Error");
      }
    });













    app.port(18080).run();
    return 0;
}




