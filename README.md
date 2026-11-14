# 3D House — Computer Graphics Midterm

Dự án dựng mô hình **ngôi nhà 3D bằng C++ và OpenGL** phục vụ môn Đồ họa máy tính. Project sử dụng OpenGL fixed-function pipeline để minh họa primitive, phép biến đổi, camera, phép chiếu, depth buffer, normal, ánh sáng và vật liệu.

## Tính năng hiện tại

- Nhà 3D gồm thân nhà, mái chữ A, ống khói, cửa chính, cửa sổ, hiên và phần trang trí.
- Sân vườn gồm mặt đất, lối đi, bồn hoa, cây và hàng rào.
- Camera orbit bằng chuột trái; zoom bằng con lăn hoặc phím + / -.
- Chuyển Perspective / Orthographic bằng P.
- Bật / tắt ánh sáng bằng L; reset bằng R.

## Công nghệ

| Thành phần | Công nghệ |
|---|---|
| Ngôn ngữ | C++17 |
| Đồ họa | OpenGL compatibility / fixed-function |
| Window và input | FreeGLUT |
| Camera / projection helper | GLU |
| Build | CMake 3.16+ |

Đây là project học tập dùng pipeline OpenGL cổ điển, không phải renderer OpenGL Core Profile hiện đại.

## Cài đặt

Fedora:
```bash
sudo dnf install gcc-c++ cmake freeglut-devel mesa-libGLU-devel
```

Ubuntu / Debian:
```bash
sudo apt update
sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev
```

Build và chạy:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/3d-house-opengl
```

Cần desktop có hỗ trợ OpenGL để mở cửa sổ đồ họa.

## Điều khiển

| Thao tác | Chức năng |
|---|---|
| Kéo chuột trái | Orbit camera |
| Con lăn | Zoom |
| + / = | Zoom in |
| - / _ | Zoom out |
| P | Perspective / Orthographic |
| L | Bật / tắt lighting |
| R | Reset |
| Q / Esc | Thoát |

## Cấu trúc

```text
.github/                 # GitHub Actions và PR template
docs/                    # project guide và report template
include/house/           # camera, scene và render API
src/
  camera.cpp             # camera orbit / zoom
  environment.cpp        # ground, trees, flowers, fence
  house.cpp              # house geometry
  main.cpp               # FreeGLUT, projection, lighting, input, HUD
  render_utils.cpp       # drawBox, material
CMakeLists.txt
CONTRIBUTING.md
LICENSE
README.md
```

## Tài liệu nhóm

- [Project guide](docs/PROJECT_GUIDE.md): mục tiêu, kiến trúc, lý thuyết đồ họa, phân công 3 thành viên, kế hoạch, checklist test và dàn ý thuyết trình.
- [Report template](docs/REPORT_TEMPLATE.md): khung báo cáo giữa kỳ để nhóm điền kết quả và ảnh thực tế.
- [Contributing guide](CONTRIBUTING.md): branch, commit, pull request và review.

## Chuỗi biến đổi cần nắm

Một điểm từ object space qua clip space:

`p_clip = P × V × M × p_local`

Trong đó M là Model, V là View và P là Projection. Khi thuyết trình, nhóm cần chỉ ra cách mã nguồn hiện thực từng phần và demo trực tiếp hai chế độ chiếu.

## License

MIT. Xem [LICENSE](LICENSE).
