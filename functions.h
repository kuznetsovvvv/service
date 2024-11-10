#pragma once
#include <boost/program_options.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <memory>
#include <cctype>
#include<stdio.h>
#include"api_namespace.h"
#include"db_namespace.h"
using namespace db;
using namespace api;
using namespace std;


class DB{
private:
string  DB_NAME;
shared_ptr<pqxx::connection> conn;
public:
DB(const string& db_name, const string& dsn)  {
DB_NAME=db_name;
conn = shared_ptr<pqxx::connection>(new pqxx::connection(dsn.data()));


pqxx::work w(*conn);
  auto row = w.exec1("SELECT 1");
  w.commit();

  if(row[0].as<int>()==1){
  std::cout<<"Succesful database connection!"<<std::endl;
  }
  else{
  std::cout<<"Connection failed"<<std::endl;
  }
}

void add_courier(db::courier& obj){

    // Добавление элемента в таблицу
try {
    pqxx::work ad_courier(*conn); 
         ad_courier.exec_params(
        "INSERT INTO couriers (courier_id, age, gender, phone, active) VALUES (DEFAULT, $1, $2, $3, $4)", 
        obj.get_age(), 
        obj.get_gender(), 
        obj.get_phone(), 
        obj.get_active()
    );
        ad_courier.commit();
        cout<<"Курьер успешно добавлен!"<<endl;
}
catch (const std::exception &e) 
{
        std::cerr << e.what() << std::endl;
}
}

void update_courier(db::courier& obj){
    int choice;
    string input;
    string input_courierid;



    try {
       pqxx::work check(*conn);
       pqxx::result result = check.exec_params("SELECT EXISTS (SELECT 1 FROM couriers WHERE courier_id = $1)", obj.get_courier_id()); 
       check.commit();
            if (!result.empty() && result[0][0].as<bool>()) {
            cout<<"Что вы хотите изменить?\n"
             <<"1.Возраст\n"
             <<"2.Номер телефона\n"
             <<"3.Активность\n";
             
             do {
                cout << "Введите свой выбор: ";
                 cin >> input;

                  // Проверка на корректный ввод возраста (числовое значение)
                  if (all_of(input.begin(), input.end(), ::isdigit)) {
                     choice = stoi(input);
                     if (choice < 0 || choice > 3) {
                     cout << "Неверный выбор,выберите цифру!\n";
                     } else {
                         break; // Выйти из цикла, если ввод корректен
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear(); // Очистить состояние потока
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Пропустить оставшиеся символы в потоке
                      }
                } while (true);



            }    else {
             cout << "Курьер с ID"<<obj.get_courier_id()<<" не существует!" << endl;
             return;
            } 
      


    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }



if(choice==1){
int new_age;
cout<<"Введите новый возраст:"<<endl;
cin>>new_age;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
            "UPDATE couriers SET age=$1 WHERE courier_id=$2",new_age,obj.get_courier_id()
        );
        update_data.commit();
        cout<<"Данные о курьере обновлены!"<<endl;
    }
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }
}
else if(choice==2){
    string new_number;
    cout<<"Введите новый номер телефона(11 цифр):"<<endl;
    cin>>new_number;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
               "UPDATE couriers SET phone=$1 WHERE courier_id=$2",new_number,obj.get_courier_id()
         );
         update_data.commit();
         cout<<"Данные о курьере обновлены!"<<endl;
    }
    catch (const std::exception &e) 
     {
        std::cerr << e.what() << std::endl;
     }

}
else if(choice ==3){
    string new_active;
    cout<<"Введите новую активность(+/-):"<<endl;
    cin>>new_active;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
            "UPDATE couriers SET active=$1 WHERE courier_id=$2",new_active,obj.get_courier_id()
        );
        update_data.commit();
        cout<<"Данные о курьере обновлены!"<<endl;
    }
     catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }


}


}

