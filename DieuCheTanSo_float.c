// 2020.09.16 ĐiềuChếTầnSố float

// Hệ thống có bốn 'giao động' (phát sóng) cho mỗi kênh
// Mỗi kênh có hai bộ Bộ Âm lượng và Bộ Tần Số chứa thông tin về bốn giao động:
//    Hệ Số Bộ Âm lượng
//      - loạiChỉnh
//      - nhịpChỉnh - chỉ cho chỉnh ngẫu nhiên
//      - tầnSốChỉnh

//      - trungBình m1
//      - trungBình m2
//      - trungBình c1
//      - trungBình c2

//      - biênĐồ m1
//      - biênĐồ m2
//      - biênĐồ c1
//      - biênĐồ c2

//    Hệ Số Bộ Tần Số
//      - loạiChinh
//      - tầnSốChỉnh

//      - trungBình m1
//      - trungBình m2
//      - trungBình c1
//      - trungBình c2

//      - biênĐồ m1
//      - biênĐồ m2
//      - biênĐồ c1
//      - biênĐồ c2


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "HangSo.h"
#include "DuLieuAmThanh.h"
#include "LuuAmThanhWAV.h"

//#define k16_SANG_FLOAT  0.00065714121
#define kPI      3.1415926f
#define kHAI_PI  6.2831852f

#define kCHINH__LOAI_KHONG_CHINH  0  // không chỉnh tần số
#define kCHINH__LOAI_TAM_GIAC     2  // tam giác
#define kCHINH__LOAI_HUYEN_NAO    3  // huyên náo
#define kCHINH__LOAI_PARABOL      4  // parabol - bậc hai
#define kCHINH__LOAI_DOC          5  // dốc - bậc một

#define kCHINH__NHIP_8_MAU   0x00f   // 16 vì nhị kênh
#define kCHINH__NHIP_16_MAU  0x01f   // 32 vì nhị kênh
#define kCHINH__NHIP_32_MAU  0x03f   // 64 vì nhị kênh
#define kCHINH__NHIP_64_MAU  0x07f   // 128 vì nhị kênh
#define kCHINH__NHIP_128_MAU 0x0ff   // 256 vì nhị kênh
#define kCHINH__NHIP_256_MAU 0x1ff   // 512 vì nhị kênh


// ----
void donViHoaLanFloat( float *lan,  unsigned int soLuongMau, float muc );
void phaTronHetLanVaoLan0( DuLieuAmThanh_float *duLieuAmThanh );

// ----
float doc( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi );
float songTamGiacf( float giaTri );
float dayParabol( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi );

// ----
typedef struct {
   
   unsigned char loaiChinh;
   unsigned short nhipChinh;  // chỉ cho đổi ngẫu nhiên
   float tanSoChinh;   // Hz - tần số chỉnh
   
   // ---- giá trị trung bình
   float m1;
   float m2;
   float c1;
   float c2;
   
   // ---- biên độ
   float bienDo_m1;
   float bienDo_m2;
   float bienDo_c1;
   float bienDo_c2;
   
} HeSoBo_float;

void chinhHeSo_float( HeSoBo_float *bo, unsigned char loaiChinh, unsigned int soMau, float thoiGian,
                     float *giaTri_m1, float *giaTri_m2, float *giaTri_c1, float *giaTri_c2 );

typedef struct {
   
   unsigned char giaiThuat;   // giải thuật điều chế tần số
   float tongAmLuong;
   float phaPhanHoi;  // phản hội
   
   // ---- âm lượng
   HeSoBo_float amLuong;
   
   // ---- tân số
   HeSoBo_float tanSo;
   
} DieuCheTanSo_float;

void giaiThuat0( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat );
void giaiThuat2( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat );
void giaiThuat3( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat );
void giaiThuat4( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat );
void giaiThuat5( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat );
void giaiThuat6( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat );
void giaiThuat7( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat );

int main( int argc, char **argv) {

   // ---- tạo đệm
   float thoiGian = 8.0f;  // 8 giây cho thử nghiệm
   unsigned int soMau = thoiGian*kTAN_SO_MAU_FLOAT;
   
   // ---- chuẩn bị dữ liệu âm thanh
   DuLieuAmThanh_float duLieuAmThanh;
   duLieuAmThanh.soLuongMau = soMau;
   duLieuAmThanh.tocDoMau = kTAN_SO_MAU_FLOAT;
   duLieuAmThanh.soLuongKenh = kNHI_KENH;
   duLieuAmThanh.mangMau = calloc( soMau*kNHI_KENH*kSO_LUONG__LAN, sizeof( float ) );
   duLieuAmThanh.lan0 = duLieuAmThanh.mangMau;
   duLieuAmThanh.lan1 = duLieuAmThanh.lan0 + soMau*kNHI_KENH;
   duLieuAmThanh.lan2 = duLieuAmThanh.lan1 + soMau*kNHI_KENH;
   
   
   printf( "main: %d  soMau*kNHI_KENH*sizeof( float ) %ld\n", soMau, soMau*kNHI_KENH*sizeof( float ) );

   if( duLieuAmThanh.mangMau != NULL ) {

      DieuCheTanSo_float goGo;    // gõ gỗ - chưa xong
      goGo.giaiThuat = 4;
      goGo.phaPhanHoi = 0;
      goGo.tongAmLuong = 1.0f;
//0.048 280 0.048 280 0.249 128 0.249 128   freq 552.251 (840384)  550.485 (837696)  184.084 (280128)  183.495 (279232)
      goGo.amLuong.m1 = 0.04700f*1.0f;
      goGo.amLuong.m2 = 0.04456f*1.0f;
      goGo.amLuong.c1 = 0.24927f*1.0f;
      goGo.amLuong.c2 = 0.24927f*1.0f;
      goGo.amLuong.bienDo_m1 = 0.00281f - 0.04700f;  //
      goGo.amLuong.bienDo_m2 = 0.00281f - 0.04700f;
      goGo.amLuong.bienDo_c1 = 0.07910f - 0.24927f;
      goGo.amLuong.bienDo_c2 = 0.07910f - 0.24927f;
      goGo.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      goGo.amLuong.tanSoChinh = 4.0f;

      goGo.tanSo.m1 = 552.251f*kHAI_PI;
      goGo.tanSo.m2 = 550.485f*kHAI_PI;
      goGo.tanSo.c1 = 184.084f*kHAI_PI;
      goGo.tanSo.c2 = 183.495f*kHAI_PI;
      goGo.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;

      giaiThuat4( duLieuAmThanh.mangMau, kTAN_SO_MAU_FLOAT, soMau, &goGo, 8.0f );
      donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, goGo.tongAmLuong );
      luuAmThanhWAV_float( "GoGo.wav", &duLieuAmThanh );
      exit(0);
      
      // ----
      DieuCheTanSo_float chuong;    // chuông
      chuong.giaiThuat = 4;
      chuong.tongAmLuong = 0.4f;
      chuong.phaPhanHoi = 0.0f;
      
      chuong.amLuong.m1 = 0.488f;
      chuong.amLuong.m2 = 0.488f;
      chuong.amLuong.c1 = 0.711f;
      chuong.amLuong.c2 = 0.711f;
      
      chuong.tanSo.m1 = 1543.073f*kHAI_PI;
      chuong.tanSo.m2 = 1532.643f*kHAI_PI;
      chuong.tanSo.c1 = 218.108f*kHAI_PI;
      chuong.tanSo.c2 = 218.949f*kHAI_PI;
      chuong.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;

      // ----
      
//      giaiThuat4( &duLieuAmThanh, kTAN_SO_MAU_FLOAT, soMau, &goGo, 8.0f );
      
      // ----
      DieuCheTanSo_float danh;    // đánh
      danh.giaiThuat = 7;
      danh.tongAmLuong = 1.0f;
      danh.phaPhanHoi = 0.0f;
      
      danh.amLuong.m1 = 0.808f;
      danh.amLuong.m2 = 0.808f;
      danh.amLuong.c1 = 0.808f;
      danh.amLuong.c2 = 0.808f;
      
      danh.tanSo.m1 = 2437.963f*kHAI_PI;
      danh.tanSo.m2 = 2437.963f*kHAI_PI;
      danh.tanSo.c1 = 2437.963f*kHAI_PI;
      danh.tanSo.c2 = 1218.982f*kHAI_PI;
      // ---- cần đổi ngẫu nhiên sau mỗi 64*48000/44100 mẫu
      danh.tanSo.bienDo_m1 = 1030.230f*kHAI_PI;
      danh.tanSo.bienDo_m2 = 1030.230f*kHAI_PI;
      danh.tanSo.bienDo_c1 = 1030.230f*kHAI_PI;
      danh.tanSo.bienDo_c2 = 515.114f*kHAI_PI;
      danh.tanSo.loaiChinh = kCHINH__LOAI_HUYEN_NAO;
      danh.tanSo.nhipChinh = kCHINH__NHIP_64_MAU;
      donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, danh.tongAmLuong );
      giaiThuat7( duLieuAmThanh.mangMau, kTAN_SO_MAU_FLOAT, soMau, &danh, 5.0f );

