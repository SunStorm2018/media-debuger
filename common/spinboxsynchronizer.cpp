#include "spinboxsynchronizer.h"

SpinBoxSynchronizer::SpinBoxSynchronizer(QObject *parent)
    : StateSynchronizer<QSpinBox>(parent)
{
    setAccessors(
        [](const QSpinBox* spinbox) -> QVariant {
            return spinbox->value();
        },
        [](QSpinBox* spinbox, const QVariant& value) {
            spinbox->setValue(value.toInt());
        },
        static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)
        );
}
