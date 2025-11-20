// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef ZSINGLETON_H
#define ZSINGLETON_H

#include <QObject>
#include <QMutex>
#include <QScopedPointer>

template <typename T>
class ZSingleton
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
        return *m_instance;
    }

protected:
    ZSingleton() = default;
    ~ZSingleton() = default;

private:
    ZSingleton(const ZSingleton&) = delete;
    ZSingleton& operator=(const ZSingleton&) = delete;
};

#define DECLARE_ZSINGLETON(Class) \
public: \
    static Class& instance() { \
        return ZSingleton<Class>::instance(); \
    } \
private: \
    friend class ZSingleton<Class>; \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;

#endif // ZSINGLETON_H