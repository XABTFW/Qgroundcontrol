import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QGroundControl
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.Controllers 1.0  // 确保已经注册了 Swarm 类型

AnalyzePage {
    id: mavlinkConsolePage
    pageName: qsTr("Swarm")
    pageDescription: qsTr("Swarm UAV status display")

    property bool isLoaded: false

    // 实例化 Swarm 对象，供 QML 页面使用
    Swarm {
        id: swarm
    }

    // 页面内容组件
    pageComponent: pageComponent

    Component {
        id: pageComponent

        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelHeight
            width: availableWidth
            height: availableHeight

            // 页面标题
            QGCLabel {
                text: qsTr("Swarm UAV Status")
                font.pointSize: ScreenTools.largeFontPointSize
            }

            // UAV1 状态显示
            QGCLabel {
                text: qsTr("UAV1")
                font.pointSize: ScreenTools.mediumFontPointSize
            }

            QGCLabel {
                text: qsTr("Position:")
                font.pointSize: ScreenTools.smallFontPointSize
            }

            RowLayout {
                QGCLabel { text: qsTr("px:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.x[0].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("py:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.y[0].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("pz:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.z[0].toFixed(2)
                    width: 100
                }
            }

            QGCLabel {
                text: qsTr("Velocity:")
                font.pointSize: ScreenTools.smallFontPointSize
            }

            RowLayout {
                QGCLabel { text: qsTr("vx:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vx[0].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("vy:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vy[0].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("vz:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vz[0].toFixed(2)
                    width: 100
                }
            }

            // UAV2 状态显示
            QGCLabel {
                text: qsTr("UAV2")
                font.pointSize: ScreenTools.mediumFontPointSize
            }

            QGCLabel {
                text: qsTr("Position:")
                font.pointSize: ScreenTools.smallFontPointSize
            }

            RowLayout {
                QGCLabel { text: qsTr("px:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.x[1].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("py:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.y[1].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("pz:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.z[1].toFixed(2)
                    width: 100
                }
            }

            QGCLabel {
                text: qsTr("Velocity:")
                font.pointSize: ScreenTools.smallFontPointSize
            }

            RowLayout {
                QGCLabel { text: qsTr("vx:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vx[1].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("vy:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vy[1].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("vz:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vz[1].toFixed(2)
                    width: 100
                }
            }

            // UAV3 状态显示
            QGCLabel {
                text: qsTr("UAV3")
                font.pointSize: ScreenTools.mediumFontPointSize
            }

            QGCLabel {
                text: qsTr("Position:")
                font.pointSize: ScreenTools.smallFontPointSize
            }

            RowLayout {
                QGCLabel { text: qsTr("px:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.x[2].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("py:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.y[2].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("pz:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.z[2].toFixed(2)
                    width: 100
                }
            }

            QGCLabel {
                text: qsTr("Velocity:")
                font.pointSize: ScreenTools.smallFontPointSize
            }

            RowLayout {
                QGCLabel { text: qsTr("vx:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vx[2].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("vy:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vy[2].toFixed(2)
                    width: 100
                }
                QGCLabel { text: qsTr("vz:") }
                QGCTextField {
                    readOnly: true
                    text: swarm.vz[2].toFixed(2)
                    width: 100
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("[SwarmPage] 页面加载完成，swarm =", swarm)
    }
}
