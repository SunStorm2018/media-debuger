#ifndef SINGLETON_H
#define SINGLETON_H

#include <QObject>
#include <QMutex>
#include <QScopedPointer>

template <typename T>
class Singleton
{
public:
    static T& instance()
    {
        static T* m_instance = nullptr;
        if (!m_instance) {
            static QMutex mutex;
            QMutexLocker locker(&mutex);
            if (!m_instance) {
                m_instance = new T;
            }
        }
        return *m_instance;}

protected:
    Singleton() = default;
    ~Singleton() = default;

private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

#define DECLARE_SINGLETON(Class) \
public: \
    static Class& instance() { \
        return Singleton<Class>::instance(); \
    } \
private: \
    friend class Singleton<Class>; \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;

#endif // SINGLETON_H
