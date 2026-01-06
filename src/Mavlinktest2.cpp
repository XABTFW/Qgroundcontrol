#include "Mavlinktest2.h"
//#include "QGCApplication.h"
//#include "UAS.h"
#include "MAVLinkInspectorController.h"
#include "mavlink_msg_uav_info.h"
#include "MultiVehicleManager.h"
#include <QtCharts/QLineSeries>
#include<iostream>
#include <QtConcurrent/QtConcurrent>

using namespace std;
Mavlinktest2::Mavlinktest2()
    : QStringListModel(),
      _cursor_home_pos{-1},
      _cursor{0},
      _vehicle{nullptr}
{
   // auto *manager = qgcApp()->toolbox()->multiVehicleManager();
    connect(MultiVehicleManager::instance(), &MultiVehicleManager::activeVehicleChanged, this, &Mavlinktest2::_setActiveVehicle);
    _setActiveVehicle(MultiVehicleManager::instance()->activeVehicle());
    MAVLinkProtocol* mavlinkProtocol = MAVLinkProtocol::instance();
    connect(mavlinkProtocol, &MAVLinkProtocol::messageReceived, this, &Mavlinktest2::_receiveMessage);
}

Mavlinktest2::~Mavlinktest2()
{
    if (_vehicle)
    {
        QByteArray msg;
        _sendSerialData(msg, true);
    }
}

void
Mavlinktest2::sendCommand(QString command)
{
    _history.append(command);
    command.append("\n");
    _sendSerialData(qPrintable(command));
    _cursor_home_pos = -1;
    _cursor = rowCount();
}

QString
Mavlinktest2::historyUp(const QString& current)
{
    return _history.up(current);
}

QString
Mavlinktest2::historyDown(const QString& current)
{
    return _history.down(current);
}

void
Mavlinktest2::_setActiveVehicle(Vehicle* vehicle)
{
    for (auto &con : _uas_connections)
    {
        disconnect(con);
    }
    _uas_connections.clear();

    _vehicle = vehicle;
    if (vehicle)
        qDebug()<<__FUNCTION__<<vehicle->id()<<_vehicle->parameterManager();
    if (_vehicle)
    {
        _incoming_buffer.clear();
        // Reset the model
        setStringList(QStringList());
        _cursor = 0;
        _cursor_home_pos = -1;
        _uas_connections << connect(_vehicle, &Vehicle::mavlinkSerialControl, this, &Mavlinktest2::_receiveData);
    }
}

void
Mavlinktest2::_receiveData(uint8_t device, uint8_t, uint16_t, uint32_t, QByteArray data)
{
    if (device != SERIAL_CONTROL_DEV_SHELL)
    {
        return;
    }
    // auto idx = index(_cursor);
    //setData(idx,  QString("%1 ttyS6 -> * [%2]").arg(QTime::currentTime().toString("HH:mm:ss.zzz")).arg(12));


            // Append incoming data and parse for ANSI codes
    _incoming_buffer.append(data);
    while(!_incoming_buffer.isEmpty())
    {
        bool newline = false;
        int idx = _incoming_buffer.indexOf('\n');
        if (idx == -1)
        {
            // Read the whole incoming buffer
            idx = _incoming_buffer.size();
        }
        else
        {
            newline = true;
        }

        QByteArray fragment = _incoming_buffer.mid(0, idx);
        if (_processANSItext(fragment))
        {
            writeLine(_cursor, fragment);
            if (newline)
            {
                _cursor++;
            }
            _incoming_buffer.remove(0, idx + (newline ? 1 : 0));
        }
        else
        {
            // ANSI processing failed, need more data
            return;
        }
    }
}

void
Mavlinktest2::_receiveMessage(LinkInterface*, mavlink_message_t message)
{


    // if( message.msgid == MAVLINK_MSG_ID_ALTITUDE) {
    //     qDebug()<<"message.msgid"<<MAVLINK_MSG_ID_ALTITUDE<<__LINE__;
    // }
    //  qDebug()<<message.msgid<<MAVLINK_MSG_ID_TEST_MAVLINK;

    if(message.msgid==MAVLINK_MSG_ID_UAV_INFO)
    {
        if(!_vehicle)return;
        WeakLinkInterfacePtr weakLink = _vehicle->vehicleLinkManager()->primaryLink();

        if (!weakLink.expired()) {
            SharedLinkInterfacePtr sharedLink = weakLink.lock();

            if (!sharedLink) {
                qCDebug(VehicleLog) << "_handlePing: primary link gone!";
                return;
            }
            auto priority_link =sharedLink;


            mavlink_uav_info_t mavlink_uavinfo;
            mavlink_message_t msg;
            mavlink_msg_uav_info_decode(&message, &mavlink_uavinfo);
            mavlink_msg_uav_info_pack_chan(static_cast<uint8_t>(MAVLinkProtocol::instance()->getSystemId()),
                                           static_cast<uint8_t>(MAVLinkProtocol::getComponentId()),
                                           priority_link->mavlinkChannel(),
                                           &msg,
                                           mavlink_uavinfo.mavid,
                                           mavlink_uavinfo.group_id,
                                           mavlink_uavinfo.is_leader,
                                           mavlink_uavinfo.lat, mavlink_uavinfo.lon,
                                           mavlink_uavinfo.yaw, mavlink_uavinfo.yaw_speed,
                                           mavlink_uavinfo.rel_alt, mavlink_uavinfo.vx, mavlink_uavinfo.vy,
                                           mavlink_uavinfo.vz, mavlink_uavinfo.land);



            _vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
        }

    }
}

