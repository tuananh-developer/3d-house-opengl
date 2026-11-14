# Quy ước đóng góp

## Branch
Không commit thẳng lên main. Dùng feature/house-model, feature/environment, feature/camera-rendering, fix/<short-description> hoặc docs/<short-description>.

## Commit
Dùng type: short description, ví dụ feat: add roof details, fix: camera zoom limit, docs: add graphics notes, refactor: split scene modules.

## Pull Request
Mô tả mục tiêu, file/module thay đổi, cách build/test, screenshot nếu thay đổi hình ảnh và lưu ý API/tọa độ liên quan. Ít nhất một người khác review trước merge.

## Build trước PR
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Không commit thư mục build, binary hoặc file IDE. Nếu đổi header hoặc quy ước tọa độ, cập nhật nơi gọi và báo các thành viên liên quan.
