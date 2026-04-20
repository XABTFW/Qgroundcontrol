/****************************************************************************
 *
 * 保通防务 启动动画
 * 四旋翼无人机从左下角和右上角飞入，围绕中心旋转后显示标题
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Controls

Rectangle {
    id: splashScreen
    anchors.fill: parent
    color: "#1a1a2e"
    z: 9999

    signal animationFinished()

    property bool _animationComplete: false

    // 无人机1 - 从左下角飞入
    Item {
        id: drone1
        width: parent.width / 8
        height: parent.width / 8
        x: -width
        y: parent.height + height

        Canvas {
            id: droneCanvas1
            anchors.fill: parent
            onPaint: {
                var ctx = getContext("2d")
                drawQuadcopter(ctx, width, height, "#00d4ff")
            }
        }
    }

    // 无人机2 - 从右上角飞入
    Item {
        id: drone2
        width: parent.width / 8
        height: parent.width / 8
        x: parent.width + width
        y: -height

        Canvas {
            id: droneCanvas2
            anchors.fill: parent
            onPaint: {
                var ctx = getContext("2d")
                drawQuadcopter(ctx, width, height, "#ff6b6b")
            }
        }
    }

    // 绘制四旋翼无人机
    function drawQuadcopter(ctx, w, h, color) {
        ctx.clearRect(0, 0, w, h)
        var centerX = w / 2
        var centerY = h / 2
        var armLength = w * 0.35
        var bodySize = w * 0.15
        var rotorSize = w * 0.18

        ctx.strokeStyle = color
        ctx.fillStyle = color
        ctx.lineWidth = 3

        // 机身
        ctx.beginPath()
        ctx.arc(centerX, centerY, bodySize, 0, Math.PI * 2)
        ctx.fill()

        // 四个机臂
        var angles = [Math.PI/4, 3*Math.PI/4, 5*Math.PI/4, 7*Math.PI/4]
        for (var i = 0; i < 4; i++) {
            var angle = angles[i]
            var endX = centerX + Math.cos(angle) * armLength
            var endY = centerY + Math.sin(angle) * armLength

            // 机臂
            ctx.beginPath()
            ctx.moveTo(centerX, centerY)
            ctx.lineTo(endX, endY)
            ctx.stroke()

            // 旋翼
            ctx.beginPath()
            ctx.arc(endX, endY, rotorSize, 0, Math.PI * 2)
            ctx.stroke()
        }
    }

    // 标题文字
    Text {
        id: titleText
        anchors.centerIn: parent
        text: "保通防务"
        font.pixelSize: parent.width / 10
        font.bold: true
        color: "#ffffff"
        opacity: 0
        scale: 0.5
    }

    // 动画序列
    SequentialAnimation {
        id: mainAnimation
        running: true

        // 阶段1: 两架无人机飞向中心
        ParallelAnimation {
            NumberAnimation {
                target: drone1
                property: "x"
                to: splashScreen.width / 2 - drone1.width / 2
                duration: 1200
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                target: drone1
                property: "y"
                to: splashScreen.height / 2 - drone1.height / 2
                duration: 1200
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                target: drone2
                property: "x"
                to: splashScreen.width / 2 - drone2.width / 2
                duration: 1200
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                target: drone2
                property: "y"
                to: splashScreen.height / 2 - drone2.height / 2
                duration: 1200
                easing.type: Easing.OutQuad
            }
        }

        PauseAnimation { duration: 200 }

        // 阶段2: 围绕中心旋转半圈
        ParallelAnimation {
            id: rotationAnimation

            SequentialAnimation {
                NumberAnimation {
                    target: drone1
                    property: "x"
                    to: splashScreen.width / 2 + splashScreen.width / 6
                    duration: 400
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    target: drone1
                    property: "x"
                    to: splashScreen.width / 2 - drone1.width / 2
                    duration: 400
                    easing.type: Easing.InOutSine
                }
            }

            SequentialAnimation {
                NumberAnimation {
                    target: drone1
                    property: "y"
                    to: splashScreen.height / 2 - drone1.height / 2
                    duration: 400
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    target: drone1
                    property: "y"
                    to: splashScreen.height / 2 + splashScreen.height / 6
                    duration: 400
                    easing.type: Easing.InOutSine
                }
            }

            SequentialAnimation {
                NumberAnimation {
                    target: drone2
                    property: "x"
                    to: splashScreen.width / 2 - splashScreen.width / 6
                    duration: 400
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    target: drone2
                    property: "x"
                    to: splashScreen.width / 2 - drone2.width / 2
                    duration: 400
                    easing.type: Easing.InOutSine
                }
            }

            SequentialAnimation {
                NumberAnimation {
                    target: drone2
                    property: "y"
                    to: splashScreen.height / 2 - drone2.height / 2
                    duration: 400
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    target: drone2
                    property: "y"
                    to: splashScreen.height / 2 - splashScreen.height / 6
                    duration: 400
                    easing.type: Easing.InOutSine
                }
            }
        }

        // 阶段3: 无人机散开，标题出现
        ParallelAnimation {
            NumberAnimation {
                target: drone1
                property: "x"
                to: splashScreen.width * 0.15
                duration: 600
                easing.type: Easing.OutBack
            }
            NumberAnimation {
                target: drone1
                property: "y"
                to: splashScreen.height * 0.7
                duration: 600
                easing.type: Easing.OutBack
            }
            NumberAnimation {
                target: drone2
                property: "x"
                to: splashScreen.width * 0.75
                duration: 600
                easing.type: Easing.OutBack
            }
            NumberAnimation {
                target: drone2
                property: "y"
                to: splashScreen.height * 0.2
                duration: 600
                easing.type: Easing.OutBack
            }
            NumberAnimation {
                target: titleText
                property: "opacity"
                to: 1
                duration: 800
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                target: titleText
                property: "scale"
                to: 1
                duration: 800
                easing.type: Easing.OutBack
            }
        }

        PauseAnimation { duration: 1500 }

        // 阶段4: 淡出
        ParallelAnimation {
            NumberAnimation {
                target: splashScreen
                property: "opacity"
                to: 0
                duration: 500
                easing.type: Easing.InQuad
            }
        }

        onFinished: {
            _animationComplete = true
            animationFinished()
        }
    }

    // 点击跳过
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (!_animationComplete) {
                mainAnimation.stop()
                splashScreen.opacity = 0
                _animationComplete = true
                animationFinished()
            }
        }
    }

    // 跳过提示
    Text {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        text: "点击屏幕跳过"
        color: "#666666"
        font.pixelSize: 14
    }
}
