#pragma once
#include <stdio.h>

#include <boost/program_options.hpp>
#include <cctype>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <string_view>

#include "api_namespace.h"
#include "db_namespace.h"
using namespace db;
using namespace api;
using namespace std;

class DB {
 private:
  string DB_NAME;
  shared_ptr<pqxx::connection> conn;

 public:
  DB(const string &db_name, const string &dsn) {
    DB_NAME = db_name;
    conn = shared_ptr<pqxx::connection>(new pqxx::connection(dsn.data()));
  }

  void add_courier(db::courier &obj) {
    // Добавление элемента в таблицу
    try {
      pqxx::work ad_courier(*conn);
      ad_courier.exec_params(
          "INSERT INTO couriers (courier_id, age, gender, phone, active) "
          "VALUES (DEFAULT, $1, $2, $3, $4)",
          obj.get_age(), obj.get_gender(), obj.get_phone(), obj.get_active());
      ad_courier.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }
  //
  void add_order(db::order &obj) {
    try {
      pqxx::work createOrder(*conn);
      createOrder.exec_params(
          "INSERT INTO orders (order_id, creation_time, time, "
          "delivery_address, status) VALUES (DEFAULT, CURRENT_TIMESTAMP(2), "
          "CURRENT_TIMESTAMP(2), $1, $2)",
          obj.get_delivery_address(), "accepted");
      createOrder.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void add_product(db::product &obj) {
    // Добавление элемента в таблицу
    try {
      pqxx::work ad_product(*conn);
      ad_product.exec_params(
          "INSERT INTO products (product_id, name, price, count) VALUES "
          "(DEFAULT, $1, $2, $3)",
          obj.get_name(), obj.get_price(), obj.get_count());
      ad_product.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void add_user(db::user &obj) {
    try {
      pqxx::work ad_user(*conn);
      ad_user.exec_params(
          "INSERT INTO users (user_id, age, gender, phone) VALUES (DEFAULT, "
          "$1, $2, $3)",
          obj.get_age(), obj.get_gender(), obj.get_phone());
      ad_user.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void add_courier_action(db::courier_action &obj) {
    try {
      pqxx::work transfer(*conn);
      transfer.exec_params(
          "INSERT INTO courier_actions (action, order_id, courier_id) VALUES "
          "($1, $2, $3)",
          obj.get_action(), obj.get_order_id(), obj.get_courier_id());
      transfer.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void add_user_action(db::user_action &obj) {
    try {
      pqxx::work transfer(*conn);
      transfer.exec_params(
          "INSERT INTO user_actions (action, order_id, user_id) VALUES ($1, "
          "$2, $3)",
          obj.get_action(), obj.get_order_id(), obj.get_user_id());
      transfer.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void add_content_item(db::content &obj) {
    try {
      pqxx::work transfer(*conn);
      transfer.exec_params(
          "INSERT INTO content (order_id, product_id) VALUES ($1, "
          "$2)",
          obj.get_order_id(), obj.get_product_id());
      transfer.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void update_courier(db::courier &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM couriers WHERE courier_id = $1)",
          obj.get_courier_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work update(*conn);
        pqxx::result res = update.exec_params(
            "UPDATE couriers SET age=$1, phone=$2, active=$3 WHERE "
            "courier_id=$4",
            obj.get_age(), obj.get_phone(), obj.get_active(),
            obj.get_courier_id());
        update.commit();
      }

    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }

  void update_courier_status(db::courier &obj) {
    try {
      // Проверка существования курьера
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM couriers WHERE courier_id = $1)",
          obj.get_courier_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        return;
      } else {
        // Удаление курьера
        pqxx::work delete_cour(*conn);
        delete_cour.exec_params(
            "UPDATE couriers SET status=$1 WHERE courier_id=$2",
            obj.get_active(), obj.get_courier_id());
        delete_cour.commit();
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void update_product(db::product &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM products WHERE product_id = $1)",
          obj.get_product_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work update(*conn);
        pqxx::result res = update.exec_params(
            "UPDATE products SET name=$1, price=$2, count=$3 WHERE "
            "product_id=$4",
            obj.get_name(), obj.get_price(), obj.get_count(),
            obj.get_product_id());
        update.commit();
      } else {
        return;
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }

  void update_product_status(db::product &obj) {
    try {
      // Проверка существования продукта
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM products WHERE product_id = $1)",
          obj.get_name());
      check.commit();
      if (result[0][0].as<bool>()) {
        return;
      } else {
        // Удаление продукта
        pqxx::work delete_prod(*conn);
        delete_prod.exec_params(
            "UPDATE products SET count=0 WHERE product_id=$1",
            obj.get_product_id());
        delete_prod.commit();
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }

  void update_user(db::user &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM users WHERE user_id = $1)",
          obj.get_user_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work update(*conn);
        pqxx::result res = update.exec_params(
            "UPDATE users SET age=$1, phone=$2 WHERE user_id=$3", obj.get_age(),
            obj.get_phone(), obj.get_user_id());
        update.commit();
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }

  void update_order_status(db::order &obj) {
    try {
      // Проверка существования курьера
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM orders WHERE order_id = $1)",
          obj.get_order_id());
      check.commit();

      if (result[0][0].as<bool>()) {
        pqxx::work update_data(*conn);
        update_data.exec_params(
            "UPDATE orders SET time=CURRENT_TIMESTAMP(2), status=$1 WHERE "
            "order_id=$2",
            obj.get_status(), obj.get_order_id());
        update_data.commit();
      } else {
        return;
      }

    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void get_info_courier(db::courier &obj) {
    try {
      // Проверка существования курьера
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM couriers WHERE courier_id = $1)",
          obj.get_courier_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work get_info(*conn);
        pqxx::result result =
            get_info.exec_params("SELECT * FROM couriers WHERE courier_id = $1",
                                 obj.get_courier_id());
        get_info.commit();
      } else {
        return;
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }

  void get_info_product(db::product &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM products WHERE name = $1)",
          obj.get_name());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work get(*conn);
        pqxx::result res = get.exec_params(
            "SELECT * FROM products WHERE name=$1", obj.get_name());
        get.commit();
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (std::exception &e) {
      cout << e.what() << endl;
    }
  }

  void get_info_user(db::user &obj) {
    try {
      // Проверка существования курьера
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM users WHERE user_id = $1)",
          obj.get_user_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work get_info(*conn);
        pqxx::result result = get_info.exec_params(
            "SELECT * FROM users WHERE user_id = $1", obj.get_user_id());
        get_info.commit();
      } else {
        return;
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }

  void get_info_order(db::order &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM orders WHERE order_id = $1)",
          obj.get_order_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work get_info(*conn);
        pqxx::result result = get_info.exec_params(
            "SELECT * FROM orders WHERE order_id = $1", obj.get_order_id());
        get_info.commit();
      } else {
        return;
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Ошибка: " << e.what() << std::endl;
    }
  }

  void get_info_all_products() {
    try {
      pqxx::work get(*conn);
      pqxx::result res = get.exec_params("SELECT * FROM products ");
      get.commit();
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void get_info_user_action(db::user_action &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM user_actions WHERE order_id = $1 AND "
          "user_id = $2)",
          obj.get_order_id(), obj.get_user_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work get_info(*conn);
        pqxx::result result = get_info.exec_params(
            "SELECT * FROM user_actions WHERE order_id = $1 AND "
            "user_id = $2",
            obj.get_order_id());
        get_info.commit();
      } else {
        return;
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void get_info_courier_action(db::courier_action &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM courier_actions WHERE order_id = $1 "
          "AND "
          "courier_id = $2)",
          obj.get_order_id(), obj.get_courier_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work get_info(*conn);
        pqxx::result result = get_info.exec_params(
            "SELECT * FROM user_actions WHERE order_id = $1 AND "
            "courier_id = $2",
            obj.get_order_id());
        get_info.commit();
      } else {
        return;
      }
    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }

  void get_orders(db::user &obj) {
    try {
      pqxx::work get_orders(*conn);
      pqxx::result result = get_orders.exec_params(
          "SELECT * FROM orders WHERE order_id IN (SELECT order_id FROM "
          "user_actions WHERE user_id = $1)",
          obj.get_user_id());
      get_orders.commit();

    } catch (const pqxx::sql_error &e) {
      std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    }
  }
};