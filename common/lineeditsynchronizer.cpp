// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "lineeditsynchronizer.h"

LineEditSynchronizer::LineEditSynchronizer(QObject *parent)
    : StateSynchronizer<QLineEdit>(parent)
{
    setAccessors(
        [](const QLineEdit* edit) -> QVariant {
            return edit->text();
        },
        [](QLineEdit* edit, const QVariant& value) {
            edit->setText(value.toString());
        }
        );
}

void LineEditSynchronizer::addObject(QLineEdit *obj) {
    if (!obj || objects().contains(obj)) return;
    
    StateSynchronizer<QLineEdit>::addObject(obj);

    connect(obj, &QLineEdit::textChanged, this, [this, obj](const QString& text) {
        onStateChanged(obj, text);
    });
}
