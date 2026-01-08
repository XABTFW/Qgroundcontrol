#pragma once

#include "MAVLinkProtocol.h"
#include "Vehicle.h"
#include "QmlObjectListModel.h"
#include "Fact.h"
#include "FactMetaData.h"
#include <QObject>
#include <QString>
#include <QMetaObject>
#include <QStringListModel>

#include <ParameterManager.h>
#include <FactMetaData.h>
// Fordward decls
class Vehicle;

/// Controller for MavlinkConsole.qml.
class Mavlinktest2 : public QStringListModel
{
    Q_OBJECT

public:
    Mavlinktest2();
    virtual ~Mavlinktest2();

    Q_INVOKABLE void sendCommand(QString command);
    Q_INVOKABLE void _sendcom(uint8_t test1,uint8_t test2,uint8_t test3,uint32_t pause, uint32_t conti);
    Q_INVOKABLE void _sendcom2(uint8_t test1,uint8_t test2,uint8_t test3,uint32_t pause, uint32_t conti);

    Q_INVOKABLE QString historyUp(const QString& current);
    Q_INVOKABLE QString historyDown(const QString& current);
    Q_INVOKABLE void set_main_airplane(int id, float x,float y,float z);
    Q_INVOKABLE void caculate_pos(int sysid,float x,float y,float z);

    Q_PROPERTY(QString          test1       READ test1      NOTIFY valueChanged)
    QString         test1           ()
    {
        return _test1;
    }

    Q_PROPERTY(QString          test2       READ test2      NOTIFY valueChanged)
    QString         test2           ()
    {
        return _test2;
    }

    Q_PROPERTY(QString          test3       READ test3      NOTIFY valueChanged)
    QString         test3           ()
    {
        return _test3;
    }

private slots:
    void _setActiveVehicle  (Vehicle* vehicle);
    void _receiveData(uint8_t device, uint8_t flags, uint16_t timeout, uint32_t baudrate, QByteArray data);
    void _receiveMessage            (LinkInterface* link, mavlink_message_t message);
signals:
    void            valueChanged        ();
    void selectedChanged                ();
    // 集群操作确认信号
    void swarmOperationAckReceived(int sysId, int opType, int result, int oldValue, int newValue, QString message);

private:
    bool _processANSItext(QByteArray &line);
    void _sendSerialData(QByteArray, bool close = false);
    void writeLine(int line, const QByteArray &text);

    class CommandHistory
    {
    public:
        void append(const QString& command);
        QString up(const QString& current);
        QString down(const QString& current);
    private:
        static constexpr int maxHistoryLength = 100;
        QList<QString> _history;
        int _index = 0;
    };
    QString     _test1,_test2,_test3;
    int           _cursor_home_pos;
    int           _cursor;
    QByteArray    _incoming_buffer;
    Vehicle*      _vehicle;
    QList<QMetaObject::Connection> _uas_connections;
    CommandHistory _history;

    QVector<float> vec_;
    QMap<int,QVector<float>> airplane_pos;
    int main_airplane = -1;
};