void Mavlinktest2::set_main_airplane(int sysid, float x,float y,float z) { // 需要给飞控发送自定义消息 设置主机
    main_airplane = sysid;

    qDebug()<<"set main_airplane "<<sysid<<x<<y<<z;
    vec_.clear();
    vec_.push_back(x);
    vec_.push_back(y);
    vec_.push_back(z);

    airplane_pos.clear();
    airplane_pos[sysid] = vec_;

            //  _sendcom2(sysid);
}

void Mavlinktest2::caculate_pos(int sysid,float x,float y,float z){

    _vehicle = MultiVehicleManager::instance()->activeVehicle();
    // qDebug()<<x<<y<<z<<_vehicle->parameterManager()<<sysid;

    if(_vehicle->parameterManager() && sysid == _vehicle->id()) {
        _vehicle->parameterManager()->myswarm_param_send(sysid, "SWARM_X_OFFSET", FactMetaData::valueTypeFloat, x);
        _vehicle->parameterManager()->myswarm_param_send(sysid, "SWARM_Y_OFFSET", FactMetaData::valueTypeFloat, y);
        _vehicle->parameterManager()->myswarm_param_send(sysid, "SWARM_Z_OFFSET", FactMetaData::valueTypeFloat, z);
    }
}

//
void Mavlinktest2::_sendcom(uint8_t test1,uint8_t test2,uint8_t test3,uint32_t pause, uint32_t conti) // 改为float
{
    if (!_vehicle)
    {
        qWarning() << "Internal error";
        return;
    }

    WeakLinkInterfacePtr weakLink = _vehicle->vehicleLinkManager()->primaryLink();
    if (!weakLink.expired()) {
        SharedLinkInterfacePtr sharedLink = weakLink.lock();

        if (!sharedLink) {
            qCDebug(VehicleLog) << "_handlePing: primary link gone!";
            return;
        }
        //            auto protocol = qgcApp()->toolbox()->mavlinkProtocol();
        auto priority_link =sharedLink;

                //  uint8_t send_test1=test1.toUInt();  不用强制转换了
                //  int16_t send_test2=test2.toShort();
        //   float send_test3=test3.toFloat();

        mavlink_message_t msg;
        // mavlink_msg_swarm_start_flag_pack_chan(_vehicle->id(), //将test_mavlink 也改掉试试
        //                                    1,
        //                                    priority_link->mavlinkChannel(),
        //                                    &msg,
        //                                    test1,
        //                                    test2,
        //                                    test3,pause,conti);

        //    mavlink_msg_swarm_start_flag_pack_chan(55, //将test_mavlink 也改掉试试
        //                                            55,
        //                                            priority_link->mavlinkChannel(),
        //                                            &msg,
        //                                            test1,
        //                                            test2,
        //                                            test3,pause,conti);


        mavlink_msg_swarm_start_flag_pack_chan(static_cast<uint8_t>(MAVLinkProtocol::instance()->getSystemId()),
                                               static_cast<uint8_t>(MAVLinkProtocol::getComponentId()),
                                               priority_link->mavlinkChannel(),
                                               &msg,
                                               test1,
                                               test2,
                                               test3,pause,conti);


        _vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
        qDebug()<<__FUNCTION__<<test1<<test2<<test3<<pause<<conti;
    }
}



void Mavlinktest2::_sendcom2(uint8_t test1,uint8_t test2,uint8_t test3,uint32_t pause, uint32_t conti) // 改为float
{
    if (!_vehicle)
    {
        qWarning() << "Internal error";
        return;
    }

    WeakLinkInterfacePtr weakLink = _vehicle->vehicleLinkManager()->primaryLink();
    if (!weakLink.expired()) {
        SharedLinkInterfacePtr sharedLink = weakLink.lock();

        if (!sharedLink) {
            qCDebug(VehicleLog) << "_handlePing: primary link gone!";
            return;
        }
        //            auto protocol = qgcApp()->toolbox()->mavlinkProtocol();
        auto priority_link =sharedLink;

        mavlink_uav_info_t mavlink_uavinfo;
        mavlink_message_t msg;
        mavlink_msg_uav_info_pack_chan(static_cast<uint8_t>(MAVLinkProtocol::instance()->getSystemId()),
                                       static_cast<uint8_t>(MAVLinkProtocol::getComponentId()),
                                       priority_link->mavlinkChannel(),
                                       &msg,
                                       mavlink_uavinfo.mavid,
                                       mavlink_uavinfo.group_id,
                                       mavlink_uavinfo.is_leader,
                                       mavlink_uavinfo.lat, mavlink_uavinfo.lon,
                                       mavlink_uavinfo.yaw, mavlink_uavinfo.yaw_speed,
                                       mavlink_uavinfo.rel_alt, mavlink_uavinfo.vx, mavlink_uavinfo.vy,
                                       mavlink_uavinfo.vz, mavlink_uavinfo.land);


                // QTimer::singleShot(100, this, [=]() {
                //     _vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
                // });




        _vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);

    }
}


