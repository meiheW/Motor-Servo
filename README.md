<div align="center">
    <h1>
    	Motor-Servo
	</h1>
</div>

## About

基于Arm-Linux实现伺服电机伺服驱动，主要硬件包括arm板、伺服电机、电机控制器、编码器、激光控制板等。


## Catalogue



```
| CgicInfo
| ---- info.c                        // 电机应用程序信息
| ---- platop.c                      // 电机运行操作
| ---- platforminfo.c                // arm平台信息
| ---- www                           // 提供网页访问方式
| Manuals
| ---- KruskalMST                    // arm、控制器以及激光控制板的文档
| Doucuments
| ---- Demo                          // 激光控制板示例程序
| ---- Instruction                   // 安装调试电机文档
| Motor-Client-UI
| ---- WinApplication                // Windows下电机调试界面
| Motor-Servo
| ---- motor-servo                   // Arm-Linux电机运行应用程序
| Motor-Term
| ---- motor-term                    // Linux下电机调试应用程序
| Scripts
| ---- auto_start.sh                 // 程序自启动脚本
| ---- fstab                         // SD卡装载示例
| ---- hold.sh                       // 程序保活脚本
| ---- info                          // 设备配置信息
| ---- sumangle.txt                  // 电机运行总角度记录


```
