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
        },
        &QLineEdit::textChanged
        );
}
