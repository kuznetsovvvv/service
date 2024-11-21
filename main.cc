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
        try {
          if (req.method != crow::HTTPMethod::POST) {
            return crow::response(405, "Method Not Allowed");
          }

          auto json = crow::json::load(req.body);
          if (!json.has("age") || !json.has("gender") || !json.has("phone") ||
              !json.has("active")) {
            return crow::response(400, "Missing required fields");
          }

          int age = json["age"].i();
          string gender = json["gender"].s();
          string phone = json["phone"].s();
          string active = json["active"].s();

          db::courier courier1(1, age, gender, phone, active);

          postgres.add_courier(courier1);
          return crow::response(
              201,
              crow::json::wvalue(
                  {{"message", "Courier added"}}));  // Возвращаем JSON ответ
        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });
  // обновить статус курьера
  CROW_ROUTE(app, "/api/v1/couriers:id")
      .methods(crow::HTTPMethod::DELETE)([&postgres](const crow::request& req) {
        // парсинг json
        try {
          if (req.method != crow::HTTPMethod::DELETE) {
            return crow::response(405, "Method Not Allowed");
          }
          auto json = crow::json::load(req.body);
          if (!json.has("active")) {
            return crow::response(400, "Missing required fields");
          }

          int courier_id = json["courier_id"].i();
          string active = json["active"].s();
          db::courier courier1(courier_id, active);
          postgres.update_courier_status(courier1);
          return crow::response(
              201, crow::json::wvalue(
                       {{"message",
                         "Courier status updated"}}));  // Возвращаем JSON ответ

        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });

  // зарегать пользователя(добавить)
  CROW_ROUTE(app, "/api/v1/register")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        // парсинг json
        try {
          if (req.method != crow::HTTPMethod::POST) {
            return crow::response(405, "Method Not Allowed");
          }

          auto json = crow::json::load(req.body);
          if (!json.has("age") || !json.has("gender") || !json.has("phone")) {
            return crow::response(400, "Missing required fields");
          }

          int age = json["age"].i();
          string gender = json["gender"].s();
          string phone = json["phone"].s();

          db::user user1(1, age, gender, phone);

          postgres.add_user(user1);
          return crow::response(
              201, crow::json::wvalue({{"message", "User added"}}));
        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });

  // Отобразить инфо про пользователя
  CROW_ROUTE(app, "/api/v1/profile")
      .methods(crow::HTTPMethod::GET)([&postgres](const crow::request& req) {
        // парсинг json
        try {
          if (req.method != crow::HTTPMethod::GET) {
            return crow::response(405, "Method Not Allowed");
          }

          std::string phone;
          auto phone_param = req.url_params.get("phone");

          if (!phone_param) {
            return crow::response(400, "Missing 'phone' parameter");
          }

          phone = phone_param;

          db::user user1(1, phone);
          auto user_info = postgres.get_info_user(user1);
          if (user_info.has_value()) {
            crow::json::wvalue responce_data{};
            responce_data["user_id"] = user_info.value().get_user_id();
            responce_data["age"] = user_info.value().get_age();
            responce_data["gender"] = user_info.value().get_gender();
            responce_data["phone"] = user_info.value().get_phone();
            return crow::response(200, responce_data);
          } else {
            return crow::response(404, "User not found");
          }
        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });

  // проверить

  // изменить инфо о курьере
  CROW_ROUTE(app, "/api/v1/couriers:id")
      .methods(crow::HTTPMethod::PATCH)([&postgres](const crow::request& req) {
        try {
          if (req.method != crow::HTTPMethod::PATCH) {
            return crow::response(405, "Method Not Allowed");
          }
          auto json = crow::json::load(req.body);
          if (!json.has("courier_id") || !json.has("age") ||
              !json.has("gender") || !json.has("phone") ||
              !json.has("active")) {
            return crow::response(400, "Missing required fields");
          }
          int courier_id = json["courier_id"].i();
          int age = json["age"].i();
          string gender = json["gender"].s();
          string phone = json["phone"].s();
          string active = json["active"].s();
          db::courier courier1(courier_id, age, gender, phone, active);
          postgres.update_courier(courier1);
          return crow::response(
              201, crow::json::wvalue({{"message", "Courier updated"}}));
        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });

  // посмотреть инфо о продукте
  CROW_ROUTE(app, "/api/v1/product:id")
      .methods(crow::HTTPMethod::GET)([&postgres](const crow::request& req) {
        try {
          if (req.method != crow::HTTPMethod::GET) {
            return crow::response(405, "Method Not Allowed");
          }

          int product_id;
          auto id_param = req.url_params.get("product_id");

          if (!id_param) {
            return crow::response(400, "Missing 'product_id' parameter");
          }

          product_id = std::stoi(id_param);

          db::product product1(product_id);
          auto product_info = postgres.get_info_product(product1);
          if (product_info.has_value()) {
            crow::json::wvalue responce_data{};
            responce_data["product_id"] = product_info.value().get_product_id();
            responce_data["name"] = product_info.value().get_name();
            responce_data["price"] = product_info.value().get_price();
            responce_data["count"] = product_info.value().get_count();
            return crow::response(200, responce_data);
          } else {
            return crow::response(404, "Product not found");
          }
        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });

  // изменить данные о продукте
  CROW_ROUTE(app, "/api/v1/product:id")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        try {
          if (req.method != crow::HTTPMethod::POST) {
            return crow::response(405, "Method Not Allowed");
          }
          auto json = crow::json::load(req.body);
          if (!json.has("product_id") || !json.has("name") ||
              !json.has("price") || !json.has("count")) {
            return crow::response(400, "Missing required fields");
          }
          int product_id = json["product_id"].i();
          string name = json["name"].s();
          int price = json["price"].i();
          int count = json["count"].i();
          db::product product1(product_id, name, price, count);
          postgres.update_product(product1);
          return crow::response(
              201, crow::json::wvalue({{"message", "Product updated"}}));
        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });
 // удалить продукт
  CROW_ROUTE(app, "/api/v1/product:id")
      .methods(crow::HTTPMethod::PATCH)([&postgres](const crow::request& req) {
        try {
          if (req.method != crow::HTTPMethod::PATCH) {
            return crow::response(405, "Method Not Allowed");
          }
          auto json = crow::json::load(req.body);
          if (!json.has("product_id")) {
            return crow::response(400, "Missing required fields");
          }
          int product_id = json["product id"].i();
          db::product product1(product_id);
          postgres.update_product_status(product1);

          return crow::response(
              201, crow::json::wvalue({{"message", "Product deleted"}}));
        } catch (const exception& e) {
          return crow::response(500, "Internal Server Error");
        }
      });

  // Изменить информацию в профиле о пользователе
  CROW_ROUTE(app, "/api/v1/profile")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        // парсинг json
        try {
          if (req.method != crow::HTTPMethod::POST) {
            return crow::response(405, "Method Not Allowed");
          }
          auto json = crow::json::load(req.body);
          if (!json.has("age") || !json.has("gender") || !json.has("phone")) {
            return crow::response(400, "Missing required fields");
          }

          int user_id = json["user_id"].i();
          int age = json["age"].i();
          string gender = json["gender"].s();
          string phone = json["phone"].s();
          db::user user1(user_id, age, gender, phone);
          postgres.update_user(user1);

          return crow::response(
              201, crow::json::wvalue({{"message", "User data updated"}}));
        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });

  // Создать заказ
  CROW_ROUTE(app, "/api/v1/order")
      .methods(crow::HTTPMethod::POST)([&postgres](const crow::request& req) {
        // парсинг json
        try {
          if (req.method != crow::HTTPMethod::POST) {
            return crow::response(405, "Method Not Allowed");
          }
          auto json = crow::json::load(req.body);
          if (!json.has("delivery address") || !json.has("products")) {
            return crow::response(400, "Missing required fields");
          }
          // во фронте добавить проверку, что у нас будет не пустой заказ
          string delivery_address = json["delivery address"].s();
          const auto& products_json = json["products"];
          vector<int> products_ids;
            for (const auto& product_data : products_json) {
                products_ids.push_back(product_data.i());
            }

          db::order order1(0, delivery_address);
          postgres.add_order(order1);
          auto data = postgres.get_last_order(order1);
          if(!data.has_value()){
            return crow::response(500,"Error retrieving last order.");
          }
          auto& data_last = *data;
          for(size_t i=0;i<products_ids.size();i++){
          db::content content1(data_last.get_order_id(),products_ids[i]);
          postgres.add_content_item(content1);
          }
          return crow::response(
              201,
              crow::json::wvalue({{"message", "Order succesfully added"}}));

        } catch (const exception& e) {
          cerr << "Error: " << e.what() << endl;
          return crow::response(500, "Internal Server Error");
        }
      });


  // посмотреть инфо о заказе
 CROW_ROUTE(app, "/api/v1/order")
      .methods(crow::HTTPMethod::GET)([&postgres](const crow::request& req) {
    try {
      if (req.method != crow::HTTPMethod::GET) {
        return crow::response(405, "Method Not Allowed");
      }
      int order_id;
      auto id_param = req.url_params.get("order_id");
      if (!id_param) {
        return crow::response(400, "Missing 'product_id' parameter");
      }
      order_id = std::stoi(id_param);
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
         return crow::response(200, responce_data);
      }
       else{
        return crow::response(
            201, crow::json::wvalue({{"message", "Order not found"}}));
       }
      }
      catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return crow::response(500, "Internal Server Error");
      }
    });

















    app.port(18080).run();
    return 0;
}



