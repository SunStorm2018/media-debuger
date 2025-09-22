#include "statesynchronizer.h"

template<typename T>
StateSynchronizer<T>::StateSynchronizer(QObject *parent)
    : QObject(parent), m_enabled(true), m_syncing(false) {}

template<typename T>
void StateSynchronizer<T>::addObject(T *obj) {
    if (!obj || m_objects.contains(obj)) return;

    m_objects.insert(obj);

    // 如果有设置访问器，建立连接
    if (m_getter && m_setter) {
        // 这里需要具体的信号类型，所以在派生类中处理更合适
    }
}

template<typename T>
void StateSynchronizer<T>::removeObject(T *obj) {
    if (m_objects.contains(obj)) {
        disconnectObject(obj);
        m_objects.remove(obj);
    }
}

template<typename T>
void StateSynchronizer<T>::clear() {
    for (T* obj : m_objects) {
        disconnectObject(obj);
    }
    m_objects.clear();
}

template<typename T>
void StateSynchronizer<T>::setAccessors(GetterFunc getter, SetterFunc setter, ChangeSignal signal) {
    m_getter = getter;
    m_setter = setter;

    // 先断开所有现有连接
    for (T* obj : m_objects) {
        disconnect(obj, nullptr, this, nullptr);
    }

    // 重新建立连接
    for (T* obj : m_objects) {
        connect(obj, signal, this, [this, obj](auto&&... args) {
            onStateChanged(obj);
        });
    }
}

template<typename T>
void StateSynchronizer<T>::setEnabled(bool enabled) { m_enabled = enabled; }

template<typename T>
bool StateSynchronizer<T>::isEnabled() const { return m_enabled; }

template<typename T>
void StateSynchronizer<T>::syncAll(const QVariant &value) {
    if (!m_enabled || !m_getter || !m_setter) return;

    m_syncing = true;

    if (value.isValid()) {
        for (T* obj : m_objects) {
            m_setter(obj, value);
        }
    } else if (!m_objects.isEmpty()) {
        T* firstObj = *m_objects.begin();
        QVariant syncValue = m_getter(firstObj);
        for (T* obj : m_objects) {
            if (obj != firstObj) {
                m_setter(obj, syncValue);
            }
        }
    }

    m_syncing = false;
}

template<typename T>
QSet<T *> StateSynchronizer<T>::objects() const { return m_objects; }

template<typename T>
void StateSynchronizer<T>::setupConnections(T *obj) {
    if (!m_signal) return;

    connect(obj, m_signal, this, [this, obj](const QVariant& value) {
        onStateChanged(obj, value);
    });
}

template<typename T>
void StateSynchronizer<T>::disconnectObject(T *obj) {
    if (m_signal) {
        disconnect(obj, m_signal, this, nullptr);
    }
}

template<typename T>
void StateSynchronizer<T>::onStateChanged(T *changedObj, const QVariant &value) {
    if (!m_enabled || m_syncing || !m_getter || !m_setter) return;

    m_syncing = true;
    QVariant value = m_getter(changedObj);

    for (T* obj : m_objects) {
        if (obj != changedObj) {
            m_setter(obj, value);
        }
    }

    m_syncing = false;
}
