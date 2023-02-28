#pragma once


/* dữ liệu âm thanh dùng kiểu short */
typedef struct {
   unsigned int soLuongMau;   // số lượng mẫu trong một kênh
   unsigned int tocDoMau;        // tốc độ mẫu (Hz)
   unsigned char soLuongKenh;    // số lượng kênh (bây giờ chỉ hỗ trợ nhi) kênh
   unsigned char soLuongLan;     // số lượng làn

   // ---- dùng kiểu int cho dễ đơn vị hóa sau
   int *mangMau;               // mảng mẫu
   int *lan0;                  // danh sách lán
   int *lan1;
   int *lan2;
   int *lan3;
   int *lan4;
   int *lan5;
   int *lan6;
   int *lan7;
} DuLieuAmThanh_short;


/* dữ liệu âm thanh dùng kiểu float */
typedef struct {
   unsigned int soLuongMau;   // số lượng mẫu trong một kênh
   unsigned int tocDoMau;        // tốc độ mẫu (Hz)
   unsigned char soLuongKenh;    // số lượng kênh (bây giờ chỉ hỗ trợ nhi) kênh
   unsigned char soLuongLan;     // số lượng làn
   
   float *mangMau;               // mảng mẫu
   float *lan0;                  // danh sách lán
   float *lan1;
   float *lan2;
   float *lan3;
   float *lan4;
   float *lan5;
   float *lan6;
   float *lan7;
} DuLieuAmThanh_float;