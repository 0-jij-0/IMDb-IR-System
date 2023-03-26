#include <iostream>

#include "search_engine_controller.hpp"

int main() {
  bool stop = false;
  SearchEngineController &search_engine_controller =
      SearchEngineController::GetInstance();
  for (std::string query = ""; !stop;
       stop = !search_engine_controller.ExecuteQuery(query)) {
    getline(std::cin, query);
  }

  std::cout << "Thank you for using the app. For feedback or feature requests, "
               "email: jihadghanna@gmail.com"
            << std::endl;
}
