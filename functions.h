
#include <stdio.h>

#include <boost/program_options.hpp>
#include <cctype>
#include <iostream>
#include <memory>
#include <optional>
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
  //+
  std::optional<int> add_courier(db::courier &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result check_result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM couriers WHERE phone = $1)",
          obj.get_phone());
      check.commit();
      if (check_result[0][0].as<bool>()) {
        return std::nullopt;
      } else {
        pqxx::work ad_courier(*conn);
        pqxx::result result = ad_courier.exec_params(
            "INSERT INTO couriers (courier_id, age, gender, phone, active) "
            "VALUES (DEFAULT, $1, $2, $3, $4) RETURNING courier_id",
            obj.get_age(), obj.get_gender(), obj.get_phone(), obj.get_active());
        ad_courier.commit();
        int courier_id;
        courier_id = result[0][0].as<int>();
        return courier_id;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка добавления курьера: " +
                               std::string(e.what()));
    }
  }
  //+
  std::optional<int> add_order(db::order &obj) {
    try {
      pqxx::work createOrder(*conn);
      pqxx::result result = createOrder.exec_params(
          "INSERT INTO orders (order_id, creation_time, time, "
          "delivery_address, status) VALUES (DEFAULT, CURRENT_TIMESTAMP(2), "
          "CURRENT_TIMESTAMP(2), $1, $2) RETURNING order_id",
          obj.get_delivery_address(), "accepted");
      createOrder.commit();
      int order_id;
      order_id = result[0][0].as<int>();
      return order_id;
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка создания заказа: " +
                               std::string(e.what()));
    }
  }
  //+
  std::optional<int> add_product(db::product &obj) {
    try {
      pqxx::work ad_product(*conn);
      pqxx::result result = ad_product.exec_params(
          "INSERT INTO products (product_id, name, price, count) VALUES "
          "(DEFAULT, $1, $2, $3) RETURNING product_id",
          obj.get_name(), obj.get_price(), obj.get_count());
      ad_product.commit();
      int product_id;
      product_id = result[0][0].as<int>();
      return product_id;
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка создания продукта:" +
                               std::string(e.what()));
    }
  }
  //+
  std::optional<int> add_user(db::user &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result check_result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM users WHERE phone = $1)",
          obj.get_phone());
      check.commit();
      if (check_result[0][0].as<bool>()) {
        return std::nullopt;
      } else {
        pqxx::work ad_user(*conn);
        pqxx::result result = ad_user.exec_params(
            "INSERT INTO users (user_id, age, gender, phone) VALUES (DEFAULT, "
            "$1, $2, $3) RETURNING user_id",
            obj.get_age(), obj.get_gender(), obj.get_phone());
        ad_user.commit();
        int user_id;
        user_id = result[0][0].as<int>();
        return user_id;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка создания пользователя:" +
                               std::string(e.what()));
    }
  }

  bool add_password(db::user_password &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result check_result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM passwords WHERE phone = $1)",
          obj.get_phone());
      check.commit();
      if (check_result[0][0].as<bool>()) {
       return false;
      } else {
        pqxx::work ad_password(*conn);
        pqxx::result result = ad_password.exec_params(
            "INSERT INTO passwords (phone, password) VALUES ($1, $2)",
            obj.get_phone(), obj.get_password());
        ad_password.commit();
        return true;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка добавления пароля: " +
                               std::string(e.what()));
    }
  }

  bool add_courier_action(db::courier_action &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result check_result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM courier_actions WHERE order_id = $1 AND courier_id = $2 AND action = $3)",
          obj.get_order_id(),obj.get_courier_id(),obj.get_action());
      check.commit();
      if (check_result[0][0].as<bool>()) {
        return false;
      } else {
      pqxx::work transfer(*conn);
      transfer.exec_params(
          "INSERT INTO courier_actions (action, order_id, courier_id) VALUES "
          "($1, $2, $3)",
          obj.get_action(), obj.get_order_id(), obj.get_courier_id());
      transfer.commit();
      return true;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка создания действия курьера:" +
                               std::string(e.what()));
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
      throw std::runtime_error("Ошибка создания действия пользователя:" +
                               std::string(e.what()));
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
      throw std::runtime_error(
          "Ошибка создания создания записи содержимого заказа:" +
          std::string(e.what()));
    }
  }
  //+
  bool update_courier(db::courier &obj) {
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
        return true;
      } else {
        return false;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка обновления курьера:" +
                               std::string(e.what()));
    }
  }
  //+
  bool update_courier_status(db::courier &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM couriers WHERE courier_id = $1)",
          obj.get_courier_id());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work delete_cour(*conn);
        delete_cour.exec_params(
            "UPDATE couriers SET active=$1 WHERE courier_id=$2",
            obj.get_active(), obj.get_courier_id());
        delete_cour.commit();
        return true;
      } else {
        return false;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка обновления статуса курьера:" +
                               std::string(e.what()));
    }
  }
  //+
  bool update_product(db::product &obj) {
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
        return true;
      } else {
        return false;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка обновления продукта:" +
                               std::string(e.what()));
    }
  }
  //+
  bool update_product_status(db::product &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM products WHERE product_id = $1)",
          obj.get_name());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work update_status(*conn);
        update_status.exec_params(
            "UPDATE products SET count=0 WHERE product_id=$1",
            obj.get_product_id());
        update_status.commit();
        return true;
      } else {
        return false;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка обновления статуса продукта:" +
                               std::string(e.what()));
    }
  }
  //+
  bool update_user(db::user &obj,std::string &old_phone) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM users WHERE phone = $1)",
          old_phone);
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work update_in_passwords(*conn);
        pqxx::result ress=update_in_passwords.exec_params(
          "UPDATE passwords SET phone=$1 WHERE phone IN (SELECT phone FROM users WHERE phone = $2)",obj.get_phone(),old_phone);
        update_in_passwords.commit();

        pqxx::work update(*conn);
        pqxx::result res = update.exec_params(
            "UPDATE users SET age=$1, phone=$2 WHERE phone=$3", obj.get_age(),
            obj.get_phone(), old_phone);
        update.commit();
       
        return true;
      } else {
        return false;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка обновления пользователя:" +
                               std::string(e.what()));
    }
  }

  bool update_order_status(db::order &obj) {
    try {
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
        return true;
      } else {
        return false;
      }

    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка обновления статуса заказа:" +
                               std::string(e.what()));
    }
  }
  //+
  std::optional<db::courier> get_info_courier(db::courier &obj) {
    try {
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
        db::courier courier;
        courier.set_courier_id(result[0][0].as<int>());
        courier.set_age(result[0][1].as<int>());
        courier.set_gender(result[0][2].as<std::string>());
        courier.set_phone(result[0][3].as<std::string>());
        courier.set_active(result[0][4].as<std::string>());
        return courier;
      } else {
        return std::nullopt;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка получения информации о курьере:" +
                               std::string(e.what()));
    }
  }
  //+
  std::optional<db::product> get_info_product(db::product &obj) {
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
        db::product product;
        product.set_product_id(res[0][0].as<int>());
        product.set_name(res[0][1].as<std::string>());
        product.set_price(res[0][2].as<int>());
        product.set_count(res[0][3].as<int>());
        return product;
      } else {
        return std::nullopt;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка получения информации о продукте:" +
                               std::string(e.what()));
    }
  }
  //+
  std::optional<db::user> get_info_user(db::user &obj) {
    try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT EXISTS (SELECT 1 FROM users WHERE phone = $1)",
          obj.get_phone());
      check.commit();
      if (result[0][0].as<bool>()) {
        pqxx::work get_info(*conn);
        pqxx::result result = get_info.exec_params(
            "SELECT * FROM users WHERE phone = $1", obj.get_phone());
        get_info.commit();
        db::user user;
        user.set_user_id(result[0][0].as<int>());
        user.set_age(result[0][1].as<int>());
        user.set_gender(result[0][2].as<std::string>());
        user.set_phone(result[0][3].as<std::string>());
        return user;
      } else {
        return std::nullopt;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка получения информации о пользователе:" +
                               std::string(e.what()));
    }
  }
  //+
  std::optional<db::order> get_info_order(db::order &obj) {
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
        db::order order;
        order.set_order_id(result[0][0].as<int>());
        order.set_creation_time(result[0][1].as<std::string>());
        order.set_time(result[0][2].as<std::string>());
        order.set_delivery_address(result[0][3].as<std::string>());
        order.set_status(result[0][4].as<std::string>());
        return order;
      } else {
        return std::nullopt;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка получения информации о заказе:" +
                               std::string(e.what()));
    }
  }

  std::optional<std::vector<db::product>> get_info_all_products() {
    try {
      pqxx::work get(*conn);
      pqxx::result res = get.exec_params("SELECT * FROM products ");
      get.commit();
      vector<db::product> products;
      for (const auto &row : res) {
        db::product product;
        product.set_product_id(res[0][0].as<int>());
        product.set_name(res[0][1].as<std::string>());
        product.set_price(res[0][2].as<int>());
        product.set_count(res[0][3].as<int>());
        products.push_back(product);
      }
      return products;
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка получения информации о всех продуктах:" +
                               std::string(e.what()));
    }
  }

  std::optional<db::user_action> get_info_user_action(db::user_action &obj) {
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
        db::user_action user_action;
        user_action.set_action(result[0][0].as<std::string>());
        user_action.set_order_id(result[0][1].as<int>());
        user_action.set_user_id(result[0][2].as<int>());
        return user_action;
      } else {
        return std::nullopt;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error(
          "Ошибка получения информации о действии пользователя:" +
          std::string(e.what()));
    }
  }

  std::optional<db::courier_action> get_info_courier_action( db::courier_action &obj) {
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
        db::courier_action courier_action;
        courier_action.set_action(result[0][0].as<std::string>());
        courier_action.set_order_id(result[0][1].as<int>());
        courier_action.set_courier_id(result[0][2].as<int>());
        return courier_action;
      } else {
        return std::nullopt;
      }
    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error(
          "Ошибка получения информации о действии курьера:" +
          std::string(e.what()));
    }
  }

  std::optional<vector<db::order>> get_orders(db::user &obj) {
    try {
      pqxx::work get_orders(*conn);
      pqxx::result result = get_orders.exec_params(
          "SELECT * FROM orders WHERE order_id IN (SELECT order_id FROM "
          "user_actions WHERE user_id IN (SELECT user_id FROM users WHERE phone = $1))",
          obj.get_phone());
      get_orders.commit();
      vector<db::order> orders;
      for (const auto &row : result) {
        db::order order;
        order.set_order_id(result[0][0].as<int>());
        order.set_creation_time(result[0][1].as<std::string>());
        order.set_time(result[0][2].as<std::string>());
        order.set_delivery_address(result[0][3].as<std::string>());
        order.set_status(result[0][4].as<std::string>());
        orders.push_back(order);
      }
      return orders;

    } catch (const pqxx::sql_error &e) {
      throw std::runtime_error("Ошибка получения заказов пользователя:" +
                               std::string(e.what()));
    }
  }

bool login(db::user_password& obj){
 try {
      pqxx::work check(*conn);
      pqxx::result result = check.exec_params(
          "SELECT password FROM passwords WHERE phone = $1",
          obj.get_phone());
      check.commit();
      if (result.size() == 0) {
            return false; // Пользователь не найден
        }
      std::string password=(result[0][0].as<std::string>());

        if(obj.get_password()==password){
        return true;
        }
        else{return false;}

      
     
 }
 catch (const pqxx::sql_error &e) {
      throw std::runtime_error(std::string(e.what()));
    }


}


};