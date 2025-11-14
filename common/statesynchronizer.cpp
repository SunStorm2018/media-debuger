// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "statesynchronizer.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

template<typename T>
StateSynchronizer<T>::StateSynchronizer(QObject *parent)
    : QObject(parent), m_enabled(true), m_syncing(false) {}

// Explicit instantiation for common types
template class StateSynchronizer<QLineEdit>;
template class StateSynchronizer<QSpinBox>;
template class StateSynchronizer<QCheckBox>;

template<typename T>
void StateSynchronizer<T>::addObject(T *obj) {
    if (!obj || m_objects.contains(obj)) return;

    m_objects.insert(obj);

    // Set signal handler
    m_signalHandler = [this](T* changedObj, const QVariant& value) {
        onStateChanged(changedObj, value);
    };
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
void StateSynchronizer<T>::setAccessors(GetterFunc getter, SetterFunc setter) {
    m_getter = getter;
    m_setter = setter;
}

template<typename T>
template<typename SignalType, typename SlotType>
void StateSynchronizer<T>::connectSignal(T* obj, SignalType signal, SlotType slot) {
    connect(obj, signal, this, slot);
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
    // Connection logic handled in derived classes
}

template<typename T>
void StateSynchronizer<T>::disconnectObject(T *obj) {
    disconnect(obj, nullptr, this, nullptr);
}

template<typename T>
void StateSynchronizer<T>::onStateChanged(T *changedObj, const QVariant &value) {
    if (!m_enabled || m_syncing || !m_getter || !m_setter) return;

    m_syncing = true;
    
    // Use passed value parameter instead of refetching
    for (T* obj : m_objects) {
        if (obj != changedObj) {
            m_setter(obj, value);
        }
    }

    m_syncing = false;
}
