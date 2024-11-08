#include <boost/program_options.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <memory>
#include <cctype>
#include <ctime>
#include <sstream>
#include<stdio.h>
#include"functions.h"
#include"api_namespace.h"
#include"db_namespace.h"
namespace po = boost::program_options;
class InvalidArguments : std::exception {};
using namespace std;
auto get_args(int argc, char** argv) -> po::variables_map {
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "show help")
    ("pgdsn", po::value<std::string>(), "PostgreSQL DSN");
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


int main(int argc, char** argv){
  auto vm = get_args(argc, argv);
  auto dsn = vm["pgdsn"].as<std::string>();
  auto conn = establish_pgconnection(dsn);
  pqxx::work w(conn);
  auto row = w.exec1("SELECT 1");
  w.commit();
  //std::cout<<"Row result:"<< row[0].as<int>()<<std::endl;

  if(row[0].as<int>()==1){
  std::cout<<"Succesful database connection!"<<std::endl;
  }
  else{
  std::cout<<"Connection failed"<<std::endl;
  }


create_order(conn);
 

    // while (true) {
    //     Admin();
    //     std::string input = "";
    //     getline(std::cin, input);

    //     try {
    //         if (std::all_of(input.begin(), input.end(), ::isdigit)) {
    //             int choice = std::stoi(input);
    //             if (choice >= 1 && choice <= 10) {

    //                 if (choice == 1) {
//                     log(conn);
    //                 }
    //                 else if (choice == 2) {
     //                add_courier(conn);
    //                 }
    //                 else if (choice == 3) {
    //                 delete_courier(conn);
    //                 }
    //                 else if (choice == 4) {
    //                 update_courier(conn);
    //                 }
    //                 else if (choice == 5) {
    //                 transfer_order(conn);   
    //                 }
    //                 else if (choice == 6) {
  //                   add_product(conn);
    //                 }
    //                 else if (choice == 7) {
    //                 update_product(conn);
    //                 }
    //                 else if (choice == 8) {
//                     delete_product(conn);
    //                 }
    //                 else if (choice == 9) {
    //                 update_order(conn);
    //                 }
    //                 else if (choice == 10) {
    //                     break;  // Выход из цикла
    //                 }
    //             }
    //             else {
    //                 throw std::invalid_argument("Ошибка: Число должно быть в диапазоне от 1 до 10.");
    //             }
    //         }
    //         else {
    //             throw std::invalid_argument("Ошибка: Введите корректное числовое значение.");
    //         }

    //     }
    //     catch (const std::invalid_argument& e) {
    //         std::cout << e.what() << std::endl;
    //     }

    // }



  std::cout<<"Конец работы с базой данных"<<std::endl;
  return 0;
}


void Guest()
{
    std::cout << "1.Зарегистрироваться" << std::endl;
    std::cout << "2.Залогиниться;" << std::endl;
}

void User()
{
    std::cout << "1.Посмотреть ифнормацию о профиле" << std::endl;
     std::cout << "2.Изменить ифнормацию в профиле;" << std::endl;
    std::cout << "3.Посмотреть каталог товаров;" << std::endl;
     std::cout<< "4.Посмотреть информацию о товаре;" << std::endl;
     std::cout << "5.Добавить товар в корзину;" << std::endl;
    std::cout << "6.Удалить товар из корзины;" << std::endl;
     std::cout << "7.Посмотреть корзину;" << std::endl;
     std::cout << "8.Сделать заказ;" << std::endl;
     std::cout << "9.Посмотреть информацию о заказе;" << std::endl;
    std::cout<< "10.Отменить заказ;" << std::endl;
}

void Admin()
{
     std::cout << "1.Залогиниться" <<std::endl;
     std::cout << "2.Добавить курьера;" << std::endl;  //
     std::cout << "3.Уволить курьера;" << std::endl; //
     std::cout << "4.Изменить ифнормацию о курьере;" << std::endl; //
     std::cout << "5.Передать заказ курьеру;" << std::endl; //~
    std::cout << "6.Добавить продукт;" << std::endl; //
     std::cout << "7.Изменить данные о продукте;" <<std::endl; //
     std::cout << "8.Удалить продукт;" << std::endl; //
     std::cout << "9.Изменить статус заказа;" << std::endl;//~
     std::cout<<"10.Выйти из программы;"<<std::endl; //
}
