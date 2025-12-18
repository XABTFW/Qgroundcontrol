#include "swarm.h"
#include <QDebug>

Swarm::Swarm(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Swarm::handleGpsData);
    m_timer->start(1000); // 每秒处理一次 GPS 数据

    // 初始化位置和速度数据
    m_positions.resize(3);
    m_velocities.resize(3);
}

Swarm::~Swarm()
{
    delete m_timer;
}

void Swarm::sendGpsRawInt(uint64_t time_usec, uint8_t fix_type, int32_t lat, int32_t lon, int32_t alt,
                          uint16_t eph, uint16_t epv, uint16_t vel, uint16_t cog, uint8_t satellites_visible,
                          int32_t alt_ellipsoid, uint32_t h_acc, uint32_t v_acc, uint32_t vel_acc, uint32_t hdg_acc, uint16_t yaw)
{
    mavlink_message_t msg;
    mavlink_msg_gps_raw_int_pack(1, 1, &msg, time_usec, fix_type, lat, lon, alt, eph, epv, vel, cog, satellites_visible, alt_ellipsoid, h_acc, v_acc, vel_acc, hdg_acc, yaw);
    // 通过 QGC 或其他地面站发送数据
    qDebug() << "Sending GPS data: " << msg.msgid;
}

void Swarm::handleGpsData(const mavlink_message_t &msg)
{
    if (msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {
        mavlink_gps_raw_int_t gpsData;
        mavlink_msg_gps_raw_int_decode(&msg, &gpsData);
        int aircraftId = 1; // 默认飞机编号
        emit gpsDataReceived(aircraftId, gpsData);

        // 将接收到的 GPS 数据保存到相应的 UAV 数据结构中
        m_positions[aircraftId].setX(gpsData.lat / 10000000.0);  // 转换为浮动精度经纬度
        m_positions[aircraftId].setY(gpsData.lon / 10000000.0);
        m_positions[aircraftId].setZ(gpsData.alt / 1000.0);  // 高度单位米

        m_velocities[aircraftId].setX(gpsData.vel / 100.0); // 转换为米/秒
        m_velocities[aircraftId].setY(gpsData.vel / 100.0); // 这里只是示例，可以根据不同的坐标轴分配不同的值
        m_velocities[aircraftId].setZ(gpsData.vel / 100.0);
    }
}
