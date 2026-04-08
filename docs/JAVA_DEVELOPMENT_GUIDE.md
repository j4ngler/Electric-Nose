# HƯỚNG DẪN PHÁT TRIỂN JAVA SPRING BOOT - ELECTRIC-NOSE BACKEND

## 📋 MỤC LỤC
1. [Tại sao chọn Java?](#1-tại-sao-chọn-java)
2. [Yêu cầu hệ thống](#2-yêu-cầu-hệ-thống)
3. [Java Technology Stack](#3-java-technology-stack)
4. [Kiến trúc Spring Boot](#4-kiến-trúc-spring-boot)
5. [Các Service chính](#5-các-service-chính)
6. [API Endpoints](#6-api-endpoints)
7. [Deployment và Infrastructure](#7-deployment-và-infrastructure)
8. [Testing Strategy](#8-testing-strategy)
9. [Monitoring và Logging](#9-monitoring-và-logging)
10. [Performance Optimization](#10-performance-optimization)
11. [Security Configuration](#11-security-configuration)
12. [Cost Estimation](#12-cost-estimation)

---

## 1. TẠI SAO CHỌN JAVA?

### **✅ Ưu điểm của Java cho dự án này:**
- **Mature Ecosystem**: Hệ sinh thái phong phú với Spring Framework
- **Enterprise Ready**: Phù hợp cho hệ thống doanh nghiệp
- **Performance**: Hiệu suất cao và ổn định
- **Security**: Bảo mật mạnh mẽ với Spring Security
- **Scalability**: Dễ dàng mở rộng với microservices
- **Community**: Cộng đồng lớn, nhiều tài liệu và hỗ trợ
- **Cross-platform**: Chạy được trên mọi hệ điều hành

---

## 2. YÊU CẦU HỆ THỐNG

### **☕ Java Runtime Environment (JRE)**
```
Java Version: 17 LTS (Long Term Support)
- OpenJDK 17 hoặc Oracle JDK 17
- Minimum RAM: 2GB
- Recommended RAM: 4GB+
```

### **💻 Development Environment**
```
IDE: IntelliJ IDEA / Eclipse / VS Code
Build Tool: Maven 3.8+ hoặc Gradle 7.0+
Version Control: Git
```

### **🚀 Application Server**
```
Spring Boot Embedded Tomcat (Recommended)
- Port: 8080 (default)
- Memory: 512MB - 2GB
- Alternative: Apache Tomcat, Jetty
```

---

## 3. JAVA TECHNOLOGY STACK

### **🔧 Core Framework**
```xml
<!-- Spring Boot Starter -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-web</artifactId>
    <version>3.2.0</version>
</dependency>

<!-- Spring Security -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-security</artifactId>
    <version>3.2.0</version>
</dependency>

<!-- Spring Data JPA -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-jpa</artifactId>
    <version>3.2.0</version>
</dependency>
```

### **🗄️ Database & Caching**
```xml
<!-- PostgreSQL Driver -->
<dependency>
    <groupId>org.postgresql</groupId>
    <artifactId>postgresql</artifactId>
    <version>42.7.1</version>
</dependency>

<!-- Redis -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-redis</artifactId>
    <version>3.2.0</version>
</dependency>

<!-- MongoDB -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-mongodb</artifactId>
    <version>3.2.0</version>
</dependency>
```

### **📡 MQTT & Messaging**
```xml
<!-- Eclipse Paho MQTT Client -->
<dependency>
    <groupId>org.eclipse.paho</groupId>
    <artifactId>org.eclipse.paho.client.mqttv3</artifactId>
    <version>1.2.5</version>
</dependency>

<!-- Spring Integration MQTT -->
<dependency>
    <groupId>org.springframework.integration</groupId>
    <artifactId>spring-integration-mqtt</artifactId>
    <version>6.2.0</version>
</dependency>
```

### **📊 Report Generation**
```xml
<!-- Apache POI for Excel -->
<dependency>
    <groupId>org.apache.poi</groupId>
    <artifactId>poi-ooxml</artifactId>
    <version>5.2.4</version>
</dependency>

<!-- iText for PDF -->
<dependency>
    <groupId>com.itextpdf</groupId>
    <artifactId>itext7-core</artifactId>
    <version>7.2.5</version>
</dependency>
```

### **🔧 Additional Libraries**
```xml
<!-- JSON Processing -->
<dependency>
    <groupId>com.fasterxml.jackson.core</groupId>
    <artifactId>jackson-databind</artifactId>
    <version>2.16.0</version>
</dependency>

<!-- Validation -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-validation</artifactId>
    <version>3.2.0</version>
</dependency>

<!-- Testing -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-test</artifactId>
    <version>3.2.0</version>
    <scope>test</scope>
</dependency>
```

---

## 4. KIẾN TRÚC SPRING BOOT

### **📁 Project Structure**
```
src/main/java/com/electricnose/
├── ElectricNoseApplication.java
├── config/
│   ├── SecurityConfig.java
│   ├── MqttConfig.java
│   └── DatabaseConfig.java
├── controller/
│   ├── AuthController.java
│   ├── DeviceController.java
│   ├── DataController.java
│   └── ReportController.java
├── service/
│   ├── AuthService.java
│   ├── DeviceService.java
│   ├── DataService.java
│   ├── MqttService.java
│   └── ReportService.java
├── repository/
│   ├── UserRepository.java
│   ├── DeviceRepository.java
│   └── SensorDataRepository.java
├── entity/
│   ├── User.java
│   ├── Device.java
│   └── SensorData.java
├── dto/
│   ├── LoginRequest.java
│   ├── DeviceResponse.java
│   └── SensorDataResponse.java
└── util/
    ├── JwtUtil.java
    ├── MqttUtil.java
    └── ReportUtil.java
```

### **⚙️ Configuration Files**

#### **application.yml**
```yaml
server:
  port: 8080
  servlet:
    context-path: /api

spring:
  application:
    name: electric-nose-backend
  
  datasource:
    url: jdbc:postgresql://localhost:5432/electric_nose
    username: ${DB_USERNAME:postgres}
    password: ${DB_PASSWORD:password}
    driver-class-name: org.postgresql.Driver
  
  jpa:
    hibernate:
      ddl-auto: update
    show-sql: true
    properties:
      hibernate:
        dialect: org.hibernate.dialect.PostgreSQLDialect
  
  data:
    redis:
      host: ${REDIS_HOST:localhost}
      port: ${REDIS_PORT:6379}
      password: ${REDIS_PASSWORD:}
    
    mongodb:
      uri: mongodb://${MONGO_USERNAME:admin}:${MONGO_PASSWORD:password}@localhost:27017/electric_nose_logs

mqtt:
  broker:
    url: ${MQTT_BROKER_URL:tcp://localhost:1883}
    username: ${MQTT_USERNAME:}
    password: ${MQTT_PASSWORD:}
    client-id: electric-nose-backend

jwt:
  secret: ${JWT_SECRET:mySecretKey}
  expiration: 86400000 # 24 hours

logging:
  level:
    com.electricnose: DEBUG
    org.springframework.security: DEBUG
```

---

## 5. CÁC SERVICE CHÍNH

### **🔐 Authentication Service**
```java
@Service
@Transactional
public class AuthService {
    
    @Autowired
    private UserRepository userRepository;
    
    @Autowired
    private PasswordEncoder passwordEncoder;
    
    @Autowired
    private JwtUtil jwtUtil;
    
    public String login(LoginRequest request) {
        User user = userRepository.findByEmail(request.getEmail())
            .orElseThrow(() -> new RuntimeException("User not found"));
        
        if (!passwordEncoder.matches(request.getPassword(), user.getPassword())) {
            throw new RuntimeException("Invalid password");
        }
        
        return jwtUtil.generateToken(user);
    }
    
    public User register(RegisterRequest request) {
        User user = new User();
        user.setEmail(request.getEmail());
        user.setPassword(passwordEncoder.encode(request.getPassword()));
        user.setName(request.getName());
        
        return userRepository.save(user);
    }
}
```

### **📡 MQTT Service**
```java
@Service
public class MqttService {
    
    @Autowired
    private MqttTemplate mqttTemplate;
    
    @Autowired
    private SensorDataService sensorDataService;
    
    @EventListener
    public void handleMqttMessage(MqttMessage message) {
        String topic = message.getTopic();
        String payload = new String(message.getPayload());
        
        if (topic.contains("sensor_data")) {
            SensorData data = parseSensorData(payload);
            sensorDataService.save(data);
        }
    }
    
    public void sendControlCommand(String deviceId, String command) {
        String topic = "device/" + deviceId + "/control";
        mqttTemplate.send(topic, command);
    }
}
```

### **📊 Report Service**
```java
@Service
public class ReportService {
    
    @Autowired
    private SensorDataRepository sensorDataRepository;
    
    public byte[] generatePdfReport(String deviceId, LocalDateTime startDate, LocalDateTime endDate) {
        List<SensorData> data = sensorDataRepository.findByDeviceIdAndTimestampBetween(
            deviceId, startDate, endDate);
        
        return ReportUtil.createPdfReport(data);
    }
    
    public byte[] generateExcelReport(String deviceId, LocalDateTime startDate, LocalDateTime endDate) {
        List<SensorData> data = sensorDataRepository.findByDeviceIdAndTimestampBetween(
            deviceId, startDate, endDate);
        
        return ReportUtil.createExcelReport(data);
    }
}
```

---

## 6. API ENDPOINTS

### **🔐 Authentication APIs**
```java
@RestController
@RequestMapping("/auth")
public class AuthController {
    
    @PostMapping("/login")
    public ResponseEntity<LoginResponse> login(@RequestBody LoginRequest request) {
        String token = authService.login(request);
        return ResponseEntity.ok(new LoginResponse(token));
    }
    
    @PostMapping("/register")
    public ResponseEntity<User> register(@RequestBody RegisterRequest request) {
        User user = authService.register(request);
        return ResponseEntity.ok(user);
    }
}
```

### **📱 Device Management APIs**
```java
@RestController
@RequestMapping("/devices")
public class DeviceController {
    
    @GetMapping
    public ResponseEntity<List<Device>> getDevices(@RequestHeader("Authorization") String token) {
        List<Device> devices = deviceService.getUserDevices(getUserIdFromToken(token));
        return ResponseEntity.ok(devices);
    }
    
    @PostMapping("/{deviceId}/connect")
    public ResponseEntity<String> connectDevice(@PathVariable String deviceId) {
        deviceService.connectDevice(deviceId);
        return ResponseEntity.ok("Device connected successfully");
    }
    
    @PostMapping("/{deviceId}/control")
    public ResponseEntity<String> controlDevice(@PathVariable String deviceId, 
                                              @RequestBody ControlRequest request) {
        mqttService.sendControlCommand(deviceId, request.getCommand());
        return ResponseEntity.ok("Command sent successfully");
    }
}
```

---

## 7. DEPLOYMENT VÀ INFRASTRUCTURE

### **🐳 Docker Configuration**
```dockerfile
FROM openjdk:17-jdk-slim

WORKDIR /app

COPY target/electric-nose-backend-1.0.0.jar app.jar

EXPOSE 8080

ENTRYPOINT ["java", "-jar", "app.jar"]
```

### **🐳 Docker Compose**
```yaml
version: '3.8'
services:
  app:
    build: .
    ports:
      - "8080:8080"
    environment:
      - DB_HOST=postgres
      - REDIS_HOST=redis
      - MQTT_BROKER_URL=tcp://mosquitto:1883
    depends_on:
      - postgres
      - redis
      - mosquitto
  
  postgres:
    image: postgres:15
    environment:
      POSTGRES_DB: electric_nose
      POSTGRES_USER: postgres
      POSTGRES_PASSWORD: password
    volumes:
      - postgres_data:/var/lib/postgresql/data
  
  redis:
    image: redis:7-alpine
    ports:
      - "6379:6379"
  
  mosquitto:
    image: eclipse-mosquitto:2.0
    ports:
      - "1883:1883"
      - "9001:9001"

volumes:
  postgres_data:
```

---

## 8. TESTING STRATEGY

### **🧪 Unit Tests**
```java
@SpringBootTest
class AuthServiceTest {
    
    @Autowired
    private AuthService authService;
    
    @Test
    void testLoginSuccess() {
        LoginRequest request = new LoginRequest("test@example.com", "password");
        String token = authService.login(request);
        assertNotNull(token);
    }
}
```

### **🔗 Integration Tests**
```java
@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
@AutoConfigureTestDatabase
class DeviceControllerIntegrationTest {
    
    @Autowired
    private TestRestTemplate restTemplate;
    
    @Test
    void testGetDevices() {
        ResponseEntity<List> response = restTemplate.getForEntity("/api/devices", List.class);
        assertEquals(HttpStatus.OK, response.getStatusCode());
    }
}
```

---

## 9. MONITORING VÀ LOGGING

### **📊 Application Properties**
```yaml
management:
  endpoints:
    web:
      exposure:
        include: health,info,metrics,prometheus
  endpoint:
    health:
      show-details: always

logging:
  pattern:
    console: "%d{yyyy-MM-dd HH:mm:ss} - %msg%n"
    file: "%d{yyyy-MM-dd HH:mm:ss} [%thread] %-5level %logger{36} - %msg%n"
  file:
    name: logs/electric-nose.log
```

### **💚 Health Check**
```java
@Component
public class CustomHealthIndicator implements HealthIndicator {
    
    @Override
    public Health health() {
        // Check database connection
        // Check MQTT broker connection
        // Check Redis connection
        return Health.up()
            .withDetail("database", "UP")
            .withDetail("mqtt", "UP")
            .withDetail("redis", "UP")
            .build();
    }
}
```

---

## 10. PERFORMANCE OPTIMIZATION

### **🗄️ Database Optimization**
```java
@Entity
@Table(name = "sensor_data", indexes = {
    @Index(name = "idx_device_timestamp", columnList = "device_id, timestamp"),
    @Index(name = "idx_timestamp", columnList = "timestamp")
})
public class SensorData {
    // Entity fields
}
```

### **⚡ Caching Strategy**
```java
@Service
public class DeviceService {
    
    @Cacheable(value = "devices", key = "#userId")
    public List<Device> getUserDevices(String userId) {
        return deviceRepository.findByUserId(userId);
    }
    
    @CacheEvict(value = "devices", key = "#device.userId")
    public Device updateDevice(Device device) {
        return deviceRepository.save(device);
    }
}
```

---

## 11. SECURITY CONFIGURATION

### **🔒 Spring Security Config**
```java
@Configuration
@EnableWebSecurity
public class SecurityConfig {
    
    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        http
            .csrf().disable()
            .authorizeHttpRequests(authz -> authz
                .requestMatchers("/auth/**").permitAll()
                .anyRequest().authenticated()
            )
            .oauth2ResourceServer(oauth2 -> oauth2.jwt(Customizer.withDefaults()));
        
        return http.build();
    }
}
```

---

## 12. COST ESTIMATION

### **⏰ Development Time**
- **Setup & Configuration**: 1 tuần
- **Core Services**: 4-6 tuần
- **API Development**: 3-4 tuần
- **Testing & Debugging**: 2-3 tuần
- **Total**: 10-14 tuần

### **💰 Infrastructure Costs (Monthly)**
- **Free Cloud Services**: $0/tháng
- **Railway**: $5 credit/tháng (Free tier)
- **Supabase**: 500MB database (Free tier)
- **MongoDB Atlas**: 512MB storage (Free tier)
- **HiveMQ Cloud**: 100 connections (Free tier)

---

**🎯 Kết luận**: Java Spring Boot là lựa chọn tuyệt vời cho backend của hệ thống Electric-Nose với khả năng mở rộng cao, bảo mật mạnh mẽ và hiệu suất ổn định. Hệ sinh thái Spring Framework cung cấp đầy đủ các công cụ cần thiết cho dự án IoT này.