void Guest() {
    std::cout << "1.Зарегистрироваться" << std::endl;  //+
    std::cout << "2.Залогиниться;" << std::endl;
}

void User() {
    std::cout << "1.Посмотреть ифнормацию о профиле" << std::endl;  //+
    std::cout << "2.Изменить ифнормацию в профиле;" << std::endl;   //+
    std::cout << "3.Посмотреть каталог товаров;" << std::endl;      //+
    std::cout << "4.Посмотреть информацию о товаре;" << std::endl;  //+
    std::cout << "5.Добавить товар в корзину;" << std::endl;
    std::cout << "6.Удалить товар из корзины;" << std::endl;
    std::cout << "7.Посмотреть корзину;" << std::endl;
    std::cout << "8.Сделать заказ;" << std::endl;                   //+
    std::cout << "9.Посмотреть информацию о заказе;" << std::endl;  //+
    std::cout << "10.Отменить заказ;" << std::endl;                 //+
}

void Admin() {
    std::cout << "1.Залогиниться" << std::endl;
    std::cout << "2.Добавить курьера;" << std::endl;               //+
    std::cout << "3.Уволить курьера;" << std::endl;                //+
    std::cout << "4.Изменить ифнормацию о курьере;" << std::endl;  //+
    std::cout << "5.Передать заказ курьеру;" << std::endl;         //+
    std::cout << "6.Добавить продукт;" << std::endl;               //+
    std::cout << "7.Изменить данные о продукте;" << std::endl;     //+
    std::cout << "8.Удалить продукт;" << std::endl;                //+
    std::cout << "9.Изменить статус заказа;" << std::endl;         //+
    std::cout << "10.Выйти из программы;" << std::endl;            //+
}