//      luuAmThanhWAV_float( "Danh.wav", &duLieuAmThanh );
      
      // ----
      DieuCheTanSo_float ren;    // rên
      ren.giaiThuat = 7;
      ren.tongAmLuong = 1.0f;
      ren.phaPhanHoi = 0.0f;
      
      ren.amLuong.m1 = 0.444f;
      ren.amLuong.m2 = 0.840f;
      ren.amLuong.c1 = 0.515f;
      ren.amLuong.c2 = 0.823f;
      
      ren.tanSo.m1 = 2547.648f*kHAI_PI;
      ren.tanSo.m2 = 1147.148f*kHAI_PI;
      ren.tanSo.c1 = 9736.373f*kHAI_PI;
      ren.tanSo.c2 = 811.365f*kHAI_PI;
      ren.tanSo.loaiChinh = kCHINH__LOAI_HUYEN_NAO;
      ren.tanSo.nhipChinh = kCHINH__NHIP_8_MAU;
 
      // ---- cần đổi ngẫu nhiên sau mỗi 64*48000/44100 mẫu
      //   phạm vi: 1458,875	657,099	5575,754	464,646
      //            3636,420	1637,196 13896,991	1158,083
      
      ren.tanSo.bienDo_m1 = 1088.773f*kHAI_PI;
      ren.tanSo.bienDo_m2 = 490.049f*kHAI_PI;
      ren.tanSo.bienDo_c1 = 4160.619f*kHAI_PI;
      ren.tanSo.bienDo_c2 = 346.719f*kHAI_PI;
//      donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, ren.tongAmLuong );
//      giaiThuat7(duLieuAmThanh.mangMau, kTAN_SO_MAU_FLOAT, soMau, &ren, 8.0f );
      
      // ----
      DieuCheTanSo_float chuong2;    // chuông 2
      chuong2.giaiThuat = 3;
      chuong2.tongAmLuong = 0.4f;
      chuong2.phaPhanHoi = 0.0;
      
      chuong2.amLuong.m1 = 0.682f;
      chuong2.amLuong.m2 = 0.750f;
      chuong2.amLuong.c1 = 0.573f;
      chuong2.amLuong.c2 = 0.834f;
      
      chuong2.tanSo.m1 = 327.793f*kHAI_PI;
      chuong2.tanSo.m2 = 27.316f*kHAI_PI;
      chuong2.tanSo.c1 = 273.160f*kHAI_PI;
      chuong2.tanSo.c2 = 54.632f*kHAI_PI;
      chuong2.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;

      // ----
      DieuCheTanSo_float day0;    // dây 0
      day0.giaiThuat = 3;
      day0.tongAmLuong = 1.0f;
      day0.phaPhanHoi = 0.0;
      
      day0.amLuong.m1 = 0.01196f;
      day0.amLuong.m2 = 0.12463f;
      day0.amLuong.c1 = 0.12463f;
      day0.amLuong.c2 = 0.64648f;
      day0.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      day0.amLuong.tanSoChinh = 4.0f;  // Hz
      day0.amLuong.bienDo_m1 = 0.00061f - 0.01196f;
      day0.amLuong.bienDo_m2 = 0.09607f - 0.12463f;
      day0.amLuong.bienDo_c1 = 0.04407f - 0.12463f;
      day0.amLuong.bienDo_c2 = 0.49854f - 0.64648f;
      
      day0.tanSo.m1 = 1158.882f*kHAI_PI;
      day0.tanSo.m2 = 38.629f*kHAI_PI;
      day0.tanSo.c1 = 77.259f*kHAI_PI;
      day0.tanSo.c2 = 38.629f*kHAI_PI;
      day0.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
 //     donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, day0.tongAmLuong );
//      giaiThuat3( duLieuAmThanh.mangMau, kTAN_SO_MAU_FLOAT, soMau, &day0, 8.0f );
      
      // ----
      DieuCheTanSo_float ghen0;    // ghẹn 0
      ghen0.giaiThuat = 5;
      ghen0.tongAmLuong = 1.0f;
      ghen0.phaPhanHoi = 0.0;
      
      ghen0.amLuong.m1 = 8.385f;
      ghen0.amLuong.m2 = 0.761f;
      ghen0.amLuong.c1 = 0.799f;
      ghen0.amLuong.c2 = 0.781f;
      
      ghen0.tanSo.m1 = 45.926f*kHAI_PI;
      ghen0.tanSo.m2 = 45.926f*kHAI_PI;
      ghen0.tanSo.c1 = 91.853f*kHAI_PI;
      ghen0.tanSo.c2 = 45.926f*kHAI_PI;
      ghen0.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//    donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, ghen0.tongAmLuong );
//      giaiThuat5( duLieuAmThanh.mangMau, kTAN_SO_MAU_FLOAT, soMau, &ghen0, 8.0f );
//      luuAmThanhWAV_float( "Chuong4f.wav", &duLieuAmThanh );

      // ----
      DieuCheTanSo_float day1;    // dây 1
      day1.giaiThuat = 4;
      day1.tongAmLuong = 1.0f;
      day1.phaPhanHoi = 0.0f;//3.14159f*0.5f;
      
      day1.amLuong.m1 = 0.613f*64.0f;  // --> 0.498  qua 3,4 giây
      day1.amLuong.m2 = 0.895f*1.0f;  // --> 0.721
      day1.amLuong.c1 = 0.615f*1.0f;  // --> 0.515
      day1.amLuong.c2 = 0.846f*1.0f;  // --> 0.798
      
      day1.tanSo.m1 = 38.608f*kHAI_PI;
      day1.tanSo.m2 = 38.650f*kHAI_PI;
      day1.tanSo.c1 = 38.692f*kHAI_PI;
      day1.tanSo.c2 = 38.524f*kHAI_PI;
      day1.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, day1.tongAmLuong );
