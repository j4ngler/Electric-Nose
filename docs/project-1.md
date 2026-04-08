# Spec Dự án 1 --- Khối điều khiển Nhiệt ẩm công suất cao (Peltier 110W)

## 1) Mục tiêu & phạm vi

- **Mục tiêu vận hành**: duy trì **(T = 20--25\^C)** và **(H =
  60--65%)** trong buồng đo.
- **Phương pháp**: làm lạnh để **ngưng tụ hút ẩm** (Peltier) + quạt/tản
  nhiệt; điều khiển **On--Off có hysteresis**.
- **Phạm vi điều khiển**: đóng/ngắt nguồn DC 12V cấp cho Peltier bằng
  **MOSFET** (ưu tiên module có **opto cách ly**).

## 2) Kiến trúc hệ thống

- **Node**: 01 ESP32 DevKit V1.
- **Khối chức năng**:
  - **Sensor**: SHT35 (I2C) đọc nhiệt độ/độ ẩm.
  - **Power stage**: MOSFET kênh N (low-side) đóng/ngắt tải 12V cho
    Peltier.
  - **Tản nhiệt mặt nóng**: heatsink nhôm + quạt 12V high-speed.
  - **(Khuyến nghị)**: cảm biến nhiệt mặt nóng để bảo vệ quá nhiệt.

## 3) Yêu cầu phần cứng (BOM tối thiểu)

- **Vi điều khiển**: ESP32 DevKit V1 (3.3V logic).
- **Cảm biến**: SHT35 (I2C, 3.3V).
- **Chấp hành**:
  - Peltier **12V \~110W** (dòng danh định xấp xỉ 9--10A).
  - Module MOSFET **chịu dòng \>15A**, **Rds(on) thấp**, **có opto cách
    ly** (ưu tiên loại "logic-level" chạy tốt với 3.3V).
    - Gợi ý: module dùng **LR7843 / IRLZ44N / tương đương**.
- **Nguồn cấp**: nguồn tổ ong **12VDC 20A** (từ 220VAC).
- **Dây tải**: tuyến Peltier dùng dây **tiết diện (,mm\^2)**.
- **Bảo vệ (khuyến nghị)**: cầu chì nhánh Peltier, TVS 12V, chống ngược
  cực, đầu cos/terminal chắc chắn.

### 3.1) So sánh MOSFET vs relay (đóng/ngắt Peltier 110W)

  ---------------------------------------------------------------
  **Tiêu chí**  **MOSFET công suất       **Relay cơ 12V (phương
                (khuyến nghị)**          án dự phòng)**
  ------------- ------------------------ ------------------------
  **Dòng danh   Chịu tốt nếu chọn module Cần relay contact DC
  định \~10A**  \>15--20A, Rds(on) thấp  ≥15--20A, tiếp điểm mòn
                → ít nóng                dần theo thời gian

  **Tổn hao &   I²·Rds(on) nhỏ → hiệu    Tổn hao nhỏ nhưng spark
  nhiệt**       suất cao, tản nhiệt dễ   tiếp điểm, phụ thuộc
                                         chất lượng relay

  **Tiếng ồn    Im lặng (không click)    Có tiếng click mỗi lần
  khi                                    đóng/ngắt
  đóng/ngắt**                            

  **Tần suất    Rất lớn; phù hợp On--Off Giới hạn số chu kỳ;
  đóng/ngắt**   chậm và dễ nâng cấp PWM  không phù hợp PWM
                trong tương lai          

  **Điều khiển  Dùng module MOSFET có    Cần driver coil + diode
  từ ESP32**    opto: 1 GPIO, dòng kích  dập, dòng coil lớn hơn
                rất nhỏ                  

  **Mở rộng sau Thuận lợi cho duty/PWM,  Hầu như chỉ phù hợp
  này**         soft‑start, các thuật    đóng/ngắt thô
                toán nâng cao            
  ---------------------------------------------------------------

**Kết luận**: dự án **ưu tiên dùng module MOSFET công suất có opto cách
ly** để điều khiển Peltier 110W; relay cơ chỉ được xem là **phương án dự
phòng** khi không có sẵn module MOSFET phù hợp.

## 4) Sơ đồ kết nối (pin map đề xuất)

### 4.1) SHT35 (I2C)

- **VCC** → 3.3V
- **GND** → GND
- **SDA** → GPIO21
- **SCL** → GPIO22

### 4.2) MOSFET module (đóng/ngắt Peltier)

- **IN/Trig** (qua opto module) → GPIO18
- **Nguồn/tải 12V**:
  - +12V → Peltier(+) → Peltier(--) → MOSFET(Drain) → MOSFET(Source) →
    GND
  - GND nguồn 12V và GND hệ thống cần nối đúng theo thiết kế module (ưu
    tiên cách ly quang và bố trí mass hợp lý).

### 4.3) Quạt tản nhiệt

- Quạt 12V cấp trực tiếp từ 12V (khuyến nghị luôn chạy khi Peltier hoạt
  động), hoặc điều khiển riêng nếu có nhu cầu.

## 5) Thuật toán điều khiển (On--Off + hysteresis)

### 5.1) Ngưỡng điều khiển

- **Bật làm lạnh/hút ẩm** khi: (T \> 25\^C) **HOẶC** (H \> 65%)
- **Tắt** khi: (T \< 20\^C) **VÀ** (H \< 60%)

### 5.2) Chống đóng/ngắt liên tục (khuyến nghị)

- **Lọc số**: trung bình trượt 5--10 mẫu cho (T/H) (chu kỳ đọc 0.5--2
  Hz).
- **Min ON/OFF time**: tối thiểu 10--30 giây cho mỗi lần bật/tắt.
- **Bảo vệ quá nhiệt mặt nóng** (nếu có cảm biến): tắt Peltier khi
  (T\_{hot} \> T\_{limit}) và chỉ cho phép bật lại khi hạ xuống dưới
  ngưỡng an toàn.

## 6) Firmware --- yêu cầu chức năng

- **Chu kỳ đọc sensor**: 1--2 lần/giây.
- **Tính toán**: lọc (T/H), đánh giá điều kiện bật/tắt, áp dụng min
  on/off.
- **Điều khiển**: xuất GPIO18 theo trạng thái Peltier.
- **Giám sát**:
  - log UART: (T), (H), trạng thái Peltier, lỗi (nếu có).
  - (Tuỳ chọn) phát telemetry lên CAN (Dự án 3).
- **Cấu hình**:
  - lưu ngưỡng (T/H), min on/off, lựa chọn enable bằng NVS/Kconfig.

## 7) Fail-safe & an toàn

- **Sensor lỗi/không đọc được**: chuyển về trạng thái an toàn (tắt
  Peltier).
- **Mất truyền thông (nếu có CAN)**: timeout → tắt Peltier.
- **EMC/đi dây**:
  - đi dây tải ngắn, tiết diện đủ; tách đường công suất và tín hiệu.
  - lọc nguồn gần tải; bố trí mass dạng "star ground" để giảm sụt
    áp/nhiễu.
