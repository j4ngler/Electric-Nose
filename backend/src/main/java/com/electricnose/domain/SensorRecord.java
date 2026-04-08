package com.electricnose.domain;

import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.AllArgsConstructor;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Document;
import org.springframework.data.mongodb.core.index.Indexed;

import java.time.LocalDateTime;

@Document(collection = "sensor_records")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class SensorRecord {
    
    @Id
    private String id;
    
    @Indexed
    private String deviceId;
    
    private Double temperature;
    
    private Double humidity;
    
    private String gasLevel;
    
    private Double pressure;
    
    private Double light;
    
    private String rawData;
    
    @Indexed
    private LocalDateTime timestamp;
    
    private LocalDateTime createdAt;
    
    public SensorRecord(String deviceId, Double temperature, Double humidity, String gasLevel) {
        this.deviceId = deviceId;
        this.temperature = temperature;
        this.humidity = humidity;
        this.gasLevel = gasLevel;
        this.timestamp = LocalDateTime.now();
        this.createdAt = LocalDateTime.now();
    }
}
