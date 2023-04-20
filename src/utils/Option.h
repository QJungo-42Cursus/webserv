#ifndef OPTION_H
#define OPTION_H
#include <exception>
template <typename T>
class Option
{
public:
	/* Canonic form */
	Option() : _isSome(false) {}
	Option(T value) : _isSome(true), _value(value) {}
	Option(const Option<T> &other) : _isSome(other._isSome), _value(other._value) {}
	Option<T> &operator=(const Option<T> &other)
	{
		_isSome = other._isSome;
		_value = other._value;
		return *this;
	}
	~Option() {}

	bool isSome() const { return _isSome; }
	bool isNone() const { return !_isSome; }
	T &get()
	{
		if (_isSome)
			return _value;
		else
			throw std::exception();
		return _value;
	}
	const T &get() const
	{
		if (_isSome)
			return _value;
		else
			throw std::exception();
		return _value;
	}

private:
	bool _isSome;
	T _value;
};

#endif // OPTION_H