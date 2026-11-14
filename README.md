# 3D House — Computer Graphics Midterm

Dự án dựng mô hình **ngôi nhà 3D bằng C++ và OpenGL** phục vụ môn Đồ họa máy tính. Project sử dụng pipeline OpenGL cổ điển (fixed-function pipeline) để nhóm có thể liên hệ trực tiếp mã nguồn với các nội dung thường học trên lớp: phép biến đổi mô hình, camera, phép chiếu, depth buffer, normal, ánh sáng và vật liệu.

> **Trạng thái:** phiên bản nền (starter) đã có nhà, mái, cửa, cửa sổ, hiên, sân vườn, cây, hàng rào, camera orbit, zoom, hai chế độ chiếu và bật/tắt ánh sáng. Nhóm tiếp tục mở rộng theo phân công trong [docs/04_TEAM_ASSIGNMENT.md](docs/04_TEAM_ASSIGNMENT.md).

## 1. Demo hiện tại

- Nhà 3D gồm thân nhà, mái chữ A, ống khói, cửa chính, cửa sổ, viền trang trí và hiên.
- Cảnh quan gồm mặt đất, lối đi, bồn hoa, cây và hàng rào.
- Camera xoay quanh nhà bằng chuột trái; zoom bằng con lăn hoặc `+` / `-`.
- Chuyển đổi Perspective / Orthographic bằng `P`.
- Bật/tắt ánh sáng bằng `L`; đặt lại góc nhìn bằng `R`.

## 2. Công nghệ

| Thành phần | Công nghệ |
|---|---|
| Ngôn ngữ | C++17 |
| Đồ họa | OpenGL compatibility / fixed-function pipeline |
| Cửa sổ và input | FreeGLUT |
| Phép chiếu / camera helper | GLU |
| Build | CMake 3.16+ |

Project chủ động dùng `glBegin/glEnd`, matrix stack, `glTranslatef`, `glRotatef`, `glScalef`, `gluLookAt`, `gluPerspective` và fixed-function lighting để phù hợp mục tiêu học tập nhập môn. Đây **không phải** renderer OpenGL Core Profile hiện đại.

## 3. Cài đặt và chạy

### Fedora

```bash
sudo dnf install gcc-c++ cmake freeglut-devel mesa-libGLU-devel
```

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev
```

### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/3d-house-opengl
```

Cần có môi trường desktop có hỗ trợ OpenGL. Chạy qua SSH/headless cần cấu hình display server hoặc X forwarding phù hợp.

## 4. Điều khiển

| Phím / thao tác | Chức năng |
|---|---|
| Kéo chuột trái | Orbit camera quanh ngôi nhà |
| Con lăn chuột | Phóng to / thu nhỏ |
| `+` hoặc `=` | Phóng to |
| `-` hoặc `_` | Thu nhỏ |
| `P` | Chuyển Perspective / Orthographic |
| `L` | Bật / tắt ánh sáng |
| `R` | Reset camera và tùy chọn hiển thị |
| `Q` hoặc `Esc` | Thoát chương trình |

## 5. Cấu trúc thư mục

```text
3d-house-opengl/
├── .github/
│   ├── PULL_REQUEST_TEMPLATE.md
│   └── workflows/build.yml
├── docs/
│   ├── 01_PROJECT_BRIEF.md
│   ├── 02_GRAPHICS_CONCEPTS.md
│   ├── 03_ARCHITECTURE.md
│   ├── 04_TEAM_ASSIGNMENT.md
│   ├── 05_WORK_PLAN.md
│   ├── 06_TEST_PLAN.md
│   ├── 07_PRESENTATION_OUTLINE.md
│   └── REPORT_TEMPLATE.md
├── include/house/
│   ├── camera.hpp
│   ├── render_utils.hpp
│   └── scene.hpp
├── src/
│   ├── camera.cpp
│   ├── environment.cpp
│   ├── house.cpp
│   ├── main.cpp
│   └── render_utils.cpp
├── CMakeLists.txt
├── CONTRIBUTING.md
├── LICENSE
└── README.md
```

## 6. Tài liệu dành cho nhóm

- [Đề cương và phạm vi project](docs/01_PROJECT_BRIEF.md)
- [Các kiến thức đồ họa cần giải thích](docs/02_GRAPHICS_CONCEPTS.md)
- [Kiến trúc và luồng render](docs/03_ARCHITECTURE.md)
- [Phân công cụ thể cho 3 thành viên](docs/04_TEAM_ASSIGNMENT.md)
- [Kế hoạch thực hiện và mốc bàn giao](docs/05_WORK_PLAN.md)
- [Checklist kiểm thử và nghiệm thu](docs/06_TEST_PLAN.md)
- [Dàn ý thuyết trình giữa kỳ](docs/07_PRESENTATION_OUTLINE.md)
- [Mẫu báo cáo để nhóm điền](docs/REPORT_TEMPLATE.md)
- [Quy ước branch, commit và pull request](CONTRIBUTING.md)

## 7. Kiến thức đồ họa trọng tâm

Mỗi đỉnh của mô hình được biến đổi từ không gian cục bộ sang không gian clip qua chuỗi ma trận khái quát:

`P_clip = P × V × M × P_local`

Trong đó `M` là Model matrix, `V` là View matrix và `P` là Projection matrix. Nhóm cần chỉ ra được trong code nơi áp dụng từng phép biến đổi; xem [docs/02_GRAPHICS_CONCEPTS.md](docs/02_GRAPHICS_CONCEPTS.md) để có giải thích và câu hỏi tự kiểm tra.

## 8. Đóng góp

Mỗi thành viên làm trên branch riêng, tạo Pull Request về `main`, tự build và kiểm tra phần mình trước khi nhờ thành viên khác review. Không commit thẳng lên `main`. Quy trình chi tiết nằm trong [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT. Xem [LICENSE](LICENSE).
