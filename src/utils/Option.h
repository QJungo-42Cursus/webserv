#ifndef OPTION_H
#define OPTION_H

#include <exception>
#include <iostream>

template<typename T>
class Option
{
public:
	Option() : _isSome(false)
	{}

	static Option<T> None()
	{ return Option<T>(); }

	static Option<T> Some(T value)
	{ return Option<T>(value); }

	Option<T> &operator=(const Option<T> &other)
	{
		_isSome = other._isSome;
		_value = other._value;
		return *this;
	}

	~Option()
	{}

	bool isSome() const
	{ return _isSome; }

	bool isNone() const
	{ return !_isSome; }

	T &unwrap()
	{
		if (!_isSome)
			throw NoneException();
		return _value;
	}

	const T &unwrap() const
	{
		if (!_isSome)
			throw NoneException();
		return _value;
	}


	struct NoneException : public std::exception
	{
		const char *what() const throw()
		{ return "Tried to unwrap None"; }
	};

	friend std::ostream &operator<<(std::ostream &os, const Option<T> &option)
	{
		if (option._isSome)
			os << "Some(" << option._value << ")";
		else
			os << "None";
		return os;
	}

	bool operator==(const Option<T> &other) const
	{
		if (_isSome != other._isSome)
			return false;
		if (_isSome)
			return _value == other._value;
		return true;
	}

	bool operator!=(const Option<T> &other) const
	{
		return !(*this == other);
	}

	bool operator>(const Option<T> &other) const
	{
		if (_isSome != other._isSome)
			return _isSome;
		if (_isSome)
			return _value > other._value;
		return false;
	}

	bool operator<(const Option<T> &other) const
	{
		if (_isSome != other._isSome)
			return !_isSome;
		if (_isSome)
			return _value < other._value;
		return false;
	}

	bool operator>=(const Option<T> &other) const
	{
		return !(*this < other);
	}

	bool operator<=(const Option<T> &other) const
	{
		return !(*this > other);
	}

private:
	bool _isSome;
	T _value;

	explicit Option(T value) : _isSome(true), _value(value)
	{}
};


#endif // OPTION_H