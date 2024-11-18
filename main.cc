#define CROW_USE_BOOST 1
#include <crow_all.h>
#include <stdio.h>

#include <optional> 
#include <boost/program_options.hpp>
#include <cctype>
#include <ctime>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <sstream>
#include <string>
#include <string_view>

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

    app.port(18080).run();

//Добавить курьера
 CROW_ROUTE(app, "/api/couriers")
 .methods(crow::HTTPMethod::OPTIONS)
 ([&postgres](const crow::request& req){
//парсинг json
try {
        if (req.method != crow::HTTPMethod::POST) {
            return crow::response(405, "Method Not Allowed");
        }

        auto json = crow::json::load(req.body);
        if(!json.has("age") || !json.has("gender") || !json.has("phone") || !json.has("active")){
            return crow::response(400, "Missing required fields");
        }

        int age = json["age"].i();
        string gender = json["gender"].s();
        string phone = json["phone"].s();
        string active = json["active"].s(); // Преобразуем в bool

        db::courier courier1(1, age, gender, phone, active); 

        postgres.add_courier(courier1);
        return crow::response(201, crow::json::wvalue( {{"message", "Courier added"}} )); // Возвращаем JSON ответ
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return crow::response(500, "Internal Server Error");
    }

    });



   























  // db::order order2("Kaluga, Svarshikov 73/2 763","accepted");
  // postgres.create_order(order2);

  //  db::user user1(2,30,"j","89814554598");
  //  db::order order1(7,"kalningrad Pyskhina street");
  //  db::user_action
  //  user_action1("cancel",order1.get_order_id(),user1.get_user_id());
  //  postgres.cancel_order(user_action1);

  // postgres.all_products();

  //db::product product1(2);
  // postgres.update_product(product1);

  // db::courier courier1(1,30,"m","89834342","+");
  // postgres.update_courier(courier1);
  // db::user user1(1,50,"m","8987654335");
  // postgres.update_user(user1);

  return 0;
}

void Guest();
void User();
void Admin();

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
