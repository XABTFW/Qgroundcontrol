#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QByteArray>
#include <QList>
#include <QMetaObject>
#include <QString>
#include "Vehicle.h"
#include "MAVLinkProtocol.h"

class Mavlinktest : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString test1 READ test1 NOTIFY test1Changed)
    Q_PROPERTY(QString test2 READ test2 NOTIFY test2Changed)
    Q_PROPERTY(QString test3 READ test3 NOTIFY test3Changed)

public:
    explicit Mavlinktest(QObject *parent = nullptr);
    ~Mavlinktest() override;

    enum Roles {
        TextRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void sendCommand(QString command);
    Q_INVOKABLE QString historyUp(const QString& current);
    Q_INVOKABLE QString historyDown(const QString& current);
    Q_INVOKABLE void _sendcom(QString test1, QString test2, QString test3);

    QString test1() const { return _test1; }
    QString test2() const { return _test2; }
    QString test3() const { return _test3; }

signals:
    void test1Changed();
    void test2Changed();
    void test3Changed();

private slots:
    void _setActiveVehicle(Vehicle* vehicle);
    void _receiveData(uint8_t device, uint8_t flags, uint16_t timeout, uint32_t baudrate, QByteArray data);
    void _receiveMessage(LinkInterface* link, mavlink_message_t message);

private:
    void _sendSerialData(QByteArray, bool close = false);
    bool _processANSItext(QByteArray &line);
    void writeLine(int line, const QByteArray &text);

    class CommandHistory {
    public:
        void append(const QString& command);
        QString up(const QString& current);
        QString down(const QString& current);
    private:
        static constexpr int maxHistoryLength = 100;
        QList<QString> _history;
        int _index = 0;
    };

    QString _test1, _test2, _test3;
    QList<QString> _lines;
    QByteArray _incoming_buffer;
    int _cursor_home_pos;
    int _cursor;
    Vehicle* _vehicle = nullptr;
    QList<QMetaObject::Connection> _uas_connections;
    CommandHistory _history;
};
