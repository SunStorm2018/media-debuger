#include "spinboxsynchronizer.h"

SpinBoxSynchronizer::SpinBoxSynchronizer(QObject *parent)
    : StateSynchronizer<QSpinBox>(parent)
{
    setAccessors(
        [](const QSpinBox* spinBox) -> QVariant {
            return spinBox->value();
        },
        [](QSpinBox* spinBox, const QVariant& value) {
            spinBox->setValue(value.toInt());
        }
        );
}

void SpinBoxSynchronizer::addObject(QSpinBox *obj) {
    if (!obj || objects().contains(obj)) return;
    
    StateSynchronizer<QSpinBox>::addObject(obj);

    connect(obj, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), 
            this, [this, obj](int value) {
        onStateChanged(obj, value);
    });
}
