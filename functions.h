#pragma once
#include <boost/program_options.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <memory>
#include <cctype>
#include<stdio.h>
using namespace std;



void add_courier(pqxx::connection& conn){

 int age;

    do {
        cout << "Сколько вам лет: ";
        cin >> age;

        if (cin.fail()) {
            cout << "Введите число!" << endl;
            cin.clear(); // Очистить состояние потока
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Пропустить оставшиеся символы в потоке
        } else if (age < 0 || age > 100) {
            cout << "Невозможное число лет!\n";
        } 
    } while (age < 0 || age > 100);
 string gender;
    do {
        cout << "Введите пол (m/f): ";
        cin >> gender;

        if (gender != "m" && gender != "f") {
            cout << "Некорректный ввод пола! Введите 'm' или 'f'." << endl;
        }
    } while (gender != "m" && gender != "f");

    // Проверка ввода телефона (длина 11 символов)
    string phone;
    do {
        cout << "Введите номер телефона (11 цифр): ";
        cin >> phone;

        if (phone.length() != 11) {
            cout << "Номер телефона должен содержать 11 цифр!" << endl;
        }
    } while (phone.length() != 11);

    // Проверка ввода активного состояния (должен быть "+" или "-")
    string active;
    do {
        cout << "Введите активное состояние (+/-): ";
        cin >> active;

        if (active != "+" && active != "-") {
            cout << "Некорректный ввод! Введите '+' или '-'." << endl;
        }
    } while (active != "+" && active != "-");

    // Добавление элемента в таблицу
try {
    pqxx::work ad_courier(conn);
    std::string age_str = std::to_string(age); 
         ad_courier.exec_params(
        "INSERT INTO couriers (courier_id, age, gender, phone, active) VALUES (DEFAULT, $1, $2, $3, $4)", 
        age_str, 
        gender, 
        phone, 
        active
    );
        ad_courier.commit();
        cout<<"Курьер успешно добавлен!"<<endl;
}
catch (const std::exception &e) 
{
        std::cerr << e.what() << std::endl;
}
}

