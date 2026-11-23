# Modern 3D Landscape Architecture Engine (Modern C++ / OpenGL 3.3+ Core)

Đồ án môn **Đồ họa máy tính** (Computer Graphics) xây dựng không gian ngoại cảnh hiện đại xung quanh biệt thự (tọa độ gốc `(0,0,0)`) bằng **Modern C++ (C++17)** và **OpenGL 3.3+ Core Profile**.

> **Tuyệt đối không sử dụng OpenGL cổ điển (`glBegin`/`glEnd`/`glPushMatrix`).**  
> 100% hình học được dựng bằng VAO/VBO/EBO, tính toán ma trận với GLM và đổ bóng/chiếu sáng bằng GLSL 330 core shaders.

---

## 1. Cài đặt thư viện trên Ubuntu Linux (cực nhanh qua `apt`)

Chạy lệnh duy nhất sau trong Terminal:

```bash
sudo apt update
sudo apt install -y build-essential libglfw3-dev libglm-dev
```

*(Lưu ý: Dự án đã tích hợp sẵn thư viện `glad` và `glm` ngay trong thư mục `include/`, do đó bạn có thể biên dịch ngay lập tức mà không cần cấu hình phức tạp).*

---

## 2. Lệnh biên dịch bằng `g++` trên Ubuntu Linux

### Cách 1: Sử dụng `make` (Khuyên dùng)
```bash
cd modern_landscape
make
./modern_landscape
```

### Cách 2: Lệnh `g++` trực tiếp với GLFW (Modern Standard)
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 \
    -Iinclude src/main.cpp src/glad.c \
    -lglfw -lGL -ldl -lpthread -o modern_landscape

./modern_landscape
```

### Cách 3: Chạy dự phòng với FreeGLUT (nếu máy chưa cài `libglfw3-dev`)
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -DUSE_GLUT \
    -Iinclude src/main.cpp src/glad.c \
    -lglut -lGL -ldl -lpthread -o modern_landscape

./modern_landscape
```

---

## 3. Các thuật toán đồ họa cốt lõi đã cài đặt

### A. Thuật toán Perlin Noise 2D & Fractal Brownian Motion (`include/PerlinNoise.hpp`, `include/Terrain.hpp`)
- **Nguyên lý toán học:**
  - Lưới ô vuông với vector gradient giả ngẫu nhiên sinh từ bảng hoán vị 512 phần tử.
  - Khử gián đoạn đạo hàm bậc 2 bằng hàm làm mịn Hermite bậc 5 của Ken Perlin:  
    $$f(t) = 6t^5 - 15t^4 + 10t^3$$
  - Tổng hợp đa tầng tần số (fBm - Fractal Brownian Motion):  
    $$\text{fBm}(x, z) = \sum_{i=0}^{\text{octaves}-1} \text{amplitude}_i \cdot \text{noise}(x \cdot \text{freq}_i, z \cdot \text{freq}_i)$$
- **Kỹ thuật làm phẳng nền biệt thự (Radial Villa Flattening):**
  - Biệt thự và hồ bơi tại $(0,0,0)$ cần mặt bằng bằng phẳng tuyệt đối. Sử dụng hàm suy giảm mượt $\text{smoothstep}$:
    $$w(r) = \text{smoothstep}(R_{\text{flat}}, R_{\text{blend}}, r), \quad r = \sqrt{x^2 + z^2}$$
    $$h(x, z) = \text{fBm}(x, z) \times w(r)$$
  - Nhờ đó, sân biệt thự phẳng phiu, trong khi khu vườn ngoài nhấp nhô đồi cỏ tự nhiên.
- **Tính toán vector pháp tuyến (Normals):** Sử dụng sai phân hữu hạn trung tâm (Central Differences) trên lưới độ cao để tính pháp tuyến chính xác cho ánh sáng Blinn-Phong.

---

### B. Thuật toán L-System & Đệ quy Fractal 3D (`include/ProceduralTree.hpp`)
- **Văn phạm hình thức L-System:**
  - $V = \{F, +, -, \&, \wedge, \backslash, /, [, ]\}$
  - Cơ chế ngăn xếp đồ họa Rùa 3D (3D Turtle Stack):
    `struct TurtleState { vec3 pos, heading, up, left; float radius, length; int depth; };`
- **Đệ quy Fractal 3D ngẫu nhiên (Stochastic Branching):**
  - Mỗi nhánh con suy giảm bán kính $r_{\text{child}} = r_{\text{parent}} \times 0.74$.
  - Góc xoay nhánh (pitch, yaw, roll) được thêm nhiễu ngẫu nhiên (Jitter) theo phân phối đều, đảm bảo không có 2 cây nào bị trùng lặp cứng nhắc.
