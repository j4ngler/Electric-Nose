package com.electricnose.service;

import com.electricnose.domain.Device;
import com.electricnose.repository.DeviceRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

@Service
public class DeviceService {
    
    @Autowired
    private DeviceRepository deviceRepository;
    
    public List<Device> getAllDevices() {
        return deviceRepository.findAll();
    }
    
    public Optional<Device> getDeviceById(Long id) {
        return deviceRepository.findById(id);
    }
    
    public Optional<Device> getDeviceByDeviceId(String deviceId) {
        return deviceRepository.findByDeviceId(deviceId);
    }
    
    public Device createDevice(String name, String description, String location) {
        Device device = new Device();
        device.setDeviceId(UUID.randomUUID().toString());
        device.setName(name);
        device.setDescription(description);
        device.setLocation(location);
        device.setStatus(Device.Status.OFFLINE);
        device.setLastSeen(LocalDateTime.now());
        
        return deviceRepository.save(device);
    }
    
    public Device updateDevice(Long id, String name, String description, String location) {
        Optional<Device> deviceOpt = deviceRepository.findById(id);
        if (deviceOpt.isPresent()) {
            Device device = deviceOpt.get();
            device.setName(name);
            device.setDescription(description);
            device.setLocation(location);
            return deviceRepository.save(device);
        }
        throw new RuntimeException("Device not found");
    }
    
    public void deleteDevice(Long id) {
        deviceRepository.deleteById(id);
    }
    
    public Device updateDeviceStatus(String deviceId, Device.Status status) {
        Optional<Device> deviceOpt = deviceRepository.findByDeviceId(deviceId);
        if (deviceOpt.isPresent()) {
            Device device = deviceOpt.get();
            device.setStatus(status);
            device.setLastSeen(LocalDateTime.now());
            return deviceRepository.save(device);
        }
        throw new RuntimeException("Device not found");
    }
    
}
