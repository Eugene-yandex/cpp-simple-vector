#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <utility>




#include "array_ptr.h"

class ReserveProxyObj {
public:

    ReserveProxyObj() noexcept = default;

    ReserveProxyObj(size_t capacity)
        : capacity_(capacity) {
    }
    size_t GetCapacity() {
        return capacity_;
    }
private:
    size_t capacity_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) :
        items_(size),
        size_(size),
        capacity_(size) {
        std::fill(begin(), end(), 0);
    }


    SimpleVector(size_t size, const Type& value)
        : SimpleVector(size) {
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : SimpleVector(init.size()) {
        std::copy(init.begin(), init.end(), items_.Get());
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        // Напишите тело самостоятельно
        if (index >= size_) {
            throw std::out_of_range("The index is larger than the size of the vector");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("The index is larger than the size of the vector");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        this->Resize(0);
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        // Напишите тело самостоятельно
        if (new_size <= size_) {
            size_ = new_size;
        }
        else if (new_size > size_ && new_size <= capacity_) {
            std::fill(items_.Get(), items_.Get() + size_, 0);
            size_ = new_size;
        }
        else {
            auto razmer = std::max(new_size, capacity_ * 2);
            ArrayPtr<Type> more(razmer);
            std::move(begin(), end(), more.Get());
            std::fill(more.Get() + size_, more.Get() + new_size, 0);
            std::generate(more.Get() + size_, more.Get() + new_size, []() {return Type(); });
            items_.swap(more);
            size_ = new_size;
            capacity_ = razmer;
        }

    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }

    SimpleVector(const SimpleVector& other) {
        // Напишите тело конструктора самостоятельно
        SimpleVector temp(other.capacity_);
        std::copy(other.begin(), other.end(), temp.begin());
        items_.swap(temp.items_);
        size_ = other.size_;
        capacity_ = temp.capacity_;
    }

    SimpleVector(ReserveProxyObj capacity_to_reserve)
        : items_(capacity_to_reserve.GetCapacity()), size_(0), capacity_(capacity_to_reserve.GetCapacity()) {
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) 
        : size_(other.size_), capacity_(other.capacity_) {
        items_.swap(other.items_);
        other.size_ = 0;
        other.capacity_ = 0;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            SimpleVector<Type> tmp(rhs.size_);
            std::move(rhs.begin(),rhs.end(), tmp.begin());
            tmp.capacity_ = rhs.capacity_;
            tmp.size_ = rhs.size_;
            swap(tmp);
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        // Напишите тело самостоятельно
        if (capacity_ > size_) {
            items_[size_] = item;
            ++size_;
        }
        else {
            auto razmer = std::max(static_cast <size_t>(1), capacity_ * 2);
            ArrayPtr<Type> temp(razmer);
            std::copy(begin(), end(), temp.Get());
            temp[size_] = item;
            items_.swap(temp);
            ++size_;
            capacity_ = razmer;
        }
    }

    void PushBack(Type&& item) {
        if (capacity_ > size_) {
            items_[size_] = std::move(item);
            ++size_;
        }
        else {
            auto razmer = std::max(static_cast <size_t>(1), capacity_ * 2);
            ArrayPtr<Type> temp(razmer);
            std::move(begin(), end(), temp.Get());
            temp[size_] = std::move(item);
            items_.swap(temp);
            ++size_;
            capacity_ = razmer;
        }
    }
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        auto dis = std::distance(cbegin(), pos);
        if (capacity_ > size_) {
            std::copy_backward(pos, cend(), items_.Get() + size_ + 1);
            items_[dis] = value;
            ++size_;
            return begin() + dis;
        }
        else {
            auto razmer = std::max(static_cast <size_t>(1), capacity_ * 2);
            ArrayPtr<Type> temp(razmer);
            std::copy(cbegin(), pos, temp.Get());
            temp[dis] = value;
            std::copy_backward(pos, cend(), temp.Get() + size_ + 1);
            items_.swap(temp);
            capacity_ = razmer;
            ++size_;
            return begin() + dis;
        }

    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        auto dis = std::distance(cbegin(), pos);
        if (capacity_ > size_) {
            std::move_backward(begin() + dis, end(), end() + 1);
            items_[dis] = std::move(value);
            ++size_;
            return begin() + dis;
        }
        else {
            auto razmer = std::max(static_cast <size_t>(1), capacity_ * 2);
            ArrayPtr<Type> temp(razmer);
            std::move(&items_[0], &items_[dis], &items_[0]);
            temp[dis] = std::move(value);
            std::move_backward(begin() + dis, end(), temp.Get() + size_ + 1);
            items_.swap(temp);
            capacity_ = razmer;
            ++size_;
            return begin() + dis;
        }

    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(size_ != 0);
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        auto dis = std::distance(cbegin(), pos);
        std::move(begin() + dis +1 , end(), &items_[dis]);
        --size_;
        return begin() + dis;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        size_t x = size_;
        size_t y = capacity_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.size_ = x;
        other.capacity_ = y;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        ArrayPtr<Type> temp(new_capacity);
        std::copy(begin(), end(), temp.Get());
        items_.swap(temp);
        capacity_ = new_capacity;
    }

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;

};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return true;
    }
    return !std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs == rhs || lhs < rhs) {
        return true;
    }
    else {
        return false;
    }
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
