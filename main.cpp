#include <iostream>

#include "HashTable.hpp"


int main() {
    HashTable<int64_t> table;
    
    table.insert(100, 42);
    table.insert(200, 84);
    table.insert(300, 126);
    
    int64_t value;
    if (table.find(200, value)) {
        std::cout << "found key 200:" << value << '\n';
    }
    
    // // Получаем значение (создаст, если нет)
    // int& val = table.get(400);  // Создаст со значением 0
    // val = 168;
    
    // // Проверяем наличие ключа
    // if (table.contains(400)) {
    //     printf("Ключ 400 существует\n");
    // }
    
    // // Удаляем элемент
    // table.remove(100);
    
    
    
    // for (uint64_t i = 0; i < 100; i++) {
    //     char buffer[20];
    //     snprintf(buffer, sizeof(buffer), "Value%d", (int)i);
    //     stringTable.insert(i * 10, buffer);
    // }
    
    // printf("\nПосле вставки 100 элементов:\n");
    
    // return 0;
}