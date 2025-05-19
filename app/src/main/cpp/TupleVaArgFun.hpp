//
// Created by cherish on 2025/5/13.
//

#ifndef OPENCV_TUPLEVAARGFUN_HPP
#define OPENCV_TUPLEVAARGFUN_HPP
#include <iostream>
#include <tuple>
#include <utility>

void print_values() {
    std::cout << std::endl;
}

template<typename T, typename... Args>
void print_values(T value, Args... args) {//递归调用
    std::cout << value << " ";
    print_values(args...);
}

template<typename... Args>
void print_with_tuple(Args... args) {
    auto tuple = std::make_tuple(args...);
    std::apply([](auto&&... values) {
        print_values(values...);
    }, tuple);
}

template<typename T>
void relay(T&& arg) {  // 万能引用
    process(std::forward<T>(arg));  // 完美转发
}
#endif //OPENCV_TUPLEVAARGFUN_HPP
