package com.example.dht11web.entity;

import jakarta.persistence.*;
import lombok.Data;
import java.time.LocalDateTime;

@Entity
@Table(name = "sensor_data") // 对应数据库表名
@Data
public class SensorData {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Integer id; // 对应数据库的 id (INT)

    // ⚠️ 这里必须改！对应数据库的 temp 字段
    @Column(name = "temp")
    private Float temp;

    // ⚠️ 这里必须改！对应数据库的 humi 字段
    @Column(name = "humi")
    private Float humi;

    // 对应数据库的 create_time 字段
    // Java里习惯用驼峰命名 (createTime)，通过 @Column 告诉它去数据库找 create_time
    @Column(name = "create_time")
    private LocalDateTime createTime;
}