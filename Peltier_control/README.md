# Peltier Controller

Firmware ESP32 điều khiển Peltier. Build từ thư mục này bằng ESP-IDF 5.1.2. Khi bật ESP-WIFI-MESH, bo này làm node và giao tiếp với bo root chạy firmware Electric-Nose.

```powershell
idf.py -D SDKCONFIG=sdkconfig.mesh -D SDKCONFIG_DEFAULTS=sdkconfig menuconfig
idf.py -D SDKCONFIG=sdkconfig.mesh -D SDKCONFIG_DEFAULTS=sdkconfig build
idf.py -D SDKCONFIG=sdkconfig.mesh -D SDKCONFIG_DEFAULTS=sdkconfig -p COMx flash monitor
```

Trong menuconfig, bật Mesh và nhập thông tin router 2,4 GHz, mật khẩu Mesh AP và STA MAC của bo root. Dùng cùng router, kênh và mật khẩu Mesh AP trên hai bo. Chọn cổng COM của bo Peltier. File ứng dụng tạo ra là `build/peltier_controller.bin`.

Xem [hướng dẫn kiểm tra hai bo](../../Electric-Nose/README.md#thử-điều-khiển-peltier-qua-esp-wifi-mesh) trong dự án Electric-Nose.

## Ngưỡng điều khiển

- Dải nhiệt độ buồng mặc định là **20–40 °C**. Dải độ ẩm là **50–65% RH**.
- Peltier yêu cầu bật khi nhiệt độ đạt 40 °C hoặc độ ẩm đạt 65% RH. Khi
  nhiệt độ xuống 20 °C hoặc độ ẩm xuống 50% RH, yêu cầu tắt được ưu tiên.
  Trong khoảng giữa các ngưỡng, bộ điều khiển giữ trạng thái hiện tại.
- Đổi trạng thái bình thường phải đủ thời gian giữ tối thiểu (mặc định
  15 giây). Lỗi cảm biến, bảo vệ quá nhiệt hoặc mất quyền chạy ngắt đầu
  ra ngay ở lần cập nhật phát hiện lỗi.

Firmware lấy các ngưỡng từ cấu hình build. Để chỉnh, vào **Peltier
Controller Configuration → Control & Protection Parameters** trong
`menuconfig`, sau đó build và nạp lại bo Peltier. Nhiệt độ và độ ẩm
trong menu nhập theo đơn vị 0,1. Web/app và gói Mesh hiện chưa hỗ trợ
thay đổi ngưỡng lúc chạy.
