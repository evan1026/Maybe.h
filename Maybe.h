#ifndef MAYBE_H
#define MAYBE_H

#include <stdexcept>

/*************************************************************
 * ~~~~~~~~~~~~~~~~~~~~~~~~                                  *
 * ~ null_maybe_exception ~                                  *
 * ~~~~~~~~~~~~~~~~~~~~~~~~                                  *
 *                                                           *
 * This is the exception thrown if someone tries to grab the *
 * value out of a null Maybe.                                *
 *************************************************************/
class null_maybe_exception : public std::runtime_error {
public:
    null_maybe_exception() : runtime_error("Atempt to turn a null Maybe into a value.") {}
};


/*********************************************************************
 * ~~~~~~~~~~~~                                                      *
 * ~ Maybe<T> ~                                                      *
 * ~~~~~~~~~~~~                                                      *
 *                                                                   *
 * This class is based off of the class of the same name in Haskell. *
 * It represents a value that may or may not exist. It is used to    *
 * handle when a value may or may not exist, without the need for an *
 * exception. A Maybe of any given type holds a pointer to a value,  *
 * which will either point to actual data or to nothing. Care should *
 * be taken to ensure that the Maybe actually stores a value before  *
 * trying to use it.                                                 *
 *                                                                   *
 * Example usage:                                                    *
 *     int a;                                                        *
 *     Maybe<int> maybeWithValue(10);                                *
 *     Maybe<int> maybeWithoutValue();                               *
 *                                                                   *
 *     if (maybeWithValue) {                                         *
 *         //there is data, using it is safe                         *
 *         a = maybeWithValue();                                     *
 *     } else {                                                      *
 *         //will not get here, since it has data                    *
 *     }                                                             *
 *                                                                   *
 *     if (maybeWithoutValue) {                                      *
 *         //will not get here, since it has no data                 *
 *     } else {                                                      *
 *         //this will throw a null_maybe_exception                  *
 *         a = maybeWithoutValue();                                  *
 *     }                                                             *
 *                                                                   *
 *     //you can also assign directly from a value                   *
 *     maybeWithoutValue = 10;                                       *
 *                                                                   *
 *     //Or from a null pointer                                      *
 *     maybeWithValue = nullptr;                                     *
 *                                                                   *
 *     if (maybeWithValue) {                                         *
 *         //it will no longer make it here, since it no longer      *
 *         //has a value                                             *
 *     }                                                             *
 *                                                                   *
 *     if(maybeWithoutValue) {                                       *
 *         //It will now make it in here, since it now has a value   *
 *     }                                                             *
 *********************************************************************/
template <typename T>
class Maybe {
    T* value;

public:
    Maybe<T>(const T val) {
        value = new T;
        *value = val;
    }
    Maybe<T>() {
        value = nullptr;
    }

    ~Maybe<T>() {
        if (value != nullptr) {
            delete value;
        }
    }
    Maybe<T>(const Maybe<T>& other) {
        value = new T;
        *value = *other.value;
    }
    Maybe<T>(Maybe<T>&& other) {
        value = other.value;
        other.value = nullptr;
    }

    Maybe<T>& operator=(const Maybe<T>& other) {
        if (value != nullptr) {
            delete value;
        }
        value = new T;
        *value = *other.value;
        return *this;
    }
    Maybe<T>& operator=(Maybe<T>&& other) {
        if (value != nullptr){
            delete value;
        }
        value = other.value;
        other.value = nullptr;
        return *this;
    }

    Maybe<T>& operator=(const T& val) {
        if (value != nullptr) {
            delete value;
        }
        value = new T;
        *value = val;
        return *this;
    }

    Maybe<T>& operator=(const std::nullptr_t& null_p) {
        if (value != nullptr) {
            delete value;
        }
        value = null_p;
        return *this;
    }

    operator bool() const {
        return value != nullptr;
    }

    T operator()() const {
        if (*this) return *value;
        else throw null_maybe_exception();
    }
};

#endif
