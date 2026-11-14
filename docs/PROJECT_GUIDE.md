# Project guide — 3D House OpenGL

## Mục tiêu và phạm vi
Dựng ngôi nhà 3D bằng C++17, OpenGL fixed-function pipeline, FreeGLUT, GLU và CMake. Phần bắt buộc: thân nhà, mái, cửa, cửa sổ, hiên, sân vườn, camera có điều khiển, Perspective / Orthographic, depth buffer và lighting. Phần mở rộng nếu còn thời gian: nội thất, texture, gạch mái, ngày/đêm và tương tác cửa.

## Quy ước tọa độ
+X là phải, +Y là lên, -Z là mặt tiền và cửa chính. Gốc tọa độ nằm gần tâm nền nhà.

## Kiến trúc
- main.cpp: cửa sổ, callback, HUD, projection, lighting và vòng render.
- house.cpp: thân nhà, mái, cửa, cửa sổ, hiên và ống khói.
- environment.cpp: sân, lối đi, bồn hoa, cây và hàng rào.
- camera.cpp: orbit, zoom, reset và gluLookAt.
- render_utils.cpp: drawBox và setMaterial.
- include/house: API module.
Một frame: clear buffers → projection → camera/view → lighting → draw scene → HUD → swap buffers.

## Kiến thức cần trình bày
1. Primitive: cube cho tường/cửa, quad và triangle cho mái, sphere cho hoa/tán cây, cone cho chi tiết hàng rào.
2. Model transformation: p' = M p; M kết hợp T (translation), R (rotation), S (scaling). Matrix stack cô lập transform.
3. View: gluLookAt xác định eye, target và up. Camera orbit dùng góc phương vị, góc nâng và bán kính.
4. Projection: gluPerspective tạo phối cảnh; glOrtho tạo phép chiếu song song.
5. Depth: GL_DEPTH_TEST với buffer depth giúp bề mặt gần che bề mặt xa.
6. Lighting: normal, material ambient/diffuse/specular và nguồn sáng ảnh hưởng màu bề mặt.
Chuỗi khái quát: p_clip = P × V × M × p_local.

## PHÂN CÔNG NHÓM 3
| Thành viên | Vai trò | Phạm vi và file chính | Tiêu chí bàn giao |
|---|---|---|---|
| TV1 | 3D House Modeling | Thân nhà, mái chữ A, cửa, cửa sổ, hiên, ống khói — src/house.cpp | Mô hình rõ, mặt tiền -Z, không xuyên hình lớn; có screenshot nhiều góc |
| TV2 | Environment & Scene | Mặt đất, lối đi, bồn hoa, cây, hàng rào — src/environment.cpp | Bố cục cân đối, object đặt đúng mặt đất, không z-fighting rõ |
| TV3 | Graphics & Integration | Camera, projection, lighting, input, HUD, CMake và CI — src/camera.cpp, src/main.cpp, CMakeLists.txt | Orbit/zoom ổn định, hai phép chiếu hoạt động, build và tích hợp thành công |

Tên và GitHub username TV1/TV2 chưa được cung cấp: nhóm tự điền trong báo cáo, không tự gán username. Cả ba cùng review PR, chạy test, viết báo cáo và chuẩn bị thuyết trình. TV1/TV2 làm trên file geometry riêng; TV3 quản lý main/camera/build để giảm merge conflict.

## Kế hoạch
- M0: cả nhóm clone, cài dependency, build baseline, thống nhất tọa độ.
- M1: TV1/TV2/TV3 phát triển theo module và mở PR riêng.
- M2: review, xử lý conflict, tích hợp vào main.
- M3: polish chi tiết, vật liệu, ánh sáng và bố cục.
- M4: chạy test, chụp ảnh, hoàn tất report/slide/demo.
Definition of Done: đúng yêu cầu, build lại Release, kiểm tra trực quan, docs cập nhật và có một người khác review.

## Checklist kiểm thử
- [ ] Mở app thấy nhà và cảnh quan.
- [ ] Kéo chuột trái orbit ngang/dọc.
- [ ] Zoom bằng wheel và + / - có giới hạn.
- [ ] P chuyển Perspective / Orthographic.
- [ ] L bật/tắt lighting.
- [ ] R reset; Q / Esc thoát.
- [ ] Resize không làm scene méo.
- [ ] Không có vật thể lơ lửng, xuyên hình hoặc z-fighting nghiêm trọng.
- [ ] Lưu screenshot góc trước, góc bên, góc chéo, so sánh projection và lighting.

## Dàn ý thuyết trình
Giới thiệu đề tài → ý tưởng và tọa độ → primitive mô hình nhà → cảnh quan → camera và projection → lighting/depth buffer → demo → kết luận. Mỗi thành viên giải thích phần mình và liên hệ trực tiếp tới code; không trình bày tính năng đề xuất như thể đã hoàn thành.
