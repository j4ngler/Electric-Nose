# Spec Dự án 2 --- Điều khiển bơm tuyến tính & phản hồi dòng (Flow Control)

## 1) Mục tiêu & phạm vi

- **Mục tiêu**: điều chỉnh công suất bơm DC 12V linh hoạt và ổn định
  theo tải bằng phản hồi **dòng điện motor**.
- **Chế độ**:
  - **Manual Mode**: đặt PWM theo biến trở/slider HMI.
  - **Auto Mode**: tự điều chỉnh PWM theo dòng đo được (giữ dòng mục
    tiêu hoặc giới hạn dòng).
- **Ghi chú**: dòng điện là đại lượng phản hồi dễ đo; lưu lượng khí thực
  có thể cần cảm biến lưu lượng nếu muốn "giữ flow" tuyệt đối.

## 2) Kiến trúc hệ thống (vòng kín)

- **Actuator**: MOSFET kênh N băm xung PWM (low-side) điều khiển motor
  bơm.
- **Feedback**: cảm biến dòng **ACS712-05A/05B** (analog) *hoặc* shunt
  (R_s) + op-amp khuếch đại.
- **Controller**: thuật toán điều khiển đơn giản (PI hoặc tăng/giảm theo
  sai số) + cơ chế bảo vệ quá dòng.

## 3) Yêu cầu phần cứng (BOM tối thiểu)

- **Vi điều khiển**: ESP32 DevKit V1.
- **Bơm khí**: motor DC 12V.
- **Công suất**:
  - MOSFET kênh N phù hợp dòng bơm (gợi ý: IRLZ44N/IRFZ44N hoặc tương
    đương).
  - Diode hồi tiếp (flyback) nếu motor/bơm không có sẵn mạch dập xung.
  - (Khuyến nghị) driver gate nếu PWM tần số cao hoặc MOSFET không
    "logic-level".
- **Đo dòng**:
  - ACS712 5A hoặc shunt + op-amp.
  - RC low-pass cho tín hiệu ADC (hoặc lọc số trong firmware).
- **Nguồn**: 12V chung, có hạ áp 5V/3.3V cho ESP32.

## 4) Sơ đồ kết nối (pin map đề xuất)

- **PWM out**: GPIO19 → IN/Driver gate MOSFET.
- **ADC dòng**: GPIO34 (ADC1) ← OUT ACS712 / OUT op-amp shunt.
- **Nguồn tải** (low-side):
  - +12V → Motor(+) → Motor(--) → MOSFET(Drain) → MOSFET(Source) → GND

## 5) Đặc tả đo dòng

### 5.1) Tần số PWM & lấy mẫu

- **PWM**: 15--25 kHz (khuyến nghị để giảm tiếng ồn), hoặc thấp hơn nếu
  phần cứng hạn chế.
- **ADC**:
  - Lấy mẫu nhanh hơn PWM là lý tưởng, nhưng thực tế có thể:
    - Lấy mẫu 500 Hz--2 kHz + lọc số mạnh (EMA/moving average).
    - Hoặc đồng bộ lấy mẫu theo pha PWM (nâng cao).

### 5.2) Hiệu chuẩn

- **Offset 0A**: đo khi bơm tắt để bù lệch.
- **Scale**: theo datasheet ACS712 hoặc hệ số khuếch đại op-amp.
- **Giới hạn hợp lệ**: nếu ADC ra ngoài dải hợp lý → báo lỗi sensor.

## 6) Luật điều khiển (Auto Mode --- đề xuất)

### 6.1) Mục tiêu điều khiển

- **Giữ dòng mục tiêu** (I\_{set}) (ổn định "mức tải"):
  - Sai số (e = I\_{set} - I\_{meas})
  - Điều chỉnh PWM theo (e) (PI hoặc tăng/giảm theo bước).
- **Giới hạn bảo vệ** (I\_{max}):
  - Nếu (I\_{meas} \> I\_{max}) → giảm PWM nhanh / ngắt tạm.

### 6.2) Ứng xử theo tình huống

- **Tắc/ma sát tăng**: dòng có xu hướng tăng → giảm PWM hoặc giữ
  (I\_{set}) để tránh quá tải.
- **Nguồn sụt áp**: PWM tăng nhưng dòng không đạt → cảnh báo "nguồn yếu"
  hoặc "tải bất thường".

## 7) Firmware --- yêu cầu chức năng

- **Interface cấu hình**:
  - `mode = MANUAL|AUTO`
  - Manual: `pwm_set_percent (0..100)`
  - Auto: `I_set (A)`, `I_max (A)`, `pwm_min/max`
- **Telemetry**:
  - PWM(%), I(A), mode, trạng thái lỗi (overcurrent, sensor fault,
    undervoltage (tuỳ chọn)).
- **Fail-safe**:
  - Sensor lỗi / không đọc được → giảm PWM về mức an toàn hoặc tắt bơm.
  - Mất lệnh (nếu có CAN) → timeout → về trạng thái an toàn.

## 8) Kiểm thử tối thiểu (acceptance)

- Manual: thay đổi slider → PWM thay đổi tuyến tính, bơm đáp ứng.
- Auto: đặt (I\_{set}) → PWM tự điều chỉnh để dòng hội tụ (không dao
  động mạnh).
- Overcurrent: cưỡng bức tải nặng → hệ thống giảm PWM/ngắt và báo lỗi
  đúng.
