# Smart LED 
  光敏获取环境灯光，通过PWM自动调节灯亮度。（也可手动进行控制，如红外，按键，同时搭建人体检测模块）  
  APP通过WIFI进行通信，使用MQTT协议，下位机发布主题 slapp/123456，上位机发布主题sldev/123456，然后各自订阅即可。可展示下位机信息、对下位机进行控制等。  
  （代码均为问题）
