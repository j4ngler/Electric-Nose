package com.electricnose.api;

import org.springframework.web.bind.annotation.*;
import org.springframework.http.ResponseEntity;
import java.util.List;
import java.util.ArrayList;

@RestController
@RequestMapping("/api/devices")
@CrossOrigin(origins = "*")
public class DeviceController {

    @GetMapping
    public ResponseEntity<List<DeviceResponse>> getAllDevices() {
        // TODO: Implement get all devices logic
        List<DeviceResponse> devices = new ArrayList<>();
        return ResponseEntity.ok(devices);
    }

    @GetMapping("/{deviceId}")
    public ResponseEntity<DeviceResponse> getDevice(@PathVariable String deviceId) {
        // TODO: Implement get device by ID logic
        DeviceResponse device = new DeviceResponse();
        device.setId(deviceId);
        device.setName("Sample Device");
        device.setStatus("online");
        return ResponseEntity.ok(device);
    }

    @PostMapping
    public ResponseEntity<DeviceResponse> createDevice(@RequestBody CreateDeviceRequest request) {
        // TODO: Implement create device logic
        DeviceResponse device = new DeviceResponse();
        device.setName(request.getName());
        device.setStatus("offline");
        return ResponseEntity.ok(device);
    }

    @PutMapping("/{deviceId}")
    public ResponseEntity<DeviceResponse> updateDevice(@PathVariable String deviceId, @RequestBody UpdateDeviceRequest request) {
        // TODO: Implement update device logic
        DeviceResponse device = new DeviceResponse();
        device.setId(deviceId);
        device.setName(request.getName());
        return ResponseEntity.ok(device);
    }

    @DeleteMapping("/{deviceId}")
    public ResponseEntity<?> deleteDevice(@PathVariable String deviceId) {
        // TODO: Implement delete device logic
        return ResponseEntity.ok("Device deleted successfully");
    }

    // DTOs
    public static class DeviceResponse {
        private String id;
        private String name;
        private String status;
        private String lastSeen;
        
        // Getters and setters
        public String getId() { return id; }
        public void setId(String id) { this.id = id; }
        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
        public String getStatus() { return status; }
        public void setStatus(String status) { this.status = status; }
        public String getLastSeen() { return lastSeen; }
        public void setLastSeen(String lastSeen) { this.lastSeen = lastSeen; }
    }

    public static class CreateDeviceRequest {
        private String name;
        private String description;
        
        // Getters and setters
        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
        public String getDescription() { return description; }
        public void setDescription(String description) { this.description = description; }
    }

    public static class UpdateDeviceRequest {
        private String name;
        private String description;
        
        // Getters and setters
        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
        public String getDescription() { return description; }
        public void setDescription(String description) { this.description = description; }
    }
}
