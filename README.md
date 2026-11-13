# LineColor

Firmware phân loại vật trên băng chuyền, chạy trên Blue Pill (STM32F103C8). Cảm biến TCS34725 đo màu, hai cảm biến vật cản phân biệt vật cao và vật thấp, servo MG996R quay đến một trong 6 ô chứa.

Code viết thẳng trên register, không dùng HAL, chạy trên FreeRTOS V11.1.0. Clock 72 MHz lấy từ thạch anh 8 MHz.

## Phần cứng

| Thiết bị | Chân | Ghi chú |
|---|---|---|
| Nút về 0° | PA1 | Nút thường hở nối GND, dùng pull-up trong |
| Nút +36° | PA2 | Từ 180° quay về 0° |
| Nút lên 180° | PA3 | |
| Nút −36° | PB1 | Dừng ở 0° |
| Nút tăng tốc băng chuyền | PB14 | Tích cực mức cao, dùng pull-down trong, chân chịu được 5 V |
| Nút giảm tốc băng chuyền | PB15 | Như PB14 |
| Servo MG996R | PB9 (TIM4_CH4) | Nguồn riêng 5–6 V, nối chung GND với board |
| L298 ENA | PA0 (TIM2_CH1) | PWM 1 kHz. Tháo jumper ENA, nên gắn 10k kéo xuống GND |
| L298 IN1 / IN2 | PB12 / PB13 | Cố định `1` / `0`. Motor nối OUT1, OUT2. GND L298 nối chung với board |
| Cảm biến vật cản dưới | PA4 | Có vật. Tích cực mức thấp, tín hiệu tối đa 3,3 V |
| Cảm biến vật cản trên | PA5 | Vật cao. Tích cực mức thấp, tín hiệu tối đa 3,3 V |
| TCS34725 SCL / SDA | PB6 / PB7 | I2C1, 100 kHz, cần điện trở kéo lên 3,3 V |
| TCS34725 INT | PB5 | Bắt buộc phải cắm, firmware chờ ngắt mới đọc mẫu |
| TCS34725 LED | PB0 | Mức cao là bật |
| TCS34725 VIN / GND | 3.3 / GND | Chân 3V3 trên module để trống |
| Mã màu ra | PA6 (MSB), PA7 (LSB) | Không nối thẳng vào ngõ vào PLC 24 V |
| ST-Link | PA14 (SWCLK), PA13 (SWDIO), GND, 3.3 | |

Mã màu trên PA6, PA7: `00` không rõ hoặc lỗi, `01` đỏ, `10` xanh lá, `11` xanh dương.

Chip trên board hiện tại là bản clone, chỉ có 3 bit priority ngắt. Vì vậy priority trong `rtos/FreeRTOSConfig.h` và `h_nvic` chỉ dùng 3 bit cao, chạy được cả trên STM32 thật.

## Build và nạp

Cần `arm-none-eabi-gcc` và `stlink` (Arch: `sudo pacman -S stlink`).

```bash
export TOOLCHAINS_PATH=/home/imbdang/Project/homekit_pro/dev_env/matter-sdk-env/cipd/packages/arm
make -j8
make flash
```

`TOOLCHAINS_PATH` là thư mục chứa `bin/arm-none-eabi-gcc`. Khi nạp phải tắt `st-util` trước, vì nó giữ ST-Link.

## Cấu trúc project

```
LineColor/
├── main.c
├── config.h               # toàn bộ số cần chỉnh: gain, cụm màu, thời gian, góc servo, task
├── color_log.gdb          # script đọc giá trị cảm biến qua ST-Link
├── application/
│   ├── app/               # vòng lặp các task FreeRTOS: a_button, a_servo, a_height, a_color, a_motor
│   └── state_driven/      # FSM và logic phân loại, không gọi register hay FreeRTOS
├── middle/                # API dùng chung: clock, thời gian, GPIO, EXTI, PWM, I2C, TCS34725
├── hardware/              # register, bảng chân, startup, linker
└── rtos/                  # FreeRTOSConfig.h và kernel
```

Chiều phụ thuộc là `application` gọi `middle`, `middle` gọi `hardware`. Thông số nghiệp vụ nằm ở `config.h`, số chân nằm ở `hardware/board/hardware.h`.

## Cách hoạt động

