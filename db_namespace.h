#pragma once
using namespace std;
namespace db {

class courier {
 private:
  int courier_id;
  int age;
  string gender;
  string phone;
  string active;

 public:
  courier() : courier_id(0), age(0), gender(""), phone(""), active("") {}

  courier(const int &id)
      : courier_id(id), age(0), gender(""), phone(""), active("") {}

  courier(const int &id,const string &active)
      : courier_id(id), age(0), gender(""), phone(""), active(active) {}

  courier(const int &id, const int &age, const string &gender,
          const string &phone, const string &active)
      : courier_id(id),
        age(age),
        gender(gender),
        phone(phone),
        active(active) {}

  int get_courier_id() { return courier_id; }
  int get_age() { return age; }
  std::string get_gender() { return gender; }
  std::string get_phone() { return phone; }
  std::string get_active() { return active; }

  void set_courier_id(int id) { this->courier_id = id; }
  void set_age(int age) { this->age = age; }
  void set_gender(const std::string &gender) { this->gender = gender; }
  void set_phone(const std::string &phone) { this->phone = phone; }
  void set_active(const std::string &active) { this->active = active; }
};

class courier_action {
 private:
  string action;  // get //delivered
  int order_id;
  int courier_id;

 public:
  courier_action() : action(""), order_id(0), courier_id(0) {}

  courier_action(const string &action, const int &order_id,
                 const int &courier_id)
      : action(action), order_id(order_id), courier_id(courier_id) {}

  std::string get_action() { return action; }
  int get_order_id() { return order_id; }
  int get_courier_id() { return courier_id; }

  void set_action(const std::string& action) {this->action=action;}
  void set_order_id(int order_id) {this->order_id=order_id;}
  void set_courier_id(int courier_id){this->courier_id=courier_id;}
};

class user {
 private:
  int user_id;
  int age;
  string gender;
  string phone;

 public:
  user() : user_id(0), age(0), gender(""), phone("") {}

  user(const string &phone) : user_id(0), age(0), gender(""), phone(phone) {}

  user(const int &id,const string &phone) : user_id(id), age(0), gender(""), phone(phone) {}

  user(const int &id, const int &age, const string &gender, const string &phone)
      : user_id(id), age(age), gender(gender), phone(phone) {}

  int get_user_id() { return user_id; }
  int get_age() { return age; }
  std::string get_gender() { return gender; }
  std::string get_phone() { return phone; }

  void set_user_id(int id){this->user_id=id;}
void set_age(int age){this->age=age;}
void set_gender(const std::string& gender) {this->gender=gender;}
void set_phone(const std::string& phone){this->phone=phone;}
};

class user_action {
 private:
  string action;  // create //cancel
  int order_id;
  int user_id;

 public:
  user_action() : action(""), order_id(0), user_id(0) {}

  user_action(const string &action, const int &order_id, const int &user_id)
      : action(action), order_id(order_id), user_id(user_id) {}

  string get_action() { return action; }
  int get_order_id() { return order_id; }
  int get_user_id() { return user_id; }

  void set_action(const std::string& action){this->action=action;}
  void set_order_id(int order_id){this->order_id=order_id;}
  void set_user_id(int user_id){this->user_id=user_id;}
};

class product {
 private:
  int product_id;
  string name;
  int price;
  int count;

 public:
  product() : product_id(0), name(""), price(0), count(0) {}

  product(const int &id) : product_id(id), name(""), price(0), count(0) {}

  product(const int &id,const string &name, const int &price, const int &count)
      : product_id(id), name(name), price(price), count(count) {}

  product(const string &name) : name(name) {}

  int get_product_id() { return product_id; }
  std::string get_name() { return name; }
  int get_price() { return price; }
  int get_count() { return count; }

  void set_product_id(int id) { this->product_id = id; }
  void set_name(const std::string &name) { this->name = name; }
  void set_price(int price) { this->price = price; }
  void set_count(int count) { this->count = count; }
};

class order {
 private:
  int order_id;
  string creation_time;
  string time;
  string delivery_address;
  string status;  // accepted  // transfer // delivered // canceled
 public:
  order()
      : order_id(0),
        creation_time(""),
        time(""),
        delivery_address(""),
        status("") {}

  order(const int &id)
      : order_id(id),
        creation_time(""),
        time(""),
        delivery_address(""),
        status("") {}

  order(const int &id, const string &delivery_address)
      : order_id(id), delivery_address(delivery_address) {}

  order(const int &id, const string &delivery_address, const string &status)
      : order_id(id), delivery_address(delivery_address), status(status) {}

  int get_order_id() { return order_id; }
  std::string get_creation_time() { return creation_time; }
  std::string get_time() { return time; }
  std::string get_delivery_address() { return delivery_address; }
  std::string get_status() { return status; }

  void set_order_id(int id){this->order_id=id;}
  void set_creation_time(const std::string& creation_time) {this->creation_time=creation_time;}
  void set_time(const std::string& time){this->time=time;}
  void set_delivery_address(const std::string& delivery_address){this->delivery_address=delivery_address;}
  void set_status(const std::string& status){this->status=status;}
};

class content {
 private:
  int order_id;
  int product_id;

 public:
  content() : order_id(0), product_id(0) {}

  content(const int &order_id, const int &product_id)
      : order_id(order_id), product_id(product_id) {}

  int get_order_id() { return order_id; }
  int get_product_id() { return product_id; }
};
}  // namespace db