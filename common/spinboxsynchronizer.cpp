// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

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