void
Mavlinktest2::_sendSerialData(QByteArray data, bool close)
{
    if (!_vehicle)
    {
        qWarning() << "Internal error";
        return;
    }

    WeakLinkInterfacePtr weakLink = _vehicle->vehicleLinkManager()->primaryLink();
    if (!weakLink.expired()) {
        SharedLinkInterfacePtr sharedLink = weakLink.lock();

        if (!sharedLink) {
            qCDebug(VehicleLog) << "_handlePing: primary link gone!";
            return;
        }


                // Send maximum sized chunks until the complete buffer is transmitted
                //        while(data.size())
                //        {
                //            QByteArray chunk{data.left(MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN)};
                //            uint8_t flags = SERIAL_CONTROL_FLAG_EXCLUSIVE |  SERIAL_CONTROL_FLAG_RESPOND | SERIAL_CONTROL_FLAG_MULTI;
                //            if (close)
                //            {
                //                flags = 0;
                //            }
                //            auto protocol = qgcApp()->toolbox()->mavlinkProtocol();
                //            auto priority_link =sharedLink;
                //            mavlink_message_t msg;



                //            mavlink_msg_serial_control_pack_chan(
                //                protocol->getSystemId(),
                //                protocol->getComponentId(),
                //                priority_link->mavlinkChannel(),
                //                &msg,
                //                SERIAL_CONTROL_DEV_SHELL,
                //                flags,
                //                0,
                //                0,
                //                chunk.size(),
                //                reinterpret_cast<uint8_t*>(chunk.data()));
                //            _vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
                //            data.remove(0, chunk.size());
                //        }
    }




}

bool
Mavlinktest2::_processANSItext(QByteArray &line)
{
    // Iterate over the incoming buffer to parse off known ANSI control codes
    for (int i = 0; i < line.size(); i++)
    {
        if (line.at(i) == '\x1B')
        {
            // For ANSI codes we expect at least 3 incoming chars
            if (i < line.size() - 2 && line.at(i+1) == '[')
            {
                // Parse ANSI code
                switch(line.at(i+2))
                {
                    default:
                        continue;
                    case 'H':
                        if (_cursor_home_pos == -1)
                        {
                            // Assign new home position if home is unset
                            _cursor_home_pos = _cursor;
                        }
                        else
                        {
                            // Rewind write cursor position to home
                            _cursor = _cursor_home_pos;
                        }
                        break;
                    case 'K':
                        // Erase the current line to the end
                        if (_cursor < rowCount())
                        {
                            setData(index(_cursor), "");
                        }
                        break;
                    case '2':
                        // Check for sufficient buffer size
                        if ( i >= line.size() - 3)
                        {
                            return false;
                        }

                        if (line.at(i+3) == 'J' && _cursor_home_pos != -1)
                        {
                            // Erase everything and rewind to home
                            bool blocked = blockSignals(true);
                            for (int j = _cursor_home_pos; j < rowCount(); j++)
                            {
                                setData(index(j), "");
                            }
                            blockSignals(blocked);
                            QVector<int> roles;
                            roles.reserve(2);
                            roles.append(Qt::DisplayRole);
                            roles.append(Qt::EditRole);
                            emit dataChanged(index(_cursor), index(rowCount()), roles);
                        }
                        // Even if we didn't understand this ANSI code, remove the 4th char
                        line.remove(i+3,1);
                        break;
                }
                // Remove the parsed ANSI code and decrement the bufferpos
                line.remove(i, 3);
                i--;
            }
            else
            {
                // We can reasonably expect a control code was fragemented
                // Stop parsing here and wait for it to come in
                return false;
            }
        }
    }
    return true;
}

void
Mavlinktest2::writeLine(int line, const QByteArray &text)
{
    auto rc = rowCount();
    if (line >= rc)
    {
        insertRows(rc, 1 + line - rc);
    }
    auto idx = index(line);
    setData(idx, data(idx, Qt::DisplayRole).toString() + text);
}

void Mavlinktest2::CommandHistory::append(const QString& command)
{
    if (command.length() > 0)
    {

        // do not append duplicates
        if (_history.length() == 0 || _history.last() != command)
        {

            if (_history.length() >= maxHistoryLength)
            {
                _history.removeFirst();
            }
            _history.append(command);
        }
    }
    _index = _history.length();
}

QString Mavlinktest2::CommandHistory::up(const QString& current)
{
    if (_index <= 0)
    {
        return current;
    }

    --_index;
    if (_index < _history.length())
    {
        return _history[_index];
    }
    return "";
}

QString Mavlinktest2::CommandHistory::down(const QString& current)
{
    if (_index >= _history.length())
    {
        return current;
    }

    ++_index;
    if (_index < _history.length())
    {
        return _history[_index];
    }
    return "";
}
