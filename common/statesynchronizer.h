// StateSynchronizer.h
#ifndef STATESYNCHRONIZER_H
#define STATESYNCHRONIZER_H

#include <QObject>
#include <QSet>
#include <functional>
#include <type_traits>
#include <QVariant>

template<typename T>
class StateSynchronizer : public QObject
{
    static_assert(std::is_base_of<QObject, T>::value,
                  "T must inherit from QObject");

public:
    using GetterFunc = std::function<QVariant(const T*)>;
    using SetterFunc = std::function<void(T*, const QVariant&)>;
    // Generic signal type using lambda to adapt different signals
    using SignalHandler = std::function<void(T*, const QVariant&)>;

    explicit StateSynchronizer(QObject *parent = nullptr);

    // Add object to synchronize
    void addObject(T* obj);

    // Remove synchronized object
    void removeObject(T* obj);

    // Clear all synchronized objects
    void clear();

    // Set state getter/setter functions
    void setAccessors(GetterFunc getter, SetterFunc setter);

    // Connect signal - template method supports different signal types
    template<typename SignalType, typename SlotType>
    void connectSignal(T* obj, SignalType signal, SlotType slot);

    // Enable/disable synchronization
    void setEnabled(bool enabled);
    bool isEnabled() const;

    // Sync all objects immediately
    void syncAll(const QVariant& value = QVariant());

    // Get all synchronized objects
    QSet<T*> objects() const;

protected:
    void setupConnections(T* obj);

    void disconnectObject(T* obj);

    void onStateChanged(T* changedObj, const QVariant& value);

private:
    QSet<T*> m_objects;
    GetterFunc m_getter;
    SetterFunc m_setter;
    SignalHandler m_signalHandler;
    bool m_enabled;
    bool m_syncing;
    QMetaObject::Connection m_connection;
};

#endif // STATESYNCHRONIZER_H