void update_courier(pqxx::connection& conn){
    int courier_id;
    int choice;
    string input;
    string input_courierid;
    // Ввод и проверка ID курьера
 
   
             do {
                 cout << "Введите корректный ID курьера (неотрицательное число): ";
                 cin >> input_courierid;

                  // Проверка на корректный ввод возраста (числовое значение)
                  if (all_of(input_courierid.begin(), input_courierid.end(), ::isdigit)) {
                     courier_id = stoi(input_courierid);
                     if (courier_id < 0 ) {
                     cout << "Неверный выбор,корректный ID курьера!\n";
                     } else {
                         break;
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear();
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                      }
              } while (true);





    try {
        // Проверка существования курьера
       pqxx::work check(conn);
        pqxx::result result = check.exec_params("SELECT 1 FROM couriers WHERE courier_id=$1", courier_id); 
        check.commit();
        if (result.empty()) {
            cout << "Курьера с таким ID не существует!" << endl;
            return;
        }
        else
        {
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
        pqxx::work update_data(conn);
        update_data.exec_params(
            "UPDATE couriers SET age=$1 WHERE courier_id=$2",new_age,courier_id
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
        pqxx::work update_data(conn);
        update_data.exec_params(
               "UPDATE couriers SET phone=$1 WHERE courier_id=$2",new_number,courier_id
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
        pqxx::work update_data(conn);
        update_data.exec_params(
            "UPDATE couriers SET active=$1 WHERE courier_id=$2",new_active,courier_id
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

void delete_courier(pqxx::connection& conn) {
    int courier_id;
    string input_courierid;
    // Ввод и проверка ID курьера
      do {
                 cout << "Введите корректный ID курьера (неотрицательное число):\n";
                 cin >> input_courierid;

                  // Проверка на корректный ввод возраста (числовое значение)
                  if (all_of(input_courierid.begin(), input_courierid.end(), ::isdigit)) {
                     courier_id = stoi(input_courierid);
                     if (courier_id < 0 ) {
                     cout << "Неверный выбор,корректный ID курьера!\n";
                     } else {
                         break;
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear();
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                      }
              } while (true);

    try {
        // Проверка существования курьера
       pqxx::work check(conn);
        pqxx::result result = check.exec_params("SELECT 1 FROM couriers WHERE courier_id=$1", courier_id); 
        check.commit();
        if (result.empty()) {
            cout << "Курьера с таким ID не существует!" << endl;
            return;
        }
else{
        // Удаление курьера
        pqxx::work delete_cour(conn);
        delete_cour.exec_params("DELETE FROM couriers WHERE courier_id=$1", courier_id);
        delete_cour.commit();
        cout << "Курьер c ID: "<<courier_id<< " успешно удалён!" << endl;
}
    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}

void add_product(pqxx::connection& conn){




string name;
int price,count;
std::string input = "";

    try {

        cout << "Введите название продукта: "<<endl;;
        getline(cin,name);

        do {
             cout << "Введите цену продукта: "<<endl;
             getline(std::cin, input);
             if (std::all_of(input.begin(), input.end(), ::isdigit)) {
             price = std::stoi(input);
             }
             else{
             cout << "Цена некорректная!"<<endl;
             }
        }while(price<=0 || price>100000);
        input="";
        do {
             cout << "Количество единиц товара: "<<endl;
             getline(std::cin, input);
             if (std::all_of(input.begin(), input.end(), ::isdigit)) {
             count = std::stoi(input);
             }
             else{
             cout << "Некорректное количество товаров!"<<endl;
             }
        }while(count<=0 || count>100000);



    }
    catch (const std::exception &e) 
    {
    std::cerr << e.what() << std::endl;
    }

    // Добавление элемента в таблицу
try {
    pqxx::work ad_product(conn);
         ad_product.exec_params(
        "INSERT INTO products (product_id, name, price, count) VALUES (DEFAULT, $1, $2, $3)", 
        name, 
        price, 
        count 
    );
        ad_product.commit();
        cout<<"Продукт успешно добавлен!"<<endl;
}
catch (const std::exception &e) 
{
        std::cerr << e.what() << std::endl;
}




 }

void update_product(pqxx::connection& conn){
    int product_id;
    int choice;
    string input;
    string input_productid;
    // Ввод и проверка ID курьера
 
   
             do {
                 cout << "Введите корректный ID продукта (неотрицательное число):\n";
                 cin >> input_productid;

                  // Проверка на корректный ввод возраста (числовое значение)
                  if (all_of(input_productid.begin(), input_productid.end(), ::isdigit)) {
                     product_id = stoi(input_productid);
                     if (product_id < 0 ) {
                     cout << "Неверный выбор,корректный ID продукта!\n";
                     } else {
                         break;
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear();
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                      }
              } while (true);

 try {
        // Проверка существования курьера
        pqxx::work check(conn);
        pqxx::result result = check.exec_params("SELECT 1 FROM products WHERE product_id=$1", product_id); 
        check.commit();
        if (result.empty()) {
            cout << "Продукт с таким ID не существует!" << endl;
            return;
        }
        else
        {
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
        pqxx::work update_data(conn);
        update_data.exec_params(
            "UPDATE products SET name=$1 WHERE product_id=$2",new_name,product_id
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
        pqxx::work update_data(conn);
        update_data.exec_params(
               "UPDATE products SET price=$1 WHERE product_id=$2",new_price,product_id
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
        pqxx::work update_data(conn);
        update_data.exec_params(
            "UPDATE products SET count=$1 WHERE product_id=$2",new_count,product_id
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

void delete_product(pqxx::connection& conn){
    int product_id;
    string input_productid;
 // Ввод и проверка ID продукта
   do {
                 cout << "Введите корректный ID продукта (неотрицательное число):\n ";
                 cin >> input_productid;

                  if (all_of(input_productid.begin(), input_productid.end(), ::isdigit)) {
                     product_id = stoi(input_productid);
                     if (product_id < 0 ) {
                     cout << "Неверный выбор,корректный ID продукта!\n";
                     } else {
                         break;
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear();
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                      }
              } while (true);


 try {
        // Проверка существования продукта
       pqxx::work check(conn);
        pqxx::result result = check.exec_params("SELECT 1 FROM products WHERE product_id=$1",product_id); 
        check.commit();
        if (result.empty()) {
            cout << "Продукта с таким ID не существует!" << endl;
            return;
        }
else{
        // Удаление продукта
        pqxx::work delete_prod(conn);
        delete_prod.exec_params("DELETE FROM products WHERE product_id=$1", product_id);
        delete_prod.commit();
        cout << "Продукт c ID: "<<product_id<<" успешно удалён!" << endl;
}
    }
    catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }




}









// Заказ создается только один раз
void create_order(pqxx::connection& conn){
string delivery_address;
string status="accepted";

  time_t rawtime;
  struct tm * timeinfo;
  time( &rawtime );                         // текущая дата, выраженная в секундах
  timeinfo = localtime ( &rawtime );        // текущая дата, представленная в нормальной форме
  string time=asctime (timeinfo);


        cout << "Введите адрес заказа(до 80 символов): ";
        getline(cin,delivery_address);
        cout<<"Нажминет Enter для создания:";
      cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

   
    // Добавление элемента в таблицу
try {
    pqxx::work createOrder(conn);
         createOrder.exec_params(
        "INSERT INTO orders (order_id, creation_time, time, delivery_address, status) VALUES (DEFAULT, $1, $2, $3, $4)", 
        time, 
        time, 
        delivery_address, 
        status
    );
        createOrder.commit();
        cout<<"Заказ создан!"<<endl;
}
catch (const std::exception &e) 
{
        std::cerr << e.what() << std::endl;
}


}




void transfer_order(pqxx::connection& conn)
{


}

//ток статус и время // time and status
void update_status_order(pqxx::connection& conn)
{
 time_t rawtime;
  struct tm * timeinfo;
  time( &rawtime );                         // текущая дата, выраженная в секундах
  timeinfo = localtime ( &rawtime );        // текущая дата, представленная в нормальной форме
  string time=asctime (timeinfo);

string input_productid;
int product_id


             do {
                 cout << "Введите корректный ID курьера (неотрицательное число): ";
                 cin >> input_courierid;

                  // Проверка на корректный ввод возраста (числовое значение)
                  if (all_of(input_courierid.begin(), input_courierid.end(), ::isdigit)) {
                     courier_id = stoi(input_courierid);
                     if (courier_id < 0 ) {
                     cout << "Неверный выбор,корректный ID курьера!\n";
                     } else {
                         break;
                     }
                     } else {
                     cout << "Введите число!" << endl;
                      cin.clear();
                      cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                      }
              } while (true);

 try {
        // Проверка существования курьера
       pqxx::work check(conn);
        pqxx::result result = check.exec_params("SELECT 1 FROM couriers WHERE courier_id=$1", courier_id); 
        check.commit();
        if (result.empty()) {
            cout << "Курьера с таким ID не существует!" << endl;
            return;
        }
        else
        {
            
             
             do {
                cout << "Введите статус заказа: ";
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

        }


    }
catch (const pqxx::sql_error &e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

            

}