| Task | Ưu tiên | Chạy khi | Việc làm |
|---|---|---|---|
| `a_servo` | 3 | Có lệnh trong queue | Xuất xung PWM đến vị trí được yêu cầu |
| `a_button` | 2 | Mỗi 5 ms | Quét nút, chống dội 20 ms, gửi lệnh servo |
| `a_height` | 2 | Mỗi 5 ms | Đọc PA4, PA5, lọc 20 ms |
| `a_motor` | 2 | Mỗi 5 ms | Quét 2 nút tốc độ, chống dội 20 ms, đặt PWM motor băng chuyền |
| `a_color` | 1 | Ngắt PB5, tối đa chờ 100 ms | Đọc cảm biến, phân loại, quyết định ô |

Xử lý một vật diễn ra theo thứ tự:

1. PA4 báo có vật, firmware bật LED và bỏ 2 mẫu đầu vì LED chưa ổn định.
2. Cứ khoảng 12 ms cảm biến báo ngắt một lần, firmware đọc và phân loại mẫu đó.
3. Hai mẫu liên tiếp cùng màu thì màu được xác nhận và xuất ra PA6, PA7.
4. Màu và chiều cao phải giữ nguyên 120 ms (`CFG_SORT_DECISION_MS`) thì servo mới nhận lệnh, để tránh quay theo màu thoáng qua ở mép vật.
5. PA4 nhả, firmware tắt LED. Servo giữ nguyên vị trí đến vật tiếp theo.

Tổng thời gian từ lúc vật vào đến khi servo bắt đầu quay ước tính khoảng 200 ms, chưa tính thời gian servo di chuyển.

Vị trí ô theo màu và chiều cao (vật cao là khi PA5 báo):

| Màu | Vật thấp | Vật cao |
|---|---|---|
| Đỏ | 0° | 36° |
| Xanh lá | 72° | 108° |
| Xanh dương | 144° | 180° |

Bốn nút bấm vẫn hoạt động song song, nút và màu gửi chung một queue nên lệnh đến sau sẽ được thực hiện.

Băng chuyền chạy ngay khi cấp nguồn với duty 50%. Mỗi lần bấm PB14 tăng 10%, bấm PB15 giảm 10%, dừng ở 20% và 80%. Giữ nút không lặp lại, bấm cả hai cùng lúc thì bỏ qua. Các số này nằm ở `CFG_MOTOR_DUTY_*` trong `config.h`. Băng chuyền chạy ngược thì đảo 2 dây motor ở OUT1, OUT2.

## Calib màu

Firmware không tự calib. Các cụm màu là số đo thật, được ghi cố định trong `config.h` và chỉ đúng với **đúng vị trí lắp, gain và ATIME lúc đo**. Thay đổi một trong các yếu tố đó, hoặc tháo lắp lại cảm biến, thì phải đo lại.

### Firmware phân loại một mẫu như thế nào

Code nằm ở `application/state_driven/color_classify.c`:

1. **Đọc C, R, G, B** với gain 60x và ATIME `0xFC` (9,6 ms).
2. **Trừ hồng ngoại:** `IR = (R + G + B − C) / 2`, trừ khỏi cả 4 kênh.
3. **Giảm kênh R và G** theo `CFG_COLOR_R_RAW_WEIGHT` và `CFG_COLOR_G_RAW_WEIGHT`.
4. **Tính tỉ lệ phần nghìn** so với C: `r = R × 1000 / C`, tương tự với `g`, `b`.
5. **Chọn bộ cụm theo chiều cao:** `CFG_COLOR_REFS_LOW` khi PA5 nhả, `CFG_COLOR_REFS_TALL` khi PA5 báo.
6. **Tìm cụm gần nhất** theo khoảng cách trong không gian `(r, g, b)`. Khoảng cách tới cụm xanh lá được nhân thêm hệ số phạt `CFG_COLOR_GREEN_PENALTY` (vật cao dùng `CFG_COLOR_GREEN_PENALTY_TALL`).
7. **Loại mẫu không tin cậy:** quá tối (`C < CFG_COLOR_MIN_CLEAR`), bão hòa (từ 3973 trở lên ở ATIME hiện tại) hoặc xa mọi cụm hơn `CFG_COLOR_MAX_DISTANCE` đều trả về "không rõ màu".

Lý do dùng cụm màu thay vì ngưỡng theo kênh trội: vật xanh lá đục có R gần bằng G, nên kênh G không hề nổi trội.

Không có cụm cho nền vì băng chuyền màu xanh lá, đọc gần giống vật xanh lá. Việc có vật hay không do cảm biến vật cản PA4 quyết định, LED và phép đo màu chỉ chạy khi PA4 báo.

### Đọc log qua ST-Link

