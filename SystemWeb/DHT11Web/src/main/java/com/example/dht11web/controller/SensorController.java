package com.example.dht11web.controller; // 👈 1. 包名必须对

import com.example.dht11web.entity.SensorData;     // 👈 2. 引用你的实体类
import com.example.dht11web.repository.SensorDataRepository; // 👈 3. 引用你的仓库
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController // 👈 4. 这个注解绝对不能少！少了它就报你那个错
@RequestMapping("/api")
public class SensorController {

    @Autowired // 👈 5. 加上 @RestController 后，这里就不报错了
    private SensorDataRepository repository;

    @GetMapping("/data")
    public List<SensorData> getAllData() {
        return repository.findAllByOrderByIdDesc();
    }
}