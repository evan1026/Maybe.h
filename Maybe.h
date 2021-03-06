#ifndef MAYBE_H
#define MAYBE_H

#include <stdexcept>
#include <memory>

// operator== check from https://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists
namespace Maybe_Tests
{
  struct No {};
  template<typename T, typename Arg> No operator== (const T&, const Arg&);

  template<typename T, typename Arg = T>
  struct EqualExists
  {
    enum { value = !std::is_same<decltype(*(T*)(0) == *(Arg*)(0)), No>::value };
  };
}

/*!
 * Exception thrown when someone tries to extract a value from an empty Maybe.
 */
class null_maybe_exception : public std::runtime_error {
public:
    null_maybe_exception() : runtime_error("Atempt to turn a null Maybe into a value.") {}
};

/*!
 * This class is based off of the class of the same name in Haskell.
 * It represents a value that may or may not exist. A Maybe of any
 * given type holds a pointer to a value, which will either point
 * to actual data or to nothing. Care should be taken to ensure
 * that the Maybe actually stores a value before trying to use it.
 *
 * Example usage:
 *
 *     int a;
 *     Maybe<int> maybeWithValue(10);
 *     Maybe<int> maybeWithoutValue();
 *
 *     if (maybeWithValue) {
 *         //there is data, using it is safe
 *         a = maybeWithValue();
 *     } else {
 *         //will not get here, since it has data
 *     }
 *
 *     if (maybeWithoutValue) {
 *         //will not get here, since it has no data
 *     } else {
 *         //this will throw a NullMaybeException
 *         a = maybeWithoutValue();
 *     }
 *
 *     //you can also assign directly from a value
 *     maybeWithoutValue = 10;
 *
 *     //Or from a null pointer
 *     maybeWithValue = nullptr;
 *
 *     if (maybeWithValue) {
 *         //it will no longer make it here, since it no longer
 *         //has a value
 *     }
 *
 *     if(maybeWithoutValue) {
 *         //It will now make it in here, since it now has a value
 *     }
 */
template <typename T>
class Maybe {

    T* value;
    std::allocator<T> alloc;

public:
    /*!
     * Constructs a non-empty Maybe by forwarding the args to the
     * templated type's constructor.
     */
    template <typename... Ts>
    Maybe<T>(const Ts&... args) {
        value = alloc.allocate(1);
        alloc.construct(value, args...);
    }

    /*!
     * Constructs an empty maybe
     */
    Maybe<T>() {
        value = nullptr;
    }

    /*!
     * Construct with null pointer
     */
    Maybe<T>(const std::nullptr_t& null_p) {
        value = null_p;
    }

    /*!
     * Frees up the contained value, if there is one
     */
    ~Maybe<T>() {
        if (value != nullptr) {
            alloc.destroy(value);
        }
    }

    /*!
     * Copy constructor
     */
    Maybe<T>(const Maybe<T>& other) {
        if (other.value != nullptr) {
            value = alloc.allocate(1);
            alloc.construct(value, *other);
        } else {
            value = nullptr;
        }
    }

    /*!
     * Move constructor
     */
    Maybe<T>(Maybe<T>&& other) {
        value = other.value;
        other.value = nullptr;
    }

    /*!
     * Copy assignment operator
     */
    Maybe<T>& operator=(const Maybe<T>& other) {
        if (value != nullptr) {
            alloc.destroy(value);
        }

        if (other.value != nullptr) {
            value = alloc.allocate(1);
            alloc.construct(value, *other);
        } else {
            value = nullptr;
        }

        return *this;
    }

    /*!
     * Move assignment operator
     */
    Maybe<T>& operator=(Maybe<T>&& other) {
        if (value != nullptr){
            alloc.destroy(value);
        }

        value = other.value;
        other.value = nullptr;

        return *this;
    }

    /*!
     * Assign to Maybe based on anything T can be assigned by.
     *
     * Note: It also needs to be constructable from the same type
     * because if the object has never been constructed, the assignment
     * operator can misbehave, and I do not want to require an object to
     * be default constructable.
     */
    template <typename O>
    Maybe<T>& operator=(const O& other) {
        if (value == nullptr) {
            value = alloc.allocate(1);
            alloc.construct(value, other);
        } else {
            *value = other;
        }

        return *this;
    }

    /*!
     * Assign with null pointer
     */
    Maybe<T>& operator=(const std::nullptr_t& null_p) {
        if (value != nullptr) {
            alloc.destroy(value);
        }

        value = null_p;

        return *this;
    }

    /*!
     * Checks if value can be extracted from this Maybe
     */
    operator bool() const {
        return value != nullptr;
    }

    /*!
     * Extracts the value from this Maybe, and throws and exception if we can't
     */
    T& operator()() const {
        if (*this) return *value;
        else throw null_maybe_exception();
    }

private:
    template<typename oT>
    typename std::enable_if<Maybe_Tests::EqualExists<T, oT>::value, bool>::type doEqualComparison(const Maybe<oT>& other) const {
        return (*this)() == other();
    }
    template<typename oT>
    typename std::enable_if<!Maybe_Tests::EqualExists<T, oT>::value, bool>::type doEqualComparison(const Maybe<oT>& other) const {
        return false;
    }

public:

    /*!
     * Compares two Maybe objects
     *
     * Truth table (a and b are aribitrary values of type T s.t. a != b
     * and c and d are of type oT s.t. c == a and d != a; nullptr<T> means
     * a nullptr assigned to a pointer of type T):
     *
     * +-------------+-------------+-------+
     * | this->value | other.value | Out   |
     * +-------------+-------------+-------+
     * | a           | a           | true  |
     * | a           | b           | false |
     * | a           | c           | true  |
     * | a           | d           | false |
     * | a           | nullptr     | false |
     * | nullptr<T>  | nullptr<T>  | true  |
     * | nullptr<T>  | nullptr<oT> | false |
     * +-------------+-------------+-------+
     */
    template<typename oT>
    bool operator==(const Maybe<oT>& other) const {
        if ((*this && !other) || (!*this && other)) {
            return false;
        }

        if (!*this && !other) {
            return std::is_same<T, oT>::value;
        }

        return doEqualComparison(other);
    }

    /*
     * != operator which leverages == operator
     */
    template<typename oT>
    bool operator!=(const Maybe<oT>& other) const {
        return !(*this == other);
    }
};

#endif
