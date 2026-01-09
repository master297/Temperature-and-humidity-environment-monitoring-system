package com.example.dht11web.repository;

import com.example.dht11web.entity.SensorData;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import java.util.List;

public interface SensorDataRepository extends JpaRepository<SensorData, Integer> {

    // 这一行代码的神奇之处：
    // 我们只要写出方法名 "findAllByOrderByIdDesc"，Spring Boot 就会自动帮我们生成 SQL 语句：
    // "SELECT * FROM sensor_data ORDER BY id DESC" (倒序查询，把最新的数据排在前面)
    List<SensorData> findAllByOrderByIdDesc();

    // 如果只想查最新的10条，可以写：
    // List<SensorData> findTop10ByOrderByIdDesc();
}