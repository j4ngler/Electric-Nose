package com.electricnose.repository;

import com.electricnose.domain.SensorRecord;
import org.springframework.data.mongodb.repository.MongoRepository;
import org.springframework.data.mongodb.repository.Query;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;

@Repository
public interface SensorRepository extends MongoRepository<SensorRecord, String> {
    
    List<SensorRecord> findByDeviceIdOrderByTimestampDesc(String deviceId);
    
    List<SensorRecord> findByDeviceIdAndTimestampBetweenOrderByTimestampDesc(
        String deviceId, LocalDateTime start, LocalDateTime end);
    
    SensorRecord findFirstByDeviceIdOrderByTimestampDesc(String deviceId);
    
    @Query("{ 'deviceId': ?0, 'timestamp': { $gte: ?1, $lte: ?2 } }")
    List<SensorRecord> findSensorDataInRange(String deviceId, LocalDateTime start, LocalDateTime end);
    
}