- **Dựng hình học 3D:**
  - Mỗi đoạn nhánh được tạo thành hình trụ đa giác (Cylinder mesh 8 cạnh) nối liền hai trạng thái.
  - Đầu cành tạo tán lá xum xuê (Foliage Clusters) bắt sáng lộng lẫy dưới ánh mặt trời.
  - Hỗ trợ nhiều chủng loại cây: Cây sồi che bóng mát (Oak), Cây hoa anh đào (Sakura), Cây thông Zen Nhật Bản (Pine) và Bụi cây cảnh tầm thấp (Shrubs).

---

### C. Đường cong Bézier bậc 3 (`include/BezierPath.hpp`)
- **Phương trình đường cong bậc 3:**
  $$\vec{B}(t) = (1-t)^3 \vec{P}_0 + 3(1-t)^2 t \vec{P}_1 + 3(1-t) t^2 \vec{P}_2 + t^3 \vec{P}_3, \quad t \in [0, 1]$$
- **Vector tiếp tuyến:**
  $$\vec{T}(t) = \vec{B}'(t) = 3(1-t)^2(\vec{P}_1 - \vec{P}_0) + 6(1-t)t(\vec{P}_2 - \vec{P}_1) + 3t^2(\vec{P}_3 - \vec{P}_2)$$
- **Khung tọa độ di động (Frenet-Serret Normal):**
  $$\vec{N}(t) = \text{normalize}(\vec{T}(t) \times \vec{\text{Up}})$$
- **Tạo lối đi lát đá tự nhiên:**
  - Dựng 48 phiến đá hình hộp chữ nhật có độ dày ($0.08\text{m}$), chiều rộng ($1.6\text{m}$) uốn lượn chữ S từ cổng vào sảnh biệt thự.
  - Mỗi góc phiến đá lấy độ cao địa hình $Y = \text{terrain.getHeight}(X, Z) + 0.08$ để bám sát sườn đồi cỏ, triệt tiêu Z-fighting.
  - Bố trí các trụ đèn năng lượng mặt trời mini dọc lối đi.

---

### D. Chiếu sáng Blinn-Phong + Real-time Shadow Mapping (`include/ShadowMap.hpp`, `include/Scene.hpp`)
- **Render Pass 1 (Tạo Shadow Map):**
  - Framebuffer Object (FBO) với Depth Texture độ phân giải $2048 \times 2048$ 32-bit float (`GL_DEPTH_COMPONENT32F`).
  - Ma trận biến đổi không gian ánh sáng (Light Space Matrix):
    $$\text{LightSpaceMatrix} = \text{ortho}(-45, 45, -45, 45, 1, 85) \times \text{lookAt}(\vec{\text{lightPos}}, \vec{0}, \vec{\text{Up}})$$
- **Render Pass 2 (Chiếu sáng & Đổ bóng thời gian thực):**
  - **Mô hình Blinn-Phong:** Sử dụng vector nửa góc (Halfway Vector) $\vec{H} = \frac{\vec{L} + \vec{V}}{\|\vec{L} + \vec{V}\|}$:
    $$I = I_{\text{ambient}} + (1 - \text{shadow}) \cdot \left( I_{\text{diffuse}} \cdot (\vec{N} \cdot \vec{L}) + I_{\text{specular}} \cdot (\vec{N} \cdot \vec{H})^{\text{shininess}} \right)$$
  - **Lọc mềm bóng râm PCF (Percentage-Closer Filtering 3x3):** Lấy mẫu 9 texel xung quanh kèm slope-scale depth bias để loại bỏ sọc răng cưa và hiện tượng Peter Panning.
  - **Mặt nước hồ bơi thời gian thực (`waterVertex`, `waterFragment`):**
    - Sóng nước biến thiên tuần hoàn bằng chồng chập sóng sin thời gian thực ($y = \sum A_i \sin(\vec{k}_i \cdot \vec{x} + \omega_i t)$).
    - Độ bóng Specular cực cao ($\text{shininess} = 160.0$) mô phỏng ánh nắng lấp lánh phản xạ trên mặt nước trong xanh.

---

### E. Điều khiển Camera (`include/Camera.hpp`)
- **Chế độ 1: First-Person Fly/Walk Camera (Mặc định)**
  - Phím `W, A, S, D`: Di chuyển tới, lui, trái, phải.
  - Phím `Space`: Bay lên cao.
  - Phím `Shift` (hoặc phím `C`): Hạ xuống thấp.
  - Di chuyển chuột: Xoay góc nhìn 360° tự do (Yaw & Pitch khóa ở $[-89^\circ, 89^\circ]$).
  - Cuộn chuột: Thu phóng trường nhìn (Zoom FOV).
- **Chế độ 2: Orbit Camera**
  - Nhấn phím `V` để chuyển đổi chế độ camera.
  - Tự động xoay quanh tâm biệt thự và cảnh quan hồ bơi.

