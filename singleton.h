#pragma once

/* @example
 *
 * class Class : public Singleton<Class>
 * {
 * public:
 *
 * protected:
 * 	Class();
 *
 * 	friend class Singleton<Class>;
 * };
 */


template<class T>
class Singleton
{
public:
    static T* getInstance()
	{
		static T instance;
		return &instance;
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator= (const Singleton) = delete;

protected:
    Singleton() {};
};