//      giaiThuat4( duLieuAmThanh.mangMau, kTAN_SO_MAU_FLOAT, soMau, &day1, 8.0f );
      
      // ----
      DieuCheTanSo_float day2;    // dây 2
      day2.giaiThuat = 0;
   
      day2.tongAmLuong = 1.0f;
      day2.phaPhanHoi = 0.0f;//3.14159f*0.5f;
      
      day2.amLuong.m1 = 0.672f*2.0f;  // --> 0.671   qua 1 giây
      day2.amLuong.m2 = 0.752f*2.0f;  // --> 0.747
      day2.amLuong.c1 = 0.717f*2.0f;  // --> 0.747
      day2.amLuong.c2 = 0.648f*2.0f;  // --> 0.737
      
      day2.tanSo.m1 = 330.295f*kHAI_PI;
      day2.tanSo.m2 = 330.652f*kHAI_PI;
      day2.tanSo.c1 = 330.842f*kHAI_PI;
      day2.tanSo.c2 = 330.484f*kHAI_PI;
      day2.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat0( duLieuAmThanh.lan0, kTAN_SO_MAU_FLOAT, soMau, &day2, 8.0f );
      
      // ----
      day2.amLuong.m1 = 0.672f*2.0f;  // --> 0.671   qua 1 giây
      day2.amLuong.m2 = 0.752f*2.0f;  // --> 0.747
      day2.amLuong.c1 = 0.717f*2.0f;  // --> 0.747
      day2.amLuong.c2 = 0.648f*2.0f;  // --> 0.737
      
      day2.tanSo.m1 = 413.883f*kHAI_PI;
      day2.tanSo.m2 = 414.283f*kHAI_PI;
      day2.tanSo.c1 = 414.472f*kHAI_PI;
      day2.tanSo.c2 = 414.073f*kHAI_PI;
//      giaiThuat0( duLieuAmThanh.lanđ, kTAN_SO_MAU_FLOAT, soMau, &day2, 8.0f );
//    donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, day2.tongAmLuong );

      // ---- lưu
//      luuAmThanhWAV_float( "day2c.wav", &duLieuAmThanh );
      
      
      DieuCheTanSo_float tron0;    // trốn 0
      tron0.giaiThuat = 6;
      duLieuAmThanh.soLuongLan = 1;
      
      tron0.tongAmLuong = 1.0f;
      tron0.phaPhanHoi = 0.0f;
      
      tron0.amLuong.m1 = 0.998f*32.0f;
      tron0.amLuong.m2 = 0.826f*32.0f;
      tron0.amLuong.c1 = 0.828f*32.0f;
      tron0.amLuong.c2 = 0.826f*32.0f;
      
      tron0.tanSo.m1 = 105.206f*kHAI_PI;
      tron0.tanSo.m2 = 105.164f*kHAI_PI;
      tron0.tanSo.c1 = 105.016f*kHAI_PI;
      tron0.tanSo.c2 = 105.311f*kHAI_PI;
      tron0.tanSo.loaiChinh = kCHINH__LOAI_TAM_GIAC;
      tron0.tanSo.tanSoChinh = 8;
      
      tron0.tanSo.bienDo_m1 = 18.4f*kHAI_PI;
      tron0.tanSo.bienDo_m2 = 18.4f*kHAI_PI;
      tron0.tanSo.bienDo_c1 = 18.4f*kHAI_PI;
      tron0.tanSo.bienDo_c2 = 18.4f*kHAI_PI;
      
//      giaiThuat6( duLieuAmThanh.lan0, kTAN_SO_MAU_FLOAT, soMau, &tron0, 8.0f );
//      donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, tron0.tongAmLuong );
//      luuAmThanhWAV_float( "tron0_200.wav", &duLieuAmThanh );
      
      // ----
      DieuCheTanSo_float day3;    // dây 3
      day3.giaiThuat = 0;
      day3.tongAmLuong = 1.0f;
      day3.phaPhanHoi = 0.0f;
      
      day3.amLuong.m1 = 0.679f*8.0f;
      day3.amLuong.m2 = 0.759f*8.0f;
      day3.amLuong.c1 = 0.743f*8.0f;
      day3.amLuong.c2 = 0.810f*8.0f;
      
      day3.tanSo.m1 = 239.452f*kHAI_PI;
      day3.tanSo.m2 = 239.220f*kHAI_PI;
      day3.tanSo.c1 = 239.136f*kHAI_PI;
      day3.tanSo.c2 = 239.305f*kHAI_PI;
      day3.tanSo.loaiChinh = kCHINH__LOAI_PARABOL;
      day3.tanSo.tanSoChinh = 1.0f;
      
      day3.tanSo.bienDo_m1 = -181.224f*kHAI_PI;
      day3.tanSo.bienDo_m2 = -181.097f*kHAI_PI;
      day3.tanSo.bienDo_c1 = -181.055*kHAI_PI;
      day3.tanSo.bienDo_c2 = -181.14f*kHAI_PI;

//      giaiThuat0( duLieuAmThanh.lan0, kTAN_SO_MAU_FLOAT, soMau, &day3, day3. );
//      donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, day3.tongAmLuong );
//      duLieuAmThanh.soLuongMau = 1.0f*kTAN_SO_MAU_FLOAT;
//      luuAmThanhWAV_float( "day3z_8.wav", &duLieuAmThanh );

      // ----
      DieuCheTanSo_float day4;    // dây 3
      day4.giaiThuat = 2;
      
      day4.tongAmLuong = 1.0f;
      
      day4.amLuong.m1 = 0.638f*8.0f;
      day4.amLuong.m2 = 1.000f*8.0f;
      day4.amLuong.c1 = 0.220f*8.0f;
      day4.amLuong.c2 = 0.704f*8.0f;
      
      float nhanTanSo = 2.0f;
      day4.tanSo.m1 = 929.040f*kHAI_PI*nhanTanSo;
      day4.tanSo.m2 = 232.575f*kHAI_PI*nhanTanSo;
      day4.tanSo.c1 = 1860.603f*kHAI_PI*nhanTanSo;
      day4.tanSo.c2 = 929.040f*kHAI_PI*nhanTanSo;
      
      day4.phaPhanHoi = 0.0f;
      day4.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
//      giaiThuat2( duLieuAmThanh.lan0, kTAN_SO_MAU, soMau, &day4, 8.0f );
      day4.tanSo.m1 = 737.344f*kHAI_PI*nhanTanSo;
      day4.tanSo.m2 = 184.609f*kHAI_PI*nhanTanSo;
      day4.tanSo.c1 = 1476.875f*kHAI_PI*nhanTanSo;
      day4.tanSo.c2 = 737.344f*kHAI_PI*nhanTanSo;
