package com.electricnose.integration;

import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;

@Service
public class WebSocketService {
    
    private final SimpMessagingTemplate messagingTemplate;
    
    public WebSocketService(SimpMessagingTemplate messagingTemplate) {
        this.messagingTemplate = messagingTemplate;
    }
    
    public void sendSensorData(String deviceId, Object data) {
        messagingTemplate.convertAndSend("/topic/sensor-data/" + deviceId, data);
    }
    
    public void sendDeviceStatus(String deviceId, Object status) {
        messagingTemplate.convertAndSend("/topic/device-status/" + deviceId, status);
    }
    
    public void sendBroadcast(Object message) {
        messagingTemplate.convertAndSend("/topic/broadcast", message);
    }
    
}
