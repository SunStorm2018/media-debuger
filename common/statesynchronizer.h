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
    // 使用通用信号类型，通过lambda适配不同的信号
    using SignalHandler = std::function<void(T*, const QVariant&)>;

    explicit StateSynchronizer(QObject *parent = nullptr);

    // 添加要同步的对象
    void addObject(T* obj);

    // 移除同步对象
    void removeObject(T* obj);

    // 清空所有同步对象
    void clear();

    // 设置状态获取和设置函数
    void setAccessors(GetterFunc getter, SetterFunc setter);

    // 连接信号 - 模板方法支持不同类型的信号
    template<typename SignalType, typename SlotType>
    void connectSignal(T* obj, SignalType signal, SlotType slot);

    // 设置是否启用同步
    void setEnabled(bool enabled);
    bool isEnabled() const;

    // 立即同步所有对象的状态
    void syncAll(const QVariant& value = QVariant());

    // 获取所有同步对象
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
