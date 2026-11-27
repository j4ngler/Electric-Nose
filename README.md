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


## Tự kết nối Wi-Fi trên firmware E-Nose của nhánh Peltier

Phần này áp dụng cho firmware **Electric_Nose_DATN** ở thư mục gốc của
nhánh `codex/peltier_controller`. Firmware bo Peltier được build riêng
từ `Peltier_control/`.

1. Trong terminal ESP-IDF 5.1.2, chạy `idf.py menuconfig` ở thư mục gốc.
   Bật **Device in use → Using WIFI station**. Bản dùng màn hình cần bật
   LCD TFT để sử dụng nút đổi Wi-Fi.
2. Build bằng `idf.py build` và nạp bo E-Nose bằng
   `idf.py -p COMx flash monitor`.
3. Khi khởi động, bo tự thử mạng đã lưu. Lần đầu chưa có mạng hoặc khi
   kết nối lại thất bại, dùng ESPTouch để gửi SSID và mật khẩu. Để đổi
   mạng, mở **Settings → Change Wi-Fi** khi Wi-Fi đang bật.

Firmware lưu mạng mới vào NVS sau khi nhận IP và thử lại mạng đã lưu nếu
mạng mới thất bại. Khi đã kết nối điểm truy cập mà DHCP chưa cấp IP sau
30 giây, firmware ngắt để thử lại.

Trong **WIFI/SMART CONFIG Configuration**, đặt `ENOSE_MQTT_BROKER_URI`
thành địa chỉ broker mà bo truy cập được và đặt `ENOSE_MQTT_DEVICE_CODE`
trùng với mã thiết bị trong AirSENSE. Khi đã có Wi-Fi và MQTT, bo gửi
SSID, RSSI và IP mỗi 10 giây lên
`electric-nose/device/<device_code>/status`. Để trống URI nếu chỉ cần
tự kết nối Wi-Fi.

## :star2: Contributing
AirSENSE is an open project and contribution is very welcome. There are many ways to contribute from simply speaking about your project, through writing examples, improving the documentation, fixing bugs or even hosting your own project under the AirSENSE organization.


Alot of people already left their fingerprint in AirSENSE. Be one them! See your here! :slightly_smiling_face:

<a href="https://github.com/Air-SENSE/AirSENSE_ESP32-IDF_RTOS/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Air-SENSE/AirSENSE_ESP32-IDF_RTOS&max=48" />
</a>

... and many other.