//      giaiThuat2( duLieuAmThanh.lan1, kTAN_SO_MAU_FLOAT, soMau, &day4, 8.0f );
      
 //     duLieuAmThanh.soLuongLan = 2;
 //     phaTronHetLanVaoLan0( &duLieuAmThanh );
 //     luuAmThanhWAV_float( "day4d.wav", &duLieuAmThanh );
   
      
      // ----
      DieuCheTanSo_float ghen1;    // ghẹn 1
      ghen1.giaiThuat = 4;
      ghen1.phaPhanHoi = 0.125; // 13
      ghen1.tongAmLuong = 0.4f;
      
      ghen1.amLuong.m1 = 0.584f*4.0f;
      ghen1.amLuong.m2 = 0.584f*4.0f;
      ghen1.amLuong.c1 = 0.653f*4.0f;
      ghen1.amLuong.c2 = 0.600f*4.0f;
      
      nhanTanSo = 0.5f;
      ghen1.tanSo.m1 = 195.145f*kHAI_PI*nhanTanSo;
      ghen1.tanSo.m2 = 97.236f*kHAI_PI*nhanTanSo;
      ghen1.tanSo.c1 = 97.488f*kHAI_PI*nhanTanSo;
      ghen1.tanSo.c2 = 48.681f*kHAI_PI*nhanTanSo;

      ghen1.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
 //     giaiThuat4( duLieuAmThanh.lan0, kTAN_SO_MAU_FLOAT, soMau, &ghen1, 8.0f );
 //     donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH );
 //     luuAmThanhWAV_float( "ghen_1f.wav", &duLieuAmThanh );
      

      // ----
      DieuCheTanSo_float day5;    // dây 3
      day5.giaiThuat = 4;
      day5.tongAmLuong = 1.0f;
      day5.phaPhanHoi = 0.0625f;
      
      nhanTanSo = 2.0f;
      day5.amLuong.m1 = 0.660f*4.0f;
      day5.amLuong.m2 = 0.716f*4.0f;
      day5.amLuong.c1 = 0.573f*4.0f;
      day5.amLuong.c2 = 0.627f*4.0f;
      day5.tanSo.m1 = 1869.477f*kHAI_PI*nhanTanSo;
      day5.tanSo.m2 = 622.528f*kHAI_PI*nhanTanSo;
      day5.tanSo.c1 = 1245.057f*kHAI_PI*nhanTanSo;
      day5.tanSo.c2 = 622.907f*kHAI_PI*nhanTanSo;
      day5.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
   
      giaiThuat4( duLieuAmThanh.lan0, kTAN_SO_MAU_FLOAT, soMau, &day5, 8.0f );
   
      day5.amLuong.m1 = 0.660f*4.0f;
      day5.amLuong.m2 = 0.716f*4.0f;
      day5.amLuong.c1 = 0.682f*4.0f;
      day5.amLuong.c2 = 0.739f*4.0f;
      day5.tanSo.m1 = 2222.630f*kHAI_PI*nhanTanSo;
      day5.tanSo.m2 = 740.120f*kHAI_PI*nhanTanSo;
      day5.tanSo.c1 = 1480.239f*kHAI_PI*nhanTanSo;
      day5.tanSo.c2 = 740.498f*kHAI_PI*nhanTanSo;
      giaiThuat4( duLieuAmThanh.lan1, kTAN_SO_MAU_FLOAT, soMau, &day5, 8.0f );
   
      day5.amLuong.m1 = 0.660f*4.0f;
      day5.amLuong.m2 = 0.716f*4.0f;
      day5.amLuong.c1 = 0.693f*4.0f;
      day5.amLuong.c2 = 0.749f*4.0f;
      day5.tanSo.m1 = 2801.377f*kHAI_PI*nhanTanSo;
      day5.tanSo.m2 = 933.035f*kHAI_PI*nhanTanSo;
      day5.tanSo.c1 = 1866.071f*kHAI_PI*nhanTanSo;
      day5.tanSo.c2 = 933.414f*kHAI_PI*nhanTanSo;
      giaiThuat4( duLieuAmThanh.lan2, kTAN_SO_MAU_FLOAT, soMau, &day5, 8.0f );
      
//      duLieuAmThanh.soLuongLan = 3;
 //     donViHoaLanFloat( duLieuAmThanh.lan0, soMau * kNHI_KENH, day5.tongAmLuong );
//      luuAmThanhWAV_float( "day5h.wav", &duLieuAmThanh );
      /*
      float viTri = 0.0f;
      while( viTri < 10.0f ) {
         float ketQua = dayParabol( viTri, 1.0f, 8.0f, -1.0f, 5.0f );
         printf( "%5.3f: %5.3f\n", viTri, ketQua );
         viTri += 0.01f;
      } */
      DieuCheTanSo_float tron1;    // trốn 0
      tron0.giaiThuat = 2;
      tron1.tongAmLuong = 1.0f;
      tron1.phaPhanHoi = 0.0f;
      
      tron1.amLuong.m1 = 0.646f*32.0f;
      tron1.amLuong.m2 = 0.190f*32.0f;
      tron1.amLuong.c1 = 0.034f*32.0f;
      tron1.amLuong.c2 = 0.323f*32.0f;
      
      tron1.tanSo.m1 = 51.856f*kHAI_PI;
      tron1.tanSo.m2 = 103.881f*kHAI_PI;
      tron1.tanSo.c1 = 155.695f*kHAI_PI;
      tron1.tanSo.c2 = 51.856f*kHAI_PI;

      tron1.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat2( duLieuAmThanh.lan0, kTAN_SO_MAU_FLOAT, soMau, &tron1, 2.0f );
//      duLieuAmThanh.soLuongMau = 2.0f*kTAN_SO_MAU_FLOAT;
//      donViHoaLanFloat( duLieuAmThanh.lan0, duLieuAmThanh.soLuongMau, tron1.tongAmLuong );
      
      DieuCheTanSo_float tron2;    // trốn 0
      tron0.giaiThuat = 2;
      tron2.tongAmLuong = 1.0f;
      tron2.phaPhanHoi = 0.0f;
      
      tron2.amLuong.m1 = 0.646f*16.0f;
      tron2.amLuong.m2 = 0.986f*16.0f;
      tron2.amLuong.c1 = 0.034f*16.0f;
      tron2.amLuong.c2 = 0.323f*16.0f;
      
      tron2.tanSo.m1 = 51.856f*kHAI_PI;
      tron2.tanSo.m2 = 103.881f*kHAI_PI;
      tron2.tanSo.c1 = 155.695f*kHAI_PI;
      tron2.tanSo.c2 = 51.856f*kHAI_PI;
      
      tron2.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat2( duLieuAmThanh.lan0, kTAN_SO_MAU_FLOAT, soMau, &tron2, 2.0f );
//      duLieuAmThanh.soLuongMau = 2.0f*kTAN_SO_MAU_FLOAT;
//      donViHoaLanFloat( duLieuAmThanh.lan0, duLieuAmThanh.soLuongMau, tron2.tongAmLuong );
      
      
      // ----
      duLieuAmThanh.soLuongMau = 2.0f*kTAN_SO_MAU_SHORT;
      DieuCheTanSo_short chuong5;    // chuông 5
      chuong5.giaiThuat = 4;
      
      chuong5.tongAmLuong = 1.0f;
      chuong5.phanHoi = 0;

      chuong5.amLuong.m1 = 0.01428f;
      chuong5.amLuong.m2 = 0.01428f;
      chuong5.amLuong.c1 = 0.07654f;
      chuong5.amLuong.c2 = 0.07654f;
      chuong5.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      chuong5.amLuong.bienDo_m1 = 0.0f;
      chuong5.amLuong.bienDo_m2 = 0.0f;
      chuong5.amLuong.bienDo_c1 = 0.13013f - 0.0765;
      chuong5.amLuong.bienDo_c2 = 0.13013f - 0.0765;
      
      chuong5.tanSo.m1 = 1468.632;
      chuong5.tanSo.m2 = 1471.996;
      chuong5.tanSo.c1 = 734.316;
      chuong5.tanSo.c2 = 735.998;
      chuong5.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &chuong5, 2.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "chuong5c.wav", &duLieuAmThanh );
      
      DieuCheTanSo_short chuong6;    // chuông 6
      chuong6.giaiThuat = 4;
      
      chuong6.tongAmLuong = 1.0f;
      chuong6.phanHoi = 0;

      chuong6.amLuong.m1 = 0.01550f;
      chuong6.amLuong.m2 = 0.01550f;
      chuong6.amLuong.c1 = 0.12878f;
      chuong6.amLuong.c2 = 0.12878f;
      chuong6.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
   
      chuong6.tanSo.m1 = 1038.304;
      chuong6.tanSo.m2 = 1040.996;
      chuong6.tanSo.c1 = 519.152;
      chuong6.tanSo.c2 = 520.498;
      chuong6.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      //      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &chuong6, 2.0f );
      //      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
      //      luuAmThanhWAV_short( "chuong5c.wav", &duLieuAmThanh );
      exit(0);


      luuAmThanhWAV_float( "chuong5a.wav", &duLieuAmThanh );
   }

   return 1;
}

