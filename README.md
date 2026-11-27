# ELECTRONIC_NOSE

This is main development repo for ESP32 device in 20221 semester - a simple buildable esp-idf example. For more information please contact [Nhu Hai Long Nguyen](https://www.linkedin.com/in/hải-long-nguyễn-như-95b718207) or [Gmail]:long27032002@gmaill.com

## :rocket: Features

**Feature Rich**<br>
It has all the features which you need in a IOT system: Read Sensor data, save data to micro SD card, publish data to MQTT server... To integrate AirSENSE into your project, all you need is an ESP32 Board, a computer with Visual Studio Code to run PlatformIO, usb capble for flashing.

**Free and Portable**
  - Support platformIO on Visual Studio Code.
  - Can be compiled to ESP32 for running freeRTOS OS.
  - Distributed under the MIT licence, so you can easily use it in commercial projects too.
  - Using ESP-IDF (In this project we use esp-idf version 4.4.3).

**Docs, Tools, and Services**
  - We are writing about documents. 

## :heart: Sponsor

**AirSENSE - Facebook**<br>
[AirSENSE](https://www.facebook.com/airsenseairqualitymornitoringsystem)

**AirSENSE - Another Version**<br>
[Another repositories](https://github.com/orgs/Air-SENSE/repositories)

## How to use project
We encourage the users to use the example as a template for the new projects.
A recommended way is to follow the instructions on a [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project).

**Project Structure**<br>

Note:
 - Kconfig.projbuild is file to config number pin GPIO, parameters, frequency,... these values can be changed. Run the command `idf.py menuconfig` in the ESP-IDF terminal to change it to suit your hardware.


## :star2: Contributing
AirSENSE is an open project and contribution is very welcome. There are many ways to contribute from simply speaking about your project, through writing examples, improving the documentation, fixing bugs or even hosting your own project under the AirSENSE organization.


Alot of people already left their fingerprint in AirSENSE. Be one them! See your here! :slightly_smiling_face:

<a href="https://github.com/Air-SENSE/AirSENSE_ESP32-IDF_RTOS/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Air-SENSE/AirSENSE_ESP32-IDF_RTOS&max=48" />
</a>

... and many other.


## Cấu hình Wi-Fi từ trình duyệt

Build E-Nose bằng **ESP-IDF 5.1.2** với `sdkconfig` của dự án.

```powershell
idf.py -B build/wifi-web build
idf.py -B build/wifi-web -p COMx flash monitor
```

E-Nose tự kết nối mạng đã lưu khi bật nguồn. Nếu chưa có cấu hình hoặc sau
20 giây vẫn chưa nhận được IP, thiết bị mở mạng Wi-Fi **E-Nose-Setup**.
Mật khẩu riêng của bo được tạo và lưu vào NVS. Xem mật khẩu trên serial
khi thiết bị mở mạng cấu hình lần đầu để dùng lại cho các lần sau.

1. Kết nối máy tính hoặc điện thoại vào **E-Nose-Setup**. Mở
   [http://192.168.4.1](http://192.168.4.1).
2. Quét mạng 2,4 GHz hoặc nhập SSID thủ công, điền mật khẩu rồi chọn
   **Kết nối**. Với mạng mở, để trống mật khẩu.
3. Khi trang hiện IP do router cấp, chuyển máy tính về mạng vừa cấu hình
   để truy cập địa chỉ đó. Chọn **Tắt mạng cấu hình** nếu muốn đóng SoftAP.

E-Nose chỉ lưu mạng mới sau khi nhận IP. Nếu kết nối thất bại, trang cấu
hình vẫn có thể truy cập qua SoftAP để thử lại. Để đổi mạng khi đang truy
cập E-Nose qua LAN, vào `/wifi`, chọn **Mở mạng cấu hình**, rồi kết nối
vào **E-Nose-Setup**. Khi đổi kênh để vào router, kết nối trình duyệt với
SoftAP có thể gián đoạn ngắn. Trang tự kiểm tra lại kết quả.

## Thử điều khiển Peltier qua ESP-WIFI-MESH

Dùng môi trường **ESP-IDF 5.1.2** cho cả hai bo. ESP32 Electric Nose
là root cố định, build từ thư mục gốc `Electric-Nose/`. ESP32 node build từ dự án ESP-IDF riêng tại `../Electric-Nose-Peltier/Peltier_control/`. Nạp `Electric_Nose_DATN.bin` cho bo root và `peltier_controller.bin` cho bo node.

Chạy các lệnh sau từ **từng thư mục dự án**. Chọn cổng COM của đúng bo.

```powershell
idf.py -D SDKCONFIG=sdkconfig.mesh -D SDKCONFIG_DEFAULTS=sdkconfig menuconfig
idf.py -D SDKCONFIG=sdkconfig.mesh -D SDKCONFIG_DEFAULTS=sdkconfig build
idf.py -D SDKCONFIG=sdkconfig.mesh -D SDKCONFIG_DEFAULTS=sdkconfig -p COMx flash monitor
```

Trong **Component config → Enable Electric Nose / Peltier ESP-WIFI-MESH**,
nhập SSID, mật khẩu và kênh của router 2,4 GHz. Nhập cùng mật khẩu Mesh AP
trên hai bo. Tại mỗi bo, nhập **STA MAC của bo còn lại**. Lần boot đầu có
thể để trống cấu hình để lấy dòng `Root STA MAC` hoặc `Node STA MAC` trên
serial. Lưu thông tin cấu hình trong `sdkconfig.mesh` cục bộ.

Khi node đã phản hồi và root hiện `ack=1`, gõ `1` trên serial của root
để cho phép FSM Peltier hoạt động, hoặc `0` để tắt. Log hiển thị thứ tự
lệnh, ACK, quyền chạy, trạng thái đầu ra, nhiệt độ, độ ẩm, nhiệt mặt nóng
và bit lỗi. `out=1` là trạng thái đầu ra do firmware đặt. Kết nối lại
sau khi node đã ngắt do timeout cần một lệnh bật mới. Nếu node vừa khởi
động lại khi root đang gửi bật, gõ `0`, chờ ACK rồi mới gõ `1`.

Thử với đầu ra công suất chưa nối tải. Xác nhận node khởi động ở OFF,
nhận cả hai lệnh, gửi số đo, và về OFF khi ngắt root. Node kiểm tra hạn
lệnh 5 giây trong vòng lấy mẫu (mặc định 1 giây). Chế độ mesh bỏ xung tự
kiểm tra đầu ra khi khởi động. Mesh dùng chung radio với chức năng Wi-Fi
cũ, nên khi bật mesh, phần SmartConfig và công tắc Wi-Fi cũ được vô hiệu.
