package com.electricnose.service;

import com.electricnose.domain.SensorRecord;
import com.electricnose.repository.SensorRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Service
public class SensorDataService {
    
    @Autowired
    private SensorRepository sensorRepository;
    
    public SensorRecord saveSensorData(String deviceId, Double temperature, Double humidity, String gasLevel) {
        SensorRecord record = new SensorRecord(deviceId, temperature, humidity, gasLevel);
        return sensorRepository.save(record);
    }
    
    public List<SensorRecord> getSensorDataByDeviceId(String deviceId) {
        return sensorRepository.findByDeviceIdOrderByTimestampDesc(deviceId);
    }
    
    public List<SensorRecord> getSensorDataInRange(String deviceId, LocalDateTime start, LocalDateTime end) {
        return sensorRepository.findByDeviceIdAndTimestampBetweenOrderByTimestampDesc(deviceId, start, end);
    }
    
    public Optional<SensorRecord> getLatestSensorData(String deviceId) {
        SensorRecord record = sensorRepository.findFirstByDeviceIdOrderByTimestampDesc(deviceId);
        return Optional.ofNullable(record);
    }
    
    public List<SensorRecord> getAllSensorData() {
        return sensorRepository.findAll();
    }
    
}
