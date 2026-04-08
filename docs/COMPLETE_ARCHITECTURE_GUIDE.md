# HƯỚNG DẪN KIẾN TRÚC HOÀN CHỈNH - APP ĐIỀU KHIỂN ELECTRIC-NOSE

## 📋 MỤC LỤC
1. [Tổng quan dự án](#1-tổng-quan-dự-án)
2. [Kiến trúc hệ thống](#2-kiến-trúc-hệ-thống)
3. [Workflow chi tiết](#3-workflow-chi-tiết)
4. [Protocol và giao tiếp](#4-protocol-và-giao-tiếp)
5. [Bảo mật và xác thực](#5-bảo-mật-và-xác-thực)
6. [Performance và scalability](#6-performance-và-scalability)
7. [Monitoring và logging](#7-monitoring-và-logging)
8. [Deployment và CI/CD](#8-deployment-và-cicd)
9. [Cost estimation](#9-cost-estimation)
10. [Timeline phát triển](#10-timeline-phát-triển)

---

## 1. TỔNG QUAN DỰ ÁN

### **🎯 Mục tiêu**
Xây dựng ứng dụng mobile để khách hàng có thể điều khiển và giám sát hệ thống Electric-Nose (Mũi điện tử) đo chất lượng không khí.

### **👥 Đối tượng người dùng**
- Cá nhân quan tâm đến chất lượng không khí trong nhà
- Doanh nghiệp cần giám sát môi trường làm việc
- Cơ sở y tế, trường học cần kiểm soát chất lượng không khí

---

## 2. KIẾN TRÚC HỆ THỐNG

### **🏗️ Kiến trúc tổng quan**

```
┌─────────────────────────────────────────────────────────────────┐
│                    WEB APPLICATION (Khách hàng)                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────┐ │
│  │   Login/    │  │  Dashboard  │  │   Data      │  │Settings │ │
│  │  Register   │  │             │  │  Analysis   │  │         │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                │
                                │ HTTPS/REST API
                                │
┌─────────────────────────────────────────────────────────────────┐
│                BACKEND SERVER (Free Cloud Services)            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────┐ │
│  │   Auth      │  │   Device    │  │   Data      │  │Notification│ │
│  │  Service    │  │  Manager    │  │  Processor  │  │ Service  │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────┘ │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────┐ │
│  │   User      │  │   Report    │  │   File      │  │   MQTT  │ │
│  │  Manager    │  │  Generator  │  │  Storage    │  │ Broker  │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                │
                                │ MQTT/HTTP
                                │
┌─────────────────────────────────────────────────────────────────┐
│                    ELECTRIC-NOSE DEVICE                         │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────┐ │
│  │   ESP32     │  │   Sensors   │  │   Storage   │  │  WiFi   │ │
│  │  (Main MCU) │  │  (SHT3x,    │  │  (SD Card)  │  │ Module  │ │
│  │             │  │   ADS1115)  │  │             │  │         │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────┘ │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────┐ │
│  │   LCD       │  │   RTC       │  │   Relay     │  │  Web    │ │
│  │  Display    │  │  (DS3231)   │  │  Control    │  │ Server  │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### **🌐 Web Application (Frontend)**
```
Platform: React.js / Vue.js / Angular
├── Authentication Module
├── Device Management Module  
├── Data Visualization Module
├── Control Module
├── Settings Module
└── Notification Module
```

**Tính năng chính:**
- Đăng nhập/Đăng ký tài khoản
- Quét QR code để kết nối thiết bị
- Dashboard hiển thị dữ liệu real-time
- Điều khiển thiết bị từ xa
- Xem lịch sử và tạo báo cáo
- Nhận thông báo và cảnh báo
- Responsive design cho mobile

### **☁️ Backend Server (Free Cloud Services)**
```
Platform: Java Spring Boot (Railway/Render/Heroku)
├── API Gateway (Spring Gateway)
├── Authentication Service (Spring Security)
├── Device Management Service (Spring Data JPA)
├── Data Processing Service (Spring Batch)
├── Report Generation Service (Apache POI, iText)
├── Notification Service (Spring Integration)
└── File Storage Service (Spring Cloud AWS)
```

**Database:**
- PostgreSQL: Lưu trữ dữ liệu người dùng, thiết bị, đo đạc
- Redis: Cache session, real-time data
- MongoDB: Lưu trữ log và analytics

### **🔧 Electric-Nose Device (Hardware)**
```
ESP32 (Main Controller)
├── Sensors
│   ├── SHT3x (Temperature & Humidity)
│   ├── ADS1115 (Gas Sensors - 8 channels)
│   └── DS3231 (Real Time Clock)
├── Storage
│   └── SD Card (Data logging)
├── Display
│   └── LCD TFT ILI9341 (User interface)
├── Control
│   └── Relay (Pump/Fan control)
└── Connectivity
    ├── WiFi (Internet connection)
    └── MQTT Client (Data transmission)
```

---

## 3. WORKFLOW CHI TIẾT

### **🔐 Workflow đăng nhập và kết nối thiết bị**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Mở App       │───▶│  Màn hình      │───▶│  Đăng nhập/    │
│                │    │  Chào mừng     │    │  Đăng ký       │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                       │
                                ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Quét QR Code  │◀───│  Chọn "Kết nối │◀───│  Xác thực      │
│  Thiết bị      │    │  thiết bị"      │    │  thành công    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Nhập thông tin │───▶│  Kết nối WiFi  │───▶│  Xác nhận kết  │
│  thiết bị       │    │  (SmartConfig)  │    │  nối thành công│
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │
                                ▼
                       ┌─────────────────┐
                       │  Vào Dashboard  │
                       │  Chính          │
                       └─────────────────┘
```

### **🌬️ Workflow đo chất lượng không khí**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Dashboard      │───▶│  Chọn "Bắt đầu │───▶│  Thiết lập     │
│  Chính          │    │  đo"            │    │  tham số đo    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                       │
                                ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Xác nhận và    │◀───│  Chọn loại đo   │───▶│  Cài đặt lịch   │
│  bắt đầu đo     │    │  (Thủ công/     │    │  đo tự động     │
│                 │    │  Tự động)       │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Thiết bị bắt   │───▶│  Hiển thị dữ    │───▶│  Lưu kết quả   │
│  đầu đo         │    │  liệu real-time │    │  và tạo báo cáo│
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                       │
                                ▼                       ▼
                       ┌─────────────────┐    ┌─────────────────┐
                       │  Gửi thông báo  │    │  Hiển thị kết   │
                       │  hoàn thành     │    │  quả cuối cùng  │
                       └─────────────────┘    └─────────────────┘
```

### **📊 Workflow xem dữ liệu và báo cáo**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Dashboard      │───▶│  Chọn "Dữ liệu"│───▶│  Lọc theo      │
│  Chính          │    │                 │    │  thời gian     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                       │
                                ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Xuất báo cáo   │◀───│  Xem biểu đồ    │───▶│  Chọn định dạng │
│  PDF/Excel      │    │  và thống kê    │    │  xuất dữ liệu   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Chia sẻ qua    │    │  Tải xuống      │    │  Gửi email      │
│  email/SMS      │    │  file           │    │  báo cáo        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### **⚙️ Workflow cài đặt và bảo trì**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Dashboard      │───▶│  Chọn "Cài đặt"│───▶│  Cấu hình      │
│  Chính          │    │                 │    │  thiết bị       │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                       │
                                ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Cập nhật       │◀───│  Lịch đo tự     │───▶│  Cảnh báo và    │
│  firmware       │    │  động           │    │  thông báo      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Kiểm tra trạng │    │  Bảo trì định   │    │  Hỗ trợ kỹ      │
│  thái thiết bị  │    │  kỳ             │    │  thuật          │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

---

## 4. PROTOCOL VÀ GIAO TIẾP

### **🌐 Web App ↔ Backend**
- **Protocol**: HTTPS/REST API + WebSocket
- **Authentication**: JWT Token
- **Data Format**: JSON
- **Endpoints**:
  - `POST /api/auth/login`
  - `GET /api/devices`
  - `POST /api/devices/connect`
  - `GET /api/data/sensor`
  - `POST /api/control/start-measurement`
  - `WS /ws/sensor-data` (Real-time updates)

### **☁️ Backend ↔ Device**
- **Protocol**: MQTT over WiFi
- **Broker**: HiveMQ Cloud Free / Eclipse Mosquitto
- **Topics**:
  - `device/{device_id}/sensor_data` (Device → Backend)
  - `device/{device_id}/control` (Backend → Device)
  - `device/{device_id}/status` (Device → Backend)

### **🔧 Device ↔ Sensors**
- **Protocol**: I2C, SPI
- **Sensors**: SHT3x, ADS1115, DS3231
- **Data Rate**: 500ms sampling interval

---

## 5. BẢO MẬT VÀ XÁC THỰC

### **🔐 Authentication**
- JWT Token cho API authentication
- Device Certificate cho MQTT connection
- OAuth2 cho social login (Google, Facebook)
- Biometric authentication (Touch ID, Face ID)

### **🛡️ Data Security**
- HTTPS cho tất cả API calls
- MQTT over TLS
- Data encryption at rest (AES-256)
- Regular security updates và patches

### **🔒 Privacy**
- GDPR compliance
- Data anonymization
- User consent management
- Right to data deletion

---

## 6. PERFORMANCE VÀ SCALABILITY

### **⚡ Performance Optimization**
- Redis cache cho session management
- Database indexing cho fast queries
- API rate limiting
- Data compression (gzip)
- CDN cho static assets

### **📈 Scalability**
- Horizontal scaling với load balancer
- Database clustering
- MQTT broker clustering
- Microservices architecture
- Auto-scaling based on load

---

## 7. MONITORING VÀ LOGGING

### **📊 Application Monitoring**
- Real-time performance metrics
- Error tracking và alerting
- User behavior analytics
- Device health monitoring
- Uptime monitoring

### **📝 Logging Strategy**
- Centralized logging với ELK stack
- Log aggregation và analysis
- Audit trail cho security
- Performance profiling
- Error tracking với Sentry

---

## 8. DEPLOYMENT VÀ CI/CD

### **🌐 Web Application**
- **Frontend**: Firebase Hosting (Free tier)
- **CDN**: Firebase CDN global
- **SSL**: Automatic HTTPS
- **Custom Domain**: Supported
- **CI/CD**: GitHub Actions

### **☁️ Backend**
- **Cloud**: Railway / Render / Heroku (Free tier)
- **Containerization**: Docker + Kubernetes
- **CI/CD**: GitHub Actions / Jenkins
- **Monitoring**: Prometheus + Grafana

### **🔧 Device Firmware**
- **OTA Updates**: Over-the-air firmware updates
- **Version Control**: Git-based firmware management
- **Testing**: Hardware-in-the-loop testing

---

## 9. COST ESTIMATION

### **💰 Development Costs**
- Web Application Development: 2-3 tháng
- Java Backend Development: 2-3 tháng
- Device Integration: 1-2 tháng
- Testing & QA: 1 tháng

### **💵 Infrastructure Costs (Monthly)**
- **Free Cloud Services**: $0/tháng
- **Railway**: $5 credit/tháng (Free tier)
- **Supabase**: 500MB database (Free tier)
- **MongoDB Atlas**: 512MB storage (Free tier)
- **HiveMQ Cloud**: 100 connections (Free tier)

---

## 10. TIMELINE PHÁT TRIỂN

### **🚀 Phase 1 (Tháng 1-2): Foundation**
- Setup development environment
- Design UI/UX
- Implement authentication
- Basic device connection

### **⚙️ Phase 2 (Tháng 3-4): Core Features**
- Real-time data display
- Device control
- Data storage và retrieval
- Basic reporting

### **📈 Phase 3 (Tháng 5-6): Advanced Features**
- Advanced analytics
- Notification system
- Report generation
- Performance optimization

### **🎯 Phase 4 (Tháng 7-8): Testing & Launch**
- Comprehensive testing
- Bug fixes
- Performance tuning
- App store submission

---

## 11. RISK MANAGEMENT

### **⚠️ Technical Risks**
- Device connectivity issues
- Data synchronization problems
- Performance bottlenecks
- Security vulnerabilities

### **🛠️ Mitigation Strategies**
- Robust error handling
- Offline mode support
- Regular security audits
- Performance monitoring
- Backup và recovery plans

---

## 12. SUCCESS METRICS

### **📊 User Engagement**
- Daily Active Users (DAU)
- Monthly Active Users (MAU)
- Session duration
- Feature adoption rate

### **⚙️ Technical Metrics**
- App performance (load time, crash rate)
- API response time
- Device connectivity success rate
- Data accuracy

### **💼 Business Metrics**
- User retention rate
- Customer satisfaction score
- Revenue per user
- Market penetration

---

**🎯 Kết luận**: Hệ thống app điều khiển Electric-Nose được thiết kế để cung cấp trải nghiệm người dùng tốt nhất cho khách hàng, với khả năng mở rộng và bảo mật cao. Kiến trúc microservices và cloud-native sẽ đảm bảo hệ thống có thể phát triển theo nhu cầu thị trường.
