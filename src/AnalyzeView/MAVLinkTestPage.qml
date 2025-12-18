import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Palette
import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.Controllers 1.0  // 注册的 Mavlinktest 类型在这里导入

AnalyzePage {
    id: mavlinkConsolePage
    pageName: qsTr("Mavlink Test")
    pageDescription: qsTr("Test sending and receiving MAVLink commands.")

    property bool isLoaded: false

    // ✅ 实例化 Mavlinktest 对象，供 QML 页面使用
    Mavlinktest {
        id: test_mavlink
    }

    // ✅ 主页面内容组件
    pageComponent: pageComponent

    Component {
        id: pageComponent

        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelHeight
            width: availableWidth
            height: availableHeight

            // 标题
            QGCLabel {
                text: qsTr("MAVLink 命令发送")
                font.pointSize: ScreenTools.largeFontPointSize
            }

            // 输入参数区域
            QGCTextField {
                id: textEdit
                Layout.fillWidth: true
                placeholderText: qsTr("请输入参数1 (test1)")
            }

            QGCTextField {
                id: textEdit1
                Layout.fillWidth: true
                placeholderText: qsTr("请输入参数2 (test2)")
            }

            QGCTextField {
                id: textEdit2
                Layout.fillWidth: true
                placeholderText: qsTr("请输入参数3 (test3)")
            }

            // 发送按钮
            QGCButton {
                text: qsTr("发送")
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    test_mavlink._sendcom(textEdit.text, textEdit1.text, textEdit2.text)
                }
            }

            // 分隔线
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: qgcPal.text
                opacity: 0.2
            }

            // 显示接收到的数据
            QGCLabel {
                text: qsTr("接收到的数据")
                font.pointSize: ScreenTools.mediumFontPointSize
            }

            QGCLabel {
                text: qsTr("test1:")
            }
            QGCTextField {
                Layout.fillWidth: true
                readOnly: true
                text: test_mavlink.test1
            }

            QGCLabel {
                text: qsTr("test2:")
            }
            QGCTextField {
                Layout.fillWidth: true
                readOnly: true
                text: test_mavlink.test2
            }

            QGCLabel {
                text: qsTr("test3:")
            }
            QGCTextField {
                Layout.fillWidth: true
                readOnly: true
                text: test_mavlink.test3
            }
        }
    }

    Component.onCompleted: {
        console.log("[MavlinkTestPage] 页面加载完成，test_mavlink =", test_mavlink)
    }
}
