/****************************************************************************
** Meta object code from reading C++ file 'componenteditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../componenteditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'componenteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ComponentEditor_t {
    QByteArrayData data[22];
    char stringdata[332];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ComponentEditor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ComponentEditor_t qt_meta_stringdata_ComponentEditor = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 21),
QT_MOC_LITERAL(2, 38, 0),
QT_MOC_LITERAL(3, 39, 25),
QT_MOC_LITERAL(4, 65, 25),
QT_MOC_LITERAL(5, 91, 24),
QT_MOC_LITERAL(6, 116, 25),
QT_MOC_LITERAL(7, 142, 27),
QT_MOC_LITERAL(8, 170, 25),
QT_MOC_LITERAL(9, 196, 4),
QT_MOC_LITERAL(10, 201, 4),
QT_MOC_LITERAL(11, 206, 10),
QT_MOC_LITERAL(12, 217, 11),
QT_MOC_LITERAL(13, 229, 7),
QT_MOC_LITERAL(14, 237, 12),
QT_MOC_LITERAL(15, 250, 11),
QT_MOC_LITERAL(16, 262, 5),
QT_MOC_LITERAL(17, 268, 15),
QT_MOC_LITERAL(18, 284, 11),
QT_MOC_LITERAL(19, 296, 3),
QT_MOC_LITERAL(20, 300, 6),
QT_MOC_LITERAL(21, 307, 24)
    },
    "ComponentEditor\0on_pushButton_clicked\0"
    "\0on_BtnColorRemove_clicked\0"
    "on_pushButtonSwap_clicked\0"
    "on_pushButtonNew_clicked\0"
    "on_pushButtonCopy_clicked\0"
    "on_pushButtonRemove_clicked\0"
    "on_pushButtonMode_clicked\0open\0save\0"
    "activateUi\0clearColors\0showAll\0"
    "toggleHidden\0voxelChosen\0_text\0"
    "updateVoxelData\0cellChanged\0row\0column\0"
    "on_pushButtonAtr_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ComponentEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x08 /* Private */,
       3,    0,  100,    2, 0x08 /* Private */,
       4,    0,  101,    2, 0x08 /* Private */,
       5,    0,  102,    2, 0x08 /* Private */,
       6,    0,  103,    2, 0x08 /* Private */,
       7,    0,  104,    2, 0x08 /* Private */,
       8,    0,  105,    2, 0x08 /* Private */,
       9,    0,  106,    2, 0x08 /* Private */,
      10,    0,  107,    2, 0x08 /* Private */,
      11,    0,  108,    2, 0x08 /* Private */,
      12,    0,  109,    2, 0x08 /* Private */,
      13,    0,  110,    2, 0x08 /* Private */,
      14,    0,  111,    2, 0x08 /* Private */,
      15,    1,  112,    2, 0x08 /* Private */,
      17,    0,  115,    2, 0x08 /* Private */,
      18,    2,  116,    2, 0x08 /* Private */,
      21,    0,  121,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   19,   20,
    QMetaType::Void,

       0        // eod
};

void ComponentEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ComponentEditor *_t = static_cast<ComponentEditor *>(_o);
        switch (_id) {
        case 0: _t->on_pushButton_clicked(); break;
        case 1: _t->on_BtnColorRemove_clicked(); break;
        case 2: _t->on_pushButtonSwap_clicked(); break;
        case 3: _t->on_pushButtonNew_clicked(); break;
        case 4: _t->on_pushButtonCopy_clicked(); break;
        case 5: _t->on_pushButtonRemove_clicked(); break;
        case 6: _t->on_pushButtonMode_clicked(); break;
        case 7: _t->open(); break;
        case 8: _t->save(); break;
        case 9: _t->activateUi(); break;
        case 10: _t->clearColors(); break;
        case 11: _t->showAll(); break;
        case 12: _t->toggleHidden(); break;
        case 13: _t->voxelChosen((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->updateVoxelData(); break;
        case 15: _t->cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 16: _t->on_pushButtonAtr_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject ComponentEditor::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ComponentEditor.data,
      qt_meta_data_ComponentEditor,  qt_static_metacall, 0, 0}
};


const QMetaObject *ComponentEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ComponentEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ComponentEditor.stringdata))
        return static_cast<void*>(const_cast< ComponentEditor*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ComponentEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
