#include "lineeditsynchronizer.h"

LineEditSynchronizer::LineEditSynchronizer(QObject *parent)
    : StateSynchronizer<QLineEdit>(parent)
{
    // 设置 QLineEdit 专用的访问器
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
    
    // 连接信号
    connect(obj, &QLineEdit::textChanged, this, [this, obj](const QString& text) {
        onStateChanged(obj, text);
    });
}