Terminal 1:

```bash
st-util
```

Terminal 2:

```bash
cd ~/Project/LineColor
gdb -q -x color_log.gdb build/main.elf
```

Log hiện trên màn hình và ghi vào `color_log.txt`, mỗi lần mở log sẽ ghi đè lần trước.

```
n=13 c=256 r=83 g=96 b=72 | r%=32 g%=37 b%=28 | color=0 state=2 id=0x4d err=0 h=1
```

| Cột | Ý nghĩa |
|---|---|
| `n` | Số mẫu đã đọc |
| `c r g b` | Giá trị đã trừ hồng ngoại, `r` và `g` đã nhân hệ số |
| `r% g% b%` | Tỉ lệ so với `c` |
| `color` | 0 chưa nhận ra, 1 đỏ, 2 xanh lá, 3 xanh dương |
| `state` | 0 khởi tạo, 1 chờ LED, 2 đang lấy mẫu, 3 đã xác nhận, 4 lỗi, 5 chờ vật (LED tắt) |
| `id` | Mã chip, phải là `0x44` hoặc `0x4d` |
| `err` | Số lần lỗi, nên đứng yên |
| `h` | 0 không có vật, 1 vật thấp, 3 vật cao, 2 bất thường (chỉ cảm biến trên báo) |

Khi dừng: bấm Ctrl+C trong gdb, gõ `quit`, bấm Ctrl+C ở `st-util`, rồi bấm nút RESET trên board. **Không dùng `st-flash reset`**, vì chip clone này sẽ bị treo.

Log chạy khoảng 1 dòng mỗi giây do GDB dừng CPU ở mỗi dòng. Trong lúc log, đừng đánh giá tốc độ phản ứng của hệ thống.

### Quy trình calib

1. Cố định cảm biến ở vị trí lắp thật, che sáng nếu có thể.
2. Mở log như trên.
3. Đặt lần lượt từng loại vật vào đúng vị trí đo, mỗi loại khoảng 15 giây: đỏ, xanh lá, xanh dương, mỗi màu cả vật thấp và vật cao. Kiểm tra `h` đúng là 1 hoặc 3 và `state` khác 5, rồi ghi lại khoảng `n` của từng loại.
4. Tính trung bình cho từng khoảng `n`, thay `a` và `b` bằng đầu và cuối khoảng:

   ```bash
   awk -F'[= ]' -v a=120 -v b=135 '$1=="n" && $2>=a && $2<=b && $4>0 { r+=$6*1000/$4; g+=$8*1000/$4; bl+=$10*1000/$4; k++ } END { if (k) printf "r=%d g=%d b=%d (%d mau)\n", r/k, g/k, bl/k, k }' color_log.txt
   ```

5. Điền kết quả vào `CFG_COLOR_REFS_LOW` (vật thấp) và `CFG_COLOR_REFS_TALL` (vật cao) trong `config.h`. Một màu có thể có nhiều cụm nếu cần.
6. Chạy `make flash`, mở log lại và đưa từng vật vào để kiểm tra cột `color`.

### Khi kết quả chưa đúng

| Hiện tượng | Cách chỉnh |
|---|---|
| Xanh dương hay bị nhận thành xanh lá | Tăng `CFG_COLOR_GREEN_PENALTY` |
| Có vật nhưng `color` luôn bằng 0 | Tăng `CFG_COLOR_MAX_DISTANCE` hoặc thêm cụm cho màu đó |
| `c` gần 3973 rồi `color` về 0 | Cảm biến bão hòa: giảm gain hoặc tăng khoảng cách |
| Số đo nhảy nhiều, `c` rất nhỏ | Tín hiệu yếu: đưa cảm biến lại gần hoặc che sáng |
| Servo phản ứng chậm | Giảm `CFG_SORT_DECISION_MS`, sau đó đến `CFG_COLOR_CONFIRM_SAMPLES` |
| Servo quay nhầm khi mép vật đi vào | Tăng `CFG_SORT_DECISION_MS` |

Nếu đổi `CFG_COLOR_R_RAW_WEIGHT` hoặc `CFG_COLOR_G_RAW_WEIGHT` thì phải đo lại toàn bộ cụm màu, vì `r` và `g` trong cụm đã được nhân hệ số này.

## Việc còn phải kiểm tra

Góc 0° và 180° của servo đã xác nhận trên servo thật. Bốn vị trí giữa trong `CFG_SERVO_PULSE_US` vẫn là giá trị ước tính, cần đo lại.
