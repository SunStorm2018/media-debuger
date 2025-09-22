// SynchronizerMacros.h
#ifndef SYNCHRONIZERMACROS_H
#define SYNCHRONIZERMACROS_H

#include "lineeditsynchronizer.h"
#include "checkboxsynchronizer.h"
#include "spinboxsynchronizer.h"

// 同步多个 QLineEdit
#define SYNC_LINEEDITS(...) \
do { \
        static LineEditSynchronizer synchronizer; \
        QLineEdit* edits[] = {__VA_ARGS__}; \
        for (QLineEdit* edit : edits) { \
            if (edit) synchronizer.addObject(edit); \
    } \
} while (0)

// 同步多个 QCheckBox
#define SYNC_CHECKBOXES(...) \
    do { \
        static CheckBoxSynchronizer synchronizer; \
        QCheckBox* checkboxes[] = {__VA_ARGS__}; \
        for (QCheckBox* checkbox : checkboxes) { \
            if (checkbox) synchronizer.addObject(checkbox); \
    } \
} while (0)

// 同步多个 QSpinBox
#define SYNC_SPINBOXES(...) \
    do { \
            static SpinBoxSynchronizer synchronizer; \
            QSpinBox* spinboxes[] = {__VA_ARGS__}; \
            for (QSpinBox* spinbox : spinboxes) { \
                if (spinbox) synchronizer.addObject(spinbox); \
        } \
    } while (0)

// 通用同步宏（需要手动指定访问器）
#define SYNC_OBJECTS(type, getter, setter, signal, ...) \
        do { \
            static StateSynchronizer<type> synchronizer; \
            synchronizer.setAccessors(getter, setter, signal); \
            type* objects[] = {__VA_ARGS__}; \
            for (type* obj : objects) { \
                if (obj) synchronizer.addObject(obj); \
        } \
    } while (0)

#endif // SYNCHRONIZERMACROS_H