void giaiThuat0( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   
   float m1 = 0.0f;  // rad/giây
   float m2 = 0.0f;  // rad/giây
   float c1 = 0.0f;  // rad/giây
   float c2 = 0.0f;  // rad/giây
   
   float tongAmLuong = dan->tongAmLuong;
   
   float amLuong_m1 = dan->amLuong.m1;
   float amLuong_m2 = dan->amLuong.m2;
   float amLuong_c1 = dan->amLuong.c1;
   float amLuong_c2 = dan->amLuong.c2;
   
   float tanSo_m1 = dan->tanSo.m1*nhipThoiGian;  // rad/giây
   float tanSo_m2 = dan->tanSo.m2*nhipThoiGian;  // rad/giây
   float tanSo_c1 = dan->tanSo.c1*nhipThoiGian;  // rad/giây
   float tanSo_c2 = dan->tanSo.c2*nhipThoiGian;  // rad/giây
   
   float pha_m1 = 0.0f;  // rad/giây
   float pha_m2 = 0.0f;  // rad/giây
   float pha_c1 = 0.0f;  // rad/giây
   float pha_c2 = 0.0f;  // rad/giây
   
   float phaPhanHoi = dan->phaPhanHoi;
   float phanHoi_truoc = 0.0f;
   float phanHoi_hienTai = 0.0f;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = expf( -thoiGian );
      
      // ---- tính phản hội
      float phanHoi = 0.0f;
      if( phaPhanHoi != 0.0f ) {
         phanHoi = phanHoi_truoc + phanHoi_hienTai;
         //      printf( "%5.3f (s)  phanHoi %5.3f = phanHoi_tr %5.3f + phanHoi_ht %5.3f\n", thoiGian, phanHoi, phanHoi_truoc, phanHoi_hienTai );
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi );
         c1 = phanHoi_hienTai;
      }
      else {
         phanHoi_truoc = phanHoi_hienTai;
         c1 = amLuong_m1*sinf( pha_m1 );
      }
      
      
      float boNho = amLuong_c1*sinf( pha_c1 + c1 );
      //      printf( "m2: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f pha %5.3f\n", m1, m2, c1, c2, pha_m2 );
      //      printf( "c1: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f\n", m1, m2, c1, c2, pha_c1 );

      c2 = amLuong_m2*sinf( pha_m2 + m2 );
      
      
      float mau = tongAm*amLuong_c2*sinf( pha_c2 + c2 );
      
      m2 = boNho;
      
      //      printf( "m1: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f - mau %5.3f\n", m1, m2, c1, c2, pha_m1, mau );
      
      if( chiSoMau < soMau ) {
         *demAm += mau;
         demAm++;
         *demAm += mau;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo_float( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo_float( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
   }
   
}


void giaiThuat2( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   
   float m1 = 0.0f;  // rad/giây
   float m2 = 0.0f;  // rad/giây
   float c1 = 0.0f;  // rad/giây
   float c2 = 0.0f;  // rad/giây
   
   float tongAmLuong = dan->tongAmLuong;
   
   float amLuong_m1 = dan->amLuong.m1;
   float amLuong_m2 = dan->amLuong.m2;
   float amLuong_c1 = dan->amLuong.c1;
   float amLuong_c2 = dan->amLuong.c2;
   
   float tanSo_m1 = dan->tanSo.m1*nhipThoiGian;  // rad/giây
   float tanSo_m2 = dan->tanSo.m2*nhipThoiGian;  // rad/giây
   float tanSo_c1 = dan->tanSo.c1*nhipThoiGian;  // rad/giây
   float tanSo_c2 = dan->tanSo.c2*nhipThoiGian;  // rad/giây
   
   float pha_m1 = 0.0f;  // rad/giây
   float pha_m2 = 0.0f;  // rad/giây
   float pha_c1 = 0.0f;  // rad/giây
   float pha_c2 = 0.0f;  // rad/giây
   
   float phaPhanHoi = dan->phaPhanHoi;
   float phanHoi_truoc = 0.0f;
   float phanHoi_hienTai = 0.0f;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;

   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tongAmLuong*expf( -thoiGian*4.0f );
      
      // ---- tính phản hội
      float phanHoi = 0.0f;
      if( phaPhanHoi != 0.0f ) {
         phanHoi = phanHoi_truoc + phanHoi_hienTai;
         //      printf( "%5.3f (s)  phanHoi %5.3f = phanHoi_tr %5.3f + phanHoi_ht %5.3f\n", thoiGian, phanHoi, phanHoi_truoc, phanHoi_hienTai );
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi );
      }
      else {
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 );
      }
      
//      printf( "  m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f pha %5.3f  phanHoi_truoc %5.3f\n", m1, m2, c1, c2, pha_m2, phanHoi_truoc );
      
      c2 = amLuong_m2*sinf( pha_m2 + m2 ) + phanHoi_truoc;
      
      float boNho = amLuong_c1*sinf( pha_c1 + c1 );
      float mau = amLuong_c2*sinf( pha_c2 + c2 );
      mau *= tongAm;
      
//      printf( "  c2: m1(boNho) %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f - mau %5.3f  phanHoi_hienTai %5.3f\n", boNho, m2, c1, c2, pha_m1, mau, phanHoi_hienTai );
//      printf( "    pha_m1 %5.3f  pha_m2 %5.3f  pha_c1 %5.3f  pha_c2 %5.3f - mau %5.3f\n",  pha_m1, pha_m2, pha_c1, pha_c2, mau );
      
      m2 = boNho;

      if( chiSoMau < soMau ) {
         *demAm += mau;
         demAm++;
         *demAm += mau;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo_float( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo_float( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau > 100 )
//         exit(0);
   }
   
}

/*
  m1:  env_m1•sin( pha_m1 + m1) --> c1  
  m2:  env_m2•sin( pha_m2 + m2) --> c2
  
  c1:  env_c1•sin( pha_c1 + c1) --> ra
  c2:  env_c2•sin( pha_c2 + c2) --> += ra
*/

