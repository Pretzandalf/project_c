#include "Controller.h"

#include <exception>
#include <iostream>

// Добрый день! или вечер, я буду стараться оставлять коменты, чтобы было почитабельнее (а еще тзшку отмечать вашу (а еще я смогу хоть, что-то вспомнить на защите)) 
int main(int argc, const char* argv[]) {
    try {
        Controller controller;
        return controller.Run(argc, argv); // отделяем фильтры
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown error\n"; // максимально общиие ошибки
        return 1;
    }
}

// просто запускаем все это дело