package com.electricnose.api;

import org.springframework.web.bind.annotation.*;
import org.springframework.http.ResponseEntity;
import java.util.List;
import java.util.ArrayList;

@RestController
@RequestMapping("/api/data")
@CrossOrigin(origins = "*")
public class DataController {

    @GetMapping("/sensors/{deviceId}")
    public ResponseEntity<List<SensorDataResponse>> getSensorData(
            @PathVariable String deviceId,
            @RequestParam(required = false) String startDate,
            @RequestParam(required = false) String endDate) {
        // TODO: Implement get sensor data logic
        List<SensorDataResponse> data = new ArrayList<>();
        return ResponseEntity.ok(data);
    }

    @GetMapping("/sensors/{deviceId}/latest")
    public ResponseEntity<SensorDataResponse> getLatestSensorData(@PathVariable String deviceId) {
        // TODO: Implement get latest sensor data logic
        SensorDataResponse data = new SensorDataResponse();
        data.setDeviceId(deviceId);
        data.setTemperature(25.5);
        data.setHumidity(60.0);
        data.setTimestamp(System.currentTimeMillis());
        return ResponseEntity.ok(data);
    }

    @PostMapping("/sensors/{deviceId}")
    public ResponseEntity<SensorDataResponse> createSensorData(
            @PathVariable String deviceId,
            @RequestBody CreateSensorDataRequest request) {
        // TODO: Implement create sensor data logic
        SensorDataResponse data = new SensorDataResponse();
        data.setDeviceId(deviceId);
        data.setTemperature(request.getTemperature());
        data.setHumidity(request.getHumidity());
        data.setTimestamp(System.currentTimeMillis());
        return ResponseEntity.ok(data);
    }

    // DTOs
    public static class SensorDataResponse {
        private String deviceId;
        private Double temperature;
        private Double humidity;
        private Long timestamp;
        private String gasLevel;
        
        // Getters and setters
        public String getDeviceId() { return deviceId; }
        public void setDeviceId(String deviceId) { this.deviceId = deviceId; }
        public Double getTemperature() { return temperature; }
        public void setTemperature(Double temperature) { this.temperature = temperature; }
        public Double getHumidity() { return humidity; }
        public void setHumidity(Double humidity) { this.humidity = humidity; }
        public Long getTimestamp() { return timestamp; }
        public void setTimestamp(Long timestamp) { this.timestamp = timestamp; }
        public String getGasLevel() { return gasLevel; }
        public void setGasLevel(String gasLevel) { this.gasLevel = gasLevel; }
    }

    public static class CreateSensorDataRequest {
        private Double temperature;
        private Double humidity;
        private String gasLevel;
        
        // Getters and setters
        public Double getTemperature() { return temperature; }
        public void setTemperature(Double temperature) { this.temperature = temperature; }
        public Double getHumidity() { return humidity; }
        public void setHumidity(Double humidity) { this.humidity = humidity; }
        public String getGasLevel() { return gasLevel; }
        public void setGasLevel(String gasLevel) { this.gasLevel = gasLevel; }
    }
}