void giaiThuat3( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   
   float m1 = 0.0f;  // rad/giây
   float m2 = 0.0f;  // rad/giây
   float c1 = 0.0f;  // rad/giây
   float c2 = 0.0f;  // rad/giây
   
   float tongAmLuong = dan->tongAmLuong;
   
   float amLuong_m1 = dan->amLuong.m1;
   float amLuong_m2 = dan->amLuong.m2;
   float amLuong_c1 = dan->amLuong.c1;
   float amLuong_c2 = dan->amLuong.c2;
   
   float tanSo_m1 = dan->tanSo.m1*nhipThoiGian;  // rad/giây
   float tanSo_m2 = dan->tanSo.m2*nhipThoiGian;  // rad/giây
   float tanSo_c1 = dan->tanSo.c1*nhipThoiGian;  // rad/giây
   float tanSo_c2 = dan->tanSo.c2*nhipThoiGian;  // rad/giây
   
   float pha_m1 = 0.0f;  // rad/giây
   float pha_m2 = 0.0f;  // rad/giây
   float pha_c1 = 0.0f;  // rad/giây
   float pha_c2 = 0.0f;  // rad/giây
   
   float phaPhanHoi = dan->phaPhanHoi;
   float phanHoi_truoc = 0.0f;
   float phanHoi_hienTai = 0.0f;
   
   float boNho = 0.0f;

   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tongAmLuong*expf( -thoiGian );
      
      // ---- tính phản hội
      float phanHoi = 0.0f;
      if( phaPhanHoi != 0.0f ) {
         phanHoi = phanHoi_truoc + phanHoi_hienTai;
         //      printf( "%5.3f (s)  phanHoi %5.3f = phanHoi_tr %5.3f + phanHoi_ht %5.3f\n", thoiGian, phanHoi, phanHoi_truoc, phanHoi_hienTai );
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi );
      }
      else      
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 );
      
      m2 = boNho;

//      printf( "m2: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f mem %8.5f  pha %5.3f\n", m1, m2, c1, c2, boNho, pha_m2 );
      
      c1 = phanHoi_hienTai;

      // ---- chứa lại
      boNho = amLuong_c1*sinf( pha_c1 + c1 );
 //     printf( "c1: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f mem %8.5f  pha %5.3f\n", m1, m2, c1, c2, boNho, pha_c1 );

      c2 = amLuong_m2*sinf( pha_m2 + m2 );


      m1 = boNho;
 //     printf( "m1: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f mem %8.5f  pha %5.3f\n", m1, m2, c1, c2, boNho, pha_m1 );

      float mau = tongAm*amLuong_c2*sinf( pha_c2 + c2 );
 //     printf( "mau: %5.3f\n", mau );
      
      if( chiSoMau < soMau ) {
         *demAm += mau;
         demAm++;
         *demAm += mau;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo_float( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo_float( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
 //    if( chiSoMau == 10 )
 //        exit(0);
   }
   
}

void giaiThuat4( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây

   float m1 = 0.0f;  // rad/giây
   float m2 = 0.0f;  // rad/giây
   float c1 = 0.0f;  // rad/giây
   float c2 = 0.0f;  // rad/giây
   
   float tongAmLuong = dan->tongAmLuong;
   
   float amLuong_m1 = dan->amLuong.m1;
   float amLuong_m2 = dan->amLuong.m2;
   float amLuong_c1 = dan->amLuong.c1;
   float amLuong_c2 = dan->amLuong.c2;

   float tanSo_m1 = dan->tanSo.m1*nhipThoiGian;  // rad/giây
   float tanSo_m2 = dan->tanSo.m2*nhipThoiGian;  // rad/giây
   float tanSo_c1 = dan->tanSo.c1*nhipThoiGian;  // rad/giây
   float tanSo_c2 = dan->tanSo.c2*nhipThoiGian;  // rad/giây
   
   float pha_m1 = 0.0f;  // rad/giây
   float pha_m2 = 0.0f;  // rad/giây
   float pha_c1 = 0.0f;  // rad/giây
   float pha_c2 = 0.0f;  // rad/giây
   
   float phaPhanHoi = dan->phaPhanHoi;
   float phanHoi_truoc = 0.0f;
   float phanHoi_hienTai = 0.0f;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh

   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng 
      float tongAm = tongAmLuong*expf( -thoiGian );

      // ---- tính phản hội
      float phanHoi = 0.0f;
      if( phaPhanHoi != 0.0f ) {
         phanHoi = phanHoi_truoc + phanHoi_hienTai;
//      printf( "%5.3f (s)  phanHoi %5.3f = phanHoi_tr %5.3f + phanHoi_ht %5.3f\n", thoiGian, phanHoi, phanHoi_truoc, phanHoi_hienTai );
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi );
      }
      else
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 );
      
      c1 = phanHoi_hienTai;
//      printf( "m2: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f pha %5.3f\n", m1, m2, c1, c2, pha_m2 );
//      printf( "c1: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f\n", m1, m2, c1, c2, pha_c1 );

      c2 = amLuong_m2*sinf( pha_m2 );


      float mau = tongAm*amLuong_c1*sinf( pha_c1 + c1 );
      mau += tongAm*amLuong_c2*sinf( pha_c2 + c2 );
      
//      printf( "m1: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f - mau %5.3f\n", m1, m2, c1, c2, pha_m1, mau );

      if( chiSoMau < soMau ) {
          *demAm = mau;
          demAm++;
          *demAm = mau;
          demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo_float( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo_float( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );

      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây

      chiSoMau += 2;
      thoiGian += nhipThoiGian;
   }

}

void giaiThuat5( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   
   float m1 = 0.0f;  // rad/giây
   float m2 = 0.0f;  // rad/giây
   float c1 = 0.0f;  // rad/giây
   float c2 = 0.0f;  // rad/giây
   
   float tongAmLuong = dan->tongAmLuong;
   
   float amLuong_m1 = dan->amLuong.m1;
   float amLuong_m2 = dan->amLuong.m2;
   float amLuong_c1 = dan->amLuong.c1;
   float amLuong_c2 = dan->amLuong.c2;
   
   float tanSo_m1 = dan->tanSo.m1*nhipThoiGian;  // rad/giây
   float tanSo_m2 = dan->tanSo.m2*nhipThoiGian;  // rad/giây
   float tanSo_c1 = dan->tanSo.c1*nhipThoiGian;  // rad/giây
   float tanSo_c2 = dan->tanSo.c2*nhipThoiGian;  // rad/giây
   
   float pha_m1 = 0.0f;  // rad/giây
   float pha_m2 = 0.0f;  // rad/giây
   float pha_c1 = 0.0f;  // rad/giây
   float pha_c2 = 0.0f;  // rad/giây
   
   float phaPhanHoi = dan->phaPhanHoi;
   float phanHoi_truoc = 0.0f;
   float phanHoi_hienTai = 0.0f;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   float boNho = 0.0f;
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tongAmLuong*expf( -thoiGian );
      
      float phanHoi = phanHoi_truoc + phanHoi_hienTai;
      phanHoi_truoc = phanHoi_hienTai;
      
      // ---- đặc biệt cho giải thuật 5
      boNho = c1 = c2 = phanHoi_truoc;
      
      // ---- tính phản hội
      phanHoi_hienTai = 0.0f;
      if( phaPhanHoi != 0.0f ) {
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi );
      }
      else {
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 );
      }

      float mau = amLuong_m2*sinf( pha_m2 + m2 );
      mau += amLuong_c1*sinf( pha_c1 + c1 );
      mau += amLuong_c2*sinf( pha_c2 + c2 );
      mau *= tongAm;
      
