package com.electricnose.integration;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import jakarta.annotation.PostConstruct;
import jakarta.annotation.PreDestroy;

@Service
public class MqttClientService {
    
    @Value("${mqtt.uri}")
    private String mqttUri;
    
    @Value("${mqtt.username:}")
    private String mqttUsername;
    
    @Value("${mqtt.password:}")
    private String mqttPassword;
    
    private MqttClient mqttClient;
    
    @PostConstruct
    public void init() {
        try {
            String clientId = "electric-nose-backend-" + System.currentTimeMillis();
            mqttClient = new MqttClient(mqttUri, clientId, new MemoryPersistence());
            
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            options.setConnectionTimeout(30);
            options.setKeepAliveInterval(60);
            
            if (!mqttUsername.isEmpty()) {
                options.setUserName(mqttUsername);
            }
            if (!mqttPassword.isEmpty()) {
                options.setPassword(mqttPassword.toCharArray());
            }
            
            mqttClient.connect(options);
            System.out.println("MQTT Client connected to: " + mqttUri);
            
        } catch (MqttException e) {
            System.err.println("Failed to connect to MQTT broker: " + e.getMessage());
        }
    }
    
    @PreDestroy
    public void cleanup() {
        try {
            if (mqttClient != null && mqttClient.isConnected()) {
                mqttClient.disconnect();
                mqttClient.close();
                System.out.println("MQTT Client disconnected");
            }
        } catch (MqttException e) {
            System.err.println("Error disconnecting MQTT client: " + e.getMessage());
        }
    }
    
    public void publish(String topic, String message) {
        try {
            if (mqttClient != null && mqttClient.isConnected()) {
                MqttMessage mqttMessage = new MqttMessage(message.getBytes());
                mqttMessage.setQos(1);
                mqttClient.publish(topic, mqttMessage);
                System.out.println("Published to topic " + topic + ": " + message);
            }
        } catch (MqttException e) {
            System.err.println("Failed to publish message: " + e.getMessage());
        }
    }
    
    public void subscribe(String topic) {
        try {
            if (mqttClient != null && mqttClient.isConnected()) {
                mqttClient.setCallback(new MqttCallback() {
                    @Override
                    public void connectionLost(Throwable cause) {
                        System.err.println("MQTT connection lost: " + cause.getMessage());
                    }
                    
                    @Override
                    public void messageArrived(String topic, MqttMessage message) throws Exception {
                        String payload = new String(message.getPayload());
                        System.out.println("Received from topic " + topic + ": " + payload);
                        // TODO: Process incoming MQTT messages
                    }
                    
                    @Override
                    public void deliveryComplete(IMqttDeliveryToken token) {
                        // Message delivery complete
                    }
                });
                mqttClient.subscribe(topic, 1);
                System.out.println("Subscribed to topic: " + topic);
            }
        } catch (MqttException e) {
            System.err.println("Failed to subscribe to topic: " + e.getMessage());
        }
    }
    
}
