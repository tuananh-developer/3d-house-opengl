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

## PHÂN CÔNG CHÍNH THỨC — NHÓM 3 THÀNH VIÊN

| Thành viên | Vai trò | Phạm vi và file chính | Tiêu chí bàn giao |
|---|---|---|---|
| **Kiệt** | 3D House Modeling | Thân nhà, mái chữ A, cửa, cửa sổ, hiên, ống khói — `src/house.cpp` | Mô hình rõ, cân đối, mặt tiền -Z, có screenshot nhiều góc |
| **Huy** | Environment & Scene | Mặt đất, sân, lối đi, bồn hoa, cây, hàng rào/cổng — `src/environment.cpp` | Bố cục hợp lý, vật thể đặt đúng mặt đất, không z-fighting rõ |
| **Bạn — Nhóm trưởng** | Graphics Lead & Integration | Camera, projection, lighting, input, HUD, CMake, CI và tích hợp — `src/camera.cpp`, `src/main.cpp`, `CMakeLists.txt` | Camera ổn định, hai phép chiếu hoạt động, build pass, PR được review và tích hợp |

### 1. Kiệt — phụ trách mô hình ngôi nhà
- Hoàn thiện thân nhà, móng, mái chính và đầu hồi.
- Thiết kế cửa chính, cửa sổ, khung viền, hiên, cột đỡ và ống khói.
- Cân đối tỷ lệ, vật liệu và màu sắc; kiểm tra normal / điểm giao hình.
- Chụp tối thiểu 3 góc: chính diện, góc chéo, góc bên.
- Làm việc chủ yếu trong `src/house.cpp`; nếu cần API mới phải trao đổi trước khi sửa `include/house/scene.hpp`.

### 2. Huy — phụ trách cảnh quan
- Hoàn thiện mặt đất, sân và đường dẫn vào nhà.
- Bố trí cây, bồn hoa, hàng rào và cổng.
- Bổ sung đèn sân / vật thể trang trí nếu phần bắt buộc đã ổn định.
- Đảm bảo cảnh quan không che khuất ngôi nhà, không lơ lửng và không z-fighting.
- Làm việc chủ yếu trong `src/environment.cpp`; tái sử dụng `drawBox` và material utilities.

### 3. Bạn — nhóm trưởng, phụ trách đồ họa và tích hợp
- Quản lý tiến độ, chia task, review PR và giữ `main` luôn build được.
- Hoàn thiện orbit camera, zoom, reset và giới hạn góc nhìn.
- Kiểm tra / giải thích View, Perspective, Orthographic, depth buffer, normal, lighting và material.
- Quản lý input, HUD, CMake, GitHub Actions và tích hợp code của Kiệt / Huy.
- Chủ trì cấu trúc báo cáo, slide, kịch bản demo; mỗi thành viên vẫn phải viết nội dung phần mình.
- Phụ trách các file `src/camera.cpp`, `src/main.cpp`, `include/house/camera.hpp`, `CMakeLists.txt` và workflow CI.

## Quy tắc phối hợp và tránh conflict
1. Kiệt sửa `src/house.cpp`; Huy sửa `src/environment.cpp); nhóm trưởng quản lý `src/main.cpp`, camera và CMake.
2. Không tự ý đổi quy ước tọa độ: +Y lên, mặt tiền -Z.
3. Mỗi người tạo branch riêng: `feature/kiet-house-model`, `feature/huy-environment`, `feature/team-lead-camera-integration`.
4. Mở PR về `main`, ghi rõ mục tiêu, file thay đổi, cách test và ảnh chụp màn hình.
5. Nhóm trưởng review và merge sau khi CI pass; không commit thẳng lên main.
6. Cả ba cùng chạy demo, kiểm thử, viết báo cáo và luyện phần hỏi đáp. Không ai chỉ biết mỗi module của mình.

## Kế hoạch thực hiện
- M0: cả nhóm clone, cài dependency, build baseline, thống nhất tọa độ.
- M1: Kiệt, Huy và nhóm trưởng phát triển song song theo module.
- M2: review, xử lý conflict, tích hợp vào main.
- M3: hoàn thiện chi tiết, vật liệu, ánh sáng và bố cục.
- M4: kiểm thử cuối, chụp ảnh, hoàn tất report / slide / demo.

Definition of Done: đúng yêu cầu, build lại Release, kiểm tra trực quan, docs cập nhật và có ít nhất một thành viên khác review.

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
Giới thiệu đề tài → ý tưởng và tọa độ → primitive mô hình nhà (Kiệt) → cảnh quan (Huy) → camera và projection → lighting/depth buffer → demo → kết luận. Nhóm trưởng điều phối; từng thành viên giải thích phần mình và liên hệ trực tiếp tới code. Không trình bày tính năng đề xuất như thể đã hoàn thành.