void delete_courier(db::courier& obj) {

   

    try {
        // Проверка существования курьера
       pqxx::work check(*conn);
        pqxx::result result = check.exec_params("SELECT 1 FROM couriers WHERE courier_id=$1", obj.get_courier_id()); 
        check.commit();
        if (result.empty()) {
            cout << "Курьера с таким ID не существует!" << endl;
            return;
        }
else{
        // Удаление курьера
        pqxx::work delete_cour(*conn);
        delete_cour.exec_params("DELETE FROM couriers WHERE courier_id=$1", obj.get_courier_id());
        delete_cour.commit();
        cout << "Курьер c ID: "<<obj.get_courier_id()<< " успешно удалён!" << endl;
}
    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void add_product(db::product& obj){
    // Добавление элемента в таблицу
     try {
        pqxx::work ad_product(*conn);
            ad_product.exec_params(
          "INSERT INTO products (product_id, name, price, count) VALUES (DEFAULT, $1, $2, $3)", 
          obj.get_name(), 
          obj.get_price(), 
          obj.get_count() 
          );
          ad_product.commit();
          cout<<"Продукт успешно добавлен!"<<endl;
        }
        catch (const std::exception &e) 
        {
           std::cerr << e.what() << std::endl;
        }

}

void update_product(db::product& obj){
    int choice;
    string input;
    string input_productid;

 try {
        // Проверка существования курьера
        pqxx::work check(*conn);
        pqxx::result result = check.exec_params("SELECT EXISTS (SELECT 1 FROM products WHERE product_id = $1)", obj.get_product_id()); 
        check.commit();
       if (!result.empty() && result[0][0].as<bool>()) {
             cout<<"Что вы хотите изменить?\n"
             <<"1.Имя\n"
             <<"2.Цена\n"
             <<"3.Количество\n";
             
             do {
                cout << "Введите свой выбор: ";
                 cin >> input;

                  // Проверка на корректный ввод возраста (числовое значение)
                  if (all_of(input.begin(), input.end(), ::isdigit)) {
                     choice = stoi(input);
                     if (choice < 0 || choice > 3) {
                     cout << "Неверный выбор,выберите цифру!\n";
                     } else {
                         break; // Выйти из цикла, если ввод корректен
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear(); // Очистить состояние потока
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Пропустить оставшиеся символы в потоке
                      }
             } while (true);
         } else {
             cout << "Продукт с таким ID не существует!" << endl;
              return;
            } 
    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }



if(choice==1){
string new_name;
cout<<"Введите новое имя:"<<endl;
cin>>new_name;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
            "UPDATE products SET name=$1 WHERE product_id=$2",new_name,obj.get_product_id()
        );
        update_data.commit();
        cout<<"Данные о продукте обновлены!"<<endl;
    }
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }
}
else if(choice==2){
    int new_price;
    cout<<"Введите новую цену:"<<endl;
    cin>>new_price;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
               "UPDATE products SET price=$1 WHERE product_id=$2",new_price,obj.get_product_id()
         );
         update_data.commit();
         cout<<"Данные о продукте обновлены!"<<endl;
    }
    catch (const std::exception &e) 
     {
        std::cerr << e.what() << std::endl;
     }
}
else if(choice ==3){
    int new_count;
    cout<<"Введите новое количество:"<<endl;
    cin>>new_count;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
            "UPDATE products SET count=$1 WHERE product_id=$2",new_count,obj.get_product_id()
        );
        update_data.commit();
        cout<<"Данные о продукте обновлены!"<<endl;
    }
     catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }
}

}