//      printf( "m1: m1(boNho) %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f - mau %5.3f\n", boNho, m2, c1, c2, pha_m1, mau );
      m2 = boNho;
      
      if( chiSoMau < soMau ) {
         *demAm += mau;
         demAm++;
         *demAm += mau;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo_float( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo_float( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
   }
   
}


void giaiThuat6( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   
   float m1 = 0.0f;  // rad/giây
   float m2 = 0.0f;  // rad/giây
   float c1 = 0.0f;  // rad/giây
   float c2 = 0.0f;  // rad/giây
   
   float tongAmLuong = dan->tongAmLuong;
   
   float amLuong_m1 = dan->amLuong.m1;
   float amLuong_m2 = dan->amLuong.m2;
   float amLuong_c1 = dan->amLuong.c1;
   float amLuong_c2 = dan->amLuong.c2;
   
   float tanSo_m1 = dan->tanSo.m1*nhipThoiGian;  // rad/giây
   float tanSo_m2 = dan->tanSo.m2*nhipThoiGian;  // rad/giây
   float tanSo_c1 = dan->tanSo.c1*nhipThoiGian;  // rad/giây
   float tanSo_c2 = dan->tanSo.c2*nhipThoiGian;  // rad/giây
   
   float pha_m1 = 0.0f;  // rad/giây
   float pha_m2 = 0.0f;  // rad/giây
   float pha_c1 = 0.0f;  // rad/giây
   float pha_c2 = 0.0f;  // rad/giây
   
   float phaPhanHoi = dan->phaPhanHoi;
   float phanHoi_truoc = 0.0f;
   float phanHoi_hienTai = 0.0f;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   float boNho = 0.0f;
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tongAmLuong*expf( -thoiGian );
      
      float phanHoi = phanHoi_truoc + phanHoi_hienTai;
      phanHoi_truoc = phanHoi_hienTai;
      
      // ---- tính phản hội
      phanHoi_hienTai = 0.0f;
      if( phaPhanHoi != 0.0f ) {
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi );
         c1 = phanHoi_hienTai;
      }
      else
         c1 = amLuong_m1*sinf( pha_m1 );
//            printf( "m2: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f pha %5.3f\n", m1, m2, c1, c2, pha_m2 );
//            printf( "c1: m1 %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f\n", m1, m2, c1, c2, pha_c1 );
      
      float mau = amLuong_m2*sinf( pha_m2 );

      mau += amLuong_c1*sinf( pha_c1 + c1 );
      mau += amLuong_c2*sinf( pha_c2 );
      mau *= tongAm;
      
//            printf( "m1: m1(boNho) %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f - mau %5.3f\n", boNho, m2, c1, c2, pha_m1, mau );
      m2 = boNho;
      
      if( chiSoMau < soMau ) {
         *demAm += mau;
         demAm++;
         *demAm += mau;
         demAm++;
      }

      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo_float( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo_float( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau > 10 )
//         exit(0);
   }
   
}


void giaiThuat7( float *demAm, float tocDoMau, unsigned int soMau, DieuCheTanSo_float *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   
   float m1 = 0.0f;  // rad/giây
   float m2 = 0.0f;  // rad/giây
   float c1 = 0.0f;  // rad/giây
   float c2 = 0.0f;  // rad/giây

   float tongAmLuong = dan->tongAmLuong;
   
   float amLuong_m1 = dan->amLuong.m1;
   float amLuong_m2 = dan->amLuong.m2;
   float amLuong_c1 = dan->amLuong.c1;
   float amLuong_c2 = dan->amLuong.c2;
   
   float tanSo_m1 = dan->tanSo.m1*nhipThoiGian;  // rad/giây
   float tanSo_m2 = dan->tanSo.m2*nhipThoiGian;  // rad/giây
   float tanSo_c1 = dan->tanSo.c1*nhipThoiGian;  // rad/giây
   float tanSo_c2 = dan->tanSo.c2*nhipThoiGian;  // rad/giây
   
   float pha_m1 = 0.0f;  // rad/giây
   float pha_m2 = 0.0f;  // rad/giây
   float pha_c1 = 0.0f;  // rad/giây
   float pha_c2 = 0.0f;  // rad/giây
   
   float phaPhanHoi = dan->phaPhanHoi;
   float phanHoi_truoc = 0.0f;
   float phanHoi_hienTai = 0.0f;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tongAmLuong*expf( -thoiGian );
      
      // ---- tính phản hội
      float phanHoi = 0.0f;
      if( phaPhanHoi != 0.0f ) {
         phanHoi = phanHoi_truoc + phanHoi_hienTai;
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi*0.125f );
         c1 = phanHoi_hienTai;
      }
      else
         c1 = amLuong_m1*sinf( pha_m1 + phanHoi*phaPhanHoi*0.125f );
      
      float mau = amLuong_m1*sinf( pha_m1 );
      mau += amLuong_m2*sinf( pha_m2 );
      mau += amLuong_c1*sinf( pha_c1 );
      mau += amLuong_c2*sinf( pha_c2 );
      mau *= tongAm;
      
      if( chiSoMau < soMau ) {
         *demAm += mau;
         demAm++;
         *demAm += mau;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo_float( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo_float( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );

      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
   }
}

#pragma mark ---- Nâng Cấp Hệ số
void chinhHeSo_float( HeSoBo_float *bo, unsigned char loaiChinh, unsigned int soMau, float thoiGian,
               float *giaTri_m1, float *giaTri_m2, float *giaTri_c1, float *giaTri_c2 ) {
   
   float bienDo = 0.0f;
   
   
   float giaTriTinh = thoiGian*bo->tanSoChinh;
   
   if( loaiChinh == kCHINH__LOAI_DOC ) {
      
      bienDo = doc( giaTriTinh, 0.0f, 1.0f, 0.0f, 1.0f );
      
   }
   else if( loaiChinh == kCHINH__LOAI_TAM_GIAC ) {
      
      bienDo = songTamGiacf( giaTriTinh );
      
   }
   else if( loaiChinh == kCHINH__LOAI_HUYEN_NAO ) {
      unsigned short nhipChinh = bo->nhipChinh;
      
      // ---- xem tới thời gian đổi chưa
      if( (soMau & nhipChinh) == 0 ) {
         
         bienDo = (float)(rand() & 0x3ff)/512.0f - 1.0f;
         
      }
      else  // không có gì để làm
         return;
   }
   else if( loaiChinh == kCHINH__LOAI_PARABOL ) {
      
      bienDo = dayParabol( giaTriTinh, 0.0f, 1.0f, 0.0f, 1.0f );
      
   }
   else  // không có gì để làm
      return;
   
   // ---- nâng cấp
   *giaTri_m1 = bo->m1 + bo->bienDo_m1*bienDo;
   *giaTri_m2 = bo->m2 + bo->bienDo_m2*bienDo;
   *giaTri_c1 = bo->c1 + bo->bienDo_c1*bienDo;
   *giaTri_c2 = bo->c2 + bo->bienDo_c2*bienDo;
}

