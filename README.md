# Graduation-Project

本人所有程序使用的IDE是VS2013,使用的程序框架是基于对话框的MFC模板
   1、MyRobot文件下是工控机的程序，工控机的操作系统是win7,工控机上有固高的GT_400_SV运动卡控制卡，通过PCI的插槽
   插入主板中，板卡的动态的链接库可以在固高的官方下载。发送信息给上位机，通过TCP/IP服务器
   2、ShowRobotData可以理解为上位机的程序，上位机是我个人的PC,win10系统，作为TCP/IP客户机接受信息
   3、TestPID是工控机上的控制程序，仅仅是测试板卡运动位置环PID参数使用的，后面没有任何作用
   
   
工控机的控制对象是固高的四自由度SCARA教学机器人，固高有例程，我借用了例程中的很多代码，但是为了满足我的控制需要
修改了大部分的代码。