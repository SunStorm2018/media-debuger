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
        },
        &QCheckBox::toggled
        );
}
