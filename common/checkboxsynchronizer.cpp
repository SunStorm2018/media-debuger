#include "checkboxsynchronizer.h"

CheckBoxSynchronizer::CheckBoxSynchronizer(QObject *parent)
    : StateSynchronizer<QCheckBox>(parent)
{
    setAccessors(
        [](const QCheckBox* checkbox) -> QVariant {
            return checkbox->isChecked();
        },
        [](QCheckBox* checkbox, const QVariant& value) {
            checkbox->setChecked(value.toBool());
        }
        );
}

void CheckBoxSynchronizer::addObject(QCheckBox *obj) {
    if (!obj || objects().contains(obj)) return;
    
    StateSynchronizer<QCheckBox>::addObject(obj);
    
    // 连接信号
    connect(obj, &QCheckBox::toggled, this, [this, obj](bool checked) {
        onStateChanged(obj, checked);
    });
}
