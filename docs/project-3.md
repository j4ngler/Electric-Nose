# Spec Dự án 3 --- Mạng điều khiển phân tán & HMI (CAN Bus + LVGL)

## 1) Mục tiêu & phạm vi

- **Mục tiêu**: tách các node công suất/đo lường khỏi hiển thị, tăng
  chống nhiễu, dễ mở rộng và quản lý tập trung.
- **Kiến trúc**: Master--Slave trên **CAN Bus**; Master làm **Gateway**
  lên Wi‑Fi/Cloud.
- **HMI**: node hiển thị (LCD/TFT) dùng **LVGL**, cho phép đặt ngưỡng và
  chế độ.

## 2) Topology hệ thống

- **Node Master (ESP32 + MCP2515)**:
  - Thu telemetry từ các node.
  - Phân phối setpoint/command.
  - Đẩy dữ liệu lên Cloud (MQTT/Blynk) (tuỳ chọn).
- **Node 1 (Peltier)**:
  - Đo (T/H) và điều khiển Peltier theo lệnh/giới hạn.
  - Fail-safe cục bộ khi mất CAN.
- **Node 2 (Bơm)**:
  - Điều khiển PWM + phản hồi dòng.
  - Fail-safe cục bộ khi mất CAN.
- **Node HMI (LVGL)**:
  - Hiển thị thông số.
  - Gửi yêu cầu thay đổi setpoint/mode lên Master.

## 3) Yêu cầu phần cứng (BOM tối thiểu)

- **ESP32** cho mỗi node.
- **CAN controller**: MCP2515 (SPI) + **CAN transceiver** (thường có sẵn
  trên module).
- **Bus vật lý**:
  - Dây CAN_H/CAN_L (ưu tiên dây xoắn đôi).
  - **Điện trở kết thúc 120Ω** ở **2 đầu** tuyến CAN (khuyến nghị).
- **Nguồn**: 12V chung, hạ áp 5V/3.3V cho từng node.

## 4) Kết nối MCP2515 (SPI) --- nguyên tắc

Do khác nhau giữa board/module, pin cụ thể có thể thay đổi; yêu cầu tối
thiểu: - SPI: **SCK, MOSI, MISO** dùng chung bus SPI. - **CS**: mỗi
MCP2515 một chân GPIO riêng. - **INT**: nối về 1 GPIO để bắt ngắt khung
CAN.

## 5) Đặc tả giao thức CAN (khuyến nghị)

### 5.1) Quy ước message

- **ID 11-bit** (standard frame).
- **Payload 0..8 bytes** (cân nhắc dùng fixed-point để tiết kiệm băng
  thông).
- **Chu kỳ**:
  - Telemetry: 200--1000 ms.
  - Command: theo sự kiện + refresh 1--2 s.

### 5.2) Bảng ID đề xuất

- `0x100`: Telemetry Node 1 → Master (T, H, trạng thái Peltier, lỗi)
- `0x110`: Telemetry Node 2 → Master (PWM, I, mode, lỗi)
- `0x200`: Command Master → Node 1 (ngưỡng T/H, enable, các tham số an
  toàn)
- `0x210`: Command Master → Node 2 (mode, pwm_set, I_set, I_max, enable)
- `0x300`: HMI → Master (thao tác user, setpoint/mode)
- `0x310`: Master → HMI (dữ liệu hiển thị tổng hợp)

## 6) Chức năng phần mềm theo node

### 6.1) Node Master

- Nhận telemetry, đánh dấu "node alive" theo timeout.
- Gửi command xuống các node theo cấu hình hiện tại.
- (Tuỳ chọn) Uplink Cloud (MQTT/Blynk), lưu lịch sử, OTA.

### 6.2) Node HMI (LVGL)

- Màn hình tối thiểu:
  - (T/H) hiện tại, trạng thái Peltier (ON/OFF, lỗi)
  - PWM/I bơm, mode (Manual/Auto, lỗi)
  - Trạng thái kết nối CAN, thời gian cập nhật cuối
- Chức năng cài đặt:
  - Ngưỡng (T) (20--25°C), (H) (60--65%)
  - Enable/Disable Peltier/Bơm
  - Manual PWM / Auto (I\_{set}), (I\_{max})

### 6.3) Node 1 (Peltier) & Node 2 (Bơm)

- Nhận command, cập nhật setpoint tham số.
- Gửi telemetry định kỳ.
- Tự kiểm tra lỗi phần cứng (sensor/ADC/module).

## 7) Fail-safe & độ tin cậy

- **Timeout CAN** (mỗi node công suất):
  - Nếu không nhận command/heartbeat trong (t\_{timeout}) (gợi ý 3--5 s)
    → về **trạng thái an toàn**:
    - Node 1: **tắt Peltier**
    - Node 2: giảm PWM về mức an toàn hoặc tắt bơm
- **Checksum/sequence** (tuỳ chọn):
  - Thêm `seq` để phát hiện mất gói/lặp gói.
- **Khởi động lại**:
  - Node công suất khởi động lên ở trạng thái an toàn và chỉ enable khi
    nhận lệnh hợp lệ.

## 8) Kiểm thử tối thiểu (acceptance)

- Rút dây CAN: Node 1 tắt Peltier sau timeout; Node 2 giảm/tắt bơm.
- Thay đổi ngưỡng từ HMI: Master phát command; node cập nhật và phản hồi
  telemetry đúng.
- Nhiễu/đường dài: bus vẫn truyền ổn định với 120Ω kết thúc và dây xoắn
  đôi.
