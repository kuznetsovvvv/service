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

void Guest();
void User();
void Admin();

int main(int argc, char** argv) {
  auto vm = get_args(argc, argv);
  auto dsn = vm["pgdsn"].as<std::string>();
  
  DB postgres("postgres", dsn);









  // db::order order2("Kaluga, Svarshikov 73/2 763","accepted");
  // postgres.create_order(order2);

  //  db::user user1(2,30,"j","89814554598");
  //  db::order order1(7,"kalningrad Pyskhina street");
  //  db::user_action
  //  user_action1("cancel",order1.get_order_id(),user1.get_user_id());
  //  postgres.cancel_order(user_action1);

  // postgres.all_products();

  db::product product1(2);
  // postgres.update_product(product1);

  // db::courier courier1(1,30,"m","89834342","+");
  // postgres.update_courier(courier1);
  // db::user user1(1,50,"m","8987654335");
  // postgres.update_user(user1);

  std::cout << "Конец работы с базой данных" << std::endl;
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
