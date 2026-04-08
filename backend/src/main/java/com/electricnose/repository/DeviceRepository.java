package com.electricnose.repository;

import com.electricnose.domain.Device;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public interface DeviceRepository extends JpaRepository<Device, Long> {
    
    Optional<Device> findByDeviceId(String deviceId);
    
    List<Device> findByUserId(Long userId);
    
    List<Device> findByStatus(Device.Status status);
    
    boolean existsByDeviceId(String deviceId);
    
}
