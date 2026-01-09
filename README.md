# 🏠 IoT Temperature and humidity environment monitoring system (温湿度环境监控系统)

> 一个基于 **STM32** (嵌入式端) 和 **Spring Boot** (服务端) 的全栈物联网项目，实现了环境数据的实时采集、传输、存储与可视化监控。

## 📸 项目展示 (Project Showcase)
*当温度小于阈值（26°C）时，UI显示风扇状态为待机中。
*<img width="1531" height="1128" alt="image" src="https://github.com/user-attachments/assets/fa02243b-70e6-4ca7-a9a5-84eaecbfb37f" />
*当温度超过阈值（26°C）时，UI 自动变红报警并开启降温。
*<img width="1542" height="1111" alt="image" src="https://github.com/user-attachments/assets/88a21e37-02c3-40f8-ad07-612d72e373d8" />

## ✨ 主要功能 (Key Features)

* **🔄 端到端数据流**：从硬件传感器感知到网页前端展示的完整链路打通。
* **📡 无线实时传输**：利用 ESP8266 WiFi 模块，通过 TCP 协议将 STM32 采集的数据实时发送至云/本地服务器。
* **💾 数据持久化**：自定义 Java Socket 服务器接收数据，并利用 JDBC/JPA 将历史数据存入 MySQL 数据库。
* **📊 动态可视化**：基于 ECharts 实现温湿度数据的实时折线图展示，支持动态刷新。
* **🚨 智能报警逻辑**：前端集成边缘规则判断，当温度超过阈值（26°C）时，UI 自动变红报警并开启风扇。
* **🌐 远程访问**：支持内网穿透（cpolar），实现随时随地远程监控。

## 🛠️ 技术栈 (Tech Stack)

### 🖥️ 硬件端 (Embedded System)
* **主控芯片**: STM32F103C8T6
* **传感器**: DHT11 (温湿度传感器)
* **通信模块**: ESP8266 (WiFi, TCP/IP 协议)
* **开发环境**: Keil uVision 5, C Language

### ☁️ 服务端 (Backend)
* **核心框架**: Spring Boot
* **网络编程**: Java Socket (多线程处理)
* **数据库**: MySQL
* **ORM框架**: Spring Data JPA / MyBatis
* **开发工具**: IntelliJ IDEA

### 💻 前端 (Frontend)
* **基础**: HTML5, CSS3, JavaScript
* **可视化**: Apache ECharts 5
* **交互**: Fetch API (AJAX Polling)

## 🚀 快速开始 (Quick Start)

1.  **硬件连接**：将 DHT11 连接至 STM32 的 PBxx 引脚，ESP8266 连接至串口。
2.  **数据库配置**：在 MySQL 中创建 `iot_db` 数据库，并导入 SQL 脚本。
3.  **启动后端**：修改 `application.properties` 中的数据库配置，运行 Spring Boot 主程序。
4.  **访问页面**：浏览器打开 `http://localhost:8081` 即可看到监控大屏。

## 🔌 硬件接线说明 (Wiring Instructions)

| 模块 (Module) | 引脚 (Pin) | STM32 连接 (STM32 Pin) | 备注 (Notes) |
| :--- | :--- | :--- | :--- |
| **DHT11** | VCC | 3.3V | 温湿度传感器电源 |
| | GND | GND | 共地 |
| | DAT | **PA8** | 数据引脚 |
| **ESP8266** | VCC | 3.3V | **⚠️ 注意：严禁接 5V** |
| | GND | GND | 共地 |
| | RX | **PA2** (USART2_TX) | STM32 发送 -> ESP 接收 |
| | TX | **PA3** (USART2_RX) | STM32 接收 <- ESP 发送 |
| | EN/CH_PD | 3.3V | 使能脚 (部分模块需接) |
| **继电器 (Relay)** | VCC | 5V | 继电器线圈供电 |
| | GND | GND | 共地 |
| | IN | **PA4** | 控制信号 (高/低电平触发) |
| **风扇 (Fan)** | 正极 (Red) | 继电器 NO (常开端) | 被控回路 |
| | 负极 (Black)| GND | 被控回路 |
| **继电器公共端 (COM)**| COM | 5V | 风扇电源输入 |
---
*Created by [锦华]*