void phaTronHetLanVaoLan0( DuLieuAmThanh_float *duLieuAmThanh ) {
   
   float *mangMau = duLieuAmThanh->mangMau;
   unsigned int soLuongMau = duLieuAmThanh->soLuongMau;
   unsigned int dichLan = soLuongMau * kNHI_KENH;
   unsigned int soLuongLan = duLieuAmThanh->soLuongLan;
   
   // ==== cộng lên hết làn
   unsigned char soLan = 1;  // bỏ qua làn đi vì cộng vào nó
   unsigned int diaChiLan = dichLan;
   while( soLan < soLuongLan ) {
      unsigned int soMau = 0;
      while( soMau < soLuongMau ) {
         // ---- cộng lên hết mẫu
         
         mangMau[soMau] += mangMau[diaChiLan+soMau];
         mangMau[soMau+1] += mangMau[diaChiLan+soMau+1];
         
         soMau += 2;  // nhị kênh
      }
      
      diaChiLan += dichLan;
      soLan++;
   }

   // ==== Đơn vị hóa làn 0
   donViHoaLanFloat( mangMau, soLuongMau*kNHI_KENH, 1.0f );
}

void donViHoaLanFloat( float *lan,  unsigned int soLuongMau, float muc ) {
   
   // ---- tìm độ lớn cực đại
   unsigned int soMau = 0;
   float doLonCucDai = 0.0f;
   while( soMau < soLuongMau ) {
      float mau = lan[soMau];
      // ---- giá trị tuyệt đối
      if( mau < 0.0f )
         mau = -mau;

      if( mau > doLonCucDai )
         doLonCucDai = mau;

      soMau++;
   }
   
   // ---- đơn vị hóa
   if( doLonCucDai > 0.0f ) {
      soMau = 0;
      while( soMau < soLuongMau ) {
         float mau = lan[soMau];
         lan[soMau] = mau*muc / doLonCucDai;
         soMau++;
      }
   }
}


void chepVaoDem( float *demDich, float *demNguon, float thoiChepVao, float chepBaoLao, unsigned int tocDoMau ) {
   
   unsigned int soMau = 0;
   unsigned int soLuongMauChep = (unsigned int)(chepBaoLao*tocDoMau) << 1;
   unsigned int diaChiChepVao = (unsigned int)(thoiChepVao*tocDoMau) << 1;
   
   // ---- đến chổ chép
   demDich += diaChiChepVao;

   while( soMau < soLuongMauChep ) {
      *demDich = *demNguon;
      demDich++;
      demNguon++;
      soMau++;
   }
   
}
// ---------
float doc( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi ) {
   
   if( viTriTinh < viTriDau )
      return doCaoDau;
   else if( viTriTinh > viTriCuoi )
      return doCaoCuoi;
   else {
      float beDaiDoc = viTriCuoi - viTriDau;
      float viTriTuongDoi = (viTriTinh - viTriDau)/beDaiDoc;
      return (doCaoCuoi - doCaoDau)*viTriTuongDoi + doCaoDau;
   }
}

// bắt đầu từ đấy/đỉnh của parabol  đ(t) = m0*(t - viTriDau)^2 + doCaoDau
// doCaoDau = m0*(0)^2 + b
// doCaoCuoi = m0*(chenhLechViTri)^2 + b
float dayParabol( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi ) {
   
   
   if( viTriTinh < viTriDau )
      return doCaoDau;
   else if( viTriTinh > viTriCuoi )
      return doCaoCuoi;
   else {
      float viTriTuongDoi = viTriTinh - viTriDau;
      float chenhLechViTri = viTriCuoi - viTriDau;
      float m0 = 0.0f;
      if( chenhLechViTri != 0.0f )
         m0 = (doCaoCuoi - doCaoDau)/(chenhLechViTri*chenhLechViTri);
      else
         m0 = (doCaoCuoi - doCaoDau);
      
      return m0*viTriTuongDoi*viTriTuongDoi + doCaoDau;
   }
   
}


float songTamGiacf( float giaTri ) {
   
   float so = fmodf( giaTri, 1.0f );

   if( so < 0.25f )
      return (4.0f*so);
   else if( so < 0.75f )
      return (2.0f - 4.0f*so);  // 0 = -4(0,5) + b = -2 + b
   else
      return -4.0f + 4.0f*so;
}


// ----
/*
void hatViDuTanSoDanChuong4( DuLieuAmThanh_float *demPhatAm ) {
   
   // ---- tạo đệm địa phương
   float thoiGian = 8.0f;  // giây
   unsigned int soMau = thoiGian*demPhatAm->tocDoMau;
   
   DuLieuAmThanh_float demDiaPhuong;
   demDiaPhuong.soLuongMau = soMau;
   demDiaPhuong.tocDoMau = kTAN_SO_MAU;
   demDiaPhuong.soLuongKenh = kNHI_KENH;
   demDiaPhuong.mangMau = malloc( soMau*kNHI_KENH*sizeof( float )*kSO_LUONG__LAN );
   demDiaPhuong.lan0 = demDiaPhuong.mangMau;
   demDiaPhuong.lan1 = demDiaPhuong.lan0 + soMau*kNHI_KENH;
   demDiaPhuong.lan2 = demDiaPhuong.lan1 + soMau*kNHI_KENH;

   DieuCheTanSo_float chuong4;    // chuông
   // ----
   chuong4.giaiThuat = 5;
   chuong4.tongAmLuong = 1.0f;
   chuong4.phaPhanHoi = 0.0f;
   chuong4.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
   
   chuong4.amLuong.m1 = 0.584f;
   chuong4.amLuong.m2 = 0.842f;
   chuong4.amLuong.c1 = 0.826f;
   chuong4.amLuong.c2 = 0.842f;
   
   // ---- đệm pha trộn
   demDiaPhuong.soLuongLan = 3;
   unsigned int dichLan = (unsigned int)(kTAN_SO_MAU*8.0f*kNHI_KENH);
   
   unsigned short soTanSo = 0;
   unsigned short soLuongBuocTanSo = 6;
   
   float tanSo = -1.0f;   
   float tanSoKetThuc = 6.0f;
   float buocTanSo = 0.5f;
   
   float thoiGianHat = 0.0f;
   while( soTanSo < soLuongBuocTanSo ) {
      float tiSoTanSo = powf( 2.0, tanSo );
   
      chuong4.tanSo.m1 = 509.311f*kHAI_PI;
      chuong4.tanSo.m2 = 146.190f*kHAI_PI;
      chuong4.tanSo.c1 = 36.379f*kHAI_PI;
      chuong4.tanSo.c2 = 146.190f*kHAI_PI;
      giaiThuat5( demDiaPhuong.lan0, kTAN_SO_MAU, soMau, &chuong4, 8.0f );
      
      chuong4.tanSo.m1 = 320.601f*kHAI_PI;
      chuong4.tanSo.m2 = 92.105f*kHAI_PI;
      chuong4.tanSo.c1 = 22.900f*kHAI_PI;
      chuong4.tanSo.c2 = 92.105f*kHAI_PI;
      giaiThuat5( demDiaPhuong.lan1, kTAN_SO_MAU, soMau, &chuong4, 8.0f );
      
      chuong4.tanSo.m1 = 239.935f*kHAI_PI;
      chuong4.tanSo.m2 = 69.058f*kHAI_PI;
      chuong4.tanSo.c1 = 17.138f*kHAI_PI;
      chuong4.tanSo.c2 = 69.058f*kHAI_PI;
      giaiThuat5( demDiaPhuong.lan2, kTAN_SO_MAU, soMau, &chuong4, 8.0f );
      
      phaTronHetLanVaoLan0( &demDiaPhuong );
      
      // ---- âm chép vào
      chepVaoDem( demPhatAm->mangMau, demDiaPhuong.mangMau, thoiGianHat, 8.0f, kTAN_SO_MAU );
      
      thoiGianHat += 8.0;
      tanSo += buocTanSo;
      soTanSo++;
   }
   
} */