void delete_product(db::product& obj){

    try {
        // Проверка существования продукта
        pqxx::work check(*conn);
        pqxx::result result = check.exec_params("SELECT 1 FROM products WHERE product_id=$1",obj.get_product_id()); 
        check.commit();
        if (result.empty()) {
            cout << "Продукта с таким ID не существует!" << endl;
            return;
        }
        else{
        // Удаление продукта
            pqxx::work delete_prod(*conn);
            delete_prod.exec_params("DELETE FROM products WHERE product_id=$1", obj.get_product_id());
            delete_prod.commit();
            cout << "Продукт c ID: "<<obj.get_product_id()<<" успешно удалён!" << endl;
        }
    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

}

void check_product(db::product& obj){
    
    try{
        pqxx::work check(*conn);
        pqxx::result result = check.exec_params("SELECT EXISTS (SELECT 1 FROM products WHERE name = $1)", obj.get_name()); 
        check.commit();
         if (!result.empty() && result[0][0].as<bool>()) {
            pqxx::work get(*conn);
            pqxx::result res = get.exec_params("SELECT * FROM products WHERE name=$1",obj.get_name());
            get.commit();
            for (const auto& row : res) {
                cout << "ID: " << row[0].as<int>() << ", Name: " << row[1].as<string>() << ", Price: "<<row[2].as<int>()<<", Count: "<<row[3].as<int>()<<endl; 

            }
       }
       else{
        std::cout<<"Продукта с именем "<<obj.get_name()<<" не существует!"<<std::endl;
       }

    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch(std::exception& e){
        cout<<e.what()<<endl;
    }
   


}

// Заказ создается только один раз
void create_order(db::order& obj){
  time_t rawtime;
  struct tm * timeinfo;
  time( &rawtime );                       
  timeinfo = localtime ( &rawtime );       
  std::string time=asctime (timeinfo);
    try {
          pqxx::work createOrder(*conn);
          createOrder.exec_params(
          "INSERT INTO orders (order_id, creation_time, time, delivery_address, status) VALUES (DEFAULT, $1, $2, $3, $4)", 
           time, 
           time, 
           obj.get_delivery_address(), 
           "accepted"
          );
        createOrder.commit();
        cout<<"Заказ создан!"<<endl;
    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch(std::exception& e){
        cout<<e.what()<<endl;
    }
}
//время на час раньше почему-то

void transfer_order(db::courier_action& obj)
{


    try {
        pqxx::work transfer(*conn); 
        transfer.exec_params(
        "INSERT INTO courier_actions (action, order_id, courier_id) VALUES ($1, $2, $3)", 
        obj.get_action(),
        obj.get_order_id(),
        obj.get_courier_id()
         );
        transfer.commit();
        cout<<"Создана запись о передаче заказа c ID "<<obj.get_order_id()<<" курьеру с ID "<<obj.get_courier_id()<<endl;
    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch(std::exception& e){
        cout<<e.what()<<endl;
    }


}

void update_status_order(db::order& obj)
{
  time_t rawtime;
  struct tm * timeinfo;
  time( &rawtime );                         // текущая дата, выраженная в секундах
  timeinfo = localtime ( &rawtime );        // текущая дата, представленная в нормальной форме
  string time=asctime (timeinfo);

    try {
        // Проверка существования курьера
        pqxx::work check(*conn);
        pqxx::result result = check.exec_params("SELECT EXISTS (SELECT 1 FROM orders WHERE order_id = $1)", obj.get_order_id()); 
        check.commit();

            if (!result.empty() && result[0][0].as<bool>()) {
                pqxx::work update_data(*conn);
                update_data.exec_params(
                "UPDATE orders SET time=$1, status=$2 WHERE order_id=$3",time,obj.get_status(), obj.get_order_id()
                 );
               update_data.commit();
              cout<<"Статус заказа с ID "<<obj.get_order_id()<<" обновлён!"<<endl;
            } else {
            cout << "Заказ с таким ID не существует!" << endl;
            return;
            } 

    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }     

}

void check_order(db::order& obj){
    try {
         pqxx::work check(*conn);
         pqxx::result result = check.exec_params("SELECT EXISTS (SELECT 1 FROM orders WHERE order_id = $1)", obj.get_order_id()); 
         check.commit();
         if (!result.empty() && result[0][0].as<bool>()) {
            pqxx::work get(*conn);
            pqxx::result res = get.exec_params("SELECT * FROM orders WHERE order_id=$1",obj.get_order_id());
            get.commit();
            for (const auto& row : res) {
                cout << "ID: " << row[0].as<int>() << ", Creation time: " << row[1].as<string>() << ", Time: "<<row[2].as<string>()<<", Delivery address: "<<row[3].as<string>()<<", Status: "<<row[4].as<string>()<<endl; 
            }
         }
         else{
            std::cout<<"Заказа с ID: "<<obj.get_order_id()<<" не существует!"<<std::endl;
         }
    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch(const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }
}

void add_user(db::user& obj){

    try {
         pqxx::work ad_user(*conn); 
         ad_user.exec_params(
        "INSERT INTO users (user_id, age, gender, phone) VALUES (DEFAULT, $1, $2, $3)", 
        obj.get_age(), 
        obj.get_gender(), 
        obj.get_phone()
        );
        ad_user.commit();
        cout<<"Пользователь успешно добавлен!"<<endl;
    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }

}

void check_user(db::user& obj){

    try {
        pqxx::work check(*conn);
        pqxx::result result = check.exec_params("SELECT EXISTS (SELECT 1 FROM users WHERE user_id = $1)", obj.get_user_id()); 
        check.commit();
         if (!result.empty() && result[0][0].as<bool>()) {
            pqxx::work get(*conn);
            pqxx::result res = get.exec_params("SELECT * FROM users WHERE user_id=$1",obj.get_user_id());
            get.commit();
            for (const auto& row : res) {
                cout << "ID: " << row[0].as<int>() << ", Age: " << row[1].as<int>() << ", Gender: "<<row[2].as<string>()<<", Phone: "<<row[3].as<string>()<<endl; 
            }
       }
       else{
        std::cout<<"Пользователя с ID "<<obj.get_user_id()<<" не существует!"<<std::endl;
       }
    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }   

}

void update_user(db::user& obj){
    int choice;
    string input;
    string input_userid;
 try {
       pqxx::work check(*conn);
       pqxx::result result = check.exec_params("SELECT EXISTS (SELECT 1 FROM users WHERE user_id = $1)", obj.get_user_id()); 
       check.commit();
            if (!result.empty() && result[0][0].as<bool>()) {
            cout<<"Что вы хотите изменить?\n"
             <<"1.Возраст\n"
             <<"2.Номер телефона\n";
             
             do {
                cout << "Введите свой выбор: ";
                 cin >> input;

                  // Проверка на корректный ввод возраста (числовое значение)
                  if (all_of(input.begin(), input.end(), ::isdigit)) {
                     choice = stoi(input);
                     if (choice < 0 || choice > 2) {
                     cout << "Неверный выбор,выберите цифру!\n";
                     } else {
                         break; // Выйти из цикла, если ввод корректен
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear(); // Очистить состояние потока
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Пропустить оставшиеся символы в потоке
                      }
                } while (true);
            }else {
             cout << "Пользоавтель с ID"<<obj.get_user_id()<<" не существует!" << endl;
             return;
            } 
      
    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

if(choice==1){
int new_age;
cout<<"Введите новый возраст:"<<endl;
cin>>new_age;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
            "UPDATE users SET age=$1 WHERE user_id=$2",new_age,obj.get_user_id()
        );
        update_data.commit();
        cout<<"Данные о пользователе обновлены!"<<endl;
    }
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }
}
else if(choice==2){
    string new_number;
    cout<<"Введите новый номер телефона(11 цифр):"<<endl;
    cin>>new_number;
    try {
        pqxx::work update_data(*conn);
        update_data.exec_params(
               "UPDATE users SET phone=$1 WHERE user_id=$2",new_number,obj.get_user_id()
         );
         update_data.commit();
         cout<<"Данные о пользователе обновлены!"<<endl;
    }
    catch (const std::exception &e) 
     {
        std::cerr << e.what() << std::endl;
     }

}


}

void add_order_action(db::user_action& obj){
     try {
        pqxx::work transfer(*conn); 
        transfer.exec_params(
        "INSERT INTO user_actions (action, order_id, user_id) VALUES ($1, $2, $3)", 
        obj.get_action(),
        obj.get_order_id(),
        obj.get_user_id()
         );
        transfer.commit();
        cout<<"Создана запись о создании заказа c ID "<<obj.get_order_id()<<" пользователем с ID "<<obj.get_user_id()<<endl;
    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch(std::exception& e){
        cout<<e.what()<<endl;
    }
}

void cancel_order(db::user_action& obj){
 try {
        pqxx::work transfer(*conn); 
        transfer.exec_params(
        "INSERT INTO user_actions (action, order_id, user_id) VALUES ($1, $2, $3)", 
        obj.get_action(),
        obj.get_order_id(),
        obj.get_user_id()
         );
        transfer.commit();
        cout<<"Создана запись об отмене заказа c ID "<<obj.get_order_id()<<" пользователем с ID "<<obj.get_user_id()<<endl;
    }
    catch (const pqxx::sql_error &e) {
         std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch(std::exception& e){
        cout<<e.what()<<endl;
    }

}